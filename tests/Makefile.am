AM_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
AM_CXXFLAGS = @PTHREAD_CFLAGS@
AM_LDFLAGS = @OPENVRML_RPATH@

TESTS_ENVIRONMENT = BOOST_TEST_REPORT_LEVEL=detailed
TESTS = color \
        rotation \
        mat4f \
        image \
        browser \
        parse_anchor \
        node_metatype_id \
        node_interface_set

check_LTLIBRARIES = libtest-openvrml.la
check_PROGRAMS = $(TESTS) parse-vrml97 parse-x3dvrml
noinst_HEADERS = test_browser.h

libtest_openvrml_la_SOURCES = test_browser.cpp
libtest_openvrml_la_LIBADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

color_SOURCES = color.cpp
color_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework

rotation_SOURCES = rotation.cpp
rotation_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework

mat4f_SOURCES = mat4f.cpp
mat4f_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework

image_SOURCES = image.cpp
image_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework

browser_SOURCES = browser.cpp
browser_LDADD = libtest-openvrml.la -lboost_unit_test_framework -lboost_filesystem

parse_anchor_SOURCES = parse_anchor.cpp
parse_anchor_LDADD = libtest-openvrml.la

node_interface_set_SOURCES = node_interface_set.cpp
node_interface_set_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework

node_metatype_id_SOURCES = node_metatype_id.cpp
node_metatype_id_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework

parse_vrml97_SOURCES = parse_vrml97.cpp
parse_vrml97_LDADD = libtest-openvrml.la

parse_x3dvrml_SOURCES = parse_x3dvrml.cpp
parse_x3dvrml_LDADD = libtest-openvrml.la

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
