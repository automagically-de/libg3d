#ifndef _IMP_BLEND_TYPES_H
#define _IMP_BLEND_TYPES_H

#include <g3d/types.h>

typedef struct {
	gchar *name;
	const gchar *tname;
	guint32 tid;
	gsize tsize;
	gsize nitems;
	gboolean ptr;
} BlendSdnaProperty;

typedef struct {
	const gchar *name;
	guint32 tid;
	gsize size;
	GSList *properties;
} BlendSdnaStruct;

typedef struct {
	const BlendSdnaStruct *sstruct;
	GHashTable *prophash;
} BlendSdnaData;

typedef enum {
	T_CHAR    = 0,
	T_UCHAR   = 1,
	T_SHORT   = 2,
	T_USHORT  = 3,
	T_INT     = 4,
	T_LONG    = 5,
	T_ULONG   = 6,
	T_FLOAT   = 7,
	T_DOUBLE  = 8,
	T_VOID    = 9,
	T_STRUCT  = 0xFF
} BlendSdnaPropType;

typedef struct {
	gchar *name;
	BlendSdnaPropType type;
	BlendSdnaProperty *sprop;
	guint32    *uval; /* unsigned integer value */
	gint32     *ival; /* signed integer value */
	G3DFloat   *fval; /* floating point value */
	guint32    *pval; /* pointer value */
	BlendSdnaData *structval; /* struct value */
} BlendSdnaPropData;

typedef struct {
	guint32 n_names;
	gchar **names;
	guint32 n_types;
	gchar **type_names;
	guint32 *type_sizes;
	GSList *structs;
} BlendSdna;

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	guint32 flags;
	BlendSdna *sdna;
} BlendGlobal;

typedef struct {
	guint32 id;
	guint32 len;
	G3DObject *object;
	guint32 object_id;
	guint32 ndata;
	BlendSdnaData **data;
} BlendLocal;

#endif
