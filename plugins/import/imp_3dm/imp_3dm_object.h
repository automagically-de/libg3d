#ifndef _IMP_3DM_OBJECT_H
#define _IMP_3DM_OBJECT_H

#include "imp_3dm_types.h"

gboolean tdm_object_read_vertex_data_uncompressed(TdmGlobal *global,
	TdmLocal *local);
gboolean tdm_object_read_vertex_data_compressed(TdmGlobal *global,
	TdmLocal *local);

#endif /* _IMP_3DM_OBJECT_H */
