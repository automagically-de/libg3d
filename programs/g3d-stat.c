#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <g3d/g3d.h>
#include <g3d/plugins.h>

typedef struct {
	guint32 n_objects;
	guint32 n_vertices;
	guint32 n_faces;
	guint32 n_textures;
} FileStats;

typedef struct {
	guint32 n_success;
	guint32 n_failure;
	GSList *no_objects;
	GSList *no_faces;
	GSList *no_vertices;
} PluginStats;

static gboolean quit = FALSE;

static gboolean dir_stat(G3DContext *context, const char *dirname);
static gboolean file_stat(G3DContext *context, const char *filename,
	gboolean detailed, FileStats *stats, gchar **plugin_used);

static void sighandler(int signal)
{
	quit = TRUE;
	fprintf(stderr, "signal %d received, quitting\n", signal);
}

static void _g_print(const gchar *str)
{
#if 0
	fputs(str, stderr);
#endif
}

#define ANSI_RESET "\033[0m"
#define ANSI_GREEN "\033[32m"
#define ANSI_RED   "\033[31m"

static void log_handler(const gchar *log_domain, GLogLevelFlags log_level,
	const gchar *message, gpointer user_data)
{
	if(message[0] == '\\') {
		printf(ANSI_GREEN "%s\n" ANSI_RESET, message + 1);
	} else if(message[0] == '|') {
		printf("%s\n", message + 1);
	} else {
		printf(ANSI_RED "%s\n" ANSI_RESET, message);
	}
}

int main(int argc, char **argv)
{
	G3DContext *context;

	if(argc < 2) {
		g_printerr("usage: %s (<file>|<directory>)\n", argv[0]);
		return EXIT_FAILURE;
	}

	signal(SIGINT, sighandler);
	signal(SIGQUIT, sighandler);

	g_set_print_handler(_g_print);
	g_set_printerr_handler(_g_print);
	g_log_set_handler("LibG3D", G_LOG_LEVEL_MASK, log_handler, NULL);

	context = g3d_context_new();

	if(g_file_test(argv[1], G_FILE_TEST_IS_DIR)) {
		return dir_stat(context, argv[1]);
	}
	else {
		return file_stat(context, argv[1], TRUE, NULL, NULL);
	}
}

static gboolean dir_stat_1(G3DContext *context, const char *dirname,
	GHashTable *plugins)
{
	GDir *dir;
	FileStats *fstats;
	PluginStats *pstats;
	const gchar *fname;
	gchar *filename, *plugin_used;
	gboolean retval;

	dir = g_dir_open(dirname, 0, NULL);
	g_return_val_if_fail(dir != NULL, FALSE);
	while((fname = g_dir_read_name(dir)) != NULL) {
		filename = g_strdup_printf("%s%c%s",
			dirname, G_DIR_SEPARATOR, fname);
		if(quit)
			return FALSE;
		if(g_file_test(filename, G_FILE_TEST_IS_DIR))
			dir_stat_1(context, filename, plugins);
		else {
			fstats = g_new0(FileStats, 1);
			plugin_used = NULL;
			fprintf(stderr, "*** DEBUG: %s\n", filename);
			retval = file_stat(context, filename, FALSE, fstats, &plugin_used);
			if(plugin_used) {
				pstats = g_hash_table_lookup(plugins, plugin_used);
				if(pstats)
					g_free(plugin_used);
				else {
					pstats = g_new0(PluginStats, 1);
					g_hash_table_insert(plugins, plugin_used, pstats);
				}

				/* update pstats for plugin */
				if(retval)
					pstats->n_success ++;
				else
					pstats->n_failure ++;

				if(fstats->n_objects == 0)
					pstats->no_objects = g_slist_append(pstats->no_objects,
						g_strdup(filename));
				else if(fstats->n_vertices == 0)
					pstats->no_vertices = g_slist_append(pstats->no_vertices,
						g_strdup(filename));
				else if(fstats->n_faces == 0)
					pstats->no_faces = g_slist_append(pstats->no_faces,
						g_strdup(filename));
			} /* plugin_used */
			g_free(fstats);
		} /* !directory */
		g_free(filename);
	}
	g_dir_close(dir);
	return TRUE;
}

static void output_plugin_stats(gpointer key, gpointer value, gpointer data)
{
	gchar *name = (gchar *)key;
	PluginStats *pstats = (PluginStats *)value;
	GSList *item;

	printf("%s:\n"
		"  num success: %d\n"
		"  num failure: %d\n"
		"  num no objects: %d\n",
		name, pstats->n_success, pstats->n_failure,
		g_slist_length(pstats->no_objects));
	for(item = pstats->no_objects; item != NULL; item = item->next)
		printf("    %s\n", (gchar *)item->data);
	printf("  num no vertices: %d\n", g_slist_length(pstats->no_vertices));
	for(item = pstats->no_vertices; item != NULL; item = item->next)
		printf("    %s\n", (gchar *)item->data);
	printf("  num no faces: %d\n", g_slist_length(pstats->no_faces));
	for(item = pstats->no_faces; item != NULL; item = item->next)
		printf("    %s\n", (gchar *)item->data);
}

static gboolean dir_stat(G3DContext *context, const char *dirname)
{
	GHashTable *plugins;

	plugins = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	dir_stat_1(context, dirname, plugins);

	/* output statistics */
	g_hash_table_foreach(plugins, output_plugin_stats, NULL);

	g_hash_table_destroy(plugins);

	return TRUE;
}

static gboolean objects_stat(GSList *objects, gboolean detailed,
	FileStats *stats, guint32 indent)
{
	GSList *item;
	G3DObject *object;
	gchar *istr;

	if(stats)
		stats->n_objects += g_slist_length(objects);

	for(item = objects; item != NULL; item = item->next) {
		object = (G3DObject *)item->data;
		g_return_val_if_fail(object != NULL, FALSE);

		if(stats) {
			stats->n_vertices += object->vertex_count;
			stats->n_faces += g_slist_length(object->faces);
		}

		if(detailed) {
			istr = g_strnfill(indent * 2, ' ');
			printf("%sobject name: %s\n"
				"%sobject num vertices: %d\n"
				"%sobject num faces: %d\n"
				"%sobject num subobjects: %d\n",
				istr, object->name,
				istr, object->vertex_count,
				istr, g_slist_length(object->faces),
				istr, g_slist_length(object->objects));
			g_free(istr);
		}

		objects_stat(object->objects, detailed, stats, indent + 1);
	}

	return TRUE;
}

static gboolean file_stat(G3DContext *context, const char *filename,
	gboolean detailed, FileStats *stats, gchar **plugin_used)
{
	G3DModel *model;

	model = g3d_model_load(context, filename);
	if(model == NULL) {
		return FALSE;
	}

	objects_stat(model->objects, detailed, stats, 0);
	if(plugin_used && model->plugin)
		*plugin_used = g_strdup(model->plugin->name);

	g3d_model_free(model);

	return TRUE;
}
