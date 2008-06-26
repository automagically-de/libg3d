#include <string.h>

#include <glib.h>
#include <libxml/tree.h>

#include "imp_dae_library.h"
#include "imp_dae_cb.h"

gchar *dae_xml_get_attr(xmlNodePtr node, const gchar *attrname)
{
	xmlAttrPtr attr;
	gchar *val = NULL;

	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(attrname != NULL, NULL);

	attr = node->properties;
	while(attr != NULL) {
		if(xmlStrcmp(attr->name, (xmlChar *)attrname) == 0) {
			val = g_strdup((gchar *)attr->children->content);
			break;
		}
		attr = attr->next;
	}

	return val;
}

xmlNodePtr dae_xml_get_child_by_tagname(xmlNodePtr parent, const gchar *tag)
{
	xmlNodePtr node;

	g_return_val_if_fail(parent != NULL, NULL);

	node = parent->children;
	while(node != NULL) {
		if((node->type == XML_ELEMENT_NODE) &&
			(xmlStrcmp(node->name, (const xmlChar *)tag) == 0)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr dae_xml_next_child(DaeLibrary *lib, xmlNodePtr parent,
	xmlNodePtr *node, xmlNodePtr *instance, gchar **nodename)
{
	gchar *url, *name;

	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(instance != NULL, NULL);
	g_return_val_if_fail(nodename != NULL, NULL);
	*instance = NULL;
	*nodename = NULL;

	if(*node == NULL)
		*node = parent->children;
	else
		*node = (*node)->next;

	/* skip TEXT nodes */
	while(*node && ((*node)->type != XML_ELEMENT_NODE))
		*node = (*node)->next;

	if(*node == NULL)
		return NULL;

	if(xmlStrncmp((*node)->name, (xmlChar *)"instance_", 9) == 0) {
		url = dae_xml_get_attr(*node, "url");
		if(url) {
			/* skip 'instance_' part of node name */
			name = g_strdup((gchar *)((*node)->name + 9));
			g_debug("DAE: looking up '%s' in '%s'", url + 1, name);
			*instance = *node;
			*node = dae_library_lookup(lib, name, url + 1 /* skip '#' */);
			g_free(url);
			g_free(name);
		}
	}
	if(*node == NULL)
		return NULL;

	*nodename = g_strdup((gchar *)(*node)->name);

	return *node;
}

gboolean dae_xml_parse(DaeGlobalData *global, xmlNodePtr parent,
	DaeChunkDesc *chunks, gpointer user_data)
{
	DaeLocalData *local;
	xmlNodePtr node = NULL, instance;
	gchar *name;
	gint i;

	g_return_val_if_fail(parent != NULL, FALSE);

	while(dae_xml_next_child(global->lib, parent, &node, &instance, &name)) {
		local = g_new0(DaeLocalData, 1);
		local->parent = parent;
		local->node = node;
		local->instance = instance;
		local->user_data = user_data;

		/* find callback */
		for(i = 0; chunks[i].name != NULL; i ++) {
			if(strcmp(chunks[i].name, name) == 0) {
				/* found chunk */
				g_debug("DAE: found chunk description for '%s'", name);
				if(chunks[i].callback)
					chunks[i].callback(global, local);
				break;
			}
		}

		if(chunks[i].name == NULL)
			g_debug("DAE: unhandled chunk '%s' in '%s'", name,
				(gchar *)parent->name);

		g_debug("DAE: dae_xml_parse: node '%s'", name);
		g_free(name);
		g_free(local);
	}

	return TRUE;
}
