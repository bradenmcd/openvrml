AUTOMAKE_OPTIONS = foreign
SUBDIRS = doc macros models lib src

ACLOCAL_AMFLAGS = -I macros

VISUALC_PROJECT_FILES = \
        win32/Install \
        win32/Makefile.win \
        win32/libvrml97.dsw \
        win32/winconfig.h

EXTRA_DIST = vrml.m4 vrml-gl.m4 Doxyfile.in openvrml.spec.in lookat.xpm \
$(VISUALC_PROJECT_FILES)

m4datadir = $(datadir)/aclocal
m4data_DATA = vrml.m4 vrml-gl.m4

pixmapdatadir = $(datadir)/pixmaps
pixmapdata_DATA = lookat.xpm

dist-hook: openvrml.spec
	cp openvrml.spec $(distdir)
