#ifndef _IMP_MAYA_CHUNKS_H
#define _IMP_MAYA_CHUNKS_H

#include <g3d/iff.h>

#include "imp_maya_callbacks.h"

static g3d_iff_chunk_info maya_chunks[] = {
	{ "\x00\x10\x2e\x42", "unknown",               1, NULL }, /* FOR4 */
	{ "AUNI", "unknown",                           0, NULL },
	{ "BRSH", "unknown",                           1, NULL }, /* FOR4 */
	{ "CHNG", "unknown",                           0, NULL },
	{ "CMPD", "unknown",                           0, NULL },
	{ "CREA", "creator",                           0, NULL },
	{ "DBL3", "unknown",                           0, NULL },
	{ "DBLE", "unknown",                           0, NULL },
	{ "DCAM", "unknown",                           1, NULL }, /* FOR4 */
	{ "DECT", "unknown",                           0, NULL },
	{ "DMSH", "unknown",                           0, NULL },
	{ "DMTI", "unknown",                           0, NULL },
	{ "DPLM", "unknown",                           0, NULL },
	{ "DSPL", "unknown",                           0, NULL },
	{ "FINF", "unknown",                           0, NULL },
	{ "FLGS", "flags",                             0, NULL },
	{ "FLT3", "unknown",                           0, NULL },
	{ "GPID", "unknown",                           1, NULL }, /* FOR4 */
	{ "GRPP", "unknown",                           0, NULL },
	{ "HEAD", "header",                            1, NULL }, /* FOR4 */
	{ "ICON", "unknown",                           0, NULL },
	{ "INCL", "unknown",                           0, NULL },
	{ "INFO", "unknown",                           0, NULL },
	{ "LIS4", "unknown",                           0, NULL },
	{ "LUNI", "unknown",                           0, NULL },
	{ "MADE", "unknown",                           0, NULL },
	{ "NRBS", "unknown",                           0, NULL },
	{ "NSRF", "unknown",                           1, NULL }, /* FOR4 */
	{ "OBJN", "unknown",                           0, NULL },
	{ "PAUP", "unknown",                           0, NULL },
	{ "PCRE", "unknown",                           0, NULL },
	{ "PCUB", "unknown",                           0, NULL },
	{ "PCYL", "unknown",                           0, NULL },
	{ "PEXE", "unknown",                           0, NULL },
	{ "PEXF", "unknown",                           0, NULL },
	{ "PFUV", "unknown",                           0, NULL },
	{ "PLUG", "unknown",                           0, NULL },
	{ "PMIR", "unknown",                           0, NULL },
	{ "PSOE", "unknown",                           0, NULL },
	{ "PSPL", "unknown",                           0, NULL },
	{ "PTUV", "unknown",                           0, NULL },
	{ "PTWK", "unknown",                           0, NULL },
	{ "RBLN", "unknown",                           0, NULL },
	{ "RLAM", "unknown",                           0, NULL },
	{ "RLLK", "unknown",                           0, NULL },
	{ "RNDL", "unknown",                           0, NULL },
	{ "RNLM", "unknown",                           0, NULL },
	{ "RPHO", "unknown",                           0, NULL },
	{ "RPL2", "unknown",                           0, NULL },
	{ "RPLD", "unknown",                           0, NULL },
	{ "RPRJ", "unknown",                           0, NULL },
	{ "RTFT", "unknown",                           0, NULL },
	{ "SCRP", "unknown",                           0, NULL },
	{ "SHAD", "unknown",                           0, NULL },
	{ "SLCT", "unknown",                           0, NULL },
	{ "STR ", "string",                            0, NULL },
	{ "TUNI", "unknown",                           0, NULL },
	{ "UVER", "minor version?",                    0, NULL },
	{ "VERS", "version",                           0, NULL },
	{ "XFRM", "transformation",                    1, NULL }, /* FOR4 */

	{ NULL, NULL, 0, NULL }
};

#endif /* _IMP_MAYA_CHUNKS_H */
