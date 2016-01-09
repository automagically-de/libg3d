#include <g3d/stream.h>
#include <g3d/matrix.h>
#include <g3d/material.h>
#include <g3d/face.h>

#include "imp_msfsmdl_callbacks.h"
#include "imp_msfsmdl_bgl.h"

typedef struct {
	guint32 type;
	guint32 scenegraph_reference;
	guint32 material_index;
	guint32 vertex_buffer_index;
	guint32 vertex_offset;
	guint32 vertex_count;
	guint32 index_offset;
	guint32 index_count;
	guint32 mouse_rectangle_ref;
} MdlPart;

typedef struct {
	/* SCEN */
	gint16 cnode_idx;
	gint16 pnode_idx;
	guint16 amap_off;

	/* AMAP */
	gint32 trans_type;
	gint32 trans_matrix_idx;

	/* PART */
	GSList *parts;
} MdlSgItem;

typedef struct {
	guint32 num_indices;
	guint32 *indices;

	guint32 num_vertex_buffers;
	guint32 *num_vertices;
	G3DFloat **vertex_data;
	G3DFloat **normal_data;
	G3DFloat **tex_vertex_data;

	guint32 num_matrices;
	G3DMatrix **matrices;

	guint32 num_parts;
	MdlPart **parts;

	guint32 num_sgitems;
	MdlSgItem *sgitems;
} MdlState;

static MdlState *get_state(G3DIffGlobal *global) {
	MdlState *state;

	if (global->user_data)
		return global->user_data;

	state = g_new0(MdlState, 1);
	global->user_data = state;

	return state;
}

gboolean msfsmdl_cb_amap(G3DIffGlobal *global, G3DIffLocal *local) {
	MdlState *state = get_state(global);
	guint32 i;

	if (state->num_sgitems)
		g_assert(state->num_sgitems == local->nb / 8);
	else {
		state->num_sgitems = local->nb / 8;
		state->sgitems = g_new0(MdlSgItem, state->num_sgitems);
	}

	for (i = 0; i < state->num_sgitems; i ++) {
		MdlSgItem *sgitem = &state->sgitems[i];

		sgitem->trans_type = g3d_stream_read_int32_le(global->stream);
		sgitem->trans_matrix_idx = g3d_stream_read_int32_le(global->stream);

		local->nb -= 8;
	}

	g_debug("| %i animation map entries", state->num_sgitems);

	return TRUE;
}

gboolean msfsmdl_cb_bgl_(G3DIffGlobal *global, G3DIffLocal *local) {
	return msfsmdl_parse_bgl(global, local);
}

gboolean msfsmdl_cb_inde(G3DIffGlobal *global, G3DIffLocal *local) {
	MdlState *state = get_state(global);
	guint32 i;

	state->num_indices = local->nb / 2;
	state->indices = g_new0(guint32, state->num_indices);

	for (i = 0; i < state->num_indices; i ++)
		state->indices[i] = g3d_stream_read_int16_le(global->stream);

	local->nb -= state->num_indices * 2;

	g_debug("| %i indices", state->num_indices);

	return TRUE;
}

gboolean msfsmdl_cb_lode(G3DIffGlobal *global, G3DIffLocal *local) {
	if (!local->finalize) {
		guint32 lod = g3d_stream_read_int32_le(global->stream);
		g_debug("| LOD value: %i", lod);
		local->nb -= 4;
	}
	return TRUE;
}

