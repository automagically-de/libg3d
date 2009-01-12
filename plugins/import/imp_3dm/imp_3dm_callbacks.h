#ifndef _IMP_3DM_CALLBACKS_H
#define _IMP_3DM_CALLBACKS_H

#include "imp_3dm.h"

typedef struct {
	guint32 tcode;
	guint32 len;
	guint32 data;
	G3DObject *object;
} TdmLocal;

typedef gboolean (* TdmCallback)(TdmGlobal *, TdmLocal *);

typedef struct {
	guint32 tcode;
	gboolean container;
	gboolean endofcnt;
	const gchar *description;
	TdmCallback callback;
} TdmChunkInfo;

#endif /* _IMP_3DM_CALLBACKS_H */
