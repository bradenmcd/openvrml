AM_CPPFLAGS = -I$(top_srcdir)/src/libopenvrml
LDADD = $(top_builddir)/src/libopenvrml/openvrml/libopenvrml.la

TESTS_ENVIRONMENT = \
        LD_LIBRARY_PATH=@GRE_PATH@:@JDK_LIBRARY_PATH@:$$LD_LIBRARY_PATH
TESTS = browser_basic

noinst_PROGRAMS = $(TESTS)

browser_basic_SOURCES = browser_basic.cpp
