LIBRARY_VERSION = 6:3:0
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
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml-gl
AM_CXXFLAGS = @GLU_CFLAGS@

if ENABLE_GL_RENDERER
lib_LTLIBRARIES = libopenvrml-gl.la
pkginclude_HEADERS = \
        openvrml-gl-config.h \
        openvrml-gl-common.h
openvrml_gl_includedir = $(pkgincludedir)/openvrml/gl
openvrml_gl_include_HEADERS =  openvrml/gl/viewer.h

libopenvrml_gl_la_SOURCES = openvrml/gl/viewer.cpp
libopenvrml_gl_la_LDFLAGS = -version-info $(LIBRARY_VERSION) @GLU_LIBS@
endif

EXTRA_DIST = \
        openvrml-gl-config.h.in \
        openvrml-gl-common.h \
        openvrml/gl/viewer.cpp \
        openvrml/gl/viewer.h
