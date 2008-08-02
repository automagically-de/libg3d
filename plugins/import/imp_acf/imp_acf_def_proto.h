#ifndef _IMP_ACF_DEF_PROTO_H
#define _IMP_ACF_DEF_PROTO_H

#include <glib.h>

typedef enum {
	XEOF,
	XCHR,
	XINT,
	XFLT
} AcfType;

typedef struct {
	AcfType type;
	gchar *name;
	guint32 num;
	gchar *xchr;
	gint32 *xint;
	gfloat *xflt;
} AcfValue;

typedef struct {
	AcfType type;
	gchar *description;
	guint32 num;
} AcfDef;

#endif
