docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

EXTRA_DIST = \
        Doxyfile.in \
        doxygen-header \
        doxygen-footer \
        index.doc

DISTCLEANFILES = manual/*

html: manual/index.html

installdirs-local:
	$(mkinstalldirs) $(DESTDIR)$(docdir)/manual

install-data-local: html installdirs-local
	$(INSTALL_DATA) $(srcdir)/manual/* $(DESTDIR)$(docdir)/manual

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/manual

manual/index.html: Doxyfile
	$(DOXYGEN)

dist-hook: manual/index.html
	cp -R $(srcdir)/manual $(distdir)
