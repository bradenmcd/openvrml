INCLUDES = -I$(top_srcdir)/src/openvrml -I$(top_srcdir)/src/openvrml-gl
AM_CXXFLAGS = @GLUT_CFLAGS@ @JNI_CFLAGS@

if ENABLE_LOOKAT
bin_PROGRAMS = lookat
lookat_SOURCES = lookat.cpp ViewerGlut.cpp
lookat_LDFLAGS = @GLUT_LIBS@ @JNI_LIBS@
lookat_LDADD = \
        $(top_builddir)/src/openvrml-gl/OpenVRML/GL/libopenvrml-gl.la \
        $(top_builddir)/src/openvrml/OpenVRML/libopenvrml.la
endif

noinst_HEADERS = ViewerGlut.h glutint.h

EXTRA_DIST = lookat.cpp ViewerGlut.cpp
