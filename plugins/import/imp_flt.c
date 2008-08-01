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

#include <g3d/types.h>
#include <g3d/context.h>
#include <g3d/read.h>
#include <g3d/material.h>

#include "imp_flt_opcodes.h"

FltOpcode *flt_opcode_info(guint32 opcode);

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model, gpointer user_data)
{
	FILE *f;
	guint16 opcode, rlen;
	FltOpcode *oi;
	FltGlobalData *gd;
	FltLocalData *ld;
	G3DObject *g3dobj = NULL;
	gpointer level_object = NULL;
	gchar *pad;

	f = fopen(filename, "rb");
	if(f == NULL)
	{
		g_warning("FLT: failed to open '%s'", filename);
		return FALSE;
	}

	gd = g_new0(FltGlobalData, 1);
	gd->context = context;
	gd->model = model;
	gd->f = f;
	gd->oqueue = g_queue_new();

	while(!feof(f))
	{
		/* get record information */
		opcode = g3d_read_int16_be(f);
		rlen = g3d_read_int16_be(f);

		/* create local data */
		ld = g_new0(FltLocalData, 1);
		ld->opcode = opcode;
		ld->nb = rlen - 4;
		ld->g3dobj = g3dobj;
		ld->level_object = level_object;

		if(opcode == 0)
		{
			/* end of file or error */
			break;
		}

		oi = flt_opcode_info(opcode);

		pad = g_strnfill(gd->level, ' ');
		g_debug("\\%s[%04d][%c] %8d: %s", pad, opcode,
			oi ? (oi->callback ? 'f' : ' ') : ' ',
			rlen,
			oi ? oi->description : "unknown");
		g_free(pad);

		if(oi && oi->callback)
			oi->callback(gd, ld);

		if(ld->nb > 0)
		{
			/* skip remaining bytes */
			fseek(f, ld->nb, SEEK_CUR);
		}

		/* free local data */
		g3dobj = ld->g3dobj;
		level_object = ld->level_object;
		g_free(ld);

		/* update caller */
		g3d_context_update_interface(context);
	}

	g_queue_free(gd->oqueue);
	if(gd->vertex_palette) {
		g_free(gd->vertex_palette->offsets);
		g_free(gd->vertex_palette->flags);
		g_free(gd->vertex_palette->vertex_data);
		g_free(gd->vertex_palette->normal_data);
		g_free(gd->vertex_palette->tex_vertex_data);
		g_free(gd->vertex_palette->vertex_materials);
		g_free(gd->vertex_palette);
	}
	g_free(gd);
	fclose(f);

	return TRUE;
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup("import plugin for OpenFlight files\n");
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("flt", ":", 0);
}

/*
 * FLT specific
 */

FltOpcode *flt_opcode_info(guint32 opcode)
{
	guint32 i;

	for(i = 0; flt_opcodes[i].opcode != 0; i ++)
	{
		if(flt_opcodes[i].opcode == opcode)
			return &(flt_opcodes[i]);
	}
	return NULL;
}

guint32 flt_read_typed_index(FILE *f, guint32 type, gint32 *len)
{
	guint32 val = 0;

	switch(type)
	{
		case 1:
			val = g3d_read_int8(f);
			*len -= 1;
			break;
		case 2:
			val = g3d_read_int16_be(f);
			*len -= 2;
			break;
		case 4:
			val = g3d_read_int32_be(f);
			*len -= 4;
			break;
		default:
			g_warning("FLT: unknown index type %d\n", type);
			break;
	}
	return val;
}

