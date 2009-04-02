#include <string.h>

#include <g3d/types.h>

#include <lauxlib.h>
#include <lualib.h>

static void check_lua_sig(lua_State *ls, ...)
{
	va_list va;
	gchar *err;
	gint type, idx = -1;

	va_start(va, ls);
	type = va_arg(va, int);
	while(type >= 0) {
		if(lua_type(ls, idx) != type) {
			err = g_strdup_printf("parameter %d: wrong type (%s, expected %s)",
				-idx,
				lua_typename(ls, lua_type(ls, idx)),
				lua_typename(ls, type));
			lua_pushstring(ls, err);
			g_free(err);
			va_end(va);
			lua_error(ls);
		}
		idx --;
		type = va_arg(va, int);
	}
	va_end(va);
}

static int _g3d_debug(lua_State *ls)
{
	check_lua_sig(ls, LUA_TSTRING, -1);
	g_debug("%s", lua_tostring(ls, -1));
	return 0;
}

static int _g3d_Object_setName(lua_State *ls)
{
	G3DObject *object;

	check_lua_sig(ls, LUA_TSTRING, LUA_TTABLE, -1);

	lua_pushstring(ls, "__g3dobject");
	lua_gettable(ls, -3);
	if(!lua_isuserdata(ls, -1)) {
		lua_pop(ls, 1);
		lua_pushstring(ls, "not a G3DObject");
		g_debug("not a G3DObject");
		lua_error(ls);
	}
	object = lua_touserdata(ls, -1);
	lua_pop(ls, 1);

	if(object->name)
		g_free(object->name);
	object->name = g_strdup(lua_tostring(ls, -1));

	g_debug("Object:setName('%s')", object->name);
	return 0;
}

static int _g3d_Material(lua_State *ls)
{
	G3DMaterial *material;

	lua_newtable(ls);

	lua_pushstring(ls, "__g3dmaterial");
	material = lua_newuserdata(ls, sizeof(G3DMaterial));
	memset(material, 0, sizeof(G3DMaterial));
	lua_settable(ls, -3);

	material->r = material->g = material->b = 0.7;
	material->a = 1.0;

	return 1;
}

static int _g3d_Object(lua_State *ls)
{
	G3DObject *object;
	
	lua_newtable(ls);

	lua_pushstring(ls, "__g3dobject");
	object = lua_newuserdata(ls, sizeof(G3DObject));
	memset(object, 0, sizeof(G3DObject));
	lua_settable(ls, -3);

	lua_pushstring(ls, "setName");
	lua_pushcfunction(ls, _g3d_Object_setName);
	lua_settable(ls, -3);

	return 1;
}

static const luaL_Reg g3d_functions[] = {
	{ "debug",       _g3d_debug },
	{ "Material",    _g3d_Material },
	{ "Object",      _g3d_Object },
	{ NULL, NULL }
};

gboolean lua_funcs_register(lua_State *ls)
{
	luaL_register(ls, "g3d", g3d_functions);
	return TRUE;
}

