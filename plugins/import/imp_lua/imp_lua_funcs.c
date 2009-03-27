#include <g3d/types.h>

#include <lauxlib.h>
#include <lualib.h>

static int _g3d_hello(lua_State *ls)
{
	return (0 != luaL_dostring(ls, "print 'Hello, World'"));
}

static const luaL_Reg g3d_functions[] = {
	{ "hello", _g3d_hello },
	{ NULL, NULL }
};

gboolean lua_funcs_register(lua_State *ls)
{
	luaL_register(ls, "g3d", g3d_functions);
	return TRUE;
}


