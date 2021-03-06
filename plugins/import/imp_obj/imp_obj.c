/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2009  Markus Dahms <mad@automagically.de>

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
#include <errno.h>
#include <locale.h>

#include <g3d/context.h>
#include <g3d/types.h>
#include <g3d/material.h>
#include <g3d/texture.h>
#include <g3d/face.h>
#include <g3d/stream.h>
#include <g3d/vector.h>
#include <g3d/matrix.h>

#define OBJ_USE_GROUPING 0

typedef struct {
	goffset goff;
	goffset ooff;
	G3DObject *object;
} ObjGroupOffset;

static gboolean obj_tryloadmat(G3DModel *model, const gchar *filename);
static G3DMaterial *obj_usemat(G3DModel *model, const gchar *matname);

static G3DObject *obj_object_by_name(G3DModel *model, const gchar *name);
#if OBJ_USE_GROUPING
static G3DObject *obj_get_offset(GSList *group_list, guint32 *voffp,
	guint32 index, G3DObject *defobj);
#endif
static gboolean line_startswith(const gchar *line, const gchar *part);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar line[2048], matname[128], matfile[1024], buf[2048];
	gchar *filename;
	G3DObject *object = NULL;
	G3DMaterial *material = NULL;
	gfloat pcnt, prev_pcnt = 0.0;
	G3DFloat x,y,z, nx,ny,nz, u,v;
	G3DMatrix *matrix;
	guint32 num_v, v_off = 1, v_cnt = 0;
#if OBJ_USE_GROUPING
	gchar oname[128];
	ObjGroupOffset *grpoff;
	GSList *group_list = NULL;
