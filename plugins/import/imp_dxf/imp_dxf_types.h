#ifndef _IMP_DXF_TYPES_H
#define _IMP_DXF_TYPES_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	gboolean binary;
} DxfGlobalData;

typedef struct {
	G3DObject *object;
	G3DObject *block;
	G3DMaterial *material;
	G3DFace *face;
	guint32 vertex_offset;
	guint32 polyline_flags;
	gint32 tmp_70, tmp_71;
	gint32 tmp_i1;
} DxfEntityData;

typedef struct {
	gint32 id;
	gint32 parentid;
	guint32 sid; /* section id */
	guint32 eid; /* entity id */
	DxfEntityData *edata;
} DxfLocalData;

typedef gboolean (* DxfCallback)(DxfGlobalData *, DxfLocalData *);

typedef struct {
	const gchar *name;
	const gchar *description;
	guint32 id;
	DxfCallback callback;
} DxfEntityInfo;

typedef struct {
	gint32 id;
	const gchar *description;
	DxfCallback callback;
} DxfChunkInfo;

#endif /* _IMP_DXF_TYPES_H */
