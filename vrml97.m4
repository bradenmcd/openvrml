AC_DEFUN(AM_PATH_VRML,
  [
    AC_ARG_WITH(vrml-prefix,
      [  --with-vrml-prefix=DIR  pass '-IDIR/include' to cpp, '-LDIR/lib' to ld]
    )
    
    if test -n "${with_vrml_prefix}"; then
      vrml__Idir="-I${with_vrml_prefix}/include"
      vrml__Ldir="-L${with_vrml_prefix}/lib"
    fi
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    dnl
    dnl Test for libvrml97
    dnl
    AC_MSG_CHECKING([for LibVRML97])
    ac_save_CPPFLAGS="${CPPFLAGS}"
    CPPFLAGS="${CPPFLAGS} ${vrml__Idir}"
    ac_save_LIBS="${LIBS}"
    LIBS="${vrml__Ldir} -lvrml97 ${LIBS}"
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
    CPPFLAGS="${ac_save_CPPFLAGS}"
    LIBS="${ac_save_LIBS}"
    
    AC_LANG_RESTORE

    if test "X${have_vrml}" = Xyes; then
      AC_MSG_RESULT([yes])
      VRML_CXXFLAGS="${vrml__Idir}"
      VRML_LIBS="${vrml__Ldir} -lvrml97"
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
