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

#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <g3d/types.h>
#include <g3d/context.h>
#include <g3d/material.h>
#include <g3d/stream.h>
#include <g3d/iff.h>

#include "imp_iob_chunks.h"

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	G3DIffGlobal *global;
	G3DIffLocal *local;
	guint32 id, len;

	if(!g3d_iff_check(stream, &id, &len) ||
		(id != G3D_IFF_MKID('T','D','D','D'))) {
		g_warning("file is not an .iob (TDDD) file %s", stream->uri);
		return FALSE;
	}

	local = g_new0(G3DIffLocal, 1);
	global = g_new0(G3DIffGlobal, 1);

	global->context = context;
	global->model = model;
	global->stream = stream;

	local->parent_id = id;
	local->nb = len;

	g3d_iff_read_ctnr(global, local, iob_chunks, G3D_IFF_PAD2);

	g_free(local);
	g_free(global);

	return TRUE;
}

char *plugin_description(void)
{
	return g_strdup(
		"import plugin for Impulse Turbo Silver / Imagine objects\n");
}

char **plugin_extensions(void)
{
	return g_strsplit("iob", ":", 0);
}


/*****************************************************************************/
/* IOB specific                                                              */
/*****************************************************************************/

gfloat iob_read_fract(G3DStream *stream)
{
	gint32 i = g3d_stream_read_int32_be(stream);
	return (gfloat)(i / 0xFFFF);
}

