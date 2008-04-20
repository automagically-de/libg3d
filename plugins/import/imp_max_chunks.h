#ifndef _IMP_MAX_CHUNKS_H
#define _IMP_MAX_CHUNKS_H

#include <glib.h>

#include "imp_max_callbacks.h"

#define PINONE -1
#define PISOME -2

typedef struct {
	gint32 id;
	gint32 parentid;
	char *desc;
	MaxCallback callback;
} MaxChunk;

static MaxChunk max_cnt_chunks[] = {
	{ 0x001B, PISOME, "geometric object",          NULL },
	{ 0x08FE, 0x0017, "mesh",                      max_cb_0x001B_0x08FE },
	{ 0x08FE, 0x001B, "mesh",                      max_cb_0x001B_0x08FE },
	{ 0x2001, PINONE, "3ds MAX x.x root node",     NULL },
	{ 0x2003, PINONE, "3ds MAX x.x root node",     NULL },
	{ 0x2004, PINONE, "3ds MAX 3.x root node",     NULL },
	{ 0x2006, PINONE, "3ds MAX 4.x root node",     NULL },
	{ 0x2008, PINONE, "3ds MAX 5.x root node",     NULL },
	{ 0x2009, PINONE, "3ds MAX 6.x root node",     NULL },
	{ 0x200A, PINONE, "3ds MAX 7.x root node",     NULL },
	{ 0x200B, PINONE, "3ds MAX 8.x root node",     NULL },
	{ 0x2038, PINONE, "DLL directory entry",       NULL },
	{ 0x2040, PINONE, "Class directory entry",     NULL },
	{ 0x2100, PINONE, "class data entry",          NULL },
	{ 0x4000, 0x0005, "material group",            max_cb_0x0005_0x4000 },
	{ 0x4000, 0x0008, "material group",            max_cb_0x0005_0x4000 },
	{ 0x4000, 0x0009, "material group",            max_cb_0x0005_0x4000 },
	{ 0x4000, 0x000C, "material group",            max_cb_0x0005_0x4000 },

	{ PINONE, PISOME, NULL, NULL }
};

static MaxChunk max_chunks[] = {
	/* 0x0001 */
	{ 0x0005, 0x0001, "properties",                max_cb_0x0001_0x0005 },
	/* 0x0003 */
	{ 0x1230, 0x0003, "file name",                 max_cb_debug_wchars },
	{ 0x1240, 0x0003, "file type",                 max_cb_debug_wchars },
	/* 0x0010 */
	{ 0x0962, 0x0010, "name",                      max_cb_debug_wchars },
	/* 0x0013 */
	{ 0x1000, 0x0013, "text",                      max_cb_debug_string },
	/* 0x0022 */
	{ 0x0110, 0x0022, "text",                      max_cb_debug_wchars },
	/* 0x08FE */
	{ 0x0100, 0x08FE, "vertices",                  max_cb_0x08FE_0x0100 },
	{ 0x010A, 0x08FE, "faces",                     max_cb_0x08FE_0x010A },
	{ 0x0912, 0x08FE, "faces",                     max_cb_0x08FE_0x0912 },
	{ 0x0914, 0x08FE, "vertices",                  max_cb_0x08FE_0x0914 },
	/* 0x39BF */
	{ 0x0100, 0x39BF, "text",                      max_cb_debug_wchars },
	/* 0x4000 */
	{ 0x4001, 0x4000, "material name",             max_cb_0x4000_0x4001 },
	{ 0x4030, 0x4000, "material color",            max_cb_0x4000_0x4030 },

	{ PINONE, PISOME, NULL, NULL }
};

#endif /* _IMP_MAX_CHUNKS_H */
