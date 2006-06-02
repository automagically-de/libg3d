#ifndef _IMP_MAYA_CHUNKS_H
#define _IMP_MAYA_CHUNKS_H

#include <g3d/iff.h>

#include "imp_maya_callbacks.h"

static g3d_iff_chunk_info maya_chunks[] = {
	{ "AUNI", "unknown",                           0, NULL },
	{ "BRSH", "unknown",                           1, NULL }, /* FOR4 */
	{ "CHNG", "changes",                           0, NULL },
	{ "CMP#", "unknown",                           0, NULL },
	{ "CMPD", "unknown",                           0, NULL },
	{ "CONN", "unknown",                           1, NULL }, /* FOR4 */
	{ "CONS", "unknown",                           1, NULL }, /* LIS4 */
	{ "CWFL", "unknown",                           0, NULL },
	{ "CREA", "creator",                           0, NULL },
	{ "DBL2", "double 2",                          0, NULL },
	{ "DBL3", "double 3",                          0, NULL },
	{ "DBLE", "double",                            0, NULL },
	{ "DCAM", "camera",                            1, NULL }, /* FOR4 */
	{ "DECT", "delete component",                  1, NULL }, /* FOR4 */
	{ "DMSH", "mesh",                              1, NULL }, /* FOR4 */
	{ "DMTI", "material info",                     1, NULL }, /* FOR4 */
	{ "DPLM", "unknown",                           0, NULL },
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
	{ "LIS4", "unknown",                           0, NULL },
	{ "LUNI", "unknown",                           0, NULL },
	{ "MADE", "unknown",                           0, NULL },
	{ "MATR", "unknown",                           0, NULL },
	{ "MESH", "mesh",                              0, maya_cb_MESH },
	{ "NRBS", "unknown",                           0, NULL },
	{ "NSRF", "unknown",                           1, NULL }, /* FOR4 */
	{ "OBJN", "unknown",                           0, NULL },
	{ "PAUP", "poly auto proj",                    1, NULL }, /* FOR4 */
	{ "PCRE", "poly create face",                  1, NULL }, /* FOR4 */
	{ "PCUB", "poly cube",                         1, NULL }, /* FOR4 */
	{ "PCYL", "poly cylinder",                     1, NULL }, /* FOR4 */
	{ "PEXE", "poly extrude",                      1, NULL }, /* FOR4 */
	{ "PEXF", "poly extrude face",                 1, NULL }, /* FOR4 */
	{ "PFUV", "unknown",                           0, NULL },
	{ "PING", "unknown",                           0, NULL },
	{ "PLUG", "unknown",                           0, NULL },
	{ "PMIR", "poly mirror",                       1, NULL }, /* FOR4 */
	{ "PMVE", "unknown",                           0, NULL },
	{ "PPCT", "unknown",                           0, NULL },
	{ "PPIP", "unknown",                           0, NULL },
	{ "PSMF", "unknown",                           0, NULL },
	{ "PSOE", "unknown",                           0, NULL },
	{ "PSPH", "unknown",                           0, NULL },
	{ "PSPL", "poly split",                        1, NULL }, /* FOR4 */
	{ "PTUV", "unknown",                           0, NULL },
	{ "PTWK", "poly tweak",                        1, NULL }, /* FOR4 */
	{ "RBLN", "unknown",                           1, NULL }, /* FOR4 */
	{ "RLAM", "lambert",                           1, NULL }, /* FOR4 */
	{ "RLLK", "unknown",                           0, NULL },
	{ "RNDL", "unknown",                           0, NULL },
	{ "RNLM", "unknown",                           0, NULL },
	{ "RPHO", "unknown",                           0, NULL },
	{ "RPL2", "place texture",                     1, NULL }, /* FOR4 */
	{ "RPLD", "unknown",                           0, NULL },
	{ "RPRJ", "projection",                        1, NULL }, /* FOR4 */
	{ "RTFT", "texture file",                      1, NULL }, /* FOR4 */
	{ "SCRP", "script",                            1, NULL }, /* FOR4 */
	{ "SHAD", "shadow",                            1, NULL }, /* FOR4 */
	{ "SLCT", "unknown",                           0, NULL }, /* evil ;) */
	{ "STR ", "string",                            0, maya_cb_STR_ },
	{ "TGEO", "unknown",                           0, NULL },
	{ "TUNI", "unknown",                           0, NULL },
	{ "UVER", "minor version?",                    0, NULL },
	{ "VERS", "version",                           0, NULL },
	{ "XFRM", "transformation",                    1, NULL }, /* FOR4 */

	{ NULL, NULL, 0, NULL }
};

#endif /* _IMP_MAYA_CHUNKS_H */
