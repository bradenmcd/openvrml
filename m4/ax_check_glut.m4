dnl @synopsis AX_CHECK_GLUT
dnl
dnl Check for GLUT.  If GLUT is found, the required compiler and linker flags
dnl are included in the output variables "GLUT_CFLAGS" and "GLUT_LIBS",
dnl respectively, and the shell variable "no_glut" is set to the empty
dnl string.  Also, if the header "GL/glut.h" is found, the symbol
dnl "HAVE_GL_GLUT_H" is defined; otherwise, if the header "GLUT/glut.h" is
dnl found, "HAVE_GLUT_GLUT_H" is defined. If GLUT is not found, "no_glut" is
dnl set to "yes".
dnl
dnl @copyright (C) 2003 Braden McDaniel
dnl @license GNU GPL
dnl @version $Id: ax_check_glut.m4,v 1.1 2003-01-07 08:18:00 braden Exp $
dnl @author Braden McDaniel <braden@endoframe.com>
dnl
AC_DEFUN([AX_CHECK_GLUT],
[AC_REQUIRE([AX_CHECK_GL])dnl
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
  
ax_save_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${GLUT_CFLAGS} ${CPPFLAGS}"

AC_CHECK_HEADERS([GL/glut.h GLUT/glut.h], [break])

AC_CACHE_CHECK([for GLUT library], [ax_cv_check_glut_libglut],
[ax_cv_check_glut_libglut="no"
ax_save_LIBS="${LIBS}"
LIBS=""
ax_check_libs="-lglut32 -lglut"
for ax_lib in ${ax_check_libs}; do
  LIBS="${ax_lib} ${GLUT_LIBS} ${ax_save_LIBS}"
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
  [ax_cv_check_glut_libglut="${ax_lib}" break])
  
done
LIBS=${ax_save_LIBS}
])
CPPFLAGS="${ax_save_CPPFLAGS}"
AC_LANG_POP(C)

if test "X${ax_cv_check_glut_libglut}" = "Xno"; then
  no_glut="yes"
else
  GLUT_LIBS="${ax_cv_check_glut_libglut} ${GLUT_LIBS}"
fi

AC_SUBST([GLUT_CFLAGS])
AC_SUBST([GLUT_LIBS])
])dnl
