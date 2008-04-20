/* $Id:$ */

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
#include <g3d/material.h>
#include <g3d/stream.h>

#include "imp_max_callbacks.h"

static gchar *max_read_wchar(G3DStream *stream, guint32 n)
{
	gint32 i;
	gchar *text;

	/* TODO: real wide char handling */

	text = g_new0(gchar, n + 1);
	for(i = 0; i < n; i ++) {
		text[i] = g3d_stream_read_int8(stream);
		g3d_stream_read_int8(stream);
	}
	return text;
}

gboolean max_cb_debug_wchars(MaxGlobalData *global, MaxLocalData *local)
{
	gchar *str;
	guint32 len;

	len = local->nb / 2;
	str = max_read_wchar(global->stream, len);
	local->nb -= len * 2;

	g_debug("\\%s[TEXT] %s (%d)",
		(global->padding + (strlen(global->padding) - local->level)),
		str, len);
	g_free(str);

	return TRUE;
}

gboolean max_cb_debug_string(MaxGlobalData *global, MaxLocalData *local)
{
	gchar *str;
	guint32 len;

	g_return_val_if_fail(local->nb >= 4, FALSE);
	len = g3d_stream_read_int32_le(global->stream);
	local->nb -= 4;
	if(len > local->nb)
		len = local->nb;

	str = g_new0(gchar, len + 1);
	g3d_stream_read(global->stream, str, 1, len);
	local->nb -= len;

	g_debug("\\%s[TEXT] %s (%d)",
		(global->padding + (strlen(global->padding) - local->level)),
		str, len);
	g_free(str);

	return TRUE;
}

gboolean max_cb_0x0001_0x0005(MaxGlobalData *global, MaxLocalData *local)
{
	gchar *str;
	gint32 len, i, w3[3], cnt = 0;

	g_return_val_if_fail(local->nb >= 4, FALSE);

	/* flags? */
	i = g3d_stream_read_int32_le(global->stream);
	local->nb -= 4;

#if DEBUG > 0
	g_debug("\\%s[PROP] 0x%08x",
		(global->padding + (strlen(global->padding) - local->level)), i);
#endif

	while(local->nb > 0) {
		len = g3d_stream_read_int32_le(global->stream);
		local->nb -= 4;
		str = g_malloc0(len + 1);
		g3d_stream_read(global->stream, str, 1, len);
		local->nb -= len;
		for(i = 0; i < 3; i ++)
			w3[i] = g3d_stream_read_int16_le(global->stream);
		local->nb -= 6;
		cnt ++;
#if DEBUG > 0
		g_debug("\\%s[PROP]  %04d: '%s' (%d bytes) [%d, %d, %d]",
			(global->padding + (strlen(global->padding) - local->level)),
			cnt, str, len,
			w3[0], w3[1], w3[2]);
#endif
		g_free(str);
	}
	return TRUE;
}

gboolean max_cb_0x08FE_0x0100(MaxGlobalData *global, MaxLocalData *local)
{
	guint32 num;
	gint i, j;
	G3DObject *object;

	if(local->nb < 4)
		return FALSE;

	/* vertices */
	num = g3d_stream_read_int32_le(global->stream);
	local->nb -= 4;
	object = (G3DObject *)local->object;
	if(object == NULL) {
		g_warning("MAX: 0x08FE::0x0100: no object");
		return FALSE;
	}

	object->vertex_count = num;
	object->vertex_data = g_new0(gfloat, 3 * num);

	for(i = 0; i < num; i ++) {
		if(local->nb < 16)
			return FALSE;
		g3d_stream_read_int32_le(global->stream); /* always 0 */
		for(j = 0; j < 3; j ++)
			object->vertex_data[i * 3 + j] =
				g3d_stream_read_float_le(global->stream);
		local->nb -= 16;
	}
	return TRUE;
}

/* triangles */
gboolean max_cb_0x08FE_0x010A(MaxGlobalData *global, MaxLocalData *local)
{
	guint32 num;
	gint i, j;
	G3DObject *object;
	G3DFace *face;
	G3DMaterial *mat;

	mat = (G3DMaterial *)g_slist_nth_data(global->model->materials, 0);

	if(local->nb < 4)
		return FALSE;

	num = g3d_stream_read_int32_le(global->stream);
	local->nb -= 4;
	object = (G3DObject *)local->object;
	if(object == NULL) {
		g_warning("MAX: 0x08FE::0x010A: no object");
		return FALSE;
	}

	for(i = 0; i < num; i ++) {
		face = g_new0(G3DFace, 1);
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		face->material = mat;
		object->faces = g_slist_append(object->faces, face);

		if(local->nb < 12)
			return FALSE;
		for(j = 0; j < 3; j ++) {
			face->vertex_indices[j] =
				g3d_stream_read_int16_le(global->stream);
			if(face->vertex_indices[j] >= object->vertex_count) {
				g_warning("MAX: 0x08FE::0x010A: vertex index too high"
					" (%d (0x%08x) >= %d)",
					face->vertex_indices[j], face->vertex_indices[j],
					object->vertex_count);
				face->vertex_indices[j] = 0;
			}
			/* FIXME: flags? */
			g3d_stream_read_int16_le(global->stream);
		}
		local->nb -= 12;
	}
	return TRUE;
}

