#ifndef _G3D_IMAGE_H
#define _G3D_IMAGE_H

#include <g3d/types.h>

G3DImage *g3d_image_new();
void g3d_image_free(G3DImage *image);
const gchar *g3d_image_get_name(G3DImage *image);
void g3d_image_set_name(G3DImage *image, const gchar *name);
void g3d_image_set_size(G3DImage *image, guint32 width, guint32 height);
guint32 g3d_image_get_width(G3DImage *image);
guint32 g3d_image_get_height(G3DImage *image);
void g3d_image_set_pixels(G3DImage *image, guint32 width, guint32 height,
	guint8 *pixels);
guint8 *g3d_image_get_pixels(G3DImage *image);

#endif /* _G3D_IMAGE_H */
