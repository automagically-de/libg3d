/* $Id:$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005, 2006  Markus Dahms <mad@automagically.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <g3d/stream.h>
#include <g3d/types.h>

#include "imp_dae_xml.h"
#include "imp_dae_library.h"

static int dae_input_read_cb(gpointer ctx, gchar *buffer, gint len)
{
	return g3d_stream_read((G3DStream *)ctx, buffer, 1, len);
}

static gboolean dae_load_scene(G3DModel *model, DaeLibrary *lib,
	xmlDocPtr xmldoc);

/*****************************************************************************/

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	xmlDocPtr xmldoc;
	DaeLibrary *lib;
	gboolean retval = FALSE;

	xmlInitParser();

	xmldoc = xmlReadIO(dae_input_read_cb, NULL, stream, stream->uri, NULL, 0);
	if(xmldoc) {
		lib = dae_library_load(xmldoc);
		retval = dae_load_scene(model, lib, xmldoc);

		dae_library_cleanup(lib);
		xmlFreeDoc(xmldoc);
	}

	xmlCleanupParser();
	return retval;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for COLLADA .dae files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("dae", ":", 0);
}

/*****************************************************************************/
/* COLLADA specific stuff                                                    */

static xmlNodePtr dae_next_child(DaeLibrary *lib, xmlNodePtr parent,
	xmlNodePtr *node, xmlNodePtr *instance, gchar **nodename)
{
	gchar *url, *name;
	xmlAttrPtr attr;

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
		attr = (*node)->properties;
		while(attr && (xmlStrcmp(attr->name, (xmlChar *)"url")))
			attr = attr->next;

		if(attr) {
			/* skip leading '#' in url attribute */
			url = g_strdup((gchar *)attr->children->content + 1);
			/* skip 'instance_' part of node name */
			name = g_strdup((gchar *)((*node)->name + 9));
			g_debug("DAE: looking up '%s' in '%s'", url, name);
			*instance = *node;
			*node = dae_library_lookup(lib, name, url);
			g_free(url);
			g_free(name);
		}
	}
	if(*node == NULL)
		return NULL;

	*nodename = g_strdup((gchar *)(*node)->name);

	return *node;
}

static gboolean dae_load_scene(G3DModel *model, DaeLibrary *lib,
	xmlDocPtr xmldoc)
{
	xmlNodePtr scenenode, node = NULL, instance;
	gchar *name;

	scenenode = dae_xml_get_child_by_tagname(
		xmlDocGetRootElement(xmldoc), "scene");

	if(scenenode == NULL) {
		g_warning("DAE: could not get scene node");
		return FALSE;
	}

	while(dae_next_child(lib, scenenode, &node, &instance, &name)) {
		g_debug("DAE: got node %s", name);
		g_free(name);
	}

	return TRUE;
}
