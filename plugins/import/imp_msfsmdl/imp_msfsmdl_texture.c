#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <g3d/texture.h>

static G3DImage* find_load_texture_from_path(G3DContext *context, G3DModel *model,
	const gchar *filename,
	const gchar *pathname) {

	const gchar *name;
	G3DImage *image = NULL;
	GDir *dir = g_dir_open(pathname, 0, NULL);

	if (!dir)
		return NULL;

	for (name = g_dir_read_name(dir); name != NULL; name = g_dir_read_name(dir)) {
		if (g_ascii_strcasecmp(name, filename) == 0) {
			gchar *path = g_strdup_printf("%s/%s", pathname, name);
			image = g3d_texture_load_cached(context, model, path);
			if (!image)
				g_warning("failed to load texture %s", path);
			g_free(path);
			break;
		}
	}

	g_dir_close(dir);
	return image;
}

G3DImage *msfsmdl_find_load_texture(G3DContext *context, G3DModel *model, const gchar *filename,
	gboolean in_fallback) {

	G3DImage *image = NULL;

	image = find_load_texture_from_path(context, model, filename, "../texture");

	if (!image) {
		GDir *dir = g_dir_open("..", 0, NULL);
		if (dir) {
			const gchar *name;
			for (name = g_dir_read_name(dir); name != NULL; name = g_dir_read_name(dir)) {
				gchar *texture_dir;
				if (g_ascii_strncasecmp(name, "texture.", 8) != 0)
					continue;
				texture_dir = g_strdup_printf("../%s", name);
#if DEBUG > 1
				g_debug("| alternative texture path: %s", texture_dir);
#endif
				image = find_load_texture_from_path(context, model, filename, texture_dir);
				g_free(texture_dir);
				if (image)
					break;
			}
			g_dir_close(dir);
		}
	}

	if (!image) {
		guint32 extoff = strlen(filename) - 4;
#if 0
		g_warning("failed to find texture file %s", filename);
#endif

		if (!in_fallback) {
			if (g_ascii_strcasecmp(filename + extoff, ".bmp") == 0) {
				gchar *ddsname = g_strdup(filename);
				memcpy(ddsname + extoff, ".dds", 4);
				image = msfsmdl_find_load_texture(context, model, ddsname, TRUE);
				g_free(ddsname);
			}
			if (g_ascii_strcasecmp(filename + extoff, ".dds") == 0) {
				gchar *bmpname = g_strdup(filename);
				memcpy(bmpname + extoff, ".bmp", 4);
				image = msfsmdl_find_load_texture(context, model, bmpname, TRUE);
				g_free(bmpname);
			}
		}
		else {
			g_warning("failed to load %s", filename);
		}
	}

	return image;
}

