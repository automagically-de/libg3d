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

static gpointer get_lua_object(lua_State *ls, gint tid, const gchar *name)
{
	gpointer object;

	lua_pushstring(ls, name);
	lua_gettable(ls, tid - 1);
	if(lua_isnil(ls, -1)) {
		lua_pop(ls, 1);
		return NULL;
	}
	if(!lua_isuserdata(ls, -1)) {
		lua_pop(ls, 1);
		lua_pushstring(ls, "not a G3DObject");
		g_debug("not a G3DObject");
		lua_error(ls);
	}
	object = lua_touserdata(ls, -1);
	lua_pop(ls, 1);
	return object;
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
	object = get_lua_object(ls, -2, "__g3dobject");
	if(object->name)
		g_free(object->name);
	object->name = g_strdup(lua_tostring(ls, -1));

	g_debug("Object:setName('%s')", object->name);
	return 0;
}

static int _g3d_Object_addVertex(lua_State *ls)
{
	G3DObject *object;
	gint i;
	G3DVector f;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER, LUA_TTABLE, -1);
	object = get_lua_object(ls, -4, "__g3dobject");
	g_return_val_if_fail(object != NULL, 0);

	object->vertex_data = g_realloc(object->vertex_data,
		sizeof(G3DVector) * 3 * (object->vertex_count + 1));

	for(i = 0; i < 3; i ++) {
		f = lua_tonumber(ls, -3 + i);
		object->vertex_data[object->vertex_count * 3 + i] = f;
	}
	object->vertex_count ++;

	lua_pushnumber(ls, object->vertex_count);
	return 1;
}

static int _g3d_Object(lua_State *ls)
{
	G3DObject *object;
	
	lua_newtable(ls);

	object = g_new0(G3DObject, 1);

	lua_pushlightuserdata(ls, object);
	lua_setfield(ls, -2, "__g3dobject");

	lua_pushcfunction(ls, _g3d_Object_setName);
	lua_setfield(ls, -2, "setName");

	lua_pushcfunction(ls, _g3d_Object_addVertex);
	lua_setfield(ls, -2, "addVertex");

	return 1;
}

static int _g3d_Face_setMaterial(lua_State *ls)
{
	G3DFace *face;
	G3DMaterial *material;

	check_lua_sig(ls, LUA_TTABLE, LUA_TTABLE, -1);
	face = get_lua_object(ls, -2, "__g3dface");
	g_return_val_if_fail(face != NULL, 0);
	material = get_lua_object(ls, -1, "__g3dmaterial");
	g_return_val_if_fail(material != NULL, 0);
	face->material = material;

	return 0;
}

static int _g3d_Face(lua_State *ls)
{
	G3DFace *face;
	
	lua_newtable(ls);

	face = g_new0(G3DFace, 1);

	lua_pushlightuserdata(ls, face);
	lua_setfield(ls, -2, "__g3dface");

	lua_pushcfunction(ls, _g3d_Face_setMaterial);
	lua_setfield(ls, -2, "setMaterial");

	return 1;
}

static int _g3d_Material(lua_State *ls)
{
	G3DMaterial *material;

	lua_newtable(ls);

	material = g_new0(G3DMaterial, 1);
	material->r = material->g = material->b = 0.7;
	material->a = 1.0;

	lua_pushlightuserdata(ls, material);
	lua_setfield(ls, -2, "__g3dmaterial");

	return 1;
}

static int _g3d_model_addObject(lua_State *ls)
{
	check_lua_sig(ls, LUA_TTABLE, LUA_TTABLE, -1);

	return 0;
}

static void _g3d_model__register(lua_State *ls)
{
	lua_getglobal(ls, "g3d");
	lua_newtable(ls);

	lua_pushcfunction(ls, _g3d_model_addObject);
	lua_setfield(ls, -2, "addObject");

	lua_setfield(ls, -2, "model");
}

static const luaL_Reg g3d_functions[] = {
	{ "debug",       _g3d_debug },
	{ "Material",    _g3d_Material },
	{ "Face",        _g3d_Face },
	{ "Object",      _g3d_Object },
	{ NULL, NULL }
};

gboolean lua_funcs_register(lua_State *ls)
{
	luaL_register(ls, "g3d", g3d_functions);
	_g3d_model__register(ls);

	return TRUE;
}

