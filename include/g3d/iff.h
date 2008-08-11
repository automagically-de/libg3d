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

#ifndef __G3D_IFF_H__
#define __G3D_IFF_H__

#ifndef G3D_DISABLE_DEPRECATED
#include <stdio.h>
#endif
#include <glib.h>

#include <g3d/types.h>

#define G3D_IFF_PAD1   0x01
#define G3D_IFF_PAD2   0x02
#define G3D_IFF_PAD4   0x04
#define G3D_IFF_PAD8   0x08

#define G3D_IFF_SUBCHUNK_LEN16   0x10
#define G3D_IFF_LEN16            0x20
#define G3D_IFF_LE               0x40 /* little endian */

#define G3D_IFF_MKID(a,b,c,d) ( \
	(((guint32)(a))<<24) | \
	(((guint32)(b))<<16) | \
	(((guint32)(c))<< 8) | \
	(((guint32)(d))    ) )

G_BEGIN_DECLS

#ifndef G3D_DISABLE_DEPRECATED
#define g3d_iff_gdata G3DIffGlobal
#define g3d_iff_ldata G3DIffLocal
#define g3d_iff_chunk_callback G3DIffChunkCallback
#define g3d_iff_chunk_info G3DIffChunkInfo
#endif

/* global data */
typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	guint32 flags;
	gpointer user_data;
#ifndef G3D_DISABLE_DEPRECATED
	FILE *f;
	long int max_fpos;
#endif
} G3DIffGlobal;

/* local data */
typedef struct {
	guint32 id;
	guint32 parent_id;
	gpointer object;
	gint32 level;
	gpointer level_object;
	gint32 nb;
	gboolean finalize;
} G3DIffLocal;

typedef gboolean (* G3DIffChunkCallback)(
	G3DIffGlobal *global, G3DIffLocal *local);

typedef struct {
	gchar *id;
	gchar *description;
	gboolean container;
	G3DIffChunkCallback callback;
} G3DIffChunkInfo;

/**
 * g3d_iff_check:
 * @stream: stream containing IFF file to check
 * @id: top level ID (out)
 * @len: length of top level container (out)
 *
 * Checks a stream for a valid IFF signature and reads the top level container.
 *
 * Returns: TRUE on success (valid IFF), FALSE else
 */
gboolean g3d_iff_check(G3DStream *stream, guint32 *id, gsize *len);

/**
 * g3d_iff_read_chunk:
 * @stream: stream to read from
 * @id: ID of chunk (out)
 * @len: length of chunk (excluding header) (out)
 * @flags: flags
 *
 * Reads one chunk header from an IFF file.
 *
 * Returns: real length of chunk including header and possible padding byte
 */
gsize g3d_iff_read_chunk(G3DStream *stream, guint32 *id, gsize *len,
	guint32 flags);

#ifndef G3D_DISABLE_DEPRECATED
/**
 * g3d_iff_open:
 * @filename: file name of IFF file
 * @id: top level ID (out)
 * @len: length of top level container (out)
 *
 * Opens an IFF file, checks it and reads its top level container.
 *
 * Returns: the file pointer of open file or NULL in case of an error
 */
FILE *g3d_iff_open(const gchar *filename, guint32 *id, guint32 *len);

/**
 * g3d_iff_readchunk:
 * @f: the open IFF file pointer
 * @id: ID of chunk (out)
 * @len: length of chunk (excluding header) (out)
 * @flags: flags
 *
 * Reads one chunk header from an IFF file.
 *
 * Returns: real length of chunk including header and possible padding byte
 */
int g3d_iff_readchunk(FILE *f, guint32 *id, guint32 *len, guint32 flags);
#endif /* G3D_DISABLE_DEPRECATED */

gchar *g3d_iff_id_to_text(guint32 id);

gboolean g3d_iff_chunk_matches(guint32 id, gchar *tid);

gpointer g3d_iff_handle_chunk(G3DIffGlobal *global, G3DIffLocal *plocal,
	G3DIffChunkInfo *chunks, guint32 flags);

gboolean g3d_iff_read_ctnr(G3DIffGlobal *global, G3DIffLocal *local,
	G3DIffChunkInfo *chunks, guint32 flags);

G_END_DECLS

#endif
