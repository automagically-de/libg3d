#ifndef IMP_MSFSMDL_CALLBACKS_H
#define IMP_MSFSMDL_CALLBACKS_H

#include <g3d/iff.h>

gboolean msfsmdl_cb_amap(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_bgl_(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_inde(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_lode(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_mate(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_mdld(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_part(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_scen(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_text(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_tran(G3DIffGlobal *global, G3DIffLocal *local);
gboolean msfsmdl_cb_vert(G3DIffGlobal *global, G3DIffLocal *local);

#endif /* IMP_MSFSMDL_CALLBACKS_H */
