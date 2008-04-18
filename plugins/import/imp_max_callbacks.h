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
} MaxGlobalData;

typedef struct {
	guint16 id;
	guint16 level;
	gint32 nb;
	gpointer object;
} MaxLocalData;

typedef gboolean (* MaxCallback)(MaxGlobalData *global, MaxLocalData *local);

/* callbacks */

gboolean max_cb_0x0005(MaxGlobalData *global, MaxLocalData *local);
gboolean max_cb_0x0100(MaxGlobalData *global, MaxLocalData *local);

#endif /* _IMP_MAX_CALLBACKS_H */
