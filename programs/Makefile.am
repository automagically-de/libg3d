bin_PROGRAMS = g3d-stat g3d-img2ppm

AM_CFLAGS = \
	-Wall -ansi -pedantic \
	-I$(top_srcdir)/include \
	@GLIB_CFLAGS@

g3d_stat_SOURCES = g3d-stat.c
g3d_stat_DEPENDENCIES =../src/libg3d.la
g3d_stat_LDADD = ../src/libg3d.la -lm @GLIB_LIBS@ @MAGIC_LIBS@

g3d_img2ppm_SOURCES = g3d-img2ppm.c
g3d_img2ppm_DEPENDENCIES =../src/libg3d.la
g3d_img2ppm_LDADD = ../src/libg3d.la -lm @GLIB_LIBS@ @MAGIC_LIBS@
