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

#ifndef __G3D_IMAGE_H__
#define __G3D_IMAGE_H__

/**
 * SECTION:image
 * @short_description: Image creation and manipulation
 * @include: g3d/image.h
 *
 * Images are most commonly used as textures.
 *
 * All images have 32 bits per pixel in RGBA order. If not needed the
 * alpha channel should be 0xFF.
 */

#include <g3d/types.h>

G_BEGIN_DECLS

/**
 * g3d_image_new:
 *
 * Create a new image structure.
 *
 * At this point it does not have any dimension yet. Set the
 * size using @g3d_image_set_size or @g3d_image_set_pixels.
 *
 * Returns: the new image
 */
G3DImage *g3d_image_new();

/**
 * g3d_image_free:
 * @image: the image to free
 *
 * Frees all memory allocated for the image.
 */
void g3d_image_free(G3DImage *image);

/**
 * g3d_image_get_name:
 * @image: the image
 *
 * Get the name of the image.
 *
 * Returns: the name of the image, may be NULL.
 */
const gchar *g3d_image_get_name(G3DImage *image);

/**
 * g3d_image_set_name:
 * @image: the image
 * @name: new name of the image
 *
 * Set the image name to @name. Memory is allocated for the name,
 * so the given buffer can be freed.
 */
void g3d_image_set_name(G3DImage *image, const gchar *name);

/**
 * g3d_image_set_size:
 * @image: the image
 * @width: new width of image
 * @height: new height of image
 *
 * Set the size of the image. All existing image data is freed at this
 * moment and a new image buffer is allocated. This buffer can be retrieved
 * with @g3d_image_get_pixels to read or write image data.
 */
void g3d_image_set_size(G3DImage *image, guint32 width, guint32 height);

/**
 * g3d_image_get_width:
 * @image: the image
 *
 * Get the width of the image.
 *
 * Returns: width of image
 */
guint32 g3d_image_get_width(G3DImage *image);

/**
 * g3d_image_get_height:
 * @image: the image
 *
 * Get the height of the image.
 *
 * Returns: height of image
 */
guint32 g3d_image_get_height(G3DImage *image);

/**
 * g3d_image_set_pixels:
 * @image: the image
 * @width: new width of image
 * @height: new height of image
 * @pixels: new pixel buffer
 *
 * Sets the new size of the image as @g3d_image_set_size but uses the given
 * pixel buffer instead of allocating a new one. The buffer has to be allocated
 * using the glib memory functions as it may be freed with @g_free.
 */
void g3d_image_set_pixels(G3DImage *image, guint32 width, guint32 height,
	guint8 *pixels);

/**
 * g3d_image_get_pixels:
 * @image: the image
 *
 * Get pixel buffer of image.
 *
 * Returns: the pixel buffer of the image. This may be NULL if
 *          @g3d_image_set_size or @g3d_image_set_pixels have not been
 *          called yet.
 */
guint8 *g3d_image_get_pixels(G3DImage *image);

G_END_DECLS

#endif /* _G3D_IMAGE_H */
