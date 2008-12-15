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
#include "imp_dxf_chunks.h"
#include "imp_dxf_entities.h"

static DxfChunkInfo *dxf_get_chunk_info(DxfChunkInfo *chunks, gint32 id)
{
	guint32 i;

	for(i = 0; chunks[i].id != DXF_CODE_INVALID; i ++)
		if(chunks[i].id == id)
			return &(chunks[i]);
	return NULL;
}

static DxfEntityInfo *dxf_get_entity_info(const gchar *str)
{
	guint32 i;

	for(i = 0; dxf_entities[i].name != NULL; i ++)
		if(strcmp(dxf_entities[i].name, str) == 0)
			return &(dxf_entities[i]);
	return NULL;
}

static gboolean dxf_parse_chunks(DxfGlobalData *global, DxfChunkInfo *chunks,
	gint32 parentid, const gchar *section)
{
	gint32 key;
	DxfChunkInfo *chunk_info;
	DxfLocalData *local;
	DxfEntityData *edata;
	DxfEntityInfo *einfo = NULL;
	gchar str[DXF_MAX_LINE + 1];
	gfloat pcnt, prev_pcnt = 0.0;

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
			einfo = dxf_get_entity_info(str);
			edata->face = NULL;
#if DEBUG > 0
			g_debug("|  entity: %s", str);
#endif
		}

		if(chunk_info) {
			if(chunk_info->callback) {
				local = g_new0(DxfLocalData, 1);
				local->id = key;
				local->parentid = parentid;
				local->eid = einfo ? einfo->id : DXF_E_OTHER;
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

		pcnt = (gfloat)g3d_stream_tell(global->stream) /
			(gfloat)g3d_stream_size(global->stream);
		if((pcnt - prev_pcnt) > 0.01) {
			prev_pcnt = pcnt;
			g3d_context_update_progress_bar(global->context, pcnt, TRUE);
		}
		g3d_context_update_interface(global->context);
	} /* endless loop */

	g_free(edata);
	return FALSE;
}

gboolean dxf_section_HEADER(DxfGlobalData *global)
{
	return dxf_parse_chunks(global, dxf_chunks, DXF_ID_HEADER, "HEADER");
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

gboolean dxf_section_OBJECTS(DxfGlobalData *global)
{
	return dxf_parse_chunks(global, dxf_chunks, DXF_ID_OBJECTS, "OBJECTS");
}

gboolean dxf_section_CLASSES(DxfGlobalData *global)
{
	return dxf_parse_chunks(global, dxf_chunks, DXF_ID_CLASSES, "TABLES");
}
