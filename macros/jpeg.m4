# Autoconf macros to check for libjpeg
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
# OV_CHECK_LIBJPEG
# ----------------
# Check for libjpeg. If libjpeg is found, the required linker flags are
# included in the output variable `JPEG_LIBS'; and the shell variable
# `no_libjpeg' is set to the empty string. If libjpeg is not found,
# `no_libjpeg' is set to `yes'.
#
AC_DEFUN(OV_CHECK_LIBJPEG,
[dnl
AC_LANG_PUSH(C)
no_libjpeg="yes"
JPEG_LIBS=""
AC_CHECK_LIB(jpeg, jpeg_read_header,
             [AC_CHECK_HEADER(jpeglib.h,
                              [no_libjpeg=""
                               JPEG_LIBS="-ljpeg"])])
AC_LANG_POP(C)
AC_SUBST(JPEG_LIBS)
])
