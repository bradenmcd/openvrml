libtoolize --automake
aclocal -I ./m4 -I ./libvrml97core/m4 -I ./libvrml97gl/m4
autoconf
autoheader
automake

