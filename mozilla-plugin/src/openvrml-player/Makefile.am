AM_CPPFLAGS = \
        -I$(top_srcdir)/lib/gtkglext \
        -I$(top_builddir)/lib/gtkglext/gdk \
        -I$(top_srcdir)/../src/libopenvrml \
        -I$(top_srcdir)/../src/libopenvrml-gl \
        -I$(mozincludedir) \
        -DGTK_DISABLE_DEPRECATED
AM_CXXFLAGS = @GTK_CFLAGS@ @GL_CFLAGS@

libexec_PROGRAMS = openvrml-player
openvrml_player_SOURCES = player.cpp
openvrml_player_LDADD = \
        $(top_builddir)/../src/libopenvrml-gl/libopenvrml-gl.la \
        $(top_builddir)/../src/libopenvrml/libopenvrml.la \
        $(top_builddir)/lib/gtkglext/gtk/libgtkglext-x11-1.0.la \
        $(top_builddir)/lib/gtkglext/gdk/libgdkglext-x11-1.0.la
openvrml_player_LDFLAGS = \
        -R@mozlibdir@ \
        @GTK_LIBS@ \
        @GL_LIBS@ \
        -lXmu -lXt @X_PRE_LIBS@ @X_LIBS@ @X_EXTRA_LIBS@
