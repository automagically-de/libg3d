pluginimpdir = $(pkglibdir)/plugins/import

pluginimp_LTLIBRARIES = imp_ar.la

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@

INCLUDES = -I$(top_builddir)/include

AM_LDFLAGS = \
	-module \
	@PLUGINS_LDFLAGS@

LIBADD = \
	@GLIB_LIBS@

LIBTOOL += --quiet

imp_ar_la_SOURCES = imp_ar.c imp_ar.h \
	imp_ar_decompress.c imp_ar_decompress.h \
	imp_ar_dof.c imp_ar_dof.h \
	imp_ar_carini.c imp_ar_carini.h

