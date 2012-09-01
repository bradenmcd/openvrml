AC_INIT([OpenVRML], [0.18.99], [openvrml-develop@lists.sourceforge.net])
AC_PREREQ([2.59])
AC_COPYRIGHT([Portions copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Braden McDaniel])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile]
                [doc/Makefile]
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
# Checks for programs.
#
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP
AM_PROG_GCJ
PKG_PROG_PKG_CONFIG([0.18.0])

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
AC_CHECK_HEADER([jni.h], [have_jni=yes], [have_jni=no])

#
# Some jni.h implementations (well, GCJ, at least), don't apply const
# in function signatures.
#
AS_IF([test X$have_jni = Xyes],
      [AC_MSG_CHECKING([if JNI function signatures use const])
       AC_CACHE_VAL([ov_cv_jni_const],
[AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
                  [#include <jni.h>],
                  [void (JNIEnv::*fn)(jfloatArray, jsize, jsize, const jfloat *) = &JNIEnv::SetFloatArrayRegion])],
                  [ov_cv_jni_const=const],
                  [ov_cv_jni_const= ])
AC_LANG_POP])
       AS_IF([test -n "$ov_cv_jni_const"],
             [AC_MSG_RESULT([yes])],
             [AC_MSG_RESULT([no])])
])

#
# Allow users to specify any Boost library name suffix
#
AS_IF([test -z "${BOOST_LIB_SUFFIX+x}"], [BOOST_LIB_SUFFIX=-mt])
AC_ARG_VAR([BOOST_LIB_SUFFIX], [Boost library name suffix [default=-mt]])

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
PKG_CHECK_MODULES([XML], [libxml-2.0 >= 2.5], , [have_libxml=no])
AS_IF([test X$have_libxml = Xno -a -z "${XML_LIBS+x}"],
      [AC_MSG_FAILURE([libxml not found])])

PKG_CHECK_MODULES([PNG], [libpng], , [have_libpng=no])
#
# fcfreetype.h is C++-safe in fontconfig >= 2.3.0.
#
PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.3.0], , [have_fontconfig=no])
PKG_CHECK_MODULES([FREETYPE], [freetype2 >= 2.2], , [have_freetype=no])

#
# Require at least XULRunner 1.9 for JS_EncodeString.
#
have_libmozjs=yes
PKG_CHECK_MODULES([JS], [mozilla-js >= 1.9], , [have_libmozjs=no])
#
# JS_BeginRequest should be there only if SpiderMonkey was built with
# -DJS_THREADSAFE.
#
AC_CHECK_LIB([js], [JS_BeginRequest], [have_libjs=yes], [have_libjs=no])

#
# Ensure that we can, in fact, find and use the SpiderMonkey headers.
#
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_CHECK_HEADER([jsapi.h], [have_jsapi_h=yes], [have_jsapi_h=no])
CPPFLAGS=$ov_save_CPPFLAGS

#
# jsval changed from being 32- to 64-bits in XULRunner 2.0.
#
AC_CACHE_CHECK([whether JSVAL_TO_DOUBLE returns a pointer],
[ov_cv_jsval_to_double_returns_pointer],
[ov_cv_jsval_to_double_returns_pointer=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_PUSH([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[
jsval val;
*JSVAL_TO_DOUBLE(val);
]])],
[ov_cv_jsval_to_double_returns_pointer=yes])
AC_LANG_POP([C])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_jsval_to_double_returns_pointer = Xyes],
      [AC_DEFINE([OPENVRML_JSVAL_TO_DOUBLE_RETURNS_POINTER], [1],
                 [SpiderMonkey's JSVAL_TO_DOUBLE returns a pointer])])

#
# The JSPropertyOp function signature changed from XULRunner 1.9.2 to
# 1.9.3: the third argument changed from jsval to jsid.
#
AC_CACHE_CHECK([whether JSPropertyOp uses jsid],
[ov_cv_jspropertyop_uses_jsid],
[ov_cv_jspropertyop_uses_jsid=no
#
# Test using C++ since C probably doesn't care so much about the type.
#
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[
JSBool (*fn)(JSContext *, JSObject *, jsval, jsval*);
JSPropertyOp op = fn;
]])], ,
[ov_cv_jspropertyop_uses_jsid=yes])
AC_LANG_POP([C++])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_jspropertyop_uses_jsid = Xyes],
      [jspropertyop_uses_jsid=1],
      [jspropertyop_uses_jsid=0])
AC_DEFINE_UNQUOTED([OPENVRML_JSPROPERTYOP_USES_JSID],
                   [$jspropertyop_uses_jsid],
                   [Whether JSPropertyOp function signatures use jsid])

