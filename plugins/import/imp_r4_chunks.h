#ifndef _IMP_R4_CHUNKS_H
#define _IMP_R4_CHUNKS_H

#include <g3d/iff.h>

static g3d_iff_chunk_info r4_chunks[] = {
	{ "INFO", "information",                       0, NULL },
	{ "DRE2", "unknown",                           0, NULL },
	{ "GMAT", "unknown",                           1, NULL },
	{ "KSYS", "unknown",                           0, NULL },
	{ "PKTM", "unknown",                           0, NULL },
	{ "RGE1", "unknown",                           0, NULL },
	{ "RKA2", "unknown",                           0, NULL },
	{ "ROBJ", "unknown",                           0, NULL },
	{ "SURF", "unknown",                           0, NULL },
	{ "TXM1", "unknown",                           0, NULL },
	{ "TXO1", "unknown",                           0, NULL },

	{ NULL, NULL, 0, NULL }
};

#endif /* _IMP_R4_CHUNKS_H */
