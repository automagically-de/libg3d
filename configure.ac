
AC_INIT(configure.ac)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)

# versioning stuff
m4_define([g3d_major_version], [0])
m4_define([g3d_minor_version], [1])
m4_define([g3d_micro_version], [0])
m4_define([g3d_interface_age], [0])
m4_define([g3d_version],
	[g3d_major_version.g3d_minor_version.g3d_micro_version])
m4_define([g3d_binary_age],
	[m4_eval(100 * g3d_minor_version + g3d_micro_version)])
VERSION=g3d_version

m4_define([lt_current],
	[m4_eval(100 * g3d_minor_version + g3d_micro_version - g3d_interface_age)])
m4_define([lt_revision], [g3d_interface_age])
m4_define([lt_age], [m4_eval(g3d_binary_age - g3d_interface_age)])
LTVERSION="lt_current:lt_revision:lt_age"

PACKAGE=libg3d

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)
AC_SUBST(VERSION)
AC_SUBST(LTVERSION)

AC_CONFIG_HEADERS(include/g3d/config.h)

AC_PROG_CC
AC_PROG_CXX
AM_PROG_LEX
AC_HEADER_STDC

AC_DISABLE_STATIC

AM_IS_WIN32
AM_CONDITIONAL(OS_WIN32, test "$os_win32" = yes)

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# gtk-doc
GTK_DOC_CHECK

# glib
AM_PATH_GLIB_2_0(2.0.0,,AC_MSG_ERROR([glib >= 2.0 is needed]), gmodule gobject)

# gdk-pixbuf
AM_PATH_GTK_2_0(2.0.0, have_gtk2=true)
if test "x$have_gtk2" = "xtrue" ; then
GDKPIXBUF_CFLAGS=$GTK_CFLAGS
GDKPIXBUF_LIBS=$GTK_LIBS
AC_SUBST(GDKPIXBUF_CFLAGS)
AC_SUBST(GDKPIXBUF_LIBS)
fi
AM_CONDITIONAL([HAVE_GDKPIXBUF], test "x$have_gtk2" = "xtrue")

# libxml2
AM_PATH_XML2(2.0.0, have_libxml2=true)
if test "x$have_libxml2" = "xtrue" ; then
LIBXML2_CFLAGS=$XML_CPPFLAGS
LIBXML2_LIBS=$XML_LIBS
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)
fi
AM_CONDITIONAL([HAVE_LIBXML2], test "x$have_libxml2" = "xtrue")

# libgsf
PKG_CHECK_MODULES(LIBGSF, libgsf-1 >= 1.0.0,
	[have_libgsf=true], [have_libgsf=false])
if test "x$have_libgsf" = "xtrue" ; then
AC_DEFINE([HAVE_LIBGSF], 1, [have GSF library])
fi
AM_CONDITIONAL([HAVE_LIBGSF], test "x$have_libgsf" = "xtrue")

# lua
PKG_CHECK_MODULES(LUA, lua5.1, [have_lua=true], [have_lua=false])
AM_CONDITIONAL([HAVE_LUA], test "x$have_lua" = "xtrue")

# libmagic
AC_ARG_ENABLE(libmagic,
	[[  --disable-libmagic      disable file type detection using libmagic]],
	[if test "$enableval" = "no" ; then disable_libmagic=true ; fi])
if test "x$disable_libmagic" != "xtrue" ; then
AC_CHECK_HEADER([magic.h], have_magic_h=true)
AC_CHECK_LIB([magic], [magic_open], have_magic_open=true)
if test "x$have_magic_h" = "xtrue" && test "x$have_magic_open" = "xtrue"; then
have_libmagic=true
AC_DEFINE([USE_LIBMAGIC], 1, [use libmagic for file type detection])
AC_DEFINE_DIR([MAGIC_FILENAME], datadir/$PACKAGE/libg3d.magic, [magic file])
MAGIC_LIBS=-lmagic
fi
fi
AC_SUBST(MAGIC_LIBS)
#AM_CONDITIONAL([USE_LIBMAGIC], test "x$have_libmagic" = "xtrue")

# libm
AC_CHECK_LIB(m, sqrt,, [AC_MSG_ERROR([requires math library])])
MATH_LIBS=-lm
AC_SUBST(MATH_LIBS)

# zlib
Z_DIR= Z_LIBS= Z_CFLAGS=
AC_ARG_WITH(zlib,
	[[  --with-zlib=DIR       use libz in DIR]],
	[case $withval in
	 yes|no) ;;
	 *) Z_DIR=$withval
	 	CPPFLAGS="${CPPFLAGS} -I$withval/include"
		LDFLAGS="${LDFLAGS} -L$withval/lib"
		;;
	 esac])
if test "x$with_zlib" != xno; then
	with_zlib=no
	AC_CHECK_HEADER(zlib.h, [AC_CHECK_LIB(z, gzread, [with_zlib=yes])])
