#include <string.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/stream-scanner.h>

G3DStreamScanner *g3d_stream_scanner_new(G3DStream *stream)
{
	G3DStreamScanner *scanner;

	scanner = g_new0(G3DStreamScanner, 1);
	scanner->stream = stream;

	scanner->gscanner = g_scanner_new(NULL);

	return scanner;
}

void g3d_stream_scanner_free(G3DStreamScanner *scanner)
{
	g_scanner_destroy(scanner->gscanner);
	g_free(scanner);
}

GScanner *g3d_stream_scanner_get_gscanner(G3DStreamScanner *scanner)
{
	return scanner->gscanner;
}

void g3d_stream_scanner_set_handler(G3DStreamScanner *scanner,
	G3DStreamScannerHandler handler, gpointer user_data)
{
	scanner->handler = handler;
	scanner->user_data = user_data;
}

gboolean g3d_stream_scanner_run(G3DStreamScanner *scanner)
{
	gchar line[2049];
	gchar *buffer = NULL;
	gsize buflen;

	while(!g3d_stream_eof(scanner->stream)) {
		if(buffer) {
			g_free(buffer);
			buffer = NULL;
		}
		buflen = 0;

		while((buflen < 64*1024) && !g3d_stream_eof(scanner->stream)) {
			memset(line, '\0', 2049);
			if(g3d_stream_read_line(scanner->stream, line, 2048)) {
				buffer = g_realloc(buffer, (buflen + strlen(line) + 1) *
					sizeof(gchar));
				strcpy(buffer + buflen, line);
				buflen += strlen(line);
			}
		}

		if(buffer && (buflen > 0)) {
			g_scanner_input_text(scanner->gscanner, buffer, buflen);
			if(scanner->handler) {
				if(!scanner->handler(scanner->gscanner, scanner->user_data)) {
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}
