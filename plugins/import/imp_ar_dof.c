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

#include <g3d/types.h>
#include <g3d/read.h>
#include <g3d/texture.h>
#include <g3d/material.h>
#include <g3d/iff.h>

gchar *ar_dof_read_string(FILE *f, gint32 *dlen)
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
	gint32 id, len, dlen, i, ntex, trans = 0, blend = 0;
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
				material->name = ar_dof_read_string(f, &dlen);
				tmp = ar_dof_read_string(f, &dlen);
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
					tmp = ar_dof_read_string(f, &dlen);
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

			case G3D_IFF_MKID('M','T','R','A'):
				/* transparency */
				trans = g3d_read_int32_le(f);
				/* blend mode */
				blend = g3d_read_int32_le(f);

				printf("D: MTRA: %s: trans: 0x%04x, blend: 0x%04x\n",
					(material->name ? material->name : "unnamed"),
					trans, blend);

				dlen -= 8;
				break;

			case G3D_IFF_MKID('M','C','F','L'):
				/* creation flags */
				g3d_read_int32_le(f);
				dlen -= 4;
				break;

			case G3D_IFF_MKID('M','U','V','W'):
				/* u offset */
				g3d_read_int32_le(f);
				/* v offset */
				g3d_read_int32_le(f);
				dlen -= 8;

				/* u tiling */
				g3d_read_int32_le(f);
				/* v tiling */
				g3d_read_int32_le(f);
				dlen -= 8;

				/* angle */
				g3d_read_float_le(f);
				/* blur */
				g3d_read_float_le(f);
				/* blur offset */
				g3d_read_int32_le(f);
				dlen -= 12;
				break;

			default:
				fseek(f, len, SEEK_CUR);
				dlen -= len;
				break;
		}
	}
	while((dlen > 0) && (id != G3D_IFF_MKID('M','E','N','D')));

	if(material->tex_image != NULL)
	{
		if(blend == 1)
			material->tex_image->tex_env = G3D_TEXENV_BLEND;
		else
			material->tex_image->tex_env = G3D_TEXENV_DECAL;
	}

	return material;
}

G3DObject *ar_dof_load_obj(G3DContext *context, G3DModel *model, FILE *f)
{
	G3DObject *object, *pobj;
	G3DFace *face;
	G3DMaterial *material;
	GSList *item;
	gint32 id, len, dlen, nverts, ntver, nnorm, nind, i, j, index;
	gfloat *tex_vertices = NULL, *normals = NULL;

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
				/* end of object */
				break;

			case G3D_IFF_MKID('G','H','D','R'):
				/* object header */
				/* flags */
				i = g3d_read_int32_le(f);
				printf("D: GHDR: flags = 0x%04X\n", i);
				/* paint flags */
				i = g3d_read_int32_le(f);
				printf("D: GHDR: paint flags = 0x%04X\n", i);

				/* material ref */
				i = g3d_read_int32_le(f);
				material = g_slist_nth_data(pobj->materials, i);
				if(material == NULL)
					material = (G3DMaterial *)g_slist_nth_data(
						model->materials, 0);

				dlen -= 12;
				break;

			case G3D_IFF_MKID('V','E','R','T'):
				/* vertices */
				nverts = g3d_read_int32_le(f);

#if DEBUG > 2
				printf("D: %d vertices\n", nverts);
#endif

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

			case G3D_IFF_MKID('N', 'O','R','M'):
				/* normals */
				nnorm = g3d_read_int32_le(f);
				normals = g_new0(gfloat, nnorm * 3);
				dlen -= 4;
				for(i = 0; i < nnorm; i ++)
				{
					normals[i * 3 + 0] = g3d_read_float_le(f);
					normals[i * 3 + 1] = g3d_read_float_le(f);
					normals[i * 3 + 2] = g3d_read_float_le(f);
					dlen -= 12;
				}
				break;

			case G3D_IFF_MKID('T', 'V','E','R'):
				/* texture vertices */
				ntver = g3d_read_int32_le(f);
				tex_vertices = g_new0(gfloat, ntver * 2);
				dlen -= 4;

#if DEBUG > 2
				printf("D: %d texture vertices @ 0x%08x\n", ntver,
					(guint32)ftell(f) - 12);
#endif

				for(i = 0; (i < ntver) && (len > 0); i ++)
				{
					tex_vertices[i * 2 + 0] = g3d_read_float_le(f);
					tex_vertices[i * 2 + 1] = 1.0 - g3d_read_float_le(f);
					dlen -= 8;
				}
				break;

			case G3D_IFF_MKID('B','R','S','T'):
				/* bursts */
				i = g3d_read_int32_le(f);
				dlen -= 4;
				fseek(f, i * 4, SEEK_CUR); /* burstStart */
				fseek(f, i * 4, SEEK_CUR); /* burstCount */
				fseek(f, i * 4, SEEK_CUR); /* burstMtlID */
				fseek(f, i * 4, SEEK_CUR); /* burstVperP */
				dlen -= (4 * 4 * i);
				break;

			case G3D_IFF_MKID('V','C','O','L'):
				/* vertex colors */
				i = g3d_read_int32_le(f);
				dlen -= 4;
				fseek(f, i * 4 * 3, SEEK_CUR);
				dlen -= (i * 4 * 3);
				break;

			case G3D_IFF_MKID('I','N','D','I'):
				/* indices */
				nind = g3d_read_int32_le(f);
				dlen -= 4;
				len -= 4;

#if DEBUG > 2
				printf("D: %d indices in %d bytes\n", nind, len);
#endif

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
#if DEBUG > 0
				printf("D: skipping tag '%c%c%c%c @ 0x%08x'\n",
					(id << 24) & 0xFF, (id << 16) & 0xFF,
					(id << 8) & 0xFF, id & 0xFF,
					(guint32)ftell(f));
#endif
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

		if(normals != NULL)
		{
			face->normals = g_new0(gfloat, 3 * 3);
			for(j = 0; j < 3; j ++)
			{
				index = face->vertex_indices[j];
				face->normals[j * 3 + 0] = normals[index * 3 + 0];
				face->normals[j * 3 + 1] = normals[index * 3 + 1];
				face->normals[j * 3 + 2] = normals[index * 3 + 2];
			}
			face->flags |= G3D_FLAG_FAC_NORMALS;
		}
	}

	/* cleanup */
	if(tex_vertices != NULL)
		g_free(tex_vertices);

	if(normals != NULL)
		g_free(normals);

	return object;
}

G3DObject *ar_dof_load(G3DContext *context, G3DModel *model,
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
				g_print("DOF: unknown ID '%c%c%c%c' @ 0x%08x\n",
					(id >> 24) & 0xFF, (id >> 16) & 0xFF,
					(id >> 8) & 0xFF, id  & 0xFF,
					(guint32)ftell(f) - 8);
				fseek(f, len, SEEK_CUR);
				dlen -= len;
				break;
		}
	}
	while((dlen > 0) && (id != G3D_IFF_MKID('E','D','O','F')) && (!feof(f)));

	return object;
}

