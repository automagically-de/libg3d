/* $Id$ */

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
#include <stdlib.h>

#include <g3d/types.h>
#include <g3d/stream.h>

#include "imp_blend_def.h"
#include "imp_blend_types.h"
#include "imp_blend_sdna.h"
#include "imp_blend_read.h"

#define HEADER_SIZE          12

static gboolean blend_read_file(BlendGlobal *global);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar buf[HEADER_SIZE + 1];
	guint32 flags = 0, version;
	BlendGlobal *global;

	if(g3d_stream_read(stream, buf, HEADER_SIZE) != HEADER_SIZE) {
		g_warning("Blend: %s: failed to read header", stream->uri);
		return FALSE;
	}
	buf[HEADER_SIZE] = '\0';
	if(strncmp(buf, "BLENDER", 7) != 0) {
		g_warning("Blend: %s: not a blender file", stream->uri);
		return FALSE;
	}
	if(buf[7] == '_')
		flags |= FLAG_POINTERSIZE_4;
	if(buf[8] == 'v')
		flags |= FLAG_LITTLE_ENDIAN;
	version = atoi(buf + 9);

	g_debug("Blend: version %.2f, %s endian, pointer size is %d",
		version / 100.0,
		(flags & FLAG_LITTLE_ENDIAN) ? "little" : "big",
		(flags & FLAG_POINTERSIZE_4) ? 4 : 8);

	if(!(flags & FLAG_POINTERSIZE_4)) {
		g_warning("Blend: 8-byte pointers are not implemented as I lack a "
			"test file. Please send me one :).");
		return FALSE;
	}

	global = g_new0(BlendGlobal, 1);
	global->context = context;
	global->stream = stream;
	global->model = model;
	global->flags = flags;

	blend_read_file(global);

	g_free(global);

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Blender files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("blend", ":", 0);
}

/*****************************************************************************/

static gboolean blend_read_file(BlendGlobal *global)
{
	guint32 code, len, old, sdnanr, nr;
	BlendSdna *sdna = NULL;
	const BlendSdnaStruct *sstruct;
	gint i;

	while(TRUE) {
		code = blend_read_uint(global->stream, global->flags);
		len = blend_read_uint(global->stream, global->flags);
		if(code == MKID('E','N','D','B'))
			return TRUE;

		old = blend_read_uint(global->stream, global->flags);
		sdnanr = blend_read_uint(global->stream, global->flags);
		nr = blend_read_uint(global->stream, global->flags);

		if((sdna == NULL) && (code != MKID('D','N','A','1'))) {
			g3d_stream_skip(global->stream, len);
			continue;
		}

		g_debug("\\%c%c%c%c: %8d bytes (0x%08x, %d, %d)",
			blend_from_id(code, 0), blend_from_id(code, 1),
			blend_from_id(code, 2), blend_from_id(code, 3),
			len, old, sdnanr, nr);

		if(len == 0)
			return FALSE;

#if DEBUG > 0
		if(sdna) {
			sstruct = blend_sdna_get_struct_by_id(sdna, sdnanr);
			if(sstruct) {
				g_debug("|struct %s /* %d */", sstruct->name, sstruct->size);
				blend_sdna_data_read(sdna, sstruct, global, &len, 0);
			}
		}
#endif

		switch(code) {
			case MKID('D','N','A','1'):
				if(sdna != NULL) {
					/* already read */
					g3d_stream_skip(global->stream, len);
					break;
				}
				sdna = blend_sdna_read_dna1(global->stream, global->flags,
					len);
				if(sdna == NULL) {
					g_warning("Blend: failed to read DNA1, giving up...");
					return FALSE;
				}
#if DEBUG > 0
				for(i = 0; i < sdna->n_types; i ++)
					g_debug("| type %04i: %s", i, sdna->type_names[i]);
#endif
				/* rewind stream to really read content */
				g3d_stream_seek(global->stream, HEADER_SIZE, G_SEEK_SET);
				break;
			case MKID('D','A','T','A'):
				g3d_stream_skip(global->stream, len);
				break;
			default:
				g3d_stream_skip(global->stream, len);
				break;
		}
	}


	blend_sdna_cleanup(sdna);
	return TRUE;
}
