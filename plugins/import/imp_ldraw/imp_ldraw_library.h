#ifndef _IMP_LDRAW_LIBRARY_H
#define _IMP_LDRAW_LIBRARY_H

#include "imp_ldraw_types.h"

LDrawLibrary *ldraw_library_init(void);
void ldraw_library_cleanup(LDrawLibrary *lib);
void ldraw_library_insert(LDrawLibrary *lib, gchar *name, gpointer data);
G3DObject *ldraw_library_lookup(LDrawLibrary *lib, const gchar *name);

#endif
