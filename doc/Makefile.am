docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

DOCUMENTATION_FILES = \
        Doxyfile \
        doxygen-header \
        doxygen-footer \
        index.doc \
        conformance.doc

DOCUMENTED_SOURCE_FILES = \
        $(top_srcdir)/src/libopenvrml/openvrml/basetypes.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/basetypes.h \
        $(top_srcdir)/src/libopenvrml/openvrml/bounding_volume.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/bounding_volume.h \
        $(top_srcdir)/src/libopenvrml/openvrml/browser.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/browser.h \
        $(top_srcdir)/src/libopenvrml/openvrml/common.h \
        $(top_srcdir)/src/libopenvrml/openvrml/event.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/event.h \
        $(top_srcdir)/src/libopenvrml/openvrml/exposedfield.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/exposedfield.h \
        $(top_srcdir)/src/libopenvrml/openvrml/field_value.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/field_value.h \
        $(top_srcdir)/src/libopenvrml/openvrml/frustum.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/frustum.h \
        $(top_srcdir)/src/libopenvrml/openvrml/img.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/img.h \
        $(top_srcdir)/src/libopenvrml/openvrml/node.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/node.h \
        $(top_srcdir)/src/libopenvrml/openvrml/node_ptr.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/node_ptr.h \
        $(top_srcdir)/src/libopenvrml/openvrml/private.h \
        $(top_srcdir)/src/libopenvrml/openvrml/rendering_context.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/rendering_context.h \
        $(top_srcdir)/src/libopenvrml/openvrml/scope.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/scope.h \
        $(top_srcdir)/src/libopenvrml/openvrml/script.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/script.h \
        $(top_srcdir)/src/libopenvrml/openvrml/ScriptJDK.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/ScriptJDK.h \
        $(top_srcdir)/src/libopenvrml/openvrml/system.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/system.h \
        $(top_srcdir)/src/libopenvrml/openvrml/viewer.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/viewer.h \
        $(top_srcdir)/src/libopenvrml/openvrml/vrml97node.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/vrml97node.h \
        $(top_srcdir)/src/libopenvrml-gl/openvrml/gl/viewer.cpp \
        $(top_srcdir)/src/libopenvrml-gl/openvrml/gl/viewer.h

EXTRA_DIST = $(DOCUMENTATION_FILES) manual

MAINTAINERCLEANFILES = $(srcdir)/manual/*

html-local: manual/index.html

installdirs-local:
	$(mkinstalldirs) $(DESTDIR)$(docdir)/manual

install-data-local: html installdirs-local
	$(INSTALL_DATA) $(srcdir)/manual/* $(DESTDIR)$(docdir)/manual

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/manual

manual/index.html: $(DOCUMENTATION_FILES) $(DOCUMENTED_SOURCE_FILES)
	cd $(srcdir) && $(DOXYGEN)