fi
if test "x$Z_DIR" != "x"; then
	Z_CFLAGS="-I$Z_DIR/include"
	Z_LIBS="-L$Z_DIR/lib -lz"
else
	Z_LIBS="-lz"
fi
AC_SUBST(Z_CFLAGS)
AC_SUBST(Z_LIBS)
if test "$with_zlib" = "yes"; then
	AC_DEFINE([HAVE_ZLIB], 1, [have zlib])
fi
AM_CONDITIONAL([HAVE_ZLIB], test "$with_zlib" = "yes")

# Debug level (verbosity & optimization)
AC_MSG_CHECKING([for debug level])
AC_ARG_ENABLE(debug, 
	[[  --enable-debug[=LVL]    enable debug level LVL or 1 (default is 0)]],
	[if test "$enableval" = "yes" ; then DEBUG=1; else DEBUG=$enableval; fi],
	DEBUG=0)
AC_MSG_RESULT($DEBUG)
AC_SUBST(DEBUG)

AC_ARG_ENABLE(experimental,
	[[  --enable-experimental   enable experimental and unfinished plugins]],
	[if test "x$enableval" = "xyes" ; then experimental=1
		else experimental=0; fi],
	experimental=0)
AM_CONDITIONAL([EXPERIMENTAL], test "x$experimental" = "x1")

AC_ARG_ENABLE(cxxtest,
	[[  --enable-cxxtest        enable C++ test]],
	[if test "x$enableval" = "xyes" ; then cxxtest=1 else cxxtest=0; fi],
	cxxtest=0)
AM_CONDITIONAL([HAVE_CXX], test "x$cxxtest" = "x1")

AC_ARG_ENABLE(profile,
	[[  --enable-profile        enable gprof profiling]],
	[if test "x$enableval" = "xyes" ; then profile="-pg"
		else profile=""; fi],
	profile="")

if test "$DEBUG" = "0"; then
	DBG_CFLAGS="-O2 $profile"
else
	DBG_CFLAGS="-g $profile"
fi
AC_SUBST(DBG_CFLAGS)

# plugin directories
AC_DEFINE_DIR(PLUGIN_DIR, libdir/$PACKAGE/plugins, [Plugin directory])

PLUGINS_LDFLAGS="-avoid-version -export-dynamic"
AC_SUBST(PLUGINS_LDFLAGS)

AC_OUTPUT(
	Makefile
	doc/Makefile
	doc/api/Makefile
	doc/support_matrix/Makefile
	include/Makefile
	include/g3d/Makefile
	m4/Makefile
	plugins/Makefile
	plugins/image/Makefile
	plugins/import/Makefile
	plugins/import/imp_3dm/Makefile
	plugins/import/imp_3dmf/Makefile
	plugins/import/imp_3ds/Makefile
	plugins/import/imp_ac3d/Makefile
	plugins/import/imp_acf/Makefile
	plugins/import/imp_ar/Makefile
	plugins/import/imp_ase/Makefile
	plugins/import/imp_blend/Makefile
	plugins/import/imp_c4d/Makefile
	plugins/import/imp_cob/Makefile
	plugins/import/imp_dae/Makefile
	plugins/import/imp_dpm/Makefile
	plugins/import/imp_dxf/Makefile
	plugins/import/imp_flt/Makefile
	plugins/import/imp_glb/Makefile
	plugins/import/imp_heightfield/Makefile
	plugins/import/imp_iob/Makefile
	plugins/import/imp_joe/Makefile
	plugins/import/imp_kmz/Makefile
	plugins/import/imp_ldraw/Makefile
	plugins/import/imp_leocad/Makefile
	plugins/import/imp_lua/Makefile
	plugins/import/imp_lwo/Makefile
	plugins/import/imp_max/Makefile
	plugins/import/imp_maya/Makefile
	plugins/import/imp_md2/Makefile
	plugins/import/imp_md3/Makefile
	plugins/import/imp_mesh/Makefile
	plugins/import/imp_msfsmdl/Makefile
	plugins/import/imp_nff/Makefile
	plugins/import/imp_obj/Makefile
	plugins/import/imp_osm/Makefile
	plugins/import/imp_pie/Makefile
	plugins/import/imp_ply/Makefile
	plugins/import/imp_q3o/Makefile
	plugins/import/imp_r4/Makefile
	plugins/import/imp_rbh/Makefile
	plugins/import/imp_ror/Makefile
	plugins/import/imp_skp/Makefile
	plugins/import/imp_stl/Makefile
	plugins/import/imp_ta/Makefile
	plugins/import/imp_test/Makefile
	plugins/import/imp_vrml/Makefile
	plugins/import/imp_vrml2/Makefile
	src/Makefile
	programs/Makefile
	tests/Makefile
	libg3d.pc
)

