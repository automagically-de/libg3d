#ifndef _IMP_3DM_CALLBACKS_H
#define _IMP_3DM_CALLBACKS_H

#include "imp_3dm.h"

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

gboolean tdm_cb_0x00027ffc(TdmGlobal *global, TdmLocal *local);
gboolean tdm_cb_0x02000071(TdmGlobal *global, TdmLocal *local);
gboolean tdm_cb_0x0200007f(TdmGlobal *global, TdmLocal *local);
gboolean tdm_cb_0x20000070(TdmGlobal *global, TdmLocal *local);

gboolean tdm_cb_o_0x00000020(TdmGlobal *global, TdmLocal *local);

#endif /* _IMP_3DM_CALLBACKS_H */
