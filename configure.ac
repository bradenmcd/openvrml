AC_PREREQ(2.52)
AC_INIT(OpenVRML, 0.12.0, openvrml-develop@lists.sourceforge.net)
AM_INIT_AUTOMAKE($PACKAGE_TARNAME, $PACKAGE_VERSION)

AM_CONFIG_HEADER(config.h)

OPENVRML_MAJOR_VERSION="0"
OPENVRML_MINOR_VERSION="12"
OPENVRML_MICRO_VERSION="0"

AC_DEFINE_UNQUOTED([OPENVRML_MAJOR_VERSION], ${OPENVRML_MAJOR_VERSION},
                   [Major version number.])
AC_DEFINE_UNQUOTED([OPENVRML_MINOR_VERSION], ${OPENVRML_MINOR_VERSION},
                   [Minor version number.])
AC_DEFINE_UNQUOTED([OPENVRML_MICRO_VERSION], ${OPENVRML_MICRO_VERSION},
                   [Micro version number.])

#
# Build libantlr as a convenience library.
#
NOINSTALL_LIBANTLR=libantlr.la
AC_SUBST(INSTALL_LIBANTLR)
AC_SUBST(NOINSTALL_LIBANTLR)
AC_SUBST(LIBANTLR_RPATH_FLAG)
AC_SUBST(ANTLR_INCLUDEDIR)
AM_CONDITIONAL(ANTLR_INSTALL_HFILES, test "apple" = "orange")

#
# Checks for Win32 stuff.
#
AC_CYGWIN

#
# Checks for programs.
#
AC_ENABLE_STATIC(yes)
enable_static=yes
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP

OV_CXX_HASH_MAP

#
# Checks for required packages.
#
OV_CHECK_ZLIB
if test "X$no_zlib" = "Xyes"; then
  AC_MSG_ERROR([zlib not found.])
fi

OV_CHECK_GL
if test "X$no_gl" = "Xyes"; then
  AC_MSG_ERROR([OpenGL/Mesa not found.])
fi

AC_PATH_XTRA
if test "X$no_x" = "Xyes"; then
  AC_MSG_ERROR([X not found.])
fi

OV_CHECK_GLUT
if test "X$no_glut" = "Xyes"; then
  AC_MSG_ERROR([GLUT not found.])
fi

#
# Optional packages.
#
AC_ARG_WITH(libjpeg, [  --with-libjpeg          use libjpeg (JPEG image support)], ,
             [# Use libjpeg by default.
              with_libjpeg=yes])
if test "X$with_libjpeg" = "Xyes"; then
  OV_CHECK_LIBJPEG
  if test "X$no_libjpeg" = "Xyes"; then
    AC_MSG_ERROR([libjpeg not found.])
  else
    AC_DEFINE(OPENVRML_HAVE_LIBJPEG, , [Defined if libjpeg is available and should be used.])
  fi
fi

AC_ARG_WITH(libpng, [  --with-libpng           use libpng (PNG image support)], ,
            [# Use libpng by default.
             with_libpng=yes])
if test "X$with_libpng" = "Xyes"; then
  OV_CHECK_LIBPNG
  if test "X$no_libpng" = "Xyes"; then
    AC_MSG_ERROR([libpng not found.])
  else
    AC_DEFINE(OPENVRML_HAVE_LIBPNG, , [Defined if libpng is available and should be used.])
  fi
fi

AC_ARG_WITH(spidermonkey, [  --with-spidermonkey     use SpiderMonkey (JavaScript support)], ,
            [# Use SpiderMonkey by default.
             with_spidermonkey=yes])
if test "X$with_spidermonkey" = "Xyes"; then
  OV_CHECK_SPIDERMONKEY
  if test "X$no_js" = "Xyes"; then
    AC_MSG_ERROR([SpiderMonkey not found.])
  else
    AC_DEFINE(OPENVRML_HAVE_SPIDERMONKEY, , [defined if SpiderMonkey is available and should be used])
  fi
fi

AC_ARG_WITH(jdk, [  --with-jdk              use the Java Development Kit (Java support)], ,
             [# Use the JDK by default.
              with_jdk=yes])
if test "X$with_jdk" = "Xyes"; then
  AC_PATH_PROG(JAVAC, javac)
  if test -z "${JAVAC}"; then
    AC_MSG_ERROR([Java compiler not found.])
  fi
  AC_SUBST(JAVA_SUBDIR, java)
  AC_PATH_PROG(JAR, jar)
  if test -z "${JAR}"; then
    AC_MSG_ERROR([Java archive utility not found.])
  fi
  OV_CHECK_JNI
  if test "X$no_jni" = "Xyes"; then
    AC_MSG_ERROR([Java Native Interface not found.])
  else
    AC_DEFINE(OPENVRML_HAVE_JNI, , [defined if JNI is available and should be used])
  fi
fi

#
# Modulate debug mode
#
AC_ARG_ENABLE(debug, [  --disable-debug         Turn off debugging],
              [case "${enableval}" in
                 yes) debug=true ;;
                 no)  debug=false ;;
                 *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
               esac], [debug=true])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

AC_OUTPUT([Makefile \
doc/Makefile \
macros/Makefile \
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
Doxyfile \
openvrml.pc \
openvrml-gl.pc \
openvrml.spec])
