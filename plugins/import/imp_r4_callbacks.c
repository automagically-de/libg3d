#include <g3d/iff.h>
#include <g3d/read.h>
#include <g3d/material.h>

#include "imp_r4_chunks.h"

/* ?? */
gboolean r4_cb_DRE2(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* RGE1 */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

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

/* geometry or something */
gboolean r4_cb_RGE1(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gchar *name;

	/* ROBJ */
	name =
		(gchar *)g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

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

	local->object = g_strdup(buffer);

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
