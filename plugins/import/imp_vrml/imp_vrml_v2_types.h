#ifndef _IMP_VRML_V2_TYPES_H
#define _IMP_VRML_V2_TYPES_H

#include "imp_vrml.h"

typedef enum {
	VRML_V2_SCOPE_DEFAULT,
	VRML_V2_SCOPE_VALUE,

	VRML_V2_SCOPE_UNKNOWN
} VrmlV2Scope;

typedef enum {
	VRML_V2_TYPE_UNKNOWN,
	VRML_V2_TYPE_INT,
	VRML_V2_TYPE_FLOAT,
	VRML_V2_TYPE_STRING,
	VRML_V2_TYPE_OBJECT,
	VRML_V2_TYPE_BOOLEAN
} VrmlV2Type;

typedef struct {
	const gchar *name;
	VrmlV2Scope scope;
	guint32 n_per_item;
	VrmlV2Type valuetype;
} VrmlV2Symbol;

typedef struct {
	gint32 listlevel;

	GQueue *liststack;
	GQueue *objectstack;
} VrmlV2Data;

#endif /* _IMP_VRML_V2_TYPES_H */
