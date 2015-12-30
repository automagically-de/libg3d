#include "imp_msfsmdl_callbacks.h"
#include "imp_msfsmdl_bgl.h"

gboolean msfsmdl_cb_bgl_(G3DIffGlobal *global, G3DIffLocal *local) {
	return msfsmdl_parse_bgl(global, local);
}
