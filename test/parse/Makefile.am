SUBDIRS = testsuite

AM_CPPFLAGS = -I$(top_srcdir)/src/libopenvrml -I$(srcdir)/..
AM_LDFLAGS = @OPENVRML_RPATH@ @JNI_LIBS@

if ENABLE_TESTS
noinst_PROGRAMS = parse
endif

parse_SOURCES = parse.cpp
parse_LDADD = ../libtest-openvrml.la
