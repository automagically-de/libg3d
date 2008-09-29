#ifndef _IMP_3DMF_CALLBACKS_H
#define _IMP_3DMF_CALLBACKS_H

#include <g3d/types.h>
#include <g3d/stream.h>

typedef struct {
	G3DContext *context;
	G3DStream *stream;
} X3dmfGlobal;

typedef struct {
	guint32 id;
	guint32 parentid;
	guint32 level;
	G3DObject *object;
} X3dmfLocal;

typedef gboolean (* X3dmfCallback)(X3dmfGlobal *global, X3dmfLocal *local);

#endif /* _IMP_3DMF_CALLBACKS_H */
