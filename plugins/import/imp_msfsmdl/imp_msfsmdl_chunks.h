#ifndef IMP_MSFSMDL_CHUNKS_H
#define IMP_MSFSMDL_CHUNKS_H

#include <glib.h>
#include <g3d/iff.h>

#include "imp_msfsmdl_callbacks.h"

static G3DIffChunkInfo mdl_chunks[] = {
	{ "AMAP", "animation map",     0, NULL },
	{ "ANIB", "animation block",   0, NULL },
	{ "ATTO", "attached objects",  0, NULL },
	{ "BBOX", "bounding box",      0, NULL },
	{ "BGL ", "BGL instructions",  0, msfsmdl_cb_bgl_ },
	{ "CRAS", "crash tree",        0, NULL },
	{ "DICT", "XXX: dictionary",   0, NULL },
	{ "INDE", "tri index list",    0, NULL },
	{ "ISFT", "XXX",               0, NULL },
	{ "LODE", "LOD entry",         0, NULL },
	{ "LODT", "LOD table",         1, NULL },
	{ "MATE", "material list",     0, NULL },
	{ "MDLD", "model data",        1, NULL },
	{ "MDLG", "model GUID",        0, NULL },
	{ "MDLH", "model header",      0, NULL },
	{ "MDLN", "model name",        0, NULL },
	{ "MREC", "XXX",               0, NULL },
	{ "MREI", "XXX",               0, NULL },
	{ "PARA", "parameter block",   0, NULL },
	{ "RADI", "radius",            0, NULL },
	{ "REFL", "attachpoint list",  1, NULL },
	{ "REFP", "attach point",      0, NULL },
	{ "SCEN", "scenegraph root",   0, NULL },
	{ "SGAL", "sg anim linkage",   0, NULL },
	{ "SGBR", "sg bone reference", 0, NULL },
	{ "SGJC", "sg joint constrs",  0, NULL },
	{ "SGVL", "sg visibility link",0, NULL },
	{ "SMAP", "XXX",               0, NULL },
	{ "TANS", "XXX: tangent space",0, NULL },
	{ "TEXT", "texture list",      0, NULL },
	{ "TRAN", "trans matrices",    0, NULL },
	{ "VERB", "vertex buffer list",1, NULL },
	{ "VERT", "vertex list",       0, NULL },
	{ "VISL", "visualization list",0, NULL },

	{ NULL, NULL, 0, NULL },
};

#endif /* IMP_MSFSMDL_CHUNKS_H */
