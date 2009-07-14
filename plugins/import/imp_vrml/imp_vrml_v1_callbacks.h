#ifndef _IMP_VRML_V1_CALLBACKS_H
#define _IMP_VRML_V1_CALLBACKS_H

#include "imp_vrml_v1_types.h"

gboolean v1_cb_coordinate3(VrmlGlobal *global, VrmlNode *node, VrmlStep s);
gboolean v1_cb_idxfaceset(VrmlGlobal *global, VrmlNode *node, VrmlStep s);
gboolean v1_cb_material(VrmlGlobal *global, VrmlNode *node, VrmlStep s);
gboolean v1_cb_separator(VrmlGlobal *global, VrmlNode *node, VrmlStep s);

#endif /* _IMP_VRML_V1_CALLBACKS_H */
