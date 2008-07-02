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

#include <locale.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <g3d/types.h>
#include <g3d/stream.h>

static int kml_stream_read_cb(gpointer ctx, gchar *buffer, gint len)
{
	return g3d_stream_read((G3DStream *)ctx, buffer, 1, len);
}

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model)
{
	G3DStream *stream_dockml, *stream_model;
	xmlDocPtr xmldoc;

	setlocale(LC_NUMERIC, "C");

	xmlInitParser();

	stream_dockml = g3d_stream_open_zip(filename, "doc.kml");
	if(stream_dockml == NULL) {
		g_warning("KMZ: failed to read 'doc.kml' from '%s'", filename);
		return FALSE;
	}

	xmldoc = xmlReadIO(kml_stream_read_cb, NULL,
		stream_dockml, "file:///tmp/doc.kml", NULL, 0);
	if(xmldoc) {
		g_debug("KMZ: parsed doc.kml");

		xmlFreeDoc(xmldoc);
	}

	g3d_stream_close(stream_dockml);
	xmlCleanupParser();

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup("import plugin for .kmz model containers\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("kmz", ":", 0);
}

