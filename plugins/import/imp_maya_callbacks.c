#include <string.h>

#include <g3d/iff.h>
#include <g3d/material.h>
#include <g3d/primitive.h>
#include <g3d/read.h>

#include "imp_maya_obj.h"
#include "imp_maya_var.h"

/* creator */
gboolean maya_cb_CREA(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	MayaObject *obj;
	gint32 max_len, flags;
	gchar *buffer;

	obj = (MayaObject *)local->object;
	if(obj)
	{
		/* flags ? */
		flags = g3d_read_int8(global->f);
		local->nb -= 1;

		max_len = local->nb;
		buffer = g_malloc(max_len + 1);

		/* object name */
		local->nb -= g3d_read_cstr(global->f, buffer, max_len);
		obj->name = g_strdup(buffer);

		/* parent name */
		if(local->nb > 0)
		{
			local->nb -= g3d_read_cstr(global->f, buffer, max_len);
		}

		g_free(buffer);
	}

	return TRUE;
}

gboolean maya_cb_DBLn(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gint32 len, flags, ndbl, i;
	gdouble val;
	gchar *var;
	gchar *padding = "                         ";

	len = local->nb - 9;
	var = g_malloc(len);
	fread(var, 1, len, global->f);
	local->nb -= len;

	flags = g3d_read_int8(global->f);
	local->nb -= 1;

	ndbl = local->nb / 8;
	for(i = 0; i < ndbl; i ++)
	{
		val = g3d_read_double_be(global->f);
		local->nb -= 8;
	}

	g_debug("%s[Maya][DBL#] %s (%d doubles) (0x%02X)",
		padding + (strlen(padding) - local->level) + 1,
		var, ndbl, flags);

	g_free(var);

	return TRUE;
}

gboolean maya_cb_DBL2(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gint32 len, flags;
	gdouble val1, val2;
	gchar *var;
	gchar *padding = "                         ";

	len = local->nb - 17;
	var = g_malloc(len);
	fread(var, 1, len, global->f);
	local->nb -= len;

	flags = g3d_read_int8(global->f);
	local->nb -= 1;

	val1 = g3d_read_double_be(global->f);
	val2 = g3d_read_double_be(global->f);
	local->nb -= 16;

	g_debug("%s[Maya][DBL2] %s = (%g,%g) (0x%02X)",
		padding + (strlen(padding) - local->level) + 1,
		var, val1, val2, flags);

	g_free(var);

	return TRUE;
}

gboolean maya_cb_DBL3(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gint32 len, flags;
	gdouble val1, val2, val3;
	gchar *var;
	gchar *padding = "                         ";

	len = local->nb - 25;
	var = g_malloc(len);
	fread(var, 1, len, global->f);
	local->nb -= len;

	flags = g3d_read_int8(global->f);
	local->nb -= 1;

	val1 = g3d_read_double_be(global->f);
	val2 = g3d_read_double_be(global->f);
	val3 = g3d_read_double_be(global->f);
	local->nb -= 24;

	g_debug("%s[Maya][DBL3] %s = (%g,%g,%g) (0x%02X)",
		padding + (strlen(padding) - local->level) + 1,
		var, val1, val2, val3, flags);

	g_free(var);

	return TRUE;
}

gboolean maya_cb_DBLE(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gint32 len, flags;
	gdouble val;
	gchar *var;
	gchar *padding = "                         ";

	len = local->nb - 9;
	var = g_malloc(len);
	fread(var, 1, len, global->f);
	local->nb -= len;

	flags = g3d_read_int8(global->f);
	local->nb -= 1;

	val = g3d_read_double_be(global->f);
	local->nb -= 8;

	g_debug("%s[Maya][DBLE] %s = %g (0x%02X)",
		padding + (strlen(padding) - local->level) + 1,
		var, val, flags);

	if(local->object)
		maya_var_set_double((MayaObject *)local->object, var, val);

	g_free(var);

	return TRUE;
}