/* mesh */
gboolean max_cb_0x001B_0x08FE(MaxGlobalData *global, MaxLocalData *local)
{
	G3DObject *object;
	object = g_new0(G3DObject, 1);
	if(object->name)
		g_free(object->name);
	object->name = g_strdup_printf("0x08FE object @ 0x%08x",
		(guint32)g3d_stream_tell(global->stream));
	local->object = object;
	global->model->objects = g_slist_append(global->model->objects, object);

	return TRUE;
}

/* triangles */
gboolean max_cb_0x08FE_0x0912(MaxGlobalData *global, MaxLocalData *local)
{
	guint32 num;
	gint i, j;
	G3DObject *object;
	G3DFace *face;
	G3DMaterial *mat;

	mat = (G3DMaterial *)g_slist_nth_data(global->model->materials, 0);

	if(local->nb < 4)
		return FALSE;

	/* faces */
	num = g3d_stream_read_int32_le(global->stream);
	local->nb -= 4;
	object = (G3DObject *)local->object;
	if(object == NULL) {
		g_warning("MAX: 0x08FE::0x0912: no object");
		return FALSE;
	}

	for(i = 0; i < num; i ++) {
		face = g_new0(G3DFace, 1);
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		face->material = mat;
		object->faces = g_slist_append(object->faces, face);

		if(local->nb < 20)
			return FALSE;
		for(j = 0; j < 3; j ++) {
			face->vertex_indices[j] =
				g3d_stream_read_int32_le(global->stream);
			if(face->vertex_indices[j] >= object->vertex_count) {
				g_warning("MAX: 0x08FE::0x0912: vertex index too high"
					" (%d (0x%08x) >= %d)",
					face->vertex_indices[j], face->vertex_indices[j],
					object->vertex_count);
				face->vertex_indices[j] = 0;
			}
		}
		/* unknown for now */
		g3d_stream_read_int32_le(global->stream);
		g3d_stream_read_int32_le(global->stream);
		local->nb -= 20;
	}
	return TRUE;
}

/* vertices */
gboolean max_cb_0x08FE_0x0914(MaxGlobalData *global, MaxLocalData *local)
{
	guint32 num;
	gint i, j;
	G3DObject *object;

	if(local->nb < 4)
		return FALSE;

	/* vertices */
	num = g3d_stream_read_int32_le(global->stream);
	local->nb -= 4;
	object = (G3DObject *)local->object;
	if(object == NULL) {
		g_warning("MAX: 0x08FE::0x0914: no object");
		return FALSE;
	}

	object->vertex_count = num;
	object->vertex_data = g_new0(gfloat, 3 * num);

	for(i = 0; i < num; i ++) {
		if(local->nb < 12)
			return FALSE;
		for(j = 0; j < 3; j ++)
			object->vertex_data[i * 3 + j] =
				g3d_stream_read_float_le(global->stream);
		local->nb -= 12;
	}
	return TRUE;
}

/* material group */
gboolean max_cb_0x0005_0x4000(MaxGlobalData *global, MaxLocalData *local)
{
	G3DMaterial *material;

	if(strcmp(global->subfile, "Scene") != 0)
		return FALSE;

	material = g3d_material_new();
	material->name = g_strdup_printf("0x4000 material @ 0x%08x",
		(guint32)g3d_stream_tell(global->stream));
	local->object = material;
	global->model->materials = g_slist_append(global->model->materials,
		material);
	return TRUE;
}

/* material name */
gboolean max_cb_0x4000_0x4001(MaxGlobalData *global, MaxLocalData *local)
{
	G3DMaterial *material = (G3DMaterial *)local->object;

	g_return_val_if_fail(material != NULL, FALSE);
	if(material->name)
		g_free(material->name);
	material->name = max_read_wchar(global->stream, local->nb / 2);
	local->nb = 0;

	return TRUE;
}

/* material color */
gboolean max_cb_0x4000_0x4030(MaxGlobalData *global, MaxLocalData *local)
{
	G3DMaterial *material = (G3DMaterial *)local->object;

	g_return_val_if_fail(material != NULL, FALSE);
	g_return_val_if_fail(local->nb >= 16, FALSE);

	material->r = g3d_stream_read_float_le(global->stream);
	material->g = g3d_stream_read_float_le(global->stream);
	material->b = g3d_stream_read_float_le(global->stream);
	material->a = g3d_stream_read_float_le(global->stream);
	local->nb -= 16;

	return TRUE;
}

