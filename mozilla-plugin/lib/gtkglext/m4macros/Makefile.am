## -*- Makefile -*-
## Makefile.am for gtkglext/m4macros

EXTRA_DIST = \
	gtkglext.m4

API_VER = @GTKGLEXT_API_VERSION@

gtkglext-$(API_VER).m4: $(srcdir)/gtkglext.m4
	rm -f $@ && cp $< $@

m4datadir = $(datadir)/aclocal
noinst_DATA = gtkglext-$(API_VER).m4

DISTCLEANFILES = gtkglext-$(API_VER).m4

