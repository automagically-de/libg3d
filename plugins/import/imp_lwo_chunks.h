#ifndef _IMP_LWO_CHUNKS_H
#define _IMP_LWO_CHUNKS_H

#include <g3d/iff.h>

#include "imp_lwo_callbacks.h"

static g3d_iff_chunk_info lwo_chunks[] = {
	{ "AAST", "antialiasing strength",       0, NULL },
	{ "AXIS", "major axis",                  0, NULL },
	{ "BBOX", "bounding box",                0, NULL },
	{ "BLOK", "block",                       1, NULL },
	{ "BTEX", "unknown",                     0, NULL },
	{ "CLIP", "image or image sequence",     0, NULL },
	{ "CMNT", "unknown",                     0, NULL },
	{ "COLR", "color",                       0, lwo_cb_COLR },
	{ "CTEX", "unknown",                     0, NULL },
	{ "DESC", "description",                 0, NULL },
	{ "DIFF", "diffuse",                     0, NULL },
	{ "FLAG", "flags",                       0, NULL },
	{ "GLOS", "unknown",                     0, NULL },
	{ "ICON", "icon",                        0, NULL },
	{ "IMAG", "image index",                 0, NULL },
	{ "IMAP", "image map (header)",          0, NULL },
	{ "LAYR", "layer",                       0, NULL },
	{ "LUMI", "unknown",                     0, NULL },
	{ "PIXB", "pixel blending",              0, NULL },
	{ "PNTS", "points",                      0, lwo_cb_PNTS },
	{ "POLS", "polygons",                    0, lwo_cb_POLS },
	{ "PROJ", "projection mode",             0, NULL },
	{ "PTAG", "polygon tag mapping",         0, lwo_cb_PTAG },
	{ "REFL", "reflection",                  0, NULL },
	{ "RFLT", "unknown",                     0, NULL },
	{ "RIMG", "unknown",                     0, NULL },
	{ "SMAN", "unknown",                     0, NULL },
	{ "SPEC", "specular",                    0, lwo_cb_SPEC },
	{ "SRFS", "surface materials",           0, lwo_cb_SRFS },
	{ "SURF", "surface material",            1, lwo_cb_SURF },
	{ "TAGS", "tags",                        0, lwo_cb_TAGS },
	{ "TAMP", "texture amplitude",           0, NULL },
	{ "TCLR", "unknown",                     0, NULL },
	{ "TCTR", "unknown",                     0, NULL },
	{ "TEXT", "unknown",                     0, NULL },
	{ "TFLG", "unknown",                     0, NULL },
	{ "TFP0", "unknown",                     0, NULL },
	{ "TFP1", "unknown",                     0, NULL },
	{ "TFRQ", "unknown",                     0, NULL },
	{ "TIMG", "unknown",                     0, NULL },
	{ "TIP0", "unknown",                     0, NULL },
	{ "TMAP", "texture map",                 0, NULL },
	{ "TRAN", "unknown",                     0, lwo_cb_TRAN },
	{ "TSIZ", "unknown",                     0, NULL },
	{ "TSP0", "unknown",                     0, NULL },
	{ "VDIF", "unknown",                     0, NULL },
	{ "VMAD", "discontinuous vertex map",    0, NULL },
	{ "VMAP", "uv vertex map",               0, NULL },
	{ "VRFL", "unknown",                     0, NULL },
	{ "VSPC", "unknown",                     0, NULL },
	{ "WRAP", "image wrap options",          0, NULL },
	{ "WRPH", "wrap amount (width)",         0, NULL },
	{ "WRPW", "wrap amount (height)",        0, NULL },

	{ NULL, NULL, 0, NULL }
};

#endif /* _IMP_LWO_CHUNKS_H */
