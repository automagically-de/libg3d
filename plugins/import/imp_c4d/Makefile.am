pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
pluginimp_LTLIBRARIES = imp_c4d.la
endif

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@

AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = \
	-no-undefined -module \
	@PLUGINS_LDFLAGS@

LIBTOOL += --quiet

imp_c4d_la_SOURCES = imp_c4d.c
imp_c4d_la_LIBADD = $(top_builddir)/src/libg3d.la @GLIB_LIBS@
