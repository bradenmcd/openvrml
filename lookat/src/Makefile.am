#
# Add any debug flags
#
if DEBUG
else
  DEBUG_CPPFLAGS = -DNDEBUG
endif

INCLUDES = -I$(top_srcdir)/libvrml97core/src -I$(top_srcdir)/libvrml97gl/src
AM_CPPFLAGS = $(DEBUG_CPPFLAGS)
AM_CXXFLAGS = -Wall @GLUT_CFLAGS@

bin_PROGRAMS = lookat
lookat_SOURCES = lookat.cpp ViewerGlut.cpp
lookat_LDFLAGS = @GLUT_LIBS@
lookat_LDADD = ../../libvrml97gl/src/vrml97gl/libopenvrml-gl.la \
../../libvrml97core/src/vrml97/libopenvrml.la

noinst_HEADERS = ViewerGlut.h glutint.h
