#!/bin/sh

autoheader
aclocal
automake --add-missing
autoreconf
./configure $@ \
	&& echo "" \
	&& echo Now type \`make\' to compile LibVRML97 \
	&& echo ""
