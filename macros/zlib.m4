dnl Autoconf macro to check for zlib
dnl Copyright (C) 2001  Braden McDaniel
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

dnl
dnl OV_CHECK_ZLIB
dnl
AC_DEFUN(OV_CHECK_ZLIB,
  [
    Z_LIBS="-lz"
    
    AC_LANG_PUSH(C)
    
    ov_save_LIBS="${LIBS}"
    LIBS="${Z_LIBS} ${LIBS}"
    have_zlib=no
    AC_CHECK_LIB(z, zlibVersion, [AC_CHECK_HEADER(zlib.h, [have_zlib=yes])])
    LIBS="${ov_save_LIBS}"
    
    AC_LANG_POP(C)
    
    if test "X${have_zlib}" = Xyes; then
      no_zlib=""
    else
      no_zlib="yes"
      Z_LIBS=""
    fi
    
    have_zlib="" dnl Clear temporary variable.
    
    AC_SUBST(Z_LIBS)
  ]
)
