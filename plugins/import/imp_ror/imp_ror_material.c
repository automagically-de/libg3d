#include <glib.h>

#include <g3d/stream.h>

#define ROR_LL 2048

GHashTable *ror_material_read(const gchar *filename)
{
	GHashTable *ht;
	G3DStream *stream;
	gchar *buffer;
	
	stream = g3d_stream_open_file(filename, "r");
	if(stream == NULL)
		return NULL;

	ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	buffer = g_new0(gchar, ROR_LL);
	while(g3d_stream_read_line(stream, buffer, ROR_LL)) {
		g_strstrip(buffer);
#if DEBUG > 0
		g_debug("RoRmat: %s", buffer);
#endif
		if(strncmp(buffer, "texture ", 8) == 0)
			g_hash_table_insert(ht, g_strdup("texture"), g_strdup(buffer + 8));
	}

	g_free(buffer);
	g3d_stream_close(stream);

	return ht;
}
