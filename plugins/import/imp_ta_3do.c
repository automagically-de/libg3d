/* $Id: imp_maya.c 75 2006-11-06 10:52:56Z mmmaddd $ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005, 2006  Markus Dahms <mad@automagically.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>

#include <g3d/types.h>
#include <g3d/read.h>
#include <g3d/matrix.h>

gboolean ta_3do_read_children(G3DContext *context, FILE *f, GSList **list,
	G3DMaterial *materials);

gboolean ta_3do_load_object(G3DContext *context, const gchar *filename,
	G3DModel *model, G3DMaterial *materials)
{
	FILE *f;
	gboolean retval;

	f = fopen(filename, "rb");
	if(f == NULL)
	{
		g_warning("E: failed to read '%s'\n", filename);
		return FALSE;
	}

	retval =  ta_3do_read_children(context, f, &(model->objects), materials);
	fclose(f);

	return retval;
}

gboolean ta_3do_read_children(G3DContext *context, FILE *f, GSList **list,
	G3DMaterial *materials)
{
	G3DObject *object;
	G3DFace *face;
	guint32 off_save, off_sav2, off_sibl, off_chld, off_vert, off_prim, off_i;
	guint32 num_prims, colidx;
	gchar buffer[1025];
	gint32 i, j, x, y, z;

	while(!feof(f))
	{
		/* signature */
		if(g3d_read_int32_le(f) != 1)
			return FALSE;

		object = g_new0(G3DObject, 1);
		*list = g_slist_append(*list, object);

		/* number of vertices */
		object->vertex_count = g3d_read_int32_le(f);
		object->vertex_data = g_new0(gfloat, 3 * object->vertex_count);

		/* number of primitives */
		num_prims = g3d_read_int32_le(f);

		/* offset of selection primitive */
		g3d_read_int32_le(f);

		/* translation from parent */
		x = g3d_read_int32_le(f);
		y = g3d_read_int32_le(f);
		z = g3d_read_int32_le(f);
		object->transformation = g_new0(G3DTransformation, 1);
		g3d_matrix_identity(object->transformation->matrix);
		g3d_matrix_translate(x, y, z, object->transformation->matrix);

		/* offset of object name */
		off_save = ftell(f) + 4;
		fseek(f, g3d_read_int32_le(f), SEEK_SET);
		g3d_read_cstr(f, buffer, 1024);
		buffer[1024] = '\0';
		object->name = g_strdup(buffer);
		fseek(f, off_save, SEEK_SET);
#if DEBUG > 1
		g_print("TA: object '%s'\n", object->name);
#endif

		/* always 0 */
		g3d_read_int32_le(f);

		/* offset of vertex array */
		off_vert = g3d_read_int32_le(f);
		off_save = ftell(f);
		fseek(f, off_vert, SEEK_SET);
		for(i = 0; i < object->vertex_count; i ++)
		{
			object->vertex_data[i * 3 + 0] = g3d_read_int32_le(f);
			object->vertex_data[i * 3 + 1] = g3d_read_int32_le(f);
			object->vertex_data[i * 3 + 2] = g3d_read_int32_le(f);
		}
		fseek(f, off_save, SEEK_SET);

		/* offset of primitive array */
		off_prim = g3d_read_int32_le(f);
		off_save = ftell(f);
		fseek(f, off_prim, SEEK_SET);
		for(i = 0; i < num_prims; i ++)
		{
			face = g_new0(G3DFace, 1);

			/* color index */
			colidx = g3d_read_int32_le(f);
#if DEBUG > 2
			g_print("TA: color index: %d\n", colidx);
#endif
			if(colidx > 255)
			{
				g_warning("TA: color index > 255 (%d)\n", colidx);
				g_free(face);
				fseek(f, 28, SEEK_CUR);
				continue;
			}
			face->material = &(materials[colidx]);

			/* number of vertices */
			face->vertex_count = g3d_read_int32_le(f);
			if(face->vertex_count < 3)
			{
				/* skip this primitive */
				g_free(face);
				fseek(f, 24, SEEK_CUR);
				continue;
			}
			face->vertex_indices = g_new0(guint32, face->vertex_count);

			/* always 0 */
			g3d_read_int32_le(f);

			/* offset of vertex index array */
			off_i = g3d_read_int32_le(f);
			off_sav2 = ftell(f);
#if DEBUG > 2
			g_print("TA: vertex index offset: 0x%08x\n", off_i);
#endif
			fseek(f, off_i, SEEK_SET);
			for(j = 0; j < face->vertex_count; j ++)
				face->vertex_indices[j] = g3d_read_int16_le(f);
			fseek(f, off_sav2, SEEK_SET);

			/* offset of texture name */
			g3d_read_int32_le(f);

			/* unknown */
			fseek(f, 12, SEEK_CUR);

			object->faces = g_slist_prepend(object->faces, face);
		}
		fseek(f, off_save, SEEK_SET);

		/* offset of sibling object */
		off_sibl = g3d_read_int32_le(f);

		/* offset of child object */
		off_chld = g3d_read_int32_le(f);

#if DEBUG > 3
		g_print("TA: child @ 0x%08x, sibling @ 0x%08x\n", off_chld, off_sibl);
#endif

		if(off_chld != 0)
		{
			off_save = ftell(f);
			fseek(f, off_chld, SEEK_SET);
			ta_3do_read_children(context, f, &(object->objects), materials);
			fseek(f, off_save, SEEK_SET);
		}

		if(off_sibl == 0)
			return TRUE;

		fseek(f, off_sibl, SEEK_SET);
	}

	return FALSE;
}
