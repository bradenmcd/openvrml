dnl
dnl OVAL_PATH_JPEG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(OVAL_PATH_JPEG,
  [
    AC_ARG_WITH(jpeg-prefix, [  --with-jpeg-prefix=DIR  prefix where libjpeg is installed])
    
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
