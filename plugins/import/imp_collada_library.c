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

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <g3d/types.h>
#include <g3d/material.h>

#include "imp_collada_xml.h"
#include "imp_collada_library.h"

static gboolean add_materials(ColladaLibrary *lib);
static gboolean lookup_effect(ColladaLibrary *lib, G3DMaterial *material,
	const gchar *id);

ColladaLibrary *col_library_init(xmlDocPtr xdoc, G3DModel *model)
{
	ColladaLibrary *lib;

	lib = g_new0(ColladaLibrary, 1);
	lib->model = model;
	lib->context = xmlXPathNewContext(xdoc);

	xmlXPathRegisterNs(lib->context, (xmlChar *)"c",
		(xmlChar *)"http://www.collada.org/2005/11/COLLADASchema");

	lib->materials = g_hash_table_new(g_str_hash, g_str_equal);
	lib->geometries = g_hash_table_new(g_str_hash, g_str_equal);

	add_materials(lib);

	return lib;
}

void col_library_free(ColladaLibrary *library)
{
	xmlXPathFreeContext(library->context);
	/* TODO: clean up */
	g_hash_table_destroy(library->materials);
	g_hash_table_destroy(library->geometries);
	g_free(library);
}

static gboolean add_materials(ColladaLibrary *lib)
{
	xmlXPathObjectPtr matsobj;
	xmlNodePtr child;
	guint32 i;
	const gchar *tmpstr;
	G3DMaterial *material;

	matsobj = xmlXPathEval((xmlChar *)"//c:library_materials/c:material",
		lib->context);
	if(matsobj == NULL)
		return FALSE; /* error in expression - should never happen */

	for(i = 0; i < xmlXPathNodeSetGetLength(matsobj->nodesetval); i ++)
	{
		material = g3d_material_new();
		tmpstr = col_xml_get_attribute(matsobj->nodesetval->nodeTab[i], "id");
		material->name = g_strdup(tmpstr ? tmpstr : "material w/o id");
		lib->model->materials = g_slist_append(lib->model->materials,
			material);
		child = matsobj->nodesetval->nodeTab[i]->children;
		while(child)
		{
			if(strcmp((char *)child->name, "instance_effect") == 0)
			{
				tmpstr = col_xml_get_attribute(child, "url");
				if(tmpstr && (tmpstr[0] == '#'))
					lookup_effect(lib, material, tmpstr + 1);
			}
			child = child->next;
		}
	}

	xmlXPathFreeObject(matsobj);

	return TRUE;
}

static gboolean lookup_effect(ColladaLibrary *lib, G3DMaterial *material,
	const gchar *id)
{
	xmlNodePtr node;
	gchar *tmp;

	/* diffuse color */
	tmp = g_strdup_printf(
		"//c:library_effects/c:effect[@id=\"%s\"]/c:profile_COMMON/"
		"c:technique[@sid=\"common\"]/c:phong/c:diffuse/c:color", id);
	node = col_xml_find_path(lib->context, tmp);
	g_debug("COLLADA: looking up effect %s diffuse color %s",
		id, node ? "succeeded" : "failed");
	g_free(tmp);
	tmp = NULL;
	if(node)
	{
		if(node->children)
			tmp = (gchar *)node->children->content;
		if(tmp)
		{
			sscanf(tmp, "%f %f %f %f", &(material->r), &(material->g),
				&(material->b), &(material->a));
		}
	}

	return TRUE;
}
