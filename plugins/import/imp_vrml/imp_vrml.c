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

#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <glib.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/material.h>

#include "imp_vrml.h"
#include "imp_vrml_v1_scanner.h"

#define VRML_FT_VRML      0x01
#define VRML_FT_INVENTOR  0x02

#define MAX_LINE_SIZE     2048

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar line[MAX_LINE_SIZE + 1];
	guint32 ver_maj, ver_min, filetype;

	memset(line, 0, MAX_LINE_SIZE);
	g3d_stream_read_line(stream, line, MAX_LINE_SIZE);
	if(strncmp(line, "#VRML", 5) == 0)
		filetype = VRML_FT_VRML;
	else if(strncmp(line, "#Inventor", 9) == 0)
		filetype = VRML_FT_INVENTOR;
	else {
		g_warning("file '%s' is not a VRML file", stream->uri);
		return FALSE;
	}

	/* FIXME: more than one space between VRML and Vx.x? */
	ver_maj = line[7] - '0';
	ver_min = line[9] - '0';

#if DEBUG > 0
	g_debug("VRML: version %d.%d", ver_maj, ver_min);
#endif

	setlocale(LC_NUMERIC, "C");

	if((filetype == VRML_FT_INVENTOR) || (ver_maj == 1)) {
		/* Inventor / VRML 1.x */
		/* read complete file to buffer */

		VrmlGlobal *global = g_new0(VrmlGlobal, 1);
		global->stream = stream;
		global->context = context;
		global->model = model;
		global->stack = g_queue_new();

		vrml_v1_scan(global);

		g_queue_free(global->stack);
		g_free(global);

		return TRUE;
	} else if(ver_maj == 2) {
		g_warning("VRML 2 is not yet supported");
		return FALSE;
	} else {
		g_warning("unknown VRML version %d.%d", ver_maj, ver_min);
		return FALSE;
	}

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup("import plugin for VRML 1.x & SGI inventor files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("vrml:iv", ":", 0);
}

