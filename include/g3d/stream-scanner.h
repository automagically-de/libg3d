/* $Id:$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2009  Markus Dahms <mad@automagically.de>

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

#ifndef _G3D_STREAM_SCANNER_H
#define _G3D_STREAM_SCANNER_H

/**
 * SECTION:stream-scanner
 * @short_description: GScanner wrapper for #G3DStream
 * @include: g3d/stream-scanner.h
 *
 * Some glue to build a lexical scanner using #GScanner on top of #G3DStream.
 */

#include <g3d/types.h>

struct _G3DStreamScanner {
	/*<private>*/
	GScanner *gscanner;
	G3DStream *stream;
	G3DStreamScannerHandler handler;
	gpointer user_data;
};

/**
 * g3d_stream_scanner_new:
 * @stream: a stream to read text data from
 *
 * Create a new stream scanner.
 *
 * Returns: a newly allocation stream scanner, or NULL in case of an error.
 */
G3DStreamScanner *g3d_stream_scanner_new(G3DStream *stream);

/**
 * g3d_stream_scanner_free:
 * @scanner: the scanner to free.
 *
 * Free all data allocated for the scanner. This does not manipulate the
 * stream.
 */
void g3d_stream_scanner_free(G3DStreamScanner *scanner);

/**
 * g3d_stream_scanner_get_gscanner:
 * @scanner: the stream scanner
 *
 * Get access to the #GScanner from the opaque stream scanner.
 *
 * Returns: the scanner.
 */
GScanner *g3d_stream_scanner_get_gscanner(G3DStreamScanner *scanner);

/**
 * g3d_stream_scanner_set_handler:
 * @scanner: a stream scanner
 * @handler: parsing callback
 * @user_data: data to be passed to handler callback
 *
 * Set the handler to be called when parsing a chunk of data.
 */
void g3d_stream_scanner_set_handler(G3DStreamScanner *scanner,
	G3DStreamScannerHandler handler, gpointer user_data);

/**
 * g3d_stream_scanner_run:
 * @scanner: a stream scanner
 *
 * Start reading data from stream and trying to parse it.
 *
 * Returns: TRUE on success, FALSE else.
 */
gboolean g3d_stream_scanner_run(G3DStreamScanner *scanner);

#endif /* _G3D_STREAM_SCANNER_H */