#endif
	goffset global_vertex_count = 0;
	guint32 index_table_count = 0;
	guint32 *index_table = NULL;
	guint32 normal_count = 0;
	G3DVector *normal_data = NULL;
	GQueue *anim_queue = g_queue_new();
	gboolean *transformed_map = NULL;

	setlocale(LC_NUMERIC, "C");
	filename = g3d_stream_get_uri(stream);

	strncpy(matfile, filename, strlen(filename) - 3);
	matfile[strlen(filename)-3] = '\0';
	strcat(matfile, "mtl");
	obj_tryloadmat(model, matfile);

	object = obj_object_by_name(model, "(default)");

	while(!g3d_stream_eof(stream))
	{
		memset(line, 0, 2048);
		g3d_stream_read_line(stream, line, 2048);
		/* remove leading and trailing whitespace characters */
		g_strstrip(line);

		if (strlen(line) == 0)
			continue;

		if (line[0] == '#')
			continue;

		if (line_startswith(line, "IDX")) {
			index_table_count ++;
			index_table = g_realloc(index_table, index_table_count * sizeof(guint32));
			if (sscanf(line + 4, "%u", index_table + (index_table_count - 1)) != 1)
				g_warning("parse error in line: %s", line);
			goto next;
		}

		if (strcmp(line, "ANIM_begin") == 0) {
			matrix = g3d_matrix_new();
			g_queue_push_tail(anim_queue, matrix);
			goto next;
		}

		if (strcmp(line, "ANIM_end") == 0) {
			if (g_queue_is_empty(anim_queue))
				g_warning("ANIM_end: anim_queue is empty");
			else {
				matrix = g_queue_pop_tail(anim_queue);
				g3d_matrix_free(matrix);
			}
			goto next;
		}

		if (line_startswith(line, "ANIM_trans")) {
			/* ANIM_trans <x1> <y1> <z1> <x2> <y2> <z2> <v1> <v2> <dataref> */
			if (sscanf(line + 11, "%g %g %g %g %g %g %g %g %s",
				&x, &y, &z, &nx, &ny, &nz, &u, &v, buf) != 9) {
				g_warning("parse error in line: %s", line);
				continue;
			}
			printf("ANIM_trans: (%g, %g, %g) (%g, %g, %g) %s = [%g, %g]\n",
				x, y, z, nx, ny, nz, buf, u, v);
			/* ignore v2 for now, always use v1 state */
			matrix = g_queue_peek_tail(anim_queue);
			if (!matrix)
				g_warning("ANIM_trans: anim_queue is empty");
			else
				g3d_matrix_translate(x, y, z, matrix);
			goto next;
		}

		if (line_startswith(line, "ANIM_rotate")) {
			/* ANIM_rotate <x> <y> <z> <r1> <r2> <v1> <v2> <dataref> */
			if (sscanf(line + 12, "%g %g %g %g %g %g %g %s",
				&x, &y, &z, &nx, &ny, &u, &v, buf) != 8) {
				g_warning("parse error in line: %s", line);
				continue;
			}
			matrix = g_queue_peek_tail(anim_queue);
			if (!matrix)
				g_warning("ANIM_trans: anim_queue is empty");
			else
				g3d_matrix_rotate(nx, x, y, z, matrix);
			goto next;
		}

		if (line_startswith(line, "IDX10")) {
			/* IDX10: 10 index table entries */
			guint32 off = index_table_count;
			index_table_count += 10;
			index_table = g_realloc(index_table, index_table_count * sizeof(guint32));
			if (sscanf(line + 6, "%u %u %u %u %u %u %u %u %u %u",
				index_table + off + 0, index_table + off + 1,
				index_table + off + 2, index_table + off + 3,
				index_table + off + 4, index_table + off + 5,
				index_table + off + 6, index_table + off + 7,
				index_table + off + 8, index_table + off + 9) != 10) {
				g_warning("parse error in line: %s", line);
			}
			goto next;
		}

		if (line_startswith(line, "TEXTURE")) {
			/* TEXTURE <texture file> */
			if (sscanf(line + 8, "%s", buf) == 1) {
				G3DMaterial *defmat = g_slist_nth_data(object->materials, 0);
				defmat->tex_image = g3d_texture_load_cached(context, model, buf);
				if (defmat->tex_image)
					g3d_texture_flip_y(defmat->tex_image);
			}
			else
				g_warning("parse error in line: %s", line);
			goto next;
		}

		if (line_startswith(line, "TRIS")) {
			guint32 i, j, k;
			G3DFace *face;
			G3DMaterial *mat;

			/* TRIS <offset> <count> */
			if (sscanf(line + 5, "%u %u", &v_off, &num_v) != 2) {
				g_warning("parse error in line: %s", line);
				continue;
			}
			if ((v_off + num_v) > index_table_count) {
				g_warning("TRIS: (%u + %u) > %u", v_off, num_v, index_table_count);
				continue;
			}

			mat = material ? material : g_slist_nth_data(object->materials, 0);

			for (i = 0; i < (num_v / 3); i ++) {
				face = g3d_face_new_tri(mat,
					index_table[v_off + i * 3 + 0],
					index_table[v_off + i * 3 + 1],
					index_table[v_off + i * 3 + 2]);

				for (j = 0; j < g_queue_get_length(anim_queue); j ++) {
					matrix = g_queue_peek_nth(anim_queue, j);
					/*printf("TRIS: apply matrix %u\n", j);*/
					for (k = 0; k < 3; k ++) {
						guint32 idx = index_table[v_off + i * 3 + k];
						if (!transformed_map)
							transformed_map = g_new0(gboolean, object->vertex_count);
						if (transformed_map[idx])
							continue;
						transformed_map[idx] = TRUE;

						g3d_vector_transform(
							object->vertex_data + idx * 3 + 0,
							object->vertex_data + idx * 3 + 1,
							object->vertex_data + idx * 3 + 2,
							matrix);
					}
				}

				if (mat->tex_image) {
					face->tex_image = mat->tex_image;
					face->tex_vertex_count = 3;
					face->tex_vertex_data = g_new0(G3DVector, 3 * 2);
					face->flags |= G3D_FLAG_FAC_TEXMAP;
					for (j = 0; j < 3; j ++) {
						guint32 idx = index_table[v_off + i * 3 + j];
						face->tex_vertex_data[j * 2 + 0] = object->tex_vertex_data[idx * 2 + 0];
						face->tex_vertex_data[j * 2 + 1] = object->tex_vertex_data[idx * 2 + 1];
					}
				}

				if (normal_data) {
					face->normals = g_new0(G3DVector, 3 * 3);
					face->flags |= G3D_FLAG_FAC_NORMALS;
					for (j = 0; j < 3; j ++) {
						guint32 idx = index_table[v_off + i * 3 + j];
						face->normals[j * 3 + 0] = normal_data[idx * 3 + 0];
						face->normals[j * 3 + 1] = normal_data[idx * 3 + 1];
						face->normals[j * 3 + 2] = normal_data[idx * 3 + 2];
					}
				}
#if 0
				printf("TRI: [%i, %i, %i]: %f %f %f\n",
					face->vertex_indices[0],
					face->vertex_indices[1],
					face->vertex_indices[2],
					object->vertex_data[face->vertex_indices[0]],
					object->vertex_data[face->vertex_indices[1]],
					object->vertex_data[face->vertex_indices[2]]);
#endif

				object->faces = g_slist_prepend(object->faces, face);
			}
			goto next;
		}

		if (line_startswith(line, "VT")) {
			/* VT <x> <y> <z> <nx> <ny> <nz> <s> <t> */
			if (sscanf(line + 3, "%g %g %g %g %g %g %g %g",
				&x, &y, &z, &nx, &ny, &nz, &u, &v) != 8) {
				g_warning("parse error in line: %s", line);
				continue;
			}

			object->vertex_count ++;
			object->vertex_data = g_realloc(object->vertex_data,
				object->vertex_count * 3 * sizeof(G3DVector));
			object->vertex_data[v_cnt * 3 + 0] = x;
			object->vertex_data[v_cnt * 3 + 1] = y;
			object->vertex_data[v_cnt * 3 + 2] = z;

			normal_count ++;
			g_assert(object->vertex_count == normal_count);
			normal_data = g_realloc(normal_data, normal_count * 3 * sizeof(G3DVector));
			normal_data[v_cnt * 3 + 0] = nx;
			normal_data[v_cnt * 3 + 1] = ny;
			normal_data[v_cnt * 3 + 2] = nz;

			object->tex_vertex_count ++;
			g_assert(object->vertex_count == object->tex_vertex_count);
			object->tex_vertex_data = g_realloc(object->tex_vertex_data,
				object->tex_vertex_count * 2 * sizeof(G3DVector));
			object->tex_vertex_data[v_cnt * 2 + 0] = u;
			object->tex_vertex_data[v_cnt * 2 + 1] = v;
#if 0
			printf("VT: %f, %f, %f\n",
				object->vertex_data[v_cnt * 3 + 0],
				object->vertex_data[v_cnt * 3 + 1],
				object->vertex_data[v_cnt * 3 + 2]);
#endif

			v_cnt ++;
			global_vertex_count ++;
			goto next;
		}

		switch(line[0]) {

			case 'g': /* group */
#if OBJ_USE_GROUPING
				if(strlen(line) == 1)
					strcpy(oname, "(default)");
				else
					sscanf(line, "g %s", oname);

				material = obj_usemat(model, oname);

				grpoff = g_new0(ObjGroupOffset, 1);
				grpoff->object = obj_object_by_name(model, oname);
				grpoff->goff = global_vertex_count;
				grpoff->ooff = grpoff->object->vertex_count;
				group_list = g_slist_append(group_list, grpoff);
#if DEBUG > 0
				g_debug("[g] 0x%08x / 0x%08x: \"%s\"",
					(guint32)grpoff->goff,
					(guint32)grpoff->ooff, grpoff->object->name);
#endif
				object = grpoff->object;
				v_cnt = grpoff->ooff;
#endif
				break;

			case 'l': /* line */
				break;

			case 'o': /* object */
				break;

			case 'v': /* vertex */
			case 'V':
				if(strncmp(line, "vn ", 3) == 0)
				{
					/* normal ? */
				}
				else if(sscanf(line, "v %g %g %g", &x, &y, &z) == 3)
				{
					object->vertex_count ++;
					object->vertex_data = g_realloc(object->vertex_data,
						object->vertex_count * 3 * sizeof(gfloat));
					object->vertex_data[v_cnt * 3 + 0] = x;
					object->vertex_data[v_cnt * 3 + 1] = y;
					object->vertex_data[v_cnt * 3 + 2] = z;

					v_cnt ++;
					global_vertex_count ++;
				}
				else g_warning("parse error in line: %s", line);
				break;

			case 'f': /* face */
				if(strncmp("f ", line, 2) == 0)
				{
					G3DFace *face;
					gchar **vertex, **vstrs = g_strsplit(line, " ", 0);
					int i;

					num_v = 0;
					face = g_new0(G3DFace, 1);
					if(material != NULL)
						face->material = material;
					else face->material =
						g_slist_nth_data(object->materials, 0);

					/* find number of vertices in line */
					vertex = vstrs;
					while(*vertex != NULL) { num_v++; vertex++; }
					face->vertex_count = num_v - 1;

					/* next one if # of vertices < 3 */
					if(face->vertex_count < 3) {
						g3d_face_free(face);
						continue;
					}

					/* calculate object-local vertex offset, indices
					 * in .obj files are absolute */
					i = strtol(vstrs[1], NULL, 10);
#if OBJ_USE_GROUPING
					object = obj_get_offset(group_list, &v_off,
						(i < 0) ? global_vertex_count - i - 1 : i,
						object);
#else
					v_off = 0;
#endif
					if(object == NULL) {
						g_warning("error: face before object");
						g3d_face_free(face);
						return FALSE;
					}

					/* read vertices */
					face->vertex_indices = g_new0(guint32, num_v - 1);
					for(i = 1; i < num_v; i ++) {
						gint32 index = strtol(vstrs[i], NULL, 10);

						if(index < 0)
							face->vertex_indices[i - 1] =
								global_vertex_count + index + v_off - 1;
						else
							face->vertex_indices[i - 1] = MIN(
								(index - 1) + v_off,
								object->vertex_count - 1);
					}
					g_strfreev(vstrs);
					object->faces = g_slist_prepend(object->faces, face);
				}
				else
					g_warning("parse error in line: %s", line);
				break;

			case 'u': /* usemat? */
			case 'm':
			case 's':
				if(sscanf(line, "usemtl %s", matname) == 1) {
					material = obj_usemat(model, matname);
				} else if(sscanf(line, "mtllib %s", matfile) == 1) {
					/* loads external material library */
					if(obj_tryloadmat(model, matfile) != TRUE)
						g_warning("error loading material library '%s'",
							matfile);
				}
				break;
			default:
#if DEBUG > 0
				g_debug("unknown type of line: %s", line);
#endif
				break;
		}

next:
#if 1
		pcnt = (gfloat)g3d_stream_tell(stream) /
			(gfloat)g3d_stream_size(stream);
		if((pcnt - prev_pcnt) > 0.01) {
			prev_pcnt = pcnt;
			g3d_context_update_progress_bar(context, pcnt, TRUE);
		}
#endif
		g3d_context_update_interface(context);
	} /* !eof(stream) */

	if (index_table)
		g_free(index_table);
	if (normal_data)
		g_free(normal_data);
	if (transformed_map)
		g_free(transformed_map);

	g_queue_free(anim_queue);

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Maya .obj files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("obj", ":", 0);
}