#
# XULRunner 1.9.3 replaced JS_{Add,Remove}Root with type-specific
# functions.
#
AC_CACHE_CHECK([whether SpiderMonkey uses type-specific root functions],
[ov_cv_js_uses_typed_root_functions],
[ov_cv_js_uses_typed_root_functions=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_PUSH([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[JS_AddNamedRoot(0, 0);]])],
[ov_cv_js_uses_typed_root_functions=yes])
AC_LANG_POP([C])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_js_uses_typed_root_functions = Xyes],
      [AC_DEFINE([OPENVRML_JS_HAS_TYPED_ROOT_FUNCTIONS], [1],
                 [Defined if SpiderMonkey uses type-specific add/remove root functions])])

#
# XULRunner 2.0 changes the type of JSNative to match that of JSFastNative.
#
AC_CACHE_CHECK([whether JSNative has the JSFastNative signature],
[ov_cv_fast_jsnative],
[ov_cv_fast_jsnative=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[JSBool (*f)(JSContext *, unsigned, jsval *); JSNative g = f;]])],
[ov_cv_fast_jsnative=yes])
AC_LANG_POP([C++])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_fast_jsnative = Xyes],
      [AC_DEFINE([OPENVRML_FAST_JSNATIVE], [1],
                 [Defined if JSNative is the same as JSFastNative])])

#
# XULRunner 2.0 changes the mutator type to JSStrictPropertyOp.
#
AC_CACHE_CHECK([whether SpiderMonkey has JSStrictPropertyOp],
[ov_cv_js_has_strictpropertyop],
[ov_cv_js_has_strictpropertyop=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_PUSH([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[JSStrictPropertyOp op;]])],
[ov_cv_js_has_strictpropertyop=yes])
AC_LANG_POP([C])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_js_has_strictpropertyop = Xyes],
      [AC_DEFINE([OPENVRML_JS_HAS_STRICTPROPERTYOP], [1],
                 [Defined if SpiderMonkey has JSStrictPropertyOp])])

#
# XULRunner 12.0 removes the JSObject* prototype argument from
# JS_ConstructObject.
#
AC_CACHE_CHECK([whether JS_ConstructObject is missing the prototype argument],
[ov_cv_js_constructobject_without_proto],
[ov_cv_js_constructobject_without_proto=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_ASSERT([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[JS_ConstructObject(0, 0, 0)]])],
[ov_cv_js_constructobject_without_proto=yes])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_js_constructobject_without_proto = Xyes],
      [AC_DEFINE([OPENVRML_JS_CONSTRUCTOBJECT_WITHOUT_PROTO], [1],
                 [Defined if JS_ConstructObject is missing the prototype argument])])

#
# XULRunner 14.0 removes JS_FinalizeStub.
#
AC_CACHE_CHECK([whether SpiderMonkey has JS_FinalizeStub],
[ov_cv_js_has_finalizestub],
[ov_cv_js_has_finalizestub=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
ov_save_LDFLAGS="$LDFLAGS"
LDFLAGS="$JS_LIBS $LDFLAGS"
AC_LANG_ASSERT([C])
AC_LINK_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[JS_FinalizeStub(0, 0)]])],
[ov_cv_js_has_finalizestub=yes])
CPPFLAGS=$ov_save_CPPFLAGS
LDFLAGS=$ov_save_LDFLAGS
])
AS_IF([test X$ov_cv_js_has_finalizestub = Xyes],
      [AC_DEFINE([OPENVRML_JS_HAS_FINALIZESTUB], [1],
                 [Defined if JS_FinalizeStub is declared in jsapi.h])])

#
# XULRunner 14.0 removes the JSContext argument from JS_{Set,Get}Private.
#
AC_CACHE_CHECK([whether SpiderMonkey JS_SetPrivate is used without a JSContext],
[ov_cv_js_private_without_context],
[ov_cv_js_private_without_context=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_ASSERT([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[JS_SetPrivate(0, 0)]])],
[ov_cv_js_private_without_context=yes])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_js_private_without_context = Xyes],
      [AC_DEFINE([OPENVRML_JS_PRIVATE_WITHOUT_CONTEXT], [1],
                 [Defined if JS_SetContext does not take a JSContext argument])])

