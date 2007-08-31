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

#ifndef IMP_COLLADA_XML_H
#define IMP_COLLADA_XML_H

#include <glib.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

xmlNodePtr col_xml_find_node(xmlNodePtr parent, const gchar *name);
xmlNodePtr col_xml_find_path(xmlXPathContextPtr context, const gchar *path);
const gchar *col_xml_get_attribute(xmlNodePtr node, const gchar *attrname);

#endif /* IMP_COLLADA_XML_H */
