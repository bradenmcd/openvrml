docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@
doc_DATA = conformance.html

EXTRA_DIST = Doxyfile.in openvrml.css doxygen-header doxygen-footer

if WITH_DOXYGEN
install-data-local:
	$(INSTALL) -d $(docdir)
	$(DOXYGEN)

uninstall-local:
	rm -rf $(docdir)/manual
endif
