# Autoconf macro to check for JNI
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
# OV_CHECK_JNI
# ------------
# Check for libraries and headers required for JNI (the Java Native Interface).
# If it succeeds, the required compiler and linker flags are included in the
# output variables `JNI_CFLAGS' and `JNI_LIBS', respectively, and the shell
# variable `no_jni' is set to the empty string. If no JNI support is found,
# `no_jni' is set to `yes'.
#
AC_DEFUN(OV_CHECK_JNI,
[AC_REQUIRE([AC_CANONICAL_HOST])

JNI_CFLAGS=""
JNI_LIBS=""
    
if test -n "${JAVA_HOME}"; then
  case "${host_os}" in
    *linux*) ov_os_idir="linux" ;;
    *)       AC_MSG_ERROR([Unsupported OS for JDK.]) ;;
  esac
  JNI_CFLAGS="-I${JAVA_HOME}/include -I${JAVA_HOME}/include/${ov_os_idir}"
  
  case "${host_cpu}" in
    i*86) ov_arch_ldir="i386" ;;
    *)    AC_MSG_ERROR([Unsupported architecture for JDK.]) ;;
  esac
  JNI_LIBS="-L${JAVA_HOME}/jre/lib/${ov_arch_ldir}/client"
fi

AC_LANG_PUSH(C)
AC_CHECK_LIB(jvm, JNI_CreateJavaVM,
             [ov_have_jni=yes
              JNI_LIBS="${JNI_LIBS} -ljvm"], [ov_have_jni=no], ${JNI_LIBS})
ov_save_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${JNI_CFLAGS} ${CPPFLAGS}"
AC_CHECK_HEADER(jni.h, [ov_have_jni=yes], [ov_have_jni=no])
CPPFLAGS="${ov_save_CPPFLAGS}"
AC_LANG_POP(C)

if test "X${ov_have_jni}" = Xyes; then
  no_jni=""
else
  no_jni="yes"
  JNI_CFLAGS=""
  JNI_LIBS=""
fi
AC_SUBST(JNI_CFLAGS)
AC_SUBST(JNI_LIBS)
])

