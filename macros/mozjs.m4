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
dnl OV_PATH_MOZJS([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl Adds `--with-mozjs' option to the package. The argument to the option
dnl may be `yes' (the default), `no', or a directory that is the prefix where
dnl SpiderMonkey (libmozjs) is installed. If the argument is `yes' or a
dnl directory, ACTION-IF-FOUND is executed if libmozjs can be located;
dnl otherwise, ACTION-IF-NOT-FOUND is executed.
dnl
dnl If SpiderMonkey is found and should be used, HAVE_MOZJS is defined, and the
dnl autoconf substitutions MOZJS_CFLAGS and MOZJS_LIBS are respectively set to
dnl the compiler and linker flags required to use the library.  If SpiderMonkey
dnl is not available--either because `--without-mozjs' has been specified, or
dnl because the library cannot be found--the shell variable `no_mozjs' is set
dnl to `yes'; otherwise it is set to the empty string.
dnl
dnl ACTION-IF-NOT-FOUND is executed only if the JavaScript support library is
dnl searched for and not found. If the package is configured using
dnl `--without-mozjs' (equivalent to `--with-mozjs=no'), ACTION-IF-NOT-FOUND
dnl will not be run.
dnl
AC_DEFUN(OV_PATH_MOZJS,
  [
    MOZJS_CFLAGS=""
    MOZJS_LIBS=""

    AC_ARG_WITH(mozjs, [  --with-mozjs[=PREFIX]   libmozjs installed under PREFIX [default=yes]])
    
    if test "X$with_mozjs" == "Xno"; then
      no_mozjs=yes
    else
      AC_MSG_CHECKING([for SpiderMonkey])

      if test -n "$with_mozjs"; then
        if test "X$with_mozjs" != "Xyes"; then
          mozjs__Idir="-I${with_mozjs}/include"
          mozjs__Ldir="-L${with_mozjs}/lib"
        fi
      fi
    
      AC_LANG_SAVE
      AC_LANG_C

      ov_save_LIBS="${LIBS}"
      LIBS="${mozjs__Ldir} -lmozjs ${LIBS}"
      AC_TRY_LINK_FUNC(JS_GetVersion,
        [
          ov_save_CPPFLAGS="${CPPFLAGS}"
          CPPFLAGS="${CPPFLAGS} -DXP_UNIX ${mozjs__Idir}"
          AC_TRY_CPP(
            [ #include<mozilla/jsapi.h> ],
            [
              have_mozjs=yes
              MOZJS_CFLAGS="-DXP_UNIX ${mozjs__Idir}"
              MOZJS_LIBS="${mozjs__Ldir} -lmozjs"
            ],
            have_mozjs=no
          )
          CPPFLAGS="${ov_save_CPPFLAGS}"
        ],
        have_mozjs=no
      )
      LIBS="${ov_save_LIBS}"

      AC_LANG_RESTORE
      
      if test "X$have_mozjs" = "Xyes"; then
        AC_MSG_RESULT([yes])
        no_mozjs=""
        AC_DEFINE(HAVE_MOZJS, , [defined if libmozjs is available and should be used])
        ifelse([$1], , :, [$1])
      else
        AC_MSG_RESULT([no])
        no_mozjs=yes
        ifelse([$2], , :, [$2])
      fi
    fi
    
    AC_SUBST(MOZJS_CFLAGS)
    AC_SUBST(MOZJS_LIBS)
  ]
)
