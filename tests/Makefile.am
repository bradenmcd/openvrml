AM_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -DBOOST_SPIRIT_THREADSAFE \
        -DBOOST_SPIRIT_CLOSURE_LIMIT=6 \
        -DPHOENIX_LIMIT=6 \
        -DBOOST_TEST_DYN_LINK
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(OPENVRML_RPATH)

TESTS_ENVIRONMENT = BOOST_TEST_REPORT_LEVEL=no
TESTS = color \
        rotation \
        mat4f \
        image \
        browser \
        parse_anchor \
        node_metatype_id \
        node_interface_set

check_LTLIBRARIES = libtest-openvrml.la
check_PROGRAMS = $(TESTS) parse-vrml97 parse-x3dvrml browser-parse-vrml
noinst_HEADERS = test_resource_fetcher.h

libtest_openvrml_la_SOURCES = test_resource_fetcher.cpp
libtest_openvrml_la_LIBADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

color_SOURCES = color.cpp
color_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework$(BOOST_LIB_SUFFIX)

rotation_SOURCES = rotation.cpp
rotation_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework$(BOOST_LIB_SUFFIX)

mat4f_SOURCES = mat4f.cpp
mat4f_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework$(BOOST_LIB_SUFFIX)

image_SOURCES = image.cpp
image_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework$(BOOST_LIB_SUFFIX)

browser_SOURCES = browser.cpp
browser_LDADD = \
        libtest-openvrml.la \
        -lboost_unit_test_framework$(BOOST_LIB_SUFFIX) \
        -lboost_filesystem$(BOOST_LIB_SUFFIX)

parse_anchor_SOURCES = parse_anchor.cpp
parse_anchor_LDADD = \
        libtest-openvrml.la

node_interface_set_SOURCES = node_interface_set.cpp
node_interface_set_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework$(BOOST_LIB_SUFFIX)

node_metatype_id_SOURCES = node_metatype_id.cpp
node_metatype_id_LDADD = \
        $(top_builddir)/src/libopenvrml/libopenvrml.la \
        -lboost_unit_test_framework$(BOOST_LIB_SUFFIX)

parse_vrml97_SOURCES = parse_vrml97.cpp
parse_vrml97_LDADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

parse_x3dvrml_SOURCES = parse_x3dvrml.cpp
parse_x3dvrml_LDADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

browser_parse_vrml_SOURCES = browser_parse_vrml.cpp
browser_parse_vrml_LDADD = libtest-openvrml.la

JAVAROOT = $(top_builddir)/tests
CLASSPATH_ENV = CLASSPATH=$(top_builddir)/src/script/java/script.jar
dist_noinst_JAVA = TestScript.java


EXTRA_DIST = \
        package.m4 \
        testsuite.at \
        testsuite \
        vrml97/bad/unrecognized-node.wrl \
        vrml97/bad/unrecognized-field.wrl \
        vrml97/bad/value-for-eventin-in-proto.wrl \
        vrml97/bad/is-outside-proto.wrl \
        vrml97/bad/is-value-type-mismatch.wrl \
        vrml97/bad/proto-eventin-conflict.wrl \
        vrml97/bad/proto-eventout-conflict.wrl \
        vrml97/bad/use-def-in-different-proto-default-value.wrl \
        vrml97/bad/route-from-field.wrl \
        vrml97/bad/route-to-field.wrl \
        vrml97/bad/route-sfint32-to-sfbool.wrl \
        vrml97/bad/exposedfield-in-script.wrl \
        vrml97/good/minimal.wrl \
        vrml97/good/def-use-in-proto-default-value.wrl \
        vrml97/good/line-number.wrl \
        vrml97/good/proto-containment-2-deep.wrl \
        vrml97/good/proto-containment-trivial.wrl \
        vrml97/good/proto-field-is-proto.wrl \
        vrml97/good/proto-impl-exposedfield-event-is.wrl \
        vrml97/good/proto-in-proto-default-value.wrl \
        vrml97/good/route-referencing-current-node.wrl \
        vrml97/good/self-referential-script-in-proto.wrl \
        vrml97/good/self-referential-script.wrl \
        vrml97/good/unimplemented-externproto.wrl \
        vrml97/good/unrecognized-externproto.wrl \
        x3dv/bad/unsupported-component-level.x3dv \
        x3dv/good/core+core2.x3dv \
        x3dv/good/minimal.x3dv \
        x3dv/good/deprecated-eventin.x3dv \
        x3dv/good/deprecated-eventout.x3dv \
        x3dv/good/deprecated-exposedfield.x3dv \
        x3dv/good/deprecated-field.x3dv \
        x3dv/good/minimal-full.x3dv \
        x3dv/good/minimal-immersive.x3dv \
        x3dv/good/minimal-interactive.x3dv \
        x3dv/good/minimal-interchange.x3dv \
        x3dv/good/minimal-mpeg4.x3dv \
        x3dv/good/x3d+vrml97-component.x3dv

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
