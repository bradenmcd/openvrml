docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

EXTRA_DIST = \
        Doxyfile \
        doxygen-header \
        doxygen-footer \
        index.doc \
        conformance.doc \
        manual

doc:
	cd $(srcdir) && $(DOXYGEN)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(docdir)/manual
	$(INSTALL_DATA) $(srcdir)/manual/* $(DESTDIR)$(docdir)/manual

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/manual
