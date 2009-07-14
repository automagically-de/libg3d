#ifndef _IMP_VRML_V1_TYPES_H
#define _IMP_VRML_V1_TYPES_H

#include "imp_vrml.h"

typedef enum {
	VRML_V1_SCOPE_DEFAULT,
	VRML_V1_SCOPE_VALUE,
	VRML_V1_SCOPE_LIST,
	VRML_V1_SCOPE_DEF,
	VRML_V1_SCOPE_USE,

	VRML_V1_SCOPE_UNKNOWN
} VrmlScope;

typedef struct {
	const gchar *name;
	VrmlScope toscope;
	guint32 n_values;
	guint32 flags;
	VrmlCallback callback;
} VrmlSymbol;


#endif /* _IMP_VRML_V1_TYPES_H */
