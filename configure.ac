AC_PREREQ([2.53])
AC_INIT([OpenVRML], [0.13.0], [openvrml-develop@lists.sourceforge.net])
AM_INIT_AUTOMAKE([$PACKAGE_TARNAME], [$PACKAGE_VERSION])

AM_CONFIG_HEADER([config.h])

REQUIRE_MOZILLA_JS="mozilla-js >= 1.2.1"

#
# Checks for programs.
#
AC_ENABLE_STATIC(yes)
enable_static=yes
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PATH_PROG([DOXYGEN], [doxygen])

#
# Checks for libraries/headers.
#
AC_CHECK_LIB([z], [zlibVersion],
             [Z_LIBS="-lz"],
             [have_zlib=no])
AC_CHECK_HEADER([zlib.h], ,
                [have_zlib=no])

AC_CHECK_LIB([png], [png_read_info],
             [PNG_LIBS="-lpng -lz -lm"],
             [have_libpng=no],
             [-lz -lm])
AC_CHECK_HEADER([png.h], ,
                [have_libpng=no])

AC_CHECK_LIB([jpeg], [jpeg_read_header],
             [JPEG_LIBS="-ljpeg"],
             [have_libjpeg=no])
AC_CHECK_HEADER([jpeglib.h], ,
                [have_libjpeg=no])

PKG_CHECK_MODULES([JS], [$REQUIRE_MOZILLA_JS], , [have_js=no])

OV_WITH_GL
OV_WITH_GLUT

#
# gzipped file support
#
AC_ARG_ENABLE([gzip], [  --disable-gzip          disable support for gzipped worlds])
if test "X$enable_gzip" = "Xno"; then
  Z_LIBS=""
else
  if test "X$have_zlib" = "Xno"; then
    AC_MSG_ERROR([zlib is required for gzipped world support.])
  fi
  AC_DEFINE([OPENVRML_ENABLE_GZIP], [1], [Defined if support for gzipped worlds is enabled.])
fi
AC_SUBST([Z_LIBS])

#
# ImageTexture node support
#
AC_ARG_ENABLE([imagetexture-node], [  --disable-imagetexture-node  disable ImageTexture node support])
if test "X$enable_imagetexture_node" = "Xno"; then
  PNG_LIBS=""
  JPEG_LIBS=""
else
  if test "X$have_libpng" = "Xno" -o "X$have_libjpeg" = "Xno"; then
    AC_MSG_ERROR([libpng and libpng are required for ImageTexture node support.])
  fi
  AC_DEFINE([OPENVRML_ENABLE_IMAGETEXTURE_NODE], [1], [Defined if ImageTexture node support is enabled.])
fi
AC_SUBST([PNG_LIBS])
AC_SUBST([JPEG_LIBS])

#
# Script node JavaScript support
#
AC_ARG_ENABLE([script-node-javascript], [  --disable-script-node-javascript  disable Script node JavaScript support])
if test "X$enable_script_node_javascript" = "Xno"; then
  JS_CFLAGS=""
  JS_LIBS=""
else
  if test "X$have_js" = "Xno"; then
    AC_MSG_ERROR([Mozilla JavaScript is required for Script node JavaScript support.])
  fi
  if test -n "${OPENVRML_PKG_REQUIRES}"; then
    REQUIRE_MOZILLA_JS="${OPENVRML_PKG_REQUIRES},"
  fi
  OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES} ${REQUIRE_MOZILLA_JS}"
  AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT], [1], [Defined if Script node JavaScript support is enabled.])
fi
AC_SUBST([JS_CFLAGS])
AC_SUBST([JS_LIBS])

#
# build API docs
#
AC_ARG_ENABLE([api-docs], [  --disable-api-docs      do not build API documentation])
if test "X$enable_api_docs" = "Xno"; then
  DOXYGEN=""
else
  if test -z "$DOXYGEN"; then
    AC_MSG_ERROR([doxygen is required to build the API documentation])
  fi
