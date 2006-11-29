AC_INIT([OpenVRML], [0.16.2], [openvrml-develop@lists.sourceforge.net])
AC_PREREQ([2.53])
AC_COPYRIGHT([Portions copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006 Braden McDaniel])
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
                [src/libopenvrml/openvrml-config.h]
                [src/libopenvrml-gl/Makefile]
                [src/libopenvrml-gl/openvrml-gl-config.h]
                [src/openvrml-gtkplug/Makefile]
                [examples/Makefile]
                [tests/Makefile]
                [tests/atlocal]
                [openvrml.pc])

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.9 foreign subdir-objects check-news tar-pax])
AM_CONFIG_HEADER([config.h])

#
# These strings are used with PKG_CHECK_MODULES.  If the corresponding
# features are enabled, they also propagate to the pkg-config metadata files
# output by configure.
#
REQUIRE_FONTCONFIG="fontconfig"
REQUIRE_FIREFOX_JS="firefox-js"
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
OV_PROG_ANTLR([2.7.4])
AS_IF([test -z "$ANTLR"],
      [AS_IF([test ! -f $srcdir/src/libopenvrml/openvrml/Vrml97Parser.cpp],
             [AC_MSG_FAILURE([antlr 2.7.4 not found])],
             [AC_MSG_WARN([antlr 2.7.4 not found; but generated files already exist])])])

AC_ARG_VAR([DOXYGEN], [Doxygen documentation generator])
AC_PATH_PROG([DOXYGEN], [doxygen])

AC_ARG_VAR([REZ], [Mac OS X resource fork compiler])
AC_PATH_PROG([REZ], [Rez], , [$PATH:/Developer/Tools])

#
# Checks for compiler features
#
OV_CXX_VISIBILITY(
[OPENVRML_API="__attribute__((visibility(\"default\")))"
OPENVRML_LOCAL="__attribute__((visibility(\"hidden\")))"
OPENVRML_GL_API="__attribute__((visibility(\"default\")))"
OPENVRML_GL_LOCAL="__attribute__((visibility(\"hidden\")))"])
AC_SUBST([OPENVRML_API])
AC_SUBST([OPENVRML_LOCAL])
AC_SUBST([OPENVRML_GL_API])
AC_SUBST([OPENVRML_GL_LOCAL])

#
# Checks for libraries/headers.
#
ACX_PTHREAD(, [AC_MSG_FAILURE([pthread support not found])])

PKG_CHECK_MODULES([FONTCONFIG], [$REQUIRE_FONTCONFIG], , [have_fontconfig=no])
AC_PATH_PROG([FREETYPE_CONFIG], [freetype-config])
#
# FreeType >= 2.2 uses const in callback function signatures.  This
# doesn't cause an error in C; but it does in C++.
#
AS_IF([test -n "${FREETYPE_CONFIG}"],
      [AC_MSG_CHECKING([if FreeType callback function signatures use const])
       AC_CACHE_VAL([ov_cv_ft_const],
[AC_LANG_PUSH([C++])
save_CXXFLAGS="${CXXFLAGS}"
CXXFLAGS="${CXXFLAGS} `${FREETYPE_CONFIG} --cflags`"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
                  [#include <ft2build.h>
#include FT_OUTLINE_H
int moveTo(const FT_Vector *, void *);],
                  [FT_Outline_MoveToFunc f = moveTo])],
                  [ov_cv_ft_const=const],
                  [ov_cv_ft_const= ])
CXXFLAGS="${save_CXXFLAGS}"
AC_LANG_POP])
       AS_IF([test -n "$ov_cv_ft_const"],
             [AC_MSG_RESULT([yes])],
             [AC_MSG_RESULT([no])])
])


PKG_CHECK_MODULES([JS], [$REQUIRE_FIREFOX_JS],
                  [mozilla_js_pkg=$REQUIRE_FIREFOX_JS],
                  [PKG_CHECK_MODULES([JS], [$REQUIRE_MOZILLA_JS],
                                     [mozilla_js_pkg=$REQUIRE_MOZILLA_JS],
                                     [have_js=no])])
PKG_CHECK_MODULES([GTK], [gtk+-2.0], , [have_gtk=no])
AC_PATH_XTRA
AX_CHECK_GLU

AC_PATH_PROG([SDL_CONFIG], [sdl-config])
have_sdl=no
if test -n "${SDL_CONFIG}"; then
    have_sdl=yes
    SDL_CFLAGS=`$SDL_CONFIG --cflags`
    SDL_LIBS=`$SDL_CONFIG --libs`
    #
    # sdl-config sometimes emits an rpath flag pointing at its library
    # installation directory.  We don't want this, as it prevents users from
    # linking sdl-viewer against, for example, a locally compiled libGL when a
    # version of the library also exists in SDL's library installation
    # directory, typically /usr/lib.
    #
    SDL_LIBS=`echo $SDL_LIBS | sed 's/-Wl,-rpath,[[^ ]]* //'`
