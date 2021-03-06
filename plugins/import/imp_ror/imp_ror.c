/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2009  Markus Dahms <mad@automagically.de>

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
#include <locale.h>
#include <math.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/object.h>
#include <g3d/face.h>
#include <g3d/vector.h>
#include <g3d/matrix.h>
#include <g3d/material.h>
#include <g3d/primitive.h>
#include <g3d/texture.h>

#include "imp_ror_material.h"

#define ROR_LL 2048 /* line buffer */

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	G3DObject *object;
	gchar *buffer;

	G3DMaterial *texmat;
	GHashTable *matfile;

	G3DObject *o_joints;
	G3DObject *o_beams;

	G3DObject *submesh_object;
	G3DFloat *submesh_texcoords;
	guint32 submesh_ncoords;
} RorGlobalData;

typedef gboolean (* RorSectionCallback)(RorGlobalData *);

typedef struct {
	const gchar *section;
	RorSectionCallback callback;
} RorSectionMap;

gboolean ror_beams_cb(RorGlobalData *global);
gboolean ror_cab_cb(RorGlobalData *global);
gboolean ror_globals_cb(RorGlobalData *global);
gboolean ror_managedmaterials_cb(RorGlobalData *global);
gboolean ror_nodes_cb(RorGlobalData *global);
gboolean ror_texcoords_cb(RorGlobalData *global);

static const RorSectionMap ror_section_map[] = {
	{ "beams",                 ror_beams_cb },
	{ "brakes",                NULL },
	{ "cab",                   ror_cab_cb },
	{ "cameras",               NULL },
	{ "commands",              NULL },
	{ "engine",                NULL },
	{ "engoption",             NULL },
	{ "globals",               ror_globals_cb },
	{ "hydros",                NULL },
	{ "managedmaterials",      ror_managedmaterials_cb },
	{ "meshwheels",            NULL },
	{ "nodes",                 ror_nodes_cb },
	{ "rotators",              NULL },
	{ "shocks",                NULL },
	{ "submesh",               NULL },
	{ "texcoords",             ror_texcoords_cb },
	{ NULL, NULL }
};

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar *buffer;
	gint i;
	GHashTable *sections;
	RorSectionMap *currentSection = NULL;
	RorGlobalData *global;
	gchar *filename, *uri, *dotp;

	global = g_new0(RorGlobalData, 1);
	global->context = context;
	global->stream = stream;
	global->model = model;

	setlocale(LC_NUMERIC, "C");

	buffer = g_new0(gchar, ROR_LL);
	global->buffer = buffer;

	/* container object */
	global->object = g_new0(G3DObject, 1);
	model->objects = g_slist_append(model->objects, global->object);

	global->o_beams = g_new0(G3DObject, 1);
	global->o_beams->name = g_strdup("beams");
	global->object->objects = g_slist_append(global->object->objects,
		global->o_beams);

	global->o_joints = g_new0(G3DObject, 1);
	global->o_joints->name = g_strdup("joints");
	global->object->objects = g_slist_append(global->object->objects,
		global->o_joints);

	/* material */
	global->texmat = g3d_material_new();
	global->texmat->name = g_strdup("texture material");
	global->texmat->g = 1.0;
	global->object->materials = g_slist_append(global->object->materials,
		global->texmat);
	uri = g3d_stream_get_uri(stream);
	dotp = strrchr(uri, '.');
	if(dotp) {
		filename = g_strdup_printf("%.*s.png", (int)(dotp - uri), uri);
		global->texmat->tex_image = g3d_texture_load_cached(context, model,
			filename);
		if(global->texmat->tex_image) {
			global->texmat->tex_image->tex_env = G3D_TEXENV_REPLACE;
		}
	}

	/* title */
	g3d_stream_read_line(stream, buffer, ROR_LL);
	g_strstrip(buffer);
	global->object->name = g_strdup(buffer);

	sections = g_hash_table_new(g_str_hash, g_str_equal);
	for(i = 0; ror_section_map[i].section; i ++)
		g_hash_table_insert(sections,
			(gchar *)ror_section_map[i].section,
			(gpointer)&(ror_section_map[i]));

	while(g3d_stream_read_line(stream, buffer, ROR_LL)) {
		g_strstrip(buffer);
		if(!strlen(buffer))
			continue;
		if(buffer[0] == ';')
			continue;

		if(g_hash_table_lookup_extended(sections, buffer, NULL, NULL)) {
			currentSection = g_hash_table_lookup(sections, buffer);
#if DEBUG > 0
			g_debug("\\[%s] at line %d", buffer,
				g3d_stream_line(stream));
#endif
			
		} else {
			if(currentSection && currentSection->callback)
				currentSection->callback(global);
		}
	}

	g_hash_table_destroy(sections);
	if(global->matfile)
		g_hash_table_destroy(global->matfile);
	if(global->submesh_texcoords)
		g_free(global->submesh_texcoords);
	g_free(global);
	g_free(buffer);

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Rigs of Rods files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("truck", ":", 0);
}

