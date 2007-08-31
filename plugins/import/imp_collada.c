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
#include <locale.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <g3d/types.h>

#include "imp_collada_xml.h"
#include "imp_collada_library.h"

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model)
{
	ColladaLibrary *lib;
	xmlDocPtr xdoc;

	setlocale(LC_NUMERIC, "C");

	xmlInitParser();
	xmlXPathInit();

	xdoc = xmlParseFile(filename);
	if(xdoc == NULL)
	{
		g_warning("COLLADA: failed to parse XML file '%s'", filename);
		xmlCleanupParser();
		return FALSE;
	}

	lib = col_library_init(xdoc, model);

	col_library_free(lib);

	xmlFreeDoc(xdoc);
	xmlCleanupParser();

	return TRUE;
}

char *plugin_description(void)
{
	return g_strdup("import plugin for COLLADA files\n");
}

char **plugin_extensions(void)
{
	return g_strsplit("dae", ":", 0);
}

