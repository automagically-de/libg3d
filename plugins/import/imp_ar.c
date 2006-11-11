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
#include <stdarg.h>
#include <locale.h>

#include <g3d/types.h>
#include <g3d/read.h>
#include <g3d/material.h>
#include <g3d/matrix.h>

#include "imp_ar.h"
#include "imp_ar_decompress.h"
#include "imp_ar_dof.h"
#include "imp_ar_carini.h"

GSList *ar_read_directory(FILE *f);

/*****************************************************************************/
/* plugin interface                                                          */

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer plugin_data)
{
	FILE *f;
	GSList *dir, *item;
	GHashTable *carini;
	G3DMaterial *material;
	G3DObject *object;
	gchar *mname;
	gfloat x, y, z;

	setlocale(LC_NUMERIC, "C");

	/* default material */
	material = g3d_material_new();
	material->name = g_strdup("default material");
	model->materials = g_slist_append(model->materials, material);

	if(g_strcasecmp(filename + (strlen(filename) - 4), ".dof") == 0)
	{
		/* single DOF file */
		ar_dof_load(context, model, filename);
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

		carini = ar_carini_load();

		/* load directory */
		dir = ar_read_directory(f);

		/* decompress files */
		for(item = dir; item != NULL; item = item->next)
			ar_decompress_to_file(f, (ArDirEntry *)item->data);

		/* load body */
		mname = g_hash_table_lookup(carini, "body.model.file");
		if(mname != NULL)
			ar_dof_load(context, model, mname);

		/* steering wheel */
		mname = g_hash_table_lookup(carini, "steer.model.file");
		if(mname != NULL)
		{
			printf("D: steering wheel (%s)\n", mname);
			object = ar_dof_load(context, model, mname);
			ar_carini_get_position(carini, "steer", &x, &y, &z);
			object->transformation = g_new0(G3DTransformation, 1);
			g3d_matrix_identity(object->transformation->matrix);
			g3d_matrix_translate(x, y, z, object->transformation->matrix);
		}

		/* load wheel 0 */
		mname = g_hash_table_lookup(carini, "wheel0~wheel_front.model.file");
		if(mname != NULL)
		{
			printf("D: loading wheel 0 (%s)\n", mname);
			object = ar_dof_load(context, model, mname);
			x = ar_carini_get_float(carini, "susp0~susp_front.x");
			y = ar_carini_get_float(carini, "susp_front.y") -
				ar_carini_get_float(carini, "wheel_front.radius");
			z = ar_carini_get_float(carini, "susp_front.z");
			object->transformation = g_new0(G3DTransformation, 1);
			g3d_matrix_identity(object->transformation->matrix);
			g3d_matrix_translate(x, y, z, object->transformation->matrix);
		}

		ar_carini_free(carini);

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


