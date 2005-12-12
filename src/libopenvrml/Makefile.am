lib_LTLIBRARIES = libopenvrml.la

pkginclude_HEADERS = \
        openvrml-config.h \
        openvrml-common.h
openvrml_includedir = $(pkgincludedir)/openvrml
openvrml_include_HEADERS = \
        openvrml/basetypes.h \
        openvrml/field_value.h \
        openvrml/event.h \
	openvrml/exposedfield.h \
        openvrml/scope.h \
        openvrml/node.h \
        openvrml/bounding_volume.h \
        openvrml/script.h \
        openvrml/browser.h \
        openvrml/viewer.h \
        openvrml/rendering_context.h \
        openvrml/frustum.h \
        openvrml/vrml97node.h \
        openvrml/node_impl_util.h

noinst_HEADERS = \
        private.h \
        openvrml/Vrml97Parser.cpp \
        openvrml/Vrml97Parser.hpp \
        openvrml/Vrml97ParserTokenTypes.hpp \
        openvrml/X3DVrmlParser.cpp \
        openvrml/X3DVrmlParser.hpp \
        openvrml/X3DVrmlParserTokenTypes.hpp \
        openvrml/ScriptJDK.h

LIBRARY_VERSION = 5:3:0
#                 | | |
#          +------+ | +---+
#          |        |     |
#       current:revision:age
#          |        |     |
#          |        |     +- Increment if interfaces have been added
#          |        |        Set to zero if interfaces have been removed or
#          |        |        changed
#          |        +- Increment if source code has changed
#          |           Set to zero if current is incremented
#          +- Increment if interfaces have been added, removed or changed

AM_CPPFLAGS = \
        -I$(top_srcdir)/lib/antlr \
        -I$(top_builddir)/java \
        -I$(top_srcdir)/src/libopenvrml \
        -DOPENVRML_LIBDIR_=\"$(libdir)\" \
        -DOPENVRML_PKGDATADIR_=\"$(pkgdatadir)\" \
        -DBOOST_SPIRIT_THREADSAFE \
        -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS \
        -DBOOST_MPL_LIMIT_VECTOR_SIZE=30
AM_CXXFLAGS = \
        @PTHREAD_CFLAGS@ \
        @FONTCONFIG_CFLAGS@ \
        @FREETYPE_CFLAGS@ \
        @JS_CFLAGS@

libopenvrml_la_SOURCES = \
        openvrml/basetypes.cpp \
        openvrml/field_value.cpp \
        openvrml/event.cpp \
        openvrml/exposedfield.cpp \
        openvrml/scope.cpp \
        openvrml/node.cpp \
        openvrml/bounding_volume.cpp \
        openvrml/script.cpp \
        openvrml/ScriptJDK.cpp \
        openvrml/browser.cpp \
        openvrml/viewer.cpp \
        openvrml/rendering_context.cpp \
        openvrml/frustum.cpp \
        openvrml/vrml97node.cpp \
        openvrml/node_impl_util.cpp

libopenvrml_la_LDFLAGS = \
        -version-info $(LIBRARY_VERSION) \
        @JPEG_LIBS@ \
        @PNG_LIBS@ \
        @FONTCONFIG_LIBS@ \
        @FREETYPE_LIBS@ \
        @JS_LIBS@ \
        @JNI_LIBS@ \
        -lboost_thread \
        @PTHREAD_LIBS@

libopenvrml_la_LIBADD = $(top_builddir)/lib/antlr/libantlr.la

EXTRA_DIST = \
        openvrml-gl-config.h.in \
        openvrml-gl-common.h \
        openvrml/Vrml97TokenTypes.txt \
        openvrml/Vrml97Parser.g \
        openvrml/Vrml97ParserTokenTypes.txt \
        openvrml/X3DVrmlTokenTypes.txt \
        openvrml/X3DVrmlParser.g \
        openvrml/X3DVrmlParserTokenTypes.txt

BUILT_SOURCES = \
        $(srcdir)/openvrml/Vrml97Parser.cpp \
        $(srcdir)/openvrml/Vrml97Parser.hpp \
        $(srcdir)/openvrml/Vrml97ParserTokenTypes.hpp \
        $(srcdir)/openvrml/X3DVrmlParser.cpp \
        $(srcdir)/openvrml/X3DVrmlParser.hpp \
        $(srcdir)/openvrml/X3DVrmlParserTokenTypes.hpp

MAINTAINERCLEANFILES = \
        $(BUILT_SOURCES) \
        $(srcdir)/openvrml/Vrml97ParserTokenTypes.txt \
        $(srcdir)/openvrml/expandedX3DVrml97Parser.g \
        $(srcdir)/openvrml/X3DVrmlParserTokenTypes.txt

$(srcdir)/openvrml/Vrml97Parser.hpp $(srcdir)/openvrml/Vrml97Parser.cpp: $(srcdir)/openvrml/Vrml97Parser.g $(srcdir)/openvrml/Vrml97TokenTypes.txt
	$(ANTLR) -o $(srcdir)/openvrml $(srcdir)/openvrml/Vrml97Parser.g

$(srcdir)/openvrml/X3DVrmlParser.hpp $(srcdir)/openvrml/X3DVrmlParser.cpp: $(srcdir)/openvrml/Vrml97Parser.g $(srcdir)/openvrml/X3DVrmlParser.g $(srcdir)/openvrml/X3DVrmlTokenTypes.txt
	$(ANTLR) -o $(srcdir)/openvrml -glib $(srcdir)/openvrml/Vrml97Parser.g $(srcdir)/openvrml/X3DVrmlParser.g
