#ifndef _IMP_MESH_H
#define _IMP_MESH_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DStream *stream;
	G3DModel *model;
	G3DMaterial *defmat;
	guint16 major;
	guint16 minor;
} MeshGlobal;

typedef struct {
	guint32 chunkid;
	guint32 level;
	G3DObject *object;
	gint32 nb;
	guint32 nindices;
	guint32 *indices;
} MeshLocal;

#endif
