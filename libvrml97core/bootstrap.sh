#!/bin/sh

libtoolize --automake
aclocal
autoheader
automake --add-missing
autoconf
./configure $@ \
	&& echo "" \
	&& echo Now type \`make\' to compile LibVRML97 \
	&& echo ""
