LIBRARY_VERSION = 5:1:0
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

AM_CPPFLAGS = \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml-gl
AM_CXXFLAGS = @GL_CFLAGS@

if ENABLE_GL_RENDERER
lib_LTLIBRARIES = libopenvrml-gl.la
openvrml_gl_includedir = $(pkgincludedir)/openvrml/gl
openvrml_gl_include_HEADERS = viewer.h

libopenvrml_gl_la_SOURCES = viewer.cpp
libopenvrml_gl_la_LDFLAGS = -version-info $(LIBRARY_VERSION) @GL_LIBS@
endif

EXTRA_DIST = viewer.cpp viewer.h
