# Autoconf macro to check for libpng
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
# OV_CHECK_LIBPNG
# ---------------
# Check for libpng. If libpng is found, the required linker flags are
# included in the output variables `PNG_LIBS' and `Z_LIBS'; and the shell
# variable `no_libpng' is set to the empty string. If libpng is not found,
# `no_libpng' is set to `yes'.
#
AC_DEFUN(OV_CHECK_LIBPNG,
[AC_REQUIRE([OV_CHECK_ZLIB])
AC_LANG_PUSH(C)
no_libpng=yes
PNG_LIBS=""
AC_CHECK_LIB(png, png_read_info,
            [AC_CHECK_HEADER(png.h,
                             [no_libpng=""
                              PNG_LIBS="-lpng -lm"])],
            , ${Z_LIBS} -lm)
AC_LANG_POP(C)
AC_SUBST(PNG_LIBS)
])