/*****************************************************************************/

/*****************************************************************************/
/* material file ops                                                         */
/*****************************************************************************/

int obj_tryloadmat(G3DModel *model, const char *filename)
{
	FILE *f;
	G3DMaterial *material = NULL;

	f = fopen(filename, "r");
	if(f == NULL) {
#if DEBUG > 1
		g_warning("obj_tryloadmat: loading '%s' failed: %s", filename,
			strerror(errno));
#endif
		return FALSE;
	}
#if DEBUG > 0
	g_debug("loading material library %s", filename);
#endif
	while(!feof(f)) {
		char line[2048];
		float r,g,b, t1,t2, ni;
		int tf, ns, il;

		fgets(line, 2048, f);
		g_strstrip(line);
		if(strlen(line))
		{
			char mname[128];

			if(line[0] == '#') continue;   /* comments */
			if(line[0] == '\n') continue;  /* empty lines */

			if(sscanf(line, "newmtl %s", mname) == 1)
			{
				/* new material */
				material = g3d_material_new();
				material->name = g_strdup(mname);
				model->materials = g_slist_append(model->materials, material);
			}
			else if(sscanf(line, " Kd %f %f %f", &r, &g, &b) == 3)
			{
				/* material color? */
				if(material != NULL)
				{
					material->r = r;
					material->g = g;
					material->b = b;
				}
			}
			else if(sscanf(line, " Ks %f %f %f", &r, &g, &b) == 3)
			{
				/* ?? */
			}
			else if(sscanf(line, " Tf %f %f %d", &t1, &t2, &tf) == 3)
			{
				/* transparency ?? */
				if(material != NULL)
				{
					if(tf == 1) material->a = 1.0 - t1;
				}
			}
			else if(sscanf(line, " Ns %d Ni %f", &ns, &ni) == 2)
			{
				/* ?? */
			}
			else if(sscanf(line, " illum %d", &il) == 1)
			{
				/* ?? */
			}
			else {
#if DEBUG > 0
				g_warning("unknown type of line: %s", line);
#endif
			}
		}
	} /* !feof */
	return TRUE;
}

