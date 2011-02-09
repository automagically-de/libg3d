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
#include <errno.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/image.h>

gboolean plugin_load_image_from_stream(G3DContext *context, G3DStream *stream,
	G3DImage *image, gpointer user_data)
{
	guint32 filesize, offset, headsize, compression;
	gint32 x, y, i;
	guint32 ncolplanes, c, width, height, depth, rowstride;
	guint8 *pixeldata;

	/* bitmap file always starts with 'BM' */
	if(g3d_stream_read_int16_le(stream) != ('B' | ('M' << 8))) {
		g_warning("bitmap magic not found: image seems to be corrupt\n");
		return FALSE;
	}

	g3d_image_set_name(image, stream->uri);

	filesize = g3d_stream_read_int32_le(stream);      /* file size */
	g3d_stream_read_int32_le(stream);                 /* 2 x UINT16 reserved */
	offset   = g3d_stream_read_int32_le(stream);      /* offset of data */
	headsize = g3d_stream_read_int32_le(stream);      /* size of header */
	width  = g3d_stream_read_int32_le(stream);        /* width */
	height = g3d_stream_read_int32_le(stream);        /* height */
	ncolplanes = g3d_stream_read_int16_le(stream);    /* num of color planes */
	depth  = g3d_stream_read_int16_le(stream);        /* bits per pixel */
	compression   = g3d_stream_read_int32_le(stream); /* compression */
	g3d_stream_read_int32_le(stream);                 /* image size */
	g3d_stream_read_int32_le(stream);                 /* v/res (dpi) */
	g3d_stream_read_int32_le(stream);                 /* h/res (dpi) */

#if DEBUG > 0
	g_debug("BMP: %dx%dx%d (%d, 0x%x)", width, height,
		depth, ncolplanes, compression);
#endif

	g3d_stream_seek(stream, offset, G_SEEK_SET);

	g3d_image_set_size(image, width, height);
	pixeldata = g3d_image_get_pixels(image);
	rowstride = width * height * 4;

	for(y = (height - 1); y >= 0; y --) {
		for(x = 0; x < width; x ++) {
			switch(depth) {
				case 8:
					c = g3d_stream_read_int8(stream);
					for(i = 0; i < 3; i ++)
						pixeldata[y * rowstride + x * 4 + i] = c;
					pixeldata[y * rowstride + x * 4 + 3] = 0xFF;
					break;
				case 24:
#if 1
					/* read BGR */
					for(i = 2; i >= 0; i --)
#else
					for(i = 0; i < 3; i ++)
#endif
						pixeldata[y * rowstride + x * 4 + i] =
							g3d_stream_read_int8(stream);
					pixeldata[y * rowstride + x * 4 + 3] = 0xFF;
					break;
				default:
					break;
			}
		} /* x */
#if 1
		/* padding */
		for(i = x; i < ((width + 3) & ~(3)); i ++)
			g3d_stream_read_int8(stream);
#endif
	} /* y */
#if DEBUG > 2
	g_debug("bitmap successfully loaded");
#endif

	return TRUE;
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup(
		"Plugin to read Windows Bitmap (.bmp) images\n"
		"(for use as textures or height fields)\n"
		"Author: Markus Dahms");
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("bmp", ":", 0);
}

