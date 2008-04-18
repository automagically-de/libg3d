#include <string.h>
#include <g3d/stream.h>

#include "imp_max_callbacks.h"

gboolean max_cb_0x0005(MaxGlobalData *global, MaxLocalData *local)
{
	gchar *str;
	gint32 n_str, len, i, w3[3], cnt = 0;

	n_str = g3d_stream_read_int16_le(global->stream);
	local->nb -= 2;

	if(local->nb == 0)
		return FALSE;

	/* flags? */
	g3d_stream_read_int16_le(global->stream);
	local->nb -= 2;

#if DEBUG > 0
	g_debug("\\%s[LSTR] %d strings",
		(global->padding + (strlen(global->padding) - local->level)), n_str);
#endif

	while(local->nb > 0) {
		len = g3d_stream_read_int32_le(global->stream);
		local->nb -= 4;
		str = g_malloc0(len + 1);
		g3d_stream_read(global->stream, str, 1, len);
		local->nb -= len;
		for(i = 0; i < 3; i ++)
			w3[i] = g3d_stream_read_int16_le(global->stream);
		local->nb -= 6;
		cnt ++;
#if DEBUG > 0
		g_debug("\\%s[LSTR]  %04d: '%s' (%d bytes) [%d, %d, %d]",
			(global->padding + (strlen(global->padding) - local->level)),
			cnt, str, len,
			w3[0], w3[1], w3[2]);
#endif
		g_free(str);
	}
	return TRUE;
}

gboolean max_cb_0x0100(MaxGlobalData *global, MaxLocalData *local)
{
	gint32 i = 0;
	gchar *text;

	text = g_malloc0(local->nb / 2 + 1);

	for(i = 0; i < local->nb; i ++) {
		if((i % 2) == 0)
			text[i / 2] = g3d_stream_read_int8(global->stream);
		else
			g3d_stream_read_int8(global->stream);
	}
	local->nb = 0;

#if DEBUG > 0
	g_debug("\\%s[TEXT] %s",
		(global->padding + (strlen(global->padding) - local->level)),
		text);
#endif

	g_free(text);
	return TRUE;
}