#
# XULRunner 14.0 replaces the JSContext* argument to JSFinalizeOp with
# JSFreeOp*.
#
AC_CACHE_CHECK([whether SpiderMonkey JSFinalizeOp uses JSFreeOp],
[ov_cv_jsfinalizeop_uses_jsfreeop],
[ov_cv_jsfinalizeop_uses_jsfreeop=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_ASSERT([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[JSFreeOp * fop = 0; JSFinalizeOp finalize = 0; finalize(fop, 0)]])],
[ov_cv_jsfinalizeop_uses_jsfreeop=yes])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_jsfinalizeop_uses_jsfreeop = Xyes],
      [AC_DEFINE([OPENVRML_JS_FINALIZEOP_USES_FREEOP], [1],
                 [Defined if JSFinalizeOp takes a JSFreeOp argument])])

#
# XULRunner 14.0 removes the JSContext argument from JS_GetParent.
#
AC_CACHE_CHECK([whether SpiderMonkey JS_GetParent is used without a JSContext],
[ov_cv_js_getparent_without_context],
[ov_cv_js_getparent_without_context=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
AC_LANG_ASSERT([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[JS_GetParent(0)]])],
[ov_cv_js_getparent_without_context=yes])
CPPFLAGS=$ov_save_CPPFLAGS
])
AS_IF([test X$ov_cv_js_getparent_without_context = Xyes],
      [AC_DEFINE([OPENVRML_JS_GETPARENT_WITHOUT_CONTEXT], [1],
                 [Defined if JS_GetParent does not take a JSContext argument])])

#
# XULRunner 15.0 removes JSVAL_IS_OBJECT.
#
AC_CACHE_CHECK([whether SpiderMonkey has JSVAL_IS_OBJECT],
[ov_cv_js_has_jsval_is_object],
[ov_cv_js_has_jsval_is_object=no
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$JS_CFLAGS $CPPFLAGS"
ov_save_LDFLAGS="$LDFLAGS"
LDFLAGS="$JS_LIBS $LDFLAGS"
AC_LANG_ASSERT([C])
AC_LINK_IFELSE([AC_LANG_PROGRAM(
[[#include <jsapi.h>]],
[[jsval val; JSVAL_IS_OBJECT(val)]])],
[ov_cv_js_has_jsval_is_object=yes])
CPPFLAGS=$ov_save_CPPFLAGS
LDFLAGS=$ov_save_LDFLAGS
])
AS_IF([test X$ov_cv_js_has_jsval_is_object = Xyes],
      [AC_DEFINE([OPENVRML_JS_HAS_JSVAL_IS_OBJECT], [1],
                 [Defined if SpiderMonkey has JSVAL_IS_OBJECT])])

#
# openvrml-xembed and openvrml-player both use GOption, which was
# introduced in GLib 2.6.
#
PKG_CHECK_MODULES([DBUS_G], [dbus-glib-1 glib-2.0 >= 2.6], ,
                  [have_dbus_glib=no])
AC_ARG_VAR([DBUS_BINDING_TOOL], [Generate stub code for dbus-glib])
AC_PATH_PROG([DBUS_BINDING_TOOL], [dbus-binding-tool])

PKG_CHECK_MODULES([GTKGL],
                  [gtkglext-1.0 gthread-2.0], ,
                  [have_gtkgl=no])
#
# openvrml-player uses GNOME_PARAM_GOPTION_CONTEXT, which was
# introduced in libgnome 2.14.
#
# openvrml-player uses GtkBuilder, which was introduced in GTK+ 2.12.
#
PKG_CHECK_MODULES([GNOMEUI],
                  [libgnomeui-2.0 libgnome-2.0 >= 2.14 gtk+-2.0 >= 2.12], ,
                  [have_gnomeui=no])
PKG_CHECK_MODULES([GIO], [gio-2.0], , [have_gio=no])
PKG_CHECK_MODULES([CURL], [libcurl], , [have_libcurl=no])
PKG_CHECK_MODULES([MOZILLA_PLUGIN], [mozilla-plugin >= 2],
                  [have_mozilla_plugin=yes],
                  [have_mozilla_plugin=no])
AC_LANG_PUSH([C++])
ov_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$MOZILLA_PLUGIN_CFLAGS $CPPFLAGS"
AC_CACHE_CHECK([if NP_GetMIMEDescription returns const char *],
               [ov_cv_np_getmimedescription_const_char],
[ov_cv_np_getmimedescription_const_char=yes
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <npfunctions.h>
char * NP_GetMIMEDescription() { return 0; }]],
                                   [])],
                  [ov_cv_np_getmimedescription_const_char=no])])
CPPFLAGS=$ov_save_CPPFLAGS
AC_LANG_POP([C++])
ov_np_getmimedescription_const=
AS_IF([test X$ov_cv_np_getmimedescription_const_char = Xyes],
      [ov_np_getmimedescription_const=const])
