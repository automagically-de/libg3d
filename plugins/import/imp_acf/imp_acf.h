#ifndef _IMP_ACF_H
#define _IMP_ACF_H

#include <g3d/types.h>

#include "imp_acf_def.h"

typedef struct {
	G3DContext *context;
	G3DModel *model;
	AcfFile *acf;
} AcfGlobalData;

#endif /* _IMP_ACF_H */
