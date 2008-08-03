#ifndef _IMP_ACF_AIRFOIL_H
#define _IMP_ACF_AIRFOIL_H

#include <glib.h>

typedef struct {
	gchar *filename;
	gchar *description;
	/* 2D vertexes */
	guint32 vertex_count;
	gfloat vertex_data;
} AcfAirfoil;

typedef struct {
	GHashTable *db;
	GSList *airfoils;
} AcfAirfoilDb;

AcfAirfoilDb *acf_airfoil_init(void);
void acf_airfoil_cleanup(AcfAirfoilDb *db);
AcfAirfoil *acf_airfoil_lookup(AcfAirfoilDb *db, const gchar *aflname);

#endif /* _IMP_ACF_AIRFOIL_H */
