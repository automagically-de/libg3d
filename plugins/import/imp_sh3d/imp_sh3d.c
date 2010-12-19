/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2009  Markus Dahms <mad@automagically.de>

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
#include <g3d/types.h>
#include <g3d/stream.h>

gboolean sh3d_javaserdata_parse(G3DStream *stream);
gboolean sh3d_javaserdata_read_level(G3DStream *stream, guint32 level);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	G3DStream *zipstream;
	gboolean retval;

	zipstream = g3d_stream_open_zip_from_stream(stream, "Home");
	if (!zipstream) {
		g_error("failed to open 'Home' in sh3d");
		return FALSE;
	}

	retval = sh3d_javaserdata_parse(zipstream);
	g3d_stream_close(zipstream);
	return retval;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for sh3d files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("sh3d", ":", 0);
}

/*****************************************************************************/

enum {
	TC_FIELD_BYTE = 'B',
	TC_FIELD_CHAR = 'C',
	TC_FIELD_DOUBLE = 'D',
	TC_FIELD_FLOAT = 'F',
	TC_FIELD_INTEGER = 'I',
	TC_FIELD_LONG = 'J',
	TC_FIELD_SHORT = 'S',
	TC_FIELD_BOOLEAN = 'Z',

	TC_FIELD_ARRAY = '[',
	TC_FIELD_OBJECT = 'L',

	TC_NULL = 0x70,
	TC_REFERENCE = 0x71,
	TC_CLASSDESC = 0x72,
	TC_OBJECT = 0x73,
	TC_STRING = 0x74,
	TC_ARRAY = 0x75,
	TC_CLASS = 0x76,
	TC_BLOCKDATA = 0x77,
	TC_ENDBLOCKDATA = 0x78,
	TC_RESET = 0x79,
	TC_BLOCKDATALONG = 0x7A,
	TC_EXCEPTION = 0x7B,
	TC_LONGSTRING = 0x7C,
	TC_PROXYCLASSDESC = 0x7D,
	TC_ENUM = 0x7E
};

gchar *sh3d_javaserdata_read_string(G3DStream *stream)
{
	guint16 len;
	gchar *str;

	len = g3d_stream_read_int16_be(stream);
	g_assert(len != 0xFFFF);
	str = g_new0(gchar, len + 1);
	g3d_stream_read(stream, str, len);
	return str;
}

gboolean sh3d_javaserdata_read_field(G3DStream *stream, guint32 level)
{
	guint8 tc;
	gchar *fname;

	tc = g3d_stream_read_int8(stream);
	fname = sh3d_javaserdata_read_string(stream);
	g_debug("%sfield: %s (%c)", debug_pad(level), fname, tc);
	switch (tc) {
		case TC_FIELD_ARRAY:
		case TC_FIELD_OBJECT:
			sh3d_javaserdata_read_level(stream, level + 1);
			break;
		default:
			break;
	}
	g_free(fname);
	return TRUE;
}

gboolean sh3d_javaserdata_read_level(G3DStream *stream, guint32 level)
{
	gint32 i;
	guint32 ref;
	guint16 nfields;
	guint8 tc;
	gchar *str;

	while (!g3d_stream_eof(stream)) {
		tc = g3d_stream_read_int8(stream);
		g_debug("%stc: 0x%02x", debug_pad(level), tc);
		switch (tc) {
			case TC_CLASSDESC:
				/* className */
				str = sh3d_javaserdata_read_string(stream);
				g_debug("%sclass name: %s", debug_pad(level), str);
				g_free(str);
				/* serialVersionUID */
				g3d_stream_read_int32_be(stream);
				/* newHandle */
				g3d_stream_read_int32_be(stream);
				/* classDescInfo */
				/* - classDescFlags */
				g3d_stream_read_int8(stream);
				/* - fields */
				nfields = g3d_stream_read_int16_be(stream);
				for (i = 0; i < nfields; i ++) {
					sh3d_javaserdata_read_field(stream, level + 1);
				}
				break;
			case TC_ENDBLOCKDATA:
				return TRUE;
			case TC_OBJECT:
				/* classDesc */
				sh3d_javaserdata_read_level(stream, level + 1);
				/* newHandle */
				/* classData[] */
				return FALSE;
				break;
			case TC_REFERENCE:
				ref = g3d_stream_read_int32_be(stream);
				g_debug("%sreference: 0x%08x", debug_pad(level), ref);
				return TRUE;
			case TC_STRING:
				/* newHandle */
				/* (utf) */
				str = sh3d_javaserdata_read_string(stream);
				g_debug("%sstring: %s", debug_pad(level), str);
				g_free(str);
				return TRUE;
			default:
				/* unhandled tc */
				return FALSE;
		}
	}
	
	return TRUE;
}

gboolean sh3d_javaserdata_parse(G3DStream *stream)
{
	guint16 magic, version;

	magic = g3d_stream_read_int16_be(stream);
	if (magic != 0xACED) {
		g_error("java serialization magic not 0xACED (0x%04X)", magic);
		return FALSE;
	}
	version = g3d_stream_read_int16_be(stream);
	if (version != 5) {
		g_error("only java serialization version 5 supported");
		return FALSE;
	}

	return sh3d_javaserdata_read_level(stream, 0);
}


