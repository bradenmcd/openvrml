dnl AM_PATH_GLUT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])

AC_DEFUN(AM_PATH_GLUT,
[
dnl Dependencies
AC_REQUIRE([AC_PATH_XTRA])
AC_REQUIRE([AM_PATH_GL])

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
  [  --with-glut-prefix=DIR    pass '-IDIR/include' to CPP, '-LDIR/lib' to LD]
)

if test -n "${with_glut_prefix}"; then
  glut__Idir="-I${with_glut_prefix}/include"
  glut__Ldir="-L${with_glut_prefix}/lib"
fi

AC_CHECK_LIB(glut, glutMainLoop,
  AC_CHECK_HEADER(GL/glut.h,
    have_glut=yes,
    have_glut=no
  ),
  have_glut=no,
  $xmu_libs $xi_libs $GL_LIBS
)

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

])
