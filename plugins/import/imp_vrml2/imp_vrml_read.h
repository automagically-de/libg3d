#ifndef _IMP_VRML_READ_H
#define _IMP_VRML_READ_H

#include "imp_vrml2.h"
#include "imp_vrml_types.h"

typedef gboolean (* vrml_read_list_item_callback)(VrmlReader *reader,
	gpointer user_data);

gboolean vrml_read_global(VrmlReader *reader);
gboolean vrml_read_check_buffer(VrmlReader *reader);
gboolean vrml_read_skip_ws(VrmlReader *reader);
gboolean vrml_read_skip_unknown(VrmlReader *reader);
gboolean vrml_read_expect(VrmlReader *reader, const gchar c);
gboolean vrml_read_list(VrmlReader *reader, vrml_read_list_item_callback cb,
	gpointer user_data);

gchar *vrml_read_token(VrmlReader *reader);
gchar *vrml_read_numeric(VrmlReader *reader);
gchar *vrml_read_id(VrmlReader *reader);
gchar *vrml_read_nodeid(VrmlReader *reader);
gchar *vrml_read_string(VrmlReader *reader);

VrmlProperty *vrml_read_property(VrmlReader *reader, guint32 level);
VrmlObject *vrml_read_object(VrmlReader *reader, guint32 level);

static inline void vrml_read_dec(VrmlReader *reader)
{
	reader->bufp ++;
	reader->bufsize --;
}

static gchar vrml_read_padding[] = "                                         ";

#endif /* _IMP_VRML_READ_H */
