AM_CPPFLAGS = \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml-gl
AM_CXXFLAGS = @GLUT_CFLAGS@
AM_LDFLAGS = -rpath @OPENVRML_RPATH@ @GLUT_LIBS@ @JNI_LIBS@
LDADD = $(top_builddir)/src/libopenvrml-gl/openvrml/gl/libopenvrml-gl.la \
        $(top_builddir)/src/libopenvrml/openvrml/libopenvrml.la

if ENABLE_LOOKAT
bin_PROGRAMS = lookat

if WITH_REZ
if ENABLE_SHARED
all-local:
	$(REZ) -d __DARWIN__ Carbon.r -t APPL -o .libs/lookat
else
all-local:
	$(REZ) -d __DARWIN__ Carbon.r -t APPL -o lookat
endif # ENABLE_SHARED
endif # WITH_REZ

endif # ENABLE_LOOKAT

noinst_HEADERS = ViewerGlut.h glutint.h

lookat_SOURCES = lookat.cpp ViewerGlut.cpp

EXTRA_DIST = $(lookat_SOURCES)
