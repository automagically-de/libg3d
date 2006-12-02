#include <g3d/iff.h>
#include <g3d/read.h>
#include <g3d/material.h>

#include "imp_r4_chunks.h"

/* triangles */
gboolean r4_cb_DRE2(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	G3DObject *object;
	G3DFace *face;
	guint32 ntris, i;

	object = g_new0(G3DObject, 1);
	object->name = g_strdup("(default)");
	global->model->objects = g_slist_append(global->model->objects, object);
	local->object = object;

	/* RGE1 */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

#if DEBUG > 2
	printf("R4: DRE2 offset after RGE1 chunk: 0x%08x\n",
		(guint32)ftell(global->f));
#endif

	/* read triangles */
	ntris = g3d_read_int32_be(global->f);
	local->nb -= 4;
	for(i = 0; i < ntris; i ++)
	{
		face = g_new0(G3DFace, 1);
		face->material = g_slist_nth_data(global->model->materials, 0);
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		face->vertex_indices[0] = g3d_read_int32_be(global->f);
		face->vertex_indices[1] = g3d_read_int32_be(global->f);
		face->vertex_indices[2] = g3d_read_int32_be(global->f);
		local->nb -= 12;
		object->faces = g_slist_append(object->faces, face);
	}

	return TRUE;
}

/* material */
gboolean r4_cb_GMAT(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	G3DMaterial *material;
	gpointer object;

	/* ROBJ */
	object = g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	if(object)
	{
		material = g3d_material_new();
		material->name = g_convert((gchar *)object, -1,
			"UTF-8", "ISO-8859-1",
			NULL, NULL, NULL);
		g_free(object);

		if(local->object)
			((G3DObject *)local->object)->materials = g_slist_append(
				((G3DObject *)local->object)->materials, material);
		else
			global->model->materials = g_slist_append(global->model->materials,
				material);
	}

	/* skip remaining bytes */
	if(local->nb)
	{
		fseek(global->f, local->nb, SEEK_CUR);
		local->nb = 0;
	}

	return TRUE;
}

/* ?? */
gboolean r4_cb_LGH3(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* RGE1 */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	return TRUE;
}

/* points */
gboolean r4_cb_PKTM(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gint32 i;
	G3DObject *object;

	object = (G3DObject *)local->object;
	if(object)
	{
		object->vertex_count = g3d_read_int32_be(global->f);
		local->nb -= 4;
		object->vertex_data = g_new0(gfloat, object->vertex_count * 3);
		for(i = 0; i < object->vertex_count; i ++)
		{
			object->vertex_data[i * 3 + 0] = g3d_read_float_be(global->f);
			object->vertex_data[i * 3 + 1] = g3d_read_float_be(global->f);
			object->vertex_data[i * 3 + 2] = g3d_read_float_be(global->f);
			local->nb -= 12;
		}
	}

	return TRUE;
}

/* geometry or something */
gboolean r4_cb_RGE1(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gchar *name;

	/* ROBJ */
	name =
		(gchar *)g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	if(name && local->object)
	{
		g_free(((G3DObject *)local->object)->name);
		((G3DObject *)local->object)->name = g_strdup(name);
	}

	/* KSYS */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	/* PKTM */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	/* skip remaining bytes */
	if(local->nb)
	{
		fseek(global->f, local->nb, SEEK_CUR);
		local->nb = 0;
	}

	return TRUE;
}

/* camera related */
gboolean r4_cb_RKA2(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* handle RGE1 chunk */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	/* more stuff... */
	/* TODO: */

	return TRUE;
}

/* object name */
gboolean r4_cb_ROBJ(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gchar buffer[512];
	gint32 len;

	len = g3d_read_int16_be(global->f);
	local->nb -= 2;

	fread(buffer, 1, len, global->f);
	local->nb -= len;
	buffer[len] = '\0';

	local->level_object = g_strdup(buffer);

#if DEBUG > 0
	g_printerr("[R4] ROBJ: %s\n", buffer);
#endif

	return TRUE;
}

/* surface */
gboolean r4_cb_SURF(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* GMAT */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	return TRUE;
}

/* texture material */
gboolean r4_cb_TXM1(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* SURF */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	return TRUE;
}

/* texture object? */
gboolean r4_cb_TXO1(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* RGE1 */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	return TRUE;
}
