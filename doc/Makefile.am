docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

EXTRA_DIST = Doxyfile.in openvrml.css doxygen-header doxygen-footer

if ENABLE_API_DOCS
install-data-local:
	$(INSTALL) -d $(docdir)
	$(DOXYGEN)

uninstall-local:
	rm -rf $(docdir)/manual
endif
