#include <string.h>

#include <g3d/object.h>

#include "imp_ldraw_types.h"
#include "imp_ldraw_part.h"
#include "imp_ldraw_color.h"

static gboolean ldraw_library_add_dir(LDrawLibrary *lib, const gchar *subdir)
{
	LDrawPart *part;
	const gchar *filename;
	gchar *partdir;
	GDir *dir;
	GError *error;
	gboolean prefix_s = FALSE;

	partdir = g_strdup_printf("%s%c%s", lib->libdir, G_DIR_SEPARATOR, subdir);
	dir = g_dir_open(partdir, 0, &error);
	if(dir == NULL) {
		g_warning("LDraw: failed to open directory '%s': %s", partdir,
			error->message);
		g_error_free(error);
		g_free(partdir);
		return FALSE;
	}

	if(strchr(subdir, G_DIR_SEPARATOR))
		prefix_s = TRUE;

	filename = g_dir_read_name(dir);
	while(filename) {
		if(g_ascii_strcasecmp(filename + strlen(filename) - 4, ".dat") == 0) {
#if DEBUG > 3
			g_debug("LDraw: Library: adding '%s'", filename);
#endif
			part = g_new0(LDrawPart, 1);
			if(prefix_s) {
				part->name = g_strdup_printf("S%c%s",
					G_DIR_SEPARATOR, filename);
				part->subdir = g_path_get_dirname(subdir);
			} else {
				part->name = g_strdup(filename);
				part->subdir = g_strdup(subdir);
			}
			g_hash_table_insert(lib->partdb, part->name, part);
			lib->partlist = g_slist_append(lib->partlist, part);
		}
		filename = g_dir_read_name(dir);
	}
	g_dir_close(dir);

	return TRUE;
}

LDrawLibrary *ldraw_library_init(void)
{
	LDrawLibrary *lib;
	const gchar *lddir;

	lib = g_new0(LDrawLibrary, 1);
	lib->partdb = g_hash_table_new(g_str_hash, g_str_equal);

	ldraw_color_init(lib);

	lddir = g_getenv("LDRAWDIR");
	if(lddir == NULL) /* warning is issued when trying to load a model */
		return lib;

	lib->libdir = g_strdup(lddir);

	ldraw_library_add_dir(lib, "PARTS");
	ldraw_library_add_dir(lib, "PARTS" G_DIR_SEPARATOR_S "S");
	ldraw_library_add_dir(lib, "P");

	return lib;
}

void ldraw_library_cleanup(LDrawLibrary *lib)
{
	GSList *item;
	LDrawPart *part;

	item = lib->partlist;
	while(item != NULL) {
		part = item->data;
		item = g_slist_remove(item, part);
		ldraw_part_free(part);
	}
	g_hash_table_destroy(lib->partdb);
	g_free(lib);
}

void ldraw_library_insert(LDrawLibrary *lib, gchar *name, gpointer data)
{
	g_hash_table_insert(lib->partdb, name, data);
}

G3DObject *ldraw_library_lookup(LDrawLibrary *lib, const gchar *name)
{
	LDrawPart *part;
	gchar *filename;

	part = g_hash_table_lookup(lib->partdb, name);
	if(part == NULL) {
		filename = g_ascii_strdown(name, -1);
		part = g_hash_table_lookup(lib->partdb, filename);
		g_free(filename);
	}
	if(part == NULL) {
		filename = g_ascii_strup(name, -1);
		part = g_hash_table_lookup(lib->partdb, filename);
		g_free(filename);
	}
	if(part == NULL) {
		g_warning("LDraw: failed to find '%s' in library", name);
		return NULL;
	}
	if(part->object == NULL) {
		if(part->stream) {
			/* MPD loader has a custom stream */
			part->object = ldraw_part_get_object(part, lib);
		} else {
			/* try to load part from standard library */
			filename = g_strdup_printf("%s%c%s%c%s", lib->libdir,
				G_DIR_SEPARATOR, part->subdir, G_DIR_SEPARATOR, part->name);
			part->stream = g3d_stream_open_file(filename, "r");
			if(part->stream) {
				part->object = ldraw_part_get_object(part, lib);
				g3d_stream_close(part->stream);
				part->stream = NULL;
			} else {
				g_warning("LDraw: failed to open stream for '%s'", filename);
			}
			g_free(filename);
		}

		if(part->object == NULL) {
			g_warning("LDraw: failed to load part '%s'", part->name);
			return NULL;
		}
	}
	return g3d_object_duplicate(part->object);
}

