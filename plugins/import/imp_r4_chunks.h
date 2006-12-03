#ifndef _IMP_R4_CHUNKS_H
#define _IMP_R4_CHUNKS_H

#include <g3d/iff.h>

#include "imp_r4_callbacks.h"

static g3d_iff_chunk_info r4_chunks[] = {
	{ "INFO", "information",                       0, NULL },
	{ "DRE2", "triangles",                         0, r4_cb_DRE2 },
	{ "GMA1", "unknown",                           0, r4_cb_GMAx },
	{ "GMAT", "unknown",                           0, r4_cb_GMAx },
	{ "KSYS", "unknown",                           0, r4_cb_KSYS },
	{ "LGH3", "light",                             0, r4_cb_LGH3 },
	{ "PKTM", "points",                            0, r4_cb_PKTM },
	{ "RGE1", "unknown",                           0, r4_cb_RGE1 },
	{ "RKA2", "camera",                            0, r4_cb_RKAx },
	{ "RKA3", "camera",                            0, r4_cb_RKAx },
	{ "ROBJ", "object name",                       0, r4_cb_ROBJ },
	{ "SUR1", "unknown",                           0, r4_cb_SURx },
	{ "SURF", "unknown",                           0, r4_cb_SURx },
	{ "TXM1", "unknown",                           0, r4_cb_TXM1 },
	{ "TXO1", "unknown",                           0, r4_cb_TXO1 },

	{ NULL, NULL, 0, NULL }
};

#endif /* _IMP_R4_CHUNKS_H */
