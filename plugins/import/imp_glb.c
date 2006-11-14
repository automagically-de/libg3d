/* $Id:$ */

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
#include <stdlib.h>
#include <string.h>

#include <g3d/types.h>
#include <g3d/read.h>
#include <g3d/material.h>
#include <g3d/iff.h>

G3DObject *glb_load_object(G3DContext *context, const gchar *filename,
	G3DModel *model);

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer user_data)
{
	return (glb_load_object(context, filename, model) != NULL);
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup("import plugin for UltimateStunts GLB models\n");
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("glb", ":", 0);
}

/*****************************************************************************/
/* GLB specific                                                              */

gfloat glb_get_float(FILE *f)
{
	return 0.001 * (
		(float)((guint32)g3d_read_int32_be(f)) - (float)(0x7FFFFFFF));
}

G3DObject *glb_load_object(G3DContext *context, const gchar *filename,
	G3DModel *model)
{
	G3DObject *pobject, *object;
	G3DMaterial *material;
	G3DFace *face;
	FILE *f;
	guint32 magic, otype;
	gint32 i, msize, namelen, datalen, nvertices, nindices;
	gchar *name;

	f = fopen(filename, "rb");
	if(f == NULL)
	{
		g_warning("failed to read '%s'\n", filename);
		return NULL;
	}

	magic = g3d_read_int32_be(f);
	if(magic != G3D_IFF_MKID('\0', 'G', 'L', 'B'))
	{
		g_warning("%s is not a correct GLB file (wrong magic)\n",
			filename);
		fclose(f);
		return NULL;
	}

	pobject = g_new0(G3DObject, 1);
	pobject->name = g_strdup(filename);
	model->objects = g_slist_append(model->objects, pobject);

	while(!feof(f))
	{
		otype = g3d_read_int32_be(f);
		namelen = g3d_read_int32_be(f);

		if(namelen == 0)
			break;

		name = g_new0(gchar, namelen + 1);
		fread(name, 1, namelen, f);
#if DEBUG > 0
		printf("GLB: object named '%s'\n", name);
#endif

		object = g_new0(G3DObject, 1);
		object->name = g_strdup(name);
		g_free(name);
		pobject->objects = g_slist_append(pobject->objects, object);

		/* hide collision planes by default */
		if(strncmp(object->name, "Collision plane", 15) == 0)
			object->hide = TRUE;

		datalen = g3d_read_int32_be(f);

		if(otype != 0)
		{
			/* skip */
			fseek(f, datalen, SEEK_CUR);
			continue;
		}

		/* object type 0 */
		msize = g3d_read_int32_be(f);
		nvertices = g3d_read_int32_be(f);
		nindices = g3d_read_int32_be(f);

#if DEBUG > 0
		printf("GLB: material size: %d bytes, %d vertices, %d indices\n",
			msize, nvertices, nindices);
#endif
		if(msize > 0)
		{
			/* material */
			material = g3d_material_new();
			material->name = g_strdup("default material");
			object->materials = g_slist_append(object->materials, material);

			material->r = (gfloat)g3d_read_int8(f) / 255.0;
			material->g = (gfloat)g3d_read_int8(f) / 255.0;
			material->b = (gfloat)g3d_read_int8(f) / 255.0;
			material->a = (gfloat)g3d_read_int8(f) / 255.0;

			if(material->a == 0.0)
				material->a = 1.0;

			/* replacement color */
			g3d_read_int8(f);
			g3d_read_int8(f);
			g3d_read_int8(f);
			g3d_read_int8(f); /* unused */

			g3d_read_int8(f); /* LODs */
			g3d_read_int8(f); /* reflectance */
			g3d_read_int8(f); /* emissivity */
			g3d_read_int8(f); /* static friction */
			g3d_read_int8(f); /* dynamic friction */
			g3d_read_int8(f); /* unused */
			g3d_read_int8(f); /* unused */
			g3d_read_int8(f); /* unused */

			/* texture name */
			namelen = msize - 16;
			if(namelen > 0)
			{
				name = g_new0(gchar, namelen + 1);
				fread(name, 1, namelen, f);
				/* TODO: texture stuff */
#if DEBUG > 0
				printf("GLB: texture name: %s\n", name);
#endif

				g_free(name);
			}
		}

		/* vertices */
		if(nvertices > 0)
		{
			object->vertex_count = nvertices;
			object->vertex_data = g_new0(gfloat, nvertices * 3);

			for(i = 0; i < nvertices; i ++)
			{
				object->vertex_data[i * 3 + 0] = glb_get_float(f);
				object->vertex_data[i * 3 + 1] = glb_get_float(f);
				object->vertex_data[i * 3 + 2] = glb_get_float(f);

#if DEBUG > 3
				printf("D: %f, %f, %f\n",
					object->vertex_data[i * 3 + 0],
					object->vertex_data[i * 3 + 1],
					object->vertex_data[i * 3 + 2]);
#endif

				/* normal */
				g3d_read_float_be(f);
				g3d_read_float_be(f);
				g3d_read_float_be(f);

				/* texture coordinates */
				g3d_read_float_be(f);
				g3d_read_float_be(f);
			}
		}

		if(nindices > 0)
		{
			for(i = 0; i < nindices; i += 3)
			{
				face = g_new0(G3DFace, 1);
				face->vertex_count = 3;
				face->vertex_indices = g_new0(guint32, 3);
				face->vertex_indices[0] = g3d_read_int32_be(f);
				face->vertex_indices[1] = g3d_read_int32_be(f);
				face->vertex_indices[2] = g3d_read_int32_be(f);
				face->material = g_slist_nth_data(object->materials, 0);
				object->faces = g_slist_append(object->faces, face);
			}
		}
	}

	fclose(f);

	return pobject;
}
