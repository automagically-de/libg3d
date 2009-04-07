#include <string.h>

#include <g3d/types.h>
#include <g3d/texture.h>
#include <g3d/matrix.h>
#include <g3d/object.h>

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

static gpointer get_lua_object(lua_State *ls, gint tid, const gchar *name,
	gboolean may_fail)
{
	gpointer object;

	lua_pushstring(ls, name);
	lua_gettable(ls, tid - 1);
	if(may_fail && lua_isnil(ls, -1)) {
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

static G3DContext *get_g3d_context(lua_State *ls)
{
	G3DContext *context;

	lua_getglobal(ls, "g3d");
	lua_getfield(ls, -1, "context");
	lua_getfield(ls, -1, "__g3dcontext");
	if(!lua_isuserdata(ls, -1)) {
		lua_pop(ls, 3);
		return 0;
	}
	context = lua_touserdata(ls, -1);
	lua_pop(ls, 3);
	return context;
}

static G3DModel *get_g3d_model(lua_State *ls)
{
	G3DModel *model;

	lua_getglobal(ls, "g3d");
	lua_getfield(ls, -1, "model");
	lua_getfield(ls, -1, "__g3dmodel");
	if(!lua_isuserdata(ls, -1)) {
		lua_pop(ls, 3);
		return 0;
	}
	model = lua_touserdata(ls, -1);
	lua_pop(ls, 3);
	return model;
}

/****************************************************************************/
/* callback functions                                                       */

static int _g3d_debug(lua_State *ls)
{
	check_lua_sig(ls, LUA_TSTRING, -1);
	g_debug("%s", lua_tostring(ls, -1));
	return 0;
}

/* G3DObject */

static int _g3d_Object_setName(lua_State *ls)
{
	G3DObject *object;

	check_lua_sig(ls, LUA_TSTRING, LUA_TTABLE, -1);
	object = get_lua_object(ls, -2, "__g3dobject", FALSE);
	if(object->name)
		g_free(object->name);
	object->name = g_strdup(lua_tostring(ls, -1));
	return 0;
}

static int _g3d_Object_addVertex(lua_State *ls)
{
	G3DObject *object;
	gint i;
	G3DVector f;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER, LUA_TTABLE, -1);
	object = get_lua_object(ls, -4, "__g3dobject", FALSE);

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

static int _g3d_Object_addFace(lua_State *ls)
{
	G3DObject *object;
	G3DFace *face;

	check_lua_sig(ls, LUA_TTABLE, LUA_TTABLE, -1);

	object = get_lua_object(ls, -2, "__g3dobject", FALSE);
	face = get_lua_object(ls, -1, "__g3dface", FALSE);

	object->faces = g_slist_append(object->faces, face);

	return g_slist_length(object->faces);
}

static int _g3d_Object_transform(lua_State *ls)
{
	G3DObject *object;
	G3DMatrix *matrix;

	check_lua_sig(ls, LUA_TTABLE, LUA_TTABLE, -1);

	object = get_lua_object(ls, -2, "__g3dobject", FALSE);
	matrix = get_lua_object(ls, -1, "__g3dmatrix", FALSE);

	g3d_object_transform(object, matrix);

	return 0;
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

	lua_pushcfunction(ls, _g3d_Object_addFace);
	lua_setfield(ls, -2, "addFace");

	lua_pushcfunction(ls, _g3d_Object_transform);
	lua_setfield(ls, -2, "transform");

	return 1;
}

/* G3DFace */

static int _g3d_Face_setMaterial(lua_State *ls)
{
	G3DFace *face;
	G3DMaterial *material;

	check_lua_sig(ls, LUA_TTABLE, LUA_TTABLE, -1);
	face = get_lua_object(ls, -2, "__g3dface", FALSE);
	material = get_lua_object(ls, -1, "__g3dmaterial", FALSE);
	face->material = material;

	return 0;
}

static int _g3d_Face_addTexVertex(lua_State *ls)
{
	G3DFace *face;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TNUMBER, LUA_TTABLE, -1);

	face = get_lua_object(ls, -3, "__g3dface", FALSE);

	face->tex_vertex_data = g_realloc(face->tex_vertex_data,
		sizeof(G3DVector) * 2 * (face->tex_vertex_count + 1));
	face->tex_vertex_data[face->tex_vertex_count * 2 + 0] =
		lua_tonumber(ls, -2);
	face->tex_vertex_data[face->tex_vertex_count * 2 + 1] =
		lua_tonumber(ls, -1);
	if(face->material && face->material->tex_image) {
		face->flags |= G3D_FLAG_FAC_TEXMAP;
		face->tex_image = face->material->tex_image;
	}
	face->tex_vertex_count ++;
	return 0;
}

static int _face_add_index(G3DFace *face, guint32 i)
{
	face->vertex_indices = g_realloc(face->vertex_indices,
		(face->vertex_count + 1) * sizeof(guint32));
	face->vertex_indices[face->vertex_count] = i;
	face->vertex_count ++;
	return face->vertex_count;
}

static int _g3d_Face(lua_State *ls)
{
	G3DFace *face;
	gint i;
	
	lua_newtable(ls);

	face = g_new0(G3DFace, 1);
	for(i = -2; lua_isnumber(ls, i); i --) {
		_face_add_index(face, (guint32)lua_tonumber(ls, i));
	}

	lua_pushlightuserdata(ls, face);
	lua_setfield(ls, -2, "__g3dface");

	lua_pushcfunction(ls, _g3d_Face_setMaterial);
	lua_setfield(ls, -2, "setMaterial");

	lua_pushcfunction(ls, _g3d_Face_addTexVertex);
	lua_setfield(ls, -2, "addTexVertex");

	return 1;
}

/* G3DMaterial */

static int _g3d_Material_setColor(lua_State *ls)
{
	G3DMaterial *material;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER, LUA_TTABLE, -1);

	material = get_lua_object(ls, -4, "__g3dmaterial", FALSE);
	material->r = lua_tonumber(ls, -3);
	material->g = lua_tonumber(ls, -2);
	material->b = lua_tonumber(ls, -1);
	
	return 0;
}

