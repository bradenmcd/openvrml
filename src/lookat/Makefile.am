AM_CPPFLAGS = \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml-gl
AM_CXXFLAGS = @GLUT_CFLAGS@ @JNI_CFLAGS@

if ENABLE_LOOKAT
bin_PROGRAMS = lookat
lookat_SOURCES = lookat.cpp ViewerGlut.cpp
lookat_LDFLAGS = @GLUT_LIBS@ @JNI_LIBS@
lookat_LDADD = \
        $(top_builddir)/src/libopenvrml-gl/openvrml/gl/libopenvrml-gl.la \
        $(top_builddir)/src/libopenvrml/openvrml/libopenvrml.la

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

EXTRA_DIST = lookat.cpp ViewerGlut.cpp
