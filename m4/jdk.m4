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
