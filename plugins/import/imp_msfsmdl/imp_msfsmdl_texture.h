#ifndef IMP_MSFSMDL_TEXTURE_H
#define IMP_MSFSMDL_TEXTURE_H

#include <g3d/types.h>

G3DImage *msfsmdl_find_load_texture(
	G3DContext *context,
	G3DModel *model,
	const gchar *filename,
    gboolean in_fallback);

#endif /* IMP_MSFSMDL_TEXTURE_H */
