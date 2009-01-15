#include "imp_blend_types.h"
#include "imp_blend_def.h"
#include "imp_blend_sdna.h"

gboolean blend_cb_DNA1(BlendGlobal *global, BlendLocal *local)
{
	if(global->sdna != NULL)
		return TRUE;
	global->sdna = blend_sdna_read_dna1(global->stream,
		global->flags, local->len);
	if(global->sdna == NULL) {
		g_warning("Blend: failed to read DNA1, giving up...");
		return FALSE;
	}
	/* rewind stream to really read content */
	g3d_stream_seek(global->stream, BLEND_HEADER_SIZE, G_SEEK_SET);
	local->len = 0;
	return TRUE;
}
