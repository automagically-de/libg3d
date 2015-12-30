#ifndef IMP_MSFSMDL_CHUNKS_H
#define IMP_MSFSMDL_CHUNKS_H

#include <glib.h>
#include <g3d/iff.h>

#include "imp_msfsmdl_callbacks.h"

static G3DIffChunkInfo mdl_chunks[] = {
	{ "BBOX", "bounding box",      0, NULL },
	{ "BGL ", "BGL instructions",  0, msfsmdl_cb_bgl_ },
	{ "DICT", "XXX: dictionary",   0, NULL },
	{ "ISFT", "XXX",               0, NULL },
	{ "MDLH", "MDL header",        0, NULL },

	{ NULL, NULL, 0, NULL },
};

#endif /* IMP_MSFSMDL_CHUNKS_H */
