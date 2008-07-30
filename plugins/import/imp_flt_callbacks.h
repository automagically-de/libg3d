/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2008  Markus Dahms <mad@automagically.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _IMP_FLT_CALLBACKS_H
#define _IMP_FLT_CALLBACKS_H

#include <stdio.h>
#include <glib.h>
#include <g3d/types.h>

typedef struct {
	guint32 n_entries;
	goffset *offsets;                /* n * goffset */
	gfloat *vertex_data;             /* 3 x n * gfloat */
	gfloat *normal_data;             /* 3 x n * gfloat */
	gfloat *tex_vertex_data;         /* 2 x n * gfloat */
	G3DMaterial **vertex_materials;  /* n * G3DMaterial* */
	goffset offset;
} FltVertexPalette;

typedef struct {
	G3DContext *context;
	G3DModel *model;
	FILE *f;
	guint32 level;
	GQueue *oqueue;
	FltVertexPalette *vertex_palette;
} FltGlobalData;

typedef struct {
	guint32 opcode;
	G3DObject *g3dobj;
	gpointer level_object;
	gint32 nb;
} FltLocalData;

typedef gboolean (*FltCallbackFunc)(FltGlobalData *gd, FltLocalData *ld);

/* callback functions */
gboolean flt_cb_0002(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0004(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0005(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0010(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0011(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0032(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0067(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0068(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0069(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0070(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0071(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0072(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0085(FltGlobalData *gd, FltLocalData *ld);
gboolean flt_cb_0086(FltGlobalData *gd, FltLocalData *ld);

#endif /* _IMP_FLT_CALLBACKS_H */
