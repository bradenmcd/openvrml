AC_INIT([OpenVRML], [0.15.99], [openvrml-develop@lists.sourceforge.net])
AC_PREREQ([2.53])
AC_COPYRIGHT([Portions copyright 2000, 2001, 2002, 2003, 2004, 2005 Braden McDaniel])
AC_CONFIG_FILES([Makefile]
                [doc/Makefile]
                [m4/Makefile]
                [ide-projects/Makefile]
                [models/Makefile]
                [models/audio/Makefile]
                [models/textures/Makefile]
                [lib/Makefile]
                [lib/antlr/GNUmakefile]
                [java/Makefile]
                [java/vrml/Makefile]
                [java/vrml/node/Makefile]
                [java/vrml/field/Makefile]
                [src/Makefile]
                [src/libopenvrml/Makefile]
                [src/libopenvrml-gl/Makefile]
                [src/lookat/Makefile]
                [test/Makefile]
                [test/parse/Makefile]
                [test/parse/testsuite/Makefile]
                [openvrml.pc]
                [openvrml.spec])

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.7 foreign dejagnu subdir-objects check-news])
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
# AM_INIT_AUTOMAKE should probably take care of this, but it doesn't.
#
AC_ARG_VAR([RUNTESTFLAGS], [DejaGnu runtest flags])

#
# Checks for libraries/headers.
#
PKG_CHECK_MODULES([FONTCONFIG], [$REQUIRE_FONTCONFIG], , [have_fontconfig=no])
AC_PATH_PROG([FREETYPE_CONFIG], [freetype-config])

PKG_CHECK_MODULES([JS], [$REQUIRE_MOZILLA_JS], , [have_js=no])

AX_CHECK_GLU

AM_PATH_SDL(, [have_sdl=yes], [have_sdl=no])

#
# sdl-config (which is used by AM_PATH_SDL above) emits an rpath flag pointing
# at its library installation directory.  We don't want this, as it prevents
# users from linking lookat against, for example, a locally compiled libGL when
# a version of the library also exists in SDL's library installation directory,
# typically /usr/lib.
#
SDL_LIBS=`echo $SDL_LIBS | sed 's/-Wl,-rpath,[[^ ]]* //'`

OV_CHECK_JNI

AC_CHECK_HEADER([dejagnu.h], [have_dejagnu_h=yes], [have_dejagnu_h=no])

#
# PNG texture support
#
AC_ARG_ENABLE([png-textures],
              [AC_HELP_STRING([--disable-png-textures],
                              [disable support for rendering PNG textures])])
if test X$enable_png_textures = Xno; then
  PNG_LIBS=""
else
  PNG_LIBS="-lpng -lz"
  AC_DEFINE([OPENVRML_ENABLE_PNG_TEXTURES], [1],
            [Defined if support for rendering PNG textures is enabled.])
fi
AC_SUBST([PNG_LIBS])

#
# JPEG texture support
#
AC_ARG_ENABLE([jpeg-textures],
              [AC_HELP_STRING([--disable-jpeg-textures],
                              [disable support for rendering JPEG textures])])
if test X$enable_jpeg_textures = Xno; then
  JPEG_LIBS=""
else
  JPEG_LIBS="-ljpeg"
  AC_DEFINE([OPENVRML_ENABLE_JPEG_TEXTURES], [1],
            [Defined if support for rendering JPEG textures is enabled.])
fi
AC_SUBST([JPEG_LIBS])

#
# Text node support
#
AC_ARG_ENABLE([render-text-node],
              [AC_HELP_STRING([--disable-render-text-node],
                              [disable support for rendering Text nodes])])
if test "X$enable_render_text_node" = "Xno"; then
  FONTCONFIG_CFLAGS=""
  FONTCONFIG_LIBS=""
  FREETYPE_CFLAGS=""
  FREETYPE_LIBS=""
