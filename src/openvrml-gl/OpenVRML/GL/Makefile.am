LIBRARY_VERSION = 4:0:0
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

AM_CPPFLAGS = -I$(top_srcdir)/src/openvrml -I$(top_srcdir)/src/openvrml-gl
AM_CXXFLAGS = @GL_CFLAGS@

if ENABLE_GL_RENDERER
lib_LTLIBRARIES = libopenvrml-gl.la
libopenvrml_gl_la_SOURCES = viewer.cpp
libopenvrml_gl_la_LDFLAGS = -version-info $(LIBRARY_VERSION) @GL_LIBS@

OpenVRMLGLincludedir = $(includedir)/OpenVRML/GL
OpenVRMLGLinclude_HEADERS = common.h viewer.h
endif

EXTRA_DIST = viewer.cpp Makefile.vc6
