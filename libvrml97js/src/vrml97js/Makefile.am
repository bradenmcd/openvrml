SUBDIRS = fdlibm

if DEBUG
DEBUG_CFLAGS = -g -DDEBUG
else
DEBUG_CFLAGS = -DNDEBUG
endif

if FREEBSD
DEFS += -DFREEBSD
endif

CFLAGS = -Wall -Wno-format -O2 $(DEBUG_CFLAGS) -DXP_UNIX
LDFLAGS = -version-info @LIBVRML97JS_SO_VERSION@

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
	jsarena.h \
	jsarray.h \
	jsatom.h \
	jsbool.h \
	jsbit.h \
	jsclist.h \
	jsconfig.h \
	jscntxt.h \
	jscpucfg.h \
	jsdate.h \
	jsdbgapi.h \
	jsdtoa.h \
	jsemit.h \
	jsexn.h \
	jsfun.h \
	jsgc.h \
	jshash.h \
	jsinterp.h \
	jslibmath.h \
	jslock.h \
	jsmath.h \
	jsnum.h \
	jsobj.h \
	jsopcode.h \
	jsosdep.h \
	jsparse.h \
	jsprvtd.h \
        jsprf.h \
	jsregexp.h \
	jsscan.h \
	jsscope.h \
	jsscript.h \
	jsstddef.h \
	jsstr.h \
        jsutil.h \
	jsxdrapi.h \
	prmjtime.h \
	resource.h \
	jsopcode.tbl \
	js.msg \
	jsshell.msg

jsincludedir = $(includedir)/vrml97js

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
	$(LIBTOOL) $(CC) $(CFLAGS) $(DEFS) -c $<

jsmathprelinked.lo: jsmath.lo jsmath.o
	$(LIBTOOL) --mode=link $(LD) $(reload_flag) -o $@ $< ./fdlibm/libfdm.la

libvrml97js_la_LIBADD = jsmathprelinked.lo

CLEANFILES = jsmath.o jsmath.lo jsautocfg.h
