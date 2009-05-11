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

#include <string.h>

#include <g3d/types.h>
#include <g3d/stream.h>

#include <lauxlib.h>
#include <lualib.h>

#include "imp_lua_funcs.h"

typedef struct {
	gchar *buffer;
	G3DStream *stream;
} _G3DLuaReaderData;

static const char *stream_reader(lua_State *ls, void *data, size_t *s);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	lua_State *ls;
	_G3DLuaReaderData *rdata;
	gint r;

	ls = lua_open();
	luaopen_base(ls);
	luaopen_math(ls);
	luaopen_string(ls);
	luaopen_table(ls);
	lua_funcs_register(ls, context, model);
	rdata = g_new0(_G3DLuaReaderData, 1);
	rdata->stream = stream;
	r = lua_load(ls, stream_reader, rdata, stream->uri);
	if(rdata->buffer)
		g_free(rdata->buffer);
	if(r == 0)
		r = lua_pcall(ls, 0, LUA_MULTRET, 0);
	if(r != 0) {
		if(r == LUA_ERRRUN) {
			if(lua_isstring(ls, -1)) {
				g_warning("lua error: %s", lua_tostring(ls, -1));
			}
		}
	}

	lua_funcs_cleanup(ls);

	g_debug("LUA: close");
	lua_close(ls);
	return (r == 0) ? TRUE : FALSE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Lua scripts files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("lua", ":", 0);
}

/*****************************************************************************/

static const char *stream_reader(lua_State *ls, void *data, size_t *s)
{
	_G3DLuaReaderData *rdata = data;

	g_return_val_if_fail(rdata != NULL, NULL);

	if(rdata->buffer)
		g_free(rdata->buffer);

	rdata->buffer = g_new0(gchar, 2048);
	if(!g3d_stream_read_line(rdata->stream, rdata->buffer, 2048)) {
		g_free(rdata->buffer);
		rdata->buffer = NULL;
		return NULL;
	}

	*s = strlen(rdata->buffer);
	return rdata->buffer;
}

