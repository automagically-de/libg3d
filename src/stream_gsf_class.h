#ifndef _STREAM_GSF_CLASS_H
#define _STREAM_GSF_CLASS_H

#include <gsf/gsf-input.h>
#include <g3d/types.h>

G_BEGIN_DECLS

#define G3D_GSF_INPUT_STREAM_TYPE      (g3d_gsf_input_stream_get_type())
#define G3D_GSF_INPUT_STREAM(o)        (G_TYPE_CHECK_INSTANCE_CAST((o), \
	G3D_GSF_INPUT_STREAM_TYPE, G3DGsfInputStream))
#define G3D_IS_GSF_INPUT_STREAM(o)     (G_TYPE_CHECK_INSTANCE_TYPE((o), \
	G3D_GSF_INPUT_STREAM_TYPE))

typedef struct _G3DGsfInputStream G3DGsfInputStream;

GType g3d_gsf_input_stream_get_type(void) G_GNUC_CONST;

GsfInput *g3d_gsf_input_stream_new(G3DStream *stream);

G_END_DECLS

#endif /* _STREAM_GSF_CLASS_H */
