SUBDIRS = . parse

AM_CPPFLAGS = -I$(top_srcdir)/src/libopenvrml
AM_LDFLAGS = @OPENVRML_RPATH@

if ENABLE_TESTS
TESTS = browser_basic \
        browser-create_vrml_from_stream \
	parse-anchor \
        node_interface_set

noinst_LTLIBRARIES = libtest-openvrml.la
noinst_PROGRAMS = $(TESTS)
endif

noinst_HEADERS = test_browser.h

libtest_openvrml_la_SOURCES = test_browser.cpp
libtest_openvrml_la_LIBADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

browser_basic_SOURCES = browser_basic.cpp
browser_basic_LDADD = libtest-openvrml.la

browser_create_vrml_from_stream_SOURCES = browser-create_vrml_from_stream.cpp
browser_create_vrml_from_stream_LDADD = libtest-openvrml.la

parse_anchor_SOURCES = parse-anchor.cpp
parse_anchor_LDADD = libtest-openvrml.la

node_interface_set_SOURCES = node_interface_set.cpp
node_interface_set_LDADD = libtest-openvrml.la

EXTRA_DIST = \
        $(libtest_openvrml_la_SOURCES) \
        $(browser_basic_SOURCES) \
        $(browser_create_vrml_from_stream_SOURCES) \
        $(parse_anchor_SOURCES) \
        $(node_interface_set_SOURCES)
