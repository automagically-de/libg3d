#include <g3d/types.h>

G3DImage *g3d_image_new(void)
{
	return g_new0(G3DImage, 1);
}

void g3d_image_free(G3DImage *image)
{
	if(image->name)
		g_free(image->name);
	if(image->pixeldata)
		g_free(image->pixeldata);
	g_free(image);
}

const gchar *g3d_image_get_name(G3DImage *image)
{
	return image->name;
}

void g3d_image_set_name(G3DImage *image, const gchar *name)
{
	if (image->name)
		g_free(image->name);
	image->name = g_strdup(name);
}

void g3d_image_set_size(G3DImage *image, guint32 width, guint32 height)
{
	if (image->pixeldata)
		g_free(image->pixeldata);
	image->depth = 32;
	image->width = width;
	image->height = height;
	image->pixeldata = g_new0(guint8, width * height * 4);
}

guint32 g3d_image_get_width(G3DImage *image)
{
	return image->width;
}

guint32 g3d_image_get_height(G3DImage *image)
{
	return image->height;
}

void g3d_image_set_pixels(G3DImage *image, guint32 width, guint32 height,
	guint8 *pixels)
{
	if (image->pixeldata)
		g_free(image->pixeldata);
	image->depth = 32;
	image->width = width;
	image->height = height;
	image->pixeldata = pixels;
}

guint8 *g3d_image_get_pixels(G3DImage *image)
{
	return image->pixeldata;
}
