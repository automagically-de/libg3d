#ifndef _IMP_BLEND_CHUNKS_H
#define _IMP_BLEND_CHUNKS_H

#include "imp_blend_def.h"
#include "imp_blend_types.h"
#include "imp_blend_callbacks.h"

typedef gboolean (* BlendCallback)(BlendGlobal *, BlendLocal *);

typedef struct {
	guint32 code;
	const gchar *description;
	BlendCallback callback;
} BlendChunkInfo;

static const BlendChunkInfo blend_chunks[] = {
	{ MKID('D','A','T','A'), "struct data",                  NULL },

	{ MKID2('C','A'),        "camera",                       NULL },
	{ MKID2('I','M'),        "image",                        NULL },
	{ MKID2('L','A'),        "lamp",                         NULL },
	{ MKID2('M','A'),        "material",                     NULL },
	{ MKID2('M','E'),        "mesh",                         NULL },
	{ MKID2('O','B'),        "object",                       NULL },
	{ MKID2('S','C'),        "scene",                        NULL },
	{ MKID2('S','R'),        "screen",                       NULL },
	{ MKID2('T','E'),        "texture",                      NULL },

	{ MKID('D','N','A','1'), "SDNA data",                    blend_cb_DNA1 },
	{ MKID('G','L','O','B'), "global data",                  NULL },
	{ MKID('R','E','N','D'), "render data",                  NULL },

	{ 0, NULL, NULL }
};

#endif /* _IMP_BLEND_CHUNKS_H */
