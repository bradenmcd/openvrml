lib_LTLIBRARIES = libopenvrml.la

openvrml_includedir = $(pkgincludedir)/openvrml
openvrml_include_HEADERS = \
        common.h \
        basetypes.h \
        doc.h \
        img.h \
        system.h \
        field.h \
        field_value_ptr.h \
        scope.h \
        scope_ptr.h \
        node.h \
        node_ptr.h \
        node_type_ptr.h \
        node_class_ptr.h \
        bounding_volume.h \
        script.h \
        browser.h \
        viewer.h \
        rendering_context.h \
        frustum.h \
        vrml97node.h

noinst_HEADERS = \
	Vrml97Parser.cpp \
        Vrml97Parser.hpp \
        Vrml97ParserTokenTypes.hpp \
        private.h \
        ScriptJDK.h

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
        @JNI_CFLAGS@ \
        @JS_CFLAGS@

libopenvrml_la_SOURCES = \
        basetypes.cpp \
        doc.cpp \
        img.cpp \
        system.cpp \
        field.cpp \
        field_value_ptr.cpp \
        scope.cpp \
        scope_ptr.cpp \
        node.cpp \
        node_ptr.cpp \
        node_type_ptr.cpp \
        node_class_ptr.cpp \
        bounding_volume.cpp \
        script.cpp \
        ScriptJDK.cpp \
        browser.cpp \
        viewer.cpp \
        rendering_context.cpp \
        frustum.cpp \
        vrml97node.cpp

libopenvrml_la_LDFLAGS = \
        -version-info $(LIBRARY_VERSION) \
        @JPEG_LIBS@ \
        @PNG_LIBS@ \
        @Z_LIBS@ \
        @FONTCONFIG_LIBS@ \
        @FREETYPE_LIBS@ \
        @JS_LIBS@ \
        @JNI_LIBS@

libopenvrml_la_LIBADD = $(top_builddir)/lib/antlr/src/libantlr.la

EXTRA_DIST = \
        Vrml97TokenTypes.txt \
        Vrml97Parser.g \
        Vrml97ParserTokenTypes.txt
