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
#include <g3d/iff.h>

#include "dxtn.h"

static guint32 get_bitshift(guint32 mask) {
	size_t i;
	guint32 shift = 0;
	for (i = 0; i < 32; i ++)
		if (mask & 1)
			break;
		else {
			mask >>= 1;
			shift ++;
		}
	return shift;
}

static guint32 get_bitlen(guint32 mask) {
	size_t i;
	guint32 n_bits = 0;
	mask >>= get_bitshift(mask);
	for (i = 0; i < 32; i ++)
		if (mask & 1) {
			n_bits ++;
			mask >>= 1;
		}
		else
			break;
	return n_bits;
}

gboolean plugin_load_image_from_stream(G3DContext *context, G3DStream *stream,
	G3DImage *image, gpointer user_data)
{
	guint32 filesize, offset, headsize, compression;
	gint32 x, y, i;
	guint32 ncolplanes, c, width, height, depth, rowstride;
	guint8 *pixeldata;

	gboolean bitfield_compression = FALSE;
	guint32 bitmask[4];
	guint32 bitlen[4];
	guint32 bitshift[4];

	/* bitmap file always starts with 'BM' */
	if(g3d_stream_read_int16_le(stream) != ('B' | ('M' << 8))) {
		g_warning("bitmap magic not found: image seems to be corrupt\n");
		return FALSE;
	}

	g3d_image_set_name(image, g3d_stream_get_uri(stream));

	/* 2 */
	filesize = g3d_stream_read_int32_le(stream);      /* file size */
	g3d_stream_read_int32_le(stream);                 /* 2 x UINT16 reserved */
	offset   = g3d_stream_read_int32_le(stream);      /* offset of data */
	headsize = g3d_stream_read_int32_le(stream);      /* size of header */
	/* 18 */
	width  = g3d_stream_read_int32_le(stream);        /* width */
	height = g3d_stream_read_int32_le(stream);        /* height */
	ncolplanes = g3d_stream_read_int16_le(stream);    /* num of color planes */
	depth  = g3d_stream_read_int16_le(stream);        /* bits per pixel */
	/* 30 */
	compression = g3d_stream_read_int32_be(stream);   /* compression, fourcc */
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
	rowstride = width * 4;

	switch (compression) {
	case G3D_IFF_MKID('D','X','T','1'):
	case G3D_IFF_MKID('D','X','T','3'):
	case G3D_IFF_MKID('D','X','T','5'):
		g_debug("supported DXT%i compression %x", (compression & 0xFF) - '0', compression);
		return decode_dxtn(image, stream, (compression & 0xFF) - '0');
		break;
	case 0x03000000: /* "3" byte-swapped as read as BE */
		g_debug("bit fields %i", headsize);
		g3d_stream_seek(stream, 0x36, G_SEEK_SET);
		bitfield_compression = TRUE;
		for (i = 0; i < 3; i ++) {
			bitmask[i] = g3d_stream_read_int32_le(stream);
			bitlen[i] = get_bitlen(bitmask[i]);
			bitshift[i] = get_bitshift(bitmask[i]);
			g_debug("mask %i: 0x%08x (len %i, shift %i)", i, bitmask[i], bitlen[i], bitshift[i]);
		}
		if (headsize > 40) {
			bitmask[3] = g3d_stream_read_int32_le(stream);
			bitlen[3] = get_bitlen(bitmask[3]);
			bitshift[3] = get_bitshift(bitmask[3]);
			g_debug("mask 3: 0x%08x", bitmask[3]);
		}
		else {
			bitmask[3] = 0;
		}
		g3d_stream_seek(stream, offset, G_SEEK_SET);
		break;
	case 0:
		break;
	default:
		g_debug("unknown compression 0x%08x, assuming uncompressed...", compression);
		break;
	}

	for(y = (height - 1); y >= 0; y --) {
		for(x = 0; x < width; x ++) {
			switch(depth) {
				case 8:
					c = g3d_stream_read_int8(stream);
					for(i = 0; i < 3; i ++)
						pixeldata[y * rowstride + x * 4 + i] = c;
					pixeldata[y * rowstride + x * 4 + 3] = 0xFF;
					break;
				case 16:
					c = g3d_stream_read_int16_le(stream);
					if (bitfield_compression) {
						for (i = 0; i < 3; i ++) {
							pixeldata[y * rowstride + x * 4 + i] =
								((c & bitmask[i]) >> bitshift[i]) << (8 - bitlen[i]);
						}
						if (bitmask[3])
							pixeldata[y * rowstride + x * 4 + 3] =
								((c & bitmask[3]) >> bitshift[3]) << (8 - bitlen[3]);
						else
							pixeldata[y * rowstride + x * 4 + 3] = 0xff;
					}
					else {
						for (i = 2; i >= 0; i --) {
							pixeldata[y * rowstride + x * 4 + i] = (c & 0x1f) << 3;
							c >>= 5;
						}
						pixeldata[y * rowstride + x * 4 + 3] = 0xff;
					}
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

