#include <string.h>

#include <g3d/face.h>
#include <g3d/primitive.h>

#include "imp_dxf.h"
#include "imp_dxf_callbacks.h"
#include "imp_dxf_vars.h"
#include "imp_dxf_def.h"

#define DXF_DEBUG_VALUES 0

gboolean dxf_debug_i16(DxfGlobalData *global, DxfLocalData *local)
{
	gint32 i;

	i = dxf_read_int16(global);
#if DEBUG > DXF_DEBUG_VALUES
	g_debug("|  [i16] %i", i);
#endif
	return TRUE;
}

gboolean dxf_debug_i32(DxfGlobalData *global, DxfLocalData *local)
{
	gint32 i;

	i = dxf_read_int32(global);
#if DEBUG > DXF_DEBUG_VALUES
	g_debug("|  [i32] %i", i);
#endif
	return TRUE;
}

gboolean dxf_debug_dbl(DxfGlobalData *global, DxfLocalData *local)
{
	gdouble d;

	d = dxf_read_float64(global);
#if DEBUG > DXF_DEBUG_VALUES
	g_debug("|  [dbl] %f", d);
#endif
	return TRUE;
}

gboolean dxf_debug_str(DxfGlobalData *global, DxfLocalData *local)
{
	gchar str[DXF_MAX_LINE + 1];

	dxf_read_string(global, str);
#if DEBUG > 0
	g_debug("|  [str] %s", str);
#endif
	return TRUE;
}

static gboolean dxf_str_in_array(gchar **array, const gchar *needle)
{
	gchar **p = array;
	while(*p != NULL) {
		if(strcmp(*p, needle) == 0)
			return TRUE;
		p ++;
	}
	return FALSE;
}

#define DXF_VAR_DEBUG 2

gboolean dxf_debug_var(DxfGlobalData *global, DxfLocalData *local)
{
	gint32 key;
	gint16 i16;
	gchar str[DXF_MAX_LINE + 1], strval[DXF_MAX_LINE + 1];
	gdouble x, y, z;

	dxf_read_string(global, str);
	if(dxf_str_in_array(dxf_vars_vector3d, str)) {
		key = dxf_read_code(global); /* 10 */
		x = dxf_read_float64(global);
		key = dxf_read_code(global); /* 20 */
		y = dxf_read_float64(global);
		key = dxf_read_code(global); /* 30 */
		z = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
		g_debug("DXF: [v3d] %s: %.2f, %.2f, %.2f", str, x, y, z);
#endif
	} else if(dxf_str_in_array(dxf_vars_vector2d, str)) {
		key = dxf_read_code(global); /* 10 */
		x = dxf_read_float64(global);
		key = dxf_read_code(global); /* 20 */
		y = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
		g_debug("DXF: [v2d] %s: %.2f, %.2f", str, x, y);
#endif
	} else {
		key = dxf_read_code(global);
		switch(key) {
			case 1: case 2: case 3: case 4:
			case 5: case 6: case 7: case 8:
				/* string */
				dxf_read_string(global, strval);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [str] %s: %s", str, strval);
#endif
				break;
			case 40:
			case 50:
				x = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [dbl] %s: %.2f", str, x);
#endif
				break;
			case 62:
			case 70:
			case 280:
			case 290: /* FIXME: boolean */
			case 370:
			case 380:
				/* 16-bit integer */
				i16 = dxf_read_int16(global);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [i16] %s: %d", str, i16);
#endif
				break;
			default:
				DXF_HANDLE_UNKNOWN(global, key, strval, "** VARIABLE **");
				break;
		}
	}
	return TRUE;
}


gboolean dxf_grpcode_0(DxfGlobalData *global, DxfLocalData *local)
{
	G3DFace *face;
	G3DObject *object = local->edata->object;

	if(object == NULL)
		return TRUE;

	if(local->eid == DXF_E_3DFACE) {
		object = g_slist_nth_data(global->model->objects, 0);
		local->edata->object = object;
		local->edata->polyline_flags = 0;

		face = g_new0(G3DFace, 1);
		face->material = local->edata->material;
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		local->edata->vertex_offset = object->vertex_count;
		face->vertex_indices[0] = local->edata->vertex_offset;
		face->vertex_indices[1] = local->edata->vertex_offset + 1;
		face->vertex_indices[2] = local->edata->vertex_offset + 2;
		object->vertex_count += 3;
		object->vertex_data = g_realloc(object->vertex_data,
			object->vertex_count * 3 * sizeof(gfloat));
		object->faces = g_slist_append(object->faces, face);
		local->edata->face = face;
	} else if(local->eid == DXF_E_POLYLINE) {
		local->edata->face = NULL;
	}

	return TRUE;
}

gboolean dxf_grpcode_70(DxfGlobalData *global, DxfLocalData *local)
{
	local->edata->tmp_70 = dxf_read_int16(global);
	if(local->eid == DXF_E_POLYLINE)
		local->edata->polyline_flags = local->edata->tmp_70;
#if DEBUG > 0
	g_debug("|  [70]: 0x%08x (%d)",
		local->edata->tmp_70, local->edata->tmp_70);
#endif
	return TRUE;
}

