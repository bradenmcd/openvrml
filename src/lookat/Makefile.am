#
# Add any debug flags
#
if DEBUG
else
  DEBUG_CPPFLAGS = -DNDEBUG
endif

INCLUDES = -I$(top_srcdir)/src/openvrml -I$(top_srcdir)/src/openvrml-gl
AM_CPPFLAGS = $(DEBUG_CPPFLAGS)
AM_CXXFLAGS = @GLUT_CFLAGS@ @JAVA_CFLAGS@

bin_PROGRAMS = lookat
lookat_SOURCES = lookat.cpp ViewerGlut.cpp
lookat_LDFLAGS = @GLUT_LIBS@ @JAVA_LIBS@
lookat_LDADD = $(top_builddir)/src/openvrml-gl/OpenVRML/GL/libopenvrml-gl.la \
$(top_builddir)/src/openvrml/OpenVRML/libopenvrml.la

noinst_HEADERS = ViewerGlut.h glutint.h
