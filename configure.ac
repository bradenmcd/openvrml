AC_INIT([OpenVRML], [0.17.99], [openvrml-develop@lists.sourceforge.net])
AC_PREREQ([2.59])
AC_COPYRIGHT([Portions copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 Braden McDaniel])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile]
                [doc/Makefile]
                [ide-projects/Makefile]
                [models/Makefile]
                [models/audio/Makefile]
                [models/textures/Makefile]
                [src/Makefile]
                [src/libopenvrml/openvrml-config.h]
                [src/libopenvrml-gl/openvrml-gl-config.h]
                [src/script/Makefile]
                [src/script/java/Makefile]
                [src/script/java/vrml/Makefile]
                [src/script/java/vrml/field/Makefile]
                [src/script/java/vrml/node/Makefile]
                [data/Makefile]
                [examples/Makefile]
                [tests/Makefile]
                [tests/atlocal]
                [openvrml.pc])

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.10.1 foreign subdir-objects check-news tar-pax])
AM_CONFIG_HEADER([config.h])

#
# These strings are used with PKG_CHECK_MODULES.  If the corresponding
# features are enabled, they also propagate to the pkg-config metadata files
# output by configure.
#
REQUIRE_FONTCONFIG="fontconfig"
REQUIRE_FREETYPE="freetype2"
require_xulrunner_js="xulrunner-js"
require_firefox_js="firefox-js"
require_seamonkey_js="seamonkey-js"
require_mozilla_js="mozilla-js >= 1.2.1"

#
# Checks for programs.
#
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP
AM_PROG_GCJ

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
OPENVRML_GL_LOCAL="__attribute__((visibility(\"hidden\")))"
OPENVRML_JAVASCRIPT_API="__attribute__((visibility(\"default\")))"
OPENVRML_JAVASCRIPT_LOCAL="__attribute__((visibility(\"hidden\")))"
OPENVRML_JAVA_API="__attribute__((visibility(\"default\")))"
OPENVRML_JAVA_LOCAL="__attribute__((visibility(\"hidden\")))"])
AC_SUBST([OPENVRML_API])
AC_SUBST([OPENVRML_LOCAL])
AC_SUBST([OPENVRML_GL_API])
AC_SUBST([OPENVRML_GL_LOCAL])

AC_DEFINE_UNQUOTED([OPENVRML_JAVASCRIPT_API], [$OPENVRML_JAVASCRIPT_API],
                   [Exported symbol])
AC_DEFINE_UNQUOTED([OPENVRML_JAVASCRIPT_LOCAL], [$OPENVRML_JAVASCRIPT_LOCAL],
                   [Library-local symbol])
AC_DEFINE_UNQUOTED([OPENVRML_JAVA_API], [$OPENVRML_JAVA_API],
                   [Exported symbol])
AC_DEFINE_UNQUOTED([OPENVRML_JAVA_LOCAL], [$OPENVRML_JAVA_LOCAL],
                   [Library-local symbol])

#
# Checks for libraries/headers.
#
ACX_PTHREAD(, [AC_MSG_FAILURE([pthread support not found])])
AC_CHECK_HEADER([ltdl.h], , [AC_MSG_FAILURE([ltdl.h not found])])
AC_CHECK_HEADER([jni.h], [have_jni=yes])

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

#
# The XmlTextReader interface appears in libxml 2.5.
#
PKG_CHECK_MODULES([XML], [libxml-2.0 >= 2.5])

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
PKG_CHECK_MODULES([JS], [$require_xulrunner_js],
                  [mozilla_js_pkg=$require_xulrunner_js],
                  [PKG_CHECK_MODULES([JS], [$require_firefox_js],
                                     [mozilla_js_pkg=$require_firefox_js],
                                     [PKG_CHECK_MODULES([JS], [$require_seamonkey_js],
                                                        [mozilla_js_pkg=$require_seamonkey_js],
                                                        [PKG_CHECK_MODULES([JS], [$require_mozilla_js],
                                                                           [mozilla_js_pkg=$require_mozilla_js],
                                                                           [have_libmozjs=no])])])])
AC_CHECK_LIB([js], [JS_Init], [have_libjs=yes], [have_libjs=no])
#
# Ensure that we can, in fact, find and use the SpiderMonkey headers.
#
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_CHECK_HEADER([jsapi.h], [have_jsapi_h=yes], [have_jsapi_h=no])
CPPFLAGS=$ov_save_CPPFLAGS

#
# openvrml-xembed and openvrml-player both use GOption, which was
# introduced in GLib 2.6.
#
PKG_CHECK_MODULES([DBUS_G], [dbus-glib-1 glib-2.0 >= 2.6], ,
                  [have_dbus_glib=no])
