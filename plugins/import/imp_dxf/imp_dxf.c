/* $Id$ */

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
#include <string.h>
#include <locale.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/material.h>

#include "imp_dxf.h"
#include "imp_dxf_section.h"

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar binmagic[22];
	G3DObject *object;
	G3DMaterial *material;
	DxfGlobalData *global;

	global = g_new0(DxfGlobalData, 1);
	global->context = context;
	global->model = model;
	global->stream = stream;
	global->line = 1;

	setlocale(LC_NUMERIC, "C");

	if((g3d_stream_read(stream, binmagic, 1, 22) == 22) &&
		 (strncmp(binmagic, "AutoCAD Binary DXF", 18) == 0))
		global->binary = TRUE;
	else {
		setlocale(LC_NUMERIC, "C");
		g3d_stream_seek(stream, 0, G_SEEK_SET);
	}

	object = g_new0(G3DObject, 1);
	object->name = g_strdup("DXF Object");
	model->objects = g_slist_append(model->objects, object);

	material = g3d_material_new();
	material->name = g_strdup("default material");
	material->flags |= G3D_FLAG_MAT_TWOSIDE;
	object->materials = g_slist_append(object->materials, material);

	while(!g3d_stream_eof(stream)) {
		int retval = dxf_read_section(global, object);
		if(retval != TRUE) {
			if(retval == 0xE0F)
				return TRUE;
			g_printerr("error in section..\n");
			return FALSE;
		}
	}
	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for AutoCAD DXF files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("dxf", ":", 0);
}

/*****************************************************************************/

gboolean dxf_read_section(DxfGlobalData *global, G3DObject *object)
{
	gint grpcode;
	G3DFace *face = NULL;
	gchar val_str[DXF_MAX_LINE + 1];
	gint32 key;
	gchar str[DXF_MAX_LINE + 1];
	gdouble val_f64;

	grpcode = dxf_read_code(global);
	if(grpcode != 0) {
#if DEBUG > 0
		g_printerr("unexpected group code: %d (0 expected)\n", grpcode);
#endif
		return FALSE;
	}
	dxf_read_string(global, val_str);
	if(strcmp("EOF", val_str) == 0)
		return 0xE0F;
	if(strcmp("SECTION", val_str) != 0) {
#if DEBUG > 0
		g_printerr("SECTION expected, found: %s\n", val_str);
#endif
		return FALSE;
	}
	grpcode = dxf_read_code(global);
	if(grpcode != 2) {
#if DEBUG > 0
		g_printerr("unexpected group code: %d (2 expected)\n", grpcode);
#endif
		return FALSE;
	}
	dxf_read_string(global, val_str);

	if(strcmp(val_str, "HEADER") == 0)
		return dxf_section_HEADER(global);
	else if(strcmp(val_str, "TABLES") == 0)
		return dxf_section_TABLES(global);

	if(
		 (strcmp(val_str, "CLASSES") == 0) ||
		 (strcmp(val_str, "BLOCKS") == 0) ||
		 (strcmp(val_str, "OBJECTS") == 0)) {
#if DEBUG > 0
		g_printerr("skipping section: %s\n", val_str);
#endif
		dxf_skip_section(global);
	} else if(strcmp(val_str, "ENTITIES") == 0) {
#if DEBUG > 0
		g_printerr("processing entities section...\n");
#endif
		while(1) {
			key = dxf_read_code(global);
			switch(key)
			{
				case DXF_CODE_INVALID:
					return 0xE0F;
					break;
				case 0:
					face = NULL;
					dxf_read_string(global, str);
					DXF_TEST_ENDSEC(str);
					if(strcmp("3DFACE", str) == 0)
					{
						int nfaces, i;

						face = g_new0(G3DFace, 1);
						object->faces = g_slist_prepend(object->faces, face);
						nfaces = g_slist_length(object->faces);
#if DEBUG > 2
						g_printerr("creating face... (#%d)\n", nfaces);
#endif
						object->vertex_count = nfaces * 4;
						object->vertex_data = g_realloc(object->vertex_data,
							object->vertex_count * 3 * sizeof(gfloat));

						face->vertex_count = 4;
						face->vertex_indices = g_new0(guint32, 4);
						for(i = 0; i < 4; i ++)
						{
							face->vertex_indices[i] = (nfaces-1) * 4 + i;
							object->vertex_data[
								face->vertex_indices[i] * 3 + 0] = 0.0;
							object->vertex_data[
								face->vertex_indices[i] * 3 + 1] = 0.0;
							object->vertex_data[
								face->vertex_indices[i] * 3 + 2] = 0.0;
						}
						face->material =
							g_slist_nth_data(object->materials, 0);
					}
					break;

				case 8:
					dxf_read_string(global, str);
					break;

				case 10: case 20: case 30:
				case 11: case 21: case 31:
				case 12: case 22: case 32:
				case 13: case 23: case 33:
					val_f64 = dxf_read_float64(global);
					if(face != NULL)
					{
						object->vertex_data[
							face->vertex_indices[key % 10] * 3 +
							(key / 10) - 1] =
							(gfloat)val_f64;
					}
					break;

				case 62:
				case 67:
				case 68:
				case 69:
				case 70:
				case 1070:
					/* 16-bit integer */
					if(global->binary)
						g3d_stream_read_int16_le(global->stream);
					else {
						g3d_stream_read_line(global->stream, str,
                            DXF_MAX_LINE);
						global->line ++;
					}
					break;

				case 40:
				case 41:
				case 50:
				case 210: case 220: case 230:
				case 1040:
					/* double */
					dxf_read_float64(global);
					break;

				case 1002:
					/* string */
					dxf_read_string(global, str);
					break;

				default:
					DXF_HANDLE_UNKNOWN(global, key, str, "** GLOBAL **");
					break;
			}
		}
	}
	else
	{
#if DEBUG > 0
		g_printerr("unknown section '%s', skipping...\n", val_str);
#endif
		dxf_skip_section(global);
	}
	return TRUE;
}

