AC_DEFUN(AM_PATH_VRML,
[
dnl Test for libjpeg
AC_CHECK_LIB(jpeg, jpeg_read_header, 
  jpeg_libs="-ljpeg"
)

dnl Test for libpng
AC_CHECK_LIB(png, png_read_info,
  png_libs="-lpng -lz",
  ,
  -lz -lm
)

dnl Test for libvrml97
AC_MSG_CHECKING([for libvrml97])
AC_LANG_CPLUSPLUS
ac_save_LIBS="$LIBS"
LIBS="-lvrml97 $jpeg_libs $png_libs $LIBS"
AC_TRY_LINK(
  [#include<vrml97/VrmlScene.h>],
  [ VrmlScene::readWrl(0, 0); ],
  have_vrml=yes,
  have_vrml=no
)
LIBS="$ac_save_LIBS"

dnl AC_CHECK_LIB(vrml97, VrmlScene::readWrl,
dnl   AC_CHECK_HEADER(vrml97/VrmlScene.h,
dnl     have_vrml=yes,
dnl     have_vrml=no
dnl   ),
dnl   have_vrml97=no,
dnl   $jpeg_libs $png_libs
dnl )

if test $have_vrml = yes; then
  AC_MSG_RESULT([yes])
  VRML_LIBS="-lvrml97 $jpeg_libs $png_libs $GLUT_LIBS"
  ifelse([$1], , :, [$1])
else
  AC_MSG_RESULT([no])
  VRML_LIBS=""
  ifelse([$2], , :, [$2])
fi

AC_SUBST(VRML_LIBS)

])
