if ENABLE_EXAMPLES
noinst_PROGRAMS = sdl-viewer pretty-print

if WITH_REZ
if ENABLE_SHARED
all-local: sdl-viewer
	$(REZ) -d __DARWIN__ Carbon.r -t APPL -o .libs/sdl-viewer
else
all-local: sdl-viewer
	$(REZ) -d __DARWIN__ Carbon.r -t APPL -o sdl-viewer
endif # ENABLE_SHARED
endif # WITH_REZ

endif # ENABLE_EXAMPLES

sdl_viewer_SOURCES = sdl_viewer.cpp
sdl_viewer_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_builddir)/src/libopenvrml-gl \
        -I$(top_srcdir)/src/libopenvrml-gl
sdl_viewer_CXXFLAGS = $(SDL_CFLAGS) $(FONTCONFIG_CFLAGS)
sdl_viewer_LDFLAGS = $(SDL_LIBS) $(GLU_LIBS) $(FONTCONFIG_LIBS)
sdl_viewer_LDADD = $(top_builddir)/src/libopenvrml-gl/libopenvrml-gl.la

pretty_print_SOURCES = pretty_print.cpp
pretty_print_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
pretty_print_LDADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

EXTRA_DIST = $(sdl_viewer_SOURCES) $(pretty_print_SOURCES) \
        pretty-print.vcxproj \
        sdl-viewer.vcxproj
