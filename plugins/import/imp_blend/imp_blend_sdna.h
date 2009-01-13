#ifndef _IMP_BLEND_SDNA_H
#define _IMP_BLEND_SDNA_H

#include <g3d/stream.h>

typedef struct {
	gchar *name;
	gchar *type;
	gsize tsize;
	gboolean ptr;
} BlendSdnaProperty;

typedef struct {
	gchar *name;
	gsize size;
	GSList *properties;
} BlendSdnaStruct;

typedef struct {
	guint32 n_names;
	gchar **names;
	guint32 n_types;
	gchar **type_names;
	guint32 *type_sizes;
	GSList *structs;
} BlendSdna;

BlendSdna *blend_sdna_read_dna1(G3DStream *stream, guint32 flags, gint32 len);
gboolean blend_sdna_dump_struct(BlendSdna *sdna, guint32 sdnanr);

#endif /* _IMP_BLEND_SDNA_H */
