libtoolize --automake
aclocal -I ./m4
autoconf
autoheader
automake -a
