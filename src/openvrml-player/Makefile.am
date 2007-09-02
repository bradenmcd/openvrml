AM_CPPFLAGS = \
        -DG_DISABLE_DEPRECATED \
        -DGTK_DISABLE_DEPRECATED \
        -DGNOME_DISABLE_DEPRECATED \
        -DOPENVRML_PLAYER_PKGDATADIR_=\"$(datadir)/openvrml-player\" \
        -DOPENVRML_LIBEXECDIR_=\"$(libexecdir)\" \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
AM_CXXFLAGS = $(GNOMEUI_CFLAGS) $(GLADE_CFLAGS) $(CURL_CFLAGS)
AM_LDFLAGS = -export-dynamic $(GNOMEUI_LIBS) $(GLADE_LIBS) $(CURL_LIBS)

gladedir = $(datadir)/openvrml-player/glade

if ENABLE_PLAYER
bin_PROGRAMS = openvrml-player
glade_DATA = openvrml-player.glade
endif

openvrml_player_SOURCES = filechooserdialog.cpp player.cpp

noinst_HEADERS = filechooserdialog.h

EXTRA_DIST = \
        $(openvrml_player_SOURCES) \
        $(glade_DATA)
