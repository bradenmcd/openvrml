#
# Add any debug flags
#
if DEBUG
  DEBUG_CXXFLAGS = -g
else
  DEBUG_CXXFLAGS = -DNDEBUG
endif

INCLUDES = -I$(top_srcdir)/libvrml97core/src
CXXFLAGS = -O2 -Wall $(DEBUG_CXXFLAGS) @GLUT_CFLAGS@
LDFLAGS = -version-info $(LIBVRML97_SO_VERSION) @GLUT_LIBS@ -L$../../../libvrml97core/src/vrml97 -lvrml97

lib_LTLIBRARIES = libvrml97gl.la

vrml97glincludedir = $(includedir)/vrml97gl
vrml97glinclude_HEADERS = ViewerOpenGL.h

noinst_HEADERS = OpenGLEvent.h

libvrml97gl_la_SOURCES = ViewerOpenGL.cpp OpenGLEvent.cpp
