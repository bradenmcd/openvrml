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
dnl OV_PATH_LIBPNG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl Adds `--with-libpng' option to the package. The argument to the option may
dnl be `yes' (the default), `no', or a directory that is the prefix where libpng
dnl is installed. If the argument is `yes' or a directory, ACTION-IF-FOUND is
dnl executed if libpng can be located; otherwise, ACTION-IF-NOT-FOUND is
dnl executed.
dnl
dnl If libpng is found and should be used, HAVE_LIBPNG is defined, and the
dnl autoconf substitutions PNG_CFLAGS and PNG_LIBS are respectively set to
dnl the compiler and linker flags required to use the library.  If libpng is
dnl not available--either because `--without-libpng' has been specified, or
dnl because the library cannot be found--the shell variable `no_libpng' is set
dnl to `yes'; otherwise it is set to the empty string.
dnl
dnl ACTION-IF-NOT-FOUND is executed only if libpng is searched for and not
dnl found. If the package is configured using `--without-libpng' (equivalent to
dnl `--with-libpng=no'), ACTION-IF-NOT-FOUND will not be run.
dnl
AC_DEFUN(OV_PATH_LIBPNG,
  [
    AC_REQUIRE([OV_PATH_ZLIB])
    
    PNG_CFLAGS=""
    PNG_LIBS=""

    AC_ARG_WITH(libpng, [  --with-libpng[=PREFIX]  libpng installed under PREFIX [default=yes]])
    
    if test "X$with_libpng" = "Xno"; then
      no_libpng=yes
    else
      AC_MSG_CHECKING([for libpng])
      
      if test -n "$with_libpng"; then
        if test "X$with_libpng" != "Xyes"; then
          libpng__Idir="-I${with_libpng}/include"
          libpng__Ldir="-L${with_libpng}/lib"
        fi
      fi
    
      AC_LANG_SAVE
      AC_LANG_C

      ov_save_LIBS="${LIBS}"
      LIBS="${libpng__Ldir} -lpng -lm ${Z_LIBS} ${LIBS}"
      AC_TRY_LINK_FUNC(png_read_info,
        [
          ov_save_CPPFLAGS="${CPPFLAGS}"
          CPPFLAGS="${CPPFLAGS} ${libpng__Idir} ${Z_CFLAGS}"
          AC_TRY_CPP(
            [ #include<png.h> ],
            [
              have_libpng=yes
              PNG_CFLAGS="${libpng__Idir} ${Z_CFLAGS}"
              PNG_LIBS="${libpng__Ldir} -lpng -lm ${Z_LIBS}"
            ],
            have_libpng=no
          )
          CPPFLAGS="${ov_save_CPPFLAGS}"
        ],
        have_libpng=no
      )
      LIBS="${ov_save_LIBS}"

      AC_LANG_RESTORE

      if test "X$have_libpng" = "Xyes"; then
        AC_MSG_RESULT([yes])
        no_libpng=""
        AC_DEFINE(HAVE_LIBPNG, , [defined if libpng is available and should be used])
        ifelse([$1], , :, [$1])
      else
        AC_MSG_RESULT([no])
        no_libpng=yes
        ifelse([$2], , :, [$2])
      fi
    fi
    
    AC_SUBST(PNG_CFLAGS)
    AC_SUBST(PNG_LIBS)
  ]
)
