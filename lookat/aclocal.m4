dnl aclocal.m4 generated automatically by aclocal 1.4a

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.


# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AC_PROG_INSTALL])
dnl We require 2.13 because we rely on SHELL being computed by configure.
AC_PREREQ([2.13])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package]))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

dnl VRMLGL_PATH_GL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl
AC_DEFUN(VRMLGL_PATH_GL,
[dnl
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
      $X__libs)


dnl
dnl
dnl

AC_ARG_WITH(GL-prefix,
  [  --with-GL-prefix=DIR    pass '-IDIR/include' to CPP, '-LDIR/lib' to LD])
AC_ARG_WITH(MesaGL,
  [  --with-MesaGL           use '-lMesaGL', instead of '-lGL'])
AC_ARG_WITH(MesaGLU,
  [  --with-MesaGLU          use '-lMesaGLU', instead of '-lGLU'])

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

dnl
dnl
dnl

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

dnl
dnl
dnl

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

])

dnl
dnl VRMLGL_PATH_GLUT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRMLGL_PATH_GLUT,
[
dnl Dependencies
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

AC_DEFUN(AM_PATH_VRMLGL,
[
dnl Dependencies
AC_REQUIRE([VRMLGL_PATH_GLUT]) dnl Need to get rid of this and just depend on OpenGL.
AC_REQUIRE([AM_PATH_VRML])

dnl Test for libvrml97gl
AC_MSG_CHECKING([for libvrml97gl])
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

AC_SUBST(VRMLGL_LIBS)

])

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

