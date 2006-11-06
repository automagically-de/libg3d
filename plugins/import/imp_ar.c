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
#include <g3d/iff.h>
#include <g3d/texture.h>
#include <g3d/material.h>

#define AR_FLAG_COPIED    0x80
#define AR_FLAG_COMPRESS  0x40

typedef struct {
	gchar *name;
	guint32 offset;
	guint32 size;
} ArDirEntry;

GSList *ar_read_directory(FILE *f);
gboolean ar_decompress_to_file(FILE *f, ArDirEntry *dirent);
G3DObject *ar_load_dof(G3DContext *context, G3DModel *model,
	const gchar *filename);

/*****************************************************************************/
/* plugin interface                                                          */

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer plugin_data)
{
	FILE *f;
	GSList *dir, *item;
	G3DMaterial *material;

	/* default material */
	material = g3d_material_new();
	material->name = g_strdup("default material");
	model->materials = g_slist_append(model->materials, material);

	if(g_strcasecmp(filename + (strlen(filename) - 4), ".dof") == 0)
	{
		/* single DOF file */
		ar_load_dof(context, model, filename);
	}
	else
	{
		/* compressed AR archive */
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
			if(g_strcasecmp(((ArDirEntry *)item->data)->name, "body.dof") == 0)
			{
				ar_load_dof(context, model, ((ArDirEntry *)item->data)->name);
			}
		}

		fclose(f);
	}

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Racer .ar files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("ar:dof", ":", 0);
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
		memcpy(dst, src + 1, *dstsize);
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
			bit = 16;
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
				i += 2;
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

	/* decompress stuff */
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

gchar *ar_read_string(FILE *f, gint32 *dlen)
{
	gint32 len;
	gchar *text;

	len = g3d_read_int16_le(f);
	*dlen -= 2;

	text = g_new0(gchar, len + 1);
	fread(text, 1, len, f);
	*dlen -= len;

	return text;
}

G3DMaterial *ar_dof_load_mat(G3DContext *context, G3DModel *model, FILE *f)
{
	G3DMaterial *material;
	gint32 id, len, dlen, i, ntex;
	gchar *tmp;

	id = g3d_read_int32_be(f);
	if(id != G3D_IFF_MKID('M','A','T','0'))
		return NULL;

	material = g3d_material_new();

	dlen = g3d_read_int32_le(f);
	do
	{
		id = g3d_read_int32_be(f);
		if(id != G3D_IFF_MKID('M','E','N','D'))
			len = g3d_read_int32_le(f);

		switch(id)
		{
			case G3D_IFF_MKID('M','E','N','D'):
				break;

			case G3D_IFF_MKID('M','H','D','R'):
				material->name = ar_read_string(f, &dlen);
				tmp = ar_read_string(f, &dlen);
				g_free(tmp);
				break;

			case G3D_IFF_MKID('M','C','O','L'):
				/* ambient */
				material->r = g3d_read_float_le(f);
				material->g = g3d_read_float_le(f);
				material->b = g3d_read_float_le(f);
				material->a = g3d_read_float_le(f);
				dlen -= 16;
				/* diffuse */
				fseek(f, 16, SEEK_CUR);
				dlen -= 16;
				/* specular */
				material->specular[0] = g3d_read_float_le(f);
				material->specular[1] = g3d_read_float_le(f);
				material->specular[2] = g3d_read_float_le(f);
				material->specular[3] = g3d_read_float_le(f);
				dlen -= 16;
				/* emission */
				fseek(f, 16, SEEK_CUR);
				dlen -= 16;
				/* shininess */
				material->shininess = g3d_read_float_le(f);
				dlen -= 4;
				break;

			case G3D_IFF_MKID('M','T','E','X'):
				ntex = g3d_read_int32_le(f);
				dlen -= 4;
				for(i = 0; i < ntex; i ++)
				{
					tmp = ar_read_string(f, &dlen);
					if(i == 0)
					{
						material->tex_image =
							g3d_texture_load_cached(context, model, tmp);
						if(material->tex_image)
							material->tex_image->tex_id = g_str_hash(tmp);
					}
					g_free(tmp);
				}
				break;

			default:
				fseek(f, len, SEEK_CUR);
				dlen -= len;
				break;
		}
	}
	while((dlen > 0) && (id != G3D_IFF_MKID('M','E','N','D')));

	return material;
}

