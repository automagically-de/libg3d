#include <g3d/types.h>

#include <lauxlib.h>
#include <lualib.h>

static int _g3d_hello(lua_State *ls)
{
	return (0 != luaL_dostring(ls, "print 'Hello, World'"));
}

static int _g3d_debug(lua_State *ls)
{
	if(lua_isstring(ls, -1)) {
		g_debug("lua: %s", lua_tolstring(ls, -1, NULL));
	}
	return 1;
}

static const luaL_Reg g3d_functions[] = {
	{ "hello", _g3d_hello },
	{ "debug", _g3d_debug },
	{ NULL, NULL }
};

gboolean lua_funcs_register(lua_State *ls)
{
	luaL_register(ls, "g3d", g3d_functions);
	return TRUE;
}


