#ifndef _G3D_R4_CALLBACKS_H
#define _G3D_R4_CALLBACKS_H

#include <g3d/iff.h>

gboolean r4_cb_DRE2(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_GMAx(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_KSYS(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_LGH3(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_PKTM(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_RGE1(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_RKAx(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_ROBJ(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_SURx(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_TXM1(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean r4_cb_TXO1(g3d_iff_gdata *global, g3d_iff_ldata *local);

#endif /* _G3D_R4_CALLBACKS_H */
