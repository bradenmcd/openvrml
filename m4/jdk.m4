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
# OV_WITH_JDK
# -----------
# Adds an argument "--with-jdk".  By default, the macro checks for the JNI
# headers and library in the default search paths.  If a directory is passed
# as an argument, it is used as the root directory of a JDK installation.
#
AC_DEFUN([OV_WITH_JDK],
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_MSG_CHECKING([for JDK])
AC_ARG_WITH([jdk], [  --with-jdk[[=JDKHOME]]    use the Java Development Kit])
if test "X$with_jdk" = "Xno"; then
  ov_jdk_result=disabled
else
  AC_CACHE_VAL([ov_cv_jdk],
  [if test -n "$with_jdk" -a "X$with_jdk" != "Xyes"; then
    ov_jdkhome="${with_jdk}"
    case "${host_os}" in
      *linux*)  ov_os_idir="linux" ;;
      *cygwin*) ov_os_idir="win32" ;;
      *)        ;;
    esac
    if test -n $ov_os_idir; then
      ov_jni_Idirs="-I${ov_jdkhome}/include -I${ov_jdkhome}/include/${ov_os_idir}"
    fi

    case "${host_cpu}" in
      i*86) ov_arch_ldir="i386" ;;
      *)    ;;
    esac
    if test -n $ov_arch_ldir; then
      #
      # The Sun JDK for Windows puts the shared libs under bin. Adding that
      # to the search path everywhere should be benign.
      #
      ov_jni_libs="-L${ov_jdkhome}/jre/bin/${ov_arch_ldir}/client -L${ov_jdkhome}/jre/lib/${ov_arch_ldir}/client"
    fi
  fi
  AC_LANG_PUSH(C)
  ov_save_CFLAGS="${CFLAGS}"
  CFLAGS="${CFLAGS} $ov_jni_Idirs"
  ov_save_LDFLAGS="${LDFLAGS}"
  LDFLAGS="$ov_jni_libs ${LDFLAGS}"
  ov_save_LIBS="${LIBS}"
  LIBS=""
  ov_check_libs="-ljvm -lgcj"
  for ov_lib in ${ov_check_libs}; do
    LIBS="${ov_lib} ${ov_save_libs}"
    AC_TRY_LINK([#include <jni.h>], [JNI_CreateJavaVM(0, 0, 0)],
                [ov_have_jdk=yes
                 ov_jni_libs="${ov_jni_libs} ${ov_lib}"
                 break],
                [ov_have_jdk=no])
  done
  CFLAGS="${ov_save_CFLAGS}"
  LDFLAGS="${ov_save_LDFLAGS}"
  LIBS="${ov_save_LIBS}"
  AC_LANG_POP(C)
  ov_cv_jdk="ov_jdk_result=$ov_have_jdk \
             JNI_CFLAGS=\"$ov_jni_Idirs\" JNI_LIBS=\"$ov_jni_libs\""])
  eval "$ov_cv_jdk"
fi
AC_MSG_RESULT([$ov_jdk_result])
if test "X$ov_jdk_result" = "Xdisabled" -o "X$ov_cv_jdk" = "Xno"; then
  no_jdk=yes
else
  AC_DEFINE([HAVE_JNI_H], , [Defined if JNI is available and should be used.])
fi
AC_SUBST([JNI_CFLAGS])
AC_SUBST([JNI_LIBS])
])


AC_DEFUN([OV_PROG_JAVA],
[AC_REQUIRE([OV_WITH_JDK])dnl
AC_ARG_VAR([JAVA], [Java interpreter command])dnl
if test -n "$with_jdk" -a "X$with_jdk" != "Xno" -a "X$with_jdk" != "Xyes"; then
  AC_PATH_PROG([JAVA], [java], , [${with_jdk}/jre/bin])
fi
if test -z "$JAVA"; then
  AC_CHECK_PROG([JAVA], [gij], [gij])
fi
if test -z "$JAVA"; then
  AC_CHECK_PROG([JAVA], [java], [java])
fi
])


AC_DEFUN([OV_PROG_JAVAC],
[AC_REQUIRE([OV_WITH_JDK])dnl
AC_ARG_VAR([JAVAC], [Java bytecode compiler command])dnl
AC_ARG_VAR([JAVACFLAGS], [Java bytecode compiler flags])dnl
if test -n "$with_jdk" -a "X$with_jdk" != "Xno" -a "X$with_jdk" != "Xyes"; then
  AC_PATH_PROG([JAVAC], [javac], , [${with_jdk}/bin])
fi
if test -z "$JAVAC"; then
  AC_CHECK_PROG([JAVAC], [gcj], [gcj -C])
fi
if test -z "$JAVAC"; then
  AC_CHECK_PROG([JAVAC], [javac], [javac])
fi
])


AC_DEFUN([OV_PROG_JAVAH],
[AC_REQUIRE([OV_WITH_JDK])dnl
AC_ARG_VAR([JAVAH], [JNI header file generator])dnl
if test -n "$with_jdk" -a "X$with_jdk" != "Xno" -a "X$with_jdk" != "Xyes"; then
  AC_PATH_PROG([JAVAH], [javah], , [${with_jdk}/bin])
fi
if test -z "$JAVAH"; then
  AC_CHECK_PROG([JAVAH], [gcjh], [gcjh -jni])
fi
if test -z "$JAVAH"; then
  AC_CHECK_PROG([JAVAH], [javah], [javah])
fi
])

AC_DEFUN([OV_PROG_JAR],
[AC_REQUIRE([OV_WITH_JDK])dnl
AC_ARG_VAR([JAR], [Java archive compiler])dnl
if test -n "$with_jdk" -a "X$with_jdk" != "Xno" -a "X$with_jdk" != "Xyes"; then
  AC_PATH_PROG([JAR], [jar], , [${with_jdk}/bin])
fi
if test -z "$JAR"; then
  AC_CHECK_PROG([JAR], [jar], [jar])
fi
])

dnl AC_DEFUN([OV_PROG_JAVADOC],
dnl [AC_REQUIRE([OV_WITH_JDK])dnl
dnl AC_ARG_VAR([JAVADOC], [Java API documentation generator])dnl
dnl ])
