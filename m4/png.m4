# Autoconf macro to check for libpng
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
# OV_WITH_LIBPNG
# --------------
# Check for libpng.  If libpng is found, the required linker flags are included
# in the output variable "PNG_LIBS"; and the preprocessor symbol "HAVE_LIBPNG"
# is defined.  If libpng is not found, the shell variable "no_libpng" is set to
# "yes".
#
AC_DEFUN([OV_WITH_LIBPNG],
[AC_REQUIRE([OV_WITH_ZLIB])
AC_ARG_WITH(libpng, [  --with-libpng           use libpng])
AC_CACHE_CHECK([for libpng], [ov_cv_libpng],
[if test "X$with_libpng" = "Xno"; then
  ov_cv_libpng=disabled
elif test "X$no_zlib" = "Xyes"; then
  ov_cv_libpng=no
else
  PNG_LIBS="-lpng -lm ${Z_LIBS}"
  AC_LANG_PUSH(C)
  ov_save_LIBS="${LIBS}"
  LIBS="${PNG_LIBS} ${LIBS}"
  AC_TRY_LINK([#include <png.h>], [png_read_info(NULL, NULL)],
              [ov_cv_libpng=yes], [ov_cv_libpng=no])
  LIBS="${ov_save_LIBS}"
  AC_LANG_POP(C)
fi
])
if test "X$ov_cv_libpng" != "Xyes"; then
  no_libpng=yes
  PNG_LIBS=""
else
  AC_DEFINE([HAVE_LIBPNG], , [Defined if libpng is available and should be used.])
  AC_SUBST(PNG_LIBS)
fi
])
