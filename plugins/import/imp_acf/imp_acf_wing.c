#include <string.h>

#include <g3d/types.h>
#include <g3d/face.h>
#include <g3d/vector.h>

#include "imp_acf_airfoil.h"

G3DObject *acf_wing(G3DMaterial *material, const gchar *name,
	gfloat *msweep, gfloat *mdihed, gfloat *mtrans,
	gfloat *vrt, gfloat *vtp,
	AcfAirfoil *aflrt, AcfAirfoil *afltp,
	gfloat wrt, gfloat wtp, gfloat lf)
{
	G3DObject *object;
	G3DFace *face;
	guint32 nverts;
	gint32 i;
	gfloat vec[3];

	nverts = aflrt->vertex_count;

	object = g_new0(G3DObject, 1);
	object->name = g_strdup(name);
	object->vertex_count = nverts * 2;
	object->vertex_data = g_new0(gfloat, object->vertex_count * 3);

	/* vertices */
	for(i = 0; i < nverts; i ++) {
		/* wing root */
		vec[2] = aflrt->vertex_data[i * 2 + 0] * wrt;
		vec[1] = aflrt->vertex_data[i * 2 + 1] * wrt;
		vec[0] = 0.0;
		g3d_vector_transform(vec, vec + 1, vec + 2, mdihed);
		g3d_vector_transform(vec, vec + 1, vec + 2, mtrans);
		memcpy(object->vertex_data + i * 3, vec, sizeof(vec));

		/* wing tip */
		vec[2] = afltp->vertex_data[i * 2 + 0] * wtp;
		vec[1] = afltp->vertex_data[i * 2 + 1] * wtp;
		vec[0] = 0.0;
		g3d_vector_transform(vec, vec + 1, vec + 2, mdihed);
		g3d_vector_transform(vec, vec + 1, vec + 2, mtrans);
		vec[0] += lf * vtp[0];
		vec[1] += lf * vtp[1];
		vec[2] += lf * vtp[2];
		memcpy(object->vertex_data + (i + nverts) * 3, vec, sizeof(vec));
	}
	/* faces */
	for(i = 0; i < (nverts - 1); i ++) {
		face = g3d_face_new_tri(material, i + 1, i, i + nverts);
		object->faces = g_slist_prepend(object->faces, face);
		face = g3d_face_new_tri(material, i + 1, i + nverts + 1, i + nverts);
		object->faces = g_slist_prepend(object->faces, face);
	}

	return object;
}
