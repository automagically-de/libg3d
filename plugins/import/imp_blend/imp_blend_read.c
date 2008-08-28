#include <g3d/stream.h>

#include "imp_blend_def.h"

guint32 blend_read_short(G3DStream *stream, guint32 flags)
{
	return (flags & FLAG_LITTLE_ENDIAN) ?
		g3d_stream_read_int16_le(stream) :
		g3d_stream_read_int16_be(stream);
}

guint32 blend_read_uint(G3DStream *stream, guint32 flags)
{
	return (flags & FLAG_LITTLE_ENDIAN) ?
		g3d_stream_read_int32_le(stream) :
		g3d_stream_read_int32_be(stream);
}

