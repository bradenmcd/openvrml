dnl Autoconf macro to check for libmozjs at a specified path
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
dnl OV_PATH_JAVASCRIPT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(OV_PATH_JAVASCRIPT,
  [
    AC_MSG_CHECKING([for JavaScript])
    
    AC_ARG_WITH(js, [  --with-js[=PREFIX]   libmozjs installed under PREFIX [default=yes]])
    have_js=yes
    if test "X$with_js" = "Xno"; then
      have_js=no
    else
      if test -n "$with_js"; then
        if test "X$with_js" != "Xyes"; then
          js__Idir="-I${with_js}/include"
          js__Ldir="-L${with_js}/lib"
        fi
      fi
    
      JS_CFLAGS=""
      JS_LIBS=""

      AC_LANG_SAVE
      AC_LANG_C

      ac_save_LIBS="${LIBS}"
      LIBS="${js__Ldir} -lmozjs ${LIBS}"
      AC_TRY_LINK_FUNC(JS_GetVersion,
        [
          ac_save_CPPFLAGS="${CPPFLAGS}"
          CPPFLAGS="${CPPFLAGS} -DXP_UNIX ${js__Idir}"
          AC_TRY_CPP(
            [ #include<mozilla/jsapi.h> ],
            [
              have_js=yes
              JS_CFLAGS="-DXP_UNIX ${js__Idir}"
              JS_LIBS="${js__Ldir} -lmozjs"
            ],
            have_js=no
          )
          CPPFLAGS="${ac_save_CPPFLAGS}"
        ],
        have_js=no
      )
      LIBS="${ac_save_LIBS}"

      AC_LANG_RESTORE
    fi
    
    if test "X$have_js" = "Xyes"; then
      AC_MSG_RESULT([yes])
      AC_DEFINE(HAVE_JAVASCRIPT, , [defined if libmozjs is available and should be used])
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      ifelse([$2], , :, [$2])
    fi
    
    AC_SUBST(JS_CFLAGS)
    AC_SUBST(JS_LIBS)
  ]
)
