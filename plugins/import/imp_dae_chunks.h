#ifndef _IMP_DAE_CHUNKS_H
#define _IMP_DAE_CHUNKS_H

#include "imp_dae_cb.h"

static DaeChunkDesc dae_chunks_geometry[] = {
	{ "mesh",            NULL },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_node[] = {
	{ "camera",          NULL },
	{ "geometry",        dae_cb_geometry },
	{ "light",           NULL },
	{ "rotate",          NULL },
	{ "translate",       NULL },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_visual_scene[] = {
	{ "node",            dae_cb_node },
	{ NULL, NULL }
};

#endif /* _IMP_DAE_CHUNKS_H */
