dnl
dnl AM_PATH_VRMLGL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(AM_PATH_VRMLGL,
  [
    AC_REQUIRE([AM_PATH_VRML])
    
    dnl
    dnl Test for libopenvrml-gl
    dnl
    AC_MSG_CHECKING([for libopenvrml-gl])
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    ac_save_CXXFLAGS="${CXXFLAGS}"
    ac_save_LIBS="$LIBS"
    LIBS="-lopenvrml-gl ${VRML_LIBS} ${LIBS}"
    CXXFLAGS="${VRML_CXXFLAGS} ${CXXFLAGS}"
    AC_TRY_LINK(
      [ #include <OpenVRML/GL/ViewerOpenGL.h> ],
      [ int x = ViewerOpenGL::MAX_LIGHTS; ],
      have_vrmlgl=yes,
      have_vrmlgl=no
    )
    CXXFLAGS="${ac_save_CXXFLAGS}"
    LIBS="$ac_save_LIBS"
    AC_LANG_RESTORE
    
    if test $have_vrmlgl = yes; then
      AC_MSG_RESULT([yes])
      VRMLGL_CXXFLAGS="${VRML_CXXFLAGS}"
      VRMLGL_LIBS="-lopenvrml-gl ${VRML_LIBS}"
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      VRMLGL_CXXFLAGS=""
      VRMLGL_LIBS=""
      ifelse([$2], , :, [$2])
    fi
    
    AC_SUBST(VRMLGL_CXXFLAGS)
    AC_SUBST(VRMLGL_LIBS)
  ]
)
