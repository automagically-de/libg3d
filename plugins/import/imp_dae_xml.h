#ifndef _IMP_DAE_XML_H
#define _IMP_DAE_XML_H

#include <glib.h>
#include <libxml/tree.h>

xmlNodePtr dae_xml_get_child_by_tagname(xmlNodePtr parent, const gchar *tag);

#endif /* _IMP_DAE_XML_H */
