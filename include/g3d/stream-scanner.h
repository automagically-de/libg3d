#ifndef _G3D_STREAM_SCANNER_H
#define _G3D_STREAM_SCANNER_H

#include <g3d/types.h>

struct _G3DStreamScanner {
	GScanner *gscanner;
	G3DStream *stream;
	G3DStreamScannerHandler handler;
	gpointer user_data;
};

G3DStreamScanner *g3d_stream_scanner_new(G3DStream *stream);
void g3d_stream_scanner_free(G3DStreamScanner *scanner);
GScanner *g3d_stream_scanner_get_gscanner(G3DStreamScanner *scanner);
void g3d_stream_scanner_set_handler(G3DStreamScanner *scanner,
	G3DStreamScannerHandler handler, gpointer user_data);
gboolean g3d_stream_scanner_run(G3DStreamScanner *scanner);

#endif /* _G3D_STREAM_SCANNER_H */

