
#include "imp_dae_cb.h"
#include "imp_dae_chunks.h"
#include "imp_dae_xml.h"

gboolean dae_cb_geometry(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object, *pobject;
	gchar *name;

	pobject = (G3DObject *)local->user_data;
	if(pobject == NULL)
		return FALSE;

	name = dae_xml_get_attr(local->node, "name");
	if(name == NULL)
		name = dae_xml_get_attr(local->node, "id");
	if(name == NULL)
		return FALSE;

	object = g_new0(G3DObject, 1);
	object->name = name;
	pobject->objects = g_slist_append(pobject->objects, object);

	return dae_xml_parse(global, local->node, dae_chunks_geometry, object);
}

gboolean dae_cb_node(DaeGlobalData *global, DaeLocalData *local)
{
	G3DObject *object;
	gchar *name;

	name = dae_xml_get_attr(local->node, "name");
	if(name == NULL)
		name = dae_xml_get_attr(local->node, "id");
	if(name == NULL)
		return FALSE;

	object = g_new0(G3DObject, 1);
	object->name = name;
	global->model->objects = g_slist_append(global->model->objects, object);

	return dae_xml_parse(global, local->node, dae_chunks_node, object);
}

gboolean dae_cb_visual_scene(DaeGlobalData *global, DaeLocalData *local)
{
	return dae_xml_parse(global, local->node, dae_chunks_visual_scene, NULL);
}
