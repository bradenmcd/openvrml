#
# Add any debug flags
#
if DEBUG
else
  DEBUG_CPPFLAGS = -DNDEBUG
endif

INCLUDES = -I$(top_srcdir)/libvrml97core/src
AM_CPPFLAGS = $(DEBUG_CPPFLAGS)
AM_CXXFLAGS = -Wall @GLUT_CFLAGS@
LDFLAGS = -version-info @VRML97GL_LIBRARY_VERSION@ @GLUT_LIBS@

lib_LTLIBRARIES = libvrml97gl.la

vrml97glincludedir = $(includedir)/vrml97gl
vrml97glinclude_HEADERS = ViewerOpenGL.h

noinst_HEADERS = OpenGLEvent.h

libvrml97gl_la_SOURCES = ViewerOpenGL.cpp OpenGLEvent.cpp
