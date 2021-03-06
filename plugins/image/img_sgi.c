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

#define SGI_STORAGE_VERBATIM  0
#define SGI_STORAGE_RLE       1

gboolean plugin_load_image_from_stream(G3DContext *context, G3DStream *stream,
	G3DImage *image, gpointer user_data)
{
	guint32 cmap, planes, x, y, p, width, height;
	guint16 dims;
	guint8 storage, bpc;
	guint8 *pixeldata;
	gchar *name;

	if(g3d_stream_read_int16_be(stream) != 474) {
		g_warning("file '%s' is not a SGI RGB file",
			g3d_stream_get_uri(stream));
		return FALSE;
	}

	storage = g3d_stream_read_int8(stream);
	bpc = g3d_stream_read_int8(stream);
	dims = g3d_stream_read_int16_be(stream);

	if(bpc != 1) {
		g_warning("SGI: %s: bpc != 1 -- unsupported",
			g3d_stream_get_uri(stream));
		return FALSE;
	}

	width = g3d_stream_read_int16_be(stream);
	height = g3d_stream_read_int16_be(stream);

	planes = g3d_stream_read_int16_be(stream); /* ZSIZE */
	g3d_stream_read_int32_be(stream); /* PIXMIN */
	g3d_stream_read_int32_be(stream); /* PIXMAX */
	g3d_stream_read_int32_be(stream); /* DUMMY */
	name = g_new0(gchar, 81);
	g3d_stream_read(stream, name, 80);

	if(strlen(name) > 0) {
#if DEBUG > 0
		g_debug("SGI: image name: %s", name);
#endif
		g3d_image_set_name(image, name);
	} else {
		g3d_image_set_name(image, g3d_stream_get_uri(stream));
	}
	g_free(name);

	cmap = g3d_stream_read_int32_be(stream); /* COLORMAP */
	g3d_stream_skip(stream, 404);

#if DEBUG > 0
	g_debug("SGI: %dx%dx%d, %d bpc, colormap: 0x%02x",
		width, height, planes, bpc, cmap);
#endif
	/* end of header */

	g3d_image_set_size(image, width, height);
	pixeldata = g3d_image_get_pixels(image);

	if(storage == SGI_STORAGE_VERBATIM) {
		for(p = 0; p < planes; p ++) {
			for(y = 0; y < height; y ++) {
				for(x = 0; x < width; x ++) {
					pixeldata[(y * width + x) * 4 + p] =
						g3d_stream_read_int8(stream);

					if(planes == 1) {
						/* greyscale: g = r; b = r; */
						pixeldata[(y * width + x) * 4 + 1] =
							pixeldata[(y * width + x) * 4];
						pixeldata[(y * width + x) * 4 + 2] =
							pixeldata[(y * width + x) * 4];
					}
				} /* x */
			} /* y */
		} /* p */
	} /* verbatim */
	else /* RLE */ {
		guint32 *starttab, *lengthtab, rleoff, rlelen;
		guint8 cnt, pixel;

		starttab = g_new0(guint32, height * planes);
		lengthtab = g_new0(guint32, height * planes);

		/* read starttab */
		for(p = 0; p < planes; p ++)
			for(y = 0; y < height; y ++)
				starttab[y * planes + p] = g3d_stream_read_int32_be(stream);
		/* read lengthtab */
		for(p = 0; p < planes; p ++)
			for(y = 0; y < height; y ++)
				lengthtab[y * planes + p] = g3d_stream_read_int32_be(stream);

		/* read image data */
		for(p = 0; p < planes; p ++)
			for(y = 0; y < height; y ++)
			{
				rleoff = starttab[y * planes + p];
				rlelen = lengthtab[y * planes + p];

				g3d_stream_seek(stream, rleoff, G_SEEK_SET);

				x = 0;

				while(1) {
					pixel = g3d_stream_read_int8(stream);
					cnt = pixel & 0x7F;

					if(cnt == 0)
						break;

					if(pixel & 0x80) {
						/* copy n bytes */
						while(cnt --) {
							pixeldata[(y * width + x) * 4 + p] =
		                        g3d_stream_read_int8(stream);
							x ++;
						}
					} else {
						/* repeat next byte n times */
						pixel = g3d_stream_read_int8(stream);
						while(cnt --) {
							pixeldata[(y * width + x) * 4 + p] = pixel;
							x ++;
						}
					}
				}
			}

		g_free(starttab);
		g_free(lengthtab);
	}

	/* set alpha to 1.0 */
	if(planes < 4)
		for(y = 0; y < height; y ++)
			for(x = 0; x < width; x ++)
				pixeldata[(y * width + x) * 4 + 3] = 0xFF;

	return TRUE;
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup(
		"Plugin to read SGI RGB (.rgb) images\n");
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("rgb:rgba:sgi", ":", 0);
}

