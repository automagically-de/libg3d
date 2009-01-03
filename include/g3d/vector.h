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

#ifndef __G3D_VECTOR_H__
#define __G3D_VECTOR_H__

/**
 * SECTION:vector
 * @short_description: Vector manipulation and calculation
 * @include: g3d/vector.h
 */

#include <g3d/types.h>

G_BEGIN_DECLS

/**
 * g3d_vector_new:
 * @size: number of items in one vector
 * @n: number of vectors to allocate
 *
 * Allocate memory for a number of vectors.
 *
 * Returns: newly allocated vectors
 */
static inline G3DVector *g3d_vector_new(guint32 size, guint32 n) {
	return g_new0(G3DVector, size * n);
}

/**
 * g3d_vector_free:
 * @vector: vector to free
 *
 * Free memory allocated for vector.
 */
static inline void g3d_vector_free(G3DVector *vector) {
	g_free(vector);
}

/**
 * g3d_vector_normal:
 * @ax: x component first vector
 * @ay: y component first vector
 * @az: z component first vector
 * @bx: x component second vector
 * @by: y component second vector
 * @bz: z component second vector
 * @nx: x component resulting normal
 * @ny: y component resulting normal
 * @nz: z component resulting normal
 *
 * calculate the normal from a plane defined by two vectors
 *
 * Returns: TRUE on success, FALSE else
 */
gboolean g3d_vector_normal(G3DFloat ax, G3DFloat ay, G3DFloat az,
	G3DFloat bx, G3DFloat by, G3DFloat bz,
	G3DFloat *nx, G3DFloat *ny, G3DFloat *nz);

/**
 * g3d_vector_unify:
 * @nx: x component of vector
 * @ny: y component of vector
 * @nz: z component of vector
 *
 * Transforms the given vector to the unit vector.
 *
 * Returns: TRUE on success, FALSE else
 */
gboolean g3d_vector_unify(G3DFloat *nx, G3DFloat *ny, G3DFloat *nz);

/**
 * g3d_vector_transform:
 * @x: x component of vector
 * @y: y component of vector
 * @z: z component of vector
 * @matrix: transformation matrix (4x4)
 *
 * Transforms the given vector corresponding to the given matrix
 *
 * Returns: TRUE on success, FALSE else
 */
gboolean g3d_vector_transform(G3DFloat *x, G3DFloat *y, G3DFloat *z,
	G3DMatrix *matrix);

G_END_DECLS

#endif /* __G3D_VECTOR_H__ */
