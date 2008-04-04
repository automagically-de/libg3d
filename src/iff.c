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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <g3d/read.h>
#include <g3d/iff.h>
#include <g3d/context.h>
#include <g3d/debug.h>

FILE *g3d_iff_open(const gchar *filename, guint32 *id, guint32 *len)
{
	FILE *f;
	guint32 form_bytes, magic;

	f = fopen(filename, "r");
	if(f == NULL)
	{
		g_critical("can't open file '%s'", filename);
		return NULL;
	}

	magic = g3d_read_int32_be(f);
	if((magic != G3D_IFF_MKID('F','O','R','M')) &&
		(magic != G3D_IFF_MKID('F','O','R','4')))
	{
		g_critical("file %s is not an IFF file", filename);
		fclose(f);
		return NULL;
	}

	form_bytes = g3d_read_int32_be(f);
	*id = g3d_read_int32_be(f);
	form_bytes -= 4;
	*len = form_bytes;

	return f;
}

int g3d_iff_readchunk(FILE *f, guint32 *id, guint32 *len, guint32 flags)
{
	*id = g3d_read_int32_be(f);
	if(flags & G3D_IFF_LEN16)
	{
		*len = (flags & G3D_IFF_LE) ?
			g3d_read_int16_le(f) : g3d_read_int16_be(f);
		return 6 + *len + (*len % 2);
	}
	else
	{
		*len = (flags & G3D_IFF_LE) ?
			g3d_read_int32_le(f) : g3d_read_int32_be(f);
		return 8 + *len + (*len % 2);
	}
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

g3d_iff_chunk_info *g3d_iff_get_chunk_info(g3d_iff_chunk_info *chunks,
	guint32 chunk_id)
{
	guint32 i = 0;

	while(chunks[i].id && !g3d_iff_chunk_matches(chunk_id, chunks[i].id))
		i ++;

	if(chunks[i].id)
		return &(chunks[i]);

	return NULL;
}

void g3d_iff_debug_chunk(g3d_iff_chunk_info *info, guint32 chunk_id,
	guint32 chunk_len, gchar chunk_type, guint32 pos, guint32 level)
{
	gchar *tid;
	gchar *padding = "                                   ";

	tid = g3d_iff_id_to_text(chunk_id);
	g_debug("\\%s(%d)[%s][%c%c%c] %s (%d) @ 0x%08x - %d bytes left",
		padding + (strlen(padding) - level), level,
		tid,
		chunk_type,
		info->container ? 'c' : ' ',
		info->callback ? 'f' : ' ',
		info->description,
		chunk_len,
		pos,
		chunk_len);
	g_free(tid);
}

gpointer g3d_iff_handle_chunk(g3d_iff_gdata *global, g3d_iff_ldata *plocal,
	g3d_iff_chunk_info *chunks, guint32 flags)
{
	gpointer object = NULL;
	guint32 chunk_id, chunk_len;
	g3d_iff_ldata *sublocal;
	g3d_iff_chunk_info *info;

	/* read info for one chunk */
	g3d_iff_readchunk(global->f, &chunk_id, &chunk_len, 0);
	plocal->nb -= 8;

	sublocal = g_new0(g3d_iff_ldata, 1);
	sublocal->parent_id = plocal->id;
	sublocal->id = chunk_id;
	sublocal->object = plocal->object;
	sublocal->level = plocal->level + 1;
	sublocal->nb = chunk_len;
	plocal->nb -= sublocal->nb;

	/* call function */
	info = g3d_iff_get_chunk_info(chunks, chunk_id);
	if(info)
	{
		g3d_iff_debug_chunk(info, chunk_id, chunk_len, 'X',
			(guint32)(ftell(global->f) - 8), plocal->level);

		if(info->callback)
			info->callback(global, sublocal);

		if(sublocal->nb > 0)
			fseek(global->f, sublocal->nb, SEEK_CUR);
	}

	object = sublocal->level_object;
	g_free(sublocal);

	return object;
}

gboolean g3d_iff_read_ctnr(g3d_iff_gdata *global, g3d_iff_ldata *local,
	g3d_iff_chunk_info *chunks, guint32 flags)
{
	g3d_iff_ldata *sublocal;
	g3d_iff_chunk_info *info;
	guint32 chunk_id, chunk_len, chunk_mod, chunk_type;
	gchar *tid;
	gpointer level_object;
	long int fpos;

	level_object = NULL;

	if(global->max_fpos == 0)
		global->max_fpos = local->nb + 12;

	while(local->nb >= ((flags & G3D_IFF_LEN16) ? 6 : 8))
	{
		chunk_id = 0;

		g3d_iff_readchunk(global->f, &chunk_id, &chunk_len, flags);
		local->nb -= ((flags & G3D_IFF_LEN16) ? 6 : 8);

		chunk_mod = flags & 0x0F;
		if(chunk_mod == 0)
		{
			g_warning("[IFF] mod = 0 (flags: 0x%02X\n)", flags);
			chunk_mod = 2;
		}
		chunk_type = ' ';

		/* handle special chunks */
		switch(chunk_id)
		{
			case 0:
			case 0xFFFFFFFF:
				g_warning(
					"[IFF] got invalid ID, skipping %d bytes @ 0x%08x",
					local->nb, (unsigned int)ftell(global->f));

				/* skip rest of parent chunk */
				if(local->nb > 0)
				{
					fseek(global->f, local->nb, SEEK_CUR);
					local->nb = 0;
				}
				return FALSE;
				break;

			case G3D_IFF_MKID('F','O','R','4'):
				chunk_id = g3d_read_int32_be(global->f);
				chunk_len -= 4;
				chunk_mod = 4;
				chunk_type = 'F';
				local->nb -= 4;
				break;

			case G3D_IFF_MKID('L','I','S','4'):
				chunk_id = g3d_read_int32_be(global->f);
				chunk_len -= 4;
				chunk_mod = 4;
				chunk_type = 'L';
				local->nb -= 4;
				break;

			default:
				break;
		}

		info = g3d_iff_get_chunk_info(chunks, chunk_id);

		if(info)
		{
			g3d_iff_debug_chunk(info, chunk_id, chunk_len, chunk_type,
				(guint32)(ftell(global->f) - ((chunk_type == ' ') ? 8 : 12)),
				local->level);

			sublocal = g_new0(g3d_iff_ldata, 1);
			sublocal->parent_id = local->id;
			sublocal->id = chunk_id;
			sublocal->object = local->object;
			sublocal->level = local->level + 1;
			sublocal->level_object = level_object;
			sublocal->nb = chunk_len;

			if(info->callback)
			{
				info->callback(global, sublocal);
			}

			if(info->container)
			{
				/* LWO has 16 bit length in subchunks */
				if(flags & G3D_IFF_SUBCHUNK_LEN16)
				{
					g3d_iff_read_ctnr(global, sublocal, chunks,
						flags | G3D_IFF_LEN16);
				}
				else
				{
					g3d_iff_read_ctnr(global, sublocal, chunks, flags);
				}
			}

			if(info->container && info->callback)
			{
				sublocal->finalize = TRUE;
				info->callback(global, sublocal);
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
			g_warning("[IFF] unknown chunk type \"%s\" (%d) @ 0x%08x",
				tid, chunk_len, (unsigned int)ftell(global->f) - 8);
			g_free(tid);
			fseek(global->f, chunk_len, SEEK_CUR);
		}

		local->nb -= chunk_len;

		if(chunk_len % chunk_mod)
		{
			fseek(global->f, chunk_mod - (chunk_len % chunk_mod), SEEK_CUR);
			local->nb -= (chunk_mod - (chunk_len % chunk_mod));
		}

		fpos = ftell(global->f);
		g3d_context_update_progress_bar(global->context,
			((gfloat)fpos / (gfloat)global->max_fpos), TRUE);
	} /* nb >= 8/6 */

	if(local->nb > 0)
	{
		g_warning("[IFF] skipping %d bytes at the end of chunk",
			local->nb);

		fseek(global->f, local->nb, SEEK_CUR);
		local->nb = 0;
	}

	return TRUE;
}

