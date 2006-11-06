/* $Id: $ */

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
#include <stdarg.h>

#include <g3d/types.h>
#include <g3d/read.h>

#define AR_FLAG_COPIED    0x80
#define AR_FLAG_COMPRESS  0x40

typedef struct {
	gchar *name;
	guint32 offset;
	guint32 size;
} ArDirEntry;

GSList *ar_read_directory(FILE *f);
gboolean ar_decompress_to_file(FILE *f, ArDirEntry *dirent);

/*****************************************************************************/
/* plugin interface                                                          */

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer plugin_data)
{
	FILE *f;
	GSList *dir, *item;

	f = fopen(filename, "rb");
	if(f == NULL)
	{
		g_printerr("AR: failed to read '%s'\n", filename);
		return FALSE;
	}

	dir = ar_read_directory(f);
	for(item = dir; item != NULL; item = item->next)
	{
		ar_decompress_to_file(f, (ArDirEntry *)item->data);
	}

	fclose(f);

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Racer .ar files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("ar", ":", 0);
}

/*****************************************************************************/

GSList *ar_read_directory(FILE *f)
{
	ArDirEntry *dirent;
	GSList *list = NULL;
	guint32 fsize, dpos;
	gint32 nbytes;
	gchar buffer[128];

	fseek(f, -4, SEEK_END);
	fsize = ftell(f);
	dpos = g3d_read_int32_le(f);

	/* start of directory */
	fseek(f, dpos, SEEK_SET);
	nbytes = fsize - dpos;
#if DEBUG > 0
	printf("D: AR: directory @ 0x%08x, %d bytes\n", dpos, nbytes);
#endif

	while(nbytes > 0)
	{
		dirent = g_new0(ArDirEntry, 1);
		list = g_slist_append(list, dirent);

		nbytes -= g3d_read_cstr(f, buffer, 127);
		dirent->name = g_strdup(buffer);
		dirent->offset = g3d_read_int32_le(f);
		dirent->size = g3d_read_int32_le(f);
		nbytes -= 8;

#if DEBUG > 0
		printf("D: AR: * %s @ 0x%08x, %d bytes\n",
			dirent->name, dirent->offset, dirent->size);
#endif
	}

	return list;
}

guint8 *ar_decompress_chunk(guint8 *src, guint16 srcsize, guint16 *dstsize)
{
	guint8 *dst = NULL, bit = 16;
	gint32 i, j = 0, k, pos, size;
	guint16 cmd;

#if DEBUG > 2
	printf("D: decompressing %d bytes chunk\n", srcsize);
#endif

	if(src[0] == AR_FLAG_COPIED)
	{
		*dstsize = srcsize - 1;
		dst = g_new0(guint8, *dstsize);
		memcpy(dst, src, *dstsize);
		return dst;
	}

	*dstsize = 0;
	cmd = (src[1] << 8) + src[2];
	for(i = 3; i < srcsize;)
	{
		if(bit == 0)
		{
			/* get new command */
			cmd = (src[i] << 8) + src[i + 1];
			i += 2;
		}

		if(cmd & 0x8000)
		{
			pos = (src[i] << 4) + (src[i + 1] >> 4);
			i ++;
			if(pos != 0)
			{
				/* copy known chunk */
				size = (src[i] & 0xF) + 3;
				*dstsize += size;
				dst = g_realloc(dst, *dstsize);
				i ++;
				for(k = 0; k < size; k ++)
					dst[j + k] = dst[j - pos + k];
				j += size;
			}
			else
			{
				/* RLE style... */
				size = (src[i] << 8) + src[i + 1] + 16;
				*dstsize += size;
				dst = g_realloc(dst, *dstsize);
				i ++;
				for(k = 0; k < size; k ++)
					dst[j + k] = src[i];
				i ++;
				j += size;
			}
		}
		else
		{
			/* plain copy */
			*dstsize += 1;
			dst = g_realloc(dst, *dstsize);
			dst[j] = src[i];
			i ++;
			j ++;
		}

		cmd <<= 1;
		bit --;
	}

	return dst;
}

gboolean ar_decompress_to_file(FILE *f, ArDirEntry *dirent)
{
	FILE *o;
	gchar cmd;
	guint32 size;
	guint16 srcsize, dstsize;
	guint8 *src, *dst;

	o = fopen(dirent->name, "wb");
	if(o == NULL)
	{
		g_printerr("failed to write to '%s'\n", dirent->name);
		return FALSE;
	}

	/* seek to file start */
	fseek(f, dirent->offset, SEEK_SET);

	/* skip tags */
	do
	{
		cmd = g3d_read_int8(f);
		if(cmd != 'D')
		{
			size = g3d_read_int32_le(f);
			fseek(f, size, SEEK_CUR);
		}
	}
	while(cmd != 'D');

#if DEBUG > 2
	printf("D: starting decompression part\n");
#endif

	/* decomress stuff */
	while(1)
	{
		srcsize = g3d_read_int16_le(f);
		if(srcsize == 0)
			break;

		src = g_new0(guint8, srcsize);
		fread(src, 1, srcsize, f);
		dst = ar_decompress_chunk(src, srcsize, &dstsize);

		if(dstsize > 0)
		{
			fwrite(dst, 1, dstsize, o);
			g_free(dst);
		}

		g_free(src);
	}

	fclose(o);

	return TRUE;
}
