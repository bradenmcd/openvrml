AM_CPPFLAGS = \
        -I$(top_srcdir)/lib/gtkglext \
        -I$(top_builddir)/lib/gtkglext/gdk \
        -I$(top_srcdir)/../src/libopenvrml \
        -I$(top_srcdir)/../src/libopenvrml-gl \
        -I$(mozincludedir) \
        -DGTK_DISABLE_DEPRECATED
AM_CXXFLAGS = @MOZILLA_PLUGIN_CFLAGS@ @GTK_CFLAGS@

idldir = $(datadir)/idl/openvrml-@PACKAGE_VERSION@

mozpluginsdir = $(libdir)/mozilla/plugins

if ENABLE_MOZILLA_PLUGIN
mozplugins_LTLIBRARIES = openvrml.la
mozplugins_DATA = openvrml.xpt
endif
noinst_HEADERS = gtkvrml/vrmlbrowser.h
dist_idl_DATA = openvrml.idl

openvrml_la_SOURCES = openvrml.cpp gtkvrml/vrmlbrowser.cc
openvrml_la_LIBADD = \
        $(top_builddir)/../src/libopenvrml-gl/libopenvrml-gl.la \
        $(top_builddir)/../src/libopenvrml/libopenvrml.la \
        $(top_builddir)/lib/gtkglext/gtk/libgtkglext-x11-1.0.la \
        $(top_builddir)/lib/gtkglext/gdk/libgdkglext-x11-1.0.la
openvrml_la_LDFLAGS = \
        -module -avoid-version \
        -R@mozlibdir@ \
        @MOZILLA_PLUGIN_LIBS@ \
        @GTK_LIBS@ \
        @GL_LIBS@ \
        -lXmu -lXt @X_PRE_LIBS@ @X_LIBS@ @X_EXTRA_LIBS@

EXTRA_DIST = $(openvrml_la_SOURCES)

BUILT_SOURCES = openvrml.h

CLEANFILES = openvrml.h openvrml.xpt

.idl.h:
	$(XPIDL) $(XPIDLFLAGS) @MOZILLA_XPIDLFLAGS@ -m header $<

.idl.xpt:
	$(XPIDL) $(XPIDLFLAGS) @MOZILLA_XPIDLFLAGS@ -m typelib $<
