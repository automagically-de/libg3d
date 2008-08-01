/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2008  Markus Dahms <mad@automagically.de>

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

#include <string.h>
#include <math.h>

#include <g3d/read.h>
#include <g3d/material.h>
#include <g3d/model.h>

#include "imp_flt.h"
#include "imp_flt_callbacks.h"

/* header */
gboolean flt_cb_0001(FltGlobalData *gd, FltLocalData *ld)
{
	gchar id[8];

	fread(id, 1, 8, gd->f);
	id[7] = '\0';
	ld->nb -= 8;
	gd->fversion = g3d_read_int32_be(gd->f);
	ld->nb -= 4;

	g_debug("FLT: header: '%s' revision %u", id, gd->fversion);

	return TRUE;
}

/* group */
gboolean flt_cb_0002(FltGlobalData *gd, FltLocalData *ld)
{
	G3DObject *object, *pobj;
	gchar namebuf[8];

	object = g_new0(G3DObject, 1);
	pobj = (G3DObject *)g_queue_peek_head(gd->oqueue);
	if(pobj != NULL)
		pobj->objects = g_slist_append(pobj->objects, object);
	else
		gd->model->objects = g_slist_append(gd->model->objects, object);
	ld->g3dobj = object;

	/* group ID */
	fread(namebuf, 1, 8, gd->f);
	ld->nb -= 8;
	namebuf[7] = '\0';
	object->name = g_strdup_printf("group '%s'", namebuf);

	return TRUE;
}

/* object */
gboolean flt_cb_0004(FltGlobalData *gd, FltLocalData *ld)
{
	G3DObject *object, *pobj;
	G3DMaterial *material;
	gchar namebuf[8];

	object = g_new0(G3DObject, 1);
	pobj = (G3DObject *)g_queue_peek_head(gd->oqueue);
	if(pobj != NULL)
		pobj->objects = g_slist_append(pobj->objects, object);
	else
		gd->model->objects = g_slist_append(gd->model->objects, object);
	ld->g3dobj = object;

	material = g3d_material_new();
	material->name = g_strdup("fallback material");
	object->materials = g_slist_append(object->materials, material);

	/* object ID */
	fread(namebuf, 1, 8, gd->f);
	ld->nb -= 8;
	namebuf[7] = '\0';
	object->name = g_strdup_printf("object '%s'", namebuf);

	/* flags */
	g3d_read_int32_be(gd->f);
	ld->nb -= 4;

	/* relative priority */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* transparency */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* special effect ID 1 */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* special effect ID 2 */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* significance */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* reserved */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	return TRUE;
}

