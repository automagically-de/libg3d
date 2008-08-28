
#include "imp_blend_def.h"
#include "imp_blend_sdna.h"
#include "imp_blend_read.h"

BlendSdna *blend_sdna_read_dna1(G3DStream *stream, guint32 flags, gint32 len)
{
	BlendSdna *sdna;
	guint32 code, n, n2, nn, m, t;
	gint32 i, j;
	gchar buf[2048], *bufp, c, **aptr;

	code = blend_read_uint(stream, flags);
	len -= 4;
	if(code != MKID('S','D','N','A')) {
		g_warning("Blend: DNA1: no SDNA");
		g3d_stream_skip(stream, len);
		return NULL;
	}

	sdna = g_new0(BlendSdna, 1);

	while(len > 0) {
		code = blend_read_uint(stream, flags);
		len -= 4;

		g_debug("\\ %c%c%c%c:",
			blend_from_id(code, 0), blend_from_id(code, 1),
			blend_from_id(code, 2), blend_from_id(code, 3));

		switch(code) {
			case MKID('N','A','M','E'):
			case MKID('T','Y','P','E'):
				n = blend_read_uint(stream, flags);
				len -= 4;
				if(code == MKID('T','Y','P','E')) {
					sdna->n_types = n;
					sdna->type_names = g_new0(gchar *, n);
					sdna->type_sizes = g_new0(guint32, n);
					aptr = sdna->type_names;
				} else {
					sdna->n_names = n;
					sdna->names = g_new0(gchar *, n);
					aptr = sdna->names;
				}
				m = 0;
				for(i = 0; i < n; i ++) {
					bufp = buf;
					do {
						c = *bufp = g3d_stream_read_int8(stream);
						bufp ++;
						len --;
						m ++;
					} while(c != '\0');
					aptr[i] = g_strdup(buf);
#if DEBUG > 2
					g_debug("\\  %s", buf);
#endif
				}
				m %= 4;
				if(m > 0) {
					/* padding */
					g3d_stream_skip(stream, 4 - m);
					len -= (4 - m);
				}
				break;
			case MKID('T','L','E','N'):
				for(i = 0; i < sdna->n_types; i ++) {
					m = blend_read_short(stream, flags);
					len -= 2;
#if DEBUG > 2
					g_debug("\\  %d", m);
#endif
					sdna->type_sizes[i] = m;
				}
				m = sdna->n_types % 2;
				if(m > 0) {
					g3d_stream_skip(stream, 2);
					len -= 2;
				}
				g_debug("len: %d @ 0x%08x", len,
					(guint32)g3d_stream_tell(stream));
				break;
			case MKID('S','T','R','C'):
				n = blend_read_uint(stream, flags);
				len -= 4;
				for(i = 0; i < n; i ++) {
					t = blend_read_short(stream, flags);
					n2 = blend_read_short(stream, flags);
					len -= 4;
					g_debug("struct %s { /* %d */",
						sdna->type_names[i + 10], sdna->type_sizes[i + 10]);
					for(j = 0; j < n2; j ++) {
						t = blend_read_short(stream, flags);
						nn = blend_read_short(stream, flags);
						len -= 4;
						g_debug("\t%-16s %-24s; /* %d */",
							sdna->type_names[t], sdna->names[nn],
							sdna->type_sizes[t]);
					}
					g_debug("};");
				}
				break;
			default:
				g_warning("Blend: DNA1: unexpected section 0x%x", code);
				return sdna;
				break;
		}
	}
	return sdna;
}

