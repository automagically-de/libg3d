#include <g3d/face.h>
#include <g3d/object.h>
#include <g3d/material.h>
#include <g3d/texture.h>

#include "imp_lua_gc.h"

struct _G3DLuaGC {
	GHashTable *store;
};

typedef struct {
	G3DLuaGCType type;
	gpointer ptr;
} G3DLuaGCItem;

G3DLuaGC *lua_gc_init(void)
{
	G3DLuaGC *gc = g_new0(G3DLuaGC, 1) ;
	gc->store = g_hash_table_new(g_direct_hash, g_direct_equal);
	return gc;
}

static gboolean lua_gc_free_item_cb(gpointer key, gpointer value, gpointer d)
{
	G3DLuaGCItem *item = value;
	G3DModel *model = d;

#if DEBUG > 0
	g_debug("gc: free data of type %d: %p", item->type, item->ptr);
#endif

	switch(item->type) {
		case T_GENERIC:
		case T_G3DMATRIX:
			g_free(item->ptr);
			break;
		case T_G3DFACE:
			g3d_face_free((G3DFace *)item->ptr);
			break;
		case T_G3DMATERIAL:
			g3d_material_free((G3DMaterial *)item->ptr);
			break;
		case T_G3DOBJECT:
			g3d_object_free((G3DObject *)item->ptr);
			break;
		case T_G3DIMAGE:
			if(g3d_texture_cache_remove(model, (G3DImage *)item->ptr)) {
				g3d_texture_free((G3DImage *)item->ptr);
			}
			break;
	}
	g_free(item);
	return TRUE;
}

void lua_gc_cleanup(G3DLuaGC *gc, G3DModel *model)
{
	g_hash_table_foreach_remove(gc->store, lua_gc_free_item_cb, model);
	g_hash_table_destroy(gc->store);
	g_free(gc);
}

gboolean lua_gc_add(G3DLuaGC *gc, G3DLuaGCType type, gpointer ptr)
{
	G3DLuaGCItem *item = g_new0(G3DLuaGCItem, 1);
	item->type = type;
	item->ptr = ptr;
	g_hash_table_insert(gc->store, item->ptr, item);
	return TRUE;
}


gboolean lua_gc_remove(G3DLuaGC *gc, gpointer ptr)
{
	G3DLuaGCItem *item = g_hash_table_lookup(gc->store, ptr);
	if(item) {
		g_hash_table_remove(gc->store, ptr);
		g_free(item);
		return TRUE;
	}
	return FALSE;
}