/* face */
gboolean flt_cb_0005(FltGlobalData *gd, FltLocalData *ld)
{
	G3DObject *object = ld->g3dobj;
	G3DMaterial *material;
	G3DFace *face;
	gint16 index;
	guint32 flags;
	gfloat r, g, b;

	g_return_val_if_fail(object != NULL, FALSE);

	if(object->materials == NULL) {
		material = g3d_material_new();
		material->name = g_strdup("(default material)");
		object->materials = g_slist_append(object->materials, material);
	}

	face = g_new0(G3DFace, 1);
	face->material = g_slist_nth_data(object->materials, 0);
	object->faces = g_slist_append(object->faces, face);
	ld->level_object = face;

	if(object->vertex_count == 0) {
		if(gd->vertex_palette) {
			/* copy vertex palette to object */
			object->vertex_count = gd->vertex_palette->n_entries;
			object->vertex_data = g_new0(gfloat, object->vertex_count * 3);
			memcpy(object->vertex_data, gd->vertex_palette->vertex_data,
				object->vertex_count * 3 * sizeof(gfloat));
		}
	}

	/* id */
	fseek(gd->f, 8, SEEK_CUR);
	ld->nb -= 8;
	/* IR color code */
	g3d_read_int32_be(gd->f);
	ld->nb -= 4;
	/* relative priority */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	/* draw type */
	g3d_read_int8(gd->f);
	ld->nb --;
	/* texture white */
	g3d_read_int8(gd->f);
	ld->nb --;
	/* color name index */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	/* alternate color name index */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	/* reserved */
	g3d_read_int8(gd->f);
	ld->nb --;
	/* template (billboard) */
	g3d_read_int8(gd->f);
	ld->nb --;
	/* detail texture pattern */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	/* texture pattern */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	/* material index */
	index = g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	if(index != -1) {
		material = g_slist_nth_data(gd->model->materials, index);
		if(material)
			face->material = material;
	}
	/* surface material code */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	/* feature ID */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	/* IR material code */
	g3d_read_int32_be(gd->f);
	ld->nb -= 4;
	/* transparency */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	/* LOD generation control */
	g3d_read_int8(gd->f);
	ld->nb --;
	/* line style index */
	g3d_read_int8(gd->f);
	ld->nb --;
	/* flags */
	flags = g3d_read_int32_be(gd->f);
	ld->nb -= 4;
	/* light mode */
	g3d_read_int8(gd->f);
	ld->nb --;
	/* reserved */
	fseek(gd->f, 7, SEEK_CUR);
	ld->nb -= 7;
	/* packed color, primary */
	if((flags & (1 << 1)) /* no color */ ||
		!(flags & (1 << 3))) /* packed color */
		g3d_read_int32_be(gd->f);
	else {
		g3d_read_int8(gd->f); /* alpha, unused */
		b = (gfloat)g3d_read_int8(gd->f) / 255.0;
		g = (gfloat)g3d_read_int8(gd->f) / 255.0;
		r = (gfloat)g3d_read_int8(gd->f) / 255.0;
		material = g3d_material_new();
		material->r = r;
		material->g = g;
		material->b = b;
		material->a = 1.0;
		face->material = material;
	}
	ld->nb -= 4;
	/* packed color, alternate */
	g3d_read_int32_be(gd->f);
	ld->nb -= 4;
	/* texture mapping index */
	index = g3d_read_int16_be(gd->f);
	ld->nb -= 2;
	if(index != -1) {
		g_debug("FLT: 0005: texture mapping index %d", index);
	}


	return TRUE;
}

/* push level */
gboolean flt_cb_0010(FltGlobalData *gd, FltLocalData *ld)
{
	g_queue_push_head(gd->oqueue, ld->g3dobj);

	gd->level ++;
	return TRUE;
}

/* pop level */
gboolean flt_cb_0011(FltGlobalData *gd, FltLocalData *ld)
{
	ld->g3dobj = g_queue_pop_head(gd->oqueue);
	ld->level_object = NULL;

	if(gd->level > 0)
		gd->level --;
	return TRUE;
}

/* color palette */
gboolean flt_cb_0032(FltGlobalData *gd, FltLocalData *ld)
{
	G3DMaterial *material;
	G3DObject *cobj;
	gint32 i;

	cobj = g_new0(G3DObject, 1);
	cobj->name = g_strdup("color palette");
	gd->model->objects = g_slist_append(gd->model->objects, cobj);

	/* skip reserved bytes */
	fseek(gd->f, 128, SEEK_CUR);
	ld->nb -= 128;

	/* get colors */
	for(i = 0; i < 1024; i ++)
	{
		material = g3d_material_new();
		material->name = g_strdup_printf("color %d", i);
		cobj->materials = g_slist_append(cobj->materials, material);

		material->a = g3d_read_int8(gd->f) / 255.0;
		material->b = g3d_read_int8(gd->f) / 255.0;
		material->g = g3d_read_int8(gd->f) / 255.0;
		material->r = g3d_read_int8(gd->f) / 255.0;
		ld->nb -= 4;
	}

	return TRUE;
}

/* texture palette */
gboolean flt_cb_0064(FltGlobalData *gd, FltLocalData *ld)
{
	gchar fname[201];

	fread(fname, 1, 200, gd->f);
	ld->nb -= 200;

	g_debug("FLT: 0064: %s", fname);
	return TRUE;
}

