pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
pluginimp_LTLIBRARIES = imp_blend.la
endif

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

imp_blend_la_SOURCES = imp_blend.c imp_blend_def.h \
	imp_blend_sdna.c imp_blend_sdna.h \
	imp_blend_read.c imp_blend_read.h

