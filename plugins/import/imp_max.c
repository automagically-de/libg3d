/* $Id: imp_3dmf.c 75 2006-11-06 10:52:56Z mmmaddd $ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005, 2006  Markus Dahms <mad@automagically.de>

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
#include <string.h>

#include <g3d/types.h>
#include <g3d/context.h>
#include <g3d/read.h>
#include <g3d/object.h>

#include "imp_max_chunks.h"

static gchar *max_read_mbstr(FILE *f, guint32 *len);
static gchar *max_read_header128(FILE *f, gint32 *data);
static gboolean max_read_chunk(MaxGlobalData *global, gint32 *nb,
	guint32 level);
static MaxChunk *max_get_chunk_desc(guint16 id, gboolean container);

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model)
{
	FILE *f;
	gchar *name;
	gint32 i, n, data[16];
	MaxGlobalData *global;

	f = fopen(filename, "rb");
	if(f == NULL) {
		g_warning("failed to open %s", filename);
		return FALSE;
	}

	fseek(f, 0x400, SEEK_SET);
	/* Root Entry */
	name = max_read_header128(f, data);
	g_free(name);
	n = data[3];
#if 0
	/* Scene */
	name = max_read_header128(f, data);
	g_free(name);
#endif
	/* additional headers */
	for(i = 0; i < n; i ++) {
		name = max_read_header128(f, data);
		g_free(name);
	}

	global = g_new0(MaxGlobalData, 1);
	global->context = context;
	global->model = model;
	global->f = f;
	global->padding = "                                               ";
	fseek(f, 0x800, SEEK_SET);
	max_read_chunk(global, NULL, 0);
	g_free(global);
	fclose(f);
	return FALSE;
}

char *plugin_description(void)
{
	return g_strdup("import plugin for 3D Studio MAX files (EXPERIMENTAL)\n");
}

char **plugin_extensions(void)
{
	return g_strsplit("max", ":", 0);
}

/*****************************************************************************
 * max specific
 *****************************************************************************/

static gchar *max_read_mbstr(FILE *f, guint32 *len)
{
	gchar *buffer, *str;
	gint32 i;

	buffer = g_malloc0(65);
	for(i = 0; i < 32; i ++) {
		buffer[i] = g3d_read_int8(f);
		g3d_read_int8(f);
	}

	if(len)
		*len = 64;

	str = g_strdup(buffer);
	g_free(buffer);

	return str;
}

static gchar *max_read_header128(FILE *f, gint32 *data)
{
	gchar *name;
	gint32 i;

	name = max_read_mbstr(f, NULL);

#if DEBUG > 0
	g_debug("MAX: section %s:", name);
#endif
	for(i = 0; i < 16; i ++) {
		data[i] = g3d_read_int32_le(f);
#if DEBUG > 0
		g_debug("MAX:   0x%08x (%d)", data[i], data[i]);
#endif
	}
	return name;
}

static gboolean max_read_chunk(MaxGlobalData *global, gint32 *nb,
	guint32 level)
{
	guint16 id;
	guint32 length;
	gboolean container;
	gint32 bytes;
	MaxChunk *chunk;
	MaxLocalData *local;

	id = g3d_read_int16_le(global->f);
	length = g3d_read_int32_le(global->f);
	container = (length & 0x80000000);
	length &= 0x0FFFFFFF;
	bytes = length - 6;

	if(nb)
		*nb -= 6;

	if(nb && (bytes > *nb))
		return FALSE;

	if(length < 6)
		return FALSE;

	if(nb)
		*nb -= bytes;

	chunk = max_get_chunk_desc(id, container);

#if DEBUG > 0
	g_debug("\\%s(%d)[0x%04X][%c%c] %s - %d (%d) bytes @ 0x%08lx",
		(global->padding + (strlen(global->padding) - level)), level,
		id, (container ? 'c' : ' '),
		(chunk && chunk->callback) ? 'f' : ' ',
		chunk ? chunk->desc : "unknown",
		length - 6, length,
		ftell(global->f));
#endif

	if(container) {
		while(bytes > 0) {
			if(!max_read_chunk(global, &bytes, level + 1)) {
				if(bytes > 0) {
					g_debug("[MAX:0x%04X] skipping %d bytes", id, bytes);
					fseek(global->f, bytes, SEEK_CUR);
					bytes = 0;
				}
			}
		}
	} else if(chunk && chunk->callback) {
		local = g_new0(MaxLocalData, 1);
		local->id = id;
		local->nb = length - 6;
		local->level = level + 1;
		chunk->callback(global, local);
		/* skip remaining bytes */
		if(local->nb > 0)
			fseek(global->f, local->nb, SEEK_CUR);
		g_free(local);
	} else {
		/* skip non-container stuff for now */
		fseek(global->f, bytes, SEEK_CUR);
	}

	return TRUE;
}

static MaxChunk *max_get_chunk_desc(guint16 id, gboolean container)
{
	MaxChunk *chunk, *chunks;
	gint32 i;

	if(container)
		chunks = max_cnt_chunks;
	else
		chunks = max_chunks;

	for(i = 0, chunk = &(chunks[i]); chunk->id <= id;
		i ++, chunk = &(chunks[i])) {
		if(chunk->id == id)
			return chunk;
	}
	return NULL;
}
