ACLOCAL_AMFLAGS = -I m4
SUBDIRS = . doc m4 include src plugins programs tests

pkgconfigdir=${libdir}/pkgconfig

pkgconfig_DATA = libg3d.pc
pkgdata_DATA = libg3d.magic

deb:
	fakeroot dpkg-buildpackage

EXTRA_DIST = libg3d.magic
