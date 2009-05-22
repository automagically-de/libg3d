#include <string.h>

#include <glib.h>

#include <g3d/stream.h>

#define ROR_LL 2048

gchar *ror_material_get_stack(GQueue *stack, const gchar *current);

GHashTable *ror_material_read(const gchar *filename)
{
	GHashTable *ht;
	GQueue *stack;
	GList *item;
	G3DStream *stream;
	gchar *buffer, *sbuffer = NULL, *path, *value, *space;
	
	stream = g3d_stream_open_file(filename, "r");
	if(stream == NULL)
		return NULL;

	stack = g_queue_new();

	ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	buffer = g_new0(gchar, ROR_LL);
	while(g3d_stream_read_line(stream, buffer, ROR_LL)) {
		g_strstrip(buffer);
#if DEBUG > 2
		g_debug("RoRmat: %s", buffer);
#endif
		if(buffer[0] == '{') {
			if(sbuffer != NULL) {
				g_queue_push_tail(stack, sbuffer);
				sbuffer = NULL;
			}
		} else if (buffer[0] == '}') {
			sbuffer = g_queue_pop_tail(stack);
			if(sbuffer) {
				g_free(sbuffer);
				sbuffer = NULL;
			}
		} else {
			space = strchr(buffer, ' ');
			if(space && g_queue_peek_tail(stack)) {
				/* data leaf */
				value = g_strdup(space + 1);
				sbuffer = g_strndup(buffer, space - buffer);
				g_hash_table_insert(ht,
					ror_material_get_stack(stack, sbuffer),
					value);
			} else {
				sbuffer = g_strdup(buffer);
			}
			path = ror_material_get_stack(stack, sbuffer);
#if DEBUG > 0
			g_debug("RoR: path: '%s'", path);
#endif
			g_free(path);
		}
		if(strncmp(buffer, "texture ", 8) == 0)
			g_hash_table_insert(ht, g_strdup("texture"), g_strdup(buffer + 8));
	}

	for(item = stack->head; item != NULL; item = item->next) {
		g_free(item->data);
		item->data = NULL;
	}
	g_queue_free(stack);
	if(sbuffer)
		g_free(sbuffer);
	g_free(buffer);
	g3d_stream_close(stream);

	return ht;
}

gchar *ror_material_get_stack(GQueue *stack, const gchar *current)
{
	GList *item;
	gchar *path, *tmp;

	path = g_strdup("");
	for(item = stack->head; item != NULL; item = item->next) {
		tmp = g_strdup_printf("%s%s%s",
			path,
			path[0] == '\0' ? "" : ".",
			(gchar *)item->data);
		g_free(path);
		path = tmp;
	}
	tmp = g_strdup_printf("%s%s%s",
		path,
		path[0] == '\0' ? "" : ".",
		current);
	g_free(path);
	path = tmp;

	return path;
}
