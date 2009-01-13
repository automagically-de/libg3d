#ifndef _IMP_3DM_TYPES_H
#define _IMP_3DM_TYPES_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DStream *stream;
	G3DModel *model;
} TdmGlobal;

typedef struct {
	guint32 tcode;
	guint32 len;
	guint32 data;
	guint32 level;
	guint16 major_version;
	guint16 minor_version;
	gpointer object;
} TdmLocal;

typedef gboolean (* TdmCallback)(TdmGlobal *, TdmLocal *);

typedef struct {
	guint32 tcode;
	gboolean container;
	gboolean endofcnt;
	const gchar *description;
	TdmCallback callback;
} TdmChunkInfo;

typedef struct {
	guint32 code;
	const gchar *description;
	TdmCallback callback;
} TdmObjectTypeInfo;

typedef struct {
	guint32 otype;
	G3DObject *object;
} TdmObjectRecord;

#endif /* _IMP_3DM_TYPES_H */
