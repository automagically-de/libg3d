#ifndef _IMP_VRML2_H
#define _IMP_VRML2_H

#include <glib.h>

#include <g3d/types.h>
#include <g3d/stream.h>

typedef struct {
	G3DStream *stream;
	gchar *buffer;
	gchar *bufp;
	gsize bufsize;
	gsize max_bufsize;
	guint32 line;

	G3DModel *model;
	G3DObject *object;
} VrmlReader;

#endif
