AM_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_builddir)/src/libopenvrml-gl \
        -I$(top_srcdir)/src/libopenvrml-gl
AM_CXXFLAGS = @SDL_CFLAGS@
AM_LDFLAGS = @OPENVRML_RPATH@ @SDL_LIBS@ @JNI_LIBS@
LDADD = $(top_builddir)/src/libopenvrml-gl/libopenvrml-gl.la \
        $(top_builddir)/src/libopenvrml/libopenvrml.la

if ENABLE_EXAMPLES
noinst_PROGRAMS = sdl-viewer

if WITH_REZ
if ENABLE_SHARED
all-local:
	$(REZ) -d __DARWIN__ Carbon.r -t APPL -o .libs/sdl-viewer
else
all-local:
	$(REZ) -d __DARWIN__ Carbon.r -t APPL -o sdl-viewer
endif # ENABLE_SHARED
endif # WITH_REZ

endif # ENABLE_EXAMPLES

sdl_viewer_SOURCES = sdl_viewer.cpp

EXTRA_DIST = $(sdl_viewer_SOURCES)
