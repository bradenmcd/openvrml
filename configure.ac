AC_INIT([OpenVRML], [0.14.3], [openvrml-develop@lists.sourceforge.net])
AC_PREREQ([2.53])
AC_COPYRIGHT([Portions copyright 2000, 2001, 2002, 2003 Braden McDaniel])
AC_CONFIG_FILES([Makefile]
                [doc/Makefile]
                [m4/Makefile]
                [ide-projects/Makefile]
                [models/Makefile]
                [models/audio/Makefile]
                [models/textures/Makefile]
                [lib/Makefile]
                [lib/antlr/GNUmakefile]
                [lib/antlr/antlr/GNUmakefile]
                [lib/antlr/src/GNUmakefile]
                [lib/regex/Makefile]
                [src/Makefile]
                [src/libopenvrml/Makefile]
                [src/libopenvrml/openvrml/Makefile]
                [src/libopenvrml-gl/Makefile]
                [src/libopenvrml-gl/openvrml/Makefile]
                [src/libopenvrml-gl/openvrml/gl/Makefile]
                [src/lookat/Makefile]
                [java/Makefile]
                [java/vrml/Makefile]
                [java/vrml/node/Makefile]
                [java/vrml/field/Makefile]
                [openvrml.pc]
                [openvrml-gl.pc]
                [openvrml.spec])

AM_INIT_AUTOMAKE([1.7 foreign])
AM_CONFIG_HEADER([config.h])

#
# These strings are used with PKG_CHECK_MODULES.  If the corresponding
# features are enabled, they also propagate to the pkg-config metadata files
# output by configure.
#
REQUIRE_FONTCONFIG="fontconfig"
REQUIRE_MOZILLA_JS="mozilla-js >= 1.2.1"

#
# Checks for programs.
#
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP
OV_PROG_JAVA
OV_PROG_JAVAC
OV_PROG_JAVAH
OV_PROG_JAR

AC_ARG_VAR([DOXYGEN], [Doxygen documentation generator])
AC_PATH_PROG([DOXYGEN], [doxygen])

AC_ARG_VAR([REZ], [Mac OS X resource fork compiler])
AC_PATH_PROG([REZ], [Rez], [$PATH:/Developer/Tools])

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

PKG_CHECK_MODULES([FONTCONFIG], [$REQUIRE_FONTCONFIG], , [have_fontconfig=no])
AC_PATH_PROG([FREETYPE_CONFIG], [freetype-config])

PKG_CHECK_MODULES([JS], [$REQUIRE_MOZILLA_JS], , [have_js=no])

AX_CHECK_GLUT

#
# gzipped file support
#
AC_ARG_ENABLE([gzip],
              [AC_HELP_STRING([--disable-gzip],
                              [disable support for gzipped worlds])])
if test "X$enable_gzip" = "Xno"; then
  Z_LIBS=""
else
  if test "X$have_zlib" = "Xno"; then
    AC_MSG_ERROR([zlib is required for gzipped world support.])
  fi
  AC_DEFINE([OPENVRML_ENABLE_GZIP], [1],
            [Defined if support for gzipped worlds is enabled.])
fi
AC_SUBST([Z_LIBS])

#
# ImageTexture node support
#
AC_ARG_ENABLE([imagetexture-node],
              [AC_HELP_STRING([--disable-imagetexture-node],
                              [disable ImageTexture node support])])
if test "X$enable_imagetexture_node" = "Xno"; then
  PNG_LIBS=""
  JPEG_LIBS=""
else
  if test "X$have_libpng" = "Xno" -o "X$have_libjpeg" = "Xno"; then
    AC_MSG_ERROR([libjpeg and libpng are required for ImageTexture node support.])
  fi
  AC_DEFINE([OPENVRML_ENABLE_IMAGETEXTURE_NODE], [1],
            [Defined if ImageTexture node support is enabled.])
fi
AC_SUBST([PNG_LIBS])
AC_SUBST([JPEG_LIBS])

#
# Text node support
#
AC_ARG_ENABLE([text-node],
              [AC_HELP_STRING([--disable-text-node],
                              [disable Text node support])])