static gboolean flt_inc_vertex_palette(FltGlobalData *gd)
{
	g_return_val_if_fail(gd->vertex_palette != NULL, FALSE);

	gd->vertex_palette->n_entries ++;
	gd->vertex_palette->offsets = g_realloc(
		gd->vertex_palette->offsets,
		gd->vertex_palette->n_entries * sizeof(goffset));
	gd->vertex_palette->flags = g_realloc(
		gd->vertex_palette->flags,
		gd->vertex_palette->n_entries * sizeof(guint32));
	gd->vertex_palette->vertex_data = g_realloc(
		gd->vertex_palette->vertex_data,
		gd->vertex_palette->n_entries * 3 * sizeof(gfloat));
	gd->vertex_palette->normal_data = g_realloc(
		gd->vertex_palette->normal_data,
		gd->vertex_palette->n_entries * 3 * sizeof(gfloat));
	gd->vertex_palette->tex_vertex_data = g_realloc(
		gd->vertex_palette->tex_vertex_data,
		gd->vertex_palette->n_entries * 2 * sizeof(gfloat));
	gd->vertex_palette->vertex_materials = g_realloc(
		gd->vertex_palette->vertex_materials,
		gd->vertex_palette->n_entries * sizeof(G3DMaterial *));
	return TRUE;
}

/* vertex palette */
gboolean flt_cb_0067(FltGlobalData *gd, FltLocalData *ld)
{
	gd->vertex_palette = g_new0(FltVertexPalette, 1);
	gd->vertex_palette->offset = 8;
	return TRUE;
}

static G3DMaterial *flt_material_by_index(FltGlobalData *gd, guint32 i)
{
	G3DObject *colobj;

	colobj = g3d_model_get_object_by_name(gd->model, "color palette");
	if(colobj == NULL)
		return NULL;

	return g_slist_nth_data(colobj->materials, i);
}

#define FLOAT_EQUALS(a, b) (fabs((a) - (b)) < 0.001)

static G3DMaterial *flt_find_color(G3DObject *pobj,
	gfloat r, gfloat g, gfloat b, gfloat a)
{
	GSList *item;
	G3DMaterial *material;

	for(item = pobj->materials; item != NULL; item = item->next) {
		material = item->data;
		if(FLOAT_EQUALS(material->r, r) && FLOAT_EQUALS(material->g, g) &&
			FLOAT_EQUALS(material->b, b) && FLOAT_EQUALS(material->a, a))
			return material;
	}
	g_debug("did not find material");
	return NULL;
}

static gboolean flt_handle_vertex_color(FltGlobalData *gd, FltLocalData *ld,
	guint32 flags)
{
	G3DObject *vcolobj;
	G3DMaterial *material;
	gfloat r, g, b, a;

	if(flags & FLT_FLAG_NO_COLOR)
		return TRUE;

	vcolobj = g3d_model_get_object_by_name(gd->model, "vertex colors");
	if(vcolobj == NULL) {
		vcolobj = g_new0(G3DObject, 1);
		vcolobj->name = g_strdup("vertex colors");
		gd->model->objects = g_slist_append(gd->model->objects, vcolobj);
	}
	a = (gfloat)g3d_read_int8(gd->f) / 255.0;
	b = (gfloat)g3d_read_int8(gd->f) / 255.0;
	g = (gfloat)g3d_read_int8(gd->f) / 255.0;
	r = (gfloat)g3d_read_int8(gd->f) / 255.0;
	ld->nb -= 4;

	if(flags & FLT_FLAG_PACKED_COLOR) {
		material = flt_find_color(vcolobj, r, g, b, a);
		if(material == NULL) {
			material = g3d_material_new();
			material->r = r;
			material->g = g;
			material->b = b;
			material->a = a;
			material->name = g_strdup_printf(
				"vertex material (%.2f, %.2f, %.2f, %.2f)", r, g, b, a);
			vcolobj->materials = g_slist_prepend(vcolobj->materials, material);
		}
	} else {
		/* color index */
		material = flt_material_by_index(gd, g3d_read_int32_be(gd->f));
		ld->nb -= 4;
	}
	gd->vertex_palette->vertex_materials[gd->vertex_palette->n_entries - 1] =
		material;
	return TRUE;
}

/* vertex with color record */
gboolean flt_cb_0068(FltGlobalData *gd, FltLocalData *ld)
{
	gint32 i, index;
	guint32 flags;

	if(!flt_inc_vertex_palette(gd))
		return FALSE;

	index = gd->vertex_palette->n_entries - 1;
	gd->vertex_palette->offsets[index] = gd->vertex_palette->offset;
	gd->vertex_palette->offset += ld->nb + 4;
	gd->vertex_palette->flags[index] = 0;

	/* color name index */
	gd->vertex_palette->vertex_materials[index] =
		flt_material_by_index(gd, g3d_read_int16_be(gd->f));
	ld->nb -= 2;

	/* flags */
	flags = g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* vertex coordinate */
	for(i = 0; i < 3; i ++) {
		gd->vertex_palette->vertex_data[index * 3 + i] =
			g3d_read_double_be(gd->f);
		ld->nb -= 8;
	}

	flt_handle_vertex_color(gd, ld, flags);

	return TRUE;
}

