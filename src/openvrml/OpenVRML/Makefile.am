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

INCLUDES = -I$(top_srcdir)/lib
AM_CPPFLAGS = $(DEBUG_CPPFLAGS) -DOPENVRML_LIBDIR_=\"$(libdir)\" \
-DOPENVRML_PKGDATADIR_=\"$(pkgdatadir)\"
AM_CXXFLAGS = $(JNI_CFLAGS) $(JS_CFLAGS)

lib_LTLIBRARIES = libopenvrml.la 
libopenvrml_la_LDFLAGS = -version-info $(LIBRARY_VERSION) $(JPEG_LIBS) \
$(PNG_LIBS) $(Z_LIBS) $(JS_LIBS) $(JNI_LIBS)

libopenvrml_la_SOURCES = Doc.cpp doc2.cpp MathUtils.cpp dummysound.cpp \
Image.cpp Audio.cpp System.cpp Viewer.cpp bvolume.cpp VrmlFrustum.cpp \
VrmlRenderContext.cpp field.cpp VrmlMatrix.cpp nodetypeptr.cpp \
VrmlNamespace.cpp Route.cpp node.cpp nodeptr.cpp proto.cpp script.cpp \
ScriptJDK.cpp VrmlScene.cpp vrml97node.cpp

libopenvrml_la_LIBADD = $(top_builddir)/lib/antlr/libantlr.la

OpenVRMLincludedir = $(includedir)/OpenVRML
OpenVRMLinclude_HEADERS = common.h Audio.h Doc.h Image.h MathUtils.h Route.h \
ScriptJDK.h script.h System.h Viewer.h bvolume.h VrmlEvent.h VrmlFrustum.h \
field.h VrmlMatrix.h nodetypeptr.h VrmlNamespace.h proto.h nodeptr.h node.h \
vrml97node.h VrmlRenderContext.h VrmlScene.h doc2.hpp

noinst_HEADERS = Vrml97Parser.hpp Vrml97ParserTokenTypes.hpp private.h sound.h

EXTRA_DIST = Vrml97TokenTypes.txt Vrml97Parser.cpp Vrml97Parser.hpp \
Vrml97ParserTokenTypes.txt Vrml97Parser.g
