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
#include <string.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/image.h>
#include <g3d/iff.h>

#include "dxtn.h"

gboolean plugin_load_image_from_stream(G3DContext *context, G3DStream *stream,
	G3DImage *image, gpointer user_data)
{
	guint32 magic, size, flags, width, height, depth;
	guint32 pfsize, pfflags, pffourcc, pfbpp;
	guint32 pfrmask, pfgmask, pfbmask, pfamask;
	guint32 caps1, caps2;
	guint32 x, y, rowstride;
	guint8 *pixeldata;
	gchar *sfourcc;

	magic = g3d_stream_read_int32_be(stream);
	if(magic != G3D_IFF_MKID('D','D','S',' ')) {
		g_warning("%s is not a DDS file", g3d_stream_get_uri(stream));
		return FALSE;
	}
	g3d_image_set_name(image, g3d_stream_get_uri(stream));

	/* 0x0004 */
	size = g3d_stream_read_int32_le(stream);
	flags = g3d_stream_read_int32_le(stream);
	height = g3d_stream_read_int32_le(stream);
	/* 0x0010 */
	width = g3d_stream_read_int32_le(stream);
	g3d_stream_read_int32_le(stream); /* pitch or linesize */
	depth = g3d_stream_read_int32_le(stream);
	g3d_stream_read_int32_le(stream); /* num mipmaps */

	/* 0x0020 */
	g3d_stream_skip(stream, 44);

	/* 0x004c */
	pfsize = g3d_stream_read_int32_le(stream);
	pfflags = g3d_stream_read_int32_le(stream);

	/* 0x0054 */
	pffourcc = g3d_stream_read_int32_be(stream);
	sfourcc = g3d_iff_id_to_text(pffourcc);

	/* 0x0058 */
	pfbpp = g3d_stream_read_int32_le(stream);
	pfrmask = g3d_stream_read_int32_le(stream);
	pfgmask = g3d_stream_read_int32_le(stream);
	pfbmask = g3d_stream_read_int32_le(stream);
	pfamask = g3d_stream_read_int32_le(stream);
	caps1 = g3d_stream_read_int32_le(stream);
	caps2 = g3d_stream_read_int32_le(stream);

	/* 0x0074 */
	g3d_stream_skip(stream, 12);

	/* 0x0080 */

#if DEBUG > 0
	g_debug("DDS: %ux%u %s 0x%08x", width, height,
		sfourcc, pfflags);
	g_debug("DDS: masks: 0x%04x, 0x%04x, 0x%04x, 0x%04x",
		pfrmask, pfgmask, pfbmask, pfamask);
#endif

	g3d_image_set_size(image, width, height);

	switch(pffourcc) {
		case 0: /* no compression */
			/* XXX: depth & RGBA masks are ignored */
			pixeldata = g3d_image_get_pixels(image);
			rowstride = width * 4;
			for (y = 0; y < height; y ++) {
				for (x = 0; x < width; x ++) {
					pixeldata[y * rowstride + x * 4 + 2] = g3d_stream_read_int8(stream);
					pixeldata[y * rowstride + x * 4 + 1] = g3d_stream_read_int8(stream);
					pixeldata[y * rowstride + x * 4 + 0] = g3d_stream_read_int8(stream);
					pixeldata[y * rowstride + x * 4 + 3] = g3d_stream_read_int8(stream);
				}
			}
			break;
		case G3D_IFF_MKID('D','X','T','1'):
		case G3D_IFF_MKID('D','X','T','3'):
		case G3D_IFF_MKID('D','X','T','5'):
			decode_dxtn(image, stream, (pffourcc & 0xff) - '0');
			break;
		default:
			g_warning("DDS: unsupported FOURCC: %s", sfourcc);
			g_free(sfourcc);
			return FALSE;
	}

	g_free(sfourcc);
	return TRUE;
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup(
		"DDS texture loader\n"
		"Author: Markus Dahms");
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("dds", ":", 0);
}
