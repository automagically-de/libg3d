#ifndef _IMP_RBH_CALLBACKS_H
#define _IMP_RBH_CALLBACKS_H

#include <g3d/iff.h>

gboolean rbh_cb_RBHH(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean rbh_cb_BODY(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean rbh_cb_RELC(g3d_iff_gdata *global, g3d_iff_ldata *local);

#endif /* _IMP_RBH_CALLBACKS_H */
