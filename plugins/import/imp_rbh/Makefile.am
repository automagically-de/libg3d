pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
pluginimp_LTLIBRARIES = imp_rbh.la
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

imp_rbh_la_SOURCES = imp_rbh.c imp_rbh_chunks.h \
	imp_rbh_callbacks.c imp_rbh_callbacks.h

