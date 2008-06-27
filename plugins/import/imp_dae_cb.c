#include <string.h>

#include <g3d/types.h>
#include <g3d/material.h>
#include <g3d/matrix.h>
#include <g3d/object.h>

#include "imp_dae_cb.h"
#include "imp_dae_chunks.h"
#include "imp_dae_xml.h"

typedef struct {
	guint32 offset;
	enum {
		SEM_UNKNOWN,
		SEM_VERTEX,
		SEM_NORMAL,
		SEM_TEXCOORD
	} semantic;
} DaeInput;

gboolean dae_cb_geometry(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object, *pobject;
	G3DMaterial *material;
	gchar *name;

	pobject = (G3DObject *)local->user_data;
	if(pobject == NULL)
		return FALSE;

#if 1
	object = pobject;
#else
	name = dae_xml_get_attr(local->node, "name");
	if(name == NULL)
		name = dae_xml_get_attr(local->node, "id");
	if(name == NULL)
		return FALSE;

	object = g_new0(G3DObject, 1);
	object->name = name;
	pobject->objects = g_slist_append(pobject->objects, object);
#endif
	material = g3d_material_new();
	material->name = g_strdup("(default material)");
	object->materials = g_slist_append(object->materials, material);

	/* parse instanced stuff */
	if(dae_xml_parse(global, local->node, dae_chunks_geometry,
		local->level, object)) {
		if(local->instance) {
			/* parse original node */
			return dae_xml_parse(global, local->instance,
				dae_chunks_geometry, local->level, object);
		}
		return TRUE;
	}
	return FALSE;
}

gboolean dae_cb_matrix(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object = local->user_data;
	G3DTransformation *transform;
	gchar *next = NULL;
	gint i;

	g_return_val_if_fail(object != NULL, FALSE);

	transform = object->transformation;
	if(transform == NULL) {
		transform = g_new0(G3DTransformation, 1);
		g3d_matrix_identity(transform->matrix);
		object->transformation = transform;
	}

	for(i = 0; i < 16; i ++)
		dae_xml_next_float(local->node, &next, transform->matrix + i);
	g3d_matrix_transpose(transform->matrix);
#if DEBUG > 3
	g_debug("DAE: matrix for '%s':", object->name);
	g3d_matrix_dump(transform->matrix);
#endif
	return TRUE;
}

gboolean dae_cb_mesh(DaeGlobalData *global, DaeLocalData *local)
{
	return dae_xml_parse(global, local->node, dae_chunks_mesh,
		local->level, local->user_data);
}

gboolean dae_cb_node(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object, *pobject;
	gchar *name;

	name = dae_xml_get_attr(local->node, "name");
	if(name == NULL)
		name = dae_xml_get_attr(local->node, "id");
	if(name == NULL)
		return FALSE;

	pobject = (G3DObject *)local->user_data;
	object = g_new0(G3DObject, 1);
	object->name = name;
	if(pobject)
		pobject->objects = g_slist_append(pobject->objects, object);
	else
		global->model->objects = g_slist_append(global->model->objects,
			object);

	if(dae_xml_parse(global, local->node, dae_chunks_node, local->level,
		object)) {
		if(object->transformation) {
			g3d_object_transform(object, object->transformation->matrix);
#if DEBUG > 3
			g_debug("DAE: transforming object '%s'", object->name);
			g3d_matrix_dump(object->transformation->matrix);
#endif
			g_free(object->transformation);
			object->transformation = NULL;
		}
		return TRUE;
	}
	return FALSE;
}

static GSList *dae_get_inputs(xmlNodePtr node)
{
	xmlNodePtr inode = NULL;
	gchar *soff, *ssem;
	DaeInput *input;
	GSList *inputs = NULL;

	while(dae_xml_next_child_by_tagname(node, &inode, "input")) {
		input = g_new0(DaeInput, 1);
		soff = dae_xml_get_attr(inode, "offset");
		if(soff) {
			input->offset = atoi(soff);
			g_free(soff);
		}
		ssem = dae_xml_get_attr(inode, "semantic");
		if(ssem) {
			if(strcmp(ssem, "VERTEX") == 0)
				input->semantic = SEM_VERTEX;
			else if(strcmp(ssem, "NORMAL") == 0)
				input->semantic = SEM_NORMAL;
			else if(strcmp(ssem, "TEXCOORD") == 0)
				input->semantic = SEM_TEXCOORD;
			else {
				g_warning("DAE: unknown input semantic '%s'", ssem);
				input->semantic = SEM_UNKNOWN;
			}
			g_free(ssem);
		}
		inputs = g_slist_append(inputs, input);
	}
	return inputs;
}

