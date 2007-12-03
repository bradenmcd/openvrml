AM_CPPFLAGS = \
        -I$(mozincludedir) \
        -DGTK_DISABLE_DEPRECATED \
        -DOPENVRML_LIBEXECDIR_=\"$(libexecdir)\"
AM_CXXFLAGS = @MOZILLA_PLUGIN_CFLAGS@ @GTK_CFLAGS@

mozpluginsdir = $(libdir)/mozilla/plugins

if ENABLE_MOZILLA_PLUGIN
mozplugins_LTLIBRARIES = openvrml.la
endif

openvrml_la_SOURCES = openvrml.cpp
openvrml_la_LDFLAGS = \
        -module -avoid-version \
        -R@mozlibdir@ \
        @MOZILLA_PLUGIN_LIBS@

EXTRA_DIST = $(openvrml_la_SOURCES)
