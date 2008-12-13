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

#include <g3d/context.h>

#include "imp_dxf.h"
#include "imp_dxf_vars.h"
#include "imp_dxf_chunks.h"

static DxfChunkInfo *dxf_get_chunk_info(DxfChunkInfo *chunks, gint32 id)
{
	guint32 i;

	for(i = 0; chunks[i].id != DXF_GRPCODE_TERMINATOR; i ++)
		if(chunks[i].id == id)
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

static gboolean dxf_parse_chunks(DxfGlobalData *global, DxfChunkInfo *chunks,
	gint32 parentid, const gchar *section)
{
	gint32 key;
	DxfChunkInfo *chunk_info;
	DxfLocalData *local;
	DxfEntityData *edata;
	gchar *entity = NULL;
	gchar str[DXF_MAX_LINE + 1];

#if DEBUG > 0
	g_debug("\\[%s]", section);
#endif

	edata = g_new0(DxfEntityData, 1);

	if((strcmp(section, "ENTITIES") == 0) ||
		(strcmp(section, "BLOCKS") == 0))	{
		edata->object = g_slist_nth_data(global->model->objects, 0);
		edata->material = g_slist_nth_data(edata->object->materials, 0);
	}

	while(TRUE) {
		key = dxf_read_code(global);
		chunk_info = dxf_get_chunk_info(chunks, key);

		if(key == DXF_CODE_INVALID) {
			g_free(edata);
			return FALSE;
		}

#if DEBUG > 0
		if(chunk_info)
			g_debug("\\ [%+4d]: %s (line %d)", key, chunk_info->description,
				g3d_stream_line(global->stream));
		else
			g_warning("unknown chunk type %d in line %d", key,
				g3d_stream_line(global->stream));
#endif

		if(key == 0) { /* new entity or end of section */
			dxf_read_string(global, str);
			DXF_TEST_ENDSEC(str);
			if(entity != NULL)
				g_free(entity);
			entity = g_strdup(str);
			edata->face = NULL;
#if DEBUG > 0
			g_debug("|  entity: %s", entity);
#endif
		}

		if(chunk_info) {
			if(chunk_info->callback) {
				local = g_new0(DxfLocalData, 1);
				local->id = key;
				local->parentid = parentid;
				local->entity = entity;
				local->edata = edata;

				chunk_info->callback(global, local);

				g_free(local);
			} /* callback */
			else {
				DXF_HANDLE_UNKNOWN(global, key, str, section);
			}
		} /* chunk_info */
		else {
			DXF_HANDLE_UNKNOWN(global, key, str, section);
		}

		g3d_context_update_interface(global->context);
	} /* endless loop */

	g_free(edata);
	return FALSE;
}

gboolean dxf_section_ENTITIES(DxfGlobalData *global)
{
	return dxf_parse_chunks(global, dxf_chunks, DXF_ID_ENTITIES, "ENTITIES");
}

gboolean dxf_section_BLOCKS(DxfGlobalData *global)
{
	return dxf_parse_chunks(global, dxf_chunks, DXF_ID_BLOCKS, "BLOCKS");
}

gboolean dxf_section_TABLES(DxfGlobalData *global)
{
	return dxf_parse_chunks(global, dxf_chunks, DXF_ID_TABLES, "TABLES");
}
