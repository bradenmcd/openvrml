dnl @synopsis AX_CHECK_GL
dnl
dnl Check for OpenGL/Mesa. Succeeds if both GL and GLU are found.  If it
dnl succeeds, the required linker flags are included in the output variable
dnl "GL_LIBS".  If the headers "GL/gl.h" and "GL/glu.h" are found, the symbols
dnl HAVE_GL_GL_H and HAVE_GL_GLU_H are defined, respectively.  Otherwise, if
dnl "OpenGL/gl.h" and "OpenGL/glu.h" are found, HAVE_OPENGL_GL_H and
dnl HAVE_OPENGL_GLU_H are defined.  If neither OpenGL nor Mesa is found,
dnl "no_gl" is set to "yes".
dnl
dnl @copyright (C) 2003 Braden McDaniel
dnl @license GNU GPL
dnl @version $Id: ax_check_gl.m4,v 1.4 2003-07-25 23:43:20 braden Exp $
dnl @author Braden McDaniel <braden@endoframe.com>
dnl
AC_DEFUN([AX_CHECK_GL],
[AC_REQUIRE([AC_PATH_X])dnl
AC_REQUIRE([ACX_PTHREAD])dnl

#
# There isn't a reliable way to know we should use the Apple OpenGL framework
# without a configure option.  A Mac OS X user may have installed an
# alternative GL implementation (e.g., Mesa), which may or may not depend on X.
#
AC_ARG_WITH([apple-opengl-framework],
            [AC_HELP_STRING([--with-apple-opengl-framework],
                            [use Apple OpenGL framework (Mac OS X only)])])
if test "X$with_apple_opengl_framework" = "Xyes"; then
  AC_DEFINE([HAVE_APPLE_OPENGL_FRAMEWORK], [1],
            [Use the Apple OpenGL framework.])
  GL_CFLAGS="-framework OpenGL"
else
  GL_CFLAGS="${PTHREAD_CFLAGS}"
  GL_LIBS="${PTHREAD_LIBS} -lm"

  #
  # Use x_includes and x_libraries if they have been set (presumably by
  # AC_PATH_X).
  #
  if test "X$no_x" != "Xyes"; then
    if test -n "$x_includes"; then
      GL_CFLAGS="-I${x_includes} ${GL_CFLAGS}"
    fi
    if test -n "$x_libraries"; then
      GL_LIBS="-L${x_libraries} -lX11 ${GL_LIBS}"
    fi
  fi

  AC_LANG_PUSH(C)

  AC_CHECK_HEADERS([windows.h])

  AC_CACHE_CHECK([for OpenGL library], [ax_cv_check_gl_libgl],
  [ax_cv_check_gl_libgl="no"
  ax_save_CPPFLAGS="${CPPFLAGS}"
  CPPFLAGS="${GL_CFLAGS} ${CPPFLAGS}"
  ax_save_LIBS="${LIBS}"
  LIBS=""
  ax_check_libs="-lopengl32 -lGL"
    for ax_lib in ${ax_check_libs}; do
    if test "X$CC" = "Xcl"; then
      ax_try_lib=`echo $ax_lib | sed -e 's/^-l//' -e 's/$/.lib/'`
    else
      ax_try_lib="${ax_lib}"
    fi
    LIBS="${ax_try_lib} ${GL_LIBS} ${ax_save_LIBS}"
    AC_TRY_LINK([
# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# include <GL/gl.h>
],
    [glBegin(0)],
    [ax_cv_check_gl_libgl="${ax_try_lib}"; break])
  done
  LIBS=${ax_save_LIBS}
  CPPFLAGS=${ax_save_CPPFLAGS}])

  if test "X${ax_cv_check_gl_libgl}" = "Xno"; then
    no_gl="yes"
  else
    GL_LIBS="${ax_cv_check_gl_libgl} ${GL_LIBS}"

    AC_CACHE_CHECK([for OpenGL Utility library], [ax_cv_check_gl_libglu],
    [ax_cv_check_gl_libglu="no"
    ax_save_CPPFLAGS="${CPPFLAGS}"
    CPPFLAGS="${GL_CFLAGS} ${CPPFLAGS}"
    ax_save_LIBS="${LIBS}"
    LIBS=""
    ax_check_libs="-lglu32 -lGLU"
    for ax_lib in ${ax_check_libs}; do
      if test "X$CC" = "Xcl"; then
        ax_try_lib=`echo $ax_lib | sed -e 's/^-l//' -e 's/$/.lib/'`
      else
        ax_try_lib="${ax_lib}"
      fi
      LIBS="${ax_try_lib} ${GL_LIBS} ${ax_save_LIBS}"
      #
      # libGLU typically links with libstdc++ on POSIX platforms. However, setting the
      # language to C++ means that test program source is named "conftest.cc"; and
      # Microsoft cl doesn't know what to do with such a file.
      #
      if test "X$CXX" != "Xcl"; then
        AC_LANG_PUSH([C++])
      fi
      AC_TRY_LINK([
# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# include <GL/glu.h>
],
      [gluBeginCurve(0)],
      [ax_cv_check_gl_libglu="${ax_try_lib}"; break])
      if test "X$CXX" != "Xcl"; then
        AC_LANG_POP([C++])
      fi
    done
    LIBS=${ax_save_LIBS}
    CPPFLAGS=${ax_save_CPPFLAGS}])
    if test "X${ax_cv_check_gl_libglu}" = "Xno"; then
      no_gl="yes"
    else
      GL_LIBS="${ax_cv_check_gl_libglu} ${GL_LIBS}"
    fi
  fi
  AC_LANG_POP(C)
fi

AC_SUBST([GL_CFLAGS])
AC_SUBST([GL_LIBS])
])dnl
