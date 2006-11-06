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
#include <locale.h>

#include <g3d/types.h>
#include <g3d/read.h>
#include <g3d/iff.h>
#include <g3d/material.h>
#include <g3d/texture.h>
#include <g3d/matrix.h>

G3DObject *joe_load_object(G3DContext *context, const gchar *filename,
	G3DModel *model);
GHashTable *joe_load_car(const gchar *filename);
void joe_destroy_car(GHashTable *hashtable);
gboolean joe_parse_vertex(const gchar *text, gfloat *x, gfloat *y, gfloat *z);
void joe_object_flip_x(G3DObject *object);

/*****************************************************************************/
/* plugin interface                                                          */
/*****************************************************************************/

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer plugin_data)
{
	G3DObject *object;
	GHashTable *cardata;
	gchar *value;
	gfloat x, y, z;

	if(g_strcasecmp(filename + strlen(filename) - 3, "car") == 0)
	{
		/* .car file */
		setlocale(LC_NUMERIC, "C");

		cardata = joe_load_car(filename);

		joe_load_object(context, "body.joe", model);
		joe_load_object(context, "interior.joe", model);
		joe_load_object(context, "glass.joe", model);

		/* wheels */
		object = joe_load_object(context, "wheel_front.joe", model);
		value = g_hash_table_lookup(cardata, "wheel-FL.position");
		if(value != NULL)
		{
			joe_parse_vertex(value, &x, &y, &z);
			object->transformation = g_new0(G3DTransformation, 1);
			g3d_matrix_identity(object->transformation->matrix);
			g3d_matrix_translate(y, x, z, object->transformation->matrix);
		}

		object = joe_load_object(context, "wheel_front.joe", model);
		joe_object_flip_x(object);
		value = g_hash_table_lookup(cardata, "wheel-FR.position");
		if(value != NULL)
		{
			joe_parse_vertex(value, &x, &y, &z);
			object->transformation = g_new0(G3DTransformation, 1);
			g3d_matrix_identity(object->transformation->matrix);
			g3d_matrix_translate(y, x, z, object->transformation->matrix);
		}

		object = joe_load_object(context, "wheel_rear.joe", model);
		value = g_hash_table_lookup(cardata, "wheel-RL.position");
		if(value != NULL)
		{
			joe_parse_vertex(value, &x, &y, &z);
			object->transformation = g_new0(G3DTransformation, 1);
			g3d_matrix_identity(object->transformation->matrix);
			g3d_matrix_translate(y, x, z, object->transformation->matrix);
		}

		object = joe_load_object(context, "wheel_rear.joe", model);
		joe_object_flip_x(object);
		value = g_hash_table_lookup(cardata, "wheel-RR.position");
		if(value != NULL)
		{
			joe_parse_vertex(value, &x, &y, &z);
			object->transformation = g_new0(G3DTransformation, 1);
			g3d_matrix_identity(object->transformation->matrix);
			g3d_matrix_translate(y, x, z, object->transformation->matrix);
		}

		joe_destroy_car(cardata);

		return TRUE;
	}
	else
	{
		/* .joe file */
		return (joe_load_object(context, filename, model) != NULL);
	}

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for VDrift .joe files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("joe:car", ":", 0);
}

/*****************************************************************************/

