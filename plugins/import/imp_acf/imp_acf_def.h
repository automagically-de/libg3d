#ifndef _IMP_ACF_DEF_H
#define _IMP_ACF_DEF_H

#include <g3d/stream.h>

#include "imp_acf_def_proto.h"

typedef struct {
	GHashTable *db;
} AcfFile;

AcfFile *acf_def_read(G3DStream *stream, const AcfDef *def,
	gboolean bigendian);
void acf_def_free(AcfFile *acf);
AcfValue *acf_def_lookup(AcfFile *acf, const gchar *name);
void acf_def_dump(AcfValue *value);

#endif /* _IMP_ACF_DEF_H */
