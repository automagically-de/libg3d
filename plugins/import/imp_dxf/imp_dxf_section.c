/* $Id: imp_dxf_section.c 312 2008-11-17 18:28:56Z mmmaddd $ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2008  Markus Dahms <mad@automagically.de>

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
#include <string.h>
#include <math.h>

#include "imp_dxf.h"
#include "imp_dxf_vars.h"
#include "imp_dxf_chunks.h"

static DxfChunkInfo *dxf_get_chunk_info(gint32 id)
{
	guint32 i;

	for(i = 0; dxf_chunks[i].id != DXF_GRPCODE_TERMINATOR; i ++)
		if(dxf_chunks[i].id == id)
			return &(dxf_chunks[i]);
	return NULL;
}


static gboolean dxf_str_in_array(gchar **array, const gchar *needle)
{
	gchar **p = array;
	while(*p != NULL) {
		if(strcmp(*p, needle) == 0)
			return TRUE;
		p ++;
	}
	return FALSE;
}

#define DXF_VAR_DEBUG 2

static gboolean dxf_read_variable(DxfGlobalData *global)
{
	gint32 key;
	gint16 i16;
	gchar str[DXF_MAX_LINE + 1], strval[DXF_MAX_LINE + 1];
	gdouble x, y, z;

	dxf_read_string(global, str);
	if(dxf_str_in_array(dxf_vars_vector3d, str)) {
		key = dxf_read_code(global); /* 10 */
		x = dxf_read_float64(global);
		key = dxf_read_code(global); /* 20 */
		y = dxf_read_float64(global);
		key = dxf_read_code(global); /* 30 */
		z = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
		g_debug("DXF: [v3d] %s: %.2f, %.2f, %.2f", str, x, y, z);
#endif
	} else if(dxf_str_in_array(dxf_vars_vector2d, str)) {
		key = dxf_read_code(global); /* 10 */
		x = dxf_read_float64(global);
		key = dxf_read_code(global); /* 20 */
		y = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
		g_debug("DXF: [v2d] %s: %.2f, %.2f", str, x, y);
#endif
	} else {
		key = dxf_read_code(global);
		switch(key) {
			case 1: case 2: case 3: case 4:
			case 5: case 6: case 7: case 8:
				/* string */
				dxf_read_string(global, strval);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [str] %s: %s", str, strval);
#endif
				break;
			case 40:
			case 50:
				x = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [dbl] %s: %.2f", str, x);
#endif
				break;
			case 62:
			case 70:
				/* 16-bit integer */
				i16 = dxf_read_int16(global);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [i16] %s: %d", str, i16);
#endif
				break;
			default:
				DXF_HANDLE_UNKNOWN(global, key, strval, "** VARIABLE **");
				break;
		}
	}
	return TRUE;
}

gboolean dxf_section_HEADER(DxfGlobalData *global)
{
	gint32 key;
	gchar str[DXF_MAX_LINE + 1];

#if DEBUG > 0
	g_debug("DXF: parsing HEADER section");
#endif
	while(TRUE) {
		key = dxf_read_code(global);
		switch(key) {
			case DXF_CODE_INVALID:
				return 0xE0F;
				break;
			case 0: /* string */
				dxf_read_string(global, str);
				DXF_TEST_ENDSEC(str);
				break;
			case 9: /* variable */
				dxf_read_variable(global);
				break;
			default:
				DXF_HANDLE_UNKNOWN(global, key, str, "HEADER");
				break;
		} /* key */
	}
	return dxf_skip_section(global);
}

gboolean dxf_section_TABLES(DxfGlobalData *global)
{
	gint32 key;
	gchar str[DXF_MAX_LINE + 1];

#if DEBUG > 0
	g_debug("DXF: parsing TABLES section");
#endif
	while(TRUE) {
		key = dxf_read_code(global);
		switch(key) {
			case DXF_CODE_INVALID:
				return 0xE0F;
				break;
			case 0: /* string */
				dxf_read_string(global, str);
				DXF_TEST_ENDSEC(str);
				break;
			default:
				DXF_HANDLE_UNKNOWN(global, key, str, "TABLES");
				break;
		} /* key */
	}
	return dxf_skip_section(global);
}

