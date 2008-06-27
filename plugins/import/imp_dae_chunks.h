#ifndef _IMP_DAE_CHUNKS_H
#define _IMP_DAE_CHUNKS_H

#include "imp_dae_cb.h"

static DaeChunkDesc dae_chunks_geometry[] = {
	{ "bind_material",   NULL },
	{ "mesh",            dae_cb_mesh },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_mesh[] = {
	{ "lines",           NULL },
	{ "polylist",        dae_cb_polylist },
	{ "source",          dae_cb_source },
	{ "triangles",       dae_cb_triangles },
	{ "vertices",        dae_cb_vertices },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_node[] = {
	{ "camera",          NULL },
	{ "geometry",        dae_cb_geometry },
	{ "light",           NULL },
	{ "matrix",          dae_cb_matrix },
	{ "node",            dae_cb_node },
	{ "scale",           dae_cb_scale },
	{ "rotate",          dae_cb_rotate },
	{ "translate",       dae_cb_translate },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_vertices[] = {
	{ "input",           dae_cb_vertices__input },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_visual_scene[] = {
	{ "node",            dae_cb_node },
	{ NULL, NULL }
};

#endif /* _IMP_DAE_CHUNKS_H */
