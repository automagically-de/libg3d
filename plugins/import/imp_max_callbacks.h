#ifndef _IMP_MAX_CALLBACKS_H
#define _IMP_MAX_CALLBACKS_H

#include <stdio.h>
#include <glib.h>
#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	gchar *padding;
	const gchar *subfile;
} MaxGlobalData;

typedef struct {
	guint16 id;
	gint32 parentid;
	guint16 level;
	gint32 nb;
	gpointer object;
} MaxLocalData;

typedef gboolean (* MaxCallback)(MaxGlobalData *global, MaxLocalData *local);

/* callbacks */

gboolean max_cb_debug_int32(MaxGlobalData *global, MaxLocalData *local);
gboolean max_cb_debug_string(MaxGlobalData *global, MaxLocalData *local);
gboolean max_cb_debug_wchars(MaxGlobalData *global, MaxLocalData *local);

gboolean max_cb_PIROOT_0x001B(MaxGlobalData *global, MaxLocalData *local);

gboolean max_cb_0x0001_0x0005(MaxGlobalData *global, MaxLocalData *local);

gboolean max_cb_0x0005_0x4000(MaxGlobalData *global, MaxLocalData *local);

gboolean max_cb_0x001B_0x08FE(MaxGlobalData *global, MaxLocalData *local);
gboolean max_cb_0x001B_0x0962(MaxGlobalData *global, MaxLocalData *local);

gboolean max_cb_0x08FE_0x0100(MaxGlobalData *global, MaxLocalData *local);
gboolean max_cb_0x08FE_0x010A(MaxGlobalData *global, MaxLocalData *local);
gboolean max_cb_0x08FE_0x0912(MaxGlobalData *global, MaxLocalData *local);
gboolean max_cb_0x08FE_0x0914(MaxGlobalData *global, MaxLocalData *local);

gboolean max_cb_0x4000_0x4001(MaxGlobalData *global, MaxLocalData *local);
gboolean max_cb_0x4000_0x4030(MaxGlobalData *global, MaxLocalData *local);

#endif /* _IMP_MAX_CALLBACKS_H */
