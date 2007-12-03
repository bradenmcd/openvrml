## -*- Makefile -*-
## Makefile.am for gtkglext

ACLOCAL_AMFLAGS = -I m4macros

SUBDIRS = gdk gtk m4macros docs
DIST_SUBDIRS = $(SUBDIRS) examples

EXTRA_DIST = \
	README.win32			\
	ChangeLog.pre-1-0		\
	gdkglext.pc.in			\
	gtkglext.pc.in			\
	gdkglext-uninstalled.pc.in	\
	gtkglext-uninstalled.pc.in	\
	gtkglext.spec.in		\
	gtkglext.nsi.in			\
	gtkglext.nsi			\
	setup-gtkglext.sh

## Copy .pc files to target-specific names
GDK_TARGET = @gdktarget@
API_VER = @GTKGLEXT_API_VERSION@

gdkglext-$(GDK_TARGET)-$(API_VER).pc: gdkglext.pc gdkglext-$(GDK_TARGET)-$(API_VER)-uninstalled.pc
	rm -f $@ && cp gdkglext.pc $@

gdkglext-$(API_VER).pc: gdkglext.pc gdkglext-$(API_VER)-uninstalled.pc
	rm -f $@ && cp gdkglext.pc $@

gtkglext-$(GDK_TARGET)-$(API_VER).pc: gtkglext.pc gtkglext-$(GDK_TARGET)-$(API_VER)-uninstalled.pc
	rm -f $@ && cp gtkglext.pc $@

gtkglext-$(API_VER).pc: gtkglext.pc gtkglext-$(API_VER)-uninstalled.pc
	rm -f $@ && cp gtkglext.pc $@

gdkglext-$(GDK_TARGET)-$(API_VER)-uninstalled.pc: gdkglext-uninstalled.pc
	rm -f $@ && cp gdkglext-uninstalled.pc $@

gdkglext-$(API_VER)-uninstalled.pc: gdkglext-uninstalled.pc gdkglext-$(GDK_TARGET)-$(API_VER)-uninstalled.pc
	rm -f $@ && cp gdkglext-uninstalled.pc $@

gtkglext-$(GDK_TARGET)-$(API_VER)-uninstalled.pc: gtkglext-uninstalled.pc
	rm -f $@ && cp gtkglext-uninstalled.pc $@

gtkglext-$(API_VER)-uninstalled.pc: gtkglext-uninstalled.pc gtkglext-$(GDK_TARGET)-$(API_VER)-uninstalled.pc
	rm -f $@ && cp gtkglext-uninstalled.pc $@

pkgconfigdir = $(libdir)/pkgconfig
noinst_DATA = \
	gdkglext-$(GDK_TARGET)-$(API_VER).pc	\
	gdkglext-$(API_VER).pc			\
	gtkglext-$(GDK_TARGET)-$(API_VER).pc	\
	gtkglext-$(API_VER).pc
$(noinst_DATA): $(top_builddir)/config.status

DISTCLEANFILES = \
	gdkglext-$(GDK_TARGET)-$(API_VER).pc			\
	gdkglext-$(API_VER).pc					\
	gtkglext-$(GDK_TARGET)-$(API_VER).pc			\
	gtkglext-$(API_VER).pc					\
	gdkglext-$(GDK_TARGET)-$(API_VER)-uninstalled.pc	\
	gdkglext-$(API_VER)-uninstalled.pc			\
	gtkglext-$(GDK_TARGET)-$(API_VER)-uninstalled.pc	\
	gtkglext-$(API_VER)-uninstalled.pc

.PHONY: examples clean-examples

examples: all
	( cd $(top_builddir)/examples && $(MAKE) $(MFLAGS) )

clean-examples:
	( cd $(top_builddir)/examples && $(MAKE) $(MFLAGS) clean )

.PHONY: changelog
changelog:
	cvs2cl -b -t --hide-filenames

