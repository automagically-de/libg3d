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

#include <string.h>

#include <g3d/types.h>
#include <g3d/context.h>
#include <g3d/read.h>
#include <g3d/stream.h>


static gchar *skp_read_char(G3DStream *stream);
static gchar *skp_read_wchar(G3DStream *stream);
static gboolean skp_parse_version_map(G3DStream *stream, guint32 *max_nlen,
	guint32 *max_version);
static gchar *skp_find_section(G3DStream *stream, guint32 max_nlen,
	guint32 max_version, guint32 *version);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar *smagic, *sversion, *stmp, *ssection;
	guint32 max_nlen = 0, max_version = 0, version = 0;

	smagic = skp_read_wchar(stream);
	if(smagic == NULL) {
		g_warning("not a valid .skp file: '%s'", stream->uri);
		return FALSE;
	}
	sversion = skp_read_wchar(stream);
	if(sversion == NULL) {
		g_warning("failed to read version from '%s'", stream->uri);
		g_free(smagic);
		return FALSE;
	}

	g_debug("SKP: magic: '%s', version: '%s'", smagic, sversion);
	g_free(smagic);
	g_free(sversion);

	g3d_stream_seek(stream, 16, G_SEEK_CUR);
	stmp = skp_read_wchar(stream);
	if(stmp != NULL)
		g_free(stmp);
	g3d_stream_read_int32_le(stream);

	ssection = skp_read_char(stream);
	if(ssection) {
		g_debug("SKP: section '%s'", ssection);
		if(strcmp(ssection, "CVersionMap") == 0)
			skp_parse_version_map(stream, &max_nlen, &max_version);
		g_free(ssection);
	}

	ssection = skp_find_section(stream, max_nlen, max_version, &version);
	while(ssection != NULL) {
		g_debug("\t%s v%u @ 0x%08x", ssection, version,
			g3d_stream_tell(stream));
		ssection = skp_find_section(stream, max_nlen, max_version, &version);
	}

	return FALSE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for SketchUp .skp files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("skp", ":", 0);
}

/*****************************************************************************/

static gchar *skp_read_char(G3DStream *stream)
{
	guint32 magic, n;
	gchar *text;

	magic = g3d_stream_read_int32_be(stream);
	if(magic != 0xffff0000) {
		g_warning("SKP: wrong text magic: 0x%08x", magic);
		return NULL;
	}
	n = g3d_stream_read_int16_le(stream);

	text = g_new0(gchar, n + 1);
	g3d_stream_read(stream, text, 1, n);

	return text;
}

static gchar *skp_read_wchar(G3DStream *stream)
{
	gint32 i;
	guint32 magic, n;
	gunichar2 *u16text;
	gchar *text;
	GError *error = NULL;

	magic = g3d_stream_read_int32_be(stream);
	if((magic & 0xFFFFFF00) != 0xfffeff00) {
		g_warning("SKP: wrong UTF-16 magic: 0x%08x", magic);
		return NULL;
	}
	n = magic & 0x000000FF;

	u16text = g_new0(gunichar2, n + 1);
	for(i = 0; i < n; i ++) {
		u16text[i] = g3d_stream_read_int16_le(stream);
	}

	text = g_utf16_to_utf8(u16text, n, NULL, NULL, &error);
	if(error != NULL) {
		g_warning("UTF-16 to UTF-8 conversion failed: %s",
			error->message);
		g_error_free(error);
	}
	g_free(u16text);

	return text;
}

static gboolean skp_parse_version_map(G3DStream *stream, guint32 *max_nlen,
	guint32 *max_version)
{
	gchar *part;
	guint32 version;

	while(TRUE) {
		part = skp_read_wchar(stream);
		if(part == NULL)
			return FALSE;
		version = g3d_stream_read_int32_le(stream);

		g_debug("\t%-30s %u", part, version);
		if(strcmp(part, "End-Of-Version-Map") == 0) {
			g_free(part);
			return TRUE;
		}

		if(version > *max_version)
			*max_version = version;
		if(strlen(part) > *max_nlen)
			*max_nlen = strlen(part);

		g_free(part);
	}
	return FALSE;
}

static gchar *skp_find_section(G3DStream *stream, guint32 max_nlen,
	guint32 max_version, guint32 *version)
{
	goffset offset;
	guint32 ver, nlen;
	gchar *name;

	while(!g3d_stream_eof(stream) && (g3d_stream_read_int8(stream) != 0xFF));

	if(g3d_stream_eof(stream))
		return NULL;

	offset = g3d_stream_tell(stream);
	if(g3d_stream_read_int8(stream) != 0xFF) {
		g3d_stream_seek(stream, offset, G_SEEK_SET);
		return skp_find_section(stream, max_nlen, max_version, version);
	}

	ver = g3d_stream_read_int16_le(stream);
	if(ver > max_version) {
		g3d_stream_seek(stream, offset, G_SEEK_SET);
		return skp_find_section(stream, max_nlen, max_version, version);
	}

	nlen = g3d_stream_read_int16_le(stream);
	if(nlen > max_nlen) {
		g3d_stream_seek(stream, offset, G_SEEK_SET);
		return skp_find_section(stream, max_nlen, max_version, version);
	}

	name = g_new0(gchar, nlen + 1);
	g3d_stream_read(stream, name, 1, nlen);
	if(name[0] != 'C') {
		g_free(name);
		g3d_stream_seek(stream, offset, G_SEEK_SET);
		return skp_find_section(stream, max_nlen, max_version, version);
	}
	*version = ver;
	return name;
}
