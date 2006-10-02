AM_CPPFLAGS = \
        -I$(top_srcdir)/lib/gtkglext \
        -I$(top_builddir)/lib/gtkglext \
        -I$(top_builddir)/lib/gtkglext/gdk \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_builddir)/src/libopenvrml-gl \
        -I$(top_srcdir)/src/libopenvrml-gl \
        -I$(mozincludedir) \
        -DGTK_DISABLE_DEPRECATED
AM_CXXFLAGS = @PTHREAD_CFLAGS@ @GTK_CFLAGS@ @GL_CFLAGS@
AM_LDFLAGS = \
        @OPENVRML_RPATH@ \
        @JNI_LIBS@ \
        @GTK_LIBS@ \
        @GL_LIBS@ \
        -lXmu -lXt @X_PRE_LIBS@ @X_LIBS@ @X_EXTRA_LIBS@ \
        -lboost_thread \
        @PTHREAD_LIBS@
LDADD = $(top_builddir)/src/libopenvrml-gl/libopenvrml-gl.la \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        $(top_builddir)/lib/gtkglext/gtk/libgtkglext-x11-1.0.la \
        $(top_builddir)/lib/gtkglext/gdk/libgdkglext-x11-1.0.la

if ENABLE_GTKPLUG
libexec_PROGRAMS = openvrml-gtkplug
endif
noinst_HEADERS = \
        bounded_buffer.h \
        command_istream.h \
        plugin_streambuf.h \
        gtkvrmlbrowser.h \
        flag.h
openvrml_gtkplug_SOURCES = \
        main.cpp \
        command_istream.cpp \
        plugin_streambuf.cpp \
        gtkvrmlbrowser.cpp \
        flag.cpp

EXTRA_DIST = $(openvrml_gtkplug_SOURCES)
