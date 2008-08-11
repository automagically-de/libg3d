pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
pluginimp_LTLIBRARIES = imp_ldraw.la
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

LIBADD = \
	@GLIB_LIBS@

LIBTOOL += --quiet

imp_ldraw_la_SOURCES = imp_ldraw.c imp_ldraw_misc.h imp_ldraw_types.h \
	imp_ldraw_part.c imp_ldraw_part.h \
	imp_ldraw_library.c imp_ldraw_library.h \
	imp_ldraw_color.c imp_ldraw_color.h

