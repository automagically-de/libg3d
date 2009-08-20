/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2009  Markus Dahms <mad@automagically.de>

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

#ifdef HAVE_CONFIG_H
#	include <g3d/config.h>
#endif

#include <locale.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/plugins.h>

static gchar *kmz_find_model(xmlDocPtr xmldoc);
static void kmz_add_metadata(xmlDocPtr xmldoc, G3DModel *model);

static int kml_stream_read_cb(gpointer ctx, gchar *buffer, gint len)
{
	return g3d_stream_read((G3DStream *)ctx, buffer, len);
}

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model)
{
	G3DStream *stream_dockml, *stream_model;
	gchar *daename;
	xmlDocPtr xmldoc;
	gboolean retval = FALSE, from_zip = TRUE;

	setlocale(LC_NUMERIC, "C");

	xmlInitParser();

	stream_dockml = g3d_stream_open_zip_from_stream(stream, "doc.kml");
	if(stream_dockml == NULL) {
		g_debug("KMZ: failed to read 'doc.kml' from '%s', trying to "
			"use input stream as 'doc.kml'", stream->uri);
		stream_dockml = stream;
		from_zip = FALSE;
	}

	xmldoc = xmlReadIO(kml_stream_read_cb, NULL,
		stream_dockml, "file:///tmp/doc.kml", NULL, 0);
	if(xmldoc) {
		g_debug("KMZ: parsed doc.kml");

		daename = kmz_find_model(xmldoc);
		if(daename != NULL) {
#if DEBUG > 1
			g_debug("KMZ: loading '%s' from '%s'", daename, filename);
#endif
			if(from_zip)
				stream_model = g3d_stream_open_zip_from_stream(stream,
					daename);
			else
				stream_model = g3d_stream_open_file(daename, "r");

			if(stream_model != NULL) {
				retval = g3d_plugins_load_model_from_stream(context,
					stream_model, model);
				g3d_stream_close(stream_model);
			} else {
				g_warning("KMZ: failed to find '%s'%s%s%s", daename,
					from_zip ? " in '" : "",
					from_zip ? stream->uri : "",
					from_zip ? "'" : "");
			}
		}

		if(retval) {
			/* model successfully loaded, try to add metadata */
			kmz_add_metadata(xmldoc, model);
		}

		xmlFreeDoc(xmldoc);
	}

	if(from_zip)
		g3d_stream_close(stream_dockml);
	xmlCleanupParser();

	return retval;
}

gchar *plugin_description(void)
{
	return g_strdup("import plugin for .kmz model containers\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("kmz:kml:zip", ":", 0);
}

/*****************************************************************************/

static xmlNodePtr kmz_find_node(xmlNodePtr parentnode, const gchar *path)
{
	gchar *slash, *elem;
	xmlNodePtr node, pathnode;
	gboolean last = FALSE;

	slash = strchr(path, '/');
	if(slash)
		elem = g_strndup(path, slash - path);
	else {
		elem = g_strdup(path);
		last = TRUE;
	}

	if(strlen(elem) == 0)
		return NULL;

	for(node = parentnode->children; node != NULL; node = node->next) {
		if(node->type != XML_ELEMENT_NODE)
			continue;
		if(xmlStrcmp(node->name, (xmlChar *)elem) == 0) {
			if(last) {
				g_free(elem);
				return node;
			} else {
				pathnode = kmz_find_node(node, slash + 1);
				if(pathnode != NULL) {
					g_free(elem);
					return pathnode;
				}
			}
		}
	}
	g_free(elem);
	return NULL;
}

static gchar * kmz_find_model(xmlDocPtr xmldoc)
{
	xmlNodePtr rootnode, hrefnode;

	rootnode = xmlDocGetRootElement(xmldoc);
	if(rootnode == NULL)
		return NULL;

	hrefnode = kmz_find_node(rootnode, "Folder/Placemark/Model/Link/href");
	if(hrefnode)
		return (gchar *)hrefnode->children->content;
	hrefnode = kmz_find_node(rootnode, "Placemark/Model/Link/href");
	if(hrefnode)
		return (gchar *)hrefnode->children->content;
	return NULL;
}

struct _KmzMetadataMap {
	const gchar *xmlpath;
	G3DMetaDataTypeHint typehint;
	const gchar *name;
};

static struct _KmzMetadataMap kmz_metadata_map[] = {
	{ "Folder/name",               G3D_MDT_OTHER,           "name" },
	{ "Folder/Placemark/description",  G3D_MDT_DC_DESCRIPTION, "description" },
	{ "Folder/Placemark/Model/Location/altitude", G3D_MDT_OTHER, "altitude" },
	{ "Folder/Placemark/Model/Location/latitude", G3D_MDT_OTHER, "latitude" },
	{ "Folder/Placemark/Model/Location/longitude",G3D_MDT_OTHER, "longitude" },
	{ NULL }
};

static void kmz_add_metadata(xmlDocPtr xmldoc, G3DModel *model)
{
	xmlNodePtr rootnode, node;
	G3DMetaDataItem *mditem;
	gint32 i;

	rootnode = xmlDocGetRootElement(xmldoc);
	if(rootnode == NULL)
		return;

	for(i = 0; kmz_metadata_map[i].xmlpath != NULL; i ++) {
		node = kmz_find_node(rootnode, kmz_metadata_map[i].xmlpath);
		if(node) {
			mditem = g_new0(G3DMetaDataItem, 1);
			mditem->name = g_strdup(kmz_metadata_map[i].name);
			mditem->typehint = kmz_metadata_map[i].typehint;
			mditem->value = g_strdup((gchar *)node->children->content);
			model->metadata = g_slist_append(model->metadata, mditem);
		}
	}
}

