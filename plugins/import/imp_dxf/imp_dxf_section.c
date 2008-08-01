#include <string.h>

#include "imp_dxf.h"
#include "imp_dxf_vars.h"

static gboolean dxf_str_in_array(gchar **array, const gchar *needle)
{
	gchar **p = array;
	while(*p != NULL) {
		if(strcmp(*p, needle) == 0)
			return TRUE;
		p ++;
	}
	return FALSE;
}

#define DXF_VAR_DEBUG 2

static gboolean dxf_read_variable(DxfGlobalData *global)
{
	gint32 key;
	gint16 i16;
	gchar str[DXF_MAX_LINE + 1], strval[DXF_MAX_LINE + 1];
	gdouble x, y, z;

	dxf_read_string(global, str);
	if(dxf_str_in_array(dxf_vars_vector3d, str)) {
		key = dxf_read_code(global); /* 10 */
		x = dxf_read_float64(global);
		key = dxf_read_code(global); /* 20 */
		y = dxf_read_float64(global);
		key = dxf_read_code(global); /* 30 */
		z = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
		g_debug("DXF: [v3d] %s: %.2f, %.2f, %.2f", str, x, y, z);
#endif
	} else if(dxf_str_in_array(dxf_vars_vector2d, str)) {
		key = dxf_read_code(global); /* 10 */
		x = dxf_read_float64(global);
		key = dxf_read_code(global); /* 20 */
		y = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
		g_debug("DXF: [v2d] %s: %.2f, %.2f", str, x, y);
#endif
	} else {
		key = dxf_read_code(global);
		switch(key) {
			case 1: case 2: case 3: case 4:
			case 5: case 6: case 7: case 8:
				/* string */
				dxf_read_string(global, strval);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [str] %s: %s", str, strval);
#endif
				break;
			case 40:
			case 50:
				x = dxf_read_float64(global);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [dbl] %s: %.2f", str, x);
#endif
				break;
			case 62:
			case 70:
				/* 16-bit integer */
				i16 = dxf_read_int16(global);
#if DEBUG > DXF_VAR_DEBUG
				g_debug("DXF: [i16] %s: %d", str, i16);
#endif
				break;
			default:
				DXF_HANDLE_UNKNOWN(global, key, strval, "** VARIABLE **");
				break;
		}
	}
	return TRUE;
}

gboolean dxf_section_HEADER(DxfGlobalData *global)
{
	gint32 key;
	gchar str[DXF_MAX_LINE + 1];

#if DEBUG > 0
	g_debug("DXF: parsing HEADER section");
#endif
	while(TRUE) {
		key = dxf_read_code(global);
		switch(key) {
			case DXF_CODE_INVALID:
				return 0xE0F;
				break;
			case 0: /* string */
				dxf_read_string(global, str);
				DXF_TEST_ENDSEC(str);
				break;
			case 9: /* variable */
				dxf_read_variable(global);
				break;
			default:
				DXF_HANDLE_UNKNOWN(global, key, str, "HEADER");
				break;
		} /* key */
	}
	return dxf_skip_section(global);
}

gboolean dxf_section_TABLES(DxfGlobalData *global)
{
	gint32 key;
	gchar str[DXF_MAX_LINE + 1];

#if DEBUG > 0
	g_debug("DXF: parsing TABLES section");
#endif
	while(TRUE) {
		key = dxf_read_code(global);
		switch(key) {
			case DXF_CODE_INVALID:
				return 0xE0F;
				break;
			case 0: /* string */
				dxf_read_string(global, str);
				DXF_TEST_ENDSEC(str);
				break;
			default:
				DXF_HANDLE_UNKNOWN(global, key, str, "TABLES");
				break;
		} /* key */
	}
	return dxf_skip_section(global);
}
