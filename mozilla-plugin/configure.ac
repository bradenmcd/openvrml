AC_PREREQ([2.53])
AC_INIT([OpenVRML Mozilla Plugin], [0.14.99],
        [openvrml-develop@lists.sourceforge.net])
AC_COPYRIGHT([Portions copyright 2000, 2001, 2002, 2003, 2004 Braden McDaniel])
AC_CONFIG_FILES([Makefile]
                [src/Makefile])
AM_INIT_AUTOMAKE([1.7 foreign])

AC_PROG_CXX

AC_DISABLE_STATIC
AC_ENABLE_SHARED
enable_shared=yes
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([MOZILLA_PLUGIN], [mozilla-plugin >= 1.6])
mozversion=`$PKG_CONFIG --modversion mozilla-plugin`
mozprefix=`$PKG_CONFIG --variable=prefix mozilla-plugin`
mozlibdir=`$PKG_CONFIG --variable=libdir mozilla-plugin`
mozincludedir=`$PKG_CONFIG --variable=includedir mozilla-plugin`
AC_PATH_PROG([XPIDL], [xpidl], , [${mozlibdir}])
if test -z "$XPIDL"; then
  AC_MSG_FAILURE([xpidl not found])
fi
mozpluginsdir=`echo "$mozlibdir" | sed s,mozilla-[^/]*$,mozilla/plugins,`
mozidldir="$mozprefix/share/idl/mozilla-$mozversion"
AC_SUBST([mozpluginsdir])
AC_SUBST([mozincludedir])
AC_SUBST([mozidldir])
PKG_CHECK_MODULES([GTKGLEXT], [gtkglext-1.0])

AC_OUTPUT
