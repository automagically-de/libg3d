#ifndef _IMP_DXF_CALLBACKS_H
#define _IMP_DXF_CALLBACKS_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
} DxfGData;

typedef struct {
	gint32 id;
	gint32 parentid;
	G3DObject *object;
} DxfLData;

typedef gboolean (* DxfCallback)(DxfGData *, DxfLData *);

#endif /* _IMP_DXF_CALLBACKS_H */
