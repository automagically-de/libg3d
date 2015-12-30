pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
pluginimp_LTLIBRARIES = imp_msfsmdl.la
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

imp_msfsmdl_la_SOURCES = imp_msfsmdl.c \
	imp_msfsmdl_callbacks.c \
	imp_msfsmdl_bgl.c
imp_msfsmdl_la_LIBADD = $(top_builddir)/src/libg3d.la @GLIB_LIBS@
