pluginimpdir = $(pkglibdir)/plugins/import

pluginimp_LTLIBRARIES = imp_vrml.la

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@

INCLUDES = -I$(top_srcdir)/include

AM_LDFLAGS = \
	-no-undefined -module \
	@PLUGINS_LDFLAGS@

LIBTOOL += --quiet

imp_vrml_la_SOURCES = \
	imp_vrml.c imp_vrml.h \
	\
	imp_vrml_v1_chunks.h imp_vrml_v1_types.h \
	imp_vrml_v1_callbacks.c imp_vrml_v1_callbacks.h \
	imp_vrml_v1_scanner.c imp_vrml_v1_scanner.h

imp_vrml_la_LIBADD = $(top_builddir)/src/libg3d.la @GLIB_LIBS@

