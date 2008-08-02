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
#include <g3d/stream.h>
#include <g3d/material.h>

#include "imp_acf.h"
#include "imp_acf_def.h"
#include "imp_acf_detect.h"

static gboolean acf_load_body(AcfGlobalData *global);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	AcfGlobalData *global;
	AcfDef *def;
	gboolean bigendian;
	gchar magic;

	magic = g3d_stream_read_int8(stream);
	if(magic == 'i') {
		bigendian = FALSE;
		g_debug("ACF: intel file found");
	} else if(magic == 'a') {
		bigendian = TRUE;
		g_debug("ACF: apple file found");
	} else {
		g_warning("ACF: not a valid .acf file");
		return FALSE;
	}

	g3d_stream_seek(stream, 0, G_SEEK_SET);

	global = g_new0(AcfGlobalData, 1);
	global->context = context;
	global->model = model;

	def = acf_detect_version(global);

	global->acf = acf_def_read(stream, def, bigendian);
	if(global->acf == NULL) {
		g_free(global);
		return FALSE;
	}

	if(!acf_load_body(global)) {
		acf_def_free(global->acf);
		g_free(global);
		return FALSE;
	}

	acf_def_free(global->acf);
	g_free(global);
	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for XPlane .acf files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("acf", ":", 0);
}

/*****************************************************************************/

#define ACF_BODY_NUMSEC 20
#define ACF_BODY_SECVER 18
#define ACF_VERTS_PER_OBJECT (ACF_BODY_NUMSEC * ACF_BODY_SECVER)

static gboolean acf_load_body(AcfGlobalData *global)
{
	AcfValue *vpart_eq, *vbody_x, *vbody_y, *vbody_z;
	AcfValue *vxarm, *vyarm, *vzarm;
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	gint32 i, j;

	vpart_eq = acf_def_lookup(global->acf, "PARTS_part_eq");
	vbody_x = acf_def_lookup(global->acf, "PARTS_body_X");
	vbody_y = acf_def_lookup(global->acf, "PARTS_body_Y");
	vbody_z = acf_def_lookup(global->acf, "PARTS_body_Z");
	vxarm = acf_def_lookup(global->acf, "PARTS_Xarm");
	vyarm = acf_def_lookup(global->acf, "PARTS_Yarm");
	vzarm = acf_def_lookup(global->acf, "PARTS_Zarm");

	material = g3d_material_new();
	material->name = g_strdup("(default material)");
	global->model->materials = g_slist_append(global->model->materials,
		material);

	for(i = 44; i <= 66; i ++) {
		if(!vpart_eq->xint[i])
			continue;

		object = g_new0(G3DObject, 1);
		object->name = g_strdup_printf("object[%d]", i);
		object->vertex_count = ACF_VERTS_PER_OBJECT;
		object->vertex_data = g_new0(gfloat, object->vertex_count * 3);
		for(j = 0; j < object->vertex_count; j ++) {
			object->vertex_data[j * 3 + 0] =
				vbody_x->xflt[i * ACF_VERTS_PER_OBJECT + j] + vxarm->xflt[i];
			object->vertex_data[j * 3 + 1] =
				vbody_y->xflt[i * ACF_VERTS_PER_OBJECT + j] + vyarm->xflt[i];
			object->vertex_data[j * 3 + 2] =
				vbody_z->xflt[i * ACF_VERTS_PER_OBJECT + j] + vzarm->xflt[i];
		}

		global->model->objects = g_slist_append(global->model->objects,
			object);

		for(j = 0; j < (ACF_VERTS_PER_OBJECT - ACF_BODY_SECVER - 1); j ++) {
			face = g_new0(G3DFace, 1);
			face->material = material;
			face->vertex_count = 3;
			face->vertex_indices = g_new0(guint32, 3);
			face->vertex_indices[0] = j + 1;
			face->vertex_indices[1] = j;
			face->vertex_indices[2] = j + ACF_BODY_SECVER;
			object->faces = g_slist_append(object->faces, face);
			face = g_new0(G3DFace, 1);
			face->material = material;
			face->vertex_count = 3;
			face->vertex_indices = g_new0(guint32, 3);
			face->vertex_indices[0] = j + ACF_BODY_SECVER;
			face->vertex_indices[1] = j + ACF_BODY_SECVER + 1;
			face->vertex_indices[2] = j + 1;
			object->faces = g_slist_append(object->faces, face);
		}
	}

	return TRUE;
}
