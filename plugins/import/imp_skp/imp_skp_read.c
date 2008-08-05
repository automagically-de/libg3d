#include "imp_skp.h"
#include "imp_skp_read.h"

guint32 skp_read_xint16(G3DStream *stream)
{
	guint32 val;

	val = g3d_stream_read_int16_le(stream);
	if(val & 0x8000L) {
		val &= 0x7FFF;
		val |= (g3d_stream_read_int16_le(stream) << 16);
	}
	return val;
}

gchar *skp_read_char(G3DStream *stream)
{
	guint32 magic, n;
	gchar *text;

	magic = g3d_stream_read_int32_be(stream);
	if(magic != 0xffff0000) {
		g_warning("SKP: wrong text magic: 0x%08x", magic);
		return NULL;
	}
	n = g3d_stream_read_int16_le(stream);

	text = g_new0(gchar, n + 1);
	g3d_stream_read(stream, text, 1, n);

	return text;
}

gchar *skp_read_wchar(G3DStream *stream)
{
	gint32 i;
	guint32 magic, n;
	gunichar2 *u16text;
	gchar *text;
	GError *error = NULL;

	magic = g3d_stream_read_int32_be(stream);
	if((magic & 0xFFFFFF00) != 0xfffeff00) {
#if DEBUG > 1
		g_debug("SKP: wrong UTF-16 magic: 0x%08x", magic);
#endif
		g3d_stream_seek(stream, -4, G_SEEK_CUR);
		return NULL;
	}
	n = magic & 0x000000FF;

	u16text = g_new0(gunichar2, n + 1);
	for(i = 0; i < n; i ++) {
		u16text[i] = g3d_stream_read_int16_le(stream);
	}

	text = g_utf16_to_utf8(u16text, n, NULL, NULL, &error);
	if(error != NULL) {
		g_warning("UTF-16 to UTF-8 conversion failed: %s",
			error->message);
		g_error_free(error);
	}
	g_free(u16text);

	return text;
}

gboolean skp_read_dbl3(G3DStream *stream,
	gdouble *d1, gdouble *d2, gdouble *d3)
{
	*d1 = g3d_stream_read_double_le(stream);
	*d2 = g3d_stream_read_double_le(stream);
	*d3 = g3d_stream_read_double_le(stream);
	return TRUE;
}

gboolean skp_read_10b(G3DStream *stream)
{
	guint32 x1;
	guint8 u1;

	x1 = g3d_stream_read_int32_be(stream);
	u1 = g3d_stream_read_int8(stream);

	if(((x1 & 0x00FFFFFF) != 0x0001) || (u1 != 0x01)) {
		g_warning("skp_read_10b: %#08x, %#02x", x1, u1);
	}
	u1 = g3d_stream_read_int8(stream);
	x1 = g3d_stream_read_int32_le(stream);
	return TRUE;
}

