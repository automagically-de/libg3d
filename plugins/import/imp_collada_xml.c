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

#include <string.h>
#include <glib.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

xmlNodePtr col_xml_find_node(xmlNodePtr parent, const gchar *name)
{
	xmlNodePtr node;

	node = parent->children;
	while(node != NULL)
	{
		if(strcmp(name, (char *)node->name) == 0)
			return node;
		node = node->next;
	}
	return NULL;
}

xmlNodePtr col_xml_find_path(xmlXPathContextPtr context, const gchar *path)
{
	xmlXPathObjectPtr obj;
	xmlNodePtr node = NULL;

	obj = xmlXPathEval((const xmlChar *)path, context);

	g_print("COLLADA:XML:XPATH: got %d nodes in set\n",
		xmlXPathNodeSetGetLength(obj->nodesetval));

	if(xmlXPathNodeSetGetLength(obj->nodesetval) == 1)
	{
		g_debug("COLLADA:XML: got a node from path");
		node = obj->nodesetval->nodeTab[0];
	}

	xmlXPathFreeObject(obj);

	return node;
}

const gchar *col_xml_get_attribute(xmlNodePtr node, const gchar *attrname)
{
	xmlAttrPtr attr;

	attr = node->properties;
	while(attr)
	{
		if(strcmp((char *)attr->name, attrname) == 0)
			return (const gchar *)attr->children->content;
		attr = attr->next;
	}

	return NULL;
}
