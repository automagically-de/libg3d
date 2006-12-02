#ifndef _IMP_R4_CHUNKS_H
#define _IMP_R4_CHUNKS_H

#include <g3d/iff.h>

#include "imp_r4_callbacks.h"

static g3d_iff_chunk_info r4_chunks[] = {
	{ "INFO", "information",                       0, NULL },
	{ "DRE2", "unknown",                           0, r4_cb_DRE2 },
	{ "GMAT", "unknown",                           0, r4_cb_GMAT },
	{ "KSYS", "unknown",                           0, NULL },
	{ "LGH3", "unknown",                           0, r4_cb_LGH3 },
	{ "PKTM", "unknown",                           0, NULL },
	{ "RGE1", "unknown",                           0, r4_cb_RGE1 },
	{ "RKA2", "unknown",                           0, r4_cb_RKA2 },
	{ "ROBJ", "unknown",                           0, r4_cb_ROBJ },
	{ "SURF", "unknown",                           0, r4_cb_SURF },
	{ "TXM1", "unknown",                           0, r4_cb_TXM1 },
	{ "TXO1", "unknown",                           0, NULL },

	{ NULL, NULL, 0, NULL }
};

#endif /* _IMP_R4_CHUNKS_H */
