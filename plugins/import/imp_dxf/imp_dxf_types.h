#ifndef _IMP_DXF_TYPES_H
#define _IMP_DXF_TYPES_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	gboolean binary;
	GHashTable *blocks;
} DxfGlobalData;

typedef struct {
	G3DObject *object;
	G3DObject *block;
	G3DMaterial *material;
	guint32 vertex_offset;
	guint32 tmp_i1;
	guint32 polyline_flags;
} DxfEntityData;

typedef struct _DxfEntityProps DxfEntityProps;

typedef struct {
	gint32 id;
	guint32 sid; /* section id */
	guint32 eid; /* entity id */
	DxfEntityData *edata;
	DxfEntityProps *eprop;
} DxfLocalData;

typedef gboolean (* DxfCallback)(DxfGlobalData *, DxfLocalData *);

typedef struct {
	const gchar *name;
	const gchar *description;
	guint32 id;
	DxfCallback callback;
} DxfEntityInfo;

typedef enum {
	DXF_T_UNKNOWN,
	DXF_T_EMPTY,
	DXF_T_INT16,
	DXF_T_INT32,
	DXF_T_FLOAT64,
	DXF_T_STRING
} DxfChunkType;

typedef struct {
	gint32 id;
	const gchar *description;
	DxfChunkType type;
} DxfChunkInfo;

#endif /* _IMP_DXF_TYPES_H */
