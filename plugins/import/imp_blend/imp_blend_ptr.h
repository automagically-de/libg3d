#ifndef _IMP_BLEND_PTR_H
#define _IMP_BLEND_PTR_H

#include "imp_blend_types.h"

GHashTable *blend_ptr_init(void);
void blend_ptr_cleanup(GHashTable *hash);
gpointer blend_ptr_get(GHashTable *hash, goffset address, BlendPtrType type);
gboolean blend_ptr_insert(GHashTable *hash, goffset address, BlendPtrType type,
	gpointer object);

#endif /* _IMP_BLEND_PTR_H */
