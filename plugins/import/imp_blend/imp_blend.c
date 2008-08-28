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
#include "imp_blend_sdna.h"
#include "imp_blend_read.h"

#define HEADER_SIZE          12

static gboolean blend_read_file(G3DContext *context, G3DStream *stream,
	G3DModel *model, guint32 flags);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar buf[HEADER_SIZE + 1];
	guint32 flags = 0, version;

	if(g3d_stream_read(stream, buf, 1, HEADER_SIZE) != HEADER_SIZE) {
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

	blend_read_file(context, stream, model, flags);

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

static gboolean blend_read_file(G3DContext *context, G3DStream *stream,
	G3DModel *model, guint32 flags)
{
	guint32 code, len, old, sdnanr, nr, x1;
	gint32 i;
	BlendSdna *sdna;

	while(TRUE) {
		code = blend_read_uint(stream, flags);
		len = blend_read_uint(stream, flags);
		if(code == MKID('E','N','D','B')) {
			return TRUE;
		}
		old = blend_read_uint(stream, flags);
		sdnanr = blend_read_uint(stream, flags);
		nr = blend_read_uint(stream, flags);

		if((sdna == NULL) && (code != MKID('D','N','A','1'))) {
			g3d_stream_skip(stream, len);
			continue;
		}

		g_debug("\\%c%c%c%c: %8d bytes (0x%08x, %d, %d)",
			blend_from_id(code, 0), blend_from_id(code, 1),
			blend_from_id(code, 2), blend_from_id(code, 3),
			len, old, sdnanr, nr);

		if(len == 0)
			return FALSE;

		switch(code) {
			case MKID('D','N','A','1'):
				sdna = blend_sdna_read_dna1(stream, flags, len);
				break;
			default:
				g3d_stream_skip(stream, len);
				break;
		}
	}
	return FALSE;
}
