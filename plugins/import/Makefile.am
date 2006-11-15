pluginimpdir = $(pkglibdir)/plugins/import

pluginimp_LTLIBRARIES = \
	imp_3dmf.la \
	imp_3ds.la \
	imp_ac3d.la \
	imp_ar.la \
	imp_cob.la \
	imp_dxf.la \
	imp_glb.la \
	imp_heightfield.la \
	imp_iob.la \
	imp_joe.la \
	imp_leocad.la \
	imp_lwo.la \
	imp_md2.la \
	imp_md3.la \
	imp_nff.la \
	imp_obj.la \
	imp_q3o.la \
	imp_vrml.la

if EXPERIMENTAL
pluginimp_LTLIBRARIES += \
	imp_flt.la \
	imp_maya.la \
	imp_r4.la \
	imp_rbh.la \
	imp_test.la
endif

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@

INCLUDES = -I$(top_builddir)/include

AM_LDFLAGS = \
	-module \
	@PLUGINS_LDFLAGS@

AM_LFLAGS = -o${LEX_OUTPUT_ROOT}.c

LIBADD = \
	@GLIB_LIBS@

LIBTOOL += --quiet

imp_3dmf_la_SOURCES = imp_3dmf.c
imp_3ds_la_SOURCES = imp_3ds.c imp_3ds.h imp_3ds_chunks.h \
	imp_3ds_callbacks.c imp_3ds_callbacks.h
imp_ac3d_la_SOURCES = imp_ac3d.c
imp_ar_la_SOURCES = imp_ar.c imp_ar.h \
	imp_ar_decompress.c imp_ar_decompress.h \
	imp_ar_dof.c imp_ar_dof.h \
	imp_ar_carini.c imp_ar_carini.h
imp_cob_la_SOURCES = imp_cob.c
imp_dxf_la_SOURCES = imp_dxf.c
imp_flt_la_SOURCES = imp_flt.c imp_flt_opcodes.h
imp_glb_la_SOURCES = imp_glb.c
imp_heightfield_la_SOURCES = imp_heightfield.c
imp_iob_la_SOURCES = imp_iob.c imp_iob.h imp_iob_chunks.h \
	imp_iob_callbacks.c imp_iob_callbacks.h
imp_joe_la_SOURCES = imp_joe.c
imp_leocad_la_SOURCES = imp_leocad.c imp_leocad_library.h imp_leocad_library.c
imp_lwo_la_SOURCES = imp_lwo.c imp_lwo.h imp_lwo_chunks.h \
	imp_lwo_callbacks.c imp_lwo_callbacks.h
imp_maya_la_SOURCES = imp_maya.c imp_maya_chunks.h \
	imp_maya_callbacks.c imp_maya_callbacks.h \
	imp_maya_obj.c imp_maya_obj.h \
	imp_maya_var.c imp_maya_var.h
imp_md2_la_SOURCES = imp_md2.c imp_md2_normals.h
imp_md3_la_SOURCES = imp_md3.c
imp_nff_la_SOURCES = imp_nff.c
imp_obj_la_SOURCES = imp_obj.c
imp_q3o_la_SOURCES = imp_q3o.c
imp_r4_la_SOURCES = imp_r4.c imp_r4_chunks.h \
	imp_r4_callbacks.c imp_r4_callbacks.h
imp_rbh_la_SOURCES = imp_rbh.c imp_rbh_chunks.h \
	imp_rbh_callbacks.c imp_rbh_callbacks.h
imp_vrml_la_SOURCES = imp_vrml_v1.l imp_vrml.c
imp_vrml_la_LIBADD = ${LEXLIB}
imp_test_la_SOURCES = imp_test.c

# by default imp_vrml_v1.c seems to be included in package, so add .h too
# also removes strong dependancy on flex
EXTRA_DIST = imp_vrml_v1.c imp_vrml_v1.h