gboolean dae_cb_polylist(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object = local->user_data;
	G3DFace *face;
	xmlNodePtr pnode, vnode;
	gchar *scnt, *nextp = NULL, *nextv = NULL;
	guint32 count;
	gint i, j, nv, tmp;
	GSList *inputs, *item;
	DaeInput *input;

	g_return_val_if_fail(object != NULL, FALSE);

	scnt = dae_xml_get_attr(local->node, "count");
	g_return_val_if_fail(scnt != NULL, FALSE);
	count = atoi(scnt);
	g_return_val_if_fail(count != 0, FALSE);
	g_free(scnt);

	pnode = dae_xml_get_child_by_tagname(local->node, "p");
	vnode = dae_xml_get_child_by_tagname(local->node, "vcount");
	g_return_val_if_fail((pnode != NULL) && (vnode != NULL), FALSE);

	/* get all inputs */
	inputs = dae_get_inputs(local->node);

	for(i = 0; i < count; i ++) {
		if(dae_xml_next_int(vnode, &nextv, &nv) && (nv > 0)) {
			face = g_new0(G3DFace, 1);
			face->vertex_count = nv;
			face->vertex_indices = g_new0(guint32, nv);
			face->material = (G3DMaterial *)
				g_slist_nth_data(object->materials, 0);
			object->faces = g_slist_append(object->faces, face);

			for(j = 0; j < nv; j ++) {
				for(item = inputs; item != NULL; item = item->next) {
					input = (DaeInput *)item->data;
					dae_xml_next_int(pnode, &nextp, &tmp);
					switch(input->semantic) {
						case SEM_VERTEX:
							face->vertex_indices[j] = tmp;
							break;
						case SEM_NORMAL:
							break;
						case SEM_TEXCOORD:
							break;
						case SEM_UNKNOWN:
							break;
					}
				} /* inputs */
			}
#if DEBUG > 3
			g_debug("DAE: %-2i, %-2i, %-2i, (%-2i)",
				face->vertex_indices[0],
				face->vertex_indices[1],
				face->vertex_indices[2],
				((nv > 3) ? face->vertex_indices[3] : -1));
#endif
		}
	}

	return TRUE;
}

gboolean dae_cb_rotate(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object = local->user_data;
	G3DTransformation *transform;
	gfloat x = 0.0, y = 0.0, z = 0.0, a = 0.0, m[16];
	gchar *next = NULL;

	g_return_val_if_fail(object != NULL, FALSE);

	transform = object->transformation;
	if(transform == NULL) {
		transform = g_new0(G3DTransformation, 1);
		g3d_matrix_identity(transform->matrix);
		object->transformation = transform;
	}

	dae_xml_next_float(local->node, &next, &x);
	dae_xml_next_float(local->node, &next, &y);
	dae_xml_next_float(local->node, &next, &z);
	dae_xml_next_float(local->node, &next, &a);
	g_return_val_if_fail((x != 0.0) || (y != 0.0) || (z != 0.0), FALSE);
	g3d_matrix_rotate(a, x, y, z, m);
	g3d_matrix_multiply(transform->matrix, m, transform->matrix);
#if DEBUG > 3
	g_debug("DAE: rotation for '%s': %.2f, %.2f, %.2f - %.2f", object->name,
		x, y, z, a);
	g3d_matrix_dump(transform->matrix);
#endif
	return TRUE;
}

gboolean dae_cb_scale(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object = local->user_data;
	G3DTransformation *transform;
	gfloat x = 0.0, y = 0.0, z = 0.0;
	gchar *next = NULL;

	g_return_val_if_fail(object != NULL, FALSE);

	transform = object->transformation;
	if(transform == NULL) {
		transform = g_new0(G3DTransformation, 1);
		g3d_matrix_identity(transform->matrix);
		object->transformation = transform;
	}

	dae_xml_next_float(local->node, &next, &x);
	dae_xml_next_float(local->node, &next, &y);
	dae_xml_next_float(local->node, &next, &z);
	g3d_matrix_scale(x, y, z, transform->matrix);
#if DEBUG > 3
	g_debug("DAE: scale for '%s': %.2f, %.2f, %.2f", object->name,
		x, y, z);
	g3d_matrix_dump(transform->matrix);
#endif
	return TRUE;
}

gboolean dae_cb_source(DaeGlobalData *global, DaeLocalData *local)
{
	gchar *id;

	id = dae_xml_get_attr(local->node, "id");
	if(id) {
		dae_library_add(global->lib, "source", id, local->node);
		g_free(id);
		return TRUE;
	}
	return FALSE;
}

