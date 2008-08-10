#include <stdio.h>
#include <string.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/material.h>
#include <g3d/matrix.h>
#include <g3d/object.h>

#include "imp_ldraw_types.h"
#include "imp_ldraw_library.h"

static gboolean ldraw_part_parse_meta(G3DObject *object, gchar *buffer)
{
	if(object->name == NULL) {
		object->name = g_strdup(buffer);
		return TRUE;
	}
#if DEBUG > 2
	g_debug("META: %s", buffer);
#endif
	return TRUE;
}

static gboolean ldraw_part_parse_ref(G3DObject *object, gchar *buffer,
	LDrawLibrary *lib)
{
	G3DObject *subobj;
	gfloat m[16], x, y, z;
	guint32 colid;
	gchar fname[256];

	g3d_matrix_identity(m);
	memset(fname, 0, 256);

	if(sscanf(buffer, "%u %f %f %f %f %f %f %f %f %f %f %f %f %255s",
		&colid, &x, &y, &z,
		m + 0 * 4 + 0, m + 1 * 4 + 0, m + 2 * 4 + 0,
		m + 0 * 4 + 1, m + 1 * 4 + 1, m + 2 * 4 + 1,
		m + 0 * 4 + 2, m + 1 * 4 + 2, m + 2 * 4 + 2,
		fname) == 14) {

		if(strncmp(fname, "s\\", 2) == 0) {
			fname[0] = 'S';
			fname[1] = G_DIR_SEPARATOR;
		}

		subobj = ldraw_library_lookup(lib, fname);
		if(subobj != NULL) {
			g3d_object_transform(subobj, m);
			g3d_matrix_identity(m);
			g3d_matrix_translate(x, y, z, m);
			g3d_object_transform(subobj, m);
			object->objects = g_slist_append(object->objects, subobj);
			return TRUE;
		}
	}
#if DEBUG > 1
	g_warning("LDraw: failed to parse/process reference: %s", buffer);
#endif
	return FALSE;
}

static gboolean ldraw_part_parse_tri(G3DObject *object, gchar *buffer)
{
	guint32 off, colid;
	G3DFace *face;

	off = object->vertex_count;
	object->vertex_count += 3;
	object->vertex_data = g_realloc(object->vertex_data,
		object->vertex_count * 3 * sizeof(gfloat));
	if(sscanf(buffer, "%u %f %f %f %f %f %f %f %f %f", &colid,
		object->vertex_data + (off + 0) * 3 + 0,
		object->vertex_data + (off + 0) * 3 + 1,
		object->vertex_data + (off + 0) * 3 + 2,
		object->vertex_data + (off + 1) * 3 + 0,
		object->vertex_data + (off + 1) * 3 + 1,
		object->vertex_data + (off + 1) * 3 + 2,
		object->vertex_data + (off + 2) * 3 + 0,
		object->vertex_data + (off + 2) * 3 + 1,
		object->vertex_data + (off + 2) * 3 + 2) == 10) {

		face = g_new0(G3DFace, 1);
		face->material = g_slist_nth_data(object->materials, 0);
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		face->vertex_indices[0] = off + 0;
		face->vertex_indices[1] = off + 1;
		face->vertex_indices[2] = off + 2;
		object->faces = g_slist_append(object->faces, face);
		return TRUE;
	}
	return FALSE;
}

static gboolean ldraw_part_parse_quad(G3DObject *object, gchar *buffer)
{
	guint32 off, colid;
	G3DFace *face;

	off = object->vertex_count;
	object->vertex_count += 4;
	object->vertex_data = g_realloc(object->vertex_data,
		object->vertex_count * 3 * sizeof(gfloat));
	if(sscanf(buffer, "%u %f %f %f %f %f %f %f %f %f %f %f %f", &colid,
		object->vertex_data + (off + 0) * 3 + 0,
		object->vertex_data + (off + 0) * 3 + 1,
		object->vertex_data + (off + 0) * 3 + 2,
		object->vertex_data + (off + 1) * 3 + 0,
		object->vertex_data + (off + 1) * 3 + 1,
		object->vertex_data + (off + 1) * 3 + 2,
		object->vertex_data + (off + 2) * 3 + 0,
		object->vertex_data + (off + 2) * 3 + 1,
		object->vertex_data + (off + 2) * 3 + 2,
		object->vertex_data + (off + 3) * 3 + 0,
		object->vertex_data + (off + 3) * 3 + 1,
		object->vertex_data + (off + 3) * 3 + 2) == 13) {

		face = g_new0(G3DFace, 1);
		face->material = g_slist_nth_data(object->materials, 0);
		face->vertex_count = 4;
		face->vertex_indices = g_new0(guint32, 4);
		face->vertex_indices[0] = off + 0;
		face->vertex_indices[1] = off + 1;
		face->vertex_indices[2] = off + 2;
		face->vertex_indices[3] = off + 3;
		object->faces = g_slist_append(object->faces, face);
		return TRUE;
	}
	return FALSE;
}

G3DObject *ldraw_part_get_object(LDrawPart *part, LDrawLibrary *lib)
{
	G3DObject *object;
	G3DMaterial *material;
	gchar buffer[1024];

	object = g_new0(G3DObject, 1);
	material = g3d_material_new();
	material->name = g_strdup("default material");
	object->materials = g_slist_append(object->materials, material);

	while(!g3d_stream_eof(part->stream)) {
		memset(buffer, 0, 1024);
		g3d_stream_read_line(part->stream, buffer, 1023);
		g_strstrip(buffer);
		if(strlen(buffer) == 0)
			continue;
		switch(buffer[0] - 0x30) {
			case 0: /* meta data */
				ldraw_part_parse_meta(object, buffer + 2);
				break;
			case 1: /* sub-file reference */
				ldraw_part_parse_ref(object, buffer + 2, lib);
				break;
			case 2: /* line */
				break;
			case 3: /* triangle */
				ldraw_part_parse_tri(object, buffer + 2);
				break;
			case 4: /* quadrilateral */
				ldraw_part_parse_quad(object, buffer + 2);
				break;
			case 5: /* optional line */
				break;
			default:
				g_warning("LDraw: unknown type of line: %s", buffer);
				break;
		}
	}

	return object;
}

void ldraw_part_free(LDrawPart *part)
{
	if(part->subdir)
		g_free(part->subdir);
	g_free(part->name);
	g_free(part);
}
