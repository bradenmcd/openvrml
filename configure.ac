AC_PREREQ([2.53])
AC_INIT([OpenVRML], [0.13.0], [openvrml-develop@lists.sourceforge.net])
AM_INIT_AUTOMAKE([$PACKAGE_TARNAME], [$PACKAGE_VERSION])

AM_CONFIG_HEADER([config.h])

#
# Checks for programs.
#
AC_ENABLE_STATIC(yes)
enable_static=yes
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP

#
# Optional packages.
#
OV_WITH_ZLIB
OV_WITH_LIBPNG
OV_WITH_LIBJPEG
AC_PATH_XTRA
AC_ARG_WITH(gl, [  --with-gl               use OpenGL/Mesa (required for GL renderer)], ,
            [# Use OpenGL by default.
             with_gl=yes])
AC_ARG_WITH(glut, [  --with-glut             use GLUT (required for LookAt and Text node support)], ,
            [# Use GLUT by default.
             with_glut=yes])
AC_ARG_WITH(spidermonkey, [  --with-spidermonkey     use SpiderMonkey (JavaScript support)], ,
            [# Use SpiderMonkey by default.
             with_spidermonkey=yes])
AC_ARG_WITH(jdk, [  --with-jdk              use the Java Development Kit (Java support)], ,
             [# Use the JDK by default.
              with_jdk=yes])
AC_ARG_WITH(ftflib, [  --with-ftflib           use ftflib (required for font support)], ,
            [# Use ftflib by default.
             with_ftflib=yes])
AC_ARG_WITH(doxygen, [  --with-doxygen          use Doxygen to generate documentation], ,
            [# Use Doxygen by default.
             with_doxygen=yes])

#
# Check for OpenGL/Mesa, GLUT.
#
if test "X$with_gl" = "Xyes"; then
  OV_CHECK_GL
  if test "X$no_gl" = "Xyes"; then
    AC_MSG_ERROR([OpenGL/Mesa not found.])
  else
    AC_DEFINE([OPENVRML_HAVE_GL], , [Defined if OpenGL/Mesa is a available and should be used.])
    
    if test "X$with_glut" = "Xyes"; then
      OV_CHECK_GLUT
      if test "X$no_glut" = "Xyes"; then
        AC_MSG_ERROR([OpenGL/Mesa not found.])
      else
        AC_DEFINE([OPENVRML_HAVE_GLUT], , [Defined if GLUT is a available and should be used.])
      fi
    else
      AC_MSG_WARN([Building without GLUT. Text node support and LookAt will not be built.])
    fi
  fi
else
  AC_MSG_WARN([Building without OpenGL/Mesa. The GL renderer and LookAt will not be built.])
fi
AM_CONDITIONAL([WITH_GL], [test "X$with_gl" = "Xyes"])
AM_CONDITIONAL([WITH_GLUT], [test "X$with_gl" = "Xyes" -a "X$with_glut" = "Xyes"])
dnl AM_CONDITIONAL([WITH_GLUT], [test "apple" = "orange"])

#
# Check for SpiderMonkey
#
if test "X$with_spidermonkey" = "Xyes"; then
  OV_CHECK_SPIDERMONKEY
  if test "X$no_js" = "Xyes"; then
    AC_MSG_ERROR([SpiderMonkey not found.])
  else
    AC_DEFINE(OPENVRML_HAVE_SPIDERMONKEY, , [defined if SpiderMonkey is available and should be used])
  fi
fi

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
    
#
# Check for Doxygen.
#
if test "X$with_doxygen" = "Xyes"; then
  AC_PATH_PROG([DOXYGEN], [doxygen])
  if test -z "${DOXYGEN}"; then
    AC_MSG_ERROR([Doxygen not found.])
  fi
fi
AM_CONDITIONAL([WITH_DOXYGEN], [test "X$with_doxygen" = "Xyes"])

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
