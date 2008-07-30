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

#include <g3d/read.h>
#include <g3d/material.h>
#include <g3d/model.h>

#include "imp_flt.h"
#include "imp_flt_callbacks.h"

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
			g_debug("FLT: 0005: copying vertex palette (%d entries) to object",
				gd->vertex_palette->n_entries);
			object->vertex_count = gd->vertex_palette->n_entries;
			object->vertex_data = g_new0(gfloat, object->vertex_count * 3);
			memcpy(object->vertex_data, gd->vertex_palette->vertex_data,
				object->vertex_count * 3 * sizeof(gfloat));
		}
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

static gboolean flt_inc_vertex_palette(FltGlobalData *gd)
{
	g_return_val_if_fail(gd->vertex_palette != NULL, FALSE);

	gd->vertex_palette->n_entries ++;
	gd->vertex_palette->offsets = g_realloc(
		gd->vertex_palette->offsets,
		gd->vertex_palette->n_entries * sizeof(goffset));
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

/* vertex with color and normal record */
gboolean flt_cb_0069(FltGlobalData *gd, FltLocalData *ld)
{
	gint32 i, index;

	if(!flt_inc_vertex_palette(gd))
		return FALSE;

	index = gd->vertex_palette->n_entries - 1;
	gd->vertex_palette->offsets[index] = gd->vertex_palette->offset;
	gd->vertex_palette->offset += ld->nb + 4;

	/* color name index */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* flags */
	g3d_read_int16_be(gd->f);
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

	/* TODO: color stuff */

	return TRUE;
}

/* vertex with color, normal and uv record */
gboolean flt_cb_0070(FltGlobalData *gd, FltLocalData *ld)
{
	gint32 i, index;

	if(!flt_inc_vertex_palette(gd))
		return FALSE;

	index = gd->vertex_palette->n_entries - 1;
	gd->vertex_palette->offsets[index] = gd->vertex_palette->offset;
	gd->vertex_palette->offset += ld->nb + 4;

	/* color name index */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* flags */
	g3d_read_int16_be(gd->f);
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

	/* TODO: color stuff */

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
	gint32 i, index;
	guint32 n;

	g_return_val_if_fail(face != NULL, FALSE);

	n = ld->nb / 4;
	face->vertex_count = n;
	face->vertex_indices = g_new0(guint32, n);

	for(i = 0; i < n; i ++) {
		index = flt_vertex_palette_index_from_offset(gd,
			g3d_read_int32_be(gd->f));
		face->vertex_indices[i] = index;
		ld->nb -= 4;
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
