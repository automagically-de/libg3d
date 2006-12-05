#include <g3d/iff.h>
#include <g3d/read.h>
#include <g3d/material.h>
#include <g3d/matrix.h>
#include <g3d/vector.h>

#include "imp_r4_chunks.h"

static void hexdump(FILE *f, guint32 len, gchar *prefix)
{
	guint32 i;
	guint8 byte;

	for(i = 0; i < len; i ++)
	{
		byte = g3d_read_int8(f);
		if(((i % 16) == 0) && (i != 0))
			printf("\n");
		if(((i % 16) == 0) && (i != (len - 1)))
			printf("%s: %06x: ", (prefix ? prefix : ""), i);
		printf("%02x", byte);
		if((i % 4) == 3)
			printf(" ");
	}
	printf("\n");
}

static void dump_remaining(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
#if DEBUG > 0
	gchar *id, *prefix;

	id = g3d_iff_id_to_text(local->id);
	prefix = g_strdup_printf("R4: %s", id);

	if(local->nb > 0)
	{
		printf("R4: %s: %d bytes remaining @ 0x%08x\n", id, local->nb,
			(guint32)ftell(global->f));
		hexdump(global->f, local->nb, prefix);
		local->nb = 0;
	}

	g_free(prefix);
	g_free(id);
#endif
}

#if DEBUG > 0
static void flagstat_register(guint8 flags, guint32 *flagstats)
{
	guint32 i;

	for(i = 0; i < 8; i ++)
		if(flags & (1 << i))
			flagstats[i] ++;
}
#endif

/* triangles */
gboolean r4_cb_DRE2(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	G3DObject *object;
	G3DFace *face;
	guint32 ntris, i;
	guint8 u, max_u = 0;
#if DEBUG > 0
	guint32 flagstats[8];
#endif

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
#if DEBUG > 0
	printf("R4: DRE2: %d triangles\n", ntris);
#endif
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

	/* flags/index of material? */
	for(i = 0; i < 8; i ++)
		flagstats[i] = 0;

	for(i = 0; i < ntris; i ++)
	{
		u = g3d_read_int8(global->f);
		local->nb --;
		if(u > max_u)
			max_u = u;
#if DEBUG > 0
		flagstat_register(u, flagstats);
#endif
	}

#if DEBUG > 0
	for(i = 0; i < 8; i ++)
		printf("R4: DRE2: flag 2^%d: %u\n", i, flagstats[i]);
#endif

#if DEBUG > 0
	printf("R4: DRE2: max. flag: %d\n", max_u);
#endif

	dump_remaining(global, local);

	/* remove transformation if set */
	if(object->transformation)
	{
		g_free(object->transformation);
		object->transformation = NULL;
	}

	return TRUE;
}

/* material: GMAT, GMA0 */
gboolean r4_cb_GMAx(g3d_iff_gdata *global, g3d_iff_ldata *local)
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

		if(local->id == G3D_IFF_MKID('G','M','A','T'))
		{
			material->r = g3d_read_float_be(global->f);
			material->g = g3d_read_float_be(global->f);
			material->b = g3d_read_float_be(global->f);
			local->nb -= 12;
		}
	}

	dump_remaining(global, local);

	return TRUE;
}

/* coordinate system */
gboolean r4_cb_KSYS(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gfloat x, y, z, f;
	G3DObject *object;
	G3DTransformation *transform;
	gint32 i, j;

	x = g3d_read_float_be(global->f);
	y = g3d_read_float_be(global->f);
	z = g3d_read_float_be(global->f);
	local->nb -= 12;

#if DEBUG > 2
	printf("R4: KSYS: %f, %f, %f\n", x, y, z);
#endif

	object = (G3DObject *)local->object;
	if(object)
	{
		transform = g_new0(G3DTransformation, 1);
		g3d_matrix_identity(transform->matrix);
		g3d_matrix_translate(x, y, z, transform->matrix);

		object->transformation = transform;

		/* matrix parts */
		for(j = 0; j < 3; j ++)
			for(i = 0; i < 3; i ++)
				transform->matrix[j * 4 + i] = g3d_read_float_be(global->f);
		local->nb -= 36;

		/* scale part */
		f = g3d_read_float_be(global->f);
		local->nb -= 4;
		g3d_matrix_scale(f, f, f, transform->matrix);
	}

	return TRUE;
}

/* sphere */
gboolean r4_cb_KUG1(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* RGE1 */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	dump_remaining(global, local);

	return TRUE;
}

/* light */
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

			/* transform vertices */
			if(object->transformation)
			{
				g3d_vector_transform(
					object->vertex_data + i * 3 + 0,
					object->vertex_data + i * 3 + 1,
					object->vertex_data + i * 3 + 2,
					object->transformation->matrix);
			}
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
		((G3DObject *)local->object)->name = g_convert(name, -1,
			"UTF-8", "ISO-8859-1", NULL, NULL, NULL);
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

	dump_remaining(global, local);

	return TRUE;
}

/* camera related: RKA2, RKA3 */
gboolean r4_cb_RKAx(g3d_iff_gdata *global, g3d_iff_ldata *local)
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

#if DEBUG > 2
	g_printerr("[R4] ROBJ: %s\n", buffer);
#endif

	return TRUE;
}

/* surface: SURF, SUR1 */
gboolean r4_cb_SURx(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* GMAT or GMA1 */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	dump_remaining(global, local);

	return TRUE;
}

/* texture material */
gboolean r4_cb_TXM1(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* SURF */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	dump_remaining(global, local);

	return TRUE;
}

/* texture object? */
gboolean r4_cb_TXO1(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	/* RGE1 */
	g3d_iff_handle_chunk(global, local, r4_chunks, G3D_IFF_PAD1);

	dump_remaining(global, local);

	return TRUE;
}
