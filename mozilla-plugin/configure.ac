AC_PREREQ([2.53])
AC_INIT([OpenVRML Mozilla Plugin], [0.15.99],
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

PKG_CHECK_MODULES([MOZILLA_PLUGIN], [mozilla-plugin >= 1.6],
                  , [have_mozilla_plugin=no])
mozversion=`$PKG_CONFIG --modversion mozilla-plugin`
mozprefix=`$PKG_CONFIG --variable=prefix mozilla-plugin`
mozlibdir=`$PKG_CONFIG --variable=libdir mozilla-plugin`
AC_ARG_VAR([XPIDL], [Mozilla XPCOM IDL compiler])
AC_ARG_VAR([XPIDLFLAGS], [Mozilla XPCOM IDL compiler flags])
AC_PATH_PROG([XPIDL], [xpidl], , [${mozlibdir}])
if test -z "$XPIDL"; then
  AC_MSG_FAILURE([xpidl not found])
fi
mozidldir="$mozprefix/share/idl/mozilla-$mozversion"
MOZILLA_XPIDLFLAGS="-I $mozidldir"
AC_SUBST([MOZILLA_XPIDLFLAGS])
PKG_CHECK_MODULES([GTKGLEXT], [gtkglext-1.0], , [have_gtkglext=no])

if test "X$enable_mozilla_plugin" != "Xno"; then
  if test "X$have_mozilla_plugin" = "Xno"; then
    AC_MSG_FAILURE([Mozilla plug-in development headers are required to build the Mozilla plug-in.])
  fi
  if test "X$have_gtkglext" = "Xno"; then
    AC_MSG_FAILURE([GtkGLExt is required to build the Mozilla plug-in])
  fi
fi
AM_CONDITIONAL([ENABLE_MOZILLA_PLUGIN],
  [test "X$enable_mozilla_plugin" != "Xno" -a "X$have_gtkglext" != "Xno"])

AC_OUTPUT
