#include <string.h>
#include <g3d/material.h>
#include <g3d/stream.h>
#include <g3d/face.h>

#include "imp_msfsmdl_bgl.h"

typedef struct {
	guint16 id;
	gsize dlen;
	gchar *description;
	G3DIffChunkCallback callback;
} BglOpCode;

gboolean msfsmdl_bgl_cb_call(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_call_32(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_case(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_crash_octree(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_draw_trilist(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_draw_linelist(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_ifin1(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_ifin2(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_ifmask(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_jump(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_jump_32(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_material_list(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_mouserect_list(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_return(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_texture_list(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_vertex_list(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_bgl_cb_vinstance_call(G3DIffGlobal *global, G3DIffLocal *local);

static BglOpCode bgl_opcodes[] = {
	{ 0x0000,    0, "EOF" },
	{ 0x0001,    0, "LOGOL" },
	{ 0x0003,    0, "CASE", msfsmdl_bgl_cb_case },
	{ 0x0005,    0, "SURFACE" },
	{ 0x0006,    6, "SPNT" },
	{ 0x000c,    0, "GCLOSURE" },
	{ 0x000d,    0, "JUMP", msfsmdl_bgl_cb_jump },
	{ 0x0010,    2, "CNTRES" },
	{ 0x0012,    2, "GSTRRES" },
	{ 0x0016,    0, "DBG_DEBUG" },
	{ 0x001c,    0, "IFIN2", msfsmdl_bgl_cb_ifin2 },
	{ 0x0020,   34, "FACET4_TMAP" },
	{ 0x0022,    0, "RETURN", msfsmdl_bgl_cb_return },
	{ 0x0023,    0, "CALL", msfsmdl_bgl_cb_call },
	{ 0x0024,    0, "IFIN1", msfsmdl_bgl_cb_ifin1 },
	{ 0x0026,    4, "SETWRD" },
	{ 0x002e,    4, "LCOLOR24" },
	{ 0x0034,    8, "SUPER_SCALE" },
	{ 0x0039,    0, "IFMASK", msfsmdl_bgl_cb_ifmask },
	{ 0x003a,   10, "VPOSITION" },
	{ 0x003b,    0, "VINSTANCE_CALL", msfsmdl_bgl_cb_vinstance_call },
	{ 0x0040,    2, "SHADOW_VPOSITION" },
	{ 0x0041,    4, "SHADOW_VICALL" },
	{ 0x0046,   20, "POINT_VICALL" },
	{ 0x004a,   10, "LANDING_LIGHTS" },
	{ 0x004c,   14, "VSCALE" },
	{ 0x005b,    2, "INDIRECT_CALL" },
	{ 0x0088,    4, "JUMP_32", msfsmdl_bgl_cb_jump_32 },
	{ 0x0089,    4, "VAR_BASE_32" },
	{ 0x008a,    4, "CALL_32", msfsmdl_bgl_cb_call_32 },
	{ 0x008e,    2, "VFILE_MARKER" },
	{ 0x0096,    4, "CRASH_START" },
	{ 0x009e,   18, "INTERPOLATE" },
	{ 0x009f,    4, "OVERRIDE" },
	{ 0x009a,   18, "TILED_ELEVATION_MAP" },
	{ 0x00ac,    2, "ZBIAS" },
	{ 0x00ad,   28, "ANIMATE" },
	{ 0x00ae,    0, "TRANSFORM_END" },
	{ 0x00af,   48, "TRANSFORM_MAT" },
	{ 0x00b0,    0, "CRASH_OCTTREE", msfsmdl_bgl_cb_crash_octree },
	{ 0x00b2,   42, "LIGHT" },
	{ 0x00b3,   14, "IFINF1" },
	{ 0x00b5,    0, "VERTEX_LIST", msfsmdl_bgl_cb_vertex_list },
	{ 0x00b6,    0, "MATERIAL_LIST", msfsmdl_bgl_cb_material_list },
	{ 0x00b7,    0, "TEXTURE_LIST", msfsmdl_bgl_cb_texture_list },
	{ 0x00b8,    4, "SET_MATERIAL" },
	{ 0x00b9,    0, "DRAW_TRILIST", msfsmdl_bgl_cb_draw_trilist },
	{ 0x00ba,    0, "DRAW_LINELIST", msfsmdl_bgl_cb_draw_linelist },
	{ 0x00bc,    4, "BEGIN" },
	{ 0x00bf,    0, "MOUSERECT_LIST", msfsmdl_bgl_cb_mouserect_list },
	{ 0x00bd,    0, "END" },
	{ 0x00c0,    2, "SET_MOUSERECT" },
	{ 0x00c1,   14, "SET_MATERIAL_ANIMATE" },
	{ 0x00c2,   24, "MODWORD" },

	{ 0xffff,    0, NULL },
};

typedef struct {
	goffset base_address;
	GQueue *call_stack;
	GHashTable *vars;
	G3DObject *current_object;
} BglState;

gboolean msfsmdl_parse_bgl(G3DIffGlobal *global, G3DIffLocal *local) {
	guint32 i;
	guint16 opcode;
	gsize dlen;
	BglState *state;

	if (!global->user_data) {
		state = g_new0(BglState, 1);
		state->base_address = g3d_stream_tell(global->stream);
		state->call_stack = g_queue_new();
		state->vars = g_hash_table_new(g_int_hash, g_int_equal);
		global->user_data = state;
	}
	else {
		state = global->user_data;
	}

	while (TRUE) {
		goffset pos = g3d_stream_tell(global->stream) - state->base_address;
		if (pos >= local->nb) {
			g_warning("out of chunk %u/%u", pos, local->nb);
			return TRUE;
		}

		opcode = g3d_stream_read_int16_le(global->stream);

		i = 0;
		while (bgl_opcodes[i].id != 0xffff && bgl_opcodes[i].id != opcode)
			i ++;

		if (bgl_opcodes[i].id == 0xffff) {
			g_error("failed to find opcode 0x%04x @ 0x%08x", opcode,
				g3d_stream_tell(global->stream) - 2);
			return FALSE;
		}

		dlen = bgl_opcodes[i].dlen;
		g_debug("\\[0x%04x] %s (%u bytes @ 0x%08x)", opcode,
			bgl_opcodes[i].description, dlen,
			g3d_stream_tell(global->stream) - 2);

		if (bgl_opcodes[i].callback) {
			bgl_opcodes[i].callback(global, local);
		}
		else {
			g3d_stream_skip(global->stream, dlen);
		}

		if (opcode == 0x0000) { /* EOF */
			g_debug("got EOF");
			break;
		}
	}
	return TRUE;
}

static gint16 bgl_get_var(G3DIffGlobal *global, gint32 avar) {
	BglState *state = global->user_data;
	gpointer valp;
	goffset saved_offset;
	gint16 val;

	if (g_hash_table_lookup_extended(state->vars, &avar, NULL, &valp)) {
		return GPOINTER_TO_INT(valp);
	}

	saved_offset = g3d_stream_tell(global->stream);
	g3d_stream_seek(global->stream, state->base_address + avar, G_SEEK_SET);
	val = g3d_stream_read_int16_le(global->stream);
	g3d_stream_seek(global->stream, saved_offset, G_SEEK_SET);

	g_hash_table_insert(state->vars, &avar, GINT_TO_POINTER(val));

	return val;
}

gboolean msfsmdl_bgl_cb_call(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;

	gint16 addr = g3d_stream_read_int16_le(global->stream);
	goffset curr = g3d_stream_tell(global->stream);
	goffset dest = curr + addr - 4;

	g_queue_push_head(state->call_stack, GSIZE_TO_POINTER(curr));

	g_debug("| CALL %p from %p (offset %i)", dest, curr, addr);
	g3d_stream_seek(global->stream, dest, G_SEEK_SET);

	return TRUE;
}

gboolean msfsmdl_bgl_cb_call_32(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;

	gint32 addr = g3d_stream_read_int32_le(global->stream);
	goffset curr = g3d_stream_tell(global->stream);
	goffset dest = curr + addr - 6;

	g_queue_push_head(state->call_stack, GSIZE_TO_POINTER(curr));

	g_debug("| CALL %p from %p (offset %i)", dest, curr, addr);
	g3d_stream_seek(global->stream, dest, G_SEEK_SET);

	return TRUE;
}

gboolean msfsmdl_bgl_cb_ifin2(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;
	gint16 addr = g3d_stream_read_int16_le(global->stream);
	gint16 var1 = g3d_stream_read_int16_le(global->stream);
	gint16 v1_l = g3d_stream_read_int16_le(global->stream);
	gint16 v1_h = g3d_stream_read_int16_le(global->stream);
	gint16 var2 = g3d_stream_read_int16_le(global->stream);
	gint16 v2_l = g3d_stream_read_int16_le(global->stream);
	gint16 v2_h = g3d_stream_read_int16_le(global->stream);

	g_debug("| IFIN2: %i (%i <= %i <= %i && %i <= %i <= %i)",
		addr,
		v1_l, var1, v1_h,
		v2_l, var2, v2_h);

	if (!(var1 >= v1_l && var1 <= v1_h) ||
		!(var2 >= v2_l && var2 <= v2_h)) {
		goffset curr = g3d_stream_tell(global->stream);
		goffset dest = curr + addr - 16;

		g_debug("| IFIN2: jump to %x (%i)", curr, addr);

		g3d_stream_seek(global->stream, dest, G_SEEK_SET);
	}

	return TRUE;
}

gboolean msfsmdl_bgl_cb_ifin1(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;
	gint16 addr = g3d_stream_read_int16_le(global->stream);
	gint16 var1 = g3d_stream_read_int16_le(global->stream);
	gint16 v1_l = g3d_stream_read_int16_le(global->stream);
	gint16 v1_h = g3d_stream_read_int16_le(global->stream);

	g_debug("| IFIN1: %i (%i <= %i <= %i)",
		addr,
		v1_l, var1, v1_h);

	if (!(var1 >= v1_l && var1 <= v1_h)) {
		goffset curr = g3d_stream_tell(global->stream);
		goffset dest = curr + addr - 10;

		g_debug("| IFIN1: jump to %x (%i)", curr, addr);

		g3d_stream_seek(global->stream, dest, G_SEEK_SET);
	}

	return TRUE;
}

gboolean msfsmdl_bgl_cb_ifmask(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;
	gint16 addr = g3d_stream_read_int16_le(global->stream);
	gint16 avar = g3d_stream_read_int16_le(global->stream);
	guint16 mask = g3d_stream_read_int16_le(global->stream);
	guint16 vvar = bgl_get_var(global, avar);

	g_debug("| IFMASK !%i@%i & %i (to %i): %s", vvar, avar, mask, addr,
		(vvar & mask ? "no jump" : "jump"));

	if (!(vvar & mask)) {
		goffset curr = g3d_stream_tell(global->stream);
		goffset dest = curr + addr - 8;

		g3d_stream_seek(global->stream, dest, G_SEEK_SET);
	}

	return TRUE;
}

gboolean msfsmdl_bgl_cb_jump(G3DIffGlobal *global, G3DIffLocal *local) {
	gint16 addr = g3d_stream_read_int16_le(global->stream);
	goffset curr = g3d_stream_tell(global->stream);
	goffset dest = curr + addr - 4;

	g_debug("| JUMP to %p from %p (offset %i)", dest, curr, addr);
	g3d_stream_seek(global->stream, dest, G_SEEK_SET);

	return TRUE;
}

gboolean msfsmdl_bgl_cb_jump_32(G3DIffGlobal *global, G3DIffLocal *local) {
	gint32 addr = g3d_stream_read_int32_le(global->stream);
	goffset curr = g3d_stream_tell(global->stream);
	goffset dest = curr + addr - 6;

	g_debug("| JUMP_32 to %p from %p (offset %i)", dest, curr, addr);
	g3d_stream_seek(global->stream, dest, G_SEEK_SET);

	return TRUE;
}

gboolean msfsmdl_bgl_cb_return(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;
	goffset curr = g3d_stream_tell(global->stream);
	goffset dest;

	if (g_queue_is_empty(state->call_stack)) {
		g_warning("RETURN with empty call stack @ %p", curr);
		return FALSE;
	}

	dest = GPOINTER_TO_SIZE(g_queue_pop_head(state->call_stack));
	g_debug("| RETURN to %p from %p", dest, curr);
	g3d_stream_seek(global->stream, dest, G_SEEK_SET);

	return TRUE;
}

gboolean msfsmdl_bgl_cb_case(G3DIffGlobal *global, G3DIffLocal *local) {
	guint32 n_elems;

	n_elems = g3d_stream_read_int16_le(global->stream);
	g3d_stream_read_int16_le(global->stream); /* var */
	g3d_stream_read_int16_le(global->stream); /* fall-through */

	g3d_stream_skip(global->stream, n_elems * 2);

	return TRUE;
}

gboolean msfsmdl_bgl_cb_crash_octree(G3DIffGlobal *global, G3DIffLocal *local) {
	guint32 n_elems;
	n_elems = g3d_stream_read_int16_le(global->stream);

	g3d_stream_skip(global->stream, n_elems - 2);

	return TRUE;
}

gboolean msfsmdl_bgl_cb_vertex_list(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;
	guint32 n_elems, i, j;
	G3DObject *object;

	object = g_new0(G3DObject, 1);
	object->name = g_strdup_printf("object 0x%08x",
		g3d_stream_tell(global->stream) - 2 - state->base_address);

	global->model->objects = g_slist_append(global->model->objects, object);
	state->current_object = object;

	n_elems = g3d_stream_read_int16_le(global->stream);
	g3d_stream_read_int32_le(global->stream);

	object->vertex_count = n_elems;
	object->vertex_data = g_new0(G3DVector, n_elems * 3);

	for (i = 0; i < n_elems; i ++) {
		/* x, y, z */
		for (j = 0; j < 3; j ++)
			object->vertex_data[i * 3 + j] = g3d_stream_read_float_le(global->stream);

		/* nx, ny, nz */
		g3d_stream_read_float_le(global->stream);
		g3d_stream_read_float_le(global->stream);
		g3d_stream_read_float_le(global->stream);
		/* u, v */
		g3d_stream_read_float_le(global->stream);
		g3d_stream_read_float_le(global->stream);
	}

	return TRUE;
}

gboolean msfsmdl_bgl_cb_material_list(G3DIffGlobal *global, G3DIffLocal *local) {
	guint32 n_elems, i, j;
	G3DFloat d[4], a[4], s[4], e[4];
	G3DMaterial *mat;

	n_elems = g3d_stream_read_int16_le(global->stream);
	g3d_stream_read_int32_le(global->stream);

	for (i = 0; i < n_elems; i ++) {
		for (j = 0; j < 4; j ++)
			d[j] = g3d_stream_read_float_le(global->stream);
		for (j = 0; j < 4; j ++)
			a[j] = g3d_stream_read_float_le(global->stream);
		for (j = 0; j < 4; j ++)
			s[j] = g3d_stream_read_float_le(global->stream);
		for (j = 0; j < 4; j ++)
			e[j] = g3d_stream_read_float_le(global->stream);

		g3d_stream_read_float_le(global->stream); /* specular power */

		g_debug("| material d=(%02f,%02f,%02f,%02f)", d[0], d[1], d[2], d[3]);

		mat = g3d_material_new();
		mat->r = d[0];
		mat->g = d[1];
		mat->b = d[2];
		mat->a = d[3];
		global->model->materials = g_slist_append(global->model->materials, mat);
	}

	return TRUE;
}

gboolean msfsmdl_bgl_cb_texture_list(G3DIffGlobal *global, G3DIffLocal *local) {
	guint32 n_elems, i;
	gchar name[65];

	n_elems = g3d_stream_read_int16_le(global->stream);
	g3d_stream_read_int32_le(global->stream);

	for (i = 0; i < n_elems; i ++) {
		g3d_stream_read_int32_le(global->stream); /* tclass */
		g3d_stream_read_int32_le(global->stream); /* tcolor */
		g3d_stream_read_int32_le(global->stream); /* handle/hint */
		g3d_stream_read_float_le(global->stream); /* lod factor */

		memset(name, '\0', 65);
		g3d_stream_read(global->stream, name, 64);

		g_debug("| texture %s", name);
	}

	return TRUE;
}

gboolean msfsmdl_bgl_cb_draw_trilist(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;
	guint32 n_elems, i;
	gint16 vbase, vcount;
	G3DMaterial *mat;
	G3DObject *object;
	G3DFace *face;

	/* vertex_base, vertex_count */
	vbase = g3d_stream_read_int16_le(global->stream);
	vcount = g3d_stream_read_int16_le(global->stream);

	n_elems = g3d_stream_read_int16_le(global->stream);

	g_debug("| DRAW_TRILIST  %i,0x%04x,%i", n_elems, vbase, vcount);

	if (n_elems % 3 != 0) {
		g_error("expected vertex count to be n * 3 (%i)", n_elems);
		return FALSE;
	}

	g_assert(global->model->materials);
	mat = g_slist_nth_data(global->model->materials, 0);

	g_assert(state->current_object);
	object = state->current_object;

	for (i = 0; i < n_elems; i += 3) {
		gint16 v1, v2, v3;
		v1 = g3d_stream_read_int16_le(global->stream);
		v2 = g3d_stream_read_int16_le(global->stream);
		v3 = g3d_stream_read_int16_le(global->stream);

		face = g3d_face_new_tri(mat, vbase + v1, vbase + v2, vbase + v3);
		object->faces = g_slist_prepend(object->faces, face);
#if 0
		g_debug("| %i", v1);
#endif
	}

	return TRUE;
}

gboolean msfsmdl_bgl_cb_draw_linelist(G3DIffGlobal *global, G3DIffLocal *local) {
	guint32 n_elems, i;
	gint16 vbase, vcount;

	/* vertex_base, vertex_count */
	vbase = g3d_stream_read_int16_le(global->stream);
	vcount = g3d_stream_read_int16_le(global->stream);

	n_elems = g3d_stream_read_int16_le(global->stream);

	g_debug("| DRAW_LINELIST %i,%i,%i", n_elems, vbase, vcount);

	if (n_elems % 3 != 0) {
		g_error("expected vertex count to be n * 2 (%i)", n_elems);
		return FALSE;
	}

	for (i = 0; i < n_elems; i += 2) {
		gint16 v1, v2;
		v1 = g3d_stream_read_int16_le(global->stream);
		v2 = g3d_stream_read_int16_le(global->stream);
#if 0
		g_debug("| %i", v1);
#endif
	}

	return TRUE;
}

gboolean msfsmdl_bgl_cb_mouserect_list(G3DIffGlobal *global, G3DIffLocal *local) {
	guint32 n_elems;

	n_elems = g3d_stream_read_int16_le(global->stream);

	g3d_stream_skip(global->stream, n_elems * 11 * 4);

	return TRUE;
}

gboolean msfsmdl_bgl_cb_vinstance_call(G3DIffGlobal *global, G3DIffLocal *local) {
	BglState *state = global->user_data;

	gint16 addr = g3d_stream_read_int16_le(global->stream);
	gint16 voff = g3d_stream_read_int16_le(global->stream);
	gint16 aval = bgl_get_var(global, voff);
	goffset curr = g3d_stream_tell(global->stream);
	goffset dest = curr + addr - 6;

	g_queue_push_head(state->call_stack, GSIZE_TO_POINTER(curr));

	g_debug("| VINSTANCE_CALL %p from %p (offset %i)", dest, curr, addr);
	g3d_stream_seek(global->stream, dest, G_SEEK_SET);

	return TRUE;
}
