#ifndef _IMP_FLT_CALLBACKS_H
#define _IMP_FLT_CALLBACKS_H

#include <stdio.h>
#include <glib.h>
#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	FILE *f;
	guint32 level;
} FltGlobalData;

typedef struct {
	guint32 opcode;
	gpointer object;
	gint32 nb;
} FltLocalData;

typedef gboolean (*FltCallbackFunc)(FltGlobalData *gd, FltLocalData *ld);

/* callback functions */
gboolean flt_cb_0004(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0005(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0010(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0011(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0072(FltGlobalData *gd, FltLocalData *ld);

#endif /* _IMP_FLT_CALLBACKS_H */
