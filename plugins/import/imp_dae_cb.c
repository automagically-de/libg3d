#include <string.h>

#include <g3d/types.h>
#include <g3d/material.h>
#include <g3d/matrix.h>
#include <g3d/object.h>
#include <g3d/texture.h>

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
	gchar *source;
} DaeInput;

/*****************************************************************************/

G3DMaterial *dae_get_material_by_name(DaeGlobalData *global, const gchar *id,
	guint32 level)
{
	G3DMaterial *material;
	GSList *mitem;
	gchar *tmp;
	xmlNodePtr matnode;

	/* try to find material */
	for(mitem = global->model->materials; mitem != NULL; mitem = mitem->next) {
		material = (G3DMaterial *)mitem->data;
		if(strcmp(material->name, id) == 0)
			return material;
	}

	material = g3d_material_new();
	material->name = g_strdup(id);
	global->model->materials = g_slist_append(global->model->materials,
		material);

	/* find material in library */
	/* FIXME: name -> id mapping */
	tmp = g_strdup_printf("%sID", id);
	matnode = dae_library_lookup(global->lib, "material", tmp);
	g_free(tmp);

	if(matnode) {
		dae_xml_parse(global, matnode, dae_chunks_material,
			level, material);
	}
	return material;
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
		input->source = dae_xml_get_attr(inode, "source");
		inputs = g_slist_append(inputs, input);
	}
	return inputs;
}

static void dae_inputs_free(GSList *inputs)
{
	GSList *item;
	DaeInput *input;

	for(item = inputs; item != NULL; item = item->next) {
		input = (DaeInput *)item->data;
		if(input->source)
			g_free(input->source);
		g_free(input);
		item->data = NULL;
	}
	g_slist_free(inputs);
}

static gboolean dae_load_source(DaeLibrary *lib, gchar *id,
	gfloat **asrc, guint32 *nsrc)
{
	xmlNodePtr snode, fnode;
	gchar *scnt, *next = NULL;
	gint i;

	snode = dae_library_lookup(lib, "source", id + 1);
	if(snode == NULL)
		return FALSE;
	g_debug("dae_load_source: got 'source' node");

	fnode = dae_xml_get_child_by_tagname(snode, "float_array");
	if(fnode == NULL)
		return FALSE;

	g_debug("dae_load_source: got 'float_array' node");

	scnt = dae_xml_get_attr(fnode, "count");
	if(scnt == NULL)
		return FALSE;
	*nsrc = atoi(scnt);
	g_free(scnt);
	if(*nsrc == 0)
		return FALSE;

	g_debug("dae_load_source: got 'count' attribute");

	*asrc = g_new0(gfloat, *nsrc);
	for(i = 0; i < *nsrc; i ++)
		if(!dae_xml_next_float(fnode, &next, &((*asrc)[i])))
			return FALSE;

	return TRUE;
}

/*****************************************************************************/

gboolean dae_cb_effect(DaeGlobalData *global, DaeLocalData *local)
{
	return dae_xml_parse(global, local->node, dae_chunks_effect,
		local->level, local->user_data);
}

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