gboolean dxf_grpcode_71(DxfGlobalData *global, DxfLocalData *local)
{
	G3DObject *object;
	G3DFace *face;

	local->edata->tmp_71 = dxf_read_int16(global);
	object = local->edata->object;
	if(object == NULL)
		return TRUE;

	if((local->eid == DXF_E_POLYLINE) &&
		(local->edata->polyline_flags & DXF_POLY_POLYFACE)) {
		object = g_new0(G3DObject, 1);
		object->name = g_strdup_printf("Poly Face @ line %d",
			g3d_stream_line(global->stream));
		object->vertex_count = local->edata->tmp_71;
		object->vertex_data = g_new0(gfloat, object->vertex_count * 3);
		global->model->objects = g_slist_append(global->model->objects,
			object);
		local->edata->object = object;
		local->edata->vertex_offset = 0;
	} else if(local->eid == DXF_E_VERTEX) {
		if((local->edata->polyline_flags & DXF_POLY_POLYFACE) &&
			(local->edata->tmp_70 & 128)) {
			/* vertex has face */
			face = g_new0(G3DFace, 1);
			face->material = local->edata->material;
			face->vertex_count = 3;
			face->vertex_indices = g_new0(guint32, face->vertex_count);
			face->vertex_indices[0] = ABS(local->edata->tmp_71) - 1;
			local->edata->face = face;
			object->faces = g_slist_prepend(object->faces, face);
		}
	}
	return TRUE;
}

gboolean dxf_grpcode_72(DxfGlobalData *global, DxfLocalData *local)
{
	G3DObject *object = local->edata->object;
	gint32 width, height;

	width = dxf_read_int16(global);
	height = local->edata->tmp_71;

	if(object == NULL)
		return TRUE;

	if(local->eid == DXF_E_POLYLINE) {
#if DEBUG > 0
		g_debug("|  POLYLINE: %d x %d (0x%08x)", width, height,
			local->edata->polyline_flags);
#endif

		if(local->edata->polyline_flags & DXF_POLY_3D_POLYMESH) {
			object = g3d_primitive_mesh(width, height,
				(local->edata->polyline_flags & DXF_POLY_CLOSED),
				(local->edata->polyline_flags & DXF_POLY_N_CLOSED),
				local->edata->material);
			object->name = g_strdup_printf("3D Poly Mesh @ line %d",
				g3d_stream_line(global->stream));
			local->edata->object = object;
			global->model->objects = g_slist_append(global->model->objects,
				object);

			local->edata->vertex_offset = 0;
			local->edata->tmp_71 = 0;
			local->edata->tmp_i1 = 0; /* vertex counter */
		}
	} else if (local->eid == DXF_E_VERTEX) {
		if(local->edata->face &&
			(local->edata->polyline_flags & DXF_POLY_POLYFACE)) {
			local->edata->face->vertex_indices[1] = ABS(width) - 1;
		}
	}
	return TRUE;
}

gboolean dxf_grpcode_73(DxfGlobalData *global, DxfLocalData *local)
{
	gint32 i;

	i = dxf_read_int16(global);
	if(local->eid == DXF_E_VERTEX) {
		if(local->edata->face &&
			(local->edata->polyline_flags & DXF_POLY_POLYFACE)) {
			local->edata->face->vertex_indices[2] = ABS(i) - 1;
		}
	}
	return TRUE;
}

gboolean dxf_grpcode_74(DxfGlobalData *global, DxfLocalData *local)
{
	gint32 i;

	i = dxf_read_int16(global);
	if(local->eid == DXF_E_VERTEX) {
		if(local->edata->face &&
			(local->edata->polyline_flags & DXF_POLY_POLYFACE)) {
			local->edata->face->vertex_count ++;
			local->edata->face->vertex_indices = g_realloc(
				local->edata->face->vertex_indices, 4 * sizeof(guint32));
			local->edata->face->vertex_indices[3] = ABS(i) - 1;
		}
	}
	return TRUE;
}

gboolean dxf_pnt_coord(DxfGlobalData *global, DxfLocalData *local)
{
	G3DFace *face = local->edata->face;
	G3DObject *object = local->edata->object;
	guint32 index, coord;
	gdouble dbl;
	gboolean is_vertex;

	dbl = dxf_read_float64(global);
	is_vertex = (local->eid == DXF_E_VERTEX);

	if(object == NULL)
		return TRUE;

	if(face == NULL)
		if(!is_vertex)
			return TRUE;

	index = local->id % 10 + local->edata->vertex_offset;
	coord = local->id / 10 - 1;

	if(is_vertex) {
		index += local->edata->tmp_i1;
		if(index >= object->vertex_count)
			return TRUE;
		if(local->id == 30)
			local->edata->tmp_i1 ++;
		if(!(local->edata->tmp_70 & 64))
			return TRUE;
	}

#if DEBUG > 0
	g_debug("|  [pnt] (%d * 3 + %d) = %f", index, coord, dbl);
#endif

	/* quad instead of triangle, resize */
	if(local->id == 13) {
		face->vertex_count = 4;
		face->vertex_indices = g_realloc(face->vertex_indices,
			4 * sizeof(guint32));
		face->vertex_indices[3] = local->edata->vertex_offset + 3;
		object->vertex_count ++;
		object->vertex_data = g_realloc(object->vertex_data,
			object->vertex_count * 3 * sizeof(gfloat));
	}

	g_return_val_if_fail(index < object->vertex_count, FALSE);

	object->vertex_data[index * 3 + coord] = dbl;

	return TRUE;
}
