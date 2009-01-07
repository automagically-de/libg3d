/* $Id:$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2009  Markus Dahms <mad@automagically.de>

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
#ifndef _G3D_QUAT_H
#define _G3D_QUAT_H

#include <g3d/types.h>

gboolean g3d_quat_normalize(G3DQuat *q);

/**
 * g3d_quat_rotate:
 * @q: resulting quat
 * @axis: rotation axis
 * @angle: rotation angle
 *
 * Encode a rotation around an axis into quaternion.
 *
 * Returns: TRUE on success, FALSE else
 */
gboolean g3d_quat_rotate(G3DQuat *q, G3DVector *axis, G3DFloat angle);

/**
 * g3d_quat_add:
 * @qr: result quat
 * @q1: first quat
 * @q2: second quat
 *
 * Add two quats.
 *
 * Returns: TRUE on success, FALSE else
 */
gboolean g3d_quat_add(G3DQuat *qr, G3DQuat *q1, G3DQuat *q2);

gboolean g3d_quat_trackball(G3DQuat *q, G3DFloat x1, G3DFloat y1,
	G3DFloat x2, G3DFloat y2, G3DFloat r);

/**
 * g3d_quat_to_matrix:
 * @q: source quat
 * @matrix: resulting matrix
 *
 * Convert a quaternion to a transformation matrix.
 *
 * Returns: TRUE on success, FALSE else
 */
gboolean g3d_quat_to_matrix(G3DQuat *q, G3DMatrix *matrix);

#endif /* _G3D_QUAT_H */
