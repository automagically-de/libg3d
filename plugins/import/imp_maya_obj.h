#ifndef _IMP_MAYA_OBJ_H
#define _IMP_MAYA_OBJ_H

#include <glib.h>

typedef struct {
	gchar *name;
	GHashTable *vars;
	gpointer user_data;
} MayaObject;

MayaObject *maya_obj_new(void);
void maya_obj_free(MayaObject *obj);

#endif /* _IMP_MAYA_OBJ_H */
