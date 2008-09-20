pluginimpdir = $(pkglibdir)/plugins/import

pluginimp_LTLIBRARIES = imp_glb.la

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@

INCLUDES = -I$(top_srcdir)/include

AM_LDFLAGS = \
	-module \
	@PLUGINS_LDFLAGS@

LIBADD = \
	@GLIB_LIBS@

LIBTOOL += --quiet

imp_glb_la_SOURCES = imp_glb.c

