#ifndef _IMP_BLEND_SDNA_H
#define _IMP_BLEND_SDNA_H

#include <g3d/stream.h>

typedef struct {
	guint32 n_names;
	gchar **names;
	guint32 n_types;
	gchar **type_names;
	guint32 *type_sizes;
} BlendSdna;

BlendSdna *blend_sdna_read_dna1(G3DStream *stream, guint32 flags, gint32 len);

#endif /* _IMP_BLEND_SDNA_H */
