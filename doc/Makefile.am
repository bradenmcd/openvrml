docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

EXTRA_DIST = \
        Doxyfile \
        openvrml.css \
        doxygen-header \
        doxygen-footer \
        index.doc \
        conformance.doc \
        manual

install-data-local:
	$(mkinstalldirs) $(docdir)/manual
	$(INSTALL_DATA) $(srcdir)/manual/* $(docdir)/manual

uninstall-local:
	rm -rf $(docdir)/manual
