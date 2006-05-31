#include <g3d/iff.h>
#include <g3d/material.h>
#include <g3d/read.h>

gboolean maya_cb_MESH(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	gint32 x1, x2, x3, x4, i;

	x1 = g3d_read_int16_be(global->f);
	x2 = g3d_read_int16_be(global->f);
	x3 = g3d_read_int16_be(global->f);
	x4 = g3d_read_int16_be(global->f);
	local->nb -= 8;

#if DEBUG > 0
	g_printerr("[Maya][MESH] %d %d %d %d @ 0x%08x\n", x1, x2, x3, x4,
		(unsigned int)ftell(global->f));
#endif

	if(x1 == 0x6369)
	{
		object = g_new0(G3DObject, 1);
		object->name = g_strdup_printf("MESH @ 0x%08x",
			(guint32)ftell(global->f) - 16);
		material = g3d_material_new();
		material->name = g_strdup("(default material)");
		object->materials = g_slist_append(object->materials, material);
		global->model->objects = g_slist_append(global->model->objects,
			object);

		object->vertex_count = x4 / 3;
		object->vertex_data = g_new0(gfloat, object->vertex_count * 3);

		for(i = 0; i < object->vertex_count; i ++)
		{
			object->vertex_data[i * 3 + 0] = g3d_read_float_be(global->f);
			object->vertex_data[i * 3 + 1] = g3d_read_float_be(global->f);
			object->vertex_data[i * 3 + 2] = g3d_read_float_be(global->f);
			local->nb -= 12;

#if DEBUG > 1
			if((i % 10) == 0)
				g_printerr("\n[Maya][MESH] ");
			g_printerr("%04.2f ", f);
#endif
		}

		x3 = g3d_read_int16_be(global->f);
#if DEBUG > 0
		g_printerr("[Maya][MESH] %d %d\n", x3, x4);
#endif
		x4 = g3d_read_int16_be(global->f);
		local->nb -= 4;

		for(i = 0; i < x4 / 3; i ++)
		{
			face = g_new0(G3DFace, 1);
			face->vertex_count = 3;
			face->vertex_indices = g_new0(guint32, 3);
			face->vertex_indices[0] = g3d_read_int32_be(global->f) & 0xFFFF;
			face->vertex_indices[1] = g3d_read_int32_be(global->f) & 0xFFFF;
			face->vertex_indices[2] = g3d_read_int32_be(global->f) & 0xFFFF;
			face->material = material;
			local->nb -= 12;

			object->faces = g_slist_append(object->faces, face);
		}
	}

	return TRUE;
}
