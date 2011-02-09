#include <math.h>

#include <g3d/g3d.h>
#include <g3d/vector.h>

struct test_vector_x3 {
	G3DVector a[3];
	G3DVector b[3];
	G3DVector r[3];
};

struct test_vector_x2f {
	G3DVector a[3];
	G3DVector b[3];
	G3DFloat r;
};

static gboolean test_vector_angle(void);

int main(int argc, char *argv[])
{
	test_vector_angle();	

	return 0;
}

static gboolean test_float_equals(G3DFloat f1, G3DFloat f2)
{
	return (fabs(f1 - f2) < 0.0001f);
}


struct test_vector_x2f angles[] = {
	{ { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }, 0.0 },
	{ { 0.0, 1.0, 0.0 }, { 0.0, -1.0, 0.0 }, G_PI },
	{ { 0.0, 1.0, 0.0 }, { 0.0, 0.0, 1.0 }, G_PI / 2.0 }
};
static int n_angles = G_N_ELEMENTS(angles);

static gboolean test_vector_angle(void)
{
	gint32 i;
	G3DFloat a;

	for(i = 0; i < n_angles; i ++) {
		a = g3d_vector_angle(angles[i].a, angles[i].b);
		if(test_float_equals(a, angles[i].r)) {
			g_print("OK\n");
		} else {
			g_print("FAILED (%f != %f)\n", a, angles[i].r);
		}
	}
	return TRUE;
}
