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
#include "imp_acf_airfoil.h"
#include "imp_acf_def.h"
#include "imp_acf_detect.h"

static gboolean acf_load_body(AcfGlobalData *global);

gpointer plugin_init(G3DContext *context)
{
	return acf_airfoil_init();
}

void plugin_cleanup(gpointer user_data)
{
	AcfAirfoilDb *db = user_data;

	if(db)
		acf_airfoil_cleanup(db);
}

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	AcfGlobalData *global;
	const AcfDef *def;
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
	global->stream = stream;
	global->afldb = user_data;

	def = acf_detect_version(global);
	if(def == NULL) {
		g_free(global);
		return FALSE;
	}

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

#define ACF_OBJECT_MIN 44
#define ACF_OBJECT_MAX 66
#define ACF_BODY_NUMSEC 20
#define ACF_BODY_SECVER 18
#define ACF_VERTS_PER_OBJECT (ACF_BODY_NUMSEC * ACF_BODY_SECVER)

#define ACF_REQUIRE_PART(var, name, t) \
	var = acf_def_lookup(global->acf, name); \
	if((var == NULL) || (var->type != t)) { \
		g_warning("ACF: %s is missing or has wrong type", name); \
		return FALSE; \
	}

static gboolean acf_load_body(AcfGlobalData *global)
{
	AcfValue *vpart_eq, *vbody_x, *vbody_y, *vbody_z, *vbody_r;
	AcfValue *vxarm, *vyarm, *vzarm, *vx_body, *vy_body, *vz_body;
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	gint32 i, j;
	guint32 min_obj, max_obj;

	/* lookup some required data in library an return FALSE if one of
	 * them is missing */
	ACF_REQUIRE_PART(vpart_eq, "PARTS_part_eq", XINT);
	ACF_REQUIRE_PART(vbody_x,  "PARTS_body_X",  XFLT);
	ACF_REQUIRE_PART(vbody_y,  "PARTS_body_Y",  XFLT);
	ACF_REQUIRE_PART(vbody_z,  "PARTS_body_Z",  XFLT);
	ACF_REQUIRE_PART(vxarm,    "PARTS_Xarm",    XFLT);
	ACF_REQUIRE_PART(vyarm,    "PARTS_Yarm",    XFLT);
	ACF_REQUIRE_PART(vzarm,    "PARTS_Zarm",    XFLT);
	ACF_REQUIRE_PART(vx_body,  "PARTS_X_body_aero", XFLT);
	ACF_REQUIRE_PART(vy_body,  "PARTS_Y_body_aero", XFLT);
	ACF_REQUIRE_PART(vz_body,  "PARTS_Z_body_aero", XFLT);
	ACF_REQUIRE_PART(vbody_r,  "PARTS_body_r",  XFLT);

	material = g3d_material_new();
	material->name = g_strdup("(default material)");
	global->model->materials = g_slist_append(global->model->materials,
		material);

	min_obj = ACF_OBJECT_MIN;
	max_obj = MIN(ACF_OBJECT_MAX, (vpart_eq->num - 1));
	if(vpart_eq->num <= 57) {
		min_obj = 30;
		max_obj = 49;
	}

	for(i = min_obj; i <= max_obj; i ++) {
		if(!vpart_eq->xint[i])
			continue;

		g_debug("ACF: part[%i]: (%.2f, %.2f, %.2f), %.2f", i,
			vx_body->xflt[i], vy_body->xflt[i], vz_body->xflt[i],
			vbody_r->xflt[i]);

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
#if DEBUG > 3
			g_debug("\tVERT: (%04d) %.3f, %.3f, %.3f", j,
				object->vertex_data[j * 3 + 0],
				object->vertex_data[j * 3 + 1],
				object->vertex_data[j * 3 + 2]);
#endif
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