AC_DEFINE_UNQUOTED([OPENVRML_NP_GETMIMEDESCRIPTION_CONST],
                   [$ov_np_getmimedescription_const],
                   [const if NP_GetMIMEDescription returns const char *.])

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

PKG_CHECK_MODULES([SDL], [sdl], [have_sdl=yes], [have_sdl=no])

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
AS_IF([test X$enable_png_textures != Xno],
      [AS_IF([test X$have_libpng = Xno -a -z "${PNG_LIBS+x}"],
             [AC_MSG_FAILURE([libpng is required for PNG texture support])])
       AC_DEFINE([OPENVRML_ENABLE_PNG_TEXTURES], [1],
                 [Defined if support for rendering PNG textures is enabled.])])

#
# JPEG texture support
#
AC_ARG_ENABLE([jpeg-textures],
              [AC_HELP_STRING([--disable-jpeg-textures],
                              [disable support for rendering JPEG textures])])
AS_IF([test X$enable_jpeg_textures = Xno],
      [JPEG_LIBS=""],
      [JPEG_LIBS="-ljpeg"
       AC_DEFINE([OPENVRML_ENABLE_JPEG_TEXTURES], [1],
                 [Defined if support for rendering JPEG textures is enabled.])])
AC_SUBST([JPEG_LIBS])

#
# Text node support
#
AC_ARG_ENABLE([render-text-node],
              [AC_HELP_STRING([--disable-render-text-node],
                              [disable support for rendering Text nodes])])
AS_IF([test X$enable_render_text_node != Xno],
      [AS_IF([test X$have_fontconfig = Xno -a -z "${FONTCONFIG_LIBS+x}"],
             [AC_MSG_FAILURE([fontconfig is required for rendering Text nodes])])
       AS_IF([test X$have_freetype = Xno -a -z "${FREETYPE_LIBS+x}"],
             [AC_MSG_FAILURE([FreeType is required for rendering Text nodes])])
       AC_DEFINE([OPENVRML_ENABLE_RENDER_TEXT_NODE], [1],
                 [Defined if support for rendering Text nodes is enabled.])])

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
AS_IF([test X$enable_script_node_javascript = Xno],
      [JS_CFLAGS="" JS_LIBS=""],
      [AS_IF([test X$with_libjs = Xyes -a X$have_libjs = Xyes],
             [AS_IF([test -z "${JS_CFLAGS+x}"], [JS_CFLAGS="-DXP_UNIX -DJS_THREADSAFE"])
              AS_IF([test -z "${JS_LIBS+x}"], [JS_LIBS="-ljs"])])
       AS_IF([test \( \( X$have_libmozjs = Xno -o \( X$with_libjs = Xyes -a X$have_libjs = Xno \) \) -a -z "${JS_LIBS+x}" \) -o X$have_jsapi_h != Xyes],
             [AC_MSG_FAILURE([SpiderMonkey is required for Script node JavaScript support.  If you are trying to build --with-libjs, make sure that libjs has been built with thread safety enabled.])])])
AC_SUBST([JS_CFLAGS])
AC_SUBST([JS_LIBS])

AM_CONDITIONAL([ENABLE_SCRIPT_NODE_JAVASCRIPT],
  [test X$enable_script_node_javascript != Xno -a X$have_js != Xno])

#
# Script node Java support
#
AC_ARG_VAR([JAVA_HOME], [Path to Java installation])
AS_IF([test -z "${JRE_HOME+x}"],
      [AS_IF([test -n "${JAVA_HOME+x}"], [JRE_HOME=$JAVA_HOME])])
AC_ARG_VAR([JRE_HOME], [Path to Java Runtime Environment [default=JAVA_HOME/jre]])
AC_ARG_ENABLE([script-node-java],
              [AC_HELP_STRING([--disable-script-node-java],
                              [disable Script node Java support])])
case $target_cpu in
     x86_64)    OPENVRML_JVM_ARCH=amd64 ;;
     i?86)      OPENVRML_JVM_ARCH=i386 ;;
     powerpc64) OPENVRML_JVM_ARCH=ppc64 ;;
     powerpc)   OPENVRML_JVM_ARCH=ppc ;;
     *)         OPENVRML_JVM_ARCH=$target_cpu ;;
