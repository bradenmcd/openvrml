dnl
dnl OVAL_PATH_ZLIB([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(OVAL_PATH_ZLIB,
  [
    AC_ARG_WITH(zlib-prefix, [  --with-zlib-prefix=DIR  prefix where zlib is installed])
    
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
            Z_CFLAGS="${zlib__Idir}"
            Z_LIBS="${zlib__Ldir} -lz"
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
      Z_CFLAGS=""
      Z_LIBS=""
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
    
    AC_SUBST(Z_CFLAGS)
    AC_SUBST(Z_LIBS)
  ]
)
