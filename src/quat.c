#include <g3d/types.h>
#include <g3d/matrix.h>

gboolean g3d_quat_to_matrix(G3DQuat *q, G3DMatrix *matrix)
{
	g3d_matrix_identity(matrix);

	matrix[0 * 4 + 0] = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
	matrix[0 * 4 + 1] = 2.0 * (q[0] * q[1] - q[2] * q[3]);
	matrix[0 * 4 + 2] = 2.0 * (q[2] * q[0] + q[1] * q[3]);

	matrix[1 * 4 + 0] = 2.0 * (q[0] * q[1] + q[2] * q[3]);
	matrix[1 * 4 + 1] = 1.0 - 2.0 * (q[2] * q[2] + q[0] * q[0]);
	matrix[1 * 4 + 2] = 2.0 * (q[1] * q[2] - q[0] * q[3]);

	matrix[2 * 4 + 0] = 2.0 * (q[2] * q[0] - q[1] * q[3]);
	matrix[2 * 4 + 1] = 2.0 * (q[1] * q[2] + q[0] * q[3]);
	matrix[2 * 4 + 2] = 1.0 - 2.0 * (q[1] * q[1] + q[0] * q[0]);

	return TRUE;
}
