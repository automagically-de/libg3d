#ifndef _IMP_DXF_CALLBACKS_H
#define _IMP_DXF_CALLBACKS_H

#include <g3d/types.h>

#include "imp_dxf.h"

typedef struct {
	gint32 id;
	gint32 parentid;
	gchar *entity;
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	guint32 vertex_offset;
} DxfLocalData;

typedef gboolean (* DxfCallback)(DxfGlobalData *, DxfLocalData *);

gboolean dxf_debug_str(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_i16(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_dbl(DxfGlobalData *global, DxfLocalData *local);

gboolean dxf_grpcode_0(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_pnt_coord(DxfGlobalData *global, DxfLocalData *local);

#endif /* _IMP_DXF_CALLBACKS_H */
