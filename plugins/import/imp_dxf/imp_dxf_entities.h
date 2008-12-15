#ifndef _IMP_DXF_ENTITIES_H
#define _IMP_DXF_ENTITIES_H

#include "imp_dxf_types.h"
#include "imp_dxf_def.h"

static DxfEntityInfo dxf_entities[] = {
	{ "3DFACE",    "3D face",                   DXF_E_3DFACE,   NULL },
	{ "POLYLINE",  "Polyline",                  DXF_E_POLYLINE, NULL },
	{ "VERTEX",    "Vertex",                    DXF_E_VERTEX,   NULL },

	{ NULL, NULL, 0, NULL }
};

#endif
