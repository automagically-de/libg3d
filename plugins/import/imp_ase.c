/* $Id:$ */

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
#include <stdarg.h>

#include <g3d/types.h>

gboolean plugin_load_model(G3DContext *context, const gchar *filename,
	G3DModel *model)
{
	setlocale(LC_NUMERIC, "C");

	return TRUE;
}

char *plugin_description(void)
{
	return g_strdup("import plugin for ASCII Scene Exporter (ASE) files\n");
}

char **plugin_extensions(void)
{
	return g_strsplit("ase", ":", 0);
}

/*
 * ASE specific
 */

static gboolean ase_parse(const gchar *line, guint32 num,
	const gchar *format, ...)
{
	va_list vl;
	guint32 num_parsed;

	va_start(vl, format);
	num_parsed = vsscanf(line, format, vl);
	va_end(vl);

	if(num_parsed == num)
		return TRUE;
	else
	{
		g_warning("ASE: failed to parse line using format '%s':\n  %s\n",
			format, line);
		return FALSE;
	}
}
