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
#include <string.h>
#include <g3d/types.h>
#include <g3d/plugins.h>
#include <g3d/stream.h>
#include <g3d/texture.h>
#include <g3d/image.h>

#ifdef G3D_DEBUG_DUMP_IMAGE
static gboolean dump_ppm(G3DImage *image, const gchar *filename);
#endif

G3DImage *g3d_texture_load(G3DContext *context, const gchar *filename)
{
	G3DImage *image;
	gchar *basename, *path, *casedup, *caseddown, *realfile = NULL;

	/* convert DOS path separator */
	path = g_strdup(filename);
	g_strdelimit(path, "\\", '/');

	if(g_file_test(path, G_FILE_TEST_EXISTS))
	{
		realfile = g_strdup(path);
	}
	else
	{
		basename = g_path_get_basename(path);
		if(g_file_test(basename, G_FILE_TEST_EXISTS))
		{
			realfile = g_strdup(basename);
		}
		else
		{
			casedup = g_ascii_strup(basename, -1);
			if(g_file_test(casedup, G_FILE_TEST_EXISTS))
			{
				realfile = g_strdup(casedup);
			}
			else
			{
				caseddown = g_ascii_strdown(basename, -1);
				if(g_file_test(caseddown, G_FILE_TEST_EXISTS))
				{
					realfile = g_strdup(caseddown);
				}
				g_free(caseddown);
			}
			g_free(casedup);
		}
		g_free(basename);
	}
	g_free(path);

	if(realfile == NULL)
	{
		g_warning("failed to find a file matching '%s'", filename);
		return NULL;
	}

	/* create emtpy G3DImage */
	image = g_new0(G3DImage, 1);
	image->tex_scale_u = 1.0;
	image->tex_scale_v = 1.0;

	if(g3d_plugins_load_image(context, realfile, image))
	{
		g_free(realfile);
		return image;
	}

	g_free(image);
	g_free(realfile);

	return NULL;
}

G3DImage *g3d_texture_load_from_stream(G3DContext *context, G3DModel *model,
	G3DStream *stream)
{
	G3DImage *image;

	if(model != NULL) {
		if(model->tex_images == NULL)
			model->tex_images = g_hash_table_new(g_str_hash, g_str_equal);

		image = g_hash_table_lookup(model->tex_images, stream->uri);
		if(image != NULL)
			return image;
	}

	image = g_new0(G3DImage, 1);
	image->tex_scale_u = 1.0;
	image->tex_scale_v = 1.0;

	if(g3d_plugins_load_image_from_stream(context, stream, image)) {
		image->tex_id = g_str_hash(stream->uri);
		if(model != NULL) {
			g_hash_table_insert(model->tex_images, g_strdup(stream->uri),
				image);
#if DEBUG > 0
			g_debug("adding texture '%s' to cache", stream->uri);
#endif
		}
		return image;
	}
	g_free(image);
	return NULL;
}

G3DImage *g3d_texture_load_cached(G3DContext *context, G3DModel *model,
	const gchar *filename)
{
	G3DImage *image;
#ifdef G3D_DEBUG_DUMP_IMAGE
	gchar *basename, *ppmname;
#endif

	/* create hash table if it does not exist yet */
	if(model->tex_images == NULL)
		model->tex_images = g_hash_table_new(g_str_hash, g_str_equal);

	/* if already loaded, return cached image */
	image = g_hash_table_lookup(model->tex_images, filename);
	if(image != NULL)
		return image;

	image = g3d_texture_load(context, filename);
	if(image != NULL)
	{
		image->tex_id = g_str_hash(filename);
#if 0
		g3d_texture_prepare(image);
#endif
		g_hash_table_insert(model->tex_images, (gpointer)g_strdup(filename),
			image);
	}

#ifdef G3D_DEBUG_DUMP_IMAGE
	if(image)
	{
		basename = g_path_get_basename(filename);
		ppmname = g_strdup_printf("/tmp/%s.ppm", basename);
		dump_ppm(image, ppmname);
		g_free(ppmname);
		g_free(basename);
	}
#endif

	return image;
}

gboolean g3d_texture_cache_remove(G3DModel *model, G3DImage *image)
{
	const gchar *name;

	name = g3d_image_get_name(image);
	g_return_val_if_fail(model->tex_images != NULL, FALSE);
	g_return_val_if_fail(name != NULL, FALSE);

	return g_hash_table_remove(model->tex_images, name);
}

void g3d_texture_free(G3DImage *texture)
{
	g3d_image_free(texture);
}

