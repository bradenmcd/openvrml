#!/bin/sh

aclocal -I /usr/local/share/aclocal
automake --add-missing
autoreconf
./configure $@ \
	&& echo "" \
	&& echo Now type \`make\' to compile lookat \
	&& echo ""
