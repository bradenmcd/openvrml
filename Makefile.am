AUTOMAKE_OPTIONS = foreign 
SUBDIRS = doc macros models libvrml97core libvrml97gl lookat

ACLOCAL_AMFLAGS = -I macros

VISUALC_PROJECT_FILES = \
        win32/Install \
        win32/Makefile.win \
        win32/libvrml97.dsw \
        win32/winconfig.h

EXTRA_DIST = vrml97.m4 vrml97gl.m4 openvrml.spec.in \
$(VISUALC_PROJECT_FILES)
m4datadir = $(datadir)/aclocal
m4data_DATA = vrml97.m4 vrml97gl.m4

dist-hook: openvrml.spec
	cp openvrml.spec $(distdir)
