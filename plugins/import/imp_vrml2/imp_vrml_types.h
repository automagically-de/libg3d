#ifndef _IMP_VRML_TYPES_H
#define _IMP_VRML_TYPES_H

#include "imp_vrml2.h"

typedef enum {
	T_UNKNOWN = 0,
	T_SPECIAL,
	T_OBJECT,
	T_TOKEN2,
	T_TOKEN3,
	T_STRING,
	T_BOOLEAN,
	T_INT,
	T_FLOAT,
	T_FLOAT2,
	T_FLOAT3,
	T_FLOAT4,
	T_FLOAT_X,
	T_LIST_OBJECT,
	T_LIST_STRING,
	T_LIST_INT,
	T_LIST_FLOAT,
	T_LIST_FLOAT2,
	T_LIST_FLOAT3,
	T_LIST_FLOAT4,
	T_LIST_FLOAT_X,
	N_TYPES
} VrmlType;

typedef struct {
	gchar *name;
	gchar *defid;
	gboolean define;
	guint32 level;
	GSList *properties;
} VrmlObject;

typedef struct {
	gchar *name;
	VrmlType id;

	gsize n_items;
	VrmlObject **v_object;
	gchar **v_str;
	gfloat *v_float;
	gint32 *v_int;
	gboolean v_boolean;
	guint32 level;
} VrmlProperty;

typedef VrmlType (* VrmlTypeDetect)(VrmlReader *reader);

typedef struct {
	const gchar *name;
	VrmlType id;
	VrmlTypeDetect detect;
} VrmlTypeDef;

typedef gboolean (* VrmlNodeCallback)(VrmlReader *reader, VrmlObject *object);

typedef struct {
	const gchar *name;
	VrmlNodeCallback callback;
} VrmlNodeDef;

#endif /* _IMP_VRML_TYPES_H */
