#ifndef _IMP_DAE_CHUNKS_H
#define _IMP_DAE_CHUNKS_H

#include "imp_dae_cb.h"

static DaeChunkDesc dae_chunks_bind_material[] = {
	{ "technique_common", dae_cb_technique_common },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_effect[] = {
	{ "profile_COMMON",   dae_cb_profile_COMMON },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_geometry[] = {
	{ "bind_material",    dae_cb_bind_material },
	{ "mesh",             dae_cb_mesh },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_material[] = {
	{ "effect",           dae_cb_effect },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_mesh[] = {
	{ "lines",            NULL },
	{ "polylist",         dae_cb_polylist },
	{ "source",           dae_cb_source },
	{ "triangles",        dae_cb_triangles },
	{ "vertices",         dae_cb_vertices },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_node[] = {
	{ "camera",           NULL },
	{ "geometry",         dae_cb_geometry },
	{ "light",            NULL },
	{ "matrix",           dae_cb_matrix },
	{ "node",             dae_cb_node },
	{ "scale",            dae_cb_scale },
	{ "rotate",           dae_cb_rotate },
	{ "translate",        dae_cb_translate },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_profile_COMMON[] = {
	{ "extra",            NULL },
	{ "newparam",         dae_cb_newparam },
	{ "technique",        dae_cb_technique },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_technique[] = {
	{ "phong",            dae_cb_phong },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_vertices[] = {
	{ "input",            dae_cb_vertices__input },
	{ NULL, NULL }
};

static DaeChunkDesc dae_chunks_visual_scene[] = {
	{ "node",             dae_cb_node },
	{ NULL, NULL }
};

#endif /* _IMP_DAE_CHUNKS_H */
