pluginimpdir = $(pkglibdir)/plugins/import

if EXPERIMENTAL
if HAVE_LUA
pluginimp_LTLIBRARIES = imp_lua.la
endif
endif

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-DG_LOG_DOMAIN=\"LibG3D\" \
	@DBG_CFLAGS@ -DDEBUG=@DEBUG@ \
	@GLIB_CFLAGS@ @LUA_CFLAGS@

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include

AM_LDFLAGS = \
	-no-undefined -module \
	@PLUGINS_LDFLAGS@

LIBTOOL += --quiet

imp_lua_la_SOURCES = imp_lua.c \
	imp_lua_funcs.h imp_lua_funcs.c \
	imp_lua_gc.c imp_lua_gc.h
imp_lua_la_LIBADD = $(top_builddir)/src/libg3d.la @GLIB_LIBS@ @LUA_LIBS@
