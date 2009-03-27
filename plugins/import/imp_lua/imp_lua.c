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

#include <g3d/types.h>
#include <g3d/stream.h>

#include <lauxlib.h>
#include <lualib.h>

#include "imp_lua_funcs.h"

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	lua_State *ls;
	gchar buffer[2048], *line;

	ls = lua_open();
	luaopen_base(ls);
	lua_funcs_register(ls);

	while(TRUE) {
		line = g3d_stream_read_line(stream, buffer, 2048);
		if(line == NULL)
			break;
		if(0 != luaL_dostring(ls, line)) {
			g_warning("imp_lua: %s:%d: error in %s",
				stream->uri, g3d_stream_line(stream), line);
			break;
		}
	}

	lua_close(ls);
	return TRUE;
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
