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

#include <g3d/stream.h>

gint32 g3d_stream_read_int8(G3DStream *stream)
{
	guint8 c;
	if(g3d_stream_read(stream, &c, 1, 1) != 1)
		return 0;
	return c;
}

gint32 g3d_stream_read_int16_be(G3DStream *stream)
{
	return (g3d_stream_read_int8(stream) << 8) | g3d_stream_read_int8(stream);
}

gint32 g3d_stream_read_int16_le(G3DStream *stream)
{
	return g3d_stream_read_int8(stream) | (g3d_stream_read_int8(stream) << 8);
}

gint32 g3d_stream_read_int32_be(G3DStream *stream)
{
	return (g3d_stream_read_int8(stream) << 24) |
		(g3d_stream_read_int8(stream) << 16) |
		(g3d_stream_read_int8(stream) << 8) | g3d_stream_read_int8(stream);
}

gint32 g3d_stream_read_int32_le(G3DStream *stream)
{
	return g3d_stream_read_int8(stream) | (g3d_stream_read_int8(stream) << 8) |
		(g3d_stream_read_int8(stream) << 16) |
		(g3d_stream_read_int8(stream) << 24);
}

/* FIXME: host byte order matters for union!? */

gfloat g3d_stream_read_float_be(G3DStream *stream)
{
	union {
		gfloat f;
		guint8 u[4];
	} u;
	gint32 i;

	for(i = 3; i >= 0; i --)
		u.u[i] = g3d_stream_read_int8(stream);
	return u.f;
}

gfloat g3d_stream_read_float_le(G3DStream *stream)
{
	union {
		gfloat f;
		guint8 u[4];
	} u;
	gint32 i;

	for(i = 0; i < 4; i ++)
		u.u[i] = g3d_stream_read_int8(stream);
	return u.f;
}
gdouble g3d_stream_read_double_be(G3DStream *stream)
{
	union {
		gdouble f;
		guint8 u[8];
	} u;
	gint32 i;

	for(i = 7; i >= 0; i --)
		u.u[i] = g3d_stream_read_int8(stream);
	return u.f;
}

gdouble g3d_stream_read_double_le(G3DStream *stream)
{
	union {
		gdouble f;
		guint8 u[8];
	} u;
	gint32 i;

	for(i = 0; i < 8; i ++)
		u.u[i] = g3d_stream_read_int8(stream);
	return u.f;
}

gint32 g3d_stream_read_cstr(G3DStream *stream, gchar *buffer, gint32 max_len)
{
	gint32 n = 0;
	gchar c;

	do {
		c = g3d_stream_read_int8(stream);
		buffer[n] = c;
		n ++;
	} while((c != 0) && (n < max_len));
	buffer[max_len - 1] = '\0';
	return n;
}

