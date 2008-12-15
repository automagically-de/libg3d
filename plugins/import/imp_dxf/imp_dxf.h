#ifndef _IMP_DXF_H
#define _IMP_DXF_H

#include <g3d/types.h>
#include <g3d/stream.h>

#include "imp_dxf_types.h"

#define DXF_TEST_ENDSEC(str) \
	if(strcmp((str), "ENDSEC") == 0) return TRUE;

#define DXF_HANDLE_UNKNOWN(global, key, str, section) \
	if(global->binary) { \
		g_warning("imp_dxf: unhandled key %d in section %s @ %#x", \
			key, section, (guint32)g3d_stream_tell(global->stream)); \
		return FALSE; \
	} else { \
		g3d_stream_read_line(global->stream, str, DXF_MAX_LINE); \
		g_debug(\
			"imp_dxf: skipping line %d (section %s, key %d):\n\t\"%s\"\n", \
			g3d_stream_line(global->stream), section, key, str); \
	}

gint32 dxf_read_section(DxfGlobalData *global, G3DObject *object);
gint32 dxf_read_code(DxfGlobalData *global);
gchar *dxf_read_string(DxfGlobalData *global, gchar *value);
gint32 dxf_read_int16(DxfGlobalData *global);
gint32 dxf_read_int32(DxfGlobalData *global);
gdouble dxf_read_float64(DxfGlobalData *global);
gboolean dxf_skip_section(DxfGlobalData *global);

#endif
