#
# This Makefile.am can be used either for building libjs as a convenience
# library or as an installed library. This is dependent on substitutions made
# during processing of the resulting Makefile.in by configure. The following
# variables affect what happens:
#
# LIBJS
#       should be "libjs.la" if installing the library; empty otherwise.
#
# NOINST_LIBJS
#       should be "libjs.la" if building a helper library; empty otherwise.
# 
# JS_RPATH_LDFLAG
#       For a convenience library, this MUST BE EMPTY. For an installed library
#       it MUST be "-rpath {libdir}", where {libdir} is the directory in which
#       the compiled library will be installed.
# 
# JS_INCLUDE_DIR
#       path where the header files will be installed. Can be empty if no
#       headers are being installed.
# 
# JS_INCLUDE_FILES
#       the headers that will be installed. Should generally be empty (if
#       building a convenience library and no js headers need to be installed)
#       or "jsapi.h jsautocfg.h jscompat.h jslong.h jsotypes.h jsosdep.h 
#       jspubtd.h jstypes.h".
#
# Finally, the configure script should employ an automake conditional to set the
# value of JS_PRELINK_FDLIBM. You probably want this variable to evaluate to
# true *unless you will be prelinking libjs against another library*. Note that
# currently, fdlibm is NOT installed *even if this variable is false*.
#

LIBRARY_VERSION = 0:0:0
#                 | | |
#          +------+ | +---+
#          |        |     |
#       current:revision:age
#          |        |     |
#          |        |     +- Increment if interfaces have been added
#          |        |        Set to zero if interfaces have been removed or
#          |        |        changed
#          |        +- Increment if source code has changed
#          |           Set to zero if current is incremented
#          +- Increment if interfaces have been added, removed or changed

SUBDIRS = fdlibm

if DEBUG
  DEBUG_CPPFLAGS = -DDEBUG
else
  DEBUG_CPPFLAGS = -DNDEBUG
endif

AIX_DEFS = -DAIX -DAIXV3
AIX_4_3_DEFS = $(AIX_DEFS) -DAIX4_3

AM_CPPFLAGS = $(DEBUG_CPPFLAGS)

if AIX
  AM_CPPFLAGS = $(AM_CPPFLAGS) $(AIX_DEFS)
endif
if AIX4_3
  AM_CPPFLAGS = $(AM_CPPFLAGS) $(AIX_4_3_DEFS)
endif

AM_CFLAGS = -Wall -Wno-format
INCLUDES = -I.

noinst_PROGRAMS = jscpucfg
jscpucfg_SOURCES = jscpucfg.c

lib_LTLIBRARIES = @LIBJS@
noinst_LTLIBRARIES = @NOINST_LIBJS@
EXTRA_LTLIBRARIES = libjs.la

noinst_HEADERS = jsarena.h jsarray.h jsatom.h jsbool.h jsbit.h jsclist.h \
jsconfig.h jscntxt.h jscpucfg.h jsdate.h jsdbgapi.h jsdtoa.h jsemit.h jsexn.h \
jsfun.h jsgc.h jshash.h jsinterp.h jslibmath.h jslock.h jsmath.h jsnum.h \
jsobj.h jsopcode.h jsosdep.h jsparse.h jsprvtd.h jsprf.h jsregexp.h jsscan.h \
jsscope.h jsscript.h jsstddef.h jsstr.h jsutil.h jsxdrapi.h prmjtime.h \
resource.h jsopcode.tbl js.msg jsshell.msg

jsincludedir = @JS_INCLUDE_DIR@
jsinclude_HEADERS = @JS_INCLUDE_FILES@
EXTRA_HEADERS = jsapi.h jsautocfg.h jscompat.h jslong.h jsotypes.h jsosdep.h \
jspubtd.h jstypes.h

BUILT_SOURCES = jsautocfg.h

jsautocfg.h: jscpucfg
	./jscpucfg > jsautocfg.h


if JS_PRELINK_FDLIBM

libjs_la_SOURCES = jsapi.c jsarena.c jsarray.c jsatom.c jsbool.c jscntxt.c \
jsdate.c jsdbgapi.c jsdtoa.c jsemit.c jsexn.c jsfun.c jsgc.c jshash.c \
jsinterp.c jslock.c jslog2.c jslong.c jsnum.c jsobj.c jsopcode.c jsparse.c \
jsprf.c jsregexp.c jsscan.c jsscope.c jsscript.c jsstr.c jsutil.c jsxdrapi.c \
prmjtime.c

jsmath.o jsmath.lo: jsmath.c
	$(LTCOMPILE) -c $<

jsmathprelinked.lo: jsmath.lo fdlibm/libfdm.la
	$(LINK) $<

libjs_la_LIBADD = jsmathprelinked.lo

else

libjs_la_SOURCES = jsapi.c jsarena.c jsarray.c jsatom.c jsbool.c jscntxt.c \
jsdate.c jsdbgapi.c jsdtoa.c jsemit.c jsexn.c jsfun.c jsgc.c jshash.c \
jsinterp.c jslock.c jslog2.c jslong.c jsnum.c jsobj.c jsopcode.c jsparse.c \
jsprf.c jsregexp.c jsscan.c jsscope.c jsscript.c jsstr.c jsutil.c jsxdrapi.c \
prmjtime.c jsmath.c

libjs_la_LIBADD = fdlibm/libfdm.la

endif

libjs_la_LDFLAGS = @RPATH_LDFLAG@ -version-info $LIBRARY_VERSION

EXTRA_DIST = jsmath.c

DISTCLEANFILES = jsautocfg.h
