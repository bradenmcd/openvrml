docdir = $(datadir)/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@
doc_DATA = conformance.html

EXTRA_DIST = Doxyfile.in openvrml.css doxygen-header doxygen-footer

if WITH_DOXYGEN
install-data-local:
	doxygen

uninstall-local:
	rm -rf $(docdir)/manual
endif