else
  if test "X$have_fontconfig" = "Xno" -o -z "$FREETYPE_CONFIG"; then
    AC_MSG_FAILURE([fontconfig and FreeType are required for rendering Text nodes.])
  fi
  if test -n "${OPENVRML_PKG_REQUIRES}"; then
    OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES},"
  fi
  OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES} ${REQUIRE_FONTCONFIG}"
  FREETYPE_CFLAGS=`$FREETYPE_CONFIG --cflags`
  FREETYPE_LIBS=`$FREETYPE_CONFIG --libs`
  AC_DEFINE([OPENVRML_ENABLE_RENDER_TEXT_NODE], [1],
            [Defined if support for rendering Text nodes is enabled.])
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
    AC_MSG_FAILURE([Mozilla JavaScript is required for Script node JavaScript support.])
  fi
  if test -n "${OPENVRML_PKG_REQUIRES}"; then
    OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES},"
  fi
  OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES} ${REQUIRE_MOZILLA_JS}"
  AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT], [1],
            [Defined if Script node JavaScript support is enabled.])
  GRE_PATH=`$PKG_CONFIG --libs-only-L mozilla-js | sed -e 's/^-L//' -e 's/ *$//'`
  if test -n "${openvrml_run_path}" -a -n "${GRE_PATH}"; then
    openvrml_run_path=":${openvrml_run_path}"
  fi
  openvrml_run_path="${GRE_PATH}${openvrml_run_path}"
fi
AC_SUBST([JS_CFLAGS])
AC_SUBST([JS_LIBS])
AC_SUBST([GRE_PATH])

#
# Script node Java support
#
AC_ARG_ENABLE([script-node-java],
              [AC_HELP_STRING([--disable-script-node-java],
                              [disable Script node Java support])])
if test "X$enable_script_node_java" != "Xno"; then
  if test "X$no_jni" = "Xyes"; then
    AC_MSG_FAILURE([The Java Native Interface is required for Script node Java support.])
  fi
  AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVA], [1],
            [Defined if Script node Java support is enabled.])
fi
AM_CONDITIONAL([ENABLE_SCRIPT_NODE_JAVA],
               [test "X$enable_script_node_java" != "Xno"])

#
# build GL renderer
#
AC_ARG_ENABLE([gl-renderer],
              [AC_HELP_STRING([--disable-gl-renderer],
                              [do not build GL renderer])])
if test "X$enable_gl_renderer" != "Xno"; then
  if test "X$no_gl" = "Xyes"; then
    AC_MSG_FAILURE([OpenGL/Mesa is required for the GL renderer])
  fi
  AC_CONFIG_FILES([openvrml-gl.pc])
fi
AM_CONDITIONAL([ENABLE_GL_RENDERER],
               [test "X$enable_gl_renderer" != "Xno" -a "X$no_gl" != "Xyes"])

#
# build lookat
#
AC_ARG_ENABLE([lookat],
              [AC_HELP_STRING([--disable-lookat], [do not build lookat])])
if test "X$enable_lookat" != "Xno"; then
  if test "X$have_sdl" != "Xyes"; then
    AC_MSG_FAILURE([SDL is required for lookat])
  fi
fi
AM_CONDITIONAL([ENABLE_LOOKAT],
               [test "X$enable_lookat" != "Xno" -a "X$have_sdl" = "Xyes"])

#
# build Mozilla plug-in
#
AC_ARG_ENABLE([mozilla-plugin],
              [AC_HELP_STRING([--disable-mozilla-plugin],
                              [do not build the Mozilla plug-in])])

#
# build tests
#
AC_ARG_ENABLE([tests], [AC_HELP_STRING([--disable-tests],
                                       [do not build the tests])])
if test X$enable_tests != Xno; then
  if test X$have_dejagnu_h != Xyes; then
    AC_MSG_FAILURE([DejaGnu is required to build and run the tests])
  fi
fi
AM_CONDITIONAL([ENABLE_TESTS],
               [test X$enable_tests != Xno -a X$have_dejagnu_h = Xyes]) 

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
if test -n "${openvrml_run_path}"; then 
  OPENVRML_RPATH="-rpath ${openvrml_run_path}"
fi
AC_SUBST([OPENVRML_RPATH])

AC_ARG_VAR([XPIDL], [Mozilla XPCOM IDL compiler])
AC_ARG_VAR([XPIDLFLAGS], [Mozilla XPCOM IDL compiler flags])
AC_CONFIG_SUBDIRS([mozilla-plugin])

AC_OUTPUT
