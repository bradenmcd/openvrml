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

AC_PATH_XTRA
if test "X$no_x" = "Xyes"; then
  AC_MSG_ERROR([X not found.])
fi

#
# Optional packages.
#
AC_ARG_WITH(gl, [  --with-gl               use OpenGL/Mesa], ,
            [# Use OpenGL by default.
             with_gl=yes])
AC_ARG_WITH(glut, [  --with-glut             use GLUT], ,
            [# Use GLUT by default.
             with_glut=yes])
if test "X$with_gl" = "Xyes"; then
  OV_CHECK_GL
  if test "X$no_gl" = "Xyes"; then
    AC_MSG_ERROR([OpenGL/Mesa not found.])
  else
    AC_SUBST([OPENVRML_GL_SUBDIR], [openvrml-gl])
    AC_SUBST([OPENVRML_GL_PC], [openvrml-gl.pc])
    AC_DEFINE([OPENVRML_HAVE_GL], , [Defined if OpenGL/Mesa is a available and should be used.])
    
    if test "X$with_glut" = "Xyes"; then
      OV_CHECK_GLUT
      if test "X$no_glut" = "Xyes"; then
        AC_MSG_ERROR([OpenGL/Mesa not found.])
      else
        AC_SUBST([LOOKAT_SUBDIR], [lookat])
        AC_DEFINE([OPENVRML_HAVE_GLUT], , [Defined if GLUT is a available and should be used.])
      fi
    else
      AC_MSG_WARN([Building without GLUT. Text node support and LookAt will not be built.])
    fi
  fi
else
  AC_MSG_WARN([Building without OpenGL/Mesa. The GL renderer and LookAt will not be built.])
fi

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

AC_ARG_WITH(doxygen, [  --with-doxygen          use Doxygen to generate documentation], ,
            [# Use Doxygen by default.
             with_doxygen=yes])
if test "X$with_doxygen" = "Xyes"; then
  AC_PATH_PROG([DOXYGEN], [doxygen])
  if test -z "${DOXYGEN}"; then
    AC_MSG_ERROR([Doxygen not found.])
  fi
fi
AM_CONDITIONAL([WITH_DOXYGEN], [test "X$with_doxygen" = "Xyes"])

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