/* vertex with color and normal record */
gboolean flt_cb_0069(FltGlobalData *gd, FltLocalData *ld)
{
	gint32 i, index;
	guint32 flags;

	if(!flt_inc_vertex_palette(gd))
		return FALSE;

	index = gd->vertex_palette->n_entries - 1;
	gd->vertex_palette->offsets[index] = gd->vertex_palette->offset;
	gd->vertex_palette->offset += ld->nb + 4;
	gd->vertex_palette->flags[index] = G3D_FLAG_FAC_NORMALS;

	/* color name index */
	gd->vertex_palette->vertex_materials[index] =
		flt_material_by_index(gd, g3d_read_int16_be(gd->f));
	ld->nb -= 2;

	/* flags */
	flags = g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* vertex coordinate */
	for(i = 0; i < 3; i ++) {
		gd->vertex_palette->vertex_data[index * 3 + i] =
			g3d_read_double_be(gd->f);
		ld->nb -= 8;
	}

	/* vertex normal */
	for(i = 0; i < 3; i ++) {
		gd->vertex_palette->normal_data[index * 3 + i] =
			g3d_read_float_be(gd->f);
		ld->nb -= 4;
	}

	flt_handle_vertex_color(gd, ld, flags);

	return TRUE;
}

/* vertex with color, normal and uv record */
gboolean flt_cb_0070(FltGlobalData *gd, FltLocalData *ld)
{
	gint32 i, index;
	guint32 flags;

	if(!flt_inc_vertex_palette(gd))
		return FALSE;

	index = gd->vertex_palette->n_entries - 1;
	gd->vertex_palette->offsets[index] = gd->vertex_palette->offset;
	gd->vertex_palette->offset += ld->nb + 4;
	gd->vertex_palette->flags[index] =
		G3D_FLAG_FAC_NORMALS | G3D_FLAG_FAC_TEXMAP;

	/* color name index */
	gd->vertex_palette->vertex_materials[index] =
		flt_material_by_index(gd, g3d_read_int16_be(gd->f));
	ld->nb -= 2;

	/* flags */
	flags = g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* vertex coordinate */
	for(i = 0; i < 3; i ++) {
		gd->vertex_palette->vertex_data[index * 3 + i] =
			g3d_read_double_be(gd->f);
		ld->nb -= 8;
	}

	/* vertex normal */
	for(i = 0; i < 3; i ++) {
		gd->vertex_palette->normal_data[index * 3 + i] =
			g3d_read_float_be(gd->f);
		ld->nb -= 4;
	}

	/* vertex texture coordinate */
	for(i = 0; i < 2; i ++) {
		gd->vertex_palette->tex_vertex_data[index * 2 + i] =
			g3d_read_float_be(gd->f);
		ld->nb -= 4;
	}

	flt_handle_vertex_color(gd, ld, flags);

	return TRUE;
}

/* vertex with color and uv record */
gboolean flt_cb_0071(FltGlobalData *gd, FltLocalData *ld)
{
	gint32 i, index;
	guint32 flags;

	if(!flt_inc_vertex_palette(gd))
		return FALSE;

	index = gd->vertex_palette->n_entries - 1;
	gd->vertex_palette->offsets[index] = gd->vertex_palette->offset;
	gd->vertex_palette->offset += ld->nb + 4;
	gd->vertex_palette->flags[index] = G3D_FLAG_FAC_TEXMAP;

	/* color name index */
	gd->vertex_palette->vertex_materials[index] =
		flt_material_by_index(gd, g3d_read_int16_be(gd->f));
	ld->nb -= 2;

	/* flags */
	flags = g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* vertex coordinate */
	for(i = 0; i < 3; i ++) {
		gd->vertex_palette->vertex_data[index * 3 + i] =
			g3d_read_double_be(gd->f);
		ld->nb -= 8;
	}

	/* vertex texture coordinate */
	for(i = 0; i < 2; i ++) {
		gd->vertex_palette->tex_vertex_data[index * 2 + i] =
			g3d_read_float_be(gd->f);
		ld->nb -= 4;
	}

	flt_handle_vertex_color(gd, ld, flags);

	return TRUE;
}

