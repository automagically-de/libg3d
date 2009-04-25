#ifndef _IMP_LUA_FUNCS_H
#define _IMP_LUA_FUNCS_H

#include <glib.h>
#include <lauxlib.h>

gboolean lua_funcs_register(lua_State *ls,
	G3DContext *context, G3DModel *model);
void lua_funcs_cleanup(lua_State *ls);

#endif /* _IMP_LUA_FUNCS_H */


