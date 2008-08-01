pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
if HAVE_LIBXML2
pluginimp_LTLIBRARIES = imp_osm.la
endif
endif

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@ @LIBXML2_CFLAGS@

INCLUDES = -I$(top_builddir)/include

AM_LDFLAGS = \
	-module \
	@PLUGINS_LDFLAGS@

LIBADD = \
	@GLIB_LIBS@

LIBTOOL += --quiet

imp_osm_la_SOURCES = imp_osm.c
imp_osm_la_LIBADD = @GLIB_LIBS@ @LIBXML2_LIBS@

