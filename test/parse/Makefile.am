SUBDIRS = testsuite

AM_CPPFLAGS = -I$(top_srcdir)/src/libopenvrml
AM_LDFLAGS = -rpath @OPENVRML_RPATH@ @JNI_LIBS@
LDADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

if ENABLE_TESTS
noinst_PROGRAMS = parse
endif

parse_SOURCES = parse.cpp
