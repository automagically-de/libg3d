#ifndef _IMP_BLEND_CALLBACKS_H
#define _IMP_BLEND_CALLBACKS_H

#include "imp_blend_types.h"

gboolean blend_cb_DATA_ME(BlendGlobal *global, BlendLocal *local);

gboolean blend_cb_DNA1(BlendGlobal *global, BlendLocal *local);

gboolean blend_cb_MA(BlendGlobal *global, BlendLocal *local);
gboolean blend_cb_ME(BlendGlobal *global, BlendLocal *local);
gboolean blend_cb_OB(BlendGlobal *global, BlendLocal *local);

#endif /* _IMP_BLEND_CALLBACKS_H */
