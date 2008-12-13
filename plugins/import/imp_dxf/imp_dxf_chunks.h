#ifndef _IMP_DXF_CHUNKS_H
#define _IMP_DXF_CHUNKS_H

#include "imp_dxf_callbacks.h"

typedef struct {
	gint32 id;
	const gchar *description;
	DxfCallback callback;
} DxfChunkInfo;

#define DXF_GRPCODE_TERMINATOR 0xDEADBEEF

static DxfChunkInfo dxf_chunks[] = {
	/* app: <0 */
	/* string: 0-9 */
	{ 0,     "entity type",                           dxf_grpcode_0 },
	{ 1,     "primary text value",                    dxf_debug_str },
	{ 2,     "name",                                  dxf_debug_str },
	{ 3,     "other text (3)",                        dxf_debug_str },
	{ 4,     "other text (4)",                        dxf_debug_str },
	{ 5,     "entity handle",                         dxf_debug_str },
	{ 6,     "linetype name",                         dxf_debug_str },
	{ 7,     "text style name",                       dxf_debug_str },
	{ 8,     "layer name",                            dxf_debug_str },
	{ 9,     "variable name identifier",              dxf_debug_str },
	/* double precision 3D point value: 10-39 */
	{ 10,    "primary point, X value",                dxf_pnt_coord },
	{ 11,    "point #2, X value",                     dxf_pnt_coord },
	{ 12,    "point #3, X value",                     dxf_pnt_coord },
	{ 13,    "point #4, X value",                     dxf_pnt_coord },
	{ 14,    "point #5, X value",                     dxf_pnt_coord },
	{ 15,    "point #6, X value",                     dxf_pnt_coord },
	{ 16,    "point #7, X value",                     dxf_pnt_coord },
	{ 17,    "point #8, X value",                     dxf_pnt_coord },
	{ 18,    "point #9, X value",                     dxf_pnt_coord },
	{ 19,    "point #10, X value",                    dxf_pnt_coord },
	{ 20,    "primary point, Y value",                dxf_pnt_coord },
	{ 21,    "point #2, Y value",                     dxf_pnt_coord },
	{ 22,    "point #3, Y value",                     dxf_pnt_coord },
	{ 23,    "point #4, Y value",                     dxf_pnt_coord },
	{ 24,    "point #5, Y value",                     dxf_pnt_coord },
	{ 25,    "point #6, Y value",                     dxf_pnt_coord },
	{ 26,    "point #7, Y value",                     dxf_pnt_coord },
	{ 27,    "point #8, Y value",                     dxf_pnt_coord },
	{ 28,    "point #9, Y value",                     dxf_pnt_coord },
	{ 29,    "point #10, Y value",                    dxf_pnt_coord },
	{ 30,    "primary point, Z value",                dxf_pnt_coord },
	{ 31,    "point #2, Z value",                     dxf_pnt_coord },
	{ 32,    "point #3, Z value",                     dxf_pnt_coord },
	{ 33,    "point #4, Z value",                     dxf_pnt_coord },
	{ 34,    "point #5, Z value",                     dxf_pnt_coord },
	{ 35,    "point #6, Z value",                     dxf_pnt_coord },
	{ 36,    "point #7, Z value",                     dxf_pnt_coord },
	{ 37,    "point #8, Z value",                     dxf_pnt_coord },
	{ 38,    "point #9, Z value",                     dxf_pnt_coord },
	{ 39,    "point #10, Z value",                    dxf_pnt_coord },
	/* double precision floating-point value: 40-59 */
	{ 40,    "double-precision floating-point value", dxf_debug_dbl },
	{ 41,    "double-precision floating-point value", dxf_debug_dbl },
	{ 42,    "double-precision floating-point value", dxf_debug_dbl },
	{ 43,    "double-precision floating-point value", dxf_debug_dbl },
	{ 44,    "double-precision floating-point value", dxf_debug_dbl },
	{ 45,    "double-precision floating-point value", dxf_debug_dbl },
	{ 46,    "double-precision floating-point value", dxf_debug_dbl },
	{ 47,    "double-precision floating-point value", dxf_debug_dbl },
	{ 48,    "linetype scale dbl-prec fp value",      dxf_debug_dbl },
	{ 49,    "repeated dbl-prec fp value",            dxf_debug_dbl },
	{ 50,    "angle",                                 dxf_debug_dbl },
	{ 51,    "angle",                                 dxf_debug_dbl },
	/* 16-bit integer value: 60-79 */
	{ 62,    "color number",                          dxf_debug_i16 },
	{ 66,    "entities follow",                       dxf_debug_i16 },
	{ 70,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 71,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 72,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 73,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 74,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 75,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 76,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 77,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 78,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 79,    "integer value, 16-bit",                 dxf_debug_i16 },
	/* 32-bit integer value: 90-99 */

	{ 210,   "extrusion direction, X value",          dxf_debug_dbl },
	{ 220,   "extrusion direction, Y value",          dxf_debug_dbl },
	{ 230,   "extrusion direction, Z value",          dxf_debug_dbl },

	{ DXF_GRPCODE_TERMINATOR, NULL, NULL },
};

#endif /* _IMP_DXF_CHUNKS_H */
