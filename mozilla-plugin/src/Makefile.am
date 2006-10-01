AM_CPPFLAGS = \
        -I$(mozincludedir) \
        -DGTK_DISABLE_DEPRECATED \
        -DOPENVRML_LIBEXECDIR_=\"$(libexecdir)\"
AM_CXXFLAGS = @MOZILLA_PLUGIN_CFLAGS@ @GTK_CFLAGS@

idldir = $(datadir)/idl/openvrml-@PACKAGE_VERSION@

mozpluginsdir = $(libdir)/mozilla/plugins

if ENABLE_MOZILLA_PLUGIN
mozplugins_LTLIBRARIES = openvrml.la
mozplugins_DATA = openvrml.xpt
endif
dist_idl_DATA = openvrml.idl

openvrml_la_SOURCES = openvrml.cpp
openvrml_la_LDFLAGS = \
        -module -avoid-version \
        -R@mozlibdir@ \
        @MOZILLA_PLUGIN_LIBS@

EXTRA_DIST = $(openvrml_la_SOURCES)

BUILT_SOURCES = openvrml.h

CLEANFILES = openvrml.h openvrml.xpt

.idl.h:
	$(XPIDL) $(XPIDLFLAGS) @MOZILLA_XPIDLFLAGS@ -m header $<

.idl.xpt:
	$(XPIDL) $(XPIDLFLAGS) @MOZILLA_XPIDLFLAGS@ -m typelib $<
