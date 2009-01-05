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
