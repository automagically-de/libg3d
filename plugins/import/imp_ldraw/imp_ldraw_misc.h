#ifndef _IMP_LDRAW_MISC_H
#define _IMP_LDRAW_MISC_H

#include <string.h>
#include <glib.h>

static inline gchar *path_sep(const gchar *text)
{
	gchar *p = strchr(text, '/');
	if(p != NULL)
		return p;
	return strchr(text, '\\');
}

#endif