AC_ARG_VAR([DBUS_BINDING_TOOL], [Generate stub code for dbus-glib])
AC_PATH_PROG([DBUS_BINDING_TOOL], [dbus-binding-tool])

#
# openvrml-player uses GtkBuilder, which was introduced in GTK+ 2.12.
#
PKG_CHECK_MODULES([GTKGL],
                  [gtkglext-1.0 gtk+-2.0 >= 2.12 gthread-2.0 glib-2.0 >= 2.6], ,
                  [have_gtkgl=no])
#
# openvrml-player uses GNOME_PARAM_GOPTION_CONTEXT, which was
# introduced in libgnome 2.14.
#
PKG_CHECK_MODULES([GNOMEUI], [libgnomeui-2.0 libgnome-2.0 >= 2.14], ,
                  [have_gnomeui=no])
PKG_CHECK_MODULES([CURL], [libcurl], , [have_libcurl=no])
PKG_CHECK_MODULES([MOZILLA_PLUGIN], [xulrunner-plugin],
                  [mozilla_plugin_pkg=xulrunner-plugin],
                  [PKG_CHECK_MODULES([MOZILLA_PLUGIN], [firefox-plugin],
                                     [mozilla_plugin_pkg=firefox-plugin],
                                     [PKG_CHECK_MODULES([MOZILLA_PLUGIN], [seamonkey-plugin],
                                                        [mozilla_plugin_pkg=seamonkey-plugin],
                                                        [PKG_CHECK_MODULES([MOZILLA_PLUGIN], [mozilla-plugin >= 1.6],
                                                                           [mozilla_plugin_pkg=mozilla-plugin],
                                                                           [have_mozilla_plugin=no])])])])

AC_PATH_XTRA
AX_CHECK_GLU
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],
                  [[#ifndef HAVE_WINDOWS_H
  choke
#endif
]])],
                  [OPENVRML_GL_HAVE_WINDOWS_H=1],
                  [OPENVRML_GL_HAVE_WINDOWS_H=0])
AC_SUBST([OPENVRML_GL_HAVE_WINDOWS_H])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],
                  [[#ifndef HAVE_GL_GLU_H
  choke
#endif
]])],
                  [OPENVRML_GL_HAVE_GL_GLU_H=1],
                  [OPENVRML_GL_HAVE_GL_GLU_H=0])
AC_SUBST([OPENVRML_GL_HAVE_GL_GLU_H])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],
                  [[#ifndef HAVE_OPENGL_GLU_H
  choke
#endif
]])],
                  [OPENVRML_GL_HAVE_OPENGL_GLU_H=1],
                  [OPENVRML_GL_HAVE_OPENGL_GLU_H=0])
AC_SUBST([OPENVRML_GL_HAVE_OPENGL_GLU_H])

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

#
# Use exception specifications?
#
AC_ARG_ENABLE([exception-specs],
              [AC_HELP_STRING([--enable-exception-specs[[=yes,no,nothrow]]],
                              [modulate use of C++ exception specifications [default=yes]])])
AS_IF([test X$enable_exception_specs = Xno],
      [OPENVRML_ENABLE_THROWING_EXCEPTION_SPECS=0
       OPENVRML_ENABLE_NOTHROW_EXCEPTION_SPECS=0],
      [OPENVRML_ENABLE_NOTHROW_EXCEPTION_SPECS=1
       AS_IF([test X$enable_exception_specs = Xnothrow],
             [OPENVRML_ENABLE_THROWING_EXCEPTION_SPECS=0],
             [OPENVRML_ENABLE_THROWING_EXCEPTION_SPECS=1])])
AC_SUBST([OPENVRML_ENABLE_THROWING_EXCEPTION_SPECS])
AC_SUBST([OPENVRML_ENABLE_NOTHROW_EXCEPTION_SPECS])

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
[AS_IF([test X$with_libjs = Xyes -a X$have_libjs = Xyes],
       [AS_IF([test -z "$JS_CFLAGS"], [JS_CFLAGS=""])
        AS_IF([test -z "$JS_LIBS"], [JS_LIBS="-ljs"])])
AS_IF([test X$have_libmozjs != Xyes -a \( X$with_libjs = Xyes -a X$have_libjs != Xyes \) -o X$have_jsapi_h != Xyes],
      [AC_MSG_FAILURE([SpiderMonkey is required for Script node JavaScript support.])])
AC_DEFINE([OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT], [1],
          [Defined if Script node JavaScript support is enabled.])
#
# Figure out whether we need to add -DJS_THREADSAFE to JS_CFLAGS.
#
# libmozjs from Firefox 2.0 needs -DJS_THREADSAFE; but the Cflags from
# the pkg-config metadata don't include it.  And if we're using the stand-
# alone SpiderMonkey, we don't have any pkg-config metadata to go on.
#
# Fortunately, there are a few library symbols that should be defined
# only if the installed binary was built with JS_THREADSAFE.  We'll
# try to link with them.
#
AS_IF([test "X$JS_CFLAGS" = "X`echo "$JS_CFLAGS" | sed s/JS_THREADSAFE//`"],
[ov_save_LDFLAGS=$LDFLAGS
LDFLAGS="$JS_LIBS $LDFLAGS"
AC_LINK_IFELSE([AC_LANG_CALL([], [JS_BeginRequest])],
	       [JS_CFLAGS="$JS_CFLAGS -DJS_THREADSAFE"])
LDFLAGS=$ov_save_LDFLAGS
])])
AC_SUBST([JS_CFLAGS])
AC_SUBST([JS_LIBS])