gboolean msfsmdl_cb_mate(G3DIffGlobal *global, G3DIffLocal *local) {
	G3DMaterial *mat;
	guint32 i;
	guint32 num_mats = local->nb / 120;

	for (i = 0; i < num_mats; i ++) {
		mat = g3d_material_new();

		g3d_stream_read_int32_le(global->stream); /* flags */
		g3d_stream_read_int32_le(global->stream); /* flags 2 */
		g3d_stream_read_int32_le(global->stream); /* diffuse tex index */
		g3d_stream_read_int32_le(global->stream); /* detail tex index */
		g3d_stream_read_int32_le(global->stream); /* bumpmap tex index */
		g3d_stream_read_int32_le(global->stream); /* specular tex index */
		g3d_stream_read_int32_le(global->stream); /* emissive tex index */
		g3d_stream_read_int32_le(global->stream); /* reflection tex index */
		g3d_stream_read_int32_le(global->stream); /* fresnel tex index */

		mat->r = g3d_stream_read_float_le(global->stream);
		mat->g = g3d_stream_read_float_le(global->stream);
		mat->b = g3d_stream_read_float_le(global->stream);
		mat->a = g3d_stream_read_float_le(global->stream);

		/* specular RGBA */
		g3d_stream_read_float_le(global->stream);
		g3d_stream_read_float_le(global->stream);
		g3d_stream_read_float_le(global->stream);
		g3d_stream_read_float_le(global->stream);

		g3d_stream_read_float_le(global->stream); /* specular power */
		g3d_stream_read_float_le(global->stream); /* detail map scale */
		g3d_stream_read_float_le(global->stream); /* bump map scale */
		g3d_stream_read_float_le(global->stream); /* reflection scale */
		g3d_stream_read_float_le(global->stream); /* precipitation_offset */
		g3d_stream_read_float_le(global->stream); /* specular_map_power_scale */
		g3d_stream_read_float_le(global->stream); /* specular_bloom_floor */
		g3d_stream_read_float_le(global->stream); /* ambient_light_scale */

		g3d_stream_read_int32_le(global->stream); /* source_blend */
		g3d_stream_read_int32_le(global->stream); /* destination_blend */
		g3d_stream_read_int32_le(global->stream); /* alpha_test_function */

		g3d_stream_read_float_le(global->stream); /* alpha_test_threshold */
		g3d_stream_read_float_le(global->stream); /* final_alpha_multiply */

		local->nb -= 120;

		mat->name = g_strdup_printf("material #%i", i);
		global->model->materials = g_slist_append(global->model->materials, mat);

		g_debug("| material (%0.2f, %0.2f, %0.2f, %0.2f)", mat->r, mat->g, mat->b, mat->a);
	}

	return TRUE;
}

static G3DObject *get_part(G3DModel *model, MdlState *state, MdlPart *part,
	G3DTransformation *transformation) {

	G3DObject *object = g_new0(G3DObject, 1);
	G3DMaterial *mat;
	guint32 i, j, vb;

	object->name = g_strdup("part");

	if (part->material_index >= g_slist_length(model->materials)) {
		g_error("material %i out of range (%i materials)",
			part->material_index, g_slist_length(model->materials));
		return NULL;
	}
	if (part->vertex_buffer_index >= state->num_vertex_buffers) {
		g_error("vertex buffer %i out of range (%i)",
			part->vertex_buffer_index, state->num_vertex_buffers);
		return NULL;
	}

	vb = part->vertex_buffer_index;

	if ((part->vertex_offset + part->vertex_count) > state->num_vertices[vb]) {
		g_error("vertex %i+%i out of range (%i)",
			part->vertex_offset, part->vertex_count, state->num_vertices[vb]);
		return NULL;
	}
	if ((part->index_offset + part->index_count) > state->num_indices) {
		g_error("index %i+%i out of range (%i)",
			part->index_offset, part->index_count, state->num_indices);
		return NULL;
	}

	mat = g_slist_nth_data(model->materials, part->material_index);

	object->vertex_count = part->vertex_count;
	object->vertex_data = g_new0(G3DFloat, part->vertex_count * 3);
	memcpy(
		object->vertex_data,
		state->vertex_data[vb] + part->vertex_offset * 3,
		part->vertex_count * 3 * sizeof(G3DFloat));

	if (transformation) {
		for (i = 0; i < part->vertex_count; i ++)
			g3d_vector_transform(
				object->vertex_data + i * 3 + 0,
				object->vertex_data + i * 3 + 1,
				object->vertex_data + i * 3 + 2,
				transformation->matrix);
	}

	if (part->type == 1) {
		/* triangle list */
		for (i = 0; i < part->index_count / 3; i ++) {
			guint32 ibase = part->index_offset + i * 3;
			G3DFace *face = g3d_face_new_tri(mat,
				state->indices[ibase + 0],
				state->indices[ibase + 1],
				state->indices[ibase + 2]);

			face->tex_vertex_count = 3;
			face->tex_vertex_data = g_new0(G3DFloat, 6);
			face->normals = g_new0(G3DFloat, 9);
			face->flags |= G3D_FLAG_FAC_NORMALS;
			for (j = 0; j < 3; j ++) {
				guint32 off = part->vertex_offset + state->indices[ibase + j];

				face->normals[j * 3 + 0] = state->normal_data[vb][off * 3 + 0];
				face->normals[j * 3 + 1] = state->normal_data[vb][off * 3 + 1];
				face->normals[j * 3 + 2] = state->normal_data[vb][off * 3 + 2];
#if 0
				if (transformation)
					g3d_vector_transform(
						face->normals + j * 3 + 0,
						face->normals + j * 3 + 1,
						face->normals + j * 3 + 2,
						transformation->matrix);
#endif

				face->tex_vertex_data[j * 2 + 0] = state->tex_vertex_data[vb][off * 2 + 0];
				face->tex_vertex_data[j * 2 + 1] = state->tex_vertex_data[vb][off * 2 + 1];
			}

			object->faces = g_slist_prepend(object->faces, face);
		}
	}
	else {
		g_warning("unhandled part type %i", part->type);
	}

	return object;
}

