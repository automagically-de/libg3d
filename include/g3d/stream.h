/* $Id:$ */

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

#ifndef _G3D_STREAM_H
#define _G3D_STREAM_H

/**
 * SECTION:stream
 * @short_description: I/O abstraction layer for plugins
 * @see_also: #G3DStream
 * @include: g3d/stream.h
 */

#include <glib.h>
#include <g3d/config.h>
#include <g3d/types.h>

G_BEGIN_DECLS

enum {
	/* shift offsets */
	G3D_STREAM_SEEKABLE = 0x00,
	G3D_STREAM_READABLE = 0x01,
	G3D_STREAM_WRITABLE = 0x02
};

typedef gsize (* G3DStreamReadFunc)(gpointer ptr, gsize size, gpointer data);
typedef gchar *(* G3DStreamReadLineFunc)(gchar *buf, gsize size,
	gpointer data);
typedef gint (*G3DStreamSeekFunc)(gpointer data, goffset offset,
	GSeekType whence);
typedef goffset (*G3DStreamTellFunc)(gpointer data);
typedef goffset (*G3DStreamSizeFunc)(gpointer data);
typedef gboolean (*G3DStreamEofFunc)(gpointer data);
typedef gint (*G3DStreamCloseFunc)(gpointer data);

/**
 * G3DStream:
 *
 * An abstraction of input handling.
 */
struct _G3DStream {
	/*< private >*/
	guint32 flags;
	gchar *uri;
	G3DStreamReadFunc read;
	G3DStreamReadLineFunc readline;
	G3DStreamSeekFunc seek;
	G3DStreamTellFunc tell;
	G3DStreamSizeFunc size;
	G3DStreamEofFunc eof;
	G3DStreamCloseFunc close;
	gpointer data;
};

/* public interface */

/**
 * g3d_stream_is_seekable:
 * @stream: the stream
 *
 * Get information whether it is possible to seek in a stream.
 *
 * Returns: TRUE if seekable, FALSE else
 */
gboolean g3d_stream_is_seekable(G3DStream *stream);

/**
 * g3d_stream_get_uri:
 * @stream: the stream
 *
 * Get the URI of a stream
 *
 * Returns: a non-NULL, zero-terminated string containing the URI of the
 * string. This return value should not be freed.
 */
gchar *g3d_stream_get_uri(G3DStream *stream);

/**
 * g3d_stream_read_int8:
 * @stream: the stream to read from
 *
 * Read a 1 byte signed integer from file.
 *
 * Returns: The read value, 0 in case of error
 */
gint32 g3d_stream_read_int8(G3DStream *stream);

/**
 * g3d_stream_read_int16_be:
 * @stream: the stream to read from
 *
 * Read a 2 byte big-endian signed integer from file.
 *
 * Returns: The read value, 0 in case of error
 */
gint32 g3d_stream_read_int16_be(G3DStream *stream);

/**
 * g3d_stream_read_int16_le:
 * @stream: the stream to read from
 *
 * Read a 2 byte little-endian signed integer from file.
 *
 * Returns: The read value, 0 in case of error
 */
gint32 g3d_stream_read_int16_le(G3DStream *stream);

/**
 * g3d_stream_read_int32_be:
 * @stream: the stream to read from
 *
 * Read a 4 byte big-endian signed integer from file.
 *
 * Returns: The read value, 0 in case of error
 */
gint32 g3d_stream_read_int32_be(G3DStream *stream);

/**
 * g3d_stream_read_int32_le:
 * @stream: the stream to read from
 *
 * Read a 4 byte little-endian signed integer from file.
 *
 * Returns: The read value, 0 in case of error
 */
gint32 g3d_stream_read_int32_le(G3DStream *stream);

/**
 * g3d_stream_read_float_be:
 * @stream: the stream to read from
 *
 * Read a 4 byte big-endian floating point number from file.
 *
 * Returns: The read value, 0 in case of error
 */
gfloat g3d_stream_read_float_be(G3DStream *stream);

/**
 * g3d_stream_read_float_le:
 * @stream: the stream to read from
 *
 * Read a 4 byte little-endian floating point number from file.
 *
 * Returns: The read value, 0 in case of error
 */
gfloat g3d_stream_read_float_le(G3DStream *stream);

