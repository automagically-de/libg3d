#ifndef _IMP_DXF_CALLBACKS_H
#define _IMP_DXF_CALLBACKS_H

#include "imp_dxf_types.h"

gboolean dxf_empty(DxfGlobalData *global, DxfLocalData *local);

gboolean dxf_debug_str(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_i16(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_i32(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_dbl(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_debug_var(DxfGlobalData *global, DxfLocalData *local);

gboolean dxf_grpcode_70(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_grpcode_71(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_grpcode_72(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_grpcode_73(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_grpcode_74(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_pnt_coord(DxfGlobalData *global, DxfLocalData *local);

gboolean dxf_e_3DFACE(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_e_BLOCK(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_e_ENDBLK(DxfGlobalData *global, DxfLocalData *local);
gboolean dxf_e_POLYLINE(DxfGlobalData *global, DxfLocalData *local);

#endif /* _IMP_DXF_CALLBACKS_H */
