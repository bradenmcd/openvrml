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
dnl OV_PATH_LIBJPEG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl Adds `--with-libjpeg' option to the package. The argument to the option may
dnl be `yes' (the default), `no', or a directory that is the prefix where
dnl libjpeg is installed. If the argument is `yes' or a directory,
dnl ACTION-IF-FOUND is executed if libjpeg can be located; otherwise,
dnl ACTION-IF-NOT-FOUND is executed.
dnl
dnl If libjpeg is found and should be used, HAVE_LIBJPEG is defined, and the
dnl autoconf substitutions JPEG_CFLAGS and JPEG_LIBS are respectively set to
dnl the compiler and linker flags required to use the library.  If libjpeg is
dnl not available--either because `--without-libjpeg' has been specified, or
dnl because the library cannot be found--the shell variable `no_libjpeg' is set
dnl to `yes'; otherwise it is set to the empty string.
dnl
dnl ACTION-IF-NOT-FOUND is executed only if libjpeg is searched for and not
dnl found. If the package is configured using `--without-jpeg' (equivalent to
dnl `--with-jpeg=no'), ACTION-IF-NOT-FOUND will not be run.
dnl
AC_DEFUN(OV_PATH_LIBJPEG,
  [
    JPEG_CFLAGS=""
    JPEG_LIBS=""

    AC_ARG_WITH(libjpeg, [  --with-libjpeg[=PREFIX] libjpeg installed under PREFIX [default=yes]])
    
    if test "X$with_libjpeg" = "Xno"; then
      no_libjpeg=yes
    else
      AC_MSG_CHECKING([for libjpeg])
      
      if test -n "$with_libjpeg"; then
        if test "X$with_libjpeg" != "Xyes"; then
          jpeg__Idir="-I${with_libjpeg}/include"
          jpeg__Ldir="-L${with_libjpeg}/lib"
        fi
      fi
    
      AC_LANG_SAVE
      AC_LANG_C

      ov_save_LIBS="${LIBS}"
      LIBS="${jpeg__Ldir} -ljpeg ${LIBS}"
      AC_TRY_LINK_FUNC(jpeg_read_header,
        [
          ac_save_CPPFLAGS="${CPPFLAGS}"
          CPPFLAGS="${CPPFLAGS} ${png__Idir}"
          AC_TRY_CPP(
            [ #include<jpeglib.h> ],
            [
              have_libjpeg=yes
              JPEG_CFLAGS="${jpeg__Idir}"
              JPEG_LIBS="${jpeg__Ldir} -ljpeg"
            ],
            have_libjpeg=no
          )
          CPPFLAGS="${ac_save_CPPFLAGS}"
        ],
        have_libjpeg=no
      )
      LIBS="${ov_save_LIBS}"

      AC_LANG_RESTORE

      if test "X$have_libjpeg" = "Xyes"; then
        AC_MSG_RESULT([yes])
        np_libjpeg=""
        AC_DEFINE(HAVE_LIBJPEG, ,[defined if libjpeg is available and should be used])
        ifelse([$1], , :, [$1])
      else
        AC_MSG_RESULT([no])
        no_libjpeg=yes
        ifelse([$2], , :, [$2])
      fi
    fi
    
    AC_SUBST(JPEG_CFLAGS)
    AC_SUBST(JPEG_LIBS)
  ]
)
