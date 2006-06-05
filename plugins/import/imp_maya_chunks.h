#ifndef _IMP_MAYA_CHUNKS_H
#define _IMP_MAYA_CHUNKS_H

#include <g3d/iff.h>

#include "imp_maya_callbacks.h"

static g3d_iff_chunk_info maya_chunks[] = {
	{ "AUNI", "unknown",                           0, NULL },
	{ "BRSH", "brush",                             1, NULL }, /* FOR4 */
	{ "CHNG", "changes",                           0, NULL },
	{ "CMP#", "unknown",                           0, NULL },
	{ "CMPD", "unknown",                           0, NULL },
	{ "CONN", "unknown",                           1, NULL }, /* FOR4 */
	{ "CONS", "unknown",                           1, NULL }, /* LIS4 */
	{ "CWFL", "unknown",                           0, NULL },
	{ "CREA", "creator",                           0, NULL },
	{ "DBL2", "double 2",                          0, maya_cb_DBL2 },
	{ "DBL3", "double 3",                          0, maya_cb_DBL3 },
	{ "DBLE", "double",                            0, maya_cb_DBLE },
	{ "DCAM", "camera",                            1, NULL }, /* FOR4 */
	{ "DECT", "delete component",                  1, NULL }, /* FOR4 */
	{ "DELA", "unknown",                           1, NULL }, /* FOR4 */
	{ "DELL", "unknown",                           1, NULL }, /* LIS4 */
	{ "DISC", "unknown",                           0, NULL },
	{ "DISL", "unknown",                           1, NULL }, /* LIS4 */
	{ "DMSH", "mesh",                              1, NULL }, /* FOR4 */
	{ "DMTI", "material info",                     1, NULL }, /* FOR4 */
	{ "DPLM", "layer manager",                     1, NULL }, /* FOR4 */
	{ "DSPL", "layer ?",                           1, NULL }, /* FOR4 */
	{ "FINF", "file information",                  0, NULL },
	{ "FLGS", "flags",                             0, NULL },
	{ "FLT2", "float 2",                           0, NULL },
	{ "FLT3", "float 3",                           0, NULL },
	{ "GPID", "group id",                          1, NULL }, /* FOR4 */
	{ "GRPP", "group parts",                       1, NULL }, /* FOR4 */
	{ "HEAD", "header",                            1, NULL }, /* FOR4 */
	{ "ICON", "icon",                              0, NULL },
	{ "INCL", "includes",                          0, NULL },
	{ "INFO", "information",                       0, NULL },
	{ "LUNI", "unknown",                           0, NULL },
	{ "MADE", "unknown",                           0, NULL },
	{ "MATR", "unknown",                           0, NULL },
	{ "MESH", "mesh",                              0, maya_cb_MESH },
	{ "NPLN", "unknown",                           0, NULL },
	{ "NRBS", "NURBS ?",                           0, NULL },
	{ "NSRF", "surface",                           1, NULL }, /* FOR4 */
	{ "OBJN", "unknown",                           0, NULL },
	{ "PAUP", "poly auto proj",                    1, NULL }, /* FOR4 */
	{ "PBOP", "poly boolean operation",            1, NULL }, /* FOR4 */
	{ "PBOP", "poly bevel",                        1, NULL }, /* FOR4 */
	{ "PCRE", "poly create face",                  1, NULL }, /* FOR4 */
	{ "PCUB", "poly cube",                         1, NULL }, /* FOR4 */
	{ "PCYL", "poly cylinder",                     1, NULL }, /* FOR4 */
	{ "PEXE", "poly extrude",                      1, NULL }, /* FOR4 */
	{ "PEXF", "poly extrude face",                 1, NULL }, /* FOR4 */
	{ "PFUV", "poly flip UV",                      1, NULL }, /* FOR4 */
	{ "PING", "unknown",                           1, NULL }, /* FOR4 */
	{ "PLUG", "plugin ?",                          0, NULL },
	{ "PMIR", "poly mirror",                       1, NULL }, /* FOR4 */
	{ "PMVE", "poly merge vertices",               1, NULL }, /* FOR4 */
	{ "PPCT", "poly cut",                          1, NULL }, /* FOR4 */
	{ "PPIP", "poly pipe",                         1, NULL }, /* FOR4 */
	{ "PRNS", "unknown",                           0, NULL },
	{ "PRNT", "unknown",                           0, NULL },
	{ "PSMF", "poly smooth face",                  1, NULL }, /* FOR4 */
	{ "PSOE", "poly soft edge",                    1, NULL }, /* FOR4 */
	{ "PSPH", "poly sphere",                       1, NULL }, /* FOR4 */
	{ "PSPL", "poly split",                        1, NULL }, /* FOR4 */
	{ "PTUV", "poly tweak UV",                     1, NULL }, /* FOR4 */
	{ "PTWK", "poly tweak",                        1, NULL }, /* FOR4 */
	{ "PUNI", "poly unite",                        1, NULL }, /* FOR4 */
	{ "RANI", "anisotropic",                       1, NULL }, /* FOR4 */
	{ "RBLN", "unknown",                           1, NULL }, /* FOR4 */
	{ "RLAM", "lambert",                           1, NULL }, /* FOR4 */
	{ "RLLK", "light linker",                      1, NULL }, /* FOR4 */
	{ "RNDL", "render layer",                      1, NULL }, /* FOR4 */
	{ "RNLM", "render layer manager",              1, NULL }, /* FOR4 */
	{ "RPHO", "phong ?",                           1, NULL }, /* FOR4 */
	{ "RPL2", "place texture",                     1, NULL }, /* FOR4 */
	{ "RPLD", "place texture",                     1, NULL }, /* FOR4 */
	{ "RPRJ", "projection",                        1, NULL }, /* FOR4 */
	{ "RTFT", "texture file",                      1, NULL }, /* FOR4 */
	{ "SCRP", "script",                            1, NULL }, /* FOR4 */
	{ "SHAD", "shadow",                            1, NULL }, /* FOR4 */
	{ "SLCT", "unknown",                           0, NULL }, /* evil ;) */
	{ "STR ", "string",                            0, maya_cb_STR_ },
	{ "TGEO", "transform geometry",                1, NULL }, /* FOR4 */
	{ "TUNI", "unknown",                           0, NULL },
	{ "UVER", "minor version?",                    0, NULL },
	{ "VERS", "version",                           0, NULL },
	{ "XFRM", "transformation",                    1, NULL }, /* FOR4 */

	{ NULL, NULL, 0, NULL }
};

#endif /* _IMP_MAYA_CHUNKS_H */
