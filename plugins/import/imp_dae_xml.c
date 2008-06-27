#include <string.h>
#include <ctype.h>

#include <g3d/context.h>

#include <glib.h>
#include <libxml/tree.h>

#include "imp_dae_library.h"
#include "imp_dae_xml.h"
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

xmlNodePtr dae_xml_next_child_by_tagname(xmlNodePtr parent, xmlNodePtr *node,
	gchar *nodename)
{
	xmlNodePtr tmpnode;
	gchar *name;

	do {
		tmpnode = dae_xml_next_child(NULL, parent, node, NULL, &name);
		if(tmpnode == NULL)
			return NULL;
		if(strcmp(name, nodename) == 0) {
			g_free(name);
			return tmpnode;
		}
		g_free(name);
	} while(1);
}

xmlNodePtr dae_xml_next_child(DaeLibrary *lib, xmlNodePtr parent,
	xmlNodePtr *node, xmlNodePtr *instance, gchar **nodename)
{
	gchar *url, *name;

	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(nodename != NULL, NULL);
	*nodename = NULL;

	if(*node == NULL)
		*node = parent->children;
	else if(instance && *instance)
		*node = (*instance)->next;
	else
		*node = (*node)->next;

	if(instance)
		*instance = NULL;

	/* skip TEXT nodes */
	while(*node && ((*node)->type != XML_ELEMENT_NODE))
		*node = (*node)->next;

	if(*node == NULL)
		return NULL;

	if(lib && instance &&
		(xmlStrncmp((*node)->name, (xmlChar *)"instance_", 9) == 0)) {
		url = dae_xml_get_attr(*node, "url");
		if(url) {
			/* skip 'instance_' part of node name */
			name = g_strdup((gchar *)((*node)->name + 9));
#if DEBUG > 1
			g_debug("DAE: looking up '%s' in '%s'", url + 1, name);
#endif
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
	DaeChunkDesc *chunks, guint32 level, gpointer user_data)
{
	DaeLocalData *local;
	xmlNodePtr node = NULL, instance = NULL;
	gchar *name;
	static gchar *padding = "                                  ";
	gint i;

	g_return_val_if_fail(parent != NULL, FALSE);

	while(dae_xml_next_child(global->lib, parent, &node, &instance, &name)) {

		g_debug("\\%s<%s>",
			padding + (strlen(padding) - level),
			name);

		if(chunks) {
			local = g_new0(DaeLocalData, 1);
			local->parent = parent;
			local->node = node;
			local->instance = instance;
			local->level = level + 1;
			local->user_data = user_data;

			/* find callback */
			for(i = 0; chunks[i].name != NULL; i ++) {
				if(strcmp(chunks[i].name, name) == 0) {
					/* found chunk */
#if DEBUG > 3
					g_debug("DAE: found chunk description for '%s'", name);
#endif
					if(chunks[i].callback)
						chunks[i].callback(global, local);
					break;
				}
			}

			g_free(local);

			if(chunks[i].name == NULL)
				g_debug("DAE: unhandled chunk '%s' in '%s'", name,
					(gchar *)parent->name);
		}

		g_free(name);
		g3d_context_update_interface(global->context);
	}

	return TRUE;
}

gboolean dae_xml_next_int(xmlNodePtr node, gchar **nextp, gint *i)
{
	gchar *s, *err = NULL;

	s = *nextp;

	if(s == NULL)
		s = (gchar *)node->children->content;

	/* skip leading whitespace */
	while(isspace(*s))
		s ++;

	*i = strtol(s, &err, 0);
	if(s == err) {
		g_debug("DAE: imp_xml_next_int: error at '%.*s...'", 5, s);
		return FALSE;
	}

	*nextp = err;
	return TRUE;
}

gboolean dae_xml_next_float(xmlNodePtr node, gchar **nextp, gfloat *f)
{
	gchar *s, *err = NULL;

	s = *nextp;

	if(s == NULL)
		s = (gchar *)node->children->content;

	/* skip leading whitespace */
	while(isspace(*s))
		s ++;

	*f = strtod(s, &err);
	if(s == err) {
		g_debug("DAE: imp_xml_next_float: error at '%.*s...'", 5, s);
		return FALSE;
	}

	*nextp = err;
	return TRUE;
}