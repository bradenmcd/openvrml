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
      zlib__Ldir="-I${with_zlib_prefix}/lib"
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