AM_CONDITIONAL([ENABLE_SCRIPT_NODE_JAVASCRIPT],
  [test X$enable_script_node_javascript != Xno -a X$have_js != Xno])

#
# Script node Java support
#
AC_ARG_VAR([JAVA_HOME], [Path to Java Runtime Environment])
AC_ARG_ENABLE([script-node-java],
              [AC_HELP_STRING([--disable-script-node-java],
                              [disable Script node Java support])])
case $target_cpu in
     x86_64) OPENVRML_JVM_ARCH=amd64 ;;
     i?86)   OPENVRML_JVM_ARCH=i386 ;;
     *)      OPENVRML_JVM_ARCH=$target_cpu ;;
esac
AC_SUBST([OPENVRML_JVM_ARCH])
AS_IF([test X$enable_script_node_java != Xno],
[AS_IF([test X$have_jni != Xyes],
       [AC_MSG_FAILURE([The Java Native Interface is required for Script node Java support.])])
AS_IF([test -z "$JAVA_HOME"],
[AC_MSG_WARN([JAVA_HOME has not been set.  JAVA_HOME must be set at run time to locate libjvm.])],
[ov_save_LDFLAGS=$LDFLAGS
LDFLAGS="-L$JAVA_HOME/lib/$OPENVRML_JVM_ARCH/client -L$JAVA_HOME/lib/$OPENVRML_JVM_ARCH/server $LDFLAGS"
AC_CHECK_LIB([jvm], [JNI_CreateJavaVM], [LIBS=$LIBS],
             [AC_MSG_WARN([no libjvm found at JAVA_HOME])])
LDFLAGS=$ov_save_LDFLAGS
])
java_subdir=java])

AC_SUBST([java_subdir])
AM_CONDITIONAL([ENABLE_SCRIPT_NODE_JAVA],
               [test X$enable_script_node_java != Xno])

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
      [AS_IF([test X$have_dbus_glib = Xno],
             [AC_MSG_FAILURE([D-Bus GLib bindings are required to build the XEmbed control])])
       AS_IF([test X$have_gtkgl = Xno],
             [AC_MSG_FAILURE([GtkGLExt is required to build the XEmbed control])])
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
             [AC_MSG_FAILURE([the Mozilla plug-in cannot be built without the XEmbed control])])
       AS_IF([test "X$have_mozilla_plugin" = "Xno"],
             [AC_MSG_FAILURE([Mozilla plug-in development headers are required to build the Mozilla plug-in.])])
       AS_IF([test "X$have_dbus_glib" = "Xno"],
             [AC_MSG_FAILURE([The D-Bus GLib binding is required to build the Mozilla plug-in])])])
AM_CONDITIONAL([ENABLE_MOZILLA_PLUGIN],
  [test "X$enable_mozilla_plugin" != "Xno" -a "X$have_dbus_glib" != "Xno"])

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
# Determine whether to apply the -rpath for the Gecko runtime libraries.
#
AS_IF([test "X$enable_gecko_rpath" = Xyes],
[AS_IF([test -z "$GRE_PATH"],
       [GRE_PATH=`echo $JS_LIBS | sed -e 's/^-L\([[^ ]]*\).*/\1/'`
        AS_IF([test "X$GRE_PATH" = "X$JS_LIBS"],
              [GRE_PATH=""
               AC_MSG_ERROR([GRE_PATH could not be derived from JS_LIBS.  Set GRE_PATH if executables linking with libopenvrml should be built with an -rpath for the Gecko runtime libraries.])])])
AS_IF([test -n "${openvrml_js_run_path}" -a -n "${GRE_PATH}"],
      [openvrml_js_run_path=":${openvrml_js_run_path}"])
openvrml_js_run_path="${GRE_PATH}${openvrml_js_run_path}"
OPENVRML_JAVASCRIPT_RPATH="-Wl,-rpath -Wl,${openvrml_js_run_path}"
])
AC_SUBST([OPENVRML_JAVASCRIPT_RPATH])

AC_CONFIG_TESTDIR([tests])

AC_OUTPUT
