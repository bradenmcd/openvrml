AM_CPPFLAGS = -I$(top_srcdir)/src/libopenvrml
AM_LDFLAGS = -rpath @OPENVRML_RPATH@
LDADD = $(top_builddir)/src/libopenvrml/openvrml/libopenvrml.la

TESTS = browser_basic \
        browser-create_vrml_from_stream \
	parse-anchor

noinst_PROGRAMS = $(TESTS)

browser_basic_SOURCES = browser_basic.cpp

browser_create_vrml_from_stream_SOURCES = browser-create_vrml_from_stream.cpp

parse_anchor_SOURCES = parse-anchor.cpp
