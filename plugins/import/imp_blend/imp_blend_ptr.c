#include "imp_blend_ptr.h"

GHashTable *blend_ptr_init(void)
{
	return g_hash_table_new_full(g_int_hash, g_int_equal, NULL, g_free);
}

void blend_ptr_cleanup(GHashTable *hash)
{
	g_hash_table_destroy(hash);
}

gboolean blend_ptr_insert(GHashTable *hash, goffset address, BlendPtrType type,
	gpointer object)
{
	BlendPtr *ptr;

	ptr = g_new0(BlendPtr, 1);
	ptr->address = address;
	ptr->type = type;
	ptr->value = object;
	g_hash_table_insert(hash, &(ptr->address), ptr);
	return TRUE;
}

gpointer blend_ptr_get(GHashTable *hash, goffset address, BlendPtrType type)
{
	BlendPtr *ptr;

	ptr = g_hash_table_lookup(hash, &address);
	if(ptr && (ptr->type == type))
		return ptr->value;
	return NULL;
}

