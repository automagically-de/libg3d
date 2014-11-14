/*
    libg3d - 3D object loading library

    Copyright (C) 2014  Markus Dahms <mad@automagically.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/material.h>
#include <g3d/texture.h>
#include <g3d/face.h>

/*
	PLY format: http://paulbourke.net/dataformats/ply/
*/

typedef enum _PlyPropertyType {
	T_INVALID,
	CHAR,      /* 1 byte */
	UCHAR,
	SHORT,     /* 2 bytes */
	USHORT,
	INT,       /* 4 bytes */
	UINT,
	FLOAT,
	DOUBLE,    /* 8 bytes */
	LIST       /* variable size */
} PlyPropertyType;

typedef enum _PlyPropertyField {
	F_INVALID,
	X,
	Y,
	Z,
	NX,
	NY,
	NZ,
	R,
	G,
	B,
	VERTEX_INDICES,
	TEXCOORD,
	TEXNUMBER
} PlyPropertyField;

typedef struct _PlyProperty {
	PlyPropertyType proptype;
	PlyPropertyType listcnttype;
	PlyPropertyType listtype;
	PlyPropertyField field;
	gchar *name;
} PlyProperty;

typedef struct _PlyElement {
	gchar *name;
	guint32 num_elements;
	guint32 num_props;
	PlyProperty *properties;
} PlyElement;

static gboolean ply_read_line(G3DStream *stream, gchar *buf, guint32 buflen);
static gboolean ply_read_token(G3DStream *stream, gchar *buf, guint32 buflen, gchar *splitters);
static PlyPropertyType ply_parse_type(const gchar *typename);
static PlyPropertyField ply_parse_field(const gchar *fieldname);
static G3DVector ply_read(G3DStream *stream, PlyPropertyType proptype, gboolean is_ascii, gboolean is_le);
static guint32 *ply_read_int_list(G3DStream *stream, PlyProperty* prop, gboolean is_ascii, gboolean is_le,
	guint32 *count);
static G3DVector *ply_read_float_list(G3DStream *stream, PlyProperty* prop, gboolean is_ascii, gboolean is_le,
	guint32 *count);
