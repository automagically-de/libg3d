#ifndef _IMP_DAE_LIBRARY_H
#define _IMP_DAE_LIBRARY_H

#include <glib.h>
#include <libxml/tree.h>

typedef struct _DaeLibrary DaeLibrary;

DaeLibrary *dae_library_load(xmlDocPtr xmldoc);
xmlNodePtr dae_library_lookup(DaeLibrary *library, const gchar *tagname,
	const gchar *id);
void dae_library_cleanup(DaeLibrary *library);

#endif /* _IMP_DAE_LIBRARY_H */
