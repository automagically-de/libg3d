#ifndef _IMP_DXF_DEF_H
#define _IMP_DXF_DEF_H

#define DXF_MAX_LINE 512
#define DXF_CODE_INVALID 0xDEADBEEF

#define DXF_ID_HEADER	0x00FF0000
#define DXF_ID_TABLES	0x00FF0001
#define DXF_ID_ENTITIES	0x00FF0002
#define DXF_ID_BLOCKS   0x00FF0003
#define DXF_ID_OBJECTS  0x00FF0004
#define DXF_ID_CLASSES  0x00FF0005

#define DXF_E_3DFACE         0x0001
#define DXF_E_POLYLINE       0x0002
#define DXF_E_VERTEX         0x0003
#define DXF_E_OTHER          0xFFFF

#define DXF_POLY_CLOSED             1
#define DXF_POLY_CURVE_FIT_ADDED    2
#define DXF_POLY_SPLINE_FIT_ADDED   4
#define DXF_POLY_3D_POLYLINE        8
#define DXF_POLY_3D_POLYMESH       16
#define DXF_POLY_N_CLOSED          32
#define DXF_POLY_POLYFACE          64
#define DXF_POLY_LT_PATTERN       128

#endif /* _IMP_DXF_DEF_H */