static G3DVector ply_normalize(G3DVector in, PlyPropertyType proptype);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar buf[BUFSIZ];
	guint32 linenum = 1, i, j;
	gboolean is_ascii = TRUE;
	gboolean is_le = TRUE;
	gboolean retval;
	gchar **tokens = NULL;
	GSList *elems = NULL, *elem;
	PlyElement *current_elem = NULL;
	PlyProperty *prop;
	G3DObject *object;
	G3DMaterial *mat, *default_material;
	GHashTable *material_by_color, *material_by_tex_index;
	G3DImage *image;
	guint32 tex_id = 0;
	G3DVector *vertex_colors = NULL;
	G3DVector *custom_normals = NULL;

	if (!ply_read_line(stream, buf, BUFSIZ) || strcmp(buf, "ply")) {
		g_warning("file %s is not a valid PLY file", stream->uri);
		return FALSE;
	}

	/* init helpers */
	material_by_color = g_hash_table_new(g_str_hash, g_str_equal);
	material_by_tex_index = g_hash_table_new(g_int_hash, g_int_equal);

	/* parse header */
	for (;;) {
		linenum ++;
		if (!ply_read_line(stream, buf, BUFSIZ)) {
			g_warning("failed to read header line (line %u)", linenum);
			goto fail;
		}
		tokens = g_strsplit(buf, " ", 0);
		if (!tokens[0]) {
			g_warning("failed to parse header line (line %u)", linenum);
			goto fail;
		}

		if (!strcmp(tokens[0], "comment")) {
			if (tokens[1]) {
				if (!strcmp(tokens[1], "TextureFile")) {
					gchar *tmp;
					if (!tokens[2]) {
						g_warning("expecting file name after 'comment TextureFile' (line %u)", linenum);
						goto fail;
					}
					tmp = g_strjoinv(" ", &tokens[2]);
					image = g3d_texture_load_cached(context, model, tmp);
					if (!image) {
						g_warning("failed to load texture image %s", tmp);
						g_free(tmp);
#if 0
						goto fail;
#endif
					}
					else {
						g3d_texture_flip_y(image);
						image->tex_id = tex_id;
						mat = g3d_material_new();
						mat->name = tmp;
						mat->tex_image = image;
						model->materials = g_slist_append(model->materials, mat);
						g_hash_table_insert(material_by_tex_index, &image->tex_id, mat);
					}

					tex_id ++;
				}
				else {
#if 0
					G3DMetaDataItem *item = g_new0(G3DMetaDataItem, 1);
					item->name = g_strdup("(comment)");
					item->value = g_strjoinv(" ", &tokens[1]);
					model->metadata = g_slist_append(model->metadata, item);
#endif
				}
			}
		}
		else if (!strcmp(tokens[0], "format")) {
			if (!tokens[1] || !tokens[2] || tokens[3]) {
				g_warning("failed to parse 'format' line, expect 'format <type> <version>' (line %u)", linenum);
				goto fail;
			}
			if (!strcmp(tokens[1], "binary_little_endian")) {
				is_ascii = FALSE;
				is_le = TRUE;
			}
			else if (!strcmp(tokens[1], "binary_big_endian")) {
				is_ascii = FALSE;
				is_le = FALSE;
			}
			else if (!strcmp(tokens[1], "ascii")) {
				is_ascii = TRUE;
			}
			else {
				g_warning("unknown format type '%s'", tokens[1]);
				goto fail;
			}
		}
		else if (!strcmp(tokens[0], "element")) {
			if (!tokens[1] || !tokens[2] || tokens[3]) {
				g_warning("failed to parse 'element' line, expect 'format <type> <count>' (line %u)", linenum);
				goto fail;
			}
			current_elem = g_new0(PlyElement, 1);
			current_elem->name = g_strdup(tokens[1]);
			current_elem->num_elements = strtoul(tokens[2], NULL, 10); /* FIXME: no error handling */

			elems = g_slist_append(elems, current_elem);
		}
		else if (!strcmp(tokens[0], "property")) {
			guint32 n = current_elem->num_props;

			if (!tokens[1] || !tokens[2]) {
				g_warning("failed to parse 'property' line, expect 'property <type> <name>' (line %u)", linenum);
				goto fail;
			}

			current_elem->num_props ++;
			current_elem->properties = g_realloc(current_elem->properties,
				sizeof(PlyProperty) * current_elem->num_props);
			prop = &(current_elem->properties[n]);

			prop->proptype = ply_parse_type(tokens[1]);
			if (prop->proptype == LIST) {
				prop->listcnttype = ply_parse_type(tokens[2]);
				prop->listtype = ply_parse_type(tokens[3]);
				prop->name = g_strdup(tokens[4]);
			}
			else {
				prop->name = g_strdup(tokens[2]);
			}
			prop->field = ply_parse_field(prop->name);

			if (!strcmp(current_elem->name, "vertex") && prop->field == R) {
				/* have per-vertex colors */
#if 0
				vertex_colors = g_new0(G3DVector, 3 * current_elem->num_elements);
#endif
			}

			if (!strcmp(current_elem->name, "vertex") && prop->field == NX) {
				custom_normals = g_new0(G3DVector, 3 * current_elem->num_elements);
			}
		}
		else if (!strcmp(tokens[0], "end_header")) {
			break;
		}

		g_strfreev(tokens);
		tokens = NULL;
	}

	/* load model */
	object = g_new0(G3DObject, 1);
	object->name = g_strdup("(default object)");
	model->objects = g_slist_append(model->objects, object);

	if (model->materials) {
		/* have at least one texture */
		default_material = model->materials->data;
	}
	else {
		mat = g3d_material_new();
		mat->name = g_strdup("(default material)");
		object->materials = g_slist_append(object->materials, mat);
		default_material = mat;
	}

	for (elem = elems; elem; elem = elem->next) {
		current_elem = elem->data;

		if (!strcmp(current_elem->name, "vertex")) {
			object->vertex_count = current_elem->num_elements;
			object->vertex_data = g_new0(G3DVector, current_elem->num_elements * 3);

			for (i = 0; i < current_elem->num_elements; i ++) {
				for (j = 0; j < current_elem->num_props; j ++) {
					prop = &(current_elem->properties[j]);
					if (prop->proptype == LIST) {
						/* XXX */
					}
					else {
						G3DVector f = ply_read(stream, prop->proptype, is_ascii, is_le);
						switch (prop->field) {
						case X:
							object->vertex_data[i * 3 + 0] = f;
							break;
						case Y:
							object->vertex_data[i * 3 + 1] = f;
							break;
						case Z:
							object->vertex_data[i * 3 + 2] = f;
							break;
						case NX:
							g_assert(custom_normals);
							custom_normals[i * 3 + 0] = f;
							break;
						case NY:
							g_assert(custom_normals);
							custom_normals[i * 3 + 1] = f;
							break;
						case NZ:
							g_assert(custom_normals);
							custom_normals[i * 3 + 2] = f;
							break;
						case R:
						case G:
						case B:
#if 0
							g_assert(vertex_colors);
							vertex_colors[i * 3 + (prop->field - R)] = ply_normalize(f, prop->proptype);
#endif
							break;
						default:
							break;
						}
					}
				}
#if 0
				printf("vertex %07i: %f, %f, %f\n", i,
					object->vertex_data[i * 3 + 0],
					object->vertex_data[i * 3 + 1],
					object->vertex_data[i * 3 + 2]);
#endif
			}
		}
		else if (!strcmp(current_elem->name, "face")) {
			for (i = 0; i < current_elem->num_elements; i ++) {
				G3DFace *face = g_new0(G3DFace, 1);
				face->material = default_material;

				for (j = 0; j < current_elem->num_props; j ++) {
					prop = &(current_elem->properties[j]);
					if (prop->proptype == LIST) {
						g_assert(prop->listcnttype != T_INVALID);
						g_assert(prop->listcnttype != FLOAT && prop->listcnttype != DOUBLE);
						if (prop->listtype == FLOAT || prop->listtype == DOUBLE) {
							guint32 cnt;
							G3DVector *fv = ply_read_float_list(stream, prop, is_ascii, is_le, &cnt);
							if (prop->field == TEXCOORD) {
								face->tex_vertex_count = cnt / 2;
								face->tex_vertex_data = fv;

#if 0
{
	guint32 k;
	for (k = 0; k < face->tex_vertex_count; k ++)
		printf("face %7u [%u]: %1.4f, %1.4f\n", i, k, face->tex_vertex_data[k*2+0], face->tex_vertex_data[k*2+1]);
}
#endif

							}
							else {
								g_free(fv);
							}
						}
						else {
							guint32 cnt;
							guint32 *fv = ply_read_int_list(stream, prop, is_ascii, is_le, &cnt);
							if (prop->field == VERTEX_INDICES) {
								face->vertex_count = cnt;
								face->vertex_indices = fv;
#if 0
{
	guint32 k;
	for (k = 0; k < face->vertex_count; k ++)
		printf("face %7u [%u]: %u\n", i, k, face->vertex_indices[k]);
}
#endif
							}
							else {
								g_free(fv);
							}
						}
					}
					else {
						G3DVector f = ply_read(stream, prop->proptype, is_ascii, is_le);
						if (prop->field == TEXNUMBER) {
							guint32 texnum = f;
							mat = g_hash_table_lookup(material_by_tex_index, &texnum);
							if (mat) {
								face->material = mat;
							}
							else {
#if 0
								g_warning("failed to look up tex number %u", texnum);
#endif
							}
						}
					}
				}

				if (vertex_colors) {
					G3DVector r = 0, g = 0, b = 0;
					gchar *colname;
					for (j = 0; j < face->vertex_count; j ++) {
						r += vertex_colors[face->vertex_indices[j] * 3 + 0];
						g += vertex_colors[face->vertex_indices[j] * 3 + 1];
						b += vertex_colors[face->vertex_indices[j] * 3 + 2];
					}
					r /= face->vertex_count;
					g /= face->vertex_count;
					b /= face->vertex_count;
					colname = g_strdup_printf("#%02x%02x%02x",
						(int)(r * 255), (int)(g * 255), (int)(b * 255));
					mat = g_hash_table_lookup(material_by_color, colname);
					if (mat) {
						g_free(colname);
					}
					else {
						mat = g3d_material_new();
						mat->name = colname;
						mat->r = r;
						mat->g = g;
						mat->b = b;
						g_hash_table_insert(material_by_color, mat->name, mat);
					}
					face->material = mat;
				}

				if (face->material->tex_image && face->tex_vertex_count) {
					face->tex_image = face->material->tex_image;
					face->flags |= G3D_FLAG_FAC_TEXMAP;
				}

				/* check face */
				if (face->vertex_count < 3)
					goto face_invalid;
				for (j = 0; j < face->vertex_count; j ++)
					if (face->vertex_indices[j] >= object->vertex_count)
						goto face_invalid;

				if (custom_normals) {
					face->normals = g_new0(G3DVector, 3 * face->vertex_count);
					face->flags |= G3D_FLAG_FAC_NORMALS;
					for (j = 0; j < face->vertex_count; j ++) {
						face->normals[j * 3 + 0] = custom_normals[face->vertex_indices[j] * 3 + 0];
						face->normals[j * 3 + 1] = custom_normals[face->vertex_indices[j] * 3 + 1];
						face->normals[j * 3 + 2] = custom_normals[face->vertex_indices[j] * 3 + 2];
					}
				}

				object->faces = g_slist_prepend(object->faces, face);
				continue;

face_invalid:
				g3d_face_free(face);
			}
		}
	}

	retval = TRUE;
	goto cleanup;
