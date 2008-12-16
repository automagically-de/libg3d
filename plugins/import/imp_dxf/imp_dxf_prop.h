#ifndef _IMP_DXF_PROP_H
#define _IMP_DXF_PROP_H

#include "imp_dxf_types.h"

DxfEntityProps *dxf_prop_create(void);
void dxf_prop_cleanup(DxfEntityProps *eprop);

gboolean dxf_prop_set_int(DxfEntityProps *eprop, gint32 key, gint32 i);
gboolean dxf_prop_set_dbl(DxfEntityProps *eprop, gint32 key, gdouble dbl);
gboolean dxf_prop_set_str(DxfEntityProps *eprop, gint32 key,
	const gchar *str);

gint32 dxf_prop_get_int(DxfEntityProps *eprop, gint32 key, gint32 dfl);
gdouble dxf_prop_get_dbl(DxfEntityProps *eprop, gint32 key, gdouble dfl);
const gchar *dxf_prop_get_str(DxfEntityProps *eprop, gint32 key,
	const gchar *dfl);

#endif /* _IMP_DXF_PROP_H */