static int _g3d_Material_setAlpha(lua_State *ls)
{
	G3DMaterial *material;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TTABLE, -1);

	material = get_lua_object(ls, -2, "__g3dmaterial", FALSE);
	material->a = lua_tonumber(ls, -1);
	
	return 0;
}

static int _g3d_Material_setTexture(lua_State *ls)
{
	G3DMaterial *material;
	G3DImage *image;

	check_lua_sig(ls, LUA_TTABLE, LUA_TTABLE, -1);

	material = get_lua_object(ls, -2, "__g3dmaterial", FALSE);
	image = get_lua_object(ls, -1, "__g3dimage", FALSE);

	material->tex_image = image;

	return 0;
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

	lua_pushcfunction(ls, _g3d_Material_setColor);
	lua_setfield(ls, -2, "setColor");

	lua_pushcfunction(ls, _g3d_Material_setAlpha);
	lua_setfield(ls, -2, "setAlpha");

	lua_pushcfunction(ls, _g3d_Material_setTexture);
	lua_setfield(ls, -2, "setTexture");

	return 1;
}

/* G3DImage */

static int _g3d_Image(lua_State *ls)
{
	G3DImage *image;

	check_lua_sig(ls, LUA_TSTRING, -1);

	image = g3d_texture_load_cached(
		get_g3d_context(ls), get_g3d_model(ls), lua_tostring(ls, -1));	
	if(image == NULL) {
		lua_pushnil(ls);
		return 1;
	}

	lua_newtable(ls);

	lua_pushlightuserdata(ls, image);
	lua_setfield(ls, -2, "__g3dimage");

	return 1;
}

/* G3DMatrix */

static int _g3d_Matrix_translate(lua_State *ls)
{
	G3DMatrix *matrix;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER, LUA_TTABLE, -1);

	matrix = get_lua_object(ls, -4, "__g3dmatrix", FALSE);
	g3d_matrix_translate(
		lua_tonumber(ls, -3),
		lua_tonumber(ls, -2),
		lua_tonumber(ls, -1),
		matrix);
	return 0;
}

