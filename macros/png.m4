dnl
dnl OV_PATH_PNG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(OV_PATH_PNG,
  [
    AC_REQUIRE([OV_PATH_ZLIB])
    
    AC_ARG_WITH(png-prefix, [  --with-png-prefix=PREFIX
                          libpng installed under PREFIX])
    
    if test -n "${with_png_prefix}"; then
      png__Idir="-I${with_png_prefix}/include"
      png__Ldir="-L${with_png_prefix}/lib"
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
