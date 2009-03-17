SUBDIRS = doc ide-projects models src mozilla-plugin data examples tests

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
        openvrml.pc.in \
        openvrml-gl.pc.in

if ENABLE_GL_RENDERER
OPENVRML_GL_PC = openvrml-gl.pc
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = openvrml.pc $(OPENVRML_GL_PC)

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc
