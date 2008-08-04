#ifndef _IMP_ACF_WING_H
#define _IMP_ACF_WING_H

#include <g3d/types.h>

#include "imp_acf_airfoil.h"

G3DObject *acf_wing(G3DMaterial *material, const gchar *name,
	gfloat *msweep, gfloat *mdihed, gfloat *mtrans,
	gfloat *vrt, gfloat *vtp,
	AcfAirfoil *aflrt, AcfAirfoil *afltp,
	gfloat wrt, gfloat wtp, gfloat lf);

#endif
