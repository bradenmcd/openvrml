dnl
dnl VRML_PATH_PNG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRML_PATH_PNG,
  [
    AC_REQUIRE([VRML_PATH_ZLIB])
    
    AC_ARG_WITH(png-prefix,
      [  --with-png-prefix=DIR   pass '-IDIR/include' to cpp, '-LDIR/lib' to ld]
    )
    
    if test -n "${with_png_prefix}"; then
      png__Idir="-I${with_png_prefix}/include"
      png__Ldir="-I${with_png_prefix}/lib"
    fi
    
    AC_LANG_SAVE
    AC_LANG_C
    
    ac_save_LDFLAGS="${LFDLAGS}"
    LDFLAGS="${LDFLAGS} ${ZLIB_CFLAGS} ${png__Ldir}"
    
    AC_CHECK_LIB(png, png_read_info,
      [
        ac_save_CPPFLAGS="${CPPFLAGS}"
        CPPFLAGS="${CPPFLAGS} ${png__Idir}"
        AC_CHECK_HEADER(png.h,
          [
            have_png=yes
            PNG_CFLAGS="${ZLIB_CFLAGS} ${png__Idir}"
            PNG_LIBS="${ZLIB_LIBS} ${png__Ldir} -lpng"
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
      ZLIB_CFLAGS=""
      ZLIB_LIBS=""
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
    
    AC_SUBST(PNG_CFLAGS)
    AC_SUBST(PNG_LIBS)
  ]
)
