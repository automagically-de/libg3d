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
	{ 0,     "entity type",                           NULL },
	{ 1,     "primary text value",                    NULL },
	{ 2,     "name",                                  NULL },
	{ 3,     "other text (3)",                        NULL },
	{ 4,     "other text (4)",                        NULL },
	{ 5,     "entity handle",                         NULL },
	{ 6,     "linetype name",                         NULL },
	{ 7,     "text style name",                       NULL },
	{ 8,     "layer name",                            NULL },
	{ 9,     "variable name identifier",              NULL },
	/* double precision 3D point value: 10-39 */
	{ 10,    "primary point, X value",                NULL },
	{ 11,    "point #2, X value",                     NULL },
	{ 20,    "primary point, Y value",                NULL },
	{ 21,    "point #2, Y value",                     NULL },
	{ 30,    "primary point, Z value",                NULL },
	{ 31,    "point #2, Z value",                     NULL },
	/* double precision floating-point value: 40-59 */
	{ 40,    "double-precision floating-point value", NULL },
	{ 41,    "double-precision floating-point value", NULL },
	{ 50,    "angle",                                 NULL },
	/* 16-bit integer value: 60-79 */
	{ 62,    "color number",                          NULL },
	{ 66,    "entities follow",                       NULL },
	{ 70,    "integer value, 16-bit",                 NULL },
	{ 71,    "integer value, 16-bit",                 NULL },
	{ 72,    "integer value, 16-bit",                 NULL },
	/* 32-bit integer value: 90-99 */

	{ 210,   "extrusion direction, X value",          NULL },
	{ 220,   "extrusion direction, Y value",          NULL },
	{ 230,   "extrusion direction, Z value",          NULL },

	{ DXF_GRPCODE_TERMINATOR, NULL, NULL },
};

#endif /* _IMP_DXF_CHUNKS_H */
