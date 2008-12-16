#ifndef _IMP_DXF_CALLBACKS_H
#define _IMP_DXF_CALLBACKS_H

#include "imp_dxf_types.h"

gboolean dxf_debug_var(DxfGlobalData *global, DxfLocalData *local);

gboolean dxf_e_3DFACE(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_e_BLOCK(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_e_ENDBLK(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_e_INSERT(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_e_POLYLINE(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_e_VERTEX(DxfGlobalData *global, DxfLocalData *local);

#endif /* _IMP_DXF_CALLBACKS_H */
