noinst_PROGRAMS = \
	test_lib \
	test_buf \
	test_vector

if HAVE_CXX
noinst_PROGRAMS += test_cxx
endif

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-I$(top_srcdir)/include \
	@GLIB_CFLAGS@

AM_CXXFLAGS = \
	-Wall \
	-I$(top_srcdir)/include \
	@GLIB_CFLAGS@

test_lib_DEPENDENCIES = ../src/libg3d.la
test_lib_SOURCES = test_lib.c
test_lib_LDADD = ../src/libg3d.la -lm @GLIB_LIBS@ @MAGIC_LIBS@
test_buf_DEPENDENCIES = ../src/libg3d.la
test_buf_SOURCES = test_buf.c
test_buf_LDADD = ../src/libg3d.la -lm @GLIB_LIBS@ @MAGIC_LIBS@
test_cxx_DEPENDENCIES = ../src/libg3d.la
test_cxx_SOURCES = test_cxx.cxx
test_cxx_LDADD = ../src/libg3d.la -lm @GLIB_LIBS@ @MAGIC_LIBS@
test_vector_DEPENDENCIES = ../src/libg3d.la
test_vector_SOURCES = test_vector.c
test_vector_LDADD = ../src/libg3d.la -lm @GLIB_LIBS@ @MAGIC_LIBS@

