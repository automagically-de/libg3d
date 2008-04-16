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

#include <stdio.h>

#include <g3d/stream.h>

static gsize g3d_stream_file_read(gpointer ptr, gsize size, gsize nmemb,
	 gpointer data)
{
	return fread((void *)ptr, size, nmemb, (FILE *)data);
}

static gchar *g3d_stream_file_read_line(gchar *buf, gsize size, gpointer data)
{
	return fgets(buf, size, (FILE *)data);
}

static gint g3d_stream_file_seek(gpointer data, guint32 offset,
	GSeekType whence)
{
	return fseek((FILE *)data, offset, whence);
}

static guint32 g3d_stream_file_tell(gpointer data)
{
	return ftell((FILE *)data);
}

static gboolean g3d_stream_file_eof(gpointer data)
{
	return (feof((FILE *)data));
}

static gint g3d_stream_file_close(gpointer data)
{
	return fclose((FILE *)data);
}

G3DStream *g3d_stream_open_file(const gchar *filename, const gchar *mode)
{
	FILE *f;
	guint32 flags = 0;

	f = fopen(filename, mode);
	if(f == NULL)
		return NULL;

	if(mode[0] == 'r')
		flags |= (1 << G3D_STREAM_READABLE);
	if(mode[0] == 'w')
		flags |= (1 << G3D_STREAM_WRITABLE);

	flags |= (1 << G3D_STREAM_SEEKABLE);

	return g3d_stream_new_custom(flags, filename,
		g3d_stream_file_read, g3d_stream_file_read_line,
		g3d_stream_file_seek, g3d_stream_file_tell, g3d_stream_file_eof,
		g3d_stream_file_close, f);
}