fail:
	retval = FALSE;

cleanup:
	if (tokens)
		g_strfreev(tokens);
	if (custom_normals)
		g_free(custom_normals);

	for (elem = elems; elem; elem = elem->next) {
	}

	return retval;
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup(
		"Import plugin .ply files\n"
		"Author: Markus Dahms\n");
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("ply", ":", 0);
}

/*****************************************************************************/

static gboolean ply_read_line(G3DStream *stream, gchar *buf, guint32 buflen)
{
	return ply_read_token(stream, buf, buflen, "\n\r");
}

static gboolean ply_read_token(G3DStream *stream, gchar *buf, guint32 buflen, gchar *splitters)
{
	guint32 idx = 0;
	gchar c, *sp;

	for (;;) {
		if (idx == (buflen - 1)) {
			buf[idx] = '\0';
			return FALSE;
		}
		c = g3d_stream_read_int8(stream);

		for (sp = splitters; *sp; sp ++) {
			if (c == *sp) {
				buf[idx] = '\0';
				/* some hack to detect CRLF */
				if (idx == 0 && c == '\n')
					return ply_read_token(stream, buf, buflen, splitters);
				return TRUE;
			}
		}

		buf[idx] = c;
		idx ++;
	}
}

static PlyPropertyType ply_parse_type(const gchar *typename)
{
	guint32 i;
	static const char *names[] = {
		"",
		"char", "uchar",
		"short", "ushort",
		"int", "uint",
		"float", "double",
		"list",
		NULL
	};
	for (i = 0; names[i]; i ++) {
		if (!strcmp(typename, names[i]))
			return i;
	}
	return T_INVALID;
}

