# Configure paths for GtkGLExt
# Naofumi Yasufuku
#
# Shamelessly stolen from gtk-2.0.m4
# Configure paths for GTK+
# Owen Taylor     1997-2001

dnl AM_PATH_GTKGLEXT_1_0([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for GtkGLExt, and define GTKGLEXT_CFLAGS and GTKGLEXT_LIBS.
dnl
AC_DEFUN([AM_PATH_GTKGLEXT_1_0],
[ no_gtkglext=""
  GTKGLEXT_CFLAGS=""
  GTKGLEXT_LIBS=""

  # GtkGLExt pkg-config module
  gtkglext_module=gtkglext-1.0

  # minimum GtkGLExt version
  gtkglext_min_version=ifelse([$1], ,1.0.0,$1)

  # minimum pkg-config version
  pkgconfig_min_version=0.7

AC_ARG_ENABLE(gtkglext-test,
[  --disable-gtkglext-test do not try to compile and run a test GtkGLExt program],
, enable_gtkglext_test=yes)

dnl 
dnl Get the cflags and libraries from pkg-config
dnl
  pkg_config_modules=$gtkglext_module

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

  if test x$PKG_CONFIG != xno ; then
    if pkg-config --atleast-pkgconfig-version $pkgconfig_min_version ; then
      :
    else
      echo ""
      echo "*** pkg-config too old; version $pkgconfig_min_version or better required."
      no_gtkglext=yes
      PKG_CONFIG=no
    fi
  else
    no_gtkglext=yes
  fi

  AC_MSG_CHECKING(for GtkGLExt - version >= $gtkglext_min_version)

  if test x$PKG_CONFIG != xno ; then
    ## don't try to run the test against uninstalled libtool libs
    if $PKG_CONFIG --uninstalled $pkg_config_modules; then
      echo ""
      echo "Will use uninstalled version of GtkGLExt found in PKG_CONFIG_PATH"
      enable_gtkglext_test=no
    fi

    if $PKG_CONFIG --atleast-version $gtkglext_min_version $pkg_config_modules; then
      :
    else
      echo ""
      echo "*** pkg-config cannot find $gtkglext_module >= $gtkglext_min_version"
      echo "*** Set the environment variable PKG_CONFIG_PATH to point to the correct"
      echo "*** configuration files."
      no_gtkglext=yes
      enable_gtkglext_test=no
    fi
  fi

  if test x"$no_gtkglext" = x ; then
    GTKGLEXT_CFLAGS=`$PKG_CONFIG --cflags $pkg_config_modules`
    GTKGLEXT_LIBS=`$PKG_CONFIG --libs $pkg_config_modules`
    gtkglext_config_major_version=`$PKG_CONFIG --modversion $gtkglext_module | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    gtkglext_config_minor_version=`$PKG_CONFIG --modversion $gtkglext_module | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    gtkglext_config_micro_version=`$PKG_CONFIG --modversion $gtkglext_module | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_gtkglext_test" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GTKGLEXT_CFLAGS"
      LIBS="$GTKGLEXT_LIBS $LIBS"
dnl
dnl Now check if the installed GtkGLExt is sufficiently new. (Also sanity
dnl checks the results of pkg-config to some extent)
dnl
      rm -f conf.gtkgltest
      AC_TRY_RUN([
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.gtkgltest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup ("$gtkglext_min_version");
  if (sscanf (tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3)
    {
      printf ("%s, bad version string\n", "$gtkglext_min_version");
      exit (1);
    }

  if ((gtkglext_major_version != $gtkglext_config_major_version) ||
      (gtkglext_minor_version != $gtkglext_config_minor_version) ||
      (gtkglext_micro_version != $gtkglext_config_micro_version))
    {
      printf ("\n*** 'pkg-config --modversion $gtkglext_module' returned %d.%d.%d, but GtkGLExt\n",
              $gtkglext_config_major_version, $gtkglext_config_minor_version, $gtkglext_config_micro_version);
      printf ("*** library %d.%d.%d was found!\n",
              gtkglext_major_version, gtkglext_minor_version, gtkglext_micro_version);
      printf ("***\n");
      printf ("*** If pkg-config was correct, then it is best to remove the old version\n");
      printf ("*** of GtkGLExt. You may also be able to fix the error by modifying your\n");
      printf ("*** LD_LIBRARY_PATH enviroment variable, or by editing /etc/ld.so.conf.\n");
      printf ("*** Make sure you have run ldconfig if that is required on your system.\n");
      printf ("***\n");
      printf ("*** If pkg-config was wrong, set the environment variable PKG_CONFIG_PATH\n");
      printf ("*** to point to the correct configuration files.\n");
    } 
  else if ((gtkglext_major_version != GTKGLEXT_MAJOR_VERSION) ||
	   (gtkglext_minor_version != GTKGLEXT_MINOR_VERSION) ||
           (gtkglext_micro_version != GTKGLEXT_MICRO_VERSION))
    {
      printf ("\n*** GtkGLExt header files (version %d.%d.%d) do not match library (version %d.%d.%d).\n",
              GTKGLEXT_MAJOR_VERSION, GTKGLEXT_MINOR_VERSION, GTKGLEXT_MICRO_VERSION,
              gtkglext_major_version, gtkglext_minor_version, gtkglext_micro_version);
    }
  else
    {
      if ((gtkglext_major_version > major) ||
          ((gtkglext_major_version == major) && (gtkglext_minor_version > minor)) ||
          ((gtkglext_major_version == major) && (gtkglext_minor_version == minor) && (gtkglext_micro_version >= micro)))
        {
          return 0;
        }
      else
        {
          printf ("\n*** An old version of GtkGLExt library (%d.%d.%d) was found.\n",
                  gtkglext_major_version, gtkglext_minor_version, gtkglext_micro_version);
          printf ("*** You need a version of GtkGLExt newer than %d.%d.%d. The latest version of\n",
                  major, minor, micro);
          printf ("*** GtkGLExt is always available from http://gtkglext.sourceforge.net/.\n");
          printf ("***\n");
          printf ("*** If you have already installed a sufficiently new version, this error\n");
          printf ("*** probably means that the wrong copy of the pkg-config shell script is\n");
          printf ("*** being found. The easiest way to fix this is to remove the old version\n");
          printf ("*** of GtkGLExt, but you can also set the PKG_CONFIG environment to point\n");
          printf ("*** to the correct copy of pkg-config. (In this case, you will have to\n");
          printf ("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
          printf ("*** so that the correct libraries are found at run-time)\n");
        }
    }
  return 1;
}
],, no_gtkglext=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_gtkglext" = x ; then
     AC_MSG_RESULT(yes (version $gtkglext_config_major_version.$gtkglext_config_minor_version.$gtkglext_config_micro_version))
     ifelse([$2], , :, [$2])
  else
     AC_MSG_RESULT(no)
     if test "$PKG_CONFIG" = "no" ; then
       echo "*** A new enough version of pkg-config was not found."
       echo "*** See http://www.freedesktop.org/software/pkgconfig/."
     elif test "x$enable_gtkglext_test" = "xyes" ; then
       if test -f conf.gtkgltest ; then
        :
       else
          echo "*** Could not run GtkGLExt test program, checking why..."
	  ac_save_CFLAGS="$CFLAGS"
	  ac_save_LIBS="$LIBS"
          CFLAGS="$CFLAGS $GTKGLEXT_CFLAGS"
          LIBS="$LIBS $GTKGLEXT_LIBS"
          AC_TRY_LINK([
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <stdio.h>
],      [ return ((gtkglext_major_version) || (gtkglext_minor_version) || (gtkglext_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GtkGLExt or finding the wrong"
          echo "*** version of GtkGLExt. If it is not finding GtkGLExt, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GtkGLExt is incorrectly installed."])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GTKGLEXT_CFLAGS=""
     GTKGLEXT_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GTKGLEXT_CFLAGS)
  AC_SUBST(GTKGLEXT_LIBS)
  rm -f conf.gtkgltest
])

dnl AC_GTKGLEXT_SUPPORTS_MULTIHEAD([ACTION-IF-SUPPORTED [, ACTION-IF-NOT-SUPPORTED]])
dnl Checks whether GtkGLExt supports multihead.
dnl
AC_DEFUN([AC_GTKGLEXT_SUPPORTS_MULTIHEAD],
[ AC_CACHE_CHECK([whether GtkGLExt supports multihead],
                 [ac_cv_gtkglext_supports_multihead],
                 [AC_TRY_LINK([#include <gdk/gdkglquery.h>], 
                              [gdk_gl_query_extension_for_display(gdk_display_get_default());],
                              [ac_cv_gtkglext_supports_multihead=yes],
                              [ac_cv_gtkglext_supports_multihead=no])])
  if test "x$ac_cv_gtkglext_supports_multihead" = "xyes" ; then
    ifelse([$1], , :, [$1])
  else
    ifelse([$2], , :, [$2])
  fi
])
