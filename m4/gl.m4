dnl Autoconf macro to check for OpenGL/Mesa
dnl Copyright (C) 2001, 2002  Braden McDaniel
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

dnl
dnl @synopsis OV_CHECK_GL
dnl
dnl Check for OpenGL/Mesa. Succeeds if both GL and GLU are found.  If it
dnl succeeds, the required linker flags are included in the output variable
dnl "GL_LIBS".  If the headers "GL/gl.h" and "GL/glu.h" are found, the symbols
dnl HAVE_GL_GL_H and HAVE_GL_GLU_H are defined, respectively.  Otherwise, if
dnl "OpenGL/gl.h" and "OpenGL/glu.h" are found, HAVE_OPENGL_GL_H and
dnl HAVE_OPENGL_GLU_H are defined.  If neither OpenGL nor Mesa is found, "no_gl"
dnl is set to "yes".
dnl
dnl @version $Id: gl.m4,v 1.12 2003-01-06 04:09:42 braden Exp $
dnl @author Braden McDaniel <braden@endoframe.com>
dnl
AC_DEFUN([OV_CHECK_GL],
[AC_REQUIRE([AC_PATH_X])dnl
AC_REQUIRE([ACX_PTHREAD])dnl
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
    GL_LIBS="-L${x_libraries} ${GL_LIBS}"
  fi
fi

AC_LANG_PUSH(C)

ov_save_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${GL_CFLAGS} ${CPPFLAGS}"

AC_CHECK_HEADERS([GL/gl.h OpenGL/gl.h], [break])

AC_CACHE_CHECK([for OpenGL library], [ov_cv_gl],
[ov_cv_gl="no"
ov_save_LIBS="${LIBS}"
LIBS=""
ov_check_libs="-lopengl32 -lGL"
for ov_lib in ${ov_check_libs}; do
 LIBS="${ov_lib} ${GL_LIBS} ${ov_save_LIBS}"
  AC_TRY_LINK([
# ifdef _WIN32
#   include <windows.h>
# endif
# ifdef HAVE_OPENGL_GL_H
#   include <OpenGL/gl.h>
# else
#   include <GL/gl.h>
# endif
],
  [glBegin(0)],
  [ov_cv_gl="${ov_lib}" break])
done
LIBS=${ov_save_LIBS}])

if test "X${ov_cv_gl}" = "Xno"; then
  no_gl="yes"
else
  GL_LIBS="${ov_cv_gl} ${GL_LIBS}"

  AC_CHECK_HEADERS([GL/glu.h OpenGL/glu.h], [break])

  AC_CACHE_CHECK([for OpenGL Utility library], [ov_cv_glu],
  [ov_cv_glu="no"
  ov_save_LIBS="${LIBS}"
  LIBS=""
  ov_check_libs="-lglu32 -lGLU"
  for ov_lib in ${ov_check_libs}; do
    LIBS="${ov_lib} ${GL_LIBS} ${ov_save_LIBS}"
    AC_LANG_PUSH([C++])
    AC_TRY_LINK([
# ifdef _WIN32
#   include <windows.h>
# endif
# ifdef HAVE_OPENGL_GLU_H
#   include <OpenGL/glu.h>
# else
#   include <GL/glu.h>
# endif
],
    [gluBeginCurve(0)],
    [ov_cv_glu="${ov_lib}" break])
    AC_LANG_POP([C++])
  done
  LIBS=${ov_save_LIBS}])
  if test "X${ov_cv_glu}" = "Xno"; then
    no_gl="yes"
  else
    GL_LIBS="${ov_cv_glu} ${GL_LIBS}"
  fi
fi
CPPFLAGS="${ov_save_CPPFLAGS}"
AC_LANG_POP(C)

AC_SUBST([GL_CFLAGS])
AC_SUBST([GL_LIBS])
])dnl
