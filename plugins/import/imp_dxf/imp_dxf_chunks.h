#ifndef _IMP_DXF_CHUNKS_H
#define _IMP_DXF_CHUNKS_H

#include "imp_dxf_callbacks.h"
#include "imp_dxf_types.h"
#include "imp_dxf_def.h"

static DxfChunkInfo dxf_chunks[] = {
	/* app: <0 */
	/* string: 0-9 */
	{ 0,     "entity type",                           dxf_empty },
	{ 1,     "primary text value",                    dxf_debug_str },
	{ 2,     "name",                                  dxf_grpcode_2 },
	{ 3,     "other text (3)",                        dxf_debug_str },
	{ 4,     "other text (4)",                        dxf_debug_str },
	{ 5,     "entity handle",                         dxf_debug_str },
	{ 6,     "linetype name",                         dxf_debug_str },
	{ 7,     "text style name",                       dxf_debug_str },
	{ 8,     "layer name",                            dxf_debug_str },
	{ 9,     "variable name identifier",              dxf_debug_var },
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
	{ 52,    "angle",                                 dxf_debug_dbl },
	/* 16-bit integer value: 60-79 */
	{ 62,    "color number",                          dxf_debug_i16 },
	{ 65,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 66,    "entities follow",                       dxf_debug_i16 },
	{ 67,    "space",                                 dxf_debug_i16 },
	{ 68,    "APP: viewport not visible",             dxf_debug_i16 },
	{ 69,    "APP: viewport identification number",   dxf_debug_i16 },
	{ 70,    "integer value, 16-bit",                 dxf_grpcode_70 },
	{ 71,    "integer value, 16-bit",                 dxf_grpcode_71 },
	{ 72,    "integer value, 16-bit",                 dxf_grpcode_72 },
	{ 73,    "integer value, 16-bit",                 dxf_grpcode_73 },
	{ 74,    "integer value, 16-bit",                 dxf_grpcode_74 },
	{ 75,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 76,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 77,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 78,    "integer value, 16-bit",                 dxf_debug_i16 },
	{ 79,    "integer value, 16-bit",                 dxf_debug_i16 },
	/* 32-bit integer value: 90-99 */
	{ 90,    "integer value, 32-bit",                 dxf_debug_i32 },
	{ 91,    "integer value, 32-bit",                 dxf_debug_i32 },

	{ 100,   "subclass data marker",                  dxf_debug_str },
	{ 102,   "control string",                        dxf_debug_str },
	{ 105,   "DIMVAR object handle",                  dxf_debug_str },
	{ 110,   "UCS origin, X value",                   dxf_debug_dbl },
	{ 111,   "UCS X-axis, X value",                   dxf_debug_dbl },
	{ 112,   "UCS Y-axis, X value",                   dxf_debug_dbl },
	{ 120,   "UCS origin, Y value",                   dxf_debug_dbl },
	{ 121,   "UCS X-axis, Y value",                   dxf_debug_dbl },
	{ 122,   "UCS Y-axis, Y value",                   dxf_debug_dbl },
	{ 130,   "UCS origin, Z value",                   dxf_debug_dbl },
	{ 131,   "UCS X-axis, Z value",                   dxf_debug_dbl },
	{ 132,   "UCS Y-axis, Z value",                   dxf_debug_dbl },

	/* double precision floating-point value: 140-149 */
	{ 140,   "double-precision floating-point value", dxf_debug_dbl },
	{ 141,   "double-precision floating-point value", dxf_debug_dbl },
	{ 142,   "double-precision floating-point value", dxf_debug_dbl },
	{ 143,   "double-precision floating-point value", dxf_debug_dbl },
	{ 144,   "double-precision floating-point value", dxf_debug_dbl },
	{ 145,   "double-precision floating-point value", dxf_debug_dbl },
	{ 146,   "double-precision floating-point value", dxf_debug_dbl },
	{ 147,   "double-precision floating-point value", dxf_debug_dbl },
	{ 148,   "double-precision floating-point value", dxf_debug_dbl },
	{ 149,   "double-precision floating-point value", dxf_debug_dbl },
	/* 16-bit integer value: 170-179 */
	{ 170,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 171,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 172,   "integer value, 16-bit",                 dxf_debug_i16 },

	{ 210,   "extrusion direction, X value",          dxf_debug_dbl },
	{ 220,   "extrusion direction, Y value",          dxf_debug_dbl },
	{ 230,   "extrusion direction, Z value",          dxf_debug_dbl },
	/* 16-bit integer value: 270-279 */
	{ 270,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 271,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 272,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 273,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 274,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 275,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 276,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 277,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 278,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 279,   "integer value, 16-bit",                 dxf_debug_i16 },
	/* 16-bit integer value: 280-289 */
	{ 280,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 281,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 282,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 283,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 284,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 285,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 286,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 287,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 288,   "integer value, 16-bit",                 dxf_debug_i16 },
	{ 289,   "integer value, 16-bit",                 dxf_debug_i16 },

	/* boolean flag value: 290-299 */

	{ 330,   "softpointer handle",                    dxf_debug_str },
	{ 331,   "softpointer handle",                    dxf_debug_str },
	{ 340,   "hardpointer handle",                    dxf_debug_str },
	{ 350,   "softowner handle",                      dxf_debug_str },

	{ 370,   "lineweight enum value",                 dxf_debug_i16 },

	{ 390,   "PlotStyleName handle",                  dxf_debug_str },

	{ 420,   "integer value, 32-bit",                 dxf_debug_i32 },

	{ 1000,  "ASCII string",                          dxf_debug_str },
	{ 1001,  "registered application name",           dxf_debug_str },
	{ 1002,  "extended data control string",          dxf_debug_str },

	{ 1010,  "extended data point, X value",          dxf_debug_dbl },
	{ 1020,  "extended data point, Y value",          dxf_debug_dbl },
	{ 1030,  "extended data point, Z value",          dxf_debug_dbl },
	{ 1040,  "extended data dbl-prec fp value",       dxf_debug_dbl },
	{ 1070,  "extended data 16-bit integer",          dxf_debug_i16 },
	{ 1071,  "extended data 32-bit integer",          dxf_debug_i32 },

	{ DXF_CODE_INVALID, NULL, NULL },
};

#endif /* _IMP_DXF_CHUNKS_H */
