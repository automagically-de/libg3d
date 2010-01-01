/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2009  Markus Dahms <mad@automagically.de>

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
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/vector.h>
#include <g3d/material.h>
#include <g3d/texture.h>

/* plugin for Warzone 2100 .pie models
 * http://developer.wz2100.net/wiki/PIE_format
 */

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar *buffer, *stmp, **strv;
	guint32 u1, u2, u3, off;
	gint32 i, j, v[3];
	G3DObject *object;
	G3DFace *face;
	G3DMaterial *material;

	setlocale(LC_NUMERIC, "C");

	buffer = g_new0(gchar, 2048 + 1);

	g3d_stream_read_line(stream, buffer, 2048);
	if(strncmp(buffer, "PIE ", 4) != 0) {
		g_warning("file is not a PIE model");
		g_free(buffer);
		return FALSE;
	}

	material = g3d_material_new();
	material->name = g_strdup("default material");

	object = g_new0(G3DObject, 1);
	object->name = g_strdup("default object");
	object->materials = g_slist_append(object->materials, material);

	model->objects = g_slist_append(model->objects, object);

	while(!g3d_stream_eof(stream)) {
		g3d_stream_read_line(stream, buffer, 2048);

		if(sscanf(buffer, "LEVEL %u", &u1) == 1) {
			if(u1 > 1) {
				g_debug("PIE: only load first model");
				break;
			}
		} else if(sscanf(buffer, "POINTS %u", &u1) == 1) {
			object->vertex_count = u1;
			object->vertex_data = g3d_vector_new(3, u1);
			for(i = 0; i < u1; i ++) {
				g3d_stream_read_line(stream, buffer, 2048);
				if(sscanf(buffer, "%d %d %d", v, v + 1, v + 2) == 3) {
					object->vertex_data[i * 3 + 0] = v[0];
					object->vertex_data[i * 3 + 1] = v[1];
					object->vertex_data[i * 3 + 2] = v[2];
				}
			}
		} else if(sscanf(buffer, "POLYGONS %u", &u1) == 1) {
			for(i = 0; i < u1; i ++) {
				g3d_stream_read_line(stream, buffer, 2048);
				strv = g_strsplit(buffer + 1, " ", 0);

				u2 = strtoul(strv[0], NULL, 16); /* flags */
				u3 = atoi(strv[1]); /* # indices */

				off = u3 + 2;
				if(u2 & 0x4000) {
					off += 4;
				}

				if(u3 >= 3) {
					face = g_new0(G3DFace, 1);
					face->vertex_count = u3;
					face->vertex_indices = g_new0(guint32, u3);
					face->material = material;
					object->faces = g_slist_append(object->faces, face);

					for(j = 0; j < u3; j ++) {
						face->vertex_indices[j] = atoi(strv[j + 2]);
					}

					if(material->tex_image != NULL) {
						face->flags |= G3D_FLAG_FAC_TEXMAP;
						face->tex_image = material->tex_image;
						face->tex_vertex_count = u3;
						face->tex_vertex_data = g3d_vector_new(2, u3);
						for(j = 0; j < u3; j ++) {
							face->tex_vertex_data[j * 2 + 0] =
								(G3DVector)atoi(strv[j * 2 + off + 0])
								/ 256.0;
							face->tex_vertex_data[j * 2 + 1] =
								(G3DVector)atoi(strv[j * 2 + off + 1])
								/ 256.0;
						}
					}
				}
				g_strfreev(strv);
			} /* POLYGONS lines 0..n */
		} else if(strncmp(buffer, "TEXTURE 0 ", 10) == 0) {
			stmp = g_strdup_printf("../../texpages/%.*s",
				(gint)(strchr(buffer + 10, ' ') - (buffer + 10)),
				buffer + 10);
			material->tex_image = g3d_texture_load_cached(
				context, model, stmp);
			g_free(stmp);
		} else {
			g_warning("PIE: unhandled line: %s", buffer);
		}
	}

	g_free(buffer);

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Warzone 2100 .pie files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("pie", ":", 0);
}

/*****************************************************************************/
