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
	ld->object = object;

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
	ld->object = object;

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
	return TRUE;
}

/* push level */
gboolean flt_cb_0010(FltGlobalData *gd, FltLocalData *ld)
{
	g_queue_push_head(gd->oqueue, ld->object);

	gd->level ++;
	return TRUE;
}

/* pop level */
gboolean flt_cb_0011(FltGlobalData *gd, FltLocalData *ld)
{
	g_queue_pop_head(gd->oqueue);

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

gboolean flt_cb_0069(FltGlobalData *gd, FltLocalData *ld)
{
	G3DObject *object;
	gint32 i;

	object = g3d_model_get_object_by_name(gd->model, "default object");
	if(object == NULL)
	{
		object = g_new0(G3DObject, 1);
		object->name = g_strdup("default object");
		gd->model->objects = g_slist_append(gd->model->objects, object);
	}

	object->vertex_count += 1;
	object->vertex_data = g_realloc(object->vertex_data,
		object->vertex_count * 3 * sizeof(gfloat));

	i = object->vertex_count - 1;
	object->vertex_data[i * 3 + 0] = g3d_read_double_be(gd->f);
	object->vertex_data[i * 3 + 1] = g3d_read_double_be(gd->f);
	object->vertex_data[i * 3 + 2] = g3d_read_double_be(gd->f);
	ld->nb -= 24;

	return TRUE;
}

/* vertex list */
gboolean flt_cb_0072(FltGlobalData *gd, FltLocalData *ld)
{
	G3DFace *face;
	gint32 i;

	for(i = 0; i < 1; i ++)
	{
		printf("FLT: 0072: %d\n", g3d_read_int32_be(gd->f));
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