gboolean g3d_texture_prepare(G3DImage *texture)
{
	guint32 nw = 1, nh = 1, y;
	guint32 width, height;
	guint8 *np, *pixeldata;

	width = g3d_image_get_width(texture);
	height = g3d_image_get_height(texture);
	pixeldata = g3d_image_get_pixels(texture);

	while(nw < width) nw *= 2;
	while(nh < height) nh *= 2;

	if((nw != width) || (nh != height))
	{
		/* blow up texture image to dimensions with a power of two */
		np = g_malloc(nw * nh * 4);
		memset(np, 0xFF, nw * nh * 4);

		/* copy image data */
		for(y = 0; y < nh; y ++)
			memcpy(np + ((nh - y - 1) * nw * 4),
				pixeldata +
					(((height - y - 1) % height) *
					width * 4),
				width * 4);

		/* calculate scaling factor */
		texture->tex_scale_u = ((G3DFloat)width / (G3DFloat)nw);
		texture->tex_scale_v = ((G3DFloat)height / (G3DFloat)nh);

#if DEBUG > 0
		g_debug("texture scaling factor for '%s' set to %.2f,%.2f",
			g3d_image_get_name(texture),
			texture->tex_scale_u, texture->tex_scale_v);
#endif

		/* update image */
		g3d_image_set_pixels(texture, nw, nh, np);

		return TRUE;
	}
	return FALSE;
}

gboolean g3d_texture_flip_y(G3DImage *texture)
{
	guint32 width, height;
	guint8 *newpixel, *pixeldata;
	gint32 y;

	g_return_val_if_fail(texture != NULL, FALSE);

	width = g3d_image_get_width(texture);
	height = g3d_image_get_height(texture);
	pixeldata = g3d_image_get_pixels(texture);

	newpixel = g_new0(guint8, width * height * 4);

	for(y = 0; y < height; y ++)
	{
		memcpy(
			newpixel + (y * width * 4),
			pixeldata + ((height - y - 1) * width * 4),
			width * 4);
	}

	g3d_image_set_pixels(texture, width, height, newpixel);

	return TRUE;
}

#ifdef G3D_DEBUG_DUMP_IMAGE
static gboolean dump_ppm(G3DImage *image, const gchar *filename)
{
	FILE *f;
	guint32 x, y;
	guint32 width, height;
	guint8 *pixeldata;

	f = fopen(filename, "w");
	if(f == NULL)
	{
		g_warning("image: failed to write to '%s'", filename);
		return FALSE;
	}

	width = g3d_image_get_width(image);
	height = g3d_image_get_height(image);
	pixeldata = g3d_image_get_pixels(image);

	fprintf(f, "P3\n# CREATOR: g3dviewer\n%d %d\n%d\n",
		width, height, 255);

	for(y = 0; y < height; y ++)
		for(x = 0; x < width; x ++)
			fprintf(f, "%d\n%d\n%d\n",
				pixeldata[(y * width + x) * 4 + 0],
				pixeldata[(y * width + x) * 4 + 1],
				pixeldata[(y * width + x) * 4 + 2]);

	fclose(f);
	return TRUE;
}
#endif

G3DImage *g3d_texture_merge_alpha(G3DImage *image, G3DImage *aimage)
{
	G3DImage *texture;
	gint32 x, y;
	gboolean negative;
	guint32 width, height;
	guint8 *apixeldata, *tpixeldata;

	g_return_val_if_fail(aimage != NULL, NULL);

	width = g3d_image_get_width(aimage);
	height = g3d_image_get_height(aimage);

	if(image && (
		(g3d_image_get_width(image) != width) ||
		(g3d_image_get_height(image) != height)))
	{
		/* size doesn't match, don't do something */
		return image;
	}

	if(image)
	{
		texture = image;
	}
	else
	{
		texture = g3d_image_new();
		g3d_image_set_size(texture, width, height);
		texture->tex_scale_u = 1.0;
		texture->tex_scale_v = 1.0;
	}

	apixeldata = g3d_image_get_pixels(aimage);
	tpixeldata = g3d_image_get_pixels(texture);

	/* negative map? */
	/* FIXME: better solution? */
	if(apixeldata[0] == 0)
		negative = TRUE;
	else
		negative = FALSE;

	for(y = 0; y < height; y ++)
	{
		for(x = 0; x < width; x ++)
		{
			tpixeldata[(y * width + x) * 4 + 3] = (negative ?
				255 - apixeldata[(y * width + x) * 4 + 0] :
				apixeldata[(y * width + x) * 4 + 0]);
		}
	}

	return texture;
}
