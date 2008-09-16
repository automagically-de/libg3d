/* $Id$ */

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

#ifndef __G3D_PRIMITIVE_H__
#define __G3D_PRIMITIVE_H__

/**
 * SECTION:primitive
 * @short_description: 3D primitive generation functions
 * @include: g3d/primitive.h
 *
 * Primitives are objects containing basic 3D geometrical structures. A
 * variety of them can be created using these functions.
 */

#include <g3d/types.h>

G_BEGIN_DECLS

/**
 * g3d_primitive_box:
 * @width: the width of the box
 * @height: the height of the box
 * @depth: the depth of the box
 * @material: the material to use for all faces
 *
 * Generates an object containing a box.
 *
 * Returns: the box object
 */

G3DObject *g3d_primitive_box(gfloat width, gfloat height, gfloat depth,
	G3DMaterial *material);

#ifndef G3D_DISABLE_DEPRECATED
/**
 * g3d_primitive_cube:
 * @width: the width of the box
 * @height: the height of the box
 * @depth: the depth of the box
 * @material: the material to use for all faces
 *
 * Generates an object containing a box. It is deprecated and now a wrapper for
 * g3d_primitive_box().
 *
 * Returns: the box object
 */
G3DObject *g3d_primitive_cube(gfloat width, gfloat height, gfloat depth,
	G3DMaterial *material);
#endif

/**
 * g3d_primitive_cylinder:
 * @radius: the radius of the cylinder
 * @height: the height of the side faces
 * @sides: number of side faces (number of circle segments)
 * @top: add top faces
 * @bottom: add bottom faces
 * @material: material to use for faces
 *
 * Generates an object containing a cylinder.
 *
 * Returns: cylinder object
 */
G3DObject *g3d_primitive_cylinder(gfloat radius, gfloat height,
	guint32 sides, gboolean top, gboolean bottom, G3DMaterial *material);

/**
 * g3d_primitive_tube:
 * @r_in: inner radius
 * @r_out: outer radius
 * @height: the height of the side faces
 * @sides: number of side faces (number of circle segments)
 * @top: add top faces
 * @bottom: add bottom faces
 * @material: material to use for faces
 *
 * Generates an object containing a tube (a cylinder with a hole).
 *
 * Returns: tube object
 */
G3DObject *g3d_primitive_tube(gfloat r_in, gfloat r_out, gfloat height,
	guint32 sides, gboolean top, gboolean bottom, G3DMaterial *material);

/**
 * g3d_primitive_sphere:
 * @radius: radius
 * @vseg: number of vertical segments
 * @hseg: number of horizontal segments
 * @material: material to use for faces
 *
 * Generates an object containing a sphere.
 *
 * Returns: sphere object
 */
G3DObject *g3d_primitive_sphere(gfloat radius, guint32 vseg, guint32 hseg,
	G3DMaterial *material);

/**
 * g3d_primitive_box_strip_2d:
 * @vcnt: number of control points
 * @vdata: 2-dimensional control point data (2 * vcnt * gdouble)
 * @height: height of resulting strip (y component)
 * @width: width of strip (corner diameter)
 * @material: material to use for faces
 *
 * Generates a strip of box segments defined by corner center points
 * using two-dimensional data (x/z plane).
 *
 * Returns: strip object
 */
G3DObject *g3d_primitive_box_strip_2d(guint32 vcnt, gdouble *vdata,
	gdouble height, gdouble width, G3DMaterial *material);

G_END_DECLS

#endif /* __G3D_PRIMITIVE_H__ */
