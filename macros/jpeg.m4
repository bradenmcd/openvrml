dnl Autoconf macro to check for libjpeg at a specified path
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
dnl OV_PATH_JPEG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(OV_PATH_JPEG,
  [
    AC_ARG_WITH(jpeg-prefix, [  --with-jpeg-prefix=PREFIX
                          libjpeg installed under PREFIX])
    
    if test -n "${with_jpeg_prefix}"; then
      jpeg__Idir="-I${with_jpeg_prefix}/include"
      jpeg__Ldir="-L${with_jpeg_prefix}/lib"
    fi
    
    JPEG_CFLAGS=""
    JPEG_LIBS=""
    
    AC_LANG_SAVE
    AC_LANG_C
    
    ac_save_LDFLAGS="${LFDLAGS}"
    LDFLAGS="${LDFLAGS} ${jpeg__Ldir}"
    
    AC_CHECK_LIB(jpeg, jpeg_read_header,
      [
        ac_save_CPPFLAGS="${CPPFLAGS}"
        CPPFLAGS="${CPPFLAGS} ${jpeg__Idir}"
        AC_CHECK_HEADER(jpeglib.h,
          [
            have_jpeg=yes
            JPEG_CFLAGS="${jpeg__Idir}"
            JPEG_LIBS="${jpeg__Ldir} -ljpeg"
          ],
          have_jpeg=no
        )
        CPPFLAGS="${ac_save_CPPFLAGS}"
      ],
      have_jpeg=no
    )
    
    LDFLAGS="${ac_save_LDFLAGS}"
    
    if test "X$have_jpeg" = "Xyes"; then
      ifelse([$1], , :, [$1])
    else
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
    
    AC_SUBST(JPEG_CFLAGS)
    AC_SUBST(JPEG_LIBS)
  ]
)
