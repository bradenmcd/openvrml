SUBDIRS = fdlibm

if DEBUG
DEBUG_CFLAGS = -g -DDEBUG
else
DEBUG_CFLAGS = -DNDEBUG
endif

CFLAGS = -Wall -Wno-format -O2 $(DEBUG_CFLAGS) -DXP_UNIX

noinst_PROGRAMS = jscpucfg

jscpucfg_SOURCES = jscpucfg.c

lib_LTLIBRARIES = libvrml97js.la

libvrml97js_la_SOURCES = \
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
	jsarray.h \
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
	jsxdrapi.h \
	jsdbgapi.h \
	jsbit.h \
	jscpucfg.h \
	jsstddef.h \
	prmjtime.h \
	resource.h \
	jsopcode.tbl \
	js.msg \
	jsshell.msg

jsincludedir = $(includedir)

jsinclude_HEADERS = \
	jsapi.h \
	jsautocfg.h \
	jscompat.h \
	jslong.h \
	jsotypes.h \
	jsosdep.h \
	jspubtd.h \
	jstypes.h

BUILT_SOURCES = jsautocfg.h

jsautocfg.h: jscpucfg
	./jscpucfg > jsautocfg.h

EXTRA_DIST = jsmath.c

jsmath.o jsmath.lo: jsmath.c
	$(top_srcdir)/libtool $(CC) $(CFLAGS) $(DEFS) -c $<

jsmathprelinked.lo: jsmath.lo jsmath.o
	$(top_srcdir)/libtool --mode=link $(LD) $(reload_flag) -o $@ $< ./fdlibm/libfdm.la

libvrml97js_la_LIBADD = jsmathprelinked.lo

CLEANFILES = jsmath.o jsmath.lo jsautocfg.h
