LIBRARY_VERSION = 1:2:0
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

#
# Add any debug flags
#
if DEBUG
else
  DEBUG_CPPFLAGS = -DNDEBUG
endif

INCLUDES = -I$(top_srcdir)/lib -I$(top_builddir)/java
AM_CPPFLAGS = $(DEBUG_CPPFLAGS) -DOPENVRML_PACKAGE_NAME_=\"$(PACKAGE_NAME)\" \
-DOPENVRML_PACKAGE_VERSION_=\"$(PACKAGE_VERSION)\" \
-DOPENVRML_LIBDIR_=\"$(libdir)\" -DOPENVRML_PKGDATADIR_=\"$(pkgdatadir)\"
AM_CXXFLAGS = $(JNI_CFLAGS) $(JS_CFLAGS)

lib_LTLIBRARIES = libopenvrml.la 
libopenvrml_la_LDFLAGS = -version-info $(LIBRARY_VERSION) $(JPEG_LIBS) \
$(PNG_LIBS) $(Z_LIBS) $(JS_LIBS) $(JNI_LIBS)

libopenvrml_la_SOURCES = \
        MathUtils.cpp \
        Doc.cpp \
        doc2.cpp \
        dummysound.cpp \
        Image.cpp \
        Audio.cpp \
        System.cpp \
        field.cpp \
        fieldvalueptr.cpp \
        VrmlMatrix.cpp \
        node.cpp \
        nodeptr.cpp \
        nodetypeptr.cpp \
        nodeclassptr.cpp \
        bvolume.cpp \
        script.cpp \
        ScriptJDK.cpp \
        VrmlNamespace.cpp \
        VrmlScene.cpp \
        Viewer.cpp \
        VrmlRenderContext.cpp \
        VrmlFrustum.cpp \
        vrml97node.cpp

libopenvrml_la_LIBADD = $(top_builddir)/lib/antlr/libantlr.la

OpenVRMLincludedir = $(includedir)/OpenVRML
OpenVRMLinclude_HEADERS = \
        common.h \
        MathUtils.h \
        Doc.h \
        doc2.hpp \
        Image.h \
        Audio.h \
        System.h \
        field.h \
        fieldvalueptr.h \
        VrmlMatrix.h \
        node.h \
        nodeptr.h \
        nodetypeptr.h \
        nodeclassptr.h \
        bvolume.h \
        script.h \
        ScriptJDK.h \
        VrmlNamespace.h \
        VrmlScene.h \
        Viewer.h \
        VrmlRenderContext.h \
        VrmlFrustum.h \
        vrml97node.h

noinst_HEADERS = Vrml97Parser.hpp Vrml97ParserTokenTypes.hpp private.h sound.h

EXTRA_DIST = Vrml97TokenTypes.txt Vrml97Parser.cpp Vrml97Parser.hpp \
Vrml97ParserTokenTypes.txt Vrml97Parser.g
