#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <g3d/types.h>
#include <g3d/stream.h>

#include "imp_3dm_types.h"

static guint32 tdm_object_read_vector_data(G3DStream *stream,
	G3DVector *vertex_data, guint32 vsize, guint32 vcount)
{
	gsize nb = 0;
	gint32 i, j;

	for(i = 0; i < vcount; i ++)
		for(j = 0; j < vsize; j ++) {
			vertex_data[i * vsize + j] = g3d_stream_read_float_le(stream);
			nb += 4;
		}
	return nb;
}

gboolean tdm_object_read_vertex_data_uncompressed(TdmGlobal *global,
	TdmLocal *local)
{
	TdmObjectRecord *obj = local->object;

	local->len -= tdm_object_read_vector_data(global->stream,
		obj->object->vertex_data, 3, obj->object->vertex_count);
	return TRUE;
}

gboolean tdm_object_read_vertex_data_compressed(TdmGlobal *global,
	TdmLocal *local)
{
#if HAVE_ZLIB
	TdmObjectRecord *obj = local->object;
	G3DStream *zstream;
	guint32 csize;

	g3d_stream_read_int32_le(global->stream); /* tcode */
	csize = g3d_stream_read_int32_le(global->stream) - 4; /* size */
	local->len -= 8;

	zstream = g3d_stream_zlib_inflate_stream(global->stream, csize);
	if(zstream) {
		tdm_object_read_vector_data(zstream,
			obj->object->vertex_data, 3, obj->object->vertex_count);
		local->len -= csize;
		g3d_stream_close(zstream);
		/* CRC */
		g3d_stream_read_int32_le(global->stream);
		local->len -= 4;
		return TRUE;
	};
#else
	g_warning("no zlib support, unable to read deflated data");
#endif /* HAVE_ZLIB */
	return FALSE;
}
