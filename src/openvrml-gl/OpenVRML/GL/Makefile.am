LIBRARY_VERSION = 1:0:0
#                 | | |
#          +------+ | +---+
#          |        |     |
#       current:revision:age
#          |        |     |
#          |        |     +- Increment if interfaces have been added
#          |        |        Set to zero if interfaces have been removed or
#          |        |        changed
#          |        +- Increment if source code has changed
#          |           Set to zero if current is incremented
#          +- Increment if interfaces have been added, removed or changed

#
# Add any debug flags
#
if DEBUG
else
  DEBUG_CPPFLAGS = -DNDEBUG
endif

INCLUDES = -I$(top_srcdir)/src/openvrml
AM_CPPFLAGS = $(DEBUG_CPPFLAGS)
AM_CXXFLAGS = @GLUT_CFLAGS@

lib_LTLIBRARIES = libopenvrml-gl.la

libopenvrml_gl_la_SOURCES = ViewerOpenGL.cpp OpenGLEvent.cpp
libopenvrml_gl_la_LDFLAGS = -version-info $(LIBRARY_VERSION) @GLUT_LIBS@

OpenVRMLGLincludedir = $(includedir)/OpenVRML/GL
OpenVRMLGLinclude_HEADERS = common.h OpenGLEvent.h ViewerOpenGL.h

noinst_HEADERS = private.h
