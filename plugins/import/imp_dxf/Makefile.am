pluginimpdir = $(pkglibdir)/plugins/import

pluginimp_LTLIBRARIES = \
	imp_dxf.la

LIBTOOL += --quiet

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@

INCLUDES = -I$(top_builddir)/include

AM_LDFLAGS = \
	-module \
	@PLUGINS_LDFLAGS@

LIBADD = \
	@GLIB_LIBS@

imp_dxf_la_SOURCES = imp_dxf.c imp_dxf.h \
	imp_dxf_section.c imp_dxf_section.h imp_dxf_vars.h