static gint32 flt_vertex_palette_index_from_offset(FltGlobalData *gd,
	goffset offset)
{
	FltVertexPalette *pal = gd->vertex_palette;
	gint i;

	g_return_val_if_fail(pal != NULL, -1);

	for(i = 0; (i < pal->n_entries) && (pal->offsets[i] < offset); i ++);
	if((i < pal->n_entries) && (pal->offsets[i] == offset))
		return i;
	g_warning("FLT: flt_vertex_palette_index_from_offset: "
		"could not get index for offset %li (i=%d)", (long int)offset, i);
	return -1;
}

/* vertex list */
gboolean flt_cb_0072(FltGlobalData *gd, FltLocalData *ld)
{
	G3DFace *face = ld->level_object;
	gint32 i, j, index;
	guint32 n;

	g_return_val_if_fail(face != NULL, FALSE);

	n = ld->nb / 4;
	face->vertex_count = n;
	face->vertex_indices = g_new0(guint32, n);

	for(i = 0; i < n; i ++) {
		j = g3d_read_int32_be(gd->f);
		ld->nb -= 4;

#define FLT_SUPPORT_BROKEN_VERTEX_LIST 1
#if FLT_SUPPORT_BROKEN_VERTEX_LIST
		/* FIXME: there are some broken models which directly specify the
		 * index instead of a byte offset in the vertex palette */

		if(gd->flags & FLT_FLAG_BROKEN_VERTEX_LIST)
			index = j;
		else
			index = flt_vertex_palette_index_from_offset(gd, j);
		if(index == -1) {
			index = j;
			gd->flags |= FLT_FLAG_BROKEN_VERTEX_LIST;
		}
#else
		index = flt_vertex_palette_index_from_offset(gd, j);
#endif
		if(index == -1)
			return FALSE;
		face->vertex_indices[i] = index;
		if(gd->vertex_palette->flags[index] & G3D_FLAG_FAC_NORMALS) {
			/* copy normal data */
			if(face->normals == NULL) {
				face->normals = g_new0(gfloat, n * 3);
				face->flags |= G3D_FLAG_FAC_NORMALS;
			}
			for(j = 0; j < 3; j ++)
				face->normals[i * 3 + j] =
					gd->vertex_palette->normal_data[index * 3 + j];
		}
		if(gd->vertex_palette->flags[index] & G3D_FLAG_FAC_TEXMAP) {
			/* copy texture coordinate data */
			if(face->tex_vertex_data == NULL) {
				face->tex_vertex_data = g_new0(gfloat, n * 2);
				face->tex_vertex_count = n;
				/* face->flags |= G3D_FLAG_FAC_TEXMAP; */
			}
			for(j = 0; j < 2; j ++)
				face->tex_vertex_data[i * 2 + j] =
					gd->vertex_palette->tex_vertex_data[index * 2 + j];
		}
		if(gd->vertex_palette->vertex_materials[index] != NULL)
			face->material = gd->vertex_palette->vertex_materials[index];
	}
	return TRUE;
}

