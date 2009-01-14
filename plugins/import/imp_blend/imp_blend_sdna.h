#ifndef _IMP_BLEND_SDNA_H
#define _IMP_BLEND_SDNA_H

#include <g3d/stream.h>

#include "imp_blend_types.h"

typedef struct {
	const gchar *name;
	const gchar *tname;
	guint32 tid;
	gsize tsize;
	gboolean ptr;
} BlendSdnaProperty;

typedef struct {
	const gchar *name;
	gsize size;
	GSList *properties;
} BlendSdnaStruct;

typedef struct {
	const BlendSdnaStruct *sstruct;
	GHashTable *prophash;
} BlendSdnaData;

typedef enum {
	T_CHAR    = 0,
	T_UCHAR   = 1,
	T_SHORT   = 2,
	T_USHORT  = 3,
	T_INT     = 4,
	T_LONG    = 5,
	T_ULONG   = 6,
	T_FLOAT   = 7,
	T_DOUBLE  = 8,
	T_VOID    = 9,
	T_STRUCT  = 0xFF
} BlendSdnaPropType;

typedef struct {
	gchar *name;
	BlendSdnaPropType type;
	BlendSdnaProperty *sprop;
	guint32 uval;   /* unsigned integer value */
	gint32 ival;    /* signed integer value */
	G3DFloat fval;  /* floating point value */
	guint32 pval;   /* pointer value */
	BlendSdnaData *structval; /* struct value */
} BlendSdnaPropData;

typedef struct {
	guint32 n_names;
	gchar **names;
	guint32 n_types;
	gchar **type_names;
	guint32 *type_sizes;
	GSList *structs;
} BlendSdna;

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
