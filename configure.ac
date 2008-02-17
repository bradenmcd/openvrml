AC_INIT([OpenVRML], [0.17.5], [openvrml-develop@lists.sourceforge.net])
AC_PREREQ([2.53])
AC_COPYRIGHT([Portions copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 Braden McDaniel])
AC_CONFIG_FILES([Makefile]
                [doc/Makefile]
                [ide-projects/Makefile]
                [models/Makefile]
                [models/audio/Makefile]
                [models/textures/Makefile]
                [lib/Makefile]
                [java/Makefile]
                [java/vrml/Makefile]
                [java/vrml/node/Makefile]
                [java/vrml/field/Makefile]
                [src/Makefile]
                [src/libopenvrml/openvrml-config.h]
                [src/libopenvrml-gl/openvrml-gl-config.h]
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
REQUIRE_FREETYPE="freetype2"
REQUIRE_XULRUNNER_JS="xulrunner-js"
REQUIRE_FIREFOX_JS="firefox-js"
REQUIRE_SEAMONKEY_JS="seamonkey-js"
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

#
# Allow users to specify any Boost library name suffix
#
AC_ARG_VAR([BOOST_LIB_SUFFIX], [Boost library name suffix])

AC_CACHE_CHECK([for boost_thread$BOOST_LIB_SUFFIX library],
[ov_cv_boost_thread],
[ov_cv_boost_thread=no
ov_save_LIBS=$LIBS
LIBS="-lboost_thread$BOOST_LIB_SUFFIX $LIBS"
AC_LANG_PUSH([C++])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <boost/thread.hpp>]],
                                [[boost::thread t]])],
               [ov_cv_boost_thread=yes])
AC_LANG_POP
LIBS=$ov_save_LIBS
])
AS_IF([test X$ov_cv_boost_thread = Xno],
      [AC_MSG_FAILURE([libboost_thread$BOOST_LIB_SUFFIX not found])])

