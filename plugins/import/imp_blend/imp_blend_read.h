#ifndef _IMP_BLEND_READ_H
#define _IMP_BLEND_READ_H

#include <g3d/stream.h>

guint32 blend_read_short(G3DStream *stream, guint32 flags);
guint32 blend_read_uint(G3DStream *stream, guint32 flags);

#endif /* _IMP_BLEND_READ_H */