static PlyPropertyField ply_parse_field(const gchar *fieldname) {
	guint32 i;
	static const char *names[] = {
		"",
		"x", "y", "z",
		"nx", "ny", "nz",
		"red", "green", "blue",
		"vertex_indices",
		"texcoord", "texnumber",
		NULL
	};
	for (i = 0; names[i]; i ++) {
		if (!strcmp(fieldname, names[i]))
			return i;
	}
	if (!strcmp(fieldname, "vertex_index"))
		return VERTEX_INDICES;

	return F_INVALID;
}

static G3DVector ply_read(G3DStream *stream, PlyPropertyType proptype, gboolean is_ascii, gboolean is_le)
{
	if (is_ascii) {
		gchar buf[BUFSIZ];
		if (!ply_read_token(stream, buf, BUFSIZ, " \n")) {
			g_warning("failed to read ASCII token @ 0x%08lx", g3d_stream_tell(stream));
			return 0; /* XXX */
		}
		return strtod(buf, NULL);
	}
	else if (is_le) {
		switch (proptype) {
		case CHAR:
			return g3d_stream_read_int8(stream);
		case UCHAR:
			return g3d_stream_read_int8(stream);
		case SHORT:
			return g3d_stream_read_int16_le(stream);
		case USHORT:
			return g3d_stream_read_int16_le(stream);
		case INT:
			return g3d_stream_read_int32_le(stream);
		case UINT:
			return g3d_stream_read_int32_le(stream);
		case FLOAT:
			return g3d_stream_read_float_le(stream);
		case DOUBLE:
			return g3d_stream_read_double_le(stream);
		default:
			return 0; /* XXX */
		}
	}
	else {
		switch (proptype) {
		case CHAR:
			return g3d_stream_read_int8(stream);
		case UCHAR:
			return g3d_stream_read_int8(stream);
		case SHORT:
			return g3d_stream_read_int16_be(stream);
		case USHORT:
			return g3d_stream_read_int16_be(stream);
		case INT:
			return g3d_stream_read_int32_be(stream);
		case UINT:
			return g3d_stream_read_int32_be(stream);
		case FLOAT:
			return g3d_stream_read_float_be(stream);
		case DOUBLE:
			return g3d_stream_read_double_be(stream);
		default:
			return 0; /* XXX */
		}
	}
}

static guint32 *ply_read_int_list(G3DStream *stream, PlyProperty* prop, gboolean is_ascii, gboolean is_le,
	guint32 *count)
{
	guint32 cnt = ply_read(stream, prop->listcnttype, is_ascii, is_le);
	guint32 *fv = g_new0(guint32, cnt);
	guint32 i;

	for (i = 0; i < cnt; i ++) {
		fv[i] = ply_read(stream, prop->listtype, is_ascii, is_le);
	}

	*count = cnt;
	return fv;
}

static G3DVector *ply_read_float_list(G3DStream *stream, PlyProperty* prop, gboolean is_ascii, gboolean is_le,
	guint32 *count)
{
	guint32 cnt = ply_read(stream, prop->listcnttype, is_ascii, is_le);
	G3DVector *fv = g_new0(G3DVector, cnt);
	guint32 i;

	for (i = 0; i < cnt; i ++) {
		fv[i] = ply_read(stream, prop->listtype, is_ascii, is_le);
	}

	*count = cnt;
	return fv;
}

static G3DVector ply_normalize(G3DVector in, PlyPropertyType proptype)
{
	switch (proptype) {
	case UCHAR:
		return in / 255.0;
	case USHORT:
		return in / 65535.0;
	case UINT:
		return in / 4294967295.0;
	default:
		g_warning("%s: unhandled property type %u", __func__, proptype);
		return 0;
	}
}
