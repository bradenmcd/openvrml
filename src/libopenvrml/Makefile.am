lib_LTLIBRARIES = libopenvrml.la

openvrml_includedir = $(pkgincludedir)/openvrml
openvrml_include_HEADERS = \
        openvrml/common.h \
        openvrml/basetypes.h \
        openvrml/img.h \
        openvrml/system.h \
        openvrml/field.h \
        openvrml/scope.h \
        openvrml/node.h \
        openvrml/node_ptr.h \
        openvrml/bounding_volume.h \
        openvrml/script.h \
        openvrml/browser.h \
        openvrml/viewer.h \
        openvrml/rendering_context.h \
        openvrml/frustum.h \
        openvrml/vrml97node.h

noinst_HEADERS = \
        openvrml/Vrml97Parser.cpp \
        openvrml/Vrml97Parser.hpp \
        openvrml/Vrml97ParserTokenTypes.hpp \
        openvrml/private.h \
        openvrml/ScriptJDK.h

LIBRARY_VERSION = 4:1:0
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
        -DOPENVRML_PKGDATADIR_=\"$(pkgdatadir)\"
AM_CXXFLAGS = \
        @FONTCONFIG_CFLAGS@ \
        @FREETYPE_CFLAGS@ \
        @JS_CFLAGS@

libopenvrml_la_SOURCES = \
        openvrml/basetypes.cpp \
        openvrml/img.cpp \
        openvrml/system.cpp \
        openvrml/field.cpp \
        openvrml/scope.cpp \
        openvrml/node.cpp \
        openvrml/node_ptr.cpp \
        openvrml/bounding_volume.cpp \
        openvrml/script.cpp \
        openvrml/ScriptJDK.cpp \
        openvrml/browser.cpp \
        openvrml/viewer.cpp \
        openvrml/rendering_context.cpp \
        openvrml/frustum.cpp \
        openvrml/vrml97node.cpp

libopenvrml_la_LDFLAGS = \
        -version-info $(LIBRARY_VERSION) \
        @JPEG_LIBS@ \
        @PNG_LIBS@ \
        @Z_LIBS@ \
        @FONTCONFIG_LIBS@ \
        @FREETYPE_LIBS@ \
        @JS_LIBS@ \
        @JNI_LIBS@

libopenvrml_la_LIBADD = $(top_builddir)/lib/antlr/libantlr.la

EXTRA_DIST = \
        openvrml/Vrml97TokenTypes.txt \
        openvrml/Vrml97Parser.g \
        openvrml/Vrml97ParserTokenTypes.txt
