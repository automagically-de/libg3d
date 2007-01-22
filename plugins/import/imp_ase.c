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
#include <locale.h>

#include <g3d/types.h>
#include <g3d/material.h>

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model)
{
	FILE *f;
	gchar line[2048], tmp[128];
	guint32 i, a, b, c, ab, bc, ca, mtlid;
	gfloat x, y, z;
	G3DObject *object = NULL;
	G3DMaterial *material;
	G3DFace *face;

	setlocale(LC_NUMERIC, "C");

	f = fopen(filename, "r");
	if(f == NULL)
	{
		g_warning("E: failed to open '%s'\n", filename);
		return EXIT_FAILURE;
	}

	while(fgets(line, 2048, f))
	{
		g_strstrip(line);
		if(strncmp(line, "*GEOMOBJECT ", 12) == 0)
		{
			object = g_new0(G3DObject, 1);
			object->name = g_strdup("(unnamed object)");

			model->objects = g_slist_append(model->objects, object);
		}
		else if(strncmp(line, "*MATERIAL ", 10) == 0)
		{
			material = g3d_material_new();
			material->name = g_strdup_printf("material %d",
				g_slist_length(model->materials));
			model->materials = g_slist_append(model->materials, material);
		}
		else if(strncmp(line, "*NODE_NAME ", 11) == 0)
		{
			if(object)
			{
				if(object->name)
					g_free(object->name);

				object->name = g_strdup(line + 11);
			}
		}
		else if(strncmp(line, "*MESH_NUMVERTEX ", 16) == 0)
		{
			if(sscanf(line, "*MESH_NUMVERTEX %u", &i) == 1)
			{
				if(object)
				{
					object->vertex_count = i;
					object->vertex_data = g_new0(gfloat, i * 3);
				}
			}
		}
		else if(strncmp(line, "*MESH_VERTEX ", 13) == 0)
		{
			if(sscanf(line, "*MESH_VERTEX %u %f %f %f", &i, &x, &y, &z) == 4)
			{
				if(object && (i < object->vertex_count))
				{
					object->vertex_data[i * 3 + 0] = x;
					object->vertex_data[i * 3 + 1] = y;
					object->vertex_data[i * 3 + 2] = z;
				}
			}
		}
		else if(strncmp(line, "*MESH_FACE ", 11) == 0)
		{
			if(object && (sscanf(line, "*MESH_FACE %u: A: %u B: %u C: %u "
				"AB: %u BC: %u CA: %u *MESH_SMOOTHING %s *MESH_MTLID %u",
				&i, &a, &b, &c, &ab, &bc, &ca, tmp, &mtlid) == 9))
			{
				face = g_new0(G3DFace, 1);
				face->vertex_count = 3;
				face->vertex_indices = g_new0(guint32, 3);
				face->vertex_indices[0] = a;
				face->vertex_indices[1] = b;
				face->vertex_indices[2] = c;
				face->material = g_slist_nth_data(model->materials, mtlid);
				if(face->material == 0)
					face->material = g_slist_nth_data(model->materials, 0);

				object->faces = g_slist_append(object->faces, face);
			}
		}
	}

	fclose(f);

	return TRUE;
}

char *plugin_description(void)
{
	return g_strdup("import plugin for ASCII Scene Exporter (ASE) files\n");
}

char **plugin_extensions(void)
{
	return g_strsplit("ase", ":", 0);
}

