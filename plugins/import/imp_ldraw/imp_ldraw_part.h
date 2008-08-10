#ifndef _IMP_LDRAW_PART_H
#define _IMP_LDRAW_PART_H

#include "imp_ldraw_types.h"

G3DObject *ldraw_part_from_file(LDrawLibrary *lib, const gchar *filename);
G3DObject *ldraw_part_get_object(LDrawPart *part, LDrawLibrary *lib);
void ldraw_part_free(LDrawPart *part);

#endif /* _IMP_LDRAW_PART_H */