PKG_CHECK_MODULES([FONTCONFIG], [$REQUIRE_FONTCONFIG], , [have_fontconfig=no])
PKG_CHECK_MODULES([FREETYPE], [$REQUIRE_FREETYPE], , [have_freetype=no])
#
# FreeType >= 2.2 uses const in callback function signatures.  This
# doesn't cause an error in C; but it does in C++.
#
AS_IF([test X$have_freetype != Xno],
      [AC_MSG_CHECKING([if FreeType callback function signatures use const])
       AC_CACHE_VAL([ov_cv_ft_const],
[AC_LANG_PUSH([C++])
save_CXXFLAGS="${CXXFLAGS}"
CXXFLAGS="${CXXFLAGS} ${FREETYPE_CFLAGS}"
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

have_libmozjs=yes
PKG_CHECK_MODULES([JS], [$REQUIRE_XULRUNNER_JS],
                  [mozilla_js_pkg=$REQUIRE_XULRUNNER_JS],
                  [PKG_CHECK_MODULES([JS], [$REQUIRE_FIREFOX_JS],
                                     [mozilla_js_pkg=$REQUIRE_FIREFOX_JS],
                                     [PKG_CHECK_MODULES([JS], [$REQUIRE_SEAMONKEY_JS],
                                                        [mozilla_js_pkg=$REQUIRE_SEAMONKEY_JS],
                                                        [PKG_CHECK_MODULES([JS], [$REQUIRE_MOZILLA_JS],
                                                                           [mozilla_js_pkg=$REQUIRE_MOZILLA_JS],
                                                                           [have_libmozjs=no])])])])
AC_CHECK_LIB([js], [JS_Init], [have_libjs=yes], [have_libjs=no])

#
# openvrml-xembed and openvrml-player both use GOption, which was
# introduced in GLib 2.6.
#
PKG_CHECK_MODULES([GTK], [gtk+-2.0 gthread-2.0 glib-2.0 >= 2.6], ,
                  [have_gtk=no])
PKG_CHECK_MODULES([GLADE], [libglade-2.0], , [have_libglade=no])
#
# openvrml-player uses GNOME_PARAM_GOPTION_CONTEXT, which was
# introduced in libgnome 2.14.
#
PKG_CHECK_MODULES([GNOMEUI], [libgnomeui-2.0 libgnome-2.0 >= 2.14], ,
                  [have_gnomeui=no])
PKG_CHECK_MODULES([CURL], [libcurl], , [have_libcurl=no])
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
# Enable use of the Gecko -rpath flag
#
AC_ARG_ENABLE([gecko-rpath],
              [AC_HELP_STRING([--enable-gecko-rpath],
                              [enable use of Gecko runtime library directory -rpath])])

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
if test X$enable_render_text_node != Xno; then
  if test X$have_fontconfig = Xno -o X$have_freetype = Xno; then
    AC_MSG_FAILURE([fontconfig and FreeType are required for rendering Text nodes.])
  fi
  if test -n "${OPENVRML_PKG_REQUIRES}"; then
    OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES},"
  fi
  OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES} ${REQUIRE_FONTCONFIG} ${REQUIRE_FREETYPE}"
  AC_DEFINE([OPENVRML_ENABLE_RENDER_TEXT_NODE], [1],
            [Defined if support for rendering Text nodes is enabled.])
  AC_DEFINE_UNQUOTED([OPENVRML_FT_CONST], [$ov_cv_ft_const],
                     [const for FreeType callback function signatures.])
fi

#
# Script node JavaScript support
#
AC_ARG_VAR([GRE_PATH], [Path to Gecko runtime libraries])
AC_ARG_ENABLE([script-node-javascript],
              [AC_HELP_STRING([--disable-script-node-javascript],
                              [disable Script node JavaScript support])])
AC_ARG_WITH([libjs],
            [AC_HELP_STRING([--with-libjs],
                            [use libjs for JavaScript support])])
AS_IF([test "X$enable_script_node_javascript" = "Xno"],
[JS_CFLAGS="" JS_LIBS=""],
[AS_IF([test X$with_libjs = Xyes],
       [JS_CFLAGS="" JS_LIBS="-ljs"])
AS_IF([test X$have_libmozjs = Xno -a X$have_libjs = Xno],
      [AC_MSG_FAILURE([SpiderMonkey is required for Script node JavaScript support.])])
AS_IF([test X$with_libjs != Xyes],
      [AS_IF([test -n "${OPENVRML_PKG_REQUIRES}"],
             [OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES},"])
       OPENVRML_PKG_REQUIRES="${OPENVRML_PKG_REQUIRES} ${mozilla_js_pkg}"])
AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT], [1],
          [Defined if Script node JavaScript support is enabled.])
#
# Ugh.  libmozjs from Firefox 2.0 needs -DJS_THREADSAFE; but the
# Cflags from the pkg-config metadata don't include it.  Check
# mozilla-config.h; if we find JS_THREADSAFE there, assume we need
# it.
#
moz_includedir=`$PKG_CONFIG --variable=includedir $mozilla_js_pkg`
AS_IF([grep JS_THREADSAFE $moz_includedir/mozilla-config.h > /dev/null],
      [AS_IF([$PKG_CONFIG --cflags $mozilla_js_pkg | grep JS_THREADSAFE > /dev/null], ,
             [JS_CFLAGS="${JS_CFLAGS} -DJS_THREADSAFE"])])
])
AC_SUBST([JS_CFLAGS])
AC_SUBST([JS_LIBS])

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
# build the XEmbed control
#
AC_ARG_ENABLE([xembed],
              [AC_HELP_STRING([--disable-xembed],
                              [do not build the XEmbed control])])
AS_IF([test X$enable_xembed != Xno],
      [AS_IF([test X$have_gtk = Xno],
             [AC_MSG_FAILURE([GTK+ is required to build the XEmbed control])])
       AS_IF([test X$enable_gl_renderer = Xno],
             [AC_MSG_FAILURE([the XEmbed control cannot be built without the GL renderer])])])
AM_CONDITIONAL([ENABLE_XEMBED], [test X$enable_xembed != Xno])

#
# build openvrml-player
#
AC_ARG_ENABLE([player],
              [AC_HELP_STRING([--disable-player],
                              [do not build OpenVRML Player])])
AS_IF([test X$enable_player != Xno],
      [AS_IF([test X$enable_xembed = Xno],
             [AC_MSG_FAILURE([OpenVRML Player cannot be built without the XEmbed control])])
       AS_IF([test X$have_libglade = Xno],
             [AC_MSG_FAILURE([libglade is required to build OpenVRML Player])])
       AS_IF([test X$have_gnomeui = Xno],
             [AC_MSG_FAILURE([libgnomeui is required to build OpenVRML Player])])
       AS_IF([test X$have_libcurl = Xno],
             [AC_MSG_FAILURE([libcurl is required to build OpenVRML Player])])
])
AM_CONDITIONAL([ENABLE_PLAYER], [test X$enable_player != Xno])

#
# build Mozilla plug-in
#
AC_ARG_ENABLE([mozilla-plugin],
              [AC_HELP_STRING([--disable-mozilla-plugin],
                              [do not build the Mozilla plug-in])])
AS_IF([test X$enable_mozilla_plugin != Xno],
      [AS_IF([test X$enable_xembed = Xno],
             [AC_MSG_FAILURE([the Mozilla plug-in cannot be built without the XEmbed control])])])

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
               [test "X`echo $GL_LIBS`" != "X`echo $GL_LIBS | sed s/-framework//`" -a -n "$REZ"])
AM_CONDITIONAL([ENABLE_SHARED], [test "X$enable_shared" != "Xno"])

#
# Optional dependencies (substituted in the pkg-config metadata files).
#
AC_SUBST([OPENVRML_PKG_REQUIRES])

#
# Determine whether to apply the -rpath for the Gecko runtime libraries.
#
AS_IF([test "X$enable_gecko_rpath" = Xyes],
[AS_IF([test -z "$GRE_PATH"],
       [GRE_PATH=`echo $JS_LIBS | sed -e 's/^-L\([[^ ]]*\).*/\1/'`
        AS_IF([test "X$GRE_PATH" = "X$JS_LIBS"],
              [GRE_PATH=""
               AC_MSG_ERROR([GRE_PATH could not be derived from JS_LIBS.  Set GRE_PATH if executables linking with libopenvrml should be built with an -rpath for the Gecko runtime libraries.])])])
AS_IF([test -n "${openvrml_run_path}" -a -n "${GRE_PATH}"],
      [openvrml_run_path=":${openvrml_run_path}"])
openvrml_run_path="${GRE_PATH}${openvrml_run_path}"
OPENVRML_RPATH="-rpath ${openvrml_run_path}"
])
AC_SUBST([OPENVRML_RPATH])

if test X$enable_xembed != Xno; then
  gtkglext_subdir=gtkglext
fi
AC_SUBST([gtkglext_subdir])

AC_CONFIG_SUBDIRS([lib/gtkglext mozilla-plugin])

AC_CONFIG_TESTDIR([tests])

AC_OUTPUT
