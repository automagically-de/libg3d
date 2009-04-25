#ifndef _IMP_LUA_GC_H
#define _IMP_LUA_GC_H

#include <g3d/types.h>

typedef struct _G3DLuaGC G3DLuaGC;

typedef enum {
	T_GENERIC,
	T_G3DFACE,
	T_G3DOBJECT,
	T_G3DMATRIX,
	T_G3DMATERIAL,
	T_G3DIMAGE
} G3DLuaGCType;

G3DLuaGC *lua_gc_init(void);
void lua_gc_cleanup(G3DLuaGC *gc, G3DModel *model);

gboolean lua_gc_add(G3DLuaGC *gc, G3DLuaGCType type, gpointer ptr);
gboolean lua_gc_remove(G3DLuaGC *gc, gpointer ptr);

#endif /* _IMP_LUA_GC_H */