/**
 * g3d_stream_read_double_be:
 * @stream: the stream to read from
 *
 * Read a 8 byte big-endian double-precision floating point number from file.
 *
 * Returns: The read value, 0 in case of error
 */
gdouble g3d_stream_read_double_be(G3DStream *stream);

/**
 * g3d_stream_read_double_be:
 * @stream: the stream to read from
 *
 * Read a 8 byte little-endian double-precision floating point number from
 * file.
 *
 * Returns: The read value, 0 in case of error
 */
gdouble g3d_stream_read_double_le(G3DStream *stream);

/**
 * g3d_stream_read_cstr:
 * @stream: the stream to read from
 * @buffer: the buffer to fill
 * @max_len: maximum number to read from stream
 *
 * Read a string (terminated by '\0') from stream
 *
 * Returns: number of bytes read from stream
 */
gint32 g3d_stream_read_cstr(G3DStream *stream, gchar *buffer, gint32 max_len);

/**
 * g3d_stream_read:
 * @stream: the stream to read from
 * @ptr: pointer to memory storage
 * @size: number of bytes to read
 *
 * Reads a number of bytes from the stream.
 *
 * Returns: number of bytes successfully read.
 */
gsize g3d_stream_read(G3DStream *stream, gpointer ptr, gsize size);

/**
 * g3d_stream_read_line:
 * @stream: stream to read a line from
 * @buf: an allocated buffer to be filled
 * @size: maximum length of line including terminating zero
 *
 * Read a line (terminated by a newline character or end of file) from a
 * stream.
 *
 * Returns: the read line or NULL in case of an error.
 */
gchar *g3d_stream_read_line(G3DStream *stream, gchar *buf, gsize size);

/**
 * g3d_stream_skip:
 * @stream: stream to skip bytes from
 * @offset: number of bytes to skip
 *
 * Skip a number of bytes (>= 0) in stream even if it does not support
 * seeking.
 *
 * Returns: 0 on success, -1 else
 */
gint g3d_stream_skip(G3DStream *stream, goffset offset);

/**
 * g3d_stream_seek:
 * @stream: stream to seek in
 * @offset: number of bytes to seek
 * @whence: seek type
 *
 * Moves around the current position in the stream.
 *
 * Returns: 0 on success, -1 else
 */
gint g3d_stream_seek(G3DStream *stream, goffset offset, GSeekType whence);

/**
 * g3d_stream_tell:
 * @stream: stream to get position from
 *
 * Tells the current position in the stream.
 *
 * Returns: current stream position
 */
goffset g3d_stream_tell(G3DStream *stream);

/**
 * g3d_stream_size:
 * @stream: stream to get size from
 *
 * Get the size in bytes of a stream.
 *
 * Returns: size of stream in bytes
 */
goffset g3d_stream_size(G3DStream *stream);

/**
 * g3d_stream_eof:
 * @stream: the stream
 *
 * Checks whether the stream has reached its end.
 *
 * Returns: TRUE if no more data can be read, FALSE else.
 */
gboolean g3d_stream_eof(G3DStream *stream);

/**
 * g3d_stream_close:
 * @stream: the stream
 *
 * Closes an open stream.
 *
 * Returns: 0 on success.
 */
gint g3d_stream_close(G3DStream *stream);


G3DStream *g3d_stream_new_custom(guint32 flags, const gchar *uri,
	G3DStreamReadFunc readfunc, G3DStreamReadLineFunc readlinefunc,
	G3DStreamSeekFunc seekfunc, G3DStreamTellFunc tellfunc,
	G3DStreamSizeFunc sizefunc,
	G3DStreamEofFunc eoffunc, G3DStreamCloseFunc closefunc, gpointer data);
G3DStream *g3d_stream_open_file(const gchar *filename, const gchar *mode);
G3DStream *g3d_stream_from_buffer(guint8 *buffer, gsize size,
	const gchar *title, gboolean free_buffer);
#ifdef HAVE_LIBGSF
G3DStream *g3d_stream_open_structured_file(const gchar *filename,
	const gchar *subfile);
G3DStream *g3d_stream_open_zip(const gchar *filename, const gchar *subfile);
G3DStream *g3d_stream_open_zip_from_stream(G3DStream *stream,
	const gchar *subfile);
#endif

G_END_DECLS

#endif /* _G3D_STREAM_H */