if test "X$enable_text_node" = "Xno"; then
  FONTCONFIG_CFLAGS=""
  FONTCONFIG_LIBS=""
  FREETYPE_CFLAGS=""
  FREETYPE_LIBS=""
else
  if test "X$have_fontconfig" = "Xno" -o -z "$FREETYPE_CONFIG"; then
    AC_MSG_ERROR([fontconfig and FreeType are required for Text node support.])
  fi
  if test -n "${OPENVRML_PKG_REQUIRES}"; then
    OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES},"
  fi
  OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES} ${REQUIRE_FONTCONFIG}"
  FREETYPE_CFLAGS=`$FREETYPE_CONFIG --cflags`
  FREETYPE_LIBS=`$FREETYPE_CONFIG --libs`
  AC_DEFINE([OPENVRML_ENABLE_TEXT_NODE], [1],
            [Defined if Text node support is enabled.])
fi
AC_SUBST([FONTCONFIG_CFLAGS])
AC_SUBST([FONTCONFIG_LIBS])
AC_SUBST([FREETYPE_CFLAGS])
AC_SUBST([FREETYPE_LIBS])

#
# Script node JavaScript support
#
AC_ARG_ENABLE([script-node-javascript],
              [AC_HELP_STRING([--disable-script-node-javascript],
                              [disable Script node JavaScript support])])
if test "X$enable_script_node_javascript" = "Xno"; then
  JS_CFLAGS=""
  JS_LIBS=""
else
  if test "X$have_js" = "Xno"; then
    AC_MSG_ERROR([Mozilla JavaScript is required for Script node JavaScript support.])
  fi
  if test -n "${OPENVRML_PKG_REQUIRES}"; then
    OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES},"
  fi
  OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES} ${REQUIRE_MOZILLA_JS}"
  AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT], [1],
            [Defined if Script node JavaScript support is enabled.])
fi
AC_SUBST([JS_CFLAGS])
AC_SUBST([JS_LIBS])

#
# Script node Java support
#
AC_ARG_ENABLE([script-node-java],
              [AC_HELP_STRING([--enable-script-node-java],
                              [enable Script node Java support])])
if test "X$enable_script_node_java" != "Xyes"; then
  JNI_CFLAGS=""
  JNI_LIBS=""
else
  if test "X$no_jdk" = "Xyes"; then
    AC_MSG_ERROR([The Java Development Kit is required for Script node Java support.])
  fi
  AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVA], [1],
            [Defined if Script node Java support is enabled.])
fi
AC_SUBST([JNI_CFLAGS])
AC_SUBST([JNI_LIBS])
AM_CONDITIONAL([ENABLE_SCRIPT_NODE_JAVA],
               [test "X$enable_script_node_java" = "Xyes" -a "X$no_jdk" != "Xyes"])

#
# build GL renderer
#
AC_ARG_ENABLE([gl-renderer],
              [AC_HELP_STRING([--disable-gl-renderer],
                              [do not build GL renderer])])
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
AC_ARG_ENABLE([lookat],
              [AC_HELP_STRING([--disable-lookat], [do not build lookat])])
if test "X$enable_lookat" != "Xno"; then
  if test "X$no_glut" = "Xyes"; then
    AC_MSG_ERROR([glut is required for lookat])
  fi
fi
AM_CONDITIONAL([ENABLE_LOOKAT],
               [test "X$enable_lookat" != "Xno" -a "X$no_glut" != "Xyes"])

#
# Apply Rez to executables? (Only seem to need this if we're using the Apple
# OpenGL framework.)
#
AM_CONDITIONAL([WITH_REZ],
               [test "X$with_apple_opengl_framework" = "Xyes" -a -n "$REZ"])
AM_CONDITIONAL([ENABLE_SHARED], [test "X$enable_shared" != "Xno"])

#
# Optional dependencies (substituted in the pkg-config metadata files).
#
AC_SUBST([OPENVRML_PKG_REQUIRES])

AC_OUTPUT