G3DObject *ar_dof_load_obj(G3DContext *context, G3DModel *model, FILE *f)
{
	G3DObject *object, *pobj;
	G3DFace *face;
	G3DMaterial *material;
	GSList *item;
	gint32 id, len, dlen, nverts, ntver, nind, i, j, index;
	gfloat *tex_vertices = NULL;

	id = g3d_read_int32_be(f);
	dlen = g3d_read_int32_le(f);

	if(id != G3D_IFF_MKID('G','O','B','1'))
	{
		fseek(f, dlen, SEEK_CUR);
		return NULL;
	}

	object = g_new0(G3DObject, 1);
	object->name = g_strdup_printf("object @ 0x%08x", (guint32)ftell(f));

	/* parent object for material references */
	pobj = (G3DObject *)g_slist_nth_data(model->objects, 0);

	/* default material */
	material = (G3DMaterial *)g_slist_nth_data(model->materials, 0);

	do
	{
		id = g3d_read_int32_be(f);
		if(id != G3D_IFF_MKID('G','E','N','D'))
			len = g3d_read_int32_le(f);

		switch(id)
		{
			case G3D_IFF_MKID('G','E','N','D'):
				break;

			case G3D_IFF_MKID('G','H','D','R'):
				/* flags */
				g3d_read_int32_le(f);
				/* paint flags */
				g3d_read_int32_le(f);

				/* material ref */
				i = g3d_read_int32_le(f);
				material = g_slist_nth_data(pobj->materials, i);
				if(material == NULL)
					material = (G3DMaterial *)g_slist_nth_data(
						model->materials, 0);

				dlen -= 12;
				break;

			case G3D_IFF_MKID('V','E','R','T'):
				nverts = g3d_read_int32_le(f);

				printf("D: %d vertices\n", nverts);

				dlen -= 4;
				if(nverts > 0)
				{
					object->vertex_count = nverts;
					object->vertex_data = g_new0(gfloat, nverts * 3);
					for(i = 0; i < nverts; i ++)
					{
						object->vertex_data[i * 3 + 0] = g3d_read_float_le(f);
						object->vertex_data[i * 3 + 1] = g3d_read_float_le(f);
						object->vertex_data[i * 3 + 2] = g3d_read_float_le(f);
						dlen -= 12;
					}
				}
				break;

			case G3D_IFF_MKID('T', 'V','E','R'):
				ntver = g3d_read_int32_le(f);
				tex_vertices = g_new0(gfloat, ntver * 2);
				dlen -= 4;

				printf("D: %d texture vertices\n", ntver);

				for(i = 0; i < ntver; i ++)
				{
					tex_vertices[i * 2 + 0] = g3d_read_float_le(f);
					tex_vertices[i * 2 + 1] = g3d_read_float_le(f);
					dlen -= 8;
				}
				break;

			case G3D_IFF_MKID('I','N','D','I'):
				nind = g3d_read_int32_le(f);
				dlen -= 4;
				len -= 4;

				printf("D: %d indices in %d bytes\n", nind, len);

				for(i = 0; i < nind; i += 3)
				{
					face = g_new0(G3DFace, 1);
					face->material = material;
					face->vertex_count = 3;
					face->vertex_indices = g_new0(guint32, 3);

					face->vertex_indices[0] = g3d_read_int16_le(f);
					face->vertex_indices[1] = g3d_read_int16_le(f);
					face->vertex_indices[2] = g3d_read_int16_le(f);
					dlen -= 6;
					len -= 6;

					object->faces = g_slist_append(object->faces, face);
				}
				break;

			default:
				fseek(f, len, SEEK_CUR);
				dlen -= len;
				break;
		}
	}
	while((dlen > 0) && (id != G3D_IFF_MKID('G','E','N','D')));

	/* fix faces with normals and texture vertices */
	for(item = object->faces; item != NULL; item = item->next)
	{
		face = (G3DFace *)item->data;

		if(tex_vertices != NULL)
		{
			face->tex_image = material->tex_image;
			face->tex_vertex_count = 3;
			face->tex_vertex_data = g_new0(gfloat, 3 * 2);
			for(j = 0; j < 3; j ++)
			{
				index = face->vertex_indices[j];
				face->tex_vertex_data[j * 2 + 0] = tex_vertices[index * 2 + 0];
				face->tex_vertex_data[j * 2 + 1] = tex_vertices[index * 2 + 1];
			}
			if(face->tex_image != NULL)
				face->flags |= G3D_FLAG_FAC_TEXMAP;
		}
	}

	if(tex_vertices != NULL)
		g_free(tex_vertices);

	return object;
}

G3DObject *ar_load_dof(G3DContext *context, G3DModel *model,
	const gchar *filename)
{
	FILE *f;
	gint32 id, dlen, len, nmat, nobj, i;
	G3DObject *object, *cobj;
	G3DMaterial *material;

	f = fopen(filename, "rb");
	if(f == NULL)
	{
		g_warning("failed to read '%s'\n", filename);
		return NULL;
	}

	/* file is little-endian, but read IDs as big-endian to use
	 * G3D_IFF_MKID to compare */

	id = g3d_read_int32_be(f);
	if(id != G3D_IFF_MKID('D','O','F','1'))
	{
		g_warning("%s is not a DOF1 file\n", filename);
		fclose(f);
		return NULL;
	}
	dlen = g3d_read_int32_le(f);

	object = g_new0(G3DObject, 1);
	object->name = g_strdup(filename);
	model->objects = g_slist_append(model->objects, object);

	do
	{
		id = g3d_read_int32_be(f);
		if(id != G3D_IFF_MKID('E','D','O','F'))
			len = g3d_read_int32_le(f);
		dlen -= 8;

		switch(id)
		{
			case G3D_IFF_MKID('E','D','O','F'):
				/* end of DOF */
				break;

			case G3D_IFF_MKID('M','A','T','S'):
				nmat = g3d_read_int32_le(f);
				for(i = 0; i < nmat; i ++)
				{
					material = ar_dof_load_mat(context, model, f);
					if(material)
						object->materials = g_slist_append(object->materials,
							material);
				}
				dlen -= len;
				break;

			case G3D_IFF_MKID('G','E','O','B'):
				nobj = g3d_read_int32_le(f);
				for(i = 0; i < nobj; i ++)
				{
					cobj = ar_dof_load_obj(context, model, f);
					if(cobj)
						object->objects =
							g_slist_append(object->objects, cobj);
				}
				dlen -= len;
				break;

			default:
				fseek(f, len, SEEK_CUR);
				dlen -= len;
				g_print("DOF: unknown ID '%c%c%c%c'\n",
					(id >> 24) & 0xFF, (id >> 16) & 0xFF,
					(id >> 8) & 0xFF, id  & 0xFF);
				break;
		}
	}
	while((dlen > 0) && (id != G3D_IFF_MKID('E','D','O','F')) && (!feof(f)));

	return object;
}