static void walk_scenegraph(G3DModel *model, MdlState *state, guint32 idx,
	G3DObject *parent_object) {

	G3DObject *object;
	MdlSgItem *sgitem;
	GSList *item;

	if (idx >= state->num_sgitems) {
		g_error("index %i > max (%i)", idx, state->num_sgitems - 1);
		return;
	}

	sgitem = &state->sgitems[idx];

	g_debug("| item %i, %i parts", idx, g_slist_length(sgitem->parts));

	object = g_new0(G3DObject, 1);
	object->name = g_strdup_printf("scenegraph entry %i", idx);

	if (sgitem->trans_matrix_idx >= 0) {
		G3DMatrix *matrix;
		guint32 i;

		if (sgitem->trans_matrix_idx >= state->num_matrices) {
			g_error("matrix %i out of range (%i)",
				sgitem->trans_matrix_idx, state->num_matrices);
			return NULL;
		}
		matrix = state->matrices[sgitem->trans_matrix_idx];

		object->transformation = g_new0(G3DTransformation, 1);
		for (i = 0; i < 16; i ++)
			object->transformation->matrix[i] = matrix[i];

		if (parent_object && parent_object->transformation)
			g3d_matrix_multiply(
				parent_object->transformation->matrix,
				object->transformation->matrix,
				object->transformation->matrix);
	}

	for (item = sgitem->parts; item != NULL; item = item->next)
		object->objects = g_slist_append(object->objects,
			get_part(model, state, item->data, object->transformation));

	if (parent_object)
		parent_object->objects = g_slist_append(parent_object->objects, object);
	else
		model->objects = g_slist_append(model->objects, object);

	if (sgitem->cnode_idx >= 0)
		walk_scenegraph(model, state, sgitem->cnode_idx, object);
	if (sgitem->pnode_idx >= 0)
		walk_scenegraph(model, state, sgitem->pnode_idx, parent_object);

	if (object->transformation) {
		g_free(object->transformation);
		object->transformation = NULL;
	}
}

gboolean msfsmdl_cb_mdld(G3DIffGlobal *global, G3DIffLocal *local) {
	MdlState *state = get_state(global);

	if (!local->finalize)
		return TRUE;

	walk_scenegraph(global->model, state, 0, NULL);

	return TRUE;
}

gboolean msfsmdl_cb_part(G3DIffGlobal *global, G3DIffLocal *local) {
	MdlState *state = get_state(global);
	MdlPart *part;

	part = g_new0(MdlPart, 1);
	state->num_parts ++;
	state->parts = g_realloc(state->parts, state->num_parts * sizeof(MdlPart*));
	state->parts[state->num_parts - 1] = part;

	part->type = g3d_stream_read_int32_le(global->stream);
	part->scenegraph_reference = g3d_stream_read_int32_le(global->stream);
	part->material_index = g3d_stream_read_int32_le(global->stream);
	part->vertex_buffer_index = g3d_stream_read_int32_le(global->stream);
	part->vertex_offset = g3d_stream_read_int32_le(global->stream);
	part->vertex_count = g3d_stream_read_int32_le(global->stream);
	part->index_offset = g3d_stream_read_int32_le(global->stream);
	part->index_count = g3d_stream_read_int32_le(global->stream);
	part->mouse_rectangle_ref = g3d_stream_read_int32_le(global->stream);

	local->nb -= 9 * 4;

	g_debug("| part type=%i, sgref=%i, mat=%i, voff=%i, vcnt=%i, ioff=%i, icnt=%i",
		part->type,
		part->scenegraph_reference,
		part->material_index,
		part->vertex_offset,
		part->vertex_count,
		part->index_offset,
		part->index_count);

	if (part->scenegraph_reference < state->num_sgitems)
		state->sgitems[part->scenegraph_reference].parts = g_slist_append(
			state->sgitems[part->scenegraph_reference].parts, part);

	return TRUE;
}

