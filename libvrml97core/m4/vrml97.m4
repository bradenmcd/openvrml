dnl
dnl VRML_PATH_ZLIB([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRML_PATH_ZLIB,
  [
    AC_ARG_WITH(zlib-prefix,
      [  --with-zlib-prefix=DIR  pass '-IDIR/include' to cpp, '-LDIR/lib' to ld]
    )
    
    if test -n "${with_zlib_prefix}"; then
      zlib__Idir="-I${with_zlib_prefix}/include"
      zlib__Ldir="-L${with_zlib_prefix}/lib"
    fi
    
    AC_LANG_SAVE
    AC_LANG_C
    
    ac_save_LDFLAGS="${LFDLAGS}"
    LDFLAGS="${LDFLAGS} ${zlib__Ldir}"
    
    AC_CHECK_LIB(z, zlibVersion,
      [
        ac_save_CPPFLAGS="${CPPFLAGS}"
        CPPFLAGS="${CPPFLAGS} ${zlib__Idir}"
        AC_CHECK_HEADER(zlib.h,
          [
            have_zlib=yes
            ZLIB_CFLAGS="${zlib__Idir}"
            ZLIB_LIBS="${zlib__Ldir} -lz"
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
    
    AC_SUBST(ZLIB_CFLAGS)
    AC_SUBST(ZLIB_LIBS)
  ]
)


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
      png__Ldir="-L${with_png_prefix}/lib"
    fi
    
    PNG_CFLAGS=""
    PNG_LIBS=""
    
    AC_LANG_SAVE
    AC_LANG_C
    
    ac_save_LIBS="${LIBS}"
    LIBS="${png__Ldir} ${ZLIB_LIBS} ${LIBS}"
    AC_CHECK_LIB(png, png_read_info,
      [
        ac_save_CPPFLAGS="${CPPFLAGS}"
        CPPFLAGS="${CPPFLAGS} ${png__Idir}"
        AC_CHECK_HEADER(png.h,
          [
            have_png=yes
            PNG_CFLAGS="${png__Idir} ${ZLIB_CFLAGS}"
            PNG_LIBS="${png__Ldir} -lpng ${ZLIB_LIBS} -lm"
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


dnl
dnl VRML_PATH_JPEG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRML_PATH_JPEG,
  [
    AC_ARG_WITH(jpeg-prefix,
      [  --with-jpeg-prefix=DIR  pass '-IDIR/include' to cpp, '-LDIR/lib' to ld]
    )
    
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

AC_DEFUN(AM_PATH_VRML,
  [
    AC_REQUIRE([VRML_PATH_JPEG])
    AC_REQUIRE([VRML_PATH_PNG])
    
    AC_ARG_WITH(vrml-prefix,
      [  --with-vrml-prefix=DIR  pass '-IDIR/include' to cpp, '-LDIR/lib' to ld]
    )
    
    if test -n "${with_vrml_prefix}"; then
      vrml__Idir="-I${with_vrml_prefix}/include"
      vrml__Ldir="-L${with_vrml_prefix}/lib"
    fi
    
    VRML_CXXFLAGS="${JPEG_CFLAGS} ${PNG_CFLAGS}"
    VRML_LIBS="${JPEG_LIBS} ${PNG_LIBS}"
    
    dnl
    dnl Check for libvrml97js
    dnl
    AC_CHECK_LIB(vrml97js, JS_GetVersion,
      [
        VRML_CXXFLAGS="${vrml__Idir} ${VRML_CXXFLAGS}"
        VRML_LIBS="${vrml__Ldir} -lvrml97js ${VRML_LIBS}"
      ],
      ,
      ${vrml__Ldir}
    )
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    dnl
    dnl Test for libvrml97
    dnl
    AC_MSG_CHECKING([for libvrml97])
    ac_save_CXXFLAGS="${CXXFLAGS}"
    ac_save_LIBS="${LIBS}"
    LIBS="-lvrml97 ${VRML_LIBS} ${LIBS}"
    CXXFLAGS="${VRML_CXXFLAGS} ${CXXFLAGS}"
    AC_TRY_LINK(
      [
        #include<vrml97/VrmlScene.h>
      ],
      [
        VrmlScene::readWrl(0, 0);
      ],
      have_vrml=yes,
      have_vrml=no
    )
    CXXFLAGS="${ac_save_CXXFLAGS}"
    LIBS="${ac_save_LIBS}"
    
    AC_LANG_RESTORE

    if test "X${have_vrml}" = Xyes; then
      AC_MSG_RESULT([yes])
      VRML_LIBS="-lvrml97 ${VRML_LIBS}"
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      VRML_CXXFLAGS=""
      VRML_LIBS=""
      ifelse([$2], , :, [$2])
    fi

    AC_SUBST(VRML_CXXFLAGS)
    AC_SUBST(VRML_LIBS)
  ]
)
