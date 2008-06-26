#ifndef _IMP_DAE_XML_H
#define _IMP_DAE_XML_H

#include <glib.h>
#include <libxml/tree.h>

#include "imp_dae_library.h"
#include "imp_dae_cb.h"

gchar *dae_xml_get_attr(xmlNodePtr node, const gchar *attrname);
xmlNodePtr dae_xml_get_child_by_tagname(xmlNodePtr parent, const gchar *tag);
xmlNodePtr dae_xml_next_child(DaeLibrary *lib, xmlNodePtr parent,
	xmlNodePtr *node, xmlNodePtr *instance, gchar **nodename);
gboolean dae_xml_parse(DaeGlobalData *global, xmlNodePtr parent,
	DaeChunkDesc *chunks, gpointer user_data);

#endif /* _IMP_DAE_XML_H */
