AM_CPPFLAGS = \
        -I$(top_srcdir)/../src/libopenvrml \
        -I$(top_srcdir)/../src/libopenvrml-gl \
        -I$(mozincludedir)
AM_CXXFLAGS = @MOZILLA_PLUGIN_CFLAGS@ @GTKGLEXT_CFLAGS@

idldir = $(datadir)/idl/@PACKAGE_NAME@-@PACKAGE_VERSION@

if ENABLE_MOZILLA_PLUGIN
mozplugins_LTLIBRARIES = openvrml.la
endif
noinst_HEADERS = gtkvrml/vrmlbrowser.h

openvrml_la_SOURCES = openvrml.cpp gtkvrml/vrmlbrowser.cc
openvrml_la_LIBADD = \
        $(top_builddir)/../src/libopenvrml/libopenvrml.la \
        $(top_builddir)/../src/libopenvrml-gl/libopenvrml-gl.la
openvrml_la_LDFLAGS = -module -avoid-version @GTKGLEXT_LIBS@

EXTRA_DIST = $(openvrml_la_SOURCES)
