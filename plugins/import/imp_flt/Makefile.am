pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
pluginimp_LTLIBRARIES = imp_flt.la
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

imp_flt_la_SOURCES = imp_flt.c imp_flt.h \
	imp_flt_opcodes.h \
	imp_flt_callbacks.c imp_flt_callbacks.h