/* local vertex pool */
gboolean flt_cb_0085(FltGlobalData *gd, FltLocalData *ld)
{
	G3DObject *object;
	guint32 nverts, attrmask;
	gint32 i;

	object = (G3DObject *)g_queue_peek_head(gd->oqueue);
	g_return_val_if_fail(object != NULL, FALSE);

	nverts = g3d_read_int32_be(gd->f);
	attrmask = g3d_read_int32_be(gd->f);
	ld->nb -= 8;

	printf("FLT: 0085: %d vertices, attrmask: 0x%08x\n", nverts, attrmask);

	object->vertex_count = nverts;
	object->vertex_data = g_new0(gfloat, nverts * 3);

	for(i = 0; i < nverts; i ++)
	{
		if(attrmask & (1 << 31)) /* has position */
		{
			object->vertex_data[i * 3 + 0] = g3d_read_double_be(gd->f);
			object->vertex_data[i * 3 + 1] = g3d_read_double_be(gd->f);
			object->vertex_data[i * 3 + 2] = g3d_read_double_be(gd->f);
			ld->nb -= 24;
		}

		if(attrmask & (1 << 30)) /* has color index */
		{
			g3d_read_int32_be(gd->f);
			ld->nb -= 4;
		}

		if(attrmask & (1 << 29)) /* has RGBA color */
		{
			g3d_read_int32_be(gd->f);
			ld->nb -= 4;
		}

		if(attrmask & (1 << 28)) /* has normal */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 12;
		}

		if(attrmask & (1 << 27)) /* has base UV */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 8;
		}

		if(attrmask & (1 << 26)) /* has UV layer 1 */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 8;
		}

		if(attrmask & (1 << 25)) /* has UV layer 2 */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 8;
		}

		if(attrmask & (1 << 24)) /* has UV layer 3 */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 8;
		}

		if(attrmask & (1 << 23)) /* has UV layer 4 */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 8;
		}

		if(attrmask & (1 << 22)) /* has UV layer 5 */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 8;
		}

		if(attrmask & (1 << 21)) /* has UV layer 6 */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 8;
		}

		if(attrmask & (1 << 20)) /* has UV layer 7 */
		{
			g3d_read_float_be(gd->f);
			g3d_read_float_be(gd->f);
			ld->nb -= 8;
		}
	}

	return TRUE;
}

/* mesh primitive */
gboolean flt_cb_0086(FltGlobalData *gd, FltLocalData *ld)
{
	guint32 type, isize, nverts;
	gint32 i;
	G3DFace *face;
	G3DObject *object;

	object = (G3DObject *)g_queue_peek_head(gd->oqueue);
	g_return_val_if_fail(object != NULL, FALSE);

	type = g3d_read_int16_be(gd->f);
	isize = g3d_read_int16_be(gd->f);
	nverts = g3d_read_int32_be(gd->f);
	ld->nb -= 8;

	printf("FLT: 0086: type: %d, index size: %d, %d vertices\n",
		type, isize, nverts);

	switch(type)
	{
		case 1: /* triangle strip */
			break;

		case 2: /* triangle fan */
			break;

		case 3: /* quadriteral strip */
			break;

		case 4: /* indexed polygon */
			face = g_new0(G3DFace, 1);
			face->material = (G3DMaterial *)g_slist_nth_data(object->materials,
				0);
			face->vertex_count = nverts;
			face->vertex_indices = g_new0(guint32, nverts);
			for(i = 0; i < nverts; i ++)
			{
				face->vertex_indices[i] = flt_read_typed_index(
					gd->f, isize, &(ld->nb));
			}
			object->faces = g_slist_append(object->faces, face);
			break;

		default:
			g_warning("FLT: mesh primitive: unknown type %d\n", type);
	}

	return TRUE;
}

/* material palette */
gboolean flt_cb_0113(FltGlobalData *gd, FltLocalData *ld)
{
	G3DMaterial *material;
	gchar name[13];

	material = g3d_material_new();

	/* material index */
	g3d_read_int32_be(gd->f);
	ld->nb -= 4;
	/* material name */
	fread(name, 1, 12, gd->f);
	ld->nb -= 12;
	name[12] = '\0';
	material->name = g_strdup(name);
	/* flags */
	g3d_read_int32_be(gd->f);
	ld->nb -= 4;
	/* ambient */
	g3d_read_float_be(gd->f);
	g3d_read_float_be(gd->f);
	g3d_read_float_be(gd->f);
	ld->nb -= 12;
	/* diffuse */
	material->r = g3d_read_float_be(gd->f);
	material->g = g3d_read_float_be(gd->f);
	material->b = g3d_read_float_be(gd->f);
	ld->nb -= 12;
	/* specular */
	material->specular[0] = g3d_read_float_be(gd->f);
	material->specular[1] = g3d_read_float_be(gd->f);
	material->specular[2] = g3d_read_float_be(gd->f);
	ld->nb -= 12;
	/* emissive */
	g3d_read_float_be(gd->f);
	g3d_read_float_be(gd->f);
	g3d_read_float_be(gd->f);
	ld->nb -= 12;
	/* shininess */
	material->shininess = g3d_read_float_be(gd->f);
	ld->nb -= 4;
	/* alpha */
	material->a = g3d_read_float_be(gd->f);
	ld->nb -= 4;

	gd->model->materials = g_slist_append(gd->model->materials, material);

	return TRUE;
}

