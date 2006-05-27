/* $Id: imp_iob.c,v 1.1.2.2 2006/01/23 17:03:06 dahms Exp $ */

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
#include <g3d/read.h>
#include <g3d/iff.h>

#include "imp_iob_chunks.h"

int iob_read_directory(FILE *f, guint32 nbytes, G3DModel *model,
	void *pobject, guint32 parentid, int level, G3DContext *context);

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer user_data)
{
	g3d_iff_gdata *global;
	g3d_iff_ldata *local;
	guint32 id, len;
	FILE *f;

	f = g3d_iff_open(filename, &id, &len);
	if(id != G3D_IFF_MKID('T','D','D','D'))
	{
		g_warning("file is not an .iob (TDDD) file %s", filename);
		fclose(f);
		return FALSE;
	}

	local = g_new0(g3d_iff_ldata, 1);
	global = g_new0(g3d_iff_gdata, 1);

	global->context = context;
	global->model = model;
	global->f = f;

	local->parent_id = id;
	local->nb = len;

	g3d_iff_read_ctnr(global, local, iob_chunks);

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

gfloat iob_read_fract(FILE *f)
{
	gint32 i = g3d_read_int32_be(f);
	return (gfloat)(i / 0xFFFF);
}

