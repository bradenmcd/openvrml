#!/bin/sh

aclocal -I /usr/local/share/aclocal -I ./macros
automake --add-missing
autoreconf
./configure $@ \
	&& echo "" \
	&& echo Now type \`make\' to compile libvrml97gl \
	&& echo ""
