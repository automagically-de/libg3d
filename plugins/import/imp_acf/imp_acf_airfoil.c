#include "imp_acf_airfoil.h"

AcfAirfoilDb *acf_airfoil_init(void)
{
	AcfAirfoilDb *db;
	GDir *dir;
	const gchar *dirname;

	dirname = g_getenv("AIRFOIL_DIR");
	if(!(dirname && g_file_test(dirname, G_FILE_TEST_IS_DIR))) {
#if DEBUG > 0
		g_warning("ACF: could not load airfoils");
#endif
		return NULL;
	}

	db = g_new0(AcfAirfoilDb, 1);
	db->db = g_hash_table_new(g_str_hash, g_str_equal);

	return db;
}

void acf_airfoil_cleanup(AcfAirfoilDb *db)
{
	g_hash_table_destroy(db->db);
	g_free(db);
}

AcfAirfoil *acf_airfoil_lookup(AcfAirfoilDb *db, const gchar *aflname)
{
	return g_hash_table_lookup(db->db, aflname);
}

