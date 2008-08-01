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
#ifndef _IMP_LWO_CALLBACKS_H
#define _IMP_LWO_CALLBACKS_H

gboolean lwo_cb_CLIP(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_COLR(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_IMAG(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_PNTS(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_POLS(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_PTAG(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_SPEC(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_SRFS(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_STIL(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_SURF(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_TAGS(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_TRAN(g3d_iff_gdata *global, g3d_iff_ldata *local);
gboolean lwo_cb_VMAP(g3d_iff_gdata *global, g3d_iff_ldata *local);

#endif
