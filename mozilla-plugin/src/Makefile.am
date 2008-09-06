AM_CPPFLAGS = \
        -DGTK_DISABLE_DEPRECATED \
        -DOPENVRML_LIBEXECDIR_=\"$(libexecdir)\"
AM_CXXFLAGS = $(MOZILLA_PLUGIN_CFLAGS) $(DBUS_G_CFLAGS)

mozpluginsdir = $(libdir)/mozilla/plugins

if ENABLE_MOZILLA_PLUGIN
mozplugins_LTLIBRARIES = openvrml.la
BUILT_SOURCES = browser-host-server-glue.h
endif

openvrml_la_SOURCES = openvrml.cpp
openvrml_la_LDFLAGS = \
        -module -avoid-version \
        $(MOZILLA_PLUGIN_LIBS) \
        $(DBUS_G_LIBS)

browser-host-server-glue.h: $(top_srcdir)/../src/openvrml-xembed/browser-host.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL) \
                   --prefix=openvrml_np_browser_host   \
                   --mode=glib-server                  \
                   --output=$@                         \
                   $?

EXTRA_DIST = $(openvrml_la_SOURCES)

CLEANFILES = $(BUILT_SOURCES)
