pluginimpdir = $(pkglibdir)/plugins/import

pluginimp_LTLIBRARIES = imp_joe.la

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

imp_joe_la_SOURCES = imp_joe.c
imp_joe_la_LIBADD = $(top_builddir)/src/libg3d.la @GLIB_LIBS@
