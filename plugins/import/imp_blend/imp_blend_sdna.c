/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2008  Markus Dahms <mad@automagically.de>

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

#include <g3d/debug.h>

#include "imp_blend_def.h"
#include "imp_blend_types.h"
#include "imp_blend_sdna.h"
#include "imp_blend_read.h"

#define BLEND_DEBUG_STRUCT 0

BlendSdna *blend_sdna_read_dna1(G3DStream *stream, guint32 flags, gint32 len)
{
	BlendSdna *sdna;
	BlendSdnaProperty *sprop;
	BlendSdnaStruct *sstruct;
	guint32 code, n, n2, nn, m, t;
	gint32 i, j;
	gchar buf[2048], *bufp, c, **aptr;

	code = blend_read_uint(stream, flags);
	len -= 4;
	if(code != MKID('S','D','N','A')) {
		g_warning("Blend: DNA1: no SDNA");
		g3d_stream_skip(stream, len);
		return NULL;
	}

	sdna = g_new0(BlendSdna, 1);

	while(len > 0) {
		code = blend_read_uint(stream, flags);
		len -= 4;

		switch(code) {
			case MKID('N','A','M','E'):
			case MKID('T','Y','P','E'):
				n = blend_read_uint(stream, flags);
				len -= 4;
				if(code == MKID('T','Y','P','E')) {
					g_debug("\\ TYPE: %d types", n);
					sdna->n_types = n;
					sdna->type_names = g_new0(gchar *, n);
					sdna->type_sizes = g_new0(guint32, n);
					aptr = sdna->type_names;
				} else {
					g_debug("\\ NAME: %d names", n);
					sdna->n_names = n;
					sdna->names = g_new0(gchar *, n);
					aptr = sdna->names;
				}
				m = 0;
				for(i = 0; i < n; i ++) {
					bufp = buf;
					do {
						c = *bufp = g3d_stream_read_int8(stream);
						bufp ++;
						len --;
						m ++;
					} while(c != '\0');
					aptr[i] = g_strdup(buf);
#if DEBUG > 2
					g_debug("\\  %s", buf);
#endif
				}
				m %= 4;
				if(m > 0) {
					/* padding */
					g3d_stream_skip(stream, 4 - m);
					len -= (4 - m);
				}
				break;
			case MKID('T','L','E','N'):
				g_debug("\\ TLEN: %d type sizes", sdna->n_types);
				for(i = 0; i < sdna->n_types; i ++) {
					m = blend_read_short(stream, flags);
					len -= 2;
#if DEBUG > 2
					g_debug("\\  %d", m);
#endif
					sdna->type_sizes[i] = m;
				}
				m = sdna->n_types % 2;
				if(m > 0) {
					g3d_stream_skip(stream, 2);
					len -= 2;
				}
				break;
			case MKID('S','T','R','C'):
				n = blend_read_uint(stream, flags);
				g_debug("\\ STRC: %d structs", n);
				len -= 4;
				for(i = 0; i < n; i ++) {
					t = blend_read_short(stream, flags);
					n2 = blend_read_short(stream, flags);
					len -= 4;
					sstruct = g_new0(BlendSdnaStruct, 1);
					sstruct->name = sdna->type_names[t];
					sstruct->size = sdna->type_sizes[t];
					sdna->structs = g_slist_append(sdna->structs, sstruct);
					for(j = 0; j < n2; j ++) {
						t = blend_read_short(stream, flags);
						nn = blend_read_short(stream, flags);
						len -= 4;
						sprop = g_new0(BlendSdnaProperty, 1);
						sprop->name = sdna->names[nn];
						sprop->tname = sdna->type_names[t];
						sprop->tid = t;
						sprop->tsize = sdna->type_sizes[t];
						sprop->ptr = (sprop->name[0] == '*');
						sstruct->properties = g_slist_append(
							sstruct->properties, sprop);
					}
				}
				break;
			default:
				g_warning("Blend: DNA1: unexpected section 0x%x", code);
				return sdna;
				break;
		}
	}
	return sdna;
}

static void blend_sdna_struct_free(BlendSdnaStruct *sstruct)
{
	GSList *item, *next;
	BlendSdnaProperty *sprop;

	item = sstruct->properties;
	while(item) {
		sprop = item->data;
		next = item->next;
		g_free(sprop);
		g_slist_free_1(item);
		item = next;
	}
	g_free(sstruct);
}

void blend_sdna_cleanup(BlendSdna *sdna)
{
	GSList *item, *next;
	BlendSdnaStruct *sstruct;

	g_return_if_fail(sdna != NULL);

	/* names */
	g_free(sdna->names);
	/* types */
	g_free(sdna->type_names);
	g_free(sdna->type_sizes);
	/* structs */
	item = sdna->structs;
	while(item) {
		sstruct = item->data;
		next = item->next;
		blend_sdna_struct_free(sstruct);
		g_slist_free_1(next);
		item = next;
	}
	/* sdna */
	g_free(sdna);
}

const BlendSdnaStruct *blend_sdna_get_struct_by_id(BlendSdna *sdna,
	guint32 sdnanr)
{
	g_return_val_if_fail(sdnanr < g_slist_length(sdna->structs), NULL);

	if(sdnanr < 10)
		return NULL;

	return g_slist_nth_data(sdna->structs, sdnanr);
}

const BlendSdnaStruct *blend_sdna_get_struct_by_name(BlendSdna *sdna,
	const gchar *name)
{
	/* TODO */
	return NULL;
}

