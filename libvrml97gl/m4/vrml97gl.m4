dnl
dnl VRMLGL_PATH_GL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRMLGL_PATH_GL,
  [dnl
    dnl
    dnl Dependencies
    dnl
    AC_REQUIRE([AC_PATH_XTRA])
    
    
    if test "x$x_includes" = "x"; then
      X__Idir=""
    else
      X__Idir="-I$x_includes"
    fi
    if test "x$x_libraries" = "x"; then
      X__Ldir=""
    else
      X__Ldir="-L$x_libraries"
    fi
    
    
    X__cflags="${X__Idir} ${X_CFLAGS}"
    X__libs="${X__Ldir} $X_LDFLAGS ${X_PRE_LIBS} ${X_LIBS} ${X_EXTRA_LIBS}"
    
    
    dnl Check for the Xext library (needed for XShm extention)
    AC_CHECK_LIB(Xext, XShmAttach,
      X__libs="-lXext $X__libs",
      # On AIX, it is in XextSam instead, but we still need -lXext
      AC_CHECK_LIB(XextSam, XShmAttach,
        X__libs="-lXextSam -lXext $X__libs",
        no_xext_lib=yes, $X__libs),
      $X__libs
    )
    
    
    
    AC_ARG_WITH(GL-prefix,
      [  --with-GL-prefix=DIR    pass '-IDIR/include' to CPP, '-LDIR/lib' to LD]
    )
    AC_ARG_WITH(MesaGL,
      [  --with-MesaGL           use '-lMesaGL', instead of '-lGL']
    )
    AC_ARG_WITH(MesaGLU,
      [  --with-MesaGLU          use '-lMesaGLU', instead of '-lGLU']
    )
    
    if test -n "${with_GL_prefix}"; then
      GL__Idir="-I${with_GL_prefix}/include"
      GL__Ldir="-L${with_GL_prefix}/lib"
    fi
    
    dnl
    dnl Checks for GL libraries.
    dnl
    ac_save_LDFLAGS="${LDFLAGS}"
    LDFLAGS="${LDFLAGS} ${GL__Ldir}"
    last_LDFLAGS="${X__libs} -lm"
    GLU_LDFLAGS="${X__libs} -lGL -lm"
    MGLU_LDFLAGS="${X__libs} -lMesaGL -lm"
    
    AC_CHECK_LIB(GL,      glEnable,     have_lib_GL=yes,      , ${last_LDFLAGS})
    AC_CHECK_LIB(MesaGL,  glEnable,     have_lib_MesaGL=yes,  , ${last_LDFLAGS})
    AC_CHECK_LIB(GLU,     gluErrorString, have_lib_GLU=yes,     , ${GLU_LDFLAGS})
    AC_CHECK_LIB(MesaGLU, gluErrorString, have_lib_MesaGLU=yes, , ${MGLU_LDFLAGS})
    
    LDFLAGS="${ac_save_LDFLAGS}"
    
    dnl
    dnl Checks for GL header files.
    dnl
    
    ac_save_CPPFLAGS="${CPPFLAGS}"
    CPPFLAGS="${CPPFLAGS} ${GL__Idir} ${X__cflags}"
    
    AC_CHECK_HEADER(GL/gl.h,  have_GL_gl_h=yes)
    AC_CHECK_HEADER(GL/glu.h, have_GL_glu_h=yes)
    AC_CHECK_HEADER(GL/glx.h, have_GL_glx_h=yes)
    
    CPPFLAGS="${ac_save_CPPFLAGS}"
    
    
    no_GL=""
    
    if test "x${have_lib_GL}" = xyes -a "x${with_MesaGL}" = xno; then
      GL_lib='GL'
    elif test "x${have_lib_MesaGL}" = xyes -a "x${with_MesaGL}" = xyes; then
      GL_lib='MesaGL'
    elif test "x${have_lib_GL}" = xyes; then
      GL_lib='GL'
    elif test "x${have_lib_MesaGL}" = xyes; then
      GL_lib='MesaGL'
    else
      no_GL=yes
    fi
    
    
    if test "x${have_lib_GLU}" = xyes -a "x${with_MesaGLU}" = xno; then
      GLU_lib='GLU'
    elif test "x${have_lib_MesaGLU}" = xyes -a "x${with_MesaGLU}" = xyes; then
      GLU_lib='MesaGLU'
    elif test "x${have_lib_GLU}" = xyes -a "x${GL_lib}" = xGL; then
      GLU_lib='GLU'
    elif test "x${have_lib_MesaGLU}" = xyes -a "x${GL_lib}" = xMesaGL; then
      GLU_lib='MesaGLU'
    elif test "x${have_lib_GLU}" = xyes; then
      GLU_lib='GLU'
    elif test "x${have_lib_MesaGLU}" = xyes; then
      GLU_lib='MesaGLU'
    else
      no_GL=yes
    fi
    
    
    if test "x$have_GL_gl_h" = x; then
      no_GL=yes
    fi
    if test "x$have_GL_glu_h" = x; then
      no_GL=yes
    fi
    if test "x$have_GL_glx_h" = x; then
      no_GL=yes
    fi
    
    
    if test "x${no_GL}" = x; then
      GL_CFLAGS="${GL__Idir} ${X__cflags}"
      GL_LIBS="${GL__Ldir} -l${GLU_lib} -l${GL_lib} ${X__libs}"
      ifelse([$1], , :, [$1])
    else
      GL_CFLAGS=""
      GL_LIBS=""
      ifelse([$2], , :, [$2])     
    fi
    
    AC_SUBST(GL_CFLAGS)
    AC_SUBST(GL_LIBS)
  ]
)