gboolean dae_cb_translate(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object = local->user_data;
	G3DTransformation *transform;
	gfloat x = 0.0, y = 0.0, z = 0.0;
	gchar *next = NULL;

	g_return_val_if_fail(object != NULL, FALSE);

	transform = object->transformation;
	if(transform == NULL) {
		transform = g_new0(G3DTransformation, 1);
		g3d_matrix_identity(transform->matrix);
		object->transformation = transform;
	}

	dae_xml_next_float(local->node, &next, &x);
	dae_xml_next_float(local->node, &next, &y);
	dae_xml_next_float(local->node, &next, &z);
	g3d_matrix_translate(x, y, z, transform->matrix);
#if DEBUG > 3
	g_debug("DAE: translation for '%s': %.2f, %.2f, %.2f", object->name,
		x, y, z);
	g3d_matrix_dump(transform->matrix);
#endif
	return TRUE;
}

gboolean dae_cb_triangles(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object = local->user_data;
	G3DFace *face;
	xmlNodePtr pnode;
	gchar *scnt, *nextp = NULL;
	guint32 count;
	gint i, j, tmp;
	GSList *inputs, *item;
	DaeInput *input;

	g_return_val_if_fail(object != NULL, FALSE);

	scnt = dae_xml_get_attr(local->node, "count");
	g_return_val_if_fail(scnt != NULL, FALSE);
	count = atoi(scnt);
	g_return_val_if_fail(count != 0, FALSE);
	g_free(scnt);

	pnode = dae_xml_get_child_by_tagname(local->node, "p");
	g_return_val_if_fail(pnode != NULL, FALSE);

	/* get all inputs */
	inputs = dae_get_inputs(local->node);

	for(i = 0; i < count; i ++) {
		face = g_new0(G3DFace, 1);
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		face->material = (G3DMaterial *)
			g_slist_nth_data(object->materials, 0);
		object->faces = g_slist_append(object->faces, face);

		for(j = 0; j < 3; j ++) {
			for(item = inputs; item != NULL; item = item->next) {
				input = (DaeInput *)item->data;
				dae_xml_next_int(pnode, &nextp, &tmp);
				switch(input->semantic) {
					case SEM_VERTEX:
						face->vertex_indices[j] = tmp;
						break;
					case SEM_NORMAL:
						break;
					case SEM_TEXCOORD:
						break;
					case SEM_UNKNOWN:
						break;
				}
			} /* inputs */
#if DEBUG > 3
			g_debug("DAE: %-2i, %-2i, %-2i",
				face->vertex_indices[0],
				face->vertex_indices[1],
				face->vertex_indices[2]);
#endif
		}
	}

	return TRUE;
}

gboolean dae_cb_vertices(DaeGlobalData *global, DaeLocalData *local)
{
	return dae_xml_parse(global, local->node, dae_chunks_vertices,
		local->level, local->user_data);
}

gboolean dae_cb_vertices__input(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object = (G3DObject *)(local->user_data);
	gchar *sid, *sem, *cnt, *next = NULL;
	gboolean skip = FALSE;
	xmlNodePtr snode, fanode;
	gint i, j;

	g_return_val_if_fail(object != NULL, FALSE);

	sem = dae_xml_get_attr(local->node, "semantic");
	sid = dae_xml_get_attr(local->node, "source");
	g_return_val_if_fail((sem != NULL) && (sid != NULL), FALSE);

	/* get 'source' node, skip leading '#' from 'source' attribute */
	snode = dae_library_lookup(global->lib, "source", sid + 1);
	g_return_val_if_fail(snode != NULL, FALSE);

	g_free(sem);
	g_free(sid);

	/* TODO: check technique_common/accessor */

	fanode = dae_xml_get_child_by_tagname(snode, "float_array");
	if(fanode) {
		cnt = dae_xml_get_attr(fanode, "count");
		g_return_val_if_fail(cnt != NULL, FALSE);

		object->vertex_count = atoi(cnt);
		g_return_val_if_fail(object->vertex_count != 0, FALSE);

		object->vertex_data = g_new0(gfloat, 3 * object->vertex_count);
		for(i = 0; i < object->vertex_count / 3; i ++) {
			for(j = 0; j < 3; j ++) {
				if(!dae_xml_next_float(fanode, &next,
					&(object->vertex_data[i * 3 + j]))) {
					skip = TRUE;
					break;
				}
			}
#if DEBUG > 3
			g_debug("DAE: %-3.2f, %-3.2f, %-3.2f",
				object->vertex_data[i * 3 + 0],
				object->vertex_data[i * 3 + 1],
				object->vertex_data[i * 3 + 2]);
#endif
			if(skip)
				break;
		}
	}

	return TRUE;
}

gboolean dae_cb_visual_scene(DaeGlobalData *global, DaeLocalData *local)
{
	return dae_xml_parse(global, local->node, dae_chunks_visual_scene,
		local->level, NULL);
}
