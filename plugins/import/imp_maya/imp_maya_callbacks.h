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
#ifndef _IMP_MAYA_CALLBACKS_H
#define _IMP_MAYA_CALLBACKS_H

#include <g3d/iff.h>

gboolean maya_cb_CMPD(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_CREA(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_CWFL(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_DBLn(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_DBL2(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_DBL3(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_DBLE(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_DMSH(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_FLT3(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_MATR(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_MESH(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_PCUB(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_PCYL(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_STR_(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean maya_cb_XFRM(g3d_iff_gdata *global, g3d_iff_ldata *local);

#endif /* _IMP_MAYA_CALLBACKS_H */
