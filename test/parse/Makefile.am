SUBDIRS = testsuite

AM_CPPFLAGS = -I$(top_srcdir)/src/libopenvrml
AM_LDFLAGS = -rpath @OPENVRML_RPATH@ @JNI_LIBS@
LDADD = $(top_builddir)/src/libopenvrml/libopenvrml.la

noinst_PROGRAMS = parse

parse_SOURCES = parse.cpp
