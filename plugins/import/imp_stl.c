/*
    libg3d - 3D object loading library

    Copyright (C) 2006  Oliver Dippel <o.dippel@gmx.de>

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
#include <g3d/plugins.h>
#include <g3d/material.h>

/*
 * Infos for the STL(A)-Format:
 *  http://www.csit.fsu.edu/~burkardt/data/stla/stla.html
 * Infos for the STL(B)-Format:
 *  http://www.csit.fsu.edu/~burkardt/data/stlb/stlb.html
 */

#define STL_ASCII  0
#define STL_BINARY 1

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer user_data)
{
	FILE *f;
	gfloat x;
	gfloat y;
	gfloat z;
	gchar line[1024];
	guint32 index = 0;
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	guint32 n = 0;
	guint32 num_faces = 0;
	gchar name[81];
	gchar type;
	if(context != NULL) {
		/* Check Filetype (ASCII or BINARY) */
		if((f = fopen(filename, "r")) == NULL) {
			g_warning("STL: failed to open '%s'", filename);
			return FALSE;
		}
		type = STL_BINARY;
		while(!feof(f)) {
			fgets(line, 1023, f);
			if (strstr(line, "solid")) {
				type = STL_ASCII;
				break;
			}
		}
		fclose(f);
		if (type == STL_BINARY) {
			if((f = fopen(filename, "rb")) == NULL) {
				g_warning("STLb: failed to open '%s'", filename);
				return FALSE;
			}
			fread(name, 80, sizeof(gchar), f);
			name[80] = 0;
			fread(&num_faces, 1, sizeof(guint32), f);
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
			object->vertex_count = num_faces * 3;
			object->vertex_data = g_new0(gfloat, object->vertex_count * 3);
			for (n = 0; n < num_faces; n++) {
				face = g_new0(G3DFace, 1);
				face->material = material;
				face->vertex_count = 3;
				face->vertex_indices = g_new0(guint32, face->vertex_count);
				face->vertex_indices[0] = index + 0;
				face->vertex_indices[1] = index + 1;
				face->vertex_indices[2] = index + 2;
				object->faces = g_slist_prepend(object->faces, face);
				/* Normals */
				fread(&x, 1, sizeof(gfloat), f);
				fread(&y, 1, sizeof(gfloat), f);
				fread(&z, 1, sizeof(gfloat), f);
				/* 1. Polygon */
				fread(&x, 1, sizeof(gfloat), f);
				fread(&y, 1, sizeof(gfloat), f);
				fread(&z, 1, sizeof(gfloat), f);
				object->vertex_data[index * 3 + 0] = x;
				object->vertex_data[index * 3 + 1] = y;
				object->vertex_data[index * 3 + 2] = z;
				index++;
				/* 2. Polygon */
				fread(&x, 1, sizeof(gfloat), f);
				fread(&y, 1, sizeof(gfloat), f);
				fread(&z, 1, sizeof(gfloat), f);
				object->vertex_data[index * 3 + 0] = x;
				object->vertex_data[index * 3 + 1] = y;
				object->vertex_data[index * 3 + 2] = z;
				index++;
				/* 3. Polygon */
				fread(&x, 1, sizeof(gfloat), f);
				fread(&y, 1, sizeof(gfloat), f);
				fread(&z, 1, sizeof(gfloat), f);
				object->vertex_data[index * 3 + 0] = x;
				object->vertex_data[index * 3 + 1] = y;
				object->vertex_data[index * 3 + 2] = z;
				index++;
				/* 2 Byte Dummy read */
				fread(name, 2, sizeof(gchar), f);
			}
			fclose(f);
		} else {
			if((f = fopen(filename, "r")) == NULL) {
				g_warning("STLa: failed to open '%s'", filename);
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
				if (strstr(line, "solid ")) {
					free(object->name);
					object->name = g_strdup(strstr(line, "solid ") + 6);
				} else if (strstr(line, "vertex")) {
					object->vertex_count++;
				}
			}
			fseek(f, 0, SEEK_SET);
			object->vertex_data = g_new0(gfloat, object->vertex_count * 3);
			index = 0;
			while(!feof(f)) {
				line[0] = 0;
				fgets(line, 1023, f);
				if (strstr(line, "facet ")) {
					face = g_new0(G3DFace, 1);
					face->material = material;
					face->vertex_count = 3;
					face->vertex_indices = g_new0(guint32, face->vertex_count);
					face->vertex_indices[0] = index + 0;
					face->vertex_indices[1] = index + 1;
					face->vertex_indices[2] = index + 2;
					object->faces = g_slist_prepend(object->faces, face);
				} else if (strstr(line, "vertex")) {
					sscanf (strstr(line, "vertex") + 6, "%f %f %f", &x, &y, &z);
					object->vertex_data[index * 3 + 0] = x;
					object->vertex_data[index * 3 + 1] = y;
					object->vertex_data[index * 3 + 2] = z;
					index++;
				}
			}
			fclose(f);
		}
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
	return g_strsplit("stl:stla:stlb", ":", 0);
}
