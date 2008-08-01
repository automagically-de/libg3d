#ifndef _IMP_DXF_VARS_H
#define _IMP_DXF_VARS_H

#include <glib.h>

static gchar *dxf_vars_vector2d[] = {
	"$AXISUNIT",
	"$LIMMAX",
	"$LIMMIN",
	"$PLIMMIN",
	"$PLIMMAX",
	NULL
};

static gchar *dxf_vars_vector3d[] = {
	"$EXTMIN",
	"$EXTMAX",
	"$INSBASE",
	"$PEXTMAX",
	"$PEXTMIN",
	"$PINSBASE",
	"$PUCSORG",
	"$PUCSORGBACK",
	"$PUCSORGBOTTOM",
	"$PUCSORGFRONT",
	"$PUCSORGLEFT",
	"$PUCSORGRIGHT",
	"$PUCSORGTOP",
	"$PUCSXDIR",
	"$PUCSYDIR",
	"$UCSORG",
	"$UCSORGBACK",
	"$UCSORGBOTTOM",
	"$UCSORGFRONT",
	"$UCSORGLEFT",
	"$UCSORGRIGHT",
	"$UCSORGTOP",
	"$UCSXDIR",
	"$UCSYDIR",
	NULL
};

#endif
