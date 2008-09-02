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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <g3d/types.h>
#include <g3d/read.h>

gboolean plugin_load_image(G3DContext *context, const gchar *filename,
	G3DImage *image, gpointer user_data)
{
	FILE *f;
	guint32 filesize, offset, headsize, compression;
	gint32 x, y, i;
	guint32 ncolplanes, c;

	f = fopen(filename, "r");
	if(f == NULL) {
		g_printerr("couldn't open image file '%s': %s\n", filename,
			strerror(errno));
		return FALSE;
	}

	/* bitmap file always starts with 'BM' */
	if(g3d_read_int16_le(f) != ('B' | ('M' << 8))) {
		g_printerr("bitmap magic not found: image seems to be corrupt\n");
		return FALSE;
	}

	image->name = g_strdup(filename);

	filesize = g3d_read_int32_le(f);      /* file size */
	g3d_read_int32_le(f);                 /* 2 x UINT16 reserved */
	offset   = g3d_read_int32_le(f);      /* offset of data */
	headsize = g3d_read_int32_le(f);      /* size of header */
	image->width  = g3d_read_int32_le(f); /* width */
	image->height = g3d_read_int32_le(f); /* height */
	ncolplanes = g3d_read_int16_le(f);    /* num of color planes */
	image->depth  = g3d_read_int16_le(f); /* bits per pixel */
	compression   = g3d_read_int32_le(f); /* compression */
	g3d_read_int32_le(f);                 /* image size */
	g3d_read_int32_le(f);                 /* v/res (dpi) */
	g3d_read_int32_le(f);                 /* h/res (dpi) */

#if DEBUG > 0
	g_debug("BMP: %dx%dx%d (%d, 0x%x)", image->width, image->height,
		image->depth, ncolplanes, compression);
#endif

	fseek(f, offset, SEEK_SET);

#define ALL32BIT
#ifndef ALL32BIT /* always 32bit for now.. */
	image->pixeldata = g_new0(guint8,
		image->width * image->height * (image->depth / 8));
#else
	image->pixeldata = g_new0(guint8, image->width * image->height * 4);
#endif

	for(y = (image->height - 1); y >= 0; y --) {
		for(x = 0; x < image->width; x ++) {
			switch(image->depth) {
				case 8:
#ifndef ALL32BIT
					image->pixeldata[y*image->width+x] = g3d_read_int8(f);
#else
					c = g3d_read_int8(f);
					for(i = 0; i < 3; i ++)
						image->pixeldata[(y * image->width + x) * 4 + i] = c;
					image->pixeldata[(y * image->width + x) * 4 + 3] = 0xFF;
#endif
					break;
				case 24:
					/* read BGR */
					for(i = 2; i >= 0; i --)
						image->pixeldata[(y * image->width + x) * 4 + i] =
							g3d_read_int8(f);
					image->pixeldata[(y * image->width + x) * 4 + 3] = 0xFF;
					break;
				default:
					break;
			}
		} /* x */
#if DEBUG > 5
		g_printerr("\n");
#endif
		/* padding */
		for(i = x; i < ((image->width + 3) & ~(3)); i ++)
			g3d_read_int8(f);
	} /* y */
	image->depth = 32;
#if DEBUG > 2
	g_printerr("bitmap successfully loaded\n");
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