fi
AM_CONDITIONAL([ENABLE_API_DOCS], [test -n "$DOXYGEN"])

#
# build GL renderer
#
AC_ARG_ENABLE([gl-renderer], [  --disable-gl-renderer   do not build GL renderer])
if test "X$enable_gl_renderer" != "Xno"; then
  if test "X$no_gl" = "Xyes"; then
    AC_MSG_ERROR([OpenGL/Mesa is required for the GL renderer])
  fi
fi
AM_CONDITIONAL([ENABLE_GL_RENDERER],
               [test "X$enable_gl_renderer" != "Xno" -a "X$no_gl" != "Xyes"])

#
# build lookat
#
AC_ARG_ENABLE([lookat], [  --disable-lookat        do not build lookat])
if test "X$enable_lookat" != "Xno"; then
  if test "X$no_glut" = "Xyes"; then
    AC_MSG_ERROR([glut is required for lookat])
  fi
fi
AM_CONDITIONAL([ENABLE_LOOKAT],
               [test "X$enable_lookat" != "Xno" -a "X$no_glut" != "Xyes"])

AC_ARG_WITH(jdk, [  --with-jdk              use the Java Development Kit (Java support)], ,
             [# Use the JDK by default.
              with_jdk=yes])
AC_ARG_WITH(ftflib, [  --with-ftflib           use ftflib (required for font support)], ,
            [# Use ftflib by default.
             with_ftflib=yes])

#
# Check for the Java Development Kit.
#
if test "X$with_jdk" = "Xyes"; then
  AC_PATH_PROG(JDK_JAVAC, javac)
  if test -z "${JDK_JAVAC}"; then
    AC_MSG_ERROR([Java compiler not found.])
  fi
  AC_PATH_PROG(JAR, jar)
  if test -z "${JAR}"; then
    AC_MSG_ERROR([Java archive utility not found.])
  fi
  AC_PATH_PROG(JAVAH, javah)
  if test -z "${JAVAH}"; then
    AC_MSG_ERROR([JNI header generator not found.])
  fi
  OV_CHECK_JNI
  if test "X$no_jni" = "Xyes"; then
    AC_MSG_ERROR([Java Native Interface not found.])
  else
    AC_DEFINE(OPENVRML_HAVE_JNI, , [defined if JNI is available and should be used])
  fi
fi
AM_CONDITIONAL([WITH_JDK], [test "X$with_jdk" = "Xyes"])

#
# Check for ftflib.
#
if test "X$with_ftflib" = "Xyes"; then
  OV_CHECK_FONTS
  if test "X$have_font" = "Xyes"; then 
    OV_CHECK_FTFLIB
    if test "X$no_ftflib" = "Xyes"; then
      AC_MSG_ERROR([ftflib not found.])
    else
      AC_DEFINE([OPENVRML_HAVE_FREETYPEFONTS], , [Defined if ftflib is available and should be used.])
    fi
  else
    AC_MSG_ERROR([At least SerifPlain, SansPlain and TypewriterPlain fonts must exist to support font. Check TrueType (*.ttf) or Type1 (*.pfa,*.pfb) in your system.])
  fi
fi

AC_SUBST([OPENVRML_PKG_REQUIRES])
    
AC_OUTPUT([Makefile \
doc/Makefile \
doc/Doxyfile \
m4/Makefile \
models/Makefile \
models/audio/Makefile \
models/textures/Makefile \
lib/Makefile \
lib/antlr/Makefile \
src/Makefile \
src/openvrml/Makefile \
src/openvrml/OpenVRML/Makefile \
src/openvrml-gl/Makefile \
src/openvrml-gl/OpenVRML/Makefile \
src/openvrml-gl/OpenVRML/GL/Makefile \
src/lookat/Makefile \
java/Makefile \
java/vrml/Makefile \
java/vrml/node/Makefile \
java/vrml/field/Makefile \
openvrml.pc \
openvrml-gl.pc \
openvrml.spec])
