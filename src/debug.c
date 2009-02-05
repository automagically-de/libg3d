#include <math.h>
#include <string.h>
#include <ctype.h>

#include <g3d/debug.h>

void debug_hexdump(guint8 *data, gsize len)
{
	static const guint32 npr = 16;
	guint32 nrows;
	gint i, j;
	gchar t[80], s[80], c;

	s[79] = '\0';
	nrows = (guint32)(ceil((gfloat)len / (gfloat)npr));
	for(i = 0; i < nrows; i ++) {
		memset(s, ' ', 78);
		g_snprintf(t, 10, "%08x:", i * npr);
		memcpy(s, t, 9);
		for(j = 0; j < MIN(npr, len - (i * npr)); j ++) {
			c = data[i * npr + j];
			g_snprintf(t, 3, "%02x", c);
			memcpy(s + 11 + j * 3, t, 2);
			s[60 + j] = isprint(c) ? c : '.';
		}
		g_debug("%s", s);
	}
}
