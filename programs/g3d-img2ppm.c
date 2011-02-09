/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2011  Markus Dahms <mad@automagically.de>

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
#include <signal.h>
#include <g3d/g3d.h>
#include <g3d/texture.h>

static void _g_print(const gchar *str)
{
	fputs(str, stderr);
}

static void log_handler(const gchar *log_domain, GLogLevelFlags log_level,
	const gchar *message, gpointer user_data)
{
	fputs(message, stderr);
	fputc('\n', stderr);
}

static void show_help(const char *pname)
{
	fprintf(stderr,
		"usage: %s <filename>\n"
		"\n"
		"The image in PPM format is written to STDOUT.\n",
		pname
	);
}

int main(int argc, char *argv[])
{
	G3DContext *context;
	G3DImage *image;
	gint x, y;
	
	if (argc < 2) {
		show_help(argv[0]);
		return EXIT_FAILURE;
	}

	g_set_print_handler(_g_print);
	g_set_printerr_handler(_g_print);
	g_log_set_handler("LibG3D", G_LOG_LEVEL_MASK, log_handler, NULL);

	context = g3d_context_new();
	image = g3d_texture_load(context, argv[1]);
	if (!image) {
		fprintf(stderr, "%s: failed to load image '%s'\n", argv[0], argv[1]);
		g3d_context_free(context);
		return EXIT_FAILURE;
	}
	printf("P6\n%d %d\n255\n",
		image->width,
		image->height);

	for (y = 0; y < image->height; y ++) {
		for (x = 0; x < image->width; x ++) {
			fwrite(image->pixeldata + (y * image->width + x) * 4, 1, 3, stdout);
		}
	}

	g3d_texture_free(image);
	g3d_context_free(context);
	return EXIT_SUCCESS;
}