gchar *dxf_read_string(DxfGlobalData *global, gchar *value)
{
	if(global->binary) {
		int pos = 0;
		int c;
		do
		{
			c = g3d_stream_read_int8(global->stream);
			value[pos] = (char)c;
			pos++;
		} while(c != '\0');
		return value;
	} else {
		gchar line[DXF_MAX_LINE + 1];

		g3d_stream_read_line(global->stream, line, DXF_MAX_LINE);
		global->line ++;
		line[DXF_MAX_LINE] = '\0';
		if(sscanf(line, "%s", value) == 1)
			return g_strchomp(value);
		if(sscanf(line, " %s", value) == 1)
			return g_strchomp(value);
		return NULL;
	}
}

gint32 dxf_read_code(DxfGlobalData *global)
{
	gint32 val = DXF_CODE_INVALID;
	gchar line[DXF_MAX_LINE + 1];

	if(global->binary)
		return g3d_stream_read_int8(global->stream);
	else {
		g3d_stream_read_line(global->stream, line, DXF_MAX_LINE);
		global->line ++;
		if((sscanf(line, "%d", &val) != 1) &&
			(sscanf(line, " %d", &val) != 1))
			return DXF_CODE_INVALID;
		if(val == 999) { /* comment */
			g3d_stream_read_line(global->stream, line, DXF_MAX_LINE);
			global->line ++;
			return dxf_read_code(global);
		}
		return val;
	}
}

gint32 dxf_read_int16(DxfGlobalData *global)
{
	if(global->binary)
		return g3d_stream_read_int16_le(global->stream);
	else
	{
		gint32 val;
		gchar line[DXF_MAX_LINE];

		g3d_stream_read_line(global->stream, line, DXF_MAX_LINE);
		global->line ++;
		if(sscanf(line, "%i", &val) == 1)
			return val;
		if(sscanf(line, " %i", &val) == 1)
			return val;
		else
			return DXF_CODE_INVALID;
	}
}

gdouble dxf_read_float64(DxfGlobalData *global)
{
	if(global->binary)
		return g3d_stream_read_double_le(global->stream);
	else
	{
		gdouble val;
		gchar line[DXF_MAX_LINE];

		g3d_stream_read_line(global->stream, line, DXF_MAX_LINE);
		global->line ++;
		if(sscanf(line, "%lf", &val) == 1)
			return val;
		if(sscanf(line, " %lf", &val) == 1)
			return val;
		else
			return 0.0;
	}
}

gboolean dxf_skip_section(DxfGlobalData *global)
{
	gchar c, buf[7], line[DXF_MAX_LINE];
	gsize read;

	while(!g3d_stream_eof(global->stream))
		if(global->binary) {
			do { c = g3d_stream_read_int8(global->stream); } while(c != 0);
			read = g3d_stream_read(global->stream, buf, 1, 7);
			if((read == 7) && (strncmp(buf, "ENDSEC", 6) == 0))
				return TRUE;
			else
				g3d_stream_seek(global->stream, -read, G_SEEK_CUR);
		} else {
			g3d_stream_read_line(global->stream, line, DXF_MAX_LINE);
			global->line ++;
			if(strncmp(line, "ENDSEC", 6) == 0)
				return TRUE;
		}
	return TRUE;
}

