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
        openvrml/node_impl_util.h \
        openvrml/vrml97node.h \
        openvrml/x3d_core.h \
        openvrml/x3d_networking.h \
        openvrml/x3d_grouping.h \
        openvrml/x3d_rendering.h \
        openvrml/x3d_shape.h \
        openvrml/x3d_geometry2d.h \
        openvrml/x3d_texturing.h \
        openvrml/x3d_interpolation.h \
        openvrml/x3d_key_device_sensor.h \
        openvrml/x3d_event_utilities.h \
        openvrml/x3d_dis.h \
        openvrml/x3d_environmental_effects.h \
        openvrml/x3d_geospatial.h \
        openvrml/x3d_hanim.h \
        openvrml/x3d_nurbs.h \
        openvrml/x3d_cad_geometry.h

VRML97PARSER_BUILT_SOURCEFILES = \
        $(srcdir)/openvrml/Vrml97Parser.cpp \
        $(srcdir)/openvrml/Vrml97Parser.hpp \
        $(srcdir)/openvrml/Vrml97ParserTokenTypes.hpp
VRML97PARSER_BUILT_TEXTFILES = \
        openvrml/Vrml97ParserTokenTypes.txt
VRML97PARSER_BUILT_FILES = \
        $(VRML97PARSER_BUILT_SOURCEFILES) \
        $(VRML97PARSER_BUILT_TEXTFILES)

X3DVRMLPARSER_BUILT_SOURCEFILES = \
        $(srcdir)/openvrml/X3DVrmlParser.cpp \
        $(srcdir)/openvrml/X3DVrmlParser.hpp \
        $(srcdir)/openvrml/X3DVrmlParserTokenTypes.hpp
X3DVRMLPARSER_BUILT_TEXTFILES = \
        $(srcdir)/openvrml/X3DVrmlParserTokenTypes.txt
X3DVRMLPARSER_BUILT_FILES = \
        $(X3DVRMLPARSER_BUILT_SOURCEFILES) \
        $(X3DVRMLPARSER_BUILT_TEXTFILES)

noinst_HEADERS = \
        private.h \
        openvrml/ScriptJDK.h \
        $(VRML97PARSER_BUILT_SOURCEFILES) \
        $(X3DVRMLPARSER_BUILT_SOURCEFILES)

LIBRARY_VERSION = 7:1:1
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
        openvrml/node_impl_util.cpp \
        openvrml/vrml97node.cpp \
        openvrml/x3d_core.cpp \
        openvrml/x3d_networking.cpp \
        openvrml/x3d_grouping.cpp \
        openvrml/x3d_rendering.cpp \
        openvrml/x3d_shape.cpp \
        openvrml/x3d_geometry2d.cpp \
        openvrml/x3d_texturing.cpp \
        openvrml/x3d_interpolation.cpp \
        openvrml/x3d_key_device_sensor.cpp \
        openvrml/x3d_event_utilities.cpp \
        openvrml/x3d_dis.cpp \
        openvrml/x3d_environmental_effects.cpp \
        openvrml/x3d_geospatial.cpp \
        openvrml/x3d_hanim.cpp \
        openvrml/x3d_nurbs.cpp \
        openvrml/x3d_cad_geometry.cpp

libopenvrml_la_LDFLAGS = \
        -version-info $(LIBRARY_VERSION) \
        @JPEG_LIBS@ \
        @PNG_LIBS@ \
        @FONTCONFIG_LIBS@ \
        @FREETYPE_LIBS@ \
        @JS_LIBS@ \
        @JNI_LIBS@ \
        @PTHREAD_LIBS@

libopenvrml_la_LIBADD = \
        $(top_builddir)/lib/antlr/libantlr.la \
        -lboost_thread@BOOST_LIB_SUFFIX@

EXTRA_DIST = \
        openvrml-config.h.in \
        openvrml/Vrml97TokenTypes.txt \
        openvrml/Vrml97Parser.g \
        $(VRML97PARSER_BUILT_TEXTFILES) \
        openvrml/X3DVrmlTokenTypes.txt \
        openvrml/X3DVrmlParser.g \
        $(X3DVRMLPARSER_BUILT_TEXTFILES)

BUILT_SOURCES = \
        $(VRML97PARSER_BUILT_SOURCEFILES) \
        $(X3DVRMLPARSER_BUILT_SOURCEFILES)

#
# Note that maintainer-clean cleans BUILT_SOURCES, too.
#
MAINTAINERCLEANFILES = \
        $(VRML97PARSER_BUILT_TEXTFILES) \
        $(srcdir)/openvrml/expandedX3DVrml97Parser.g \
        $(X3DVRMLPARSER_BUILT_TEXTFILES)

#
# antlr doesn't update timestamps correctly; so files are touched after being
# generated.
#
$(VRML97PARSER_BUILT_FILES): $(srcdir)/openvrml/Vrml97Parser.g $(srcdir)/openvrml/Vrml97TokenTypes.txt
	$(ANTLR) -o $(srcdir)/openvrml $(srcdir)/openvrml/Vrml97Parser.g \
&& touch $(VRML97PARSER_BUILT_FILES)

$(X3DVRMLPARSER_BUILT_FILES): $(srcdir)/openvrml/Vrml97Parser.g $(srcdir)/openvrml/X3DVrmlParser.g $(srcdir)/openvrml/X3DVrmlTokenTypes.txt
	$(ANTLR) -o $(srcdir)/openvrml -glib $(srcdir)/openvrml/Vrml97Parser.g $(srcdir)/openvrml/X3DVrmlParser.g \
&& touch $(X3DVRMLPARSER_BUILT_FILES)
