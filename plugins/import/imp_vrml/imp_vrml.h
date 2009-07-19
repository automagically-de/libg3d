#ifndef _IMP_VRML_H
#define _IMP_VRML_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;

	guint32 line_offset;
	guint32 scope;
	gint32 level;

	/* v1 */
	gchar *defname;
	guint32 value_counter;
	guint32 value_reload;
	gboolean in_list;
	gboolean negate_next;
	guint32 list_item_count;
	const gchar *current_group;
	const gchar *current_option;

	gpointer property;

	gint32 i_list;
	gint32 i_val;

	GQueue *stack;
	/* V2 */
	gpointer opaque;

	/* common */
} VrmlGlobal;

typedef struct {
	GTokenType type;
	guint32 n_items;
	guint32 n_per_value;
	union {
		gchar *s;
		G3DFloat *fp;
		gint32 *ip;
	} u;
} VrmlProperty;

typedef struct {
	const gchar *name;
	gchar *defname;
	gpointer symbol;
	GHashTable *properties;
	G3DObject *object;
} VrmlNode;

typedef enum {
	VRML_STEP_OPEN,
	VRML_STEP_CLOSE
} VrmlStep;

typedef gboolean (* VrmlCallback)(VrmlGlobal *, VrmlNode *, VrmlStep);

#endif /* _IMP_VRML_H */
