#include <g3d/types.h>
#include <g3d/material.h>
#include <g3d/debug.h>

#include "imp_vrml_v1_types.h"

static G3DObject *v1_get_parent_object(VrmlGlobal *global)
{
	GList *item;
	VrmlNode *node;

	for(item = global->stack->head; item != NULL; item = item->next) {
		node = item->data;
		if(node->object)
			return node->object;
	}
	return NULL;
}

static G3DObject *v1_get_parent_vertex_object(VrmlGlobal *global)
{
	GList *item;
	VrmlNode *node;

	for(item = global->stack->head; item != NULL; item = item->next) {
		node = item->data;
		if(node->object && node->object->vertex_data)
			return node->object;
	}
	return NULL;
}

gboolean v1_cb_coordinate3(VrmlGlobal *global, VrmlNode *node, VrmlStep s)
{
	G3DObject *parent;
	VrmlProperty *prop;
	guint32 n, m;

	if(s == VRML_STEP_CLOSE) {
		parent = v1_get_parent_object(global);
		if(parent == NULL)
			return FALSE;
		prop = g_hash_table_lookup(node->properties, "point");
		if(prop) {
			parent->vertex_count = prop->n_items;
			parent->vertex_data = g_new0(G3DVector, prop->n_items * 3);
			for(n = 0; n < prop->n_items; n ++)
				for(m = 0; m < 3; m ++) {
					parent->vertex_data[n * 3 + m] =
						prop->u.fp[n * 3 + m];
				}
		}
	}
	return TRUE;
}

static void finalize_face(G3DObject *object, G3DFace *face, G3DMaterial *m)
{
	if(face->vertex_count >= 3) {
		face->material = m;
		object->faces = g_slist_prepend(object->faces, face);
	} else {
		g_free(face);
	}
}

gboolean v1_cb_idxfaceset(VrmlGlobal *global, VrmlNode *node, VrmlStep s)
{
	G3DObject *parent, *vparent;
	G3DFace *face;
	G3DMaterial *material;
	VrmlProperty *prop;
	guint32 n;
	gint32 idx;

	if(s == VRML_STEP_CLOSE) {
		parent = v1_get_parent_object(global);
		if(parent == NULL)
			return FALSE;
		vparent = v1_get_parent_vertex_object(global);
		if(vparent == NULL)
			return FALSE;

		if(parent != vparent) {
			parent->vertex_count = vparent->vertex_count;
			parent->vertex_data = g_new0(G3DVector, parent->vertex_count * 3);
			memcpy(parent->vertex_data, vparent->vertex_data,
				parent->vertex_count * 3 * sizeof(G3DVector));
		}

		if(parent->materials == NULL) {
			if(vparent->materials != NULL) {
				material = g_slist_nth_data(vparent->materials, 0);
			} else {
				material = g3d_material_new();
				parent->materials = g_slist_append(parent->materials,
					material);
			}
		} else {
			material = g_slist_nth_data(parent->materials, 0);
		}

		prop = g_hash_table_lookup(node->properties, "coordIndex");
		if(prop) {
			face = g_new0(G3DFace, 1);
			for(n = 0; n < prop->n_items; n ++) {
				idx = prop->u.ip[n];
				if(idx == -1) {
					finalize_face(parent, face, material);
					face = g_new0(G3DFace, 1);
				} else {
					face->vertex_count ++;
					face->vertex_indices = g_realloc(face->vertex_indices,
						face->vertex_count * sizeof(guint32));
					face->vertex_indices[face->vertex_count - 1] = idx;
				}
			}
			finalize_face(parent, face, material);
		}
	}

	return TRUE;
}

gboolean v1_cb_material(VrmlGlobal *global, VrmlNode *node, VrmlStep s)
{
	G3DObject *object;
	G3DMaterial *material;
	VrmlProperty *prop;

	if(s == VRML_STEP_CLOSE) {
		object = v1_get_parent_object(global);
		if(object == NULL)
			return FALSE;
		material = g3d_material_new();
		object->materials = g_slist_append(object->materials, material);
		if(node->defname)
			material->name = g_strdup(node->defname);
		else
			material->name = g_strdup("unnamed material");
		prop = g_hash_table_lookup(node->properties, "diffuseColor");
		if(prop && prop->n_items >= 1) {
			material->r = prop->u.fp[0];
			material->g = prop->u.fp[1];
			material->b = prop->u.fp[2];
		}
		prop = g_hash_table_lookup(node->properties, "transparency");
		if(prop) {
			material->a = 1.0 - prop->u.fp[0];
		}
	}
	return TRUE;
}

gboolean v1_cb_separator(VrmlGlobal *global, VrmlNode *node, VrmlStep s)
{
	G3DObject *parent;

	if(s == VRML_STEP_OPEN) {
		node->object = g_new0(G3DObject, 1);
		if(node->defname)
			node->object->name = g_strdup(node->defname);
		else
			node->object->name = g_strdup_printf("%s", node->name);
		parent = v1_get_parent_object(global);
		if(parent)
			parent->objects = g_slist_append(parent->objects, node->object);
		else
			global->model->objects = g_slist_append(global->model->objects,
				node->object);
	}
	return TRUE;
}