fi
AC_SUBST([SDL_CFLAGS])
AC_SUBST([SDL_LIBS])

OV_CHECK_JNI

#
# Allow users to specify any Boost library name suffix
#
AC_ARG_VAR([BOOST_LIB_SUFFIX], [Boost library name suffix])

#
# Use exception specifications?
#
AC_ARG_ENABLE([exception-specs],
              [AC_HELP_STRING([--disable-exception-specs],
                              [disable use of C++ exception specifications])])
if test X$enable_exceptions_specs = Xno; then
  OPENVRML_ENABLE_EXCEPTION_SPECS=0
else
  OPENVRML_ENABLE_EXCEPTION_SPECS=1
fi
AC_SUBST([OPENVRML_ENABLE_EXCEPTION_SPECS])

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
  AC_DEFINE_UNQUOTED([OPENVRML_FT_CONST], [$ov_cv_ft_const],
                     [const for FreeType callback function signatures.])
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
  OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES} ${mozilla_js_pkg}"
  AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT], [1],
            [Defined if Script node JavaScript support is enabled.])
  GRE_PATH=`$PKG_CONFIG --libs-only-L ${mozilla_js_pkg%% *} | sed -e 's/^-L//' -e 's/ *$//'`
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
              [AC_HELP_STRING([--enable-script-node-java],
                              [enable Script node Java support])])
if test "X$enable_script_node_java" = "Xyes"; then
  if test "X$no_jni" = "Xyes"; then
    AC_MSG_FAILURE([The Java Native Interface is required for Script node Java support.])
  fi
  AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVA], [1],
            [Defined if Script node Java support is enabled.])
fi
AM_CONDITIONAL([ENABLE_SCRIPT_NODE_JAVA],
               [test "X$enable_script_node_java" = "Xyes"])

#
# build GL renderer
#
AC_ARG_ENABLE([gl-renderer],
              [AC_HELP_STRING([--disable-gl-renderer],
                              [do not build GL renderer])])
if test "X$enable_gl_renderer" != "Xno"; then
  if test "X$no_gl" = "Xyes"; then
    AC_MSG_FAILURE([OpenGL/Mesa (libGL) is required for the GL renderer])
  fi
  if test "X$no_glu" = "Xyes"; then
    AC_MSG_FAILURE([the OpenGL Utility library (libGLU) is required for the GL renderer])
  fi
  AC_CONFIG_FILES([openvrml-gl.pc])
fi
AM_CONDITIONAL([ENABLE_GL_RENDERER],
               [test "X$enable_gl_renderer" != "Xno" -a "X$no_gl" != "Xyes"])

#
# build the GtkPlug
#
AC_ARG_ENABLE([gtkplug],
              [AC_HELP_STRING([--disable-gtkplug],
                              [do not build the GtkPlug application])])
if test X$enable_gtkplug != Xno; then
  if test X$have_gtk = Xno; then
    AC_MSG_FAILURE([GTK+ is required to build the GtkPlug])
  fi
  if test X$enable_gl_renderer = Xno; then
    AC_MSG_FAILURE([the Mozilla plug-in cannot be built without the GL renderer])
  fi
fi
AM_CONDITIONAL([ENABLE_GTKPLUG],
               [test X$enable_gtkplug != Xno -a X$have_gtk != Xno])

#
# build Mozilla plug-in
#
AC_ARG_ENABLE([mozilla-plugin],
              [AC_HELP_STRING([--disable-mozilla-plugin],
                              [do not build the Mozilla plug-in])])
if test X$enable_mozilla_plugin != Xno; then
  if test X$enable_gtkplug = Xno; then
    AC_MSG_FAILURE([the Mozilla plug-in cannot be built without the GtkPlug application])
  fi
fi

if test X$enable_gtkplug != Xno; then
  if test X$have_gtk = Xno; then
    AC_MSG_FAILURE([GTK+ is required to build the GtkPlug])
  fi
fi
AM_CONDITIONAL([ENABLE_GTKPLUG],
               [test X$enable_gtkplug != Xno -a X$have_gtk != Xno])

#
# build examples
#
AC_ARG_ENABLE([examples],
              [AC_HELP_STRING([--disable-examples],
                              [do not build the example programs])])
if test "X$enable_examples" != "Xno"; then
  if test "X$have_sdl" != "Xyes"; then
    AC_MSG_FAILURE([SDL is required for the example programs])
  fi
fi
AM_CONDITIONAL([ENABLE_EXAMPLES],
               [test "X$enable_examples" != "Xno" -a "X$have_sdl" = "Xyes"])

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

if test X$enable_gtkplug != Xno; then
  gtkglext_subdir=gtkglext
fi
AC_SUBST([gtkglext_subdir])

AC_CONFIG_SUBDIRS([lib/gtkglext mozilla-plugin])

AC_CONFIG_TESTDIR([tests])

AC_OUTPUT
