#ifndef _IMP_RBH_CHUNKS_H
#define _IMP_RBH_CHUNKS_H

#include <g3d/iff.h>

#include "imp_rbh_callbacks.h"

static g3d_iff_chunk_info rbh_chunks[] = {
	{ "RBHH", "RBH header",                        0, rbh_cb_RBHH },
	{ "BODY", "body",                              0, rbh_cb_BODY },
	{ "RELC", "unknown",                           0, rbh_cb_RELC },

	{ NULL, NULL, 0, NULL }
};

#endif /* _IMP_RBH_CHUNKS_H */
