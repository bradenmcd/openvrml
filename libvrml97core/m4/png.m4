dnl
dnl VRML_PATH_PNG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRML_PATH_PNG,
  [
    AC_REQUIRE([VRML_PATH_ZLIB])
    AC_REQUIRE([AC_CHECK_LIBM]) dnl recent versions of libpng need pow(3M)
    
    AC_ARG_WITH(png-prefix,
      [  --with-png-prefix=DIR   pass '-IDIR/include' to cpp, '-LDIR/lib' to ld]
    )
    
    if test -n "${with_png_prefix}"; then
      png__Idir="-I${with_png_prefix}/include"
      png__Ldir="-L${with_png_prefix}/lib"
    fi
    
    PNG_CFLAGS=""
    PNG_LIBS=""
    
    AC_LANG_SAVE
    AC_LANG_C
    
    ac_save_LDFLAGS="${LFDLAGS}"
    LDFLAGS="${LDFLAGS} ${ZLIB_LIBS} ${png__Ldir} ${LIBM}"
    
    AC_CHECK_LIB(png, png_read_info,
      [
        ac_save_CPPFLAGS="${CPPFLAGS}"
        CPPFLAGS="${CPPFLAGS} ${png__Idir}"
        AC_CHECK_HEADER(png.h,
          [
            have_png=yes
            PNG_CFLAGS="${ZLIB_CFLAGS} ${png__Idir}"
            PNG_LIBS="${ZLIB_LIBS} ${png__Ldir} -lpng ${LIBM}"
          ],
          have_png=no
        )
        CPPFLAGS="${ac_save_CPPFLAGS}"
      ],
      have_png=no
    )
    
    LDFLAGS="${ac_save_LDFLAGS}"
    
    if test "X$have_png" = "Xyes"; then
      ifelse([$1], , :, [$1])
    else
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
    
    AC_SUBST(PNG_CFLAGS)
    AC_SUBST(PNG_LIBS)
  ]
)
