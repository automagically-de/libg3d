pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
pluginimp_LTLIBRARIES = imp_sh3d.la
endif

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include

AM_LDFLAGS = \
	-no-undefined -module \
	@PLUGINS_LDFLAGS@

LIBTOOL += --quiet

imp_sh3d_la_SOURCES = imp_sh3d.c \
	javaser.h javaser.c
imp_sh3d_la_LIBADD = $(top_builddir)/src/libg3d.la @GLIB_LIBS@
