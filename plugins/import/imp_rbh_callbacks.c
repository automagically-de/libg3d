#include <string.h>

#include <g3d/iff.h>
#include <g3d/read.h>
#include <g3d/material.h>

static gchar *padding = "                    ";

/* header */
gboolean rbh_cb_RBHH(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	guint32 x0, x1, x2;
	guint32 i, num;

	num = local->nb / 12;
	for(i = 0; i < num; i ++)
	{
		x0 = g3d_read_int32_le(global->f);
		x1 = g3d_read_int32_le(global->f);
		x2 = g3d_read_int32_le(global->f);
		local->nb -= 12;

		g_debug("[RBH][RBHH] %d: 0x%08x 0x%08x 0x%08x", i + 1, x0, x1, x2);
	}

	return TRUE;
}

/* body */
gboolean rbh_cb_BODY(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	guint32 nverts, nfaces, vertoff = 0;
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;

	if(local->nb < 4) return TRUE; /* zero size BODY tags? */


	nverts = g3d_read_int16_le(global->f);

	nfaces = g3d_read_int16_le(global->f);
	local->nb -= 4;

	g_debug(
		"%s[RBH][BODY] %d verts, %d faces, %d bytes remaining (%d x 4 + %d)",
		padding + (strlen(padding) - local->level) + 2,
		nverts, nfaces, local->nb,
		local->nb / 4, local->nb % 4);

	if(nverts == 0) return TRUE; /* skip for now */

	object = g_new0(G3DObject, 1);
	object->name = g_strdup("BODY");

	material = g3d_material_new();
	object->materials = g_slist_append(object->materials, material);

	global->model->objects = g_slist_append(global->model->objects, object);

	return TRUE;
}
