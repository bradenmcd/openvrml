dnl Autoconf macro to check for GLUT
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
dnl @synopsis OV_CHECK_GLUT
dnl
dnl Check for GLUT.  If GLUT is found, the required compiler and linker flags
dnl are included in the output variables "GLUT_CFLAGS" and "GLUT_LIBS",
dnl respectively, and the shell variable "no_glut" is set to the empty
dnl string.  Also, if the header "GL/glut.h" is found, the symbol
dnl "HAVE_GL_GLUT_H" is defined; otherwise, if the header "GLUT/glut.h" is
dnl found, "HAVE_GLUT_GLUT_H" is defined. If GLUT is not found, "no_glut" is
dnl set to "yes".
dnl
dnl @version $Id: glut.m4,v 1.7 2003-01-06 04:09:42 braden Exp $
dnl @author Braden McDaniel <braden@endoframe.com>
dnl
AC_DEFUN(OV_CHECK_GLUT,
[AC_REQUIRE([OV_CHECK_GL])dnl
AC_REQUIRE([AC_PATH_XTRA])dnl
GLUT_LIBS="${GL_LIBS}"

#
# If X is present, assume GLUT depends on it.
#
if test "X${no_x}" != "Xyes"; then
  GLUT_CFLAGS="${GL_CFLAGS}"
  GLUT_LIBS="${GLUT_LIBS} ${X_PRE_LIBS} -lXmu -lXi -lX11 ${X_EXTRA_LIBS}"
fi

AC_LANG_PUSH(C)
  
ov_save_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${GLUT_CFLAGS} ${CPPFLAGS}"

AC_CHECK_HEADERS([GL/glut.h GLUT/glut.h], [break])

AC_CACHE_CHECK([for GLUT library], [ov_cv_glut],
[ov_cv_glut="no"
ov_save_LIBS="${LIBS}"
LIBS=""
ov_check_libs="-lglut32 -lglut"
for ov_lib in ${ov_check_libs}; do
  LIBS="${ov_lib} ${GLUT_LIBS} ${ov_save_LIBS}"
  AC_TRY_LINK([
# ifdef _WIN32
#   include <windows.h>
# endif
# ifdef HAVE_OPENGL_GLUT_H
#   include <OpenGL/glut.h>
# else
#   include <GL/glut.h>
# endif
],
  [glutMainLoop()],
  [ov_cv_glut="${ov_lib}" break])
  
done
LIBS=${ov_save_LIBS}
])
CPPFLAGS="${ov_save_CPPFLAGS}"
AC_LANG_POP(C)

if test "X${ov_cv_glut}" = "Xno"; then
  no_glut="yes"
else
  GLUT_LIBS="${ov_cv_glut} ${GLUT_LIBS}"
fi

AC_SUBST([GLUT_CFLAGS])
AC_SUBST([GLUT_LIBS])
])
