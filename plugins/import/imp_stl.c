/*
 *  imp_stl.c: plugin to load STL-Files (ASCII-Format)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 *  (C) 2006 Oliver Dippel <o.dippel@gmx.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <g3d/types.h>
#include <g3d/plugins.h>
#include <g3d/material.h>

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer user_data)
{
	FILE *f;
	gfloat x;
	gfloat y;
	gfloat z;
	gchar line[1024];
	guint32 index;
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	if(context != NULL) {
		if((f = fopen(filename, "r")) == NULL) {
			g_warning("STL: failed to open '%s'", filename);
			return FALSE;
		}
		object = g_new0(G3DObject, 1);
		object->name = g_strdup("STL-Model");
		model->objects = g_slist_append(model->objects, object);
		material = g3d_material_new();
		material->name = g_strdup("default material");
		material->r = 0.2;
		material->g = 1.0;
		material->b = 0.1;
		material->a = 1.0;
		object->materials = g_slist_append(object->materials, material);
		object->vertex_count = 0;
		while(!feof(f)) {
			line[0] = 0;
			fgets(line, 1023, f);
			if (strstr(line, "vertex")) {
				object->vertex_count++;
			}
		}
		fseek(f, 0, SEEK_SET);
		object->vertex_data = g_new0(gfloat, object->vertex_count * 3);
		index = 0;
		while(!feof(f)) {
			line[0] = 0;
			fgets(line, 1023, f);
			if (strstr(line, "vertex")) {
				sscanf (strstr(line, "vertex") + 6, "%f %f %f", &x, &y, &z);
				object->vertex_data[index * 3 + 0] = x;
				object->vertex_data[index * 3 + 1] = y;
				object->vertex_data[index * 3 + 2] = z;
				index++;
			} else if (strstr(line, "facet ")) {
				face = g_new0(G3DFace, 1);
				face->material = material;
				face->vertex_count = 3;
				face->vertex_indices = g_new0(guint32, face->vertex_count);
				face->vertex_indices[0] = index + 0;
				face->vertex_indices[1] = index + 1;
				face->vertex_indices[2] = index + 2;
				object->faces = g_slist_prepend(object->faces, face);
			}
		}
		fclose(f);
	}
	return TRUE;
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup(
		"Import plugin .stl files\n"
		"Author: Oliver Dippel\n");
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("stl:stla", ":", 0);
}