static gboolean dxf_vertex_equals(gfloat *v1, gfloat *v2)
{
	return ((fabs(v1[0] - v2[0]) < 0.0001) &&
		(fabs(v1[1] - v2[1]) < 0.0001) &&
		(fabs(v1[2] - v2[2]) < 0.0001));
}


gboolean dxf_section_ENTITIES(DxfGlobalData *global)
{
	gint32 key;
	gchar str[DXF_MAX_LINE + 1];
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face = NULL;
	gdouble dbl;
	guint32 voff = 0;
	DxfChunkInfo *chunk_info;

#if DEBUG > 0
	g_debug("DXF: parsing ENTITIES section");
#endif

	object = g_slist_nth_data(global->model->objects, 0);
	material = g_slist_nth_data(object->materials, 0);

	while(TRUE) {
		key = dxf_read_code(global);
		chunk_info = dxf_get_chunk_info(key);
#if DEBUG > 0
		if(chunk_info)
			g_debug("\\[%+4d]: %s", key, chunk_info->description);
		else
			g_warning("unknown chunk type %d", key);
#endif
		switch(key) {
			case DXF_CODE_INVALID:
				return 0xE0F;
				break;
			case 0: /* string */
				face = NULL;
				dxf_read_string(global, str);
				DXF_TEST_ENDSEC(str);
				if(strcmp(str, "3DFACE") == 0) {
					face = g_new0(G3DFace, 1);
					face->material = material;
					face->vertex_count = 3;
					face->vertex_indices = g_new0(guint32, 3);
					voff = object->vertex_count;
					face->vertex_indices[0] = voff;
					face->vertex_indices[1] = voff + 1;
					face->vertex_indices[2] = voff + 2;
					object->vertex_count += 3;
					object->vertex_data = g_realloc(object->vertex_data,
						object->vertex_count * 3 * sizeof(gfloat));
					object->faces = g_slist_append(object->faces, face);
				}
				break;
			case 2: /* name */
				dxf_read_string(global, str);
				break;
			case 8:
				dxf_read_string(global, str);
				break;
			case 10:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[voff * 3 + 0] = dbl;
				break;
			case 11:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[(voff + 1) * 3 + 0] = dbl;
				break;
			case 12:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[(voff + 2) * 3 + 0] = dbl;
				break;
			case 13:
				dbl = dxf_read_float64(global);
				if(face) {
					face->vertex_count = 4;
					face->vertex_indices = g_realloc(face->vertex_indices,
						4 * sizeof(guint32));
					face->vertex_indices[3] = voff + 3;
					object->vertex_count ++;
					object->vertex_data = g_realloc(object->vertex_data,
						object->vertex_count * 3 * sizeof(gfloat));
					object->vertex_data[(voff + 3) * 3 + 0] = dbl;
				}
				break;
			case 20:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[voff * 3 + 1] = dbl;
				break;
			case 21:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[(voff + 1) * 3 + 1] = dbl;
				break;
			case 22:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[(voff + 2) * 3 + 1] = dbl;
				break;
			case 23:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[(voff + 3) * 3 + 1] = dbl;
				break;
			case 30:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[voff * 3 + 2] = dbl;
				break;
			case 31:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[(voff + 1) * 3 + 2] = dbl;
				break;
			case 32:
				dbl = dxf_read_float64(global);
				if(face)
					object->vertex_data[(voff + 2) * 3 + 2] = dbl;
				break;
			case 33:
				dbl = dxf_read_float64(global);
				if(face) {
					object->vertex_data[(voff + 3) * 3 + 2] = dbl;
					if(dxf_vertex_equals(
						object->vertex_data + (voff + 2) * 3,
						object->vertex_data + (voff + 3) * 3))
						face->vertex_count = 3;
				}
				break;
			case 40:
			case 41:
			case 50: /* angle */
				dbl = dxf_read_float64(global);
				break;
			case 62: /* color number */
			case 66:
			case 70:
			case 71:
			case 72:
				dxf_read_int16(global);
				break;
			case 210: /* extrusion */
			case 220:
			case 230:
				dbl = dxf_read_float64(global);
				break;
			default:
				DXF_HANDLE_UNKNOWN(global, key, str, "ENTITIES");
				break;
		} /* key */
	}
	return dxf_skip_section(global);
}
