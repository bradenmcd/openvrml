# Autoconf macros to check for libjpeg
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
# OV_WITH_LIBJPEG
# ---------------
# Check for libjpeg.  If libjpeg is found, the required linker flags are
# included in the output variable "JPEG_LIBS"; and the preprocessor symbol
# "HAVE_LIBJPEG" is defined.  If libjpeg is not found, the shell variable
# "no_libjpeg" is set to "yes".
#
AC_DEFUN([OV_WITH_LIBJPEG],
[AC_ARG_WITH(libjpeg, [  --with-libjpeg          use libjpeg])
AC_CACHE_CHECK([for libjpeg], [ov_cv_libjpeg],
[if test "X$with_zlib" = "Xno"; then
  ov_cv_libjpeg=disabled
else
  JPEG_LIBS="-ljpeg"
  AC_LANG_PUSH(C)
  ov_save_LIBS="${LIBS}"
  LIBS="${JPEG_LIBS} ${LIBS}"
  AC_TRY_LINK(
[#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>], [jpeg_read_header(0, FALSE)],
              [ov_cv_libjpeg=yes], [ov_cv_libjpeg=no])
  LIBS="${ov_save_LIBS}"
  AC_LANG_POP(C)
fi
])
if test "X$ov_cv_libjpeg" != "Xyes"; then
  no_libjpeg=yes
  JPEG_LIBS=""
else
  AC_DEFINE([HAVE_LIBJPEG], , [Defined if libjpeg is available and should be used.])
  AC_SUBST([JPEG_LIBS])
fi
])
