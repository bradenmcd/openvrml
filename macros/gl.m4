dnl
dnl OV_PATH_GL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(OV_PATH_GL,
  [
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
    
    
    
    AC_ARG_WITH(gl-prefix, [  --with-gl-prefix=PREFIX OpenGL/Mesa installed under PREFIX])
    AC_ARG_WITH(MesaGL, [  --with-MesaGL           use MesaGL library, instead of GL library])
    AC_ARG_WITH(MesaGLU, [  --with-MesaGLU          use MesaGLU library, instead of GLU library])
    
    if test -n "${with_gl_prefix}"; then
      GL__Idir="-I${with_gl_prefix}/include"
      GL__Ldir="-L${with_gl_prefix}/lib"
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
