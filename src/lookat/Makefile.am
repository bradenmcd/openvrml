AM_CPPFLAGS = \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml-gl
AM_CXXFLAGS = @GLUT_CFLAGS@ @JNI_CFLAGS@
AM_LDFLAGS = -rpath @OPENVRML_RPATH@ @GLUT_LIBS@ @JNI_LIBS@
LDADD = $(top_builddir)/src/libopenvrml-gl/openvrml/gl/libopenvrml-gl.la \
        $(top_builddir)/src/libopenvrml/openvrml/libopenvrml.la

if ENABLE_LOOKAT
bin_PROGRAMS = lookat
lookat_SOURCES = lookat.cpp ViewerGlut.cpp
endif

noinst_HEADERS = ViewerGlut.h glutint.h

EXTRA_DIST = lookat.cpp ViewerGlut.cpp
