SUBDIRS = fdlibm

#
# Add any debug flags
#
if DEBUG
  DEBUG_CFLAGS = -g
else
  DEBUG_CFLAGS = -DNDEBUG
endif

CFLAGS = -Wall -O2 $(DEBUG_CFLAGS) @JSCFLAGS@

noinst_PROGRAMS = jscpucfg js

jscpucfg_SOURCES = jscpucfg.c

BUILT_SOURCES = jsautocfg.h
jsautocfg.h: jscpucfg
	./jscpucfg > jsautocfg.h


noinst_LTLIBRARIES = libjs.la

libjs_la_LIBADD = ./fdlibm/libfdm.la

libjs_la_SOURCES = \
	jsapi.c	 \
	jsarena.c \
	jsarray.c \
	jsatom.c \
	jsbool.c \
	jscntxt.c \
	jsdate.c \
	jsdbgapi.c \
	jsdtoa.c \
	jsemit.c \
	jsexn.c \
	jsfun.c \
	jsgc.c \
	jshash.c \
	jsinterp.c \
	jslock.c \
	jslog2.c \
	jslong.c \
	jsmath.c \
	jsnum.c \
	jsobj.c \
	jsopcode.c \
	jsparse.c \
	jsprf.c \
	jsregexp.c \
	jsscan.c \
	jsscope.c \
	jsscript.c \
	jsstr.c \
	jsutil.c \
	jsxdrapi.c \
	prmjtime.c

noinst_HEADERS = \
	jsatom.h \
	jsbool.h \
	jsconfig.h \
	jscpucfg.h \
	jscntxt.h \
	jsdate.h \
	jsemit.h \
	jsexn.h \
	jsfun.h \
	jsgc.h \
	jsinterp.h \
	jslibmath.h \
	jslock.h \
	jsmath.h \
	jsnum.h \
	jsobj.h \
	jsopcode.h \
	jsparse.h \
        jsprf.h \
	jsarena.h \
	jsclist.h \
	jsdtoa.h \
	jshash.h \
	jsosdep.h \
	jsprvtd.h \
	jsregexp.h \
	jsscan.h \
	jsscope.h \
	jsscript.h \
	jsstr.h \
        jsutil.h \
	jsxdrapi.h \
	jsdbgapi.h \
	jsaddr.h \
	jsbit.h \
	jscpucfg.h \
	jsstddef.h \
	prmjtime.h \
	resource.h \
	jsopcode.tbl \
	js.msg \
	jsshell.msg

jsincludedir = $(includedir)/vrml97/libjs

jsinclude_HEADERS = \
	jsarray.h \
	jsapi.h \
	jsautocfg.h \
	jscompat.h \
	jslong.h \
	jsotypes.h \
	jsosdep.h \
	jspubtd.h \
	jstypes.h

js_LDADD = libjs.la

js_SOURCES = js.c