gboolean msfsmdl_cb_scen(G3DIffGlobal *global, G3DIffLocal *local) {
	MdlState *state = get_state(global);
	guint32 i;

	if (state->num_sgitems)
		g_assert(state->num_sgitems == local->nb / 8);
	else {
		state->num_sgitems = local->nb / 8;
		state->sgitems = g_new0(MdlSgItem, state->num_sgitems);
	}

	for (i = 0; i < state->num_sgitems; i ++) {
		MdlSgItem *sgitem = &state->sgitems[i];

		sgitem->cnode_idx = g3d_stream_read_int16_le(global->stream);
		sgitem->pnode_idx = g3d_stream_read_int16_le(global->stream);
		sgitem->amap_off = g3d_stream_read_int16_le(global->stream);
		g3d_stream_read_int16_le(global->stream);

		local->nb -= 8;
	}

	g_debug("| %i scene graph entries", state->num_sgitems);

	return TRUE;
}

gboolean msfsmdl_cb_text(G3DIffGlobal *global, G3DIffLocal *local) {
	guint32 i;
	gchar *name;

	for (i = 0; i < local->nb / 64; i ++) {
		name = g_new0(gchar, 65);
		g3d_stream_read(global->stream, name, 64);
		local->nb -= 64;

		g_debug("| texture %s", name);
		g_free(name);
	}

	return TRUE;
}

gboolean msfsmdl_cb_tran(G3DIffGlobal *global, G3DIffLocal *local) {
	MdlState *state = get_state(global);
	guint32 i, j;

	state->num_matrices = local->nb / 64;
	state->matrices = g_new0(G3DMatrix*, state->num_matrices);

	for (i = 0; i < state->num_matrices; i ++) {
		state->matrices[i] = g3d_matrix_new();
		for (j = 0; j < 16; j ++)
			state->matrices[i][j] = g3d_stream_read_float_le(global->stream);

#if DEBUG > 2
		g_debug("matrix %i:", i);
		g3d_matrix_dump(state->matrices[i]);
#endif
	}

	local->nb -= state->num_matrices * 64;

	g_debug("| %i matrices", state->num_matrices);

	return TRUE;
}

gboolean msfsmdl_cb_vert(G3DIffGlobal *global, G3DIffLocal *local) {
	MdlState *state = get_state(global);
	guint32 i, vb;

	vb = state->num_vertex_buffers;
	state->num_vertex_buffers ++;
	state->num_vertices = g_realloc(state->num_vertices,
		state->num_vertex_buffers * sizeof(guint32));
	state->vertex_data = g_realloc(state->vertex_data,
		state->num_vertex_buffers * sizeof(G3DFloat*));
	state->normal_data = g_realloc(state->normal_data,
		state->num_vertex_buffers * sizeof(G3DFloat*));
	state->tex_vertex_data = g_realloc(state->tex_vertex_data,
		state->num_vertex_buffers * sizeof(G3DFloat*));

	state->num_vertices[vb] = local->nb / 32;
	state->vertex_data[vb] = g_new0(G3DFloat, state->num_vertices[vb] * 3);
	state->normal_data[vb] = g_new0(G3DFloat, state->num_vertices[vb] * 3);
	state->tex_vertex_data[vb] = g_new0(G3DFloat, state->num_vertices[vb] * 2);

	for (i = 0; i < state->num_vertices[vb]; i ++) {
		state->vertex_data[vb][i * 3 + 0] = g3d_stream_read_float_le(global->stream);
		state->vertex_data[vb][i * 3 + 1] = g3d_stream_read_float_le(global->stream);
		state->vertex_data[vb][i * 3 + 2] = g3d_stream_read_float_le(global->stream);

		state->normal_data[vb][i * 3 + 0] = g3d_stream_read_float_le(global->stream);
		state->normal_data[vb][i * 3 + 1] = g3d_stream_read_float_le(global->stream);
		state->normal_data[vb][i * 3 + 2] = g3d_stream_read_float_le(global->stream);

		state->tex_vertex_data[vb][i * 2 + 0] = g3d_stream_read_float_le(global->stream);
		state->tex_vertex_data[vb][i * 2 + 1] = g3d_stream_read_float_le(global->stream);
	}

	local->nb -= state->num_vertices[vb] * 32;

	g_debug("| %i vertices", state->num_vertices[vb]);

	return TRUE;
}

