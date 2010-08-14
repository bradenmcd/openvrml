SUBDIRS = doc models src data examples tests

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
        openvrml.pc.in \
        openvrml-gl.pc.in \
        OpenVRML.sln

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = openvrml.pc
if ENABLE_GL_RENDERER
pkgconfig_DATA += openvrml-gl.pc
endif
