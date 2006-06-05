#ifndef _IMP_MAYA_CALLBACKS_H
#define _IMP_MAYA_CALLBACKS_H

#include <g3d/iff.h>

gboolean maya_cb_DBL2(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_DBL3(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_DBLE(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_MESH(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_STR_(g3d_iff_gdata *global, g3d_iff_ldata *local);

#endif /* _IMP_MAYA_CALLBACKS_H */
