
#include "imp_acf_detect.h"
#include "imp_acf_def_acf651.h"
#include "imp_acf_def_acf740.h"

AcfDef *acf_detect_version(AcfGlobalData *global)
{
	return acf_def_acf740;
}
