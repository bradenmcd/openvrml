AUTOMAKE_OPTIONS = 1.6.1c foreign

SUBDIRS = doc m4 models lib java src

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = openvrml.pc.in openvrml-gl.pc.in openvrml.spec.in lookat.xpm

if WITH_GL
OPENVRML_GL_PC = openvrml-gl.pc
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = openvrml.pc $(OPENVRML_GL_PC)

pixmapdatadir = $(datadir)/pixmaps
pixmapdata_DATA = lookat.xpm

dist-hook: openvrml.spec
	cp openvrml.spec $(distdir)