gboolean dae_cb_newparam(DaeGlobalData *global, DaeLocalData *local)
{
	G3DMaterial *material = (G3DMaterial *)local->user_data;
	xmlNodePtr n1, n2;
	gchar *siid = NULL;

	g_return_val_if_fail(material != NULL, FALSE);

	g_debug("DAE: dae_cb_newparam");

	n1 = dae_xml_get_child_by_tagname(local->node, "surface");
	if(n1 != NULL) {
		n2 = dae_xml_get_child_by_tagname(n1, "init_from");
		if(n2 != NULL)
			siid = g_strdup((gchar *)n2->children->content);
	}
	if(siid == NULL)
		return FALSE;

	g_debug("DAE: looking up image '%s'", siid);

	n1 = dae_library_lookup(global->lib, "image", siid);
	g_free(siid);
	if(n1 == NULL)
		return FALSE;

	n2 = dae_xml_get_child_by_tagname(n1, "init_from");
	if(n2 == NULL)
		return FALSE;

	material->tex_image = g3d_texture_load_cached(global->context,
		global->model, (gchar *)n2->children->content);

	return TRUE;
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

gboolean dae_cb_phong(DaeGlobalData *global, DaeLocalData *local)
{
	G3DMaterial *material = (G3DMaterial *)local->user_data;
	xmlNodePtr n1, n2;
	gchar *next;

	g_return_val_if_fail(material != NULL, FALSE);

	/* diffuse */
	n1 = dae_xml_get_child_by_tagname(local->node, "diffuse");
	if(n1 != NULL) {
		n2 = dae_xml_get_child_by_tagname(n1, "color");
		if(n2 != NULL) {
			next = NULL;
			dae_xml_next_float(n2, &next, &(material->r));
			dae_xml_next_float(n2, &next, &(material->g));
			dae_xml_next_float(n2, &next, &(material->b));
			dae_xml_next_float(n2, &next, &(material->a));
		}
	}

	/* specular */
	n1 = dae_xml_get_child_by_tagname(local->node, "specular");
	if(n1 != NULL) {
		n2 = dae_xml_get_child_by_tagname(n1, "color");
		if(n2 != NULL) {
			next = NULL;
			dae_xml_next_float(n2, &next, &(material->specular[0]));
			dae_xml_next_float(n2, &next, &(material->specular[1]));
			dae_xml_next_float(n2, &next, &(material->specular[2]));
			dae_xml_next_float(n2, &next, &(material->specular[3]));
		}
	}

	return TRUE;
}

gboolean dae_cb_polylist(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object = local->user_data;
	G3DFace *face;
	G3DMaterial *material;
	xmlNodePtr pnode, vnode;
	gchar *scnt, *smat, *nextp = NULL, *nextv = NULL;
	guint32 count, normal_count, tex_count, flags = 0;
	gint i, j, nv, tmp;
	GSList *inputs, *item;
	DaeInput *input;
	gfloat *normal_data = NULL, *tex_data = NULL;

	g_return_val_if_fail(object != NULL, FALSE);

	scnt = dae_xml_get_attr(local->node, "count");
	g_return_val_if_fail(scnt != NULL, FALSE);
	count = atoi(scnt);
	g_return_val_if_fail(count != 0, FALSE);
	g_free(scnt);

	pnode = dae_xml_get_child_by_tagname(local->node, "p");
	vnode = dae_xml_get_child_by_tagname(local->node, "vcount");
	g_return_val_if_fail((pnode != NULL) && (vnode != NULL), FALSE);

	/* material */
	material = g_slist_nth_data(object->materials, 0);
	smat = dae_xml_get_attr(local->node, "material");
	if(smat != NULL) {
		material = dae_get_material_by_name(global, smat, local->level);
		g_free(smat);
	}

	/* get all inputs */
	inputs = dae_get_inputs(local->node);
	for(item = inputs; item != NULL; item = item->next) {
		input = (DaeInput *)item->data;
		if(input->semantic == SEM_NORMAL)
			if(dae_load_source(global->lib, input->source,
				&normal_data, &normal_count))
				flags |= G3D_FLAG_FAC_NORMALS;
		if(input->semantic == SEM_TEXCOORD)
			if(dae_load_source(global->lib, input->source,
				&tex_data, &tex_count) && (material->tex_image != NULL))
				flags |= G3D_FLAG_FAC_TEXMAP;
	}

	for(i = 0; i < count; i ++) {
		if(dae_xml_next_int(vnode, &nextv, &nv) && (nv > 0)) {
			face = g_new0(G3DFace, 1);
			face->vertex_count = nv;
			face->vertex_indices = g_new0(guint32, nv);
			face->material = material;
			face->flags = flags;
			object->faces = g_slist_append(object->faces, face);

			if(face->flags & G3D_FLAG_FAC_NORMALS) {
				face->normals = g_new0(gfloat, nv * 3);
			}
			if(face->flags & G3D_FLAG_FAC_TEXMAP) {
				face->tex_image = material->tex_image;
				face->tex_vertex_count = nv;
				face->tex_vertex_data = g_new0(gfloat, nv * 2);
			}

			for(j = 0; j < nv; j ++) {
				for(item = inputs; item != NULL; item = item->next) {
					input = (DaeInput *)item->data;
					dae_xml_next_int(pnode, &nextp, &tmp);
					switch(input->semantic) {
						case SEM_VERTEX:
							face->vertex_indices[j] = tmp;
							break;
						case SEM_NORMAL:
							if(flags & G3D_FLAG_FAC_NORMALS) {
								face->normals[j * 3 + 0] =
									normal_data[tmp * 3 + 0];
								face->normals[j * 3 + 1] =
									normal_data[tmp * 3 + 1];
								face->normals[j * 3 + 2] =
									normal_data[tmp * 3 + 2];
							}
							break;
						case SEM_TEXCOORD:
							if(flags & G3D_FLAG_FAC_TEXMAP) {
								face->tex_vertex_data[j * 2 + 0] =
									tex_data[tmp * 2 + 0];
								face->tex_vertex_data[j * 2 + 1] =
									tex_data[tmp * 2 + 1];
							}
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

	if(tex_data)
		g_free(tex_data);
	if(normal_data)
		g_free(normal_data);
	dae_inputs_free(inputs);

	return TRUE;
}

gboolean dae_cb_profile_COMMON(DaeGlobalData *global, DaeLocalData *local)
{
	return dae_xml_parse(global, local->node, dae_chunks_profile_COMMON,
		local->level, local->user_data);
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

gboolean dae_cb_technique(DaeGlobalData *global, DaeLocalData *local)
{
	return dae_xml_parse(global, local->node, dae_chunks_technique,
		local->level, local->user_data);
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
	G3DMaterial *material;
	xmlNodePtr pnode;
	gchar *scnt, *smat, *nextp = NULL;
	guint32 count, normal_count, tex_count, flags = 0;
	gfloat *normal_data = NULL, *tex_data = NULL;
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

	/* material */
	material = g_slist_nth_data(object->materials, 0);
	smat = dae_xml_get_attr(local->node, "material");
	if(smat != NULL) {
		material = dae_get_material_by_name(global, smat, local->level);
		g_free(smat);
	}

	/* get all inputs */
	inputs = dae_get_inputs(local->node);
	for(item = inputs; item != NULL; item = item->next) {
		input = (DaeInput *)item->data;
		if(input->semantic == SEM_NORMAL)
			if(dae_load_source(global->lib, input->source,
				&normal_data, &normal_count)) {
				g_debug("dae_cb_triangles: normals loaded");
				flags |= G3D_FLAG_FAC_NORMALS;
			}
		if(input->semantic == SEM_TEXCOORD)
			if(dae_load_source(global->lib, input->source,
				&tex_data, &tex_count) && (material->tex_image != NULL)) {
				g_debug("dae_cb_triangles: texture coordinates loaded");
				flags |= G3D_FLAG_FAC_TEXMAP;
			}
	}

	for(i = 0; i < count; i ++) {
		face = g_new0(G3DFace, 1);
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		face->material = material;
		face->flags = flags;
		object->faces = g_slist_append(object->faces, face);

		if(face->flags & G3D_FLAG_FAC_NORMALS) {
			face->normals = g_new0(gfloat, 3 * 3);
		}
		if(face->flags & G3D_FLAG_FAC_TEXMAP) {
			face->tex_image = material->tex_image;
			face->tex_vertex_count = 3;
			face->tex_vertex_data = g_new0(gfloat, 3 * 2);
		}

		for(j = 0; j < 3; j ++) {
			for(item = inputs; item != NULL; item = item->next) {
				input = (DaeInput *)item->data;
				dae_xml_next_int(pnode, &nextp, &tmp);
				switch(input->semantic) {
					case SEM_VERTEX:
						face->vertex_indices[j] = tmp;
						break;
					case SEM_NORMAL:
						if(flags & G3D_FLAG_FAC_NORMALS) {
							face->normals[j * 3 + 0] =
								normal_data[tmp * 3 + 0];
							face->normals[j * 3 + 1] =
								normal_data[tmp * 3 + 1];
							face->normals[j * 3 + 2] =
								normal_data[tmp * 3 + 2];
						}
						break;
					case SEM_TEXCOORD:
						if(flags & G3D_FLAG_FAC_TEXMAP) {
							face->tex_vertex_data[j * 2 + 0] = 1.0 -
								tex_data[tmp * 2 + 0];
							face->tex_vertex_data[j * 2 + 1] = 1.0 -
								tex_data[tmp * 2 + 1];
						}
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
	if(tex_data)
		g_free(tex_data);
	if(normal_data)
		g_free(normal_data);
	dae_inputs_free(inputs);

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
