/* $Id: iff.c,v 1.1.2.2 2006/01/23 16:38:46 dahms Exp $ */

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <g3d/read.h>
#include <g3d/iff.h>
#include <g3d/context.h>

FILE *g3d_iff_open(const gchar *filename, guint32 *id, guint32 *len)
{
	FILE *f;
	guint32 form_bytes;

	f = fopen(filename, "r");
	if(f == NULL)
	{
		g_warning("can't open file '%s'", filename);
		return NULL;
	}

	if(g3d_read_int32_be(f) != G3D_IFF_MKID('F','O','R','M'))
	{
		g_warning("file %s is not an IFF file", filename);
		fclose(f);
		return NULL;
	}

	form_bytes = g3d_read_int32_be(f);
	*id = g3d_read_int32_be(f);
	form_bytes -= 4;
	*len = form_bytes;

	return f;
}

int g3d_iff_readchunk(FILE *f, guint32 *id, guint32 *len)
{
	*id = g3d_read_int32_be(f);
	*len = g3d_read_int32_be(f);
	return 8 + *len + (*len % 2);
}

gchar *g3d_iff_id_to_text(guint32 id)
{
	gchar *tid;

	tid = g_new0(gchar, 5);

	tid[0] = (id >> 24) & 0xFF;
	tid[1] = (id >> 16) & 0xFF;
	tid[2] = (id >> 8) & 0xFF;
	tid[3] = id & 0xFF;

	return tid;
}

gboolean g3d_iff_chunk_matches(guint32 id, gchar *tid)
{
	if(((id >> 24) & 0xFF) != tid[0]) return FALSE;
	if(((id >> 16) & 0xFF) != tid[1]) return FALSE;
	if(((id >> 8) & 0xFF) != tid[2]) return FALSE;
	return (id & 0xFF) == tid[3];
}

gboolean g3d_iff_read_ctnr(g3d_iff_gdata *global, g3d_iff_ldata *local,
	g3d_iff_chunk_info *chunks, gboolean chunk_even_len)
{
	g3d_iff_ldata *sublocal;
	guint32 chunk_id, chunk_len;
	gint32 i;
#if DEBUG > 0
	gint32 j;
#endif
	gchar *tid;
	gpointer level_object;

	level_object = NULL;

	while(local->nb > 0)
	{
		chunk_id = 0;

		g3d_iff_readchunk(global->f, &chunk_id, &chunk_len);
		local->nb -= 8;

		if(chunk_id == 0)
		{
			/* skip rest of parent chunk */
			fseek(global->f, local->nb, SEEK_CUR);
			local->nb = 0;
			return FALSE;
		}

		i = 0;
		while(chunks[i].id && !g3d_iff_chunk_matches(chunk_id, chunks[i].id))
			i ++;

		if(chunks[i].id)
		{
#if DEBUG > 0
			for(j = 0; j < local->level; j ++) g_printerr("  ");
			tid = g3d_iff_id_to_text(chunk_id);
			g_printerr("[%s] %s (%d)\n", tid, chunks[i].description,
				chunk_len);
			g_free(tid);
#endif

			sublocal = g_new0(g3d_iff_ldata, 1);
			sublocal->parent_id = local->id;
			sublocal->id = chunk_id;
			sublocal->object = local->object;
			sublocal->level = local->level + 1;
			sublocal->level_object = level_object;
			sublocal->nb = chunk_len;

			if(chunks[i].callback)
			{
				chunks[i].callback(global, sublocal);
			}

			if(chunks[i].container)
			{
				g3d_iff_read_ctnr(global, sublocal, chunks, chunk_even_len);
			}

			if(sublocal->nb > 0)
			{
				fseek(global->f, sublocal->nb, SEEK_CUR);
			}

			level_object = sublocal->level_object;

			g_free(sublocal);
		}
		else
		{
			tid = g3d_iff_id_to_text(chunk_id);
			g_printerr("[IFF] unknown chunk type \"%s\" (%d)\n", tid,
				chunk_len);
			g_free(tid);
			fseek(global->f, chunk_len, SEEK_CUR);
		}

		local->nb -= chunk_len;

		if(chunk_even_len && (chunk_len % 2))
		{
			fseek(global->f, 1, SEEK_CUR);
			local->nb -= 1;
		}

		g3d_context_update_interface(global->context);
	} /* nb > 0 */

	return TRUE;
}