static int _g3d_Matrix_scale(lua_State *ls)
{
	G3DMatrix *matrix;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER, LUA_TTABLE, -1);

	matrix = get_lua_object(ls, -4, "__g3dmatrix", FALSE);
	g3d_matrix_scale(
		lua_tonumber(ls, -3),
		lua_tonumber(ls, -2),
		lua_tonumber(ls, -1),
		matrix);
	return 0;
}

static int _g3d_Matrix_rotateXYZ(lua_State *ls)
{
	G3DMatrix *matrix;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER, LUA_TTABLE, -1);

	matrix = get_lua_object(ls, -4, "__g3dmatrix", FALSE);
	g3d_matrix_rotate_xyz(
		lua_tonumber(ls, -3),
		lua_tonumber(ls, -2),
		lua_tonumber(ls, -1),
		matrix);
	return 0;
}

static int _g3d_Matrix_rotate(lua_State *ls)
{
	G3DMatrix *matrix;

	check_lua_sig(ls, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER,
		LUA_TTABLE, -1);

	matrix = get_lua_object(ls, -5, "__g3dmatrix", FALSE);
	g3d_matrix_rotate(
		lua_tonumber(ls, -4),
		lua_tonumber(ls, -3),
		lua_tonumber(ls, -2),
		lua_tonumber(ls, -1),
		matrix);
	return 0;
}

static int _g3d_Matrix(lua_State *ls)
{
	G3DMatrix *matrix;

	matrix = g3d_matrix_new();

	lua_newtable(ls);

	lua_pushlightuserdata(ls, matrix);
	lua_setfield(ls, -2, "__g3dmatrix");

	lua_pushcfunction(ls, _g3d_Matrix_translate);
	lua_setfield(ls, -2, "translate");
	
	lua_pushcfunction(ls, _g3d_Matrix_scale);
	lua_setfield(ls, -2, "scale");

	lua_pushcfunction(ls, _g3d_Matrix_rotateXYZ);
	lua_setfield(ls, -2, "rotateXYZ");

	lua_pushcfunction(ls, _g3d_Matrix_rotate);
	lua_setfield(ls, -2, "rotate");

	return 1;
}

/* G3DModel */

static int _g3d_model_addObject(lua_State *ls)
{
	G3DModel *model;
	G3DObject *object;

	check_lua_sig(ls, LUA_TTABLE, LUA_TTABLE, -1);

	model = get_lua_object(ls, -2, "__g3dmodel", FALSE);
	object = get_lua_object(ls, -1, "__g3dobject", FALSE);

	model->objects = g_slist_append(model->objects, object);

	return 0;
}

/*****************************************************************************/
/* register functions                                                        */

static void _g3d_model__register(lua_State *ls, G3DModel *model)
{
	lua_getglobal(ls, "g3d");

	lua_newtable(ls);

	lua_pushlightuserdata(ls, model);
	lua_setfield(ls, -2, "__g3dmodel");

	lua_pushcfunction(ls, _g3d_model_addObject);
	lua_setfield(ls, -2, "addObject");

	lua_setfield(ls, -2, "model");

	lua_pop(ls, 1);
}

static void _g3d_context__register(lua_State *ls, G3DContext *context)
{
	lua_getglobal(ls, "g3d");

	lua_newtable(ls);

	lua_pushlightuserdata(ls, context);
	lua_setfield(ls, -2, "__g3dcontext");

	lua_setfield(ls, -2, "context");

	lua_pop(ls, 1);
}

static const luaL_Reg g3d_functions[] = {
	{ "debug",       _g3d_debug },
	{ "Material",    _g3d_Material },
	{ "Face",        _g3d_Face },
	{ "Object",      _g3d_Object },
	{ "Image",       _g3d_Image },
	{ "Matrix",      _g3d_Matrix },
	{ NULL, NULL }
};

gboolean lua_funcs_register(lua_State *ls,
	G3DContext *context, G3DModel *model)
{
	luaL_register(ls, "g3d", g3d_functions);
	_g3d_model__register(ls, model);
	_g3d_context__register(ls, context);

	return TRUE;
}

