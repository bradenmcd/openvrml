AM_CPPFLAGS = -I$(top_srcdir)/src/libopenvrml
AM_LDFLAGS = @OPENVRML_RPATH@
LDADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

TESTS_ENVIRONMENT = BOOST_TEST_REPORT_LEVEL=detailed
TESTS = browser_basic \
        browser_create_vrml_from_stream \
        parse_anchor \
        node_interface_set

noinst_PROGRAMS = $(TESTS) parse

browser_basic_SOURCES = browser_basic.cpp

browser_create_vrml_from_stream_SOURCES = browser_create_vrml_from_stream.cpp

parse_anchor_SOURCES = parse_anchor.cpp

node_interface_set_SOURCES = node_interface_set.cpp
node_interface_set_LDFLAGS = -lboost_unit_test_framework

parse_SOURCES = parse.cpp

EXTRA_DIST = \
        package.m4 \
        testsuite.at \
        testsuite

TESTSUITE = $(srcdir)/testsuite
check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) $(TESTSUITE)

AUTOTEST = autom4te --language=autotest
$(TESTSUITE): $(srcdir)/package.m4 $(srcdir)/testsuite.at
	$(AUTOTEST) -I $(srcdir) $@.at -o $@.tmp; mv $@.tmp $@

$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	{                                      \
          echo '# Signature of the current package.'; \
          echo 'm4_define([AT_PACKAGE_NAME],      [@PACKAGE_NAME@])'; \
          echo 'm4_define([AT_PACKAGE_TARNAME],   [@PACKAGE_TARNAME@])'; \
          echo 'm4_define([AT_PACKAGE_VERSION],   [@PACKAGE_VERSION@])'; \
          echo 'm4_define([AT_PACKAGE_STRING],    [@PACKAGE_STRING@])'; \
          echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
        } > $(srcdir)/package.m4

clean-local:
	test ! -f $(TESTSUITE) || $(SHELL) $(TESTSUITE) --clean
	rm -f *.tmp
	rm -f -r autom4te.cache

DISTCLEANFILES = atconfig
