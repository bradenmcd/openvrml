libtoolize --automake
aclocal -I ./macros
autoconf
autoheader
automake -a