G3DObject *joe_load_object(G3DContext *context, const gchar *filename,
	G3DModel *model)
{
	FILE *f;
	gchar *basename, *texname;
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	G3DImage *image;
	GSList *item;
	guint32 magic, version;
	guint32 num_faces, num_frames, num_verts, num_texcoords, num_normals;
	gint32 frame, i, j, index;
	guint16 *tex_indices, *normal_indices;
	gfloat *normals = NULL, *texcoords = NULL;

	f = fopen(filename, "rb");
	if(f == NULL)
	{
		g_printerr("JOE: failed to read '%s'\n", filename);
		return NULL;
	}

	magic = g3d_read_int32_be(f);
	if(magic != G3D_IFF_MKID('I','D','P','2'))
	{
		g_printerr("JOE: wrong magic in '%s'\n", filename);
		fclose(f);
		return NULL;
	}

	/* base file name for object name & texture loading */
	basename = g_path_get_basename(filename);

	/* version 3 */

	version = g3d_read_int32_le(f);
	num_faces = g3d_read_int32_le(f);
	num_frames = g3d_read_int32_le(f);

	printf("JOE: faces: %d, frames: %d\n", num_faces, num_frames);

	/* create object */
	object = g_new0(G3DObject, 1);
	object->name = g_strdup(basename);
	model->objects = g_slist_append(model->objects, object);

	/* load texture image */
	memcpy(basename + strlen(basename) - 3, "png", 3);
	texname = g_strdup_printf("textures/%s", basename);
	image = g3d_texture_load_cached(context, model, texname);
	if(image == NULL)
		g_warning("JOE: failed to load texture '%s'\n", texname);
	else
		image->tex_id = g_str_hash(texname);
	g_free(texname);

	/* create default material */
	material = g3d_material_new();
	material->name = g_strdup("default material");
	material->tex_image = image;
	object->materials = g_slist_append(object->materials, material);

	/* frames */
	for(frame = 0; frame < 1; frame ++)
	{
		/* create temporary storage */
		tex_indices = g_new0(guint16, num_faces * 3 * 3);
		normal_indices = g_new0(guint16, num_faces * 3 * 2);

		/* faces blob */
		for(i = 0; i < num_faces; i ++)
		{
			face = g_new0(G3DFace, 1);
			face->material = material;
			face->vertex_count = 3;
			face->vertex_indices = g_new0(guint32, 3);

			for(j = 0; j < 3; j ++)
				face->vertex_indices[j] = g3d_read_int16_le(f);

			/* normalIndex */
			for(j = 0; j < 3; j ++)
				normal_indices[i * 3 + j] = g3d_read_int16_le(f);

			/* textureIndex */
			/* JOE_MAX_TEXTURES times, 1x for version 3 */
			for(j = 0; j < 3; j ++)
				tex_indices[i * 3 + j] = g3d_read_int16_le(f);

			object->faces = g_slist_append(object->faces, face);
		}

		/* num_verts */
		num_verts = g3d_read_int32_le(f);

		/* num_texcoords */
		num_texcoords = g3d_read_int32_le(f);
		if(num_texcoords != 0)
			texcoords = g_new0(gfloat, num_texcoords * 2);

		/* num_normals */
		num_normals = g3d_read_int32_le(f);
		if(num_normals != 0)
			normals = g_new0(gfloat, num_normals * 3);

		printf("JOE: verts: %d, texcoords: %d, normals: %d\n",
			num_verts, num_texcoords, num_normals);

		/* verts blob */
		object->vertex_count = num_verts;
		object->vertex_data = g_new0(gfloat, num_verts * 3);
		for(i = 0; i < num_verts; i ++)
		{
			object->vertex_data[i * 3 + 0] = g3d_read_float_le(f);
			object->vertex_data[i * 3 + 1] = g3d_read_float_le(f);
			object->vertex_data[i * 3 + 2] = g3d_read_float_le(f);
		}

		/* normals blob */
		for(i = 0; i < num_normals; i ++)
		{
			normals[i * 3 + 0] = - g3d_read_float_le(f);
			normals[i * 3 + 1] = - g3d_read_float_le(f);
			normals[i * 3 + 2] = - g3d_read_float_le(f);
		}

		/* texcoords blob */
		for(i = 0; i < num_texcoords; i ++)
		{
			texcoords[i * 2 + 0] = g3d_read_float_le(f);
			texcoords[i * 2 + 1] = g3d_read_float_le(f);
		}

		/* fix faces */
		for(item = object->faces, i = 0; item != NULL; item = item->next, i ++)
		{
			face = (G3DFace *)item->data;

			face->flags |= G3D_FLAG_FAC_NORMALS;
			if(image != NULL) face->flags |= G3D_FLAG_FAC_TEXMAP;

			face->normals = g_new0(gfloat, 3 * 3);
			face->tex_image = image;
			face->tex_vertex_count = 3;
			face->tex_vertex_data = g_new0(gfloat, 3 * 2);
			for(j = 0; j < 3; j ++)
			{
				index = normal_indices[i * 3 + j];
				face->normals[j * 3 + 0] = normals[index * 3 + 0];
				face->normals[j * 3 + 1] = normals[index * 3 + 1];
				face->normals[j * 3 + 2] = normals[index * 3 + 2];

				index = tex_indices[i * 3 + j];
				face->tex_vertex_data[j * 2 + 0] = texcoords[index * 2 + 0];
				face->tex_vertex_data[j * 2 + 1] = texcoords[index * 2 + 1];
			}
		}

		/* clear temporary storage */
		g_free(normal_indices);
		g_free(tex_indices);

		if(normals != NULL)
		{
			g_free(normals);
			normals = NULL;
		}

		if(texcoords != NULL)
		{
			g_free(texcoords);
			texcoords = NULL;
		}
	}

	/* clean up */
	g_free(basename);
	fclose(f);

	return object;
}

GHashTable *joe_load_car(const gchar *filename)
{
	FILE *f;
	GHashTable *ht;
	gchar buffer[2048 + 1], section[256], varname[256], value[256];
	gchar *ep;

	f = fopen(filename, "r");
	if(f == NULL)
	{
		g_printerr("JOE: failed to read '%s'\n", filename);
		return NULL;
	}

	ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	memset(section, 0, 256);

	while(!feof(f))
	{
		fgets(buffer, 2048, f);
		if((buffer[0] == '\0') || (buffer[0] == '\n'))
			continue;

		if(buffer[0] == '[')
		{
			/* section title */
			if(sscanf(buffer, "[ %s ]", section) != 1)
			{
				g_warning("JOE: CAR: failed to read section title '%s'\n",
					buffer);
			}
		}
		else
		{
			/* property */
			ep = strchr(buffer, '=');
			if(ep != NULL)
			{
				memset(varname, 0, 256);
				strncpy(varname, buffer, (ep - buffer));
				g_strstrip(varname);

				strcpy(value, ep + 1);
				g_strstrip(value);
#if DEBUG > 0
				g_print("JOE: %s.%s = %s\n", section, varname, value);
#endif
				g_hash_table_insert(ht,
					g_strdup_printf("%s.%s", section, varname),
					g_strdup(value));
			}
		}
	}

	return ht;
}

void joe_destroy_car(GHashTable *hashtable)
{
	g_hash_table_destroy(hashtable);
}

gboolean joe_parse_vertex(const gchar *text, gfloat *x, gfloat *y, gfloat *z)
{
	return (sscanf(text, "%f, %f, %f", x, y, z) == 3);
}

void joe_object_flip_x(G3DObject *object)
{
	guint32 i;

	for(i = 0; i < object->vertex_count; i ++)
		object->vertex_data[i * 3 + 0] *= -1;
}
