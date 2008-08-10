#ifndef _IMP_LDRAW_COLOR_H
#define _IMP_LDRAW_COLOR_H

#include "imp_ldraw_types.h"

gboolean ldraw_color_init(LDrawLibrary *lib);
G3DMaterial *ldraw_color_lookup(LDrawLibrary *lib, guint32 colid);

#endif /* _IMP_LDRAW_COLOR_H */
