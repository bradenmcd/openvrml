#
# Add any debug flags
#
if DEBUG
  DEBUG_CXXFLAGS = -g
else
  DEBUG_CXXFLAGS = -DNDEBUG
endif

INCLUDES = -I$(top_srcdir)/libvrml97core/src -I$(top_srcdir)/libvrml97gl/src
CXXFLAGS = -O2 -Wall $(DEBUG_CXXFLAGS) @GLUT_CFLAGS@
LDADD = ../../libvrml97gl/src/vrml97gl/libvrml97gl.la
LDFLAGS = @GLUT_LIBS@

bin_PROGRAMS = lookat
noinst_HEADERS = ViewerGlut.h glutint.h

lookat_SOURCES = lookat.cpp ViewerGlut.cpp
