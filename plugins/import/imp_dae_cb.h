#ifndef _IMP_DAE_CB_H
#define _IMP_DAE_CB_H

#include <g3d/types.h>
#include <libxml/tree.h>

#include "imp_dae_library.h"

typedef struct {
	G3DModel *model;
	DaeLibrary *lib;
	xmlDocPtr xmldoc;
} DaeGlobalData;

typedef struct {
	xmlNodePtr parent;
	xmlNodePtr node;
	xmlNodePtr instance;
	gpointer user_data;
} DaeLocalData;

typedef gboolean (* DaeCallback)(DaeGlobalData *global, DaeLocalData *local);

typedef struct {
	const gchar *name;
	DaeCallback callback;
} DaeChunkDesc;

gboolean dae_cb_geometry(DaeGlobalData *global, DaeLocalData *local);
gboolean dae_cb_node(DaeGlobalData *global, DaeLocalData *local);
gboolean dae_cb_visual_scene(DaeGlobalData *global, DaeLocalData *local);

#endif /* _IMP_DAE_CB_H */
