#ifndef _IMP_BLEND_DEF_H
#define _IMP_BLEND_DEF_H

#include <glib.h>

#define FLAG_LITTLE_ENDIAN   1
#define FLAG_POINTERSIZE_4   4
#define FLAG_POINTERSIZE_8   8
#define FLAG_POINTER_MASK  0xC

#define MKID(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

static inline gchar blend_from_id(guint32 id, guint8 pos) {
	gchar c = ((id >> (pos * 8)) & 0xFF);
	return ((c < 0x20) || (c > 0x7F)) ? '_' : c;
}

#endif /* _IMP_BLEND_DEF_H */
