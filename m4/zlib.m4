dnl Autoconf macro to check for zlib
dnl Copyright (C) 2002  Braden McDaniel
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

#
# OV_WITH_ZLIB
# ------------
# Check for zlib.  If zlib is found, the required linker flags are included in
# the output variable "Z_LIBS"; and the preprocessor symbol "HAVE_ZLIB" is
# defined.  If zlib is not found, the shell variable "no_zlib" is set to "yes".
#
AC_DEFUN([OV_WITH_ZLIB],
[AC_ARG_WITH([zlib], [  --with-zlib             use zlib])
AC_CACHE_CHECK([for zlib], [ov_cv_zlib],
[if test "X$with_zlib" = "Xno"; then
  ov_cv_zlib=disabled
else
  Z_LIBS="-lz"
  AC_LANG_PUSH(C)
  ov_save_LIBS="${LIBS}"
  LIBS="${Z_LIBS} ${LIBS}"
  AC_TRY_LINK([#include <zlib.h>], [zlibVersion()],
              [ov_cv_zlib=yes], [ov_cv_zlib=no])
  LIBS="${ov_save_LIBS}"
  AC_LANG_POP(C)
fi
])
if test "X$ov_cv_zlib" != "Xyes"; then
  no_zlib=yes
  Z_LIBS=""
else
  AC_DEFINE([HAVE_ZLIB], , [Defined if zlib is available and should be used.])
  AC_SUBST([Z_LIBS])
fi
])
