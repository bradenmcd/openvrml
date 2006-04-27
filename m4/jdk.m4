# Autoconf macros to check for a Java Development Kit
# Copyright (C) 2002  Braden McDaniel
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
# OV_CHECK_JNI
# ------------
# Check for a Java Native Interface implementation.  If one is found, the
# required linker flags are included in the output variable "JNI_LIBS";
# otherwise, the shell variable "no_jni" is set to "yes".
#
AC_DEFUN([OV_CHECK_JNI],
[AC_MSG_CHECKING([for JNI library])
AC_CACHE_VAL([ov_cv_check_jni_libjvm],
[ov_cv_check_jni_libjvm=no
AC_LANG_PUSH(C)
ov_save_LIBS="${LIBS}"
LIBS=""
ov_check_libs="-ljvm -lgcj"
for ov_lib in ${ov_check_libs}; do
  LIBS="${ov_lib} ${ov_save_libs}"
  AC_TRY_LINK([#include <jni.h>], [JNI_CreateJavaVM(0, 0, 0)],
              [ov_cv_check_jni_libjvm=${ov_lib}; break])
done
LIBS="${ov_save_LIBS}"
AC_LANG_POP(C)])
AC_MSG_RESULT([$ov_cv_check_jni_libjvm])
if test "X${ov_cv_check_jni_libjvm}" = "Xno"; then
  no_jni=yes
else
  JNI_LIBS="${ov_cv_check_jni_libjvm}"
fi
AC_SUBST([JNI_LIBS])
])


AC_DEFUN([OV_PROG_JAVA],
[AC_ARG_VAR([JAVA], [Java interpreter command])dnl
AC_ARG_VAR([JAVAFLAGS], [Java interpreter flags])dnl
if test -z "$JAVA"; then
  AC_CHECK_PROG([JAVA], [gij], [gij])
fi
if test -z "$JAVA"; then
  AC_CHECK_PROG([JAVA], [java], [java])
fi
])


AC_DEFUN([OV_PROG_JAVAC],
[AC_ARG_VAR([JAVAC], [Java bytecode compiler command])dnl
AC_ARG_VAR([JAVACFLAGS], [Java bytecode compiler flags])dnl
if test -z "$JAVAC"; then
  AC_CHECK_PROG([JAVAC], [gcj], [gcj -C])
fi
if test -z "$JAVAC"; then
  AC_CHECK_PROG([JAVAC], [javac], [javac])
fi
])


AC_DEFUN([OV_PROG_JAVAH],
[AC_ARG_VAR([JAVAH], [JNI header file generator])dnl
if test -z "$JAVAH"; then
  AC_CHECK_PROG([JAVAH], [gcjh], [gcjh -jni])
fi
if test -z "$JAVAH"; then
  AC_CHECK_PROG([JAVAH], [javah], [javah])
fi
])

AC_DEFUN([OV_PROG_JAR],
[AC_ARG_VAR([JAR], [Java archive compiler])dnl
if test -z "$JAR"; then
  AC_CHECK_PROG([JAR], [jar], [jar])
fi
])

dnl AC_DEFUN([OV_PROG_JAVADOC],
dnl [AC_REQUIRE([OV_WITH_JDK])dnl
dnl AC_ARG_VAR([JAVADOC], [Java API documentation generator])dnl
dnl ])
