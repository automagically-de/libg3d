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

#include <g3d/types.h>
#include <g3d/context.h>
#include <g3d/material.h>
#include <g3d/primitive.h>

/*****************************************************************************/
/* plugin interface                                                          */
/*****************************************************************************/

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer user_data)
{
	G3DObject *sphere;
	G3DMaterial *material;

	material = g3d_material_new();
	model->materials = g_slist_append(model->materials, material);

	sphere = g3d_primitive_sphere(1.0, 32, 32, material);
	model->objects = g_slist_append(model->objects, sphere);

	return TRUE;
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup(
		"Test plugin\n"
		);
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("test", ":", 0);
}

