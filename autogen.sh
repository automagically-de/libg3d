#!/bin/sh

mkdir -p config

echo "++ aclocal -I m4/ $ACLOCAL_FLAGS $INCLUDES"
aclocal -I m4/ $ACLOCAL_FLAGS $INCLUDES

echo "++ autoheader"
autoheader

echo "++ libtoolize --automake --copy --force"
libtoolize --automake --copy --force

echo "++ gtkdocize --copy"
gtkdocize --copy

echo "++ automake --gnu --add-missing --copy"
automake --gnu --add-missing --copy

echo "++ autoconf"
autoconf

configure_options="--enable-debug --enable-experimental --enable-gtk-doc"

if [ "$1" = "-conf" ]; then
	shift
	echo "++ ./configure $configure_options $@"
	./configure $configure_options "$@"
fi
