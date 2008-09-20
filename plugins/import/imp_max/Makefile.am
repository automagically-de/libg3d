pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
if HAVE_LIBGSF
pluginimp_LTLIBRARIES = imp_max.la
endif
endif

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@
	@LIBGSF_CFLAGS@

INCLUDES = -I$(top_srcdir)/include

AM_LDFLAGS = \
	-module \
	@PLUGINS_LDFLAGS@

LIBTOOL += --quiet

imp_max_la_SOURCES = imp_max.c \
	imp_max_chunks.h imp_max_callbacks.c imp_max_callbacks.h
imp_max_la_LIBADD = @GLIB_LIBS@ @LIBGSF_LIBS@

