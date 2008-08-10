/* $Id$ */

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

#include <g3d/types.h>
#include <g3d/stream.h>

#include "imp_ldraw_types.h"
#include "imp_ldraw_part.h"
#include "imp_ldraw_library.h"

gpointer plugin_init(G3DContext *context)
{
	return ldraw_library_init();
}

void plugin_cleanup(gpointer user_data)
{
	LDrawLibrary *lib = user_data;

	ldraw_library_cleanup(lib);
}

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	G3DObject *object;
	LDrawLibrary *lib = user_data;
	LDrawPart *part;

	part = g_new0(LDrawPart, 1);
	part->name = g_path_get_basename(stream->uri);
	part->stream = stream;

	object = ldraw_part_get_object(part, lib);
	if(object)
		model->objects = g_slist_append(model->objects, object);

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for LDraw files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("ldr:mpd:dat", ":", 0);
}

/*****************************************************************************/
