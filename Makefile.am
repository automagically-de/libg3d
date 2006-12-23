SUBDIRS = . doc m4 include plugins src tests

pkgconfigdir=${libdir}/pkgconfig

pkgconfig_DATA = libg3d.pc
pkglib_DATA = libg3d.magic

deb:
	fakeroot dpkg-buildpackage

EXTRA_DIST = libg3d.magic