dnl
dnl VRMLGL_PATH_GLUT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRMLGL_PATH_GLUT,
  [dnl
    dnl
    dnl Dependencies
    dnl
    AC_REQUIRE([AC_PATH_XTRA])
    AC_REQUIRE([VRMLGL_PATH_GL])
    
    if test "x$x_includes" = "x"; then
      X__Idir=""
    else
      X__Idir="-I$x_includes"
    fi
    if test "x$x_libraries" = "x"; then
      X__Ldir=""
    else
      X__Ldir="-L$x_libraries"
    fi

    X__cflags="${X__Idir} ${X_CFLAGS}"
    X__libs="${X__Ldir} $X_LDFLAGS ${X_PRE_LIBS} ${X_LIBS} ${X_EXTRA_LIBS}"
    
    AC_CHECK_LIB(Xi, XListInputDevices,
      xi_libs="-lXi", 
      ,
      $X__libs
    )
    
    AC_CHECK_LIB(Xmu, XmuLookupStandardColormap,
      xmu_libs="-lXmu", 
      ,
      $X__libs
    )
    
    AC_ARG_WITH(glut-prefix,
      [  --with-glut-prefix=DIR  pass '-IDIR/include' to CPP, '-LDIR/lib' to LD]
    )
    
    if test -n "${with_glut_prefix}"; then
      glut__Idir="-I${with_glut_prefix}/include"
      glut__Ldir="-L${with_glut_prefix}/lib"
    fi
    
    ac_save_LDFLAGS="${LFDLAGS}"
    LDFLAGS="${glut__Ldir} ${LDFLAGS}"
    
    AC_CHECK_LIB(glut, glutMainLoop,
      [
        ac_save_CPPFLAGS="${CPPFLAGS}"
        CPPFLAGS="${CPPFLAGS} ${glut__Idir}"
        AC_CHECK_HEADER(GL/glut.h,
          have_glut=yes,
          have_glut=no
        )
        CPPFLAGS="${ac_save_CPPFLAGS}"
      ],
      have_glut=no,
      $xmu_libs $xi_libs $GL_LIBS -lm
    )

    LDFLAGS="${ac_save_LDFLAGS}"
    
    if test $have_glut = yes; then
      GLUT_CFLAGS="${glut__Idir} $GL_CFLAGS"
      GLUT_LIBS="${glut__Ldir} -lglut $xmu_libs $xi_libs $GL_LIBS"
      ifelse([$1], , :, [$1])
    else
      GLUT_LIBS=""
      ifelse([$2], , :, [$2])
    fi
    
    AC_SUBST(GLUT_CFLAGS)
    AC_SUBST(GLUT_LIBS)
  ]
)

dnl
dnl AM_PATH_VRMLGL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(AM_PATH_VRMLGL,
  [dnl
    dnl
    dnl Dependencies
    dnl
    AC_REQUIRE([VRMLGL_PATH_GLUT]) dnl Need to get rid of this and just depend on OpenGL.
    AC_REQUIRE([AM_PATH_VRML])
    
    dnl
    dnl Test for libvrml97gl
    dnl
    AC_MSG_CHECKING([for libvrml97gl])
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    ac_save_LIBS="$LIBS"
    LIBS="-lvrml97gl $VRML_LIBS $GLUT_LIBS $LIBS"
    AC_TRY_LINK(
      [ #include<vrml97gl/ViewerOpenGL.h> ],
      [ int x = ViewerOpenGL::MAX_LIGHTS; ],
      have_vrmlgl=yes,
      have_vrmlgl=no
    )
    LIBS="$ac_save_LIBS"
    
    if test $have_vrmlgl = yes; then
      AC_MSG_RESULT([yes])
      VRMLGL_LIBS="-lvrml97gl $VRML_LIBS $GLUT_LIBS"
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      VRMLGL_LIBS=""
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
    AC_SUBST(VRMLGL_LIBS)
  ]
)
