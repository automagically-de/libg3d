/*
    libg3d - 3D object loading library

    Copyright (C) 2015 - 2016  Markus Dahms <mad@automagically.de>

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
#include <g3d/debug.h>
#include <g3d/iff.h>

#include "imp_msfsmdl_chunks.h"

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	guint32 id, type;
	gsize len, r;
	G3DIffGlobal *global;
	G3DIffLocal *local;
	gboolean ret;

	r = g3d_iff_read_chunk(stream, &id, &len, G3D_IFF_LE);
	if (id != G3D_IFF_MKID('R','I','F','F')) {
		g_error("file is not a RIFF container");
		return FALSE;
	}

	type = g3d_stream_read_int32_be(stream);
	len -= 4;

	switch (type) {
	case G3D_IFF_MKID('M','D','L','8'):
		g_info("MDL8 file");
		break;
	case G3D_IFF_MKID('M','D','L','9'):
		g_info("MDL9 file");
		break;
	case G3D_IFF_MKID('M','D','L','X'):
		g_info("MDLX file");
		break;
	default: {
		gchar *t = g3d_iff_id_to_text(type);
		g_warning("unknown RIFF type %s", t);
		g_free(t);
		return FALSE;
	}
	}

	global = g_new0(G3DIffGlobal, 1);
	global->context = context;
	global->model = model;
	global->stream = stream;
	global->flags = G3D_IFF_LE | G3D_IFF_PAD2;
	global->user_data = NULL;

	local = g_new0(G3DIffLocal, 1);
	local->id = type;
	local->nb = len;

	ret =  g3d_iff_read_ctnr(global, local, mdl_chunks, G3D_IFF_LE);

	g_free(local);
	g_free(global);

	return ret;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for MS Flight Simulator mdl files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("mdl:msfsmdl", ":", 0);
}


