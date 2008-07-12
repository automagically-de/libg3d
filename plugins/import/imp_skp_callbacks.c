#include <g3d/types.h>
#include <g3d/material.h>

#include "imp_skp.h"
#include "imp_skp_callbacks.h"

gboolean skp_cb_material(SkpGlobalData *global, SkpLocalData *local)
{
	gchar *name, *tmp;
	guint32 x1, type, size;
	gfloat r, g, b, a;
	G3DMaterial *material;

	g3d_stream_read_int16_le(global->stream);

	name = skp_read_wchar(global->stream);
	while(name) {
		tmp = NULL;

		material = g3d_material_new();
		material->name = g_strdup(name);
		global->model->materials = g_slist_append(global->model->materials,
			material);

		type = g3d_stream_read_int16_le(global->stream);
		switch(type) {
			case 0x0000: /* simple color */
				r = g3d_stream_read_int8(global->stream);
				g = g3d_stream_read_int8(global->stream);
				b = g3d_stream_read_int8(global->stream);
				a = g3d_stream_read_int8(global->stream);

				material->r = r / 255.0;
				material->g = g / 255.0;
				material->b = b / 255.0;
				material->a = a / 255.0;
				break;

			case 0x0001: /* texture */
				g3d_stream_read_int8(global->stream);
				g3d_stream_read_int8(global->stream);
				g3d_stream_read_int8(global->stream);
			case 0x0101: /* texture */
				/* number of textures? */
				x1 = g3d_stream_read_int32_le(global->stream);
				size = g3d_stream_read_int32_le(global->stream);
				g3d_stream_seek(global->stream, size, G_SEEK_CUR);
				g3d_stream_seek(global->stream, 16, G_SEEK_CUR);
				tmp = skp_read_wchar(global->stream);
				g3d_stream_seek(global->stream, 8, G_SEEK_CUR);
				break;

			default:
				g_debug("SKP: material: unknown type 0x%04X @ 0x%08x",
					type, (guint32)g3d_stream_tell(global->stream));
				return FALSE;
		}

		x1 = g3d_stream_read_int32_be(global->stream);
		g_debug("SKP: material: %-30s 0x%04x, 0x%08x, '%s'",
			name, type, x1, tmp ? tmp : "(null)");

		switch(x1 & 0x00FFL) {
			case 0x00:
				g3d_stream_seek(global->stream, 21, G_SEEK_CUR);
				break;
			case 0xFF:
				g3d_stream_seek(global->stream, 22, G_SEEK_CUR);
				break;
			default:
				g_debug("x1: 0x%08x", x1);
				break;
		}

		/* clean up */
		if(tmp)
			g_free(tmp);
		g_free(name);

		/* next one */
		name = skp_read_wchar(global->stream);
	}
	return TRUE;
}

gboolean skp_cb_vertex(SkpGlobalData *global, SkpLocalData *local)
{
	g3d_stream_read_int16_le(global->stream);

	return FALSE;
}
