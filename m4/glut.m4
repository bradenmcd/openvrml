# Autoconf macro to check for GLUT
# Copyright (C) 2001  Braden McDaniel
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#
# OV_CHECK_GLUT
# -------------
# Check for GLUT. If GLUT is found, the required linker flags are included in
# the output variable `GL_LIBS' and the shell variable `no_glut' is set to the
# empty string. Also, the proper GLUT library header inclusion is defined in
# `OPENVRML_GLUT_H'. If GLUT is not found, `no_glut' is set to `yes'.
#
AC_DEFUN(OV_CHECK_GLUT,
[AC_REQUIRE([OV_CHECK_GL])
AC_REQUIRE([AC_PATH_XTRA])

AC_LANG_PUSH(C)
ov_have_glut=no
AC_CHECK_LIB(glut, glutMainLoop,
             [AC_CHECK_HEADER(GL/glut.h,
                              [ov_have_glut=yes
                               AC_DEFINE(OPENVRML_GLUT_H, [<GL/glut.h>],
                                         [Header for GLUT])],
                              [# Mac OS X thinks different
                               AC_CHECK_HEADER(GLUT/glut.h,
                                               [ov_have_glut=yes
                                                AC_DEFINE(OPENVRML_GLUT_H,
                                                          [<GLUT/glut.h>],
                                                          [Header for GLUT])])])],
             , ${GL_LIBS} ${X_PRE_LIBS} ${X_LIBS} ${X_EXTRA_LIBS})
AC_LANG_POP(C)
    
if test "X${ov_have_glut}" = Xyes; then
  no_glut=""
  GLUT_LIBS="-lglut"
else
  no_glut="yes"
  GLUT_LIBS=""
fi
AC_SUBST(GLUT_LIBS)
])