/*****************************************************************************/

gboolean ror_beams_cb(RorGlobalData *global)
{
	gint32 i;
	guint32 n1, n2;
	G3DVector *v1, *v2, v[3], vx[3], vi[3] = { 0.0, 0.0, 1.0 };
	G3DFloat angle, len;
	G3DObject *beam;
	G3DMaterial *material;
	G3DMatrix matrix[16];

	if(sscanf(global->buffer, "%u, %u", &n1, &n2) == 2) {
		if((n1 < global->object->vertex_count) &&
			(n2 < global->object->vertex_count)) {
			v1 = global->object->vertex_data + n1 * 3;
			v2 = global->object->vertex_data + n2 * 3;
			for(i = 0; i < 3; i ++)
				v[i] = v2[i] - v1[i];
			len = g3d_vector_length(v);
			angle = acos(g3d_vector_dot(vi, v));
			g3d_vector_normal(
				vi[0], vi[1], vi[2],
				v[0], v[1], v[2],
				&vx[0], &vx[1], &vx[2]);
			material = g3d_material_new();
			material->name = g_strdup("default");
			beam = g3d_primitive_cylinder(0.03, len, 6, TRUE, TRUE, material);
			beam->name = g_strdup_printf("beam %u->%u", n1, n2);
			beam->materials = g_slist_append(beam->materials, material);

			g3d_matrix_identity(matrix);
			g3d_matrix_rotate(angle, vx[0], vx[1], vx[2], matrix);
			g3d_object_transform(beam, matrix);

			g3d_matrix_identity(matrix);
			g3d_matrix_translate(v1[0], v1[1], v1[2], matrix);
			g3d_object_transform(beam, matrix);

			global->o_beams->objects = g_slist_append(global->o_beams->objects,
				beam);
		} else {
			g_warning("RoR: beam index out of bounds: %u, %u (%u)",
				n1, n2, global->object->vertex_count);
		}
	} else {
		g_warning("RoR: skipping line %s", global->buffer);
	}
	return TRUE;
}

gboolean ror_cab_cb(RorGlobalData *global)
{
	G3DFace *face;
	gint32 i;
	guint32 idx[3];

	if(sscanf(global->buffer, "%u, %u, %u", &idx[0], &idx[1], &idx[2]) == 3) {
		face = g3d_face_new_tri(global->texmat, idx[0], idx[1], idx[2]);
		global->object->faces = g_slist_prepend(
			global->object->faces, face);
		if(global->texmat->tex_image) {
			face->tex_image = global->texmat->tex_image;
			face->tex_vertex_count = 3;
			face->tex_vertex_data = g_new0(G3DFloat, 3 * 2);
			for(i = 0; i < 3; i ++) {
				face->tex_vertex_data[i * 2 + 0] =
					global->submesh_texcoords[idx[i] * 2 + 0];
				face->tex_vertex_data[i * 2 + 1] =
					global->submesh_texcoords[idx[i] * 2 + 1];
			}
			face->flags |= G3D_FLAG_FAC_TEXMAP;
		}
	}

	return TRUE;
}

