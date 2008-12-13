#include <string.h>

#include <g3d/face.h>

#include "imp_dxf.h"
#include "imp_dxf_callbacks.h"

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

gboolean dxf_grpcode_0(DxfGlobalData *global, DxfLocalData *local)
{
	G3DFace *face;
	G3DObject *object = local->edata->object;

	if(object == NULL)
		return TRUE;

	if(strcmp(local->entity, "3DFACE") == 0) {
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
	}

	return TRUE;
}

gboolean dxf_grpcode_70(DxfGlobalData *global, DxfLocalData *local)
{
	local->edata->tmp_70 = dxf_read_int16(global);
	return TRUE;
}

gboolean dxf_grpcode_71(DxfGlobalData *global, DxfLocalData *local)
{
	local->edata->tmp_71 = dxf_read_int16(global);
	return TRUE;
}

gboolean dxf_grpcode_72(DxfGlobalData *global, DxfLocalData *local)
{
	G3DObject *object = local->edata->object;
	G3DFace *face;
	guint32 width, height;
	gint32 x, y;

	height = dxf_read_int16(global);
	width = local->edata->tmp_71;

	if(object == NULL)
		return TRUE;

	if(strcmp(local->entity, "POLYLINE") == 0) {
#if DEBUG > 0
		g_debug("|  POLYLINE: %d x %d (0x%08x)", width, height,
			local->edata->tmp_70);
#endif

		if(!(local->edata->tmp_70 & DXF_POLY_3D_POLYMESH))
			return TRUE;

		local->edata->vertex_offset = object->vertex_count;
		object->vertex_count += width * height;
		object->vertex_data = g_realloc(object->vertex_data,
			object->vertex_count * 3 * sizeof(gfloat));
		memset(object->vertex_data +
			local->edata->vertex_offset * 3,
			0, width * height * 3 * sizeof(gfloat));
		local->edata->tmp_71 = 0;
		local->edata->tmp_i1 = 0; /* vertex counter */

		for(y = 0; y < (height - 1); y ++)
			for(x = 0; x < (width - 1); x ++) {
				face = g3d_face_new_tri(local->edata->material,
					local->edata->vertex_offset + (y * width) + x,
					local->edata->vertex_offset + (y * width) + x + 1,
					local->edata->vertex_offset + ((y + 1) * width) + x);
				object->faces = g_slist_append(object->faces, face);

				face = g3d_face_new_tri(local->edata->material,
					local->edata->vertex_offset + ((y + 1) * width) + x,
					local->edata->vertex_offset + (y * width) + x + 1,
					local->edata->vertex_offset + ((y + 1) * width) + x + 1);
				object->faces = g_slist_append(object->faces, face);
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
	is_vertex = (strcmp(local->entity, "VERTEX") == 0);

	if(object == NULL)
		return TRUE;

	if(face == NULL)
		if(!is_vertex)
			return TRUE;

	index = local->id % 10 + local->edata->vertex_offset;
	coord = local->id / 10 - 1;

	if(is_vertex) {
		if(!(local->edata->tmp_70 & DXF_POLY_3D_POLYMESH))
			return TRUE;
		index += local->edata->tmp_i1;
		if(local->id == 30)
			local->edata->tmp_i1 ++;
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
