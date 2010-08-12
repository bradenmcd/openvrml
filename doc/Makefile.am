docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@
javadocdir = $(datadir)/javadoc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

DOCUMENTATION_FILES = \
        Doxyfile \
        doxygen-header \
        doxygen-footer \
        index.doc \
        intro.doc \
        parsing.doc \
        libopenvrml.doc \
        gpl.doc \
        lgpl.doc \
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
        $(top_srcdir)/src/libopenvrml/openvrml/read_write_mutex.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/read_write_mutex.h \
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
        $(top_srcdir)/src/libopenvrml/openvrml/vrml97_grammar.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/vrml97_grammar.h \
        $(top_srcdir)/src/libopenvrml/openvrml/x3d_vrml_grammar.cpp \
        $(top_srcdir)/src/libopenvrml/openvrml/x3d_vrml_grammar.h \
        $(top_srcdir)/src/libopenvrml-gl/openvrml/gl/viewer.cpp \
        $(top_srcdir)/src/libopenvrml-gl/openvrml/gl/viewer.h

DOXYGEN_OUTPUT_FILES = \
        $(srcdir)/manual/*.css \
        $(srcdir)/manual/*.html \
        $(srcdir)/manual/*.map \
        $(srcdir)/manual/*.md5 \
        $(srcdir)/manual/*__incl.png \
        $(srcdir)/manual/*inherit__graph*.png \
        $(srcdir)/manual/bc_s.png \
        $(srcdir)/manual/closed.png \
        $(srcdir)/manual/doxygen.png \
        $(srcdir)/manual/formula.repository \
        $(srcdir)/manual/form_*.png \
        $(srcdir)/manual/graph_legend.png \
        $(srcdir)/manual/nav_f.png \
        $(srcdir)/manual/nav_h.png \
        $(srcdir)/manual/open.png \
        $(srcdir)/manual/tab_a.png \
        $(srcdir)/manual/tab_b.png \
        $(srcdir)/manual/tab_h.png \
        $(srcdir)/manual/tab_s.png

CUSTOMIZED_IMAGES = \
        $(srcdir)/manual/OGL_sm_wht.png \
        $(srcdir)/manual/x3d-white-on-black.png

JAVADOC_FILES = \
        $(srcdir)/javadoc/*.html \
        $(srcdir)/javadoc/package-list \
        $(srcdir)/javadoc/stylesheet.css

JAVADOC_RESOURCES_FILES = \
        $(srcdir)/javadoc/resources/inherit.gif

JAVADOC_VRML_FILES = \
        $(srcdir)/javadoc/vrml/*.html

JAVADOC_VRML_FIELD_FILES = \
        $(srcdir)/javadoc/vrml/field/*.html

JAVADOC_VRML_NODE_FILES = \
        $(srcdir)/javadoc/vrml/node/*.html

JAVADOC_OUTPUT_FILES = \
        $(JAVADOC_FILES) \
        $(JAVADOC_RESOURCES_FILES) \
        $(JAVADOC_VRML_FILES) \
        $(JAVADOC_VRML_FIELD_FILES) \
        $(JAVADOC_VRML_NODE_FILES)

EXTRA_DIST = \
        $(DOCUMENTATION_FILES) \
        $(DOXYGEN_OUTPUT_FILES) \
        $(CUSTOMIZED_IMAGES) \
        $(JAVADOC_OUTPUT_FILES)

MAINTAINERCLEANFILES = $(DOXYGEN_OUTPUT_FILES)

.PHONY: html-local doxygen-html javadoc-html

doxygen-html:
	for x in $(DOXYGEN_OUTPUT_FILES); do rm -f $$x; done
	cd $(srcdir) && $(DOXYGEN)

javadoc-html:
	rm -rf $(srcdir)/javadoc
	cd $(srcdir) \
                && javadoc -sourcepath ../src/script/java \
                           -link $(javadocdir)/java \
                           -d javadoc \
                           vrml vrml.field vrml.node

html-local: doxygen-html javadoc-html

installdirs-local:
	$(mkinstalldirs) $(DESTDIR)$(docdir)/manual \
                         $(DESTDIR)$(javadocdir)/resources \
                         $(DESTDIR)$(javadocdir)/vrml/node \
                         $(DESTDIR)$(javadocdir)/vrml/field

install-data-local: installdirs-local
	$(INSTALL_DATA) $(DOXYGEN_OUTPUT_FILES) $(CUSTOMIZED_IMAGES) \
                        $(DESTDIR)$(docdir)/manual
	$(INSTALL_DATA) $(JAVADOC_FILES) \
                        $(DESTDIR)$(javadocdir)
	$(INSTALL_DATA) $(JAVADOC_RESOURCES_FILES) \
                        $(DESTDIR)$(javadocdir)/resources
	$(INSTALL_DATA) $(JAVADOC_VRML_FILES) \
                        $(DESTDIR)$(javadocdir)/vrml
	$(INSTALL_DATA) $(JAVADOC_VRML_FIELD_FILES) \
                        $(DESTDIR)$(javadocdir)/vrml/field
	$(INSTALL_DATA) $(JAVADOC_VRML_NODE_FILES) \
                        $(DESTDIR)$(javadocdir)/vrml/node

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/manual $(DESTDIR)$(javadocdir)
