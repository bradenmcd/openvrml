# Autoconf macro to check for OpenGL/Mesa
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
# OV_CHECK_GL
# -----------
# Check for OpenGL/Mesa. Succeeds if both libGL and libGLU are found. If it
# succeeds, the required linker flags are included in the output variable
# `GL_LIBS', and the shell variable `no_gl' is set to the empty string. Also,
# the proper OpenGL library header inclusion is defined in `OPENVRML_GL_H', and
# the proper OpenGL Utility library header inclusion is defined in
# `OPENVRML_GLU_H'. If neither OpenGL nor Mesa is found, `no_gl' is set to
# `yes'.
#
AC_DEFUN(OV_CHECK_GL,
[GL_LIBS="-lpthread"
AC_LANG_PUSH(C)
ov_have_gl=no
AC_CHECK_LIB(GL, glAccum,
             [GL_LIBS="-lGL ${GL_LIBS}"
              AC_CHECK_HEADER(GL/gl.h,
                              [ov_have_gl=yes
                               AC_DEFINE(OPENVRML_GL_H, [<GL/gl.h>],
                                         [Header for OpenGL])],
                              [# Mac OS X thinks different
                               AC_CHECK_HEADER(OpenGL/gl.h,
                                               [ov_have_gl=yes
                                                AC_DEFINE(OPENVRML_GL_H,
                                                          [<OpenGL/gl.h>],
                                                          [Header for OpenGL])])])],
             , ${GL_LIBS})
    
AC_CHECK_LIB(GLU, gluBeginCurve,
             [GL_LIBS="-lGLU ${GL_LIBS}"
              AC_CHECK_HEADER(GL/glu.h,
                              [ov_have_gl=yes
                               AC_DEFINE(OPENVRML_GLU_H, [<GL/glu.h>],
                                         [Header for OpenGL Utility Library])],
                              [# Mac OS X thinks different
                               AC_CHECK_HEADER(OpenGL/glu.h,
                                               [ov_have_gl=yes
                                                AC_DEFINE(OPENVRML_GLU_H,
                                                          [<OpenGL/glu.h>],
                                                          [Header for OpenGL Utility Library])])])],
             [ov_have_gl=no], ${GL_LIBS})
AC_LANG_POP(C)
    
if test "X${ov_have_gl}" = Xyes; then
  no_gl=""
else
  no_gl=yes
  GL_LIBS=""
fi

AC_SUBST([GL_LIBS])
])