G3DMaterial *obj_usemat(G3DModel *model, const gchar *matname)
{
	/* sets new active material from named list */
	GSList *mlist = model->materials;
	while(mlist != NULL)
	{
		G3DMaterial *mat = (G3DMaterial*)mlist->data;
		if(strcmp(matname, mat->name) == 0)
		{
			return mat;
		}
		mlist = mlist->next;
	}

	return NULL;
}

static G3DObject *obj_object_by_name(G3DModel *model, const gchar *name)
{
	G3DObject *object;
	G3DMaterial *material;
	GSList *oitem;

#if DEBUG > 4
	g_debug("looking for object '%s'", name);
#endif

	for(oitem = model->objects; oitem != NULL; oitem = oitem->next) {
		object = oitem->data;
		if(strcmp(object->name, name) == 0)
			return object;
	}

	material = g3d_material_new();
	material->name = g_strdup("(default material)");

	object = g_new0(G3DObject, 1);
	object->name = g_strdup(name);
	object->materials = g_slist_append(object->materials, material);
	model->objects = g_slist_append(model->objects, object);

	return object;
}

#if OBJ_USE_GROUPING
static G3DObject *obj_get_offset(GSList *group_list, guint32 *voffp,
	guint32 index, G3DObject *defobj)
{
	GSList *leitem, *gitem;
	ObjGroupOffset *grpoff;

	for(leitem = gitem = group_list; gitem != NULL; gitem = gitem->next) {
		grpoff = gitem->data;

		/* this one is too big */
		if(grpoff->goff > index) {
			grpoff = leitem->data;
			*voffp = grpoff->ooff - grpoff->goff;
#if DEBUG > 0
			g_debug("[o]: i=%-6d, go=%-6d, oo=%-6d, vo=%-6d (%s, %d vtxs)",
				index, (guint32)grpoff->goff, (guint32)grpoff->ooff, *voffp,
				grpoff->object->name, grpoff->object->vertex_count);
#endif
			return grpoff->object;
		}
		leitem = gitem;
	}

	*voffp = 0;
	return defobj;
}
#endif

static gboolean line_startswith(const gchar *line, const gchar *part) {
	guint32 l = strlen(part);
	if (strncmp(line, part, l) != 0)
		return FALSE;
	if (strlen(line) < (l + 1))
		return FALSE;
	if (line[l] != ' ' && line[l] != '\t')
		return FALSE;
	return TRUE;
}