gboolean maya_cb_MESH(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	gint32 x1, x2, x3, x4, i, i1, i2;

	x1 = g3d_read_int16_be(global->f);
	x2 = g3d_read_int16_be(global->f);
	x3 = g3d_read_int16_be(global->f);
	x4 = g3d_read_int16_be(global->f);
	local->nb -= 8;
#if 0
	g_debug("[Maya][MESH] %d %d %d %d @ 0x%08x", x1, x2, x3, x4,
		(unsigned int)ftell(global->f));
#endif
	if(x1 == 0x6369)
	{
		object = g_new0(G3DObject, 1);
		object->name = g_strdup_printf("MESH @ 0x%08x",
			(guint32)ftell(global->f) - 16);
		material = g3d_material_new();
		material->name = g_strdup("(default material)");
		object->materials = g_slist_append(object->materials, material);
		global->model->objects = g_slist_append(global->model->objects,
			object);

		object->vertex_count = x4 / 3;
		object->vertex_data = g_new0(gfloat, object->vertex_count * 3);

		for(i = 0; i < object->vertex_count; i ++)
		{
			object->vertex_data[i * 3 + 0] = g3d_read_float_be(global->f);
			object->vertex_data[i * 3 + 1] = g3d_read_float_be(global->f);
			object->vertex_data[i * 3 + 2] = g3d_read_float_be(global->f);
			local->nb -= 12;
		}

		x3 = g3d_read_int16_be(global->f);
		x4 = g3d_read_int16_be(global->f);
		local->nb -= 4;
#if 0
		g_debug("[Maya][MESH] %d %d", x3, x4);
#endif
		i1 = -1;
		i2 = -1;
		for(i = 0; i < x4 / 2; i ++)
		{
			if(i1 == -1)
			{
				i1 = g3d_read_int32_be(global->f) & 0xFFFFFF;
				i2 = g3d_read_int32_be(global->f) & 0xFFFFFF;
				local->nb -= 8;
			}
			else
			{
				face = g_new0(G3DFace, 1);
				face->vertex_count = 4;
				face->vertex_indices = g_new0(guint32, 4);
				face->vertex_indices[0] = i1;
				face->vertex_indices[1] = i2;
				face->vertex_indices[2] =
					g3d_read_int32_be(global->f) & 0xFFFFFF;
				face->vertex_indices[3] =
					g3d_read_int32_be(global->f) & 0xFFFFFF;
				face->material = material;
				local->nb -= 8;
				i1 = face->vertex_indices[3];
				i2 = face->vertex_indices[2];

				object->faces = g_slist_append(object->faces, face);
			}
		}

		x3 = g3d_read_int16_be(global->f);
		x4 = g3d_read_int16_be(global->f);
		local->nb -= 4;
#if 0
		g_debug("[Maya][MESH] %d %d", x3, x4);
#endif
#if 0
		for(i = 0; i < x4 / 4; i ++)
		{
			face = g_new0(G3DFace, 1);
			face->vertex_count = 4;
			face->vertex_indices = g_new0(guint32, 4);
			face->vertex_indices[0] = g3d_read_int32_be(global->f) & 0xFFFFFF;
			face->vertex_indices[1] = g3d_read_int32_be(global->f) & 0xFFFFFF;
			face->vertex_indices[2] = g3d_read_int32_be(global->f) & 0xFFFFFF;
			face->vertex_indices[3] = g3d_read_int32_be(global->f) & 0xFFFFFF;
			face->material = material;
			local->nb -= 16;

			object->faces = g_slist_append(object->faces, face);
		}
#endif
	}
	return TRUE;
}

/* poly cube */
gboolean maya_cb_PCUB(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	MayaObject *obj;
	G3DObject *object;
	G3DMaterial *material;
	gdouble w, h, d;

	if(local->finalize)
	{
		obj = (MayaObject *)local->object;
		g_return_val_if_fail(obj != NULL, FALSE);

		w = maya_var_get_double(obj, "sw");
		h = maya_var_get_double(obj, "sh");
		d = maya_var_get_double(obj, "sd");

		material = g3d_material_new();
		object = g3d_primitive_cube(w, h, d, material);
		object->materials = g_slist_append(object->materials, material);
		global->model->objects = g_slist_append(global->model->objects,
			object);

		if(obj->name) object->name = g_strdup(obj->name);

		/* destroy object */
		maya_obj_free(obj);
	}
	else
	{
		/* create object */
		obj = maya_obj_new();
		local->object = obj;
	}

	return TRUE;
}

gboolean maya_cb_STR_(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gchar *buffer, *p;

	/* maximum size */
	buffer = g_malloc(local->nb);

	while(local->nb > 0)
	{
		p = buffer;
		do
		{
			*p = g3d_read_int8(global->f);
			local->nb --;
			p ++;
		}
		while(*(p - 1) != '\0');
	}

	g_free(buffer);

	return FALSE;
}
