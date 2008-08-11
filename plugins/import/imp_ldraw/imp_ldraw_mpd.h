#ifndef _IMP_LDRAW_MPD_H
#define _IMP_LDRAW_MPD_H

#include <g3d/types.h>
#include <g3d/stream.h>

#include "imp_ldraw_types.h"

gboolean ldraw_mpd_load(G3DStream *stream, G3DModel *model,
	LDrawLibrary *lib);

#endif /* _IMP_LDRAW_MPD_H */
