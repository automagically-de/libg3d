#ifndef _IMP_DXF_H
#define _IMP_DXF_H

#include <g3d/types.h>
#include <g3d/stream.h>

typedef struct {
	G3DContext *context;
	G3DModel *model;
	G3DStream *stream;
	gboolean binary;
} DxfGlobalData;

typedef struct {
	G3DObject *object;
	G3DMaterial *material;
	G3DFace *face;
	guint32 vertex_offset;
	guint32 polyline_flags;
	gint32 tmp_70, tmp_71;
	gint32 tmp_i1;
} DxfEntityData;

#define DXF_MAX_LINE 512
#define DXF_CODE_INVALID 0xDEADBEEF

#define DXF_ID_HEADER	0x00FF0000
#define DXF_ID_TABLES	0x00FF0001
#define DXF_ID_ENTITIES	0x00FF0002
#define DXF_ID_BLOCKS   0x00FF0003
#define DXF_ID_OBJECTS  0x00FF0004
#define DXF_ID_CLASSES  0x00FF0005

#define DXF_POLY_CLOSED             1
#define DXF_POLY_CURVE_FIT_ADDED    2
#define DXF_POLY_SPLINE_FIT_ADDED   4
#define DXF_POLY_3D_POLYLINE        8
#define DXF_POLY_3D_POLYMESH       16
#define DXF_POLY_N_CLOSED          32
#define DXF_POLY_POLYFACE          64
#define DXF_POLY_LT_PATTERN       128

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
