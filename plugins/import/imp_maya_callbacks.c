#include <g3d/iff.h>
#include <g3d/read.h>

gboolean maya_cb_MESH(g3d_iff_gdata *global, g3d_iff_ldata *local)
{
	gint32 x1, x2, x3, x4;

	x1 = g3d_read_int16_be(global->f);
	x2 = g3d_read_int16_be(global->f);
	x3 = g3d_read_int16_be(global->f);
	x4 = g3d_read_int16_be(global->f);
	local->nb -= 8;

#if DEBUG > 0
	g_printerr("[Maya][MESH] %d %d %d %d\n", x1, x2, x3, x4);
#endif

	return TRUE;
}
