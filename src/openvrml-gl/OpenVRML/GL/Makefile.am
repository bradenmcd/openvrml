LIBRARY_VERSION = 2:0:0
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

INCLUDES = -I$(top_srcdir)/src/openvrml
AM_CXXFLAGS = $(X_CFLAGS)

if WITH_GL
lib_LTLIBRARIES = libopenvrml-gl.la
libopenvrml_gl_la_SOURCES = ViewerOpenGL.cpp OpenGLEvent.cpp
libopenvrml_gl_la_LDFLAGS = -version-info $(LIBRARY_VERSION) $(GLUT_LIBS) \
$(GL_LIBS) $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)

OpenVRMLGLincludedir = $(includedir)/OpenVRML/GL
OpenVRMLGLinclude_HEADERS = common.h OpenGLEvent.h ViewerOpenGL.h
endif

noinst_HEADERS = private.h

EXTRA_DIST = ViewerOpenGL.cpp OpenGLEvent.cpp