gboolean ror_globals_cb(RorGlobalData *global)
{
	G3DFloat dm, cm;
	gchar *matname, *basename;

	matname = g_new0(gchar, ROR_LL);
	if(sscanf(global->buffer, "%f, %f, %s", &dm, &cm, matname) == 3) {
#if DEBUG > 0
		g_debug("RoR: dry mass: %.2f, cargo mass: %.2f, material: %s", dm, cm,
			matname);
#endif
		strcat(matname, ".material");
		basename = g_path_get_basename(matname);
		global->matfile = ror_material_read(basename);
		g_free(basename);

		if(global->matfile) {
			basename = g_hash_table_lookup(global->matfile, "texture");
			if(basename != NULL)
				global->texmat->tex_image = g3d_texture_load_cached(
					global->context, global->model, basename);
		}
	}
	g_free(matname);

	return TRUE;
}

gboolean ror_managedmaterials_cb(RorGlobalData *global)
{
	gchar *buf[3];
	gint32 i;

	for(i = 0; i < 3; i ++)
		buf[i] = g_new0(gchar, ROR_LL);

	if(sscanf(global->buffer, "%s %s %s", buf[0], buf[1], buf[2]) == 3) {
		/* load base_texture */
		global->texmat->tex_image = g3d_texture_load_cached(global->context,
			global->model, buf[2]);
	}

	for(i = 0; i < 3; i ++)
		g_free(buf[i]);

	return TRUE;
}

gboolean ror_nodes_cb(RorGlobalData *global)
{
	guint32 idx;
	G3DVector x, y, z;
	G3DObject *joint;
	G3DMaterial *material;
	G3DMatrix matrix[16];

	if(sscanf(global->buffer, "%u, %f, %f, %f", &idx, &x, &y, &z) == 4) {
#if DEBUG > 2
		g_debug("|node %u: %f, %f, %f", idx, x, y, z);
#endif
		if(global->object->vertex_count < (idx + 1)) {
			global->object->vertex_count = idx + 1;
			global->object->vertex_data = g_realloc(
				global->object->vertex_data,
				global->object->vertex_count * sizeof(G3DVector) * 3);
		}
		global->object->vertex_data[idx * 3 + 0] = x;
		global->object->vertex_data[idx * 3 + 1] = y;
		global->object->vertex_data[idx * 3 + 2] = z;

		material = g3d_material_new();
		material->name = g_strdup("default");
		material->r = 1.0;
		joint = g3d_primitive_sphere(0.1, 6, 6, material);
		joint->name = g_strdup_printf("joint #%u", idx);
		joint->materials = g_slist_append(joint->materials, material);

		g3d_matrix_identity(matrix);
		g3d_matrix_translate(x, y, z, matrix);
		g3d_object_transform(joint, matrix);

		global->o_joints->objects = g_slist_append(global->o_joints->objects,
			joint);
	} else {
		g_warning("RoR: skipping line %s", global->buffer);
	}
	return TRUE;
}

gboolean ror_texcoords_cb(RorGlobalData *global)
{
	guint32 idx;
	G3DFloat u, v;

	if(sscanf(global->buffer, "%u, %f, %f", &idx, &u, &v) == 3) {
		if(idx >= global->submesh_ncoords) {
			global->submesh_ncoords = idx + 1;
			global->submesh_texcoords = g_realloc(global->submesh_texcoords,
				global->submesh_ncoords * sizeof(G3DFloat) * 2);
		}
		global->submesh_texcoords[idx * 2 + 0] = u;
		global->submesh_texcoords[idx * 2 + 1] = v;
	}
	return TRUE;
}

