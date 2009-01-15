#ifndef _IMP_BLEND_TYPES_H
#define _IMP_BLEND_TYPES_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	guint32 flags;
} BlendGlobal;

typedef struct {
	guint32 id;
	guint32 len;
	gpointer obj;
} BlendLocal;

#endif
