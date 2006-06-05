#include "imp_maya_obj.h"

MayaObject *maya_obj_new(void)
{
	MayaObject *obj;

	obj = g_new0(MayaObject, 1);
	obj->vars = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	return obj;
}

void maya_obj_free(MayaObject *obj)
{
	g_hash_table_destroy(obj->vars);
	if(obj->name) g_free(obj->name);
	g_free(obj);
}

