dnl Autoconf macro to check for libpng at a specified path
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
dnl OV_PATH_PNG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(OV_PATH_PNG,
  [
    AC_REQUIRE([OV_PATH_ZLIB])
    
    AC_ARG_WITH(png, [  --with-png[=PREFIX]     libpng installed under PREFIX [default=yes]])
    have_png=yes
    if test "X$with_png" = "Xno"; then
      have_png=no
    else
      if test -n "$with_png"; then
        if test "X$with_png" != "Xyes"; then
          png__Idir="-I${with_png}/include"
          png__Ldir="-L${with_png}/lib"
        fi
      fi
    
      PNG_CFLAGS=""
      PNG_LIBS=""

      AC_LANG_SAVE
      AC_LANG_C

      ac_save_LIBS="${LIBS}"
      LIBS="${png__Ldir} ${Z_LIBS} ${LIBS}"
      AC_CHECK_LIB(png, png_read_info,
        [
          ac_save_CPPFLAGS="${CPPFLAGS}"
          CPPFLAGS="${CPPFLAGS} ${png__Idir}"
          AC_CHECK_HEADER(png.h,
            [
              have_png=yes
              PNG_CFLAGS="${png__Idir} ${Z_CFLAGS}"
              PNG_LIBS="${png__Ldir} -lpng ${Z_LIBS} -lm"
            ],
            have_png=no
          )
          CPPFLAGS="${ac_save_CPPFLAGS}"
        ],
        have_png=no,
        -lm
      )
      LIBS="${ac_save_LIBS}"

      AC_LANG_RESTORE
    fi
    
    if test "X$have_png" = "Xyes"; then
      AC_DEFINE(HAVE_PNG, , [defined if libpng is available and should be used])
      ifelse([$1], , :, [$1])
    else
      ifelse([$2], , :, [$2])
    fi
    
    AC_SUBST(PNG_CFLAGS)
    AC_SUBST(PNG_LIBS)
  ]
)
