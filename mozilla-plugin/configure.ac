AC_PREREQ([2.53])
AC_INIT([OpenVRML Mozilla Plugin], [0.16.2],
        [openvrml-develop@lists.sourceforge.net])
AC_COPYRIGHT([Portions copyright 2004, 2005, 2006 Braden McDaniel])
AC_CONFIG_FILES([Makefile]
                [src/Makefile])
AM_INIT_AUTOMAKE([1.7 foreign])

AC_PROG_CXX

AC_DISABLE_STATIC
AC_ENABLE_SHARED
enable_shared=yes
AC_PROG_LIBTOOL

ACX_PTHREAD(, [AC_MSG_FAILURE([pthread support not found])])

PKG_CHECK_MODULES([MOZILLA_PLUGIN],
                  [firefox-plugin],
                  [mozilla_plugin_pkg=firefox-plugin],
                  [PKG_CHECK_MODULES([MOZILLA_PLUGIN],
                                     [mozilla-plugin >= 1.6],
                                     [mozilla_plugin_pkg=mozilla-plugin],
                                     [have_mozilla_plugin=no])])
mozversion=`$PKG_CONFIG --modversion $mozilla_plugin_pkg`
mozprefix=`$PKG_CONFIG --variable=prefix $mozilla_plugin_pkg`
mozlibdir=`$PKG_CONFIG --variable=libdir $mozilla_plugin_pkg`
AC_SUBST([mozlibdir])
PKG_CHECK_MODULES([GTK], [gtk+-2.0], , [have_gtk=no])

if test "X$enable_mozilla_plugin" != "Xno"; then
  if test "X$have_mozilla_plugin" = "Xno"; then
    AC_MSG_FAILURE([Mozilla plug-in development headers are required to build the Mozilla plug-in.])
  fi
  if test "X$have_gtk" = "Xno"; then
    AC_MSG_FAILURE([GTK+ is required to build the Mozilla plug-in])
  fi
fi
AM_CONDITIONAL([ENABLE_MOZILLA_PLUGIN],
  [test "X$enable_mozilla_plugin" != "Xno" -a "X$have_gtk" != "Xno"])

AC_OUTPUT
