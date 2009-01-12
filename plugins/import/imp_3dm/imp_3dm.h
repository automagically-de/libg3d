#ifndef _IMP_3DM_H
#define _IMP_3DM_H

#include <g3d/types.h>

typedef struct {
	G3DContext *context;
	G3DStream *stream;
	G3DModel *model;
} TdmGlobal;

#define TCODE_TABLE      0x10000000
#define TCODE_DATA       0x80000000
#define TCODE_CRC        0x00008000

#define TCODE_ENDOFTABLE 0xFFFFFFFF

#endif /* _IMP_3DM_H */
