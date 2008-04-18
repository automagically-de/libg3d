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

#include <string.h>

#include <g3d/types.h>
#include <g3d/context.h>
#include <g3d/read.h>
#include <g3d/object.h>
#include <g3d/stream.h>

#include "imp_max_chunks.h"

static gboolean max_read_subfile(G3DContext *context, G3DModel *model,
	const gchar *filename, const gchar *subfile);
static gboolean max_read_chunk(MaxGlobalData *global, gint32 *nb,
	guint32 level);
static MaxChunk *max_get_chunk_desc(guint16 id, gboolean container);

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model)
{
	gboolean retval;

	retval = max_read_subfile(context, model, filename, "Config");
	retval = max_read_subfile(context, model, filename, "Scene");

	return retval;
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

static gboolean max_read_subfile(G3DContext *context, G3DModel *model,
	const gchar *filename, const gchar *subfile)
{
	G3DStream *ssf;
	MaxGlobalData *global;
	gint32 fsize;

	ssf = g3d_stream_open_structured_file(filename, subfile);
	if(ssf == NULL) {
		g_warning("MAX: failed to open '%s' in structured file '%s'",
			subfile, filename);
		return FALSE;
	}

	global = g_new0(MaxGlobalData, 1);
	global->context = context;
	global->model = model;
	global->stream = ssf;
	global->padding = "                                               ";

	fsize = g3d_stream_size(ssf);
	while(max_read_chunk(global, &fsize, 0));

	g_free(global);
	g3d_stream_close(ssf);

	return TRUE;
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

	if(nb && (*nb < 6))
		return FALSE;

	id = g3d_stream_read_int16_le(global->stream);
	length = g3d_stream_read_int32_le(global->stream);
	container = (length & 0x80000000);
	length &= 0x7FFFFFFF;
	bytes = length - 6;

	if(nb && (length > *nb))
		return FALSE;

	if(nb)
		*nb -= 6;

	if(nb)
		*nb -= bytes;

	chunk = max_get_chunk_desc(id, container);

#if DEBUG > 0
	g_debug("\\%s(%d)[0x%04X][%c%c] %s - %d (%d) bytes @ 0x%08x",
		(global->padding + (strlen(global->padding) - level)), level,
		id, (container ? 'c' : ' '),
		(chunk && chunk->callback) ? 'f' : ' ',
		chunk ? chunk->desc : "unknown",
		length - 6, length,
		(guint32)g3d_stream_tell(global->stream) - 6);
#endif

	if(container) {
		while(bytes > 0) {
			if(!max_read_chunk(global, &bytes, level + 1))
				return FALSE;
		}
	} else if(chunk && chunk->callback) {
		local = g_new0(MaxLocalData, 1);
		local->id = id;
		local->nb = length - 6;
		local->level = level + 1;
		chunk->callback(global, local);
		/* skip remaining bytes */
		if(local->nb > 0)
			g3d_stream_seek(global->stream, local->nb, G_SEEK_CUR);
		g_free(local);
	} else {
		/* skip non-container stuff for now */
		g3d_stream_seek(global->stream, bytes, G_SEEK_CUR);
	}

	g3d_context_update_interface(global->context);

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
