dnl Autoconf macro to check for zlib at a specified path
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
dnl OV_PATH_ZLIB([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(OV_PATH_ZLIB,
  [
    AC_ARG_WITH(zlib-prefix, [  --with-zlib-prefix=PREFIX
                          zlib installed under PREFIX])
    
    if test -n "${with_zlib_prefix}"; then
      zlib__Idir="-I${with_zlib_prefix}/include"
      zlib__Ldir="-L${with_zlib_prefix}/lib"
    fi
    
    AC_LANG_SAVE
    AC_LANG_C
    
    ac_save_LDFLAGS="${LDFLAGS}"
    LDFLAGS="${LDFLAGS} ${zlib__Ldir}"
    
    AC_CHECK_LIB(z, zlibVersion,
      [
        ac_save_CPPFLAGS="${CPPFLAGS}"
        CPPFLAGS="${CPPFLAGS} ${zlib__Idir}"
        AC_CHECK_HEADER(zlib.h,
          [
            have_zlib=yes
            Z_CFLAGS="${zlib__Idir}"
            Z_LIBS="${zlib__Ldir} -lz"
          ],
          have_zlib=no
        )
        CPPFLAGS="${ac_save_CPPFLAGS}"
      ],
      have_zlib=no
    )
    
    LDFLAGS="${ac_save_LDFLAGS}"
    
    if test "X$have_zlib" = "Xyes"; then
      ifelse([$1], , :, [$1])
    else
      Z_CFLAGS=""
      Z_LIBS=""
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
    
    AC_SUBST(Z_CFLAGS)
    AC_SUBST(Z_LIBS)
  ]
)
