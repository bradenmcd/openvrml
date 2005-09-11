AM_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
AM_CXXFLAGS = @PTHREAD_CFLAGS@
AM_LDFLAGS = @OPENVRML_RPATH@
LDADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

TESTS_ENVIRONMENT = BOOST_TEST_REPORT_LEVEL=detailed
TESTS = \
        browser \
        browser_basic \
        parse_anchor \
        node_interface_set

check_LTLIBRARIES = libtest-openvrml.la
check_PROGRAMS = $(TESTS) parse
noinst_HEADERS = test_browser.h

libtest_openvrml_la_SOURCES = test_browser.cpp
libtest_openvrml_la_LIBADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

browser_SOURCES = browser.cpp
browser_LDADD = libtest-openvrml.la
browser_LDFLAGS = -lboost_unit_test_framework

browser_basic_SOURCES = browser_basic.cpp
browser_basic_LDADD = libtest-openvrml.la

parse_anchor_SOURCES = parse_anchor.cpp
parse_anchor_LDADD = libtest-openvrml.la

node_interface_set_SOURCES = node_interface_set.cpp
node_interface_set_LDFLAGS = -lboost_unit_test_framework

parse_SOURCES = parse.cpp
parse_LDADD = libtest-openvrml.la

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
