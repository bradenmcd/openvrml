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
LDADD = ../../libvrml97gl/src/vrml97gl/libvrml97gl.la \
../../libvrml97core/src/vrml97/libvrml97.la

LDFLAGS = @GLUT_LIBS@

bin_PROGRAMS = lookat
noinst_HEADERS = ViewerGlut.h glutint.h

lookat_SOURCES = lookat.cpp ViewerGlut.cpp
