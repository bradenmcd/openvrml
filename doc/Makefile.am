info_TEXINFOS = openvrml-gtkplug.texi
openvrml_gtkplug_TEXINFOS = fdl.texi

docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

DOCUMENTATION_FILES = \
        Doxyfile \
        doxygen-header \
        doxygen-footer \
        index.doc \
        conformance.doc

DOCUMENTED_SOURCE_FILES = \
        $(top_srcdir)/src/libopenvrml/private.h \
        $(top_srcdir)/src/libopenvrml/openvrml/basetypes.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/basetypes.h \
        $(top_srcdir)/src/libopenvrml/openvrml/bounding_volume.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/bounding_volume.h \
        $(top_srcdir)/src/libopenvrml/openvrml/browser.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/browser.h \
        $(top_srcdir)/src/libopenvrml/openvrml/event.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/event.h \
        $(top_srcdir)/src/libopenvrml/openvrml/exposedfield.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/exposedfield.h \
        $(top_srcdir)/src/libopenvrml/openvrml/field_value.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/field_value.h \
        $(top_srcdir)/src/libopenvrml/openvrml/frustum.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/frustum.h \
        $(top_srcdir)/src/libopenvrml/openvrml/node.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/node.h \
        $(top_srcdir)/src/libopenvrml/openvrml/node_impl_util.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/node_impl_util.h \
        $(top_srcdir)/src/libopenvrml/openvrml/rendering_context.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/rendering_context.h \
        $(top_srcdir)/src/libopenvrml/openvrml/scope.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/scope.h \
        $(top_srcdir)/src/libopenvrml/openvrml/script.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/script.h \
        $(top_srcdir)/src/libopenvrml/openvrml/ScriptJDK.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/ScriptJDK.h \
        $(top_srcdir)/src/libopenvrml/openvrml/viewer.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/viewer.h \
        $(top_srcdir)/src/libopenvrml/openvrml/vrml97node.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/vrml97node.h \
        $(top_srcdir)/src/libopenvrml-gl/openvrml/gl/viewer.cpp \
        $(top_srcdir)/src/libopenvrml-gl/openvrml/gl/viewer.h

DOXYGEN_OUTPUT_FILES = \
        $(srcdir)/manual/*.css \
        $(srcdir)/manual/*.html \
        $(srcdir)/manual/*.map \
        $(srcdir)/manual/*.md5 \
        $(srcdir)/manual/*inherit__graph*.png \
        $(srcdir)/manual/doxygen.png \
        $(srcdir)/manual/formula.repository \
        $(srcdir)/manual/form_*.png \
        $(srcdir)/manual/graph_legend.dot \
        $(srcdir)/manual/graph_legend.png \
        $(srcdir)/manual/tab_?.gif

CUSTOMIZED_IMAGES = \
        $(srcdir)/manual/tab_b-openvrml.png \
        $(srcdir)/manual/tab_l-openvrml.png \
        $(srcdir)/manual/tab_r-openvrml.png

EXTRA_DIST = $(DOCUMENTATION_FILES) $(DOXYGEN_OUTPUT_FILES) $(CUSTOMIZED_IMAGES)

MAINTAINERCLEANFILES = $(DOXYGEN_OUTPUT_FILES)

.PHONY: html-local

html-local:
	for x in $(DOXYGEN_OUTPUT_FILES); do rm -f $$x; done
	cd $(srcdir) && $(DOXYGEN)

installdirs-local:
	$(mkinstalldirs) $(DESTDIR)$(docdir)/manual

install-data-local: installdirs-local
	$(INSTALL_DATA) $(DOXYGEN_OUTPUT_FILES) $(CUSTOMIZED_IMAGES) $(DESTDIR)$(docdir)/manual

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/manual
