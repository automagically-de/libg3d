#ifndef _IMP_VRML_V1_CHUNKS_H
#define _IMP_VRML_V1_CHUNKS_H

#include "imp_vrml_v2_types.h"

static VrmlV2Symbol vrml_v2_symbols[] = {
	{ "Appearance",     VRML_V2_SCOPE_DEFAULT },
	{ "Box",            VRML_V2_SCOPE_DEFAULT },
	{ "Coordinate",     VRML_V2_SCOPE_DEFAULT },
	{ "IndexedFaceSet", VRML_V2_SCOPE_DEFAULT },
	{ "Material",       VRML_V2_SCOPE_DEFAULT },
	{ "NavigationInfo", VRML_V2_SCOPE_DEFAULT },
	{ "Shape",          VRML_V2_SCOPE_DEFAULT },
	{ "Transform",      VRML_V2_SCOPE_DEFAULT },

	{ "appearance",     VRML_V2_SCOPE_DEFAULT, 1, VRML_V2_TYPE_OBJECT },
	{ "children",       VRML_V2_SCOPE_DEFAULT, 1, VRML_V2_TYPE_OBJECT },
	{ "coord",          VRML_V2_SCOPE_DEFAULT, 1, VRML_V2_TYPE_OBJECT },
	{ "coordIndex",     VRML_V2_SCOPE_VALUE,   1, VRML_V2_TYPE_INT },
	{ "creaseAngle",    VRML_V2_SCOPE_VALUE,   1, VRML_V2_TYPE_FLOAT },
	{ "diffuseColor",   VRML_V2_SCOPE_VALUE,   3, VRML_V2_TYPE_FLOAT },
	{ "geometry",       VRML_V2_SCOPE_DEFAULT, 1, VRML_V2_TYPE_OBJECT },
	{ "headlight",      VRML_V2_SCOPE_VALUE,   1, VRML_V2_TYPE_BOOLEAN },
	{ "material",       VRML_V2_SCOPE_DEFAULT, 1, VRML_V2_TYPE_OBJECT },
	{ "point",          VRML_V2_SCOPE_VALUE,   3, VRML_V2_TYPE_FLOAT },
	{ "size",           VRML_V2_SCOPE_VALUE,   3, VRML_V2_TYPE_FLOAT },
	{ "solid",          VRML_V2_SCOPE_VALUE,   1, VRML_V2_TYPE_BOOLEAN },
	{ "translation",    VRML_V2_SCOPE_VALUE,   3, VRML_V2_TYPE_FLOAT },
	{ "transparency",   VRML_V2_SCOPE_VALUE,   1, VRML_V2_TYPE_FLOAT },

	{ NULL }
};

#endif /* _IMP_VRML_V1_CHUNKS_H */