esac
AC_SUBST([OPENVRML_JVM_ARCH])
AS_IF([test X$enable_script_node_java != Xno],
[AS_IF([test X$have_jni != Xyes],
       [AC_MSG_FAILURE([The Java Native Interface is required for Script node Java support.])])
AC_DEFINE_UNQUOTED([OPENVRML_JNI_CONST], [$ov_cv_jni_const],
                   [const for JNI function signatures.])
AS_IF([test -z "${JRE_HOME+x}" -a -z "${JAVA_HOME+x}"],
[AC_MSG_WARN([Neither JRE_HOME nor JAVA_HOME has been set.  If neither of these is set at build time, at least one of them must be set at run time to locate libjvm.])],
[ov_save_LDFLAGS=$LDFLAGS
LDFLAGS="-L$JRE_HOME/lib/$OPENVRML_JVM_ARCH/client -L$JRE_HOME/lib/$OPENVRML_JVM_ARCH/server $LDFLAGS"
AC_CHECK_LIB([jvm], [JNI_CreateJavaVM], [LIBS=$LIBS],
             [AC_MSG_WARN([no libjvm found at JRE_HOME])])
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
AS_IF([test X$enable_gl_renderer != Xno],
      [AS_IF([test X$no_gl = Xyes],
             [AC_MSG_FAILURE([OpenGL/Mesa (libGL) is required for the GL renderer])])
       AS_IF([test X$no_glu = Xyes],
             [AC_MSG_FAILURE([the OpenGL Utility library (libGLU) is required for the GL renderer])])
       AC_CONFIG_FILES([openvrml-gl.pc])])
AM_CONDITIONAL([ENABLE_GL_RENDERER],
               [test X$enable_gl_renderer != Xno -a X$no_gl != Xyes])

#
# build the XEmbed control
#
AC_ARG_ENABLE([xembed],
              [AC_HELP_STRING([--disable-xembed],
                              [do not build the XEmbed control])])
AS_IF([test X$enable_xembed != Xno],
      [AS_IF([test X$have_dbus_glib = Xno -a -z "${DBUS_G_LIBS+x}"],
             [AC_MSG_FAILURE([D-Bus GLib bindings are required to build the XEmbed control])])
       AS_IF([test X$have_gtkgl = Xno -a -z "${GTKGL_LIBS+x}"],
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
       AS_IF([test X$have_gio = Xno -a -z "${GIO_LIBS+x}"],
             [AC_MSG_FAILURE([GIO is required to build OpenVRML Player])])
       AS_IF([test X$have_gnomeui = Xno -a -z "${GNOMEUI_LIBS+x}"],
             [AC_MSG_FAILURE([libgnomeui is required to build OpenVRML Player])])
       AS_IF([test X$have_libcurl = Xno -a -z "${CURL_LIBS+x}"],
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
       AS_IF([test X$have_mozilla_plugin = Xno -a -z "${MOZILLA_PLUGIN_CFLAGS+x}"],
             [AC_MSG_FAILURE([Mozilla plug-in development headers are required to build the Mozilla plug-in.])])
       AS_IF([test X$have_dbus_glib = Xno -a -z "${DBUS_G_LIBS+x}"],
             [AC_MSG_FAILURE([The D-Bus GLib binding is required to build the Mozilla plug-in])])])
AM_CONDITIONAL([ENABLE_MOZILLA_PLUGIN],
  [test "X$enable_mozilla_plugin" != "Xno" -a "X$have_dbus_glib" != "Xno"])

#
# build examples
#
AC_ARG_ENABLE([examples],
              [AC_HELP_STRING([--disable-examples],
                              [do not build the example programs])])
AS_IF([test X$enable_examples != Xno -a \( X$have_sdl != Xyes -o -z "${SDL_LIBS+x}" \)],
      [AC_MSG_FAILURE([SDL is required for the example programs])])
AM_CONDITIONAL([ENABLE_EXAMPLES],
               [test X$enable_examples != Xno -a X$have_sdl = Xyes])

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
[AS_IF([test -z "${GRE_PATH+x}"],
       [GRE_PATH=`echo $JS_LIBS | sed -e 's/^-L\([[^ ]]*\).*/\1/'`
        AS_IF([test "X$GRE_PATH" = "X$JS_LIBS"],
              [GRE_PATH=""
               AC_MSG_ERROR([GRE_PATH could not be derived from JS_LIBS.  Set GRE_PATH if an -rpath is needed to locate the Gecko runtime libraries at runtime.])])])
OPENVRML_JAVASCRIPT_RPATH="-Wl,-rpath -Wl,${GRE_PATH}"
])
AC_SUBST([OPENVRML_JAVASCRIPT_RPATH])

AC_CONFIG_TESTDIR([tests])

AC_OUTPUT
