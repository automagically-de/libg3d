#ifndef _IMP_ACF_H
#define _IMP_ACF_H

#include <g3d/types.h>
#include <g3d/stream.h>

#include "imp_acf_def.h"
#include "imp_acf_airfoil.h"

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	AcfAirfoilDb *afldb;
	AcfFile *acf;
} AcfGlobalData;

#endif /* _IMP_ACF_H */
