/* $Id: object.h,v 1.1.2.2 2006/01/23 16:43:19 dahms Exp $ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005, 2006  Markus Dahms <mad@automagically.de>

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

#ifndef __G3D_OBJECT_H__
#define __G3D_OBJECT_H__

#include <g3d/types.h>

G_BEGIN_DECLS

void g3d_object_free(G3DObject *object);

gdouble g3d_object_radius(G3DObject *object);

gboolean g3d_object_scale(G3DObject *object, gfloat scale);

gboolean g3d_object_transform(G3DObject *object, gfloat *matrix);

G3DObject *g3d_object_duplicate(G3DObject *object);

gboolean g3d_object_optimize(G3DObject *object);

gboolean g3d_object_smooth(G3DObject *object);

gboolean g3d_object_merge(G3DObject *o1, G3DObject *o2);

G_END_DECLS

#endif /* __G3D_OBJECT_H__ */
