
#include "imp_acf_detect.h"
#include "imp_acf_def_acf625.h"
#include "imp_acf_def_acf651.h"
#include "imp_acf_def_acf740.h"

typedef struct {
	goffset size;
	gchar *version;
	const AcfDef *def;
} AcfDetectInfo;

static AcfDetectInfo acf_detect_info[] = {
	{ 433337, "6.03", NULL },
	{ 433693, "6.25", acf_def_acf625 },
	{ 453981, "6.30", NULL },
	{ 454977, "6.40", NULL },
	{ 454985, "6.51", acf_def_acf651 },
	{ 646626, "7.00", NULL },
	{ 647101, "7.40", acf_def_acf740 },
	{ 0, NULL, NULL }
};

const AcfDef *acf_detect_version(AcfGlobalData *global)
{
	gint32 i;

	for(i = 0; acf_detect_info[i].size > 0; i ++) {
		if(g3d_stream_size(global->stream) == acf_detect_info[i].size) {
#if DEBUG > 0
			g_debug("ACF: version %s detected", acf_detect_info[i].version);
#endif
			return acf_detect_info[i].def;
		}
	}

	for(i = 0; acf_detect_info[i].size > 0; i ++)
		if(acf_detect_info[i].size > g3d_stream_size(global->stream)) {
#if DEBUG > 0
			g_debug("ACF: falling back to version %s",
				acf_detect_info[i].version);
#endif
			return acf_detect_info[i].def;
		}

	/* FIXME: return 7.40 for models newer than that */

	g_warning("ACF: could not detect version");
	return NULL;
}
