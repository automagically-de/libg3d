#include <string.h>

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

	if(local->object == NULL)
		return TRUE;

	if(strcmp(local->entity, "3DFACE") == 0) {
		face = g_new0(G3DFace, 1);
		face->material = local->material;
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		local->vertex_offset = local->object->vertex_count;
		face->vertex_indices[0] = local->vertex_offset;
		face->vertex_indices[1] = local->vertex_offset + 1;
		face->vertex_indices[2] = local->vertex_offset + 2;
		local->object->vertex_count += 3;
		local->object->vertex_data = g_realloc(local->object->vertex_data,
			local->object->vertex_count * 3 * sizeof(gfloat));
		local->object->faces = g_slist_append(local->object->faces, face);
		local->face = face;
	}

	return TRUE;
}

gboolean dxf_pnt_coord(DxfGlobalData *global, DxfLocalData *local)
{
	guint32 index, coord;
	gdouble dbl;

	dbl = dxf_read_float64(global);

	if((local->object == NULL) || (local->face == NULL))
		return TRUE;

	index = local->id % 10 + local->vertex_offset;
	coord = local->id / 10 - 1;

#if DEBUG > 0
	g_debug("|  [pnt] (%d * 3 + %d) = %f", index, coord, dbl);
#endif

	/* quad instead of triangle, resize */
	if(local->id == 13) {
		local->face->vertex_count = 4;
		local->face->vertex_indices = g_realloc(local->face->vertex_indices,
			4 * sizeof(guint32));
		local->face->vertex_indices[3] = local->vertex_offset + 3;
		local->object->vertex_count ++;
		local->object->vertex_data = g_realloc(local->object->vertex_data,
			local->object->vertex_count * 3 * sizeof(gfloat));
	}

	local->object->vertex_data[index * 3 + coord] = dbl;

	return TRUE;
}
