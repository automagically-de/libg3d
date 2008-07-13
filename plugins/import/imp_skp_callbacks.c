#include <g3d/types.h>
#include <g3d/material.h>

#include "imp_skp.h"
#include "imp_skp_callbacks.h"

gboolean skp_cb_attribute_container(SkpGlobalData *global, SkpLocalData *local)
{
	guint16 w1;

	w1 = g3d_stream_read_int16_le(global->stream);
	if(w1 != 0x0000)
		g_debug("SKP: CAttributeContainer: 0x%04X", w1);

	return TRUE;
}

gboolean skp_cb_attribute_named(SkpGlobalData *global, SkpLocalData *local)
{
	guint32 x1;
	guint16 w1, w2, w3;
	guint8 u1, u2;
	gdouble d1;
	gchar *name, *val, *tmp;

	w1 = g3d_stream_read_int16_le(global->stream);
	w2 = g3d_stream_read_int16_le(global->stream);
	w3 = g3d_stream_read_int16_le(global->stream);

	name = skp_read_wchar(global->stream);
	while(name != NULL) {
		tmp = skp_read_wchar(global->stream);
		if(tmp != NULL) {
			/* got a section */
			g_debug("\t%s", name);
			g_free(name);
			name = tmp;
			continue;
		}
		u1 = g3d_stream_read_int8(global->stream);
		switch(u1) {
			case 0x00: /* end of CAttributeNamed */
				g_free(name);
				return TRUE;
				break;
			case 0x04: /* 32-bit */
				x1 = g3d_stream_read_int32_le(global->stream);
				g_debug("\t\t%-20s: 0x%08x", name, x1);
				break;
			case 0x06: /* double */
				d1 = g3d_stream_read_double_le(global->stream);
				g_debug("\t\t%-20s: %.4f", name, d1);
				break;
			case 0x07: /* boolean */
				u2 = g3d_stream_read_int8(global->stream);
				g_debug("\t\t%-20s: %s", name, (u2 ? "true" : "false"));
				break;
			case 0x09: /* end of section? */
				g3d_stream_seek(global->stream, 7, G_SEEK_CUR);
				g_debug("\t\t%-20s: EOS", name);
				break;
			case 0x0A: /* string */
				val = skp_read_wchar(global->stream);
				g_return_val_if_fail(val != NULL, FALSE);
				g_debug("\t\t%-20s: %s", name, val);
				g_free(val);
				break;
			default:
				g_warning(
					"SKP: CAttributeNamed: unknown attribute type: 0x%02X",
					u1);
				return FALSE;
				break;
		}

		g_free(name);
		name = skp_read_wchar(global->stream);
	}
	return TRUE;
}

gboolean skp_cb_material(SkpGlobalData *global, SkpLocalData *local)
{
	gchar *name, *tmp;
	guint8 u1, u2, u3;
	guint32 x1, type, size;
	gfloat r, g, b, a;
	G3DMaterial *material;

	g3d_stream_read_int16_le(global->stream);

	name = skp_read_wchar(global->stream);
	while(name) {
		tmp = NULL;
		size = 0;

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
#if DEBUG > 0
				g_debug(
					"\tmaterial: %-30s 0x%04x, "
					"color (%.1f, %.1f, %.1f)",
					name, type, material->r, material->g, material->b);
#endif
				break;

			case 0x0001: /* texture */
				u1 = g3d_stream_read_int8(global->stream);
				u2 = g3d_stream_read_int8(global->stream);
				u3 = g3d_stream_read_int8(global->stream);
				if(u3 == 0x80) {
					/* number of textures? */
					x1 = g3d_stream_read_int32_le(global->stream);
					if(x1 > 0) {
						size = g3d_stream_read_int32_le(global->stream);
						g3d_stream_seek(global->stream, size, G_SEEK_CUR);
					}
				} else {
					x1 = 0x0004;
				}
				switch(x1) {
					case 0x0000:
						g3d_stream_seek(global->stream, 12, G_SEEK_CUR);
						break;
					case 0x0001:
						g3d_stream_seek(global->stream, 20, G_SEEK_CUR);
						break;
					case 0x0002:
						g3d_stream_seek(global->stream, 16, G_SEEK_CUR);
						break;
					case 0x0004:
						g3d_stream_seek(global->stream, 16, G_SEEK_CUR);
						break;
					default:
						g3d_stream_seek(global->stream, 16, G_SEEK_CUR);
						g_debug("SKP: mat0001: x1=%x: "
							"%02X%02X %02X%02X %02X%02X %02X%02X", x1,
							g3d_stream_read_int8(global->stream),
							g3d_stream_read_int8(global->stream),
							g3d_stream_read_int8(global->stream),
							g3d_stream_read_int8(global->stream),
							g3d_stream_read_int8(global->stream),
							g3d_stream_read_int8(global->stream),
							g3d_stream_read_int8(global->stream),
							g3d_stream_read_int8(global->stream));
						break;
				}
				tmp = skp_read_wchar(global->stream);
				g3d_stream_seek(global->stream, 8, G_SEEK_CUR);
#if DEBUG > 0
				g_debug(
					"\tmaterial: %-30s 0x%04x, %02X, %02X, %02X, 0x%08x\n"
					"\t\ttexture (%d bytes, 0x%08x): %s",
					name, type, u1, u2, u3, x1, size,
					(guint32)g3d_stream_tell(global->stream), tmp);
#endif

				break;

			case 0x0101: /* texture */
				/* number of textures? */
				x1 = g3d_stream_read_int32_le(global->stream);
				size = g3d_stream_read_int32_le(global->stream);
				g3d_stream_seek(global->stream, size, G_SEEK_CUR);
				g3d_stream_seek(global->stream, 16, G_SEEK_CUR);
				tmp = skp_read_wchar(global->stream);
				g3d_stream_seek(global->stream, 8, G_SEEK_CUR);
#if DEBUG > 0
				g_debug(
					"\tmaterial: %-30s 0x%04x, 0x%08x\n"
					"\t\ttexture (%i bytes): %s",
					name, type, x1, size, tmp);
#endif
				break;

			default:
				g_debug("SKP: material: unknown type 0x%04X @ 0x%08x",
					type, (guint32)g3d_stream_tell(global->stream));
				return FALSE;
		}

		x1 = g3d_stream_read_int32_be(global->stream);
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
