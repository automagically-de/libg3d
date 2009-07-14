#ifndef _IMP_VRML_V1_CHUNKS_H
#define _IMP_VRML_V1_CHUNKS_H

#include "imp_vrml_v1_types.h"
#include "imp_vrml_v1_callbacks.h"

static VrmlSymbol vrml_symbols[] = {
	{ "Cone",             VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Coordinate3",      VRML_V1_SCOPE_DEFAULT, 0, 0, v1_cb_coordinate3 },
	{ "Cube",             VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Cylinder",         VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Group",            VRML_V1_SCOPE_DEFAULT, 0 },
	{ "IndexedFaceSet",   VRML_V1_SCOPE_DEFAULT, 0, 0, v1_cb_idxfaceset },
	{ "IndexedLineSet",   VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Info",             VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Material",         VRML_V1_SCOPE_DEFAULT, 0, 0, v1_cb_material },
	{ "MaterialBinding",  VRML_V1_SCOPE_DEFAULT, 0 },
	{ "MatrixTransform",  VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Normal",           VRML_V1_SCOPE_DEFAULT, 0 },
	{ "NormalBinding",    VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Separator",        VRML_V1_SCOPE_DEFAULT, 0, 0, v1_cb_separator },
	{ "ShapeHints",       VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Sphere",           VRML_V1_SCOPE_DEFAULT, 0 },
	{ "SpotLight",        VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Texture2",         VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Texture2Transform",VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Transform",        VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Translation",      VRML_V1_SCOPE_DEFAULT, 0 },

	{ "ambientColor",     VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "bottomRadius",     VRML_V1_SCOPE_VALUE,   1, 1 },
	{ "center",           VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "color",            VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "coordIndex",       VRML_V1_SCOPE_VALUE,   1, 0 },
	{ "creaseAngle",      VRML_V1_SCOPE_VALUE,   1, 1 },
	{ "cutOffAngle",      VRML_V1_SCOPE_VALUE,   1, 1 },
	{ "depth",            VRML_V1_SCOPE_VALUE,   1, 1 },
	{ "diffuseColor",     VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "direction",        VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "dropOffRate",      VRML_V1_SCOPE_VALUE,   1 },
	{ "emissiveColor",    VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "faceType",         VRML_V1_SCOPE_VALUE,   1 },
	{ "filename",         VRML_V1_SCOPE_VALUE,   1 },
	{ "height",           VRML_V1_SCOPE_VALUE,   1, 1 },
	{ "intensity",        VRML_V1_SCOPE_VALUE,   1 },
	{ "location",         VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "materialIndex",    VRML_V1_SCOPE_VALUE,   1, 0 },
	{ "matrix",           VRML_V1_SCOPE_VALUE,   16, 1 },
	{ "normalIndex",      VRML_V1_SCOPE_VALUE,   1, 0 },
	{ "on",               VRML_V1_SCOPE_VALUE,   1 },
	{ "point",            VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "radius",           VRML_V1_SCOPE_VALUE,   1, 1},
	{ "rotation",         VRML_V1_SCOPE_VALUE,   4, 1 },
	{ "scaleFactor",      VRML_V1_SCOPE_VALUE,   2, 1 },
	{ "shapeType",        VRML_V1_SCOPE_VALUE,   1 },
	{ "shininess",        VRML_V1_SCOPE_VALUE,   1, 1 },
	{ "specularColor",    VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "string",           VRML_V1_SCOPE_VALUE,   1 },
	{ "textureCoordIndex",VRML_V1_SCOPE_VALUE,   1, 0 },
	{ "translation",      VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "transparency",     VRML_V1_SCOPE_VALUE,   1, 1 },
	{ "value",            VRML_V1_SCOPE_VALUE,   1 },
	{ "vector",           VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "vertexOrdering",   VRML_V1_SCOPE_VALUE,   1 },
	{ "width",            VRML_V1_SCOPE_VALUE,   1, 1 },

	{ "DEF",              VRML_V1_SCOPE_DEF,     0 },
	{ "USE",              VRML_V1_SCOPE_USE,     0 },
	{ NULL }
};

#endif /* _IMP_VRML_V1_CHUNKS_H */