BlendSdnaData *blend_sdna_data_read(BlendSdna *sdna,
	const BlendSdnaStruct *sstruct, BlendGlobal *global, gsize *r,
	guint32 level)
{
	BlendSdnaData *sdata;
	BlendSdnaProperty *sprop;
	BlendSdnaPropData *spdata;
	const BlendSdnaStruct *substruct;
	GSList *pitem;

	sdata = g_new0(BlendSdnaData, 1);
	sdata->sstruct = sstruct;
	sdata->prophash = g_hash_table_new(g_str_hash, g_str_equal);

#if DEBUG > BLEND_DEBUG_STRUCT
	g_debug("|%s{",
		debug_pad(level * 2));
#endif

	for(pitem = sstruct->properties; pitem != NULL; pitem = pitem->next) {
		sprop = pitem->data;
		spdata = g_new0(BlendSdnaPropData, 1);
		spdata->name = g_strdup(sprop->name);
		spdata->sprop = sprop;
		g_hash_table_insert(sdata->prophash, spdata->name, spdata);
		spdata->type = (sprop->tid < 10) ? sprop->tid : 0xFF;

		if(sprop->ptr) {
			spdata->pval = g3d_stream_read_int32_le(global->stream);
			(*r) -= 4;
#if DEBUG > BLEND_DEBUG_STRUCT
			g_debug("|%s  %-10s %-10s; /* len = %d */",
				debug_pad(level * 2),
				sprop->tname, sprop->name, *r);
#endif
			continue;
		}
		switch(spdata->type) {
			case T_CHAR:
				spdata->ival = g3d_stream_read_int8(global->stream);
				(*r) -= 1;
				break;
			case T_UCHAR:
				spdata->uval = g3d_stream_read_int8(global->stream);
				(*r) -= 1;
				break;
			case T_SHORT: /* FIXME: BE/LE */
				spdata->ival = g3d_stream_read_int16_le(global->stream);
				(*r) -= 2;
				break;
			case T_USHORT:
				spdata->uval = g3d_stream_read_int16_le(global->stream);
				(*r) -= 2;
				break;
			case T_INT: /* FIXME: 32/64 bit */
				spdata->ival = g3d_stream_read_int32_le(global->stream);
				(*r) -= 4;
				break;
			case T_LONG:
				spdata->ival = g3d_stream_read_int32_le(global->stream);
				(*r) -= 4;
				break;
			case T_ULONG:
				spdata->uval = g3d_stream_read_int32_le(global->stream);
				(*r) -= 4;
				break;
			case T_FLOAT:
				spdata->fval = g3d_stream_read_float_le(global->stream);
				(*r) -= 4;
				break;
			case T_DOUBLE:
				spdata->fval = g3d_stream_read_double_le(global->stream);
				(*r) -= 8;
				break;
			case T_VOID:
				g_warning("SDNA: non-pointer void type");
				break;
			case T_STRUCT:
#if DEBUG > BLEND_DEBUG_STRUCT
				g_debug("|%s  %-10s %-10s = /* len = %d */",
					debug_pad(level * 2),
					sprop->tname, sprop->name, *r);
#endif
				substruct = blend_sdna_get_struct_by_id(sdna, sprop->tid);
				spdata->structval = blend_sdna_data_read(sdna, substruct,
					global, r, level + 1);
				break;
		} /* switch(spdata->type) */
#if DEBUG > 0
		switch(spdata->type) {
			case T_CHAR:
			case T_SHORT:
			case T_INT:
			case T_LONG:
				g_debug("|%s  %-10s %-10s = %i; /* len = %d */",
					debug_pad(level * 2),
					sprop->tname, sprop->name,
					spdata->ival, *r);
				break;
			case T_UCHAR:
			case T_USHORT:
			case T_ULONG:
				g_debug("|%s  %-10s %-10s = %u; /* len = %d */;",
					debug_pad(level * 2),
					sprop->tname, sprop->name,
					spdata->uval, *r);
				break;
			case T_FLOAT:
			case T_DOUBLE:
				g_debug("|%s  %-10s %-10s = %f; /* len = %d */",
					debug_pad(level * 2),
					sprop->tname, sprop->name,
					spdata->fval, *r);
				break;
			case T_STRUCT:
				break;
			default:
				g_debug("|%s  %-10s %-10s; /* len = %d */",
					debug_pad(level * 2),
					sprop->tname, sprop->name, *r);
				break;
		}
#endif
	} /* properties */

#if DEBUG > BLEND_DEBUG_STRUCT
	g_debug("|%s};", debug_pad(level * 2));
#endif

	return sdata;
}

void blend_sdna_data_free(BlendSdnaData *sdata)
{
}

gboolean blend_sdna_dump_struct(BlendSdna *sdna, guint32 sdnanr)
{
	const BlendSdnaStruct *sstruct;
	BlendSdnaProperty *sprop;
	GSList *pitem;

	sstruct = blend_sdna_get_struct_by_id(sdna, sdnanr);
	if(!sstruct)
		return FALSE;
	sstruct = g_slist_nth_data(sdna->structs, sdnanr);
	g_debug("| struct %s { /* %d */", sstruct->name, sstruct->size);
	for(pitem = sstruct->properties; pitem != NULL; pitem = pitem->next) {
		sprop = pitem->data;
		g_debug("| \t%-16s %-24s;", sprop->tname, sprop->name);
	}
	g_debug("| };");

	return TRUE;
}
