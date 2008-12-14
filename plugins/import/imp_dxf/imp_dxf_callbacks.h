#ifndef _IMP_DXF_CALLBACKS_H
#define _IMP_DXF_CALLBACKS_H

#include <g3d/types.h>

#include "imp_dxf.h"

typedef struct {
	gint32 id;
	gint32 parentid;
	gchar *entity;
	DxfEntityData *edata;
} DxfLocalData;

typedef gboolean (* DxfCallback)(DxfGlobalData *, DxfLocalData *);

gboolean dxf_debug_str(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_i16(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_i32(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_dbl(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_var(DxfGlobalData *global, DxfLocalData *local);

gboolean dxf_grpcode_0(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_grpcode_70(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_grpcode_71(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_grpcode_72(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_pnt_coord(DxfGlobalData *global, DxfLocalData *local);

#endif /* _IMP_DXF_CALLBACKS_H */
