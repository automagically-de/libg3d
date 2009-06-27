#ifndef _IMP_VRML_H
#define _IMP_VRML_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;

	guint32 scope;
	gint32 level;
	gchar *defname;
	guint32 value_counter;
	guint32 value_reload;
	gboolean in_list;
	guint32 list_item_count;
	const gchar *current_group;
	const gchar *current_option;
} VrmlGlobal;

#endif /* _IMP_VRML_H */
