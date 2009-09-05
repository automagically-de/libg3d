/* $Id$ */

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

#include <math.h>
#include <g3d/types.h>

void g3d_vector_copy(G3DVector *vdst, G3DVector *vsrc)
{
	gint32 i;
	for(i = 0; i < 3; i ++) {
		vdst[i] = vsrc[i];
	}
}

void g3d_vector_cross(G3DVector *a, G3DVector *b, G3DVector *r)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

/* deprecated: use g3d_vector_cross */
gboolean g3d_vector_normal(G3DVector ax, G3DVector ay, G3DVector az,
	G3DVector bx, G3DVector by, G3DVector bz,
	G3DVector *nx, G3DVector *ny, G3DVector *nz)
{
	*nx = ay * bz - az * by;
	*ny = az * bx - ax * bz;
	*nz = ax * by - ay * bx;

	return TRUE;
}

G3DFloat g3d_vector_length(G3DVector *v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

/* deprecated: use g3d_vector_unitize() */
gboolean g3d_vector_unify(G3DVector *nx, G3DVector *ny, G3DVector *nz)
{
	G3DFloat r;

	r = sqrt(*nx * *nx + *ny * *ny + *nz * *nz);
	if(r == 0.0F)
		*nx = *ny = *nz = 0.0F;
	else {
		*nx /= r;
		*ny /= r;
		*nz /= r;
	}

	return TRUE;
}

void g3d_vector_unitize(G3DVector *v)
{
	G3DFloat r;

	r = g3d_vector_length(v);
	if(r == 0.0F) {
		v[0] = v[1] = v[2] = 0.0F;
	} else {
		v[0] /= r;
		v[1] /= r;
		v[2] /= r;
	}
}

gboolean g3d_vector_transform(G3DVector *x, G3DVector *y, G3DVector *z,
	G3DMatrix *matrix)
{
	G3DVector vector[4], result[4];
	guint32 i, k;

	vector[0] = *x;
	vector[1] = *y;
	vector[2] = *z;
	vector[3] = 1.0;

	for(i = 0; i < 4; i ++) {
		result[i] = 0.0;

		for(k = 0; k < 4; k ++)
#if 0
			result[i] += matrix[i * 4 + k] * vector[k];
#else
			result[i] += matrix[k * 4 + i] * vector[k];
#endif
	}

	*x = result[0];
	*y = result[1];
	*z = result[2];

	return TRUE;
}

gboolean g3d_vector_from_spherical(G3DFloat lat, G3DFloat lon, G3DFloat r,
	G3DVector *rv)
{
	G3DDouble rlat, rlon;

	rlon = lon * G_PI / 180.0;
	rlat = (90 - lat) * G_PI / 180.0;
	rv[0] = r * cos(rlon) * sin(rlat);
	rv[1] = r * sin(rlon) * sin(rlat);
	rv[2] = r * cos(rlat);

	return TRUE;
}

G3DFloat g3d_vector_dot(G3DVector *v1, G3DVector *v2)
{
	G3DFloat result = 0.0;
	gint32 i;
	
	for(i = 0; i < 3; i ++)
		result += v1[i] * v2[i];

	return result;
}

G3DFloat g3d_vector_angle(G3DVector *v1, G3DVector *v2)
{
	G3DVector uv1[3], uv2[3];

	g3d_vector_copy(uv1, v1);
	g3d_vector_copy(uv2, v2);

	g3d_vector_unitize(uv1);
	g3d_vector_unitize(uv2);

	return acos(g3d_vector_dot(v1, v2));
}

