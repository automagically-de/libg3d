#ifndef _IMP_BLEND_SDNA_H
#define _IMP_BLEND_SDNA_H

#include <g3d/stream.h>

#include "imp_blend_types.h"

/* as in #if DEBUG > BLEND_DEBUG_STRUCT */
#define BLEND_DEBUG_STRUCT 1

BlendSdna *blend_sdna_read_dna1(G3DStream *stream, guint32 flags, gint32 len);
void blend_sdna_cleanup(BlendSdna *sdna);

const BlendSdnaStruct *blend_sdna_get_struct_by_id(BlendSdna *sdna,
	guint32 sdnanr);
const BlendSdnaStruct *blend_sdna_get_struct_by_name(BlendSdna *sdna,
	const gchar *name);

BlendSdnaData *blend_sdna_data_read(BlendSdna *sdna,
	const BlendSdnaStruct *sstruct, BlendGlobal *global, gsize *r,
	guint32 level);
void blend_sdna_data_free(BlendSdnaData *sdata);

gboolean blend_sdna_dump_struct(BlendSdna *sdna, guint32 sdnanr);

#endif /* _IMP_BLEND_SDNA_H */
