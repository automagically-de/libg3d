#include <g3d/read.h>

#include "imp_flt_callbacks.h"

/* group */
gboolean flt_cb_0002(FltGlobalData *gd, FltLocalData *ld)
{
	G3DObject *object, *pobj;
	gchar namebuf[8];

	object = g_new0(G3DObject, 1);
	pobj = (G3DObject *)g_queue_peek_head(gd->oqueue);
	if(pobj != NULL)
		pobj->objects = g_slist_append(pobj->objects, object);
	else
		gd->model->objects = g_slist_append(gd->model->objects, object);
	ld->object = object;

	/* group ID */
	fread(namebuf, 1, 8, gd->f);
	ld->nb -= 8;
	namebuf[7] = '\0';
	object->name = g_strdup_printf("group '%s'", namebuf);

	return TRUE;
}

/* object */
gboolean flt_cb_0004(FltGlobalData *gd, FltLocalData *ld)
{
	G3DObject *object, *pobj;
	gchar namebuf[8];

	object = g_new0(G3DObject, 1);
	pobj = (G3DObject *)g_queue_peek_head(gd->oqueue);
	if(pobj != NULL)
		pobj->objects = g_slist_append(pobj->objects, object);
	else
		gd->model->objects = g_slist_append(gd->model->objects, object);
	ld->object = object;

	/* object ID */
	fread(namebuf, 1, 8, gd->f);
	ld->nb -= 8;
	namebuf[7] = '\0';
	object->name = g_strdup_printf("object '%s'", namebuf);

	/* flags */
	g3d_read_int32_be(gd->f);
	ld->nb -= 4;

	/* relative priority */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* transparency */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* special effect ID 1 */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* special effect ID 2 */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* significance */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	/* reserved */
	g3d_read_int16_be(gd->f);
	ld->nb -= 2;

	return TRUE;
}

/* face */
gboolean flt_cb_0005(FltGlobalData *gd, FltLocalData *ld)
{
	return TRUE;
}

/* push level */
gboolean flt_cb_0010(FltGlobalData *gd, FltLocalData *ld)
{
	g_queue_push_head(gd->oqueue, ld->object);

	gd->level ++;
	return TRUE;
}

/* pop level */
gboolean flt_cb_0011(FltGlobalData *gd, FltLocalData *ld)
{
	g_queue_pop_head(gd->oqueue);

	if(gd->level > 0)
		gd->level --;
	return TRUE;
}

/* vertex list */
gboolean flt_cb_0072(FltGlobalData *gd, FltLocalData *ld)
{
	return TRUE;
}
