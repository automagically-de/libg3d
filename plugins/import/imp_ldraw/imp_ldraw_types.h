#ifndef _IMP_LDRAW_TYPES_H
#define _IMP_LDRAW_TYPES_H

#include <glib.h>

#include <g3d/types.h>
#include <g3d/stream.h>

typedef struct {
	gchar *name;
	gchar *subdir;
	G3DObject *object;
	G3DStream *stream;
	gboolean failed;
} LDrawPart;

typedef struct {
	gchar *libdir;
	GHashTable *partdb;
	GSList *partlist;
	GHashTable *colordb;
	GSList *colorlist;
} LDrawLibrary;

#endif /* _IMP_LDRAW_TYPES_H */
