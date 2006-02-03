/* $Id: model.c,v 1.1.2.4 2006/01/23 16:38:47 dahms Exp $ */

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
#include <g3d/context.h>
#include <g3d/model.h>
#include <g3d/object.h>
#include <g3d/material.h>
#include <g3d/plugins.h>

G3DModel *g3d_model_new(void)
{
	G3DModel *model;

	model = g_new0(G3DModel, 1);

	return model;
}

G3DModel *g3d_model_load(G3DContext *context, const gchar *filename)
{
	G3DModel *model;
	G3DObject *object;
	GSList *oitem;
	gdouble radius, max_rad = 0.0;

	model = g3d_model_new();

	g3d_context_update_progress_bar(context, 0.0, TRUE);

	if(g3d_plugins_load_model(context, filename, model))
	{
		g3d_context_update_progress_bar(context, 0.0, FALSE);

		/* check model */
		if(!g3d_model_check(model))
		{
			g3d_model_free(model);
			return NULL;
		}

		/* center model */
		g3d_model_center(model);

		/* get maximum radius of all objects */
		oitem = model->objects;
		while(oitem)
		{
			object = (G3DObject *)oitem->data;
			radius = g3d_object_radius(object);
			if(radius > max_rad)
				max_rad = radius;

			oitem = oitem->next;
		}

		/* scale and optimize objects */
		oitem = model->objects;
		while(oitem)
		{
			object = (G3DObject *)oitem->data;

			g3d_object_scale(object, (10.0 / max_rad));
			g3d_object_optimize(object);

			oitem = oitem->next;
		}

		return model;
	}
	else
	{
		g3d_context_update_progress_bar(context, 0.0, FALSE);
		g3d_model_free(model);
	}

	return NULL;
}

gboolean g3d_model_check(G3DModel *model)
{
	G3DFace *face;
	G3DObject *object;
	GSList *fitem, *oitem;
	guint32 i, no = 0, nf;

	oitem = model->objects;
	while(oitem)
	{
		object = (G3DObject *)oitem->data;

		nf = 0;
		fitem = object->faces;
		while(fitem)
		{
			face = (G3DFace *)fitem->data;

			if(face->material == NULL)
			{
				g_warning("g3d_object_check: face->material is NULL"
					" (o: %d, f: %d)", no, nf);
				return FALSE;
			}

			if(face->vertex_count < 3)
			{
				g_warning("g3d_object_check: face->num_vertices < 3 (%d)"
					" (o: %d, f: %d)", face->vertex_count, no, nf);
				return FALSE;
			}

			for(i = 0; i < face->vertex_count; i ++)
			{
				if(face->vertex_indices[i] >= object->vertex_count)
				{
					g_warning("g3d_object_check: "
						"vertex_indices[%d] >= vertex_count (%d >= %d)"
						" (o: %d, f: %d)",
						i, face->vertex_indices[i], object->vertex_count,
						no, nf);
					return FALSE;
				}
			}

			nf ++;
			fitem = fitem->next;
		} /* while(fitem) */
		oitem = oitem->next;
	} /* while(oitem) */
	return TRUE;
}

gboolean g3d_model_center(G3DModel *model)
{
	gdouble min_x = 10.0e99, min_y = 10.0e99, min_z = 10.0e99;
	gdouble max_x = -9.9e99, max_y = -9.9e99, max_z = -9.9e99;
	gdouble off_x, off_y, off_z;
	GSList *oitem;
	G3DObject *object;
	guint32 i;

	/* determine maximum extension */
	oitem = model->objects;
	while(oitem)
	{
		object = (G3DObject *)oitem->data;
		for(i = 0; i < object->vertex_count; i ++)
		{
			if(object->vertex_data[i * 3 + 0] < min_x)
				min_x = object->vertex_data[i * 3 + 0];
			if(object->vertex_data[i * 3 + 1] < min_y)
				min_y = object->vertex_data[i * 3 + 1];
			if(object->vertex_data[i * 3 + 2] < min_z)
				min_z = object->vertex_data[i * 3 + 2];

			if(object->vertex_data[i * 3 + 0] > max_x)
				max_x = object->vertex_data[i * 3 + 0];
			if(object->vertex_data[i * 3 + 1] > max_y)
				max_y = object->vertex_data[i * 3 + 1];
			if(object->vertex_data[i * 3 + 2] > max_z)
				max_z = object->vertex_data[i * 3 + 2];
		}
		oitem = oitem->next;
	}

	/* move model */
	off_x = max_x - ((max_x - min_x) / 2.0);
	off_y = max_y - ((max_y - min_y) / 2.0);
	off_z = max_z - ((max_z - min_z) / 2.0);

	oitem = model->objects;
	while(oitem)
	{
		object = (G3DObject *)oitem->data;
		for(i = 0; i < object->vertex_count; i ++)
		{
			object->vertex_data[i * 3 + 0] -= off_x;
			object->vertex_data[i * 3 + 1] -= off_y;
			object->vertex_data[i * 3 + 2] -= off_z;
		}
		oitem = oitem->next;
	}

	return TRUE;
}

static gboolean remove_teximg(gpointer key, gpointer value, gpointer data)
{
	G3DImage *image;

	image = (G3DImage *)value;
	if(image->name)
		g_free(image->name);
	if(image->pixeldata)
		g_free(image->pixeldata);
	g_free(image);

	g_free(key);

	return TRUE;
}

void g3d_model_clear(G3DModel *model)
{
	GSList *list, *next;
	G3DObject *object;
	G3DMaterial *mat;

	/* lights */
	/* TODO */

	/* objects */
	list = model->objects;
	while(list)
	{
		object = (G3DObject*)list->data;
		g3d_object_free(object);
		next = list->next;
		g_slist_free_1(list);
		list = next;
	}
	model->objects = NULL;

	/* materials */
	list = model->materials;
	while(list)
	{
		mat = (G3DMaterial*)list->data;
		g3d_material_free(mat);
		next = list->next;
		g_slist_free_1(list);
		list = next;
	}
	model->materials = NULL;

	if(model->tex_images)
	{
		g_hash_table_foreach_remove(model->tex_images, remove_teximg, NULL);
	}
}

void g3d_model_free(G3DModel *model)
{
	g3d_model_clear(model);
	g_free(model);
}


