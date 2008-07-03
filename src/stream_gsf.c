/* $Id:$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2008  Markus Dahms <mad@automagically.de>

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

#include <string.h>

#include <gsf/gsf-input.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-input-gzip.h>
#include <gsf/gsf-infile-msole.h>
#include <gsf/gsf-infile-zip.h>

#include <g3d/stream.h>

typedef struct {
	GsfInput *input_container;
	GsfInfile *infile_msole;
	GsfInfile *infile_zip;
	GsfInput *input_subfile;
	gchar *uri;
} G3DStreamGsf;

static gsize g3d_stream_gsf_read(gpointer ptr, gsize size, gsize nmemb,
	 gpointer data)
{
	G3DStreamGsf *sg = (G3DStreamGsf *)data;
	gsize n;

	n = MIN(nmemb * size, gsf_input_remaining(sg->input_subfile));
	gsf_input_read(sg->input_subfile, n, (guint8 *)ptr);
	return n;
}

static gint g3d_stream_gsf_seek(gpointer data, goffset offset,
	GSeekType whence)
{
	G3DStreamGsf *sg = (G3DStreamGsf *)data;
	return gsf_input_seek(sg->input_subfile, offset, whence);
}

static goffset g3d_stream_gsf_tell(gpointer data)
{
	G3DStreamGsf *sg = (G3DStreamGsf *)data;
	return gsf_input_tell(sg->input_subfile);
}

static goffset g3d_stream_gsf_size(gpointer data)
{
	G3DStreamGsf *sg = (G3DStreamGsf *)data;
	return gsf_input_size(sg->input_subfile);
}

static gboolean g3d_stream_gsf_eof(gpointer data)
{
	G3DStreamGsf *sg = (G3DStreamGsf *)data;
	if(gsf_input_remaining(sg->input_subfile) <= 0)
		return TRUE;
	return gsf_input_eof(sg->input_subfile);
}

static gint g3d_stream_gsf_close(gpointer data)
{
	G3DStreamGsf *sg = (G3DStreamGsf *)data;
	g_object_unref(sg->input_subfile);
	if(sg->infile_msole)
		g_object_unref(sg->infile_msole);
	if(sg->infile_zip)
		g_object_unref(sg->infile_zip);
	if(sg->uri)
		g_free(sg->uri);
	g_object_unref(sg->input_container);
	g_free(sg);
	return 0;
}

static GsfInput *stream_gsf_chdir(GsfInput *input, gchar *dirname)
{
	GsfInput *parent = input, *newinput;
	gchar **dirs, **dir;

	dirs = g_strsplit(dirname, "/", 0);
	for(dir = dirs; *dir != NULL; dir ++) {
		newinput = gsf_infile_child_by_name(GSF_INFILE(parent),
			*dir);
		if((newinput == NULL) || (!GSF_IS_INFILE(newinput))) {
			g_strfreev(dirs);
			return NULL;
		}
		parent = newinput;
	}
	g_strfreev(dirs);
	return newinput;
}

G3DStream *g3d_stream_open_zip(const gchar *filename, const gchar *subfile)
{
	G3DStreamGsf *sg;
	GError *error = NULL;
	guint32 flags = 0;
	GsfInput *input_dir;
	gchar *basename, *dirname;

	sg = g_new0(G3DStreamGsf, 1);

	sg->input_container = gsf_input_stdio_new(filename, &error);
	if(error != NULL) {
		g_warning("error opening container file '%s': %s", filename,
			error->message);
		g_error_free(error);
		g_free(sg);
		return NULL;
	}

	sg->infile_zip = gsf_infile_zip_new(sg->input_container, &error);
	if(error != NULL) {
		g_warning("error reading ZIP data from '%s': %s",
			filename, error->message);
		g_object_unref(sg->input_container);
		g_error_free(error);
		g_free(sg);
		return NULL;
	}

	if(strchr(subfile, '/')) {
		basename = g_path_get_basename(subfile);
		dirname = g_path_get_dirname(subfile);
		input_dir = stream_gsf_chdir(GSF_INPUT(sg->infile_zip), dirname);
		sg->input_subfile = gsf_infile_child_by_name(GSF_INFILE(input_dir),
			basename);
		g_free(basename);
		g_free(dirname);
	} else {
		sg->input_subfile = gsf_infile_child_by_name(sg->infile_zip, subfile);
	}

	if(!GSF_IS_INPUT(sg->input_subfile)) {
		g_warning("error: %s is not an input file", subfile);
		g_object_unref(sg->infile_zip);
		g_object_unref(sg->input_container);
		g_free(sg);
		return NULL;
	}

	flags |= (1 << G3D_STREAM_READABLE);
	flags |= (1 << G3D_STREAM_SEEKABLE);

	sg->uri = g_strdup_printf("zip://%s:%s", filename, subfile);

	return g3d_stream_new_custom(flags, sg->uri,
		g3d_stream_gsf_read, NULL,
		g3d_stream_gsf_seek, g3d_stream_gsf_tell,
		g3d_stream_gsf_size, g3d_stream_gsf_eof,
		g3d_stream_gsf_close, sg);
}

G3DStream *g3d_stream_open_structured_file(const gchar *filename,
	const gchar *subfile)
{
	G3DStreamGsf *sg;
	GsfInput *input_gzipped, *input_tmp;
	GError *error = NULL;
	guint32 flags = 0;

	sg = g_new0(G3DStreamGsf, 1);

#if DEBUG > 2
	g_debug("GSF: Hello, World!");
#endif

	sg->input_container = gsf_input_stdio_new(filename, &error);
	if(error != NULL) {
		g_warning("error opening structured file '%s': %s", filename,
			error->message);
		g_error_free(error);
		g_free(sg);
		return NULL;
	}
#if DEBUG > 2
	g_debug("GSF: opened file '%s'", filename);
#endif
	sg->infile_msole = gsf_infile_msole_new(sg->input_container, &error);
	if(error != NULL) {
		g_warning("error reading OLE data from '%s': %s",
			filename, error->message);
		g_object_unref(sg->input_container);
		g_error_free(error);
		g_free(sg);
		return NULL;
	}
#if DEBUG > 2
	g_debug("GSF: new MSOLE infile");
#endif
	sg->input_subfile = gsf_infile_child_by_name(sg->infile_msole, subfile);
	if(error != NULL) {
		g_warning("error opening contained file '%s' in '%s': %s",
			subfile, filename, error->message);
		g_object_unref(sg->infile_msole);
		g_object_unref(sg->input_container);
		g_error_free(error);
		g_free(sg);
		return NULL;
	}
	if(!GSF_IS_INPUT(sg->input_subfile)) {
		g_object_unref(sg->infile_msole);
		g_object_unref(sg->input_container);
		g_free(sg);
		return NULL;
	}

	/* try to open subfile as gzipped */
	input_gzipped = gsf_input_gzip_new(sg->input_subfile, &error);
	if(error != NULL) {
		/* failed to open gzip data */
		g_error_free(error);
	} else {
		/* succeeded to open gzip data */
		input_tmp = sg->input_subfile;
		sg->input_subfile = input_gzipped;
		g_object_unref(input_tmp);
	}

#if DEBUG > 2
	g_debug("GSF: got subfile '%s'", subfile);
#endif
	flags |= (1 << G3D_STREAM_READABLE);
	flags |= (1 << G3D_STREAM_SEEKABLE);

	return g3d_stream_new_custom(flags, filename,
		g3d_stream_gsf_read, NULL,
		g3d_stream_gsf_seek, g3d_stream_gsf_tell,
		g3d_stream_gsf_size, g3d_stream_gsf_eof,
		g3d_stream_gsf_close, sg);
}

