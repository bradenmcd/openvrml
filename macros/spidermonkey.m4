# Autoconf macro to check for SpiderMonkey
# Copyright (C) 2001  Braden McDaniel
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#
# OV_CHECK_SPIDERMONKEY
# ---------------------
# Check for SpiderMonkey, the Mozilla JavaScript engine. If SpiderMonkey is
# found, the required compiler and linker flags are output in the variables
# `JS_CFLAGS' and `JS_LIBS', respectively; and the shell variable `no_js' is
# set to the empty string. If SpiderMonkey is not found, `no_js' is set to
# `yes'.
#
AC_DEFUN(OV_CHECK_SPIDERMONKEY,
[JS_CFLAGS="-DXP_UNIX"
no_js="yes"
AC_LANG_PUSH(C)
ov_save_LIBS="${LIBS}"
LIBS=""
AC_SEARCH_LIBS(JS_GetVersion, js mozjs,
               [JS_LIBS="${LIBS}"
                ov_save_CPPFLAGS="${CPPFLAGS}"
                CPPFLAGS="${JS_CFLAGS} ${CPPFLAGS}"
                AC_CHECK_HEADER(jsapi.h,
                                [no_js=""
                                 AC_DEFINE(OPENVRML_JSAPI_H, [<jsapi.h>],
                                           [Include jsapi.h (primary header for SpiderMonkey)])],
                                [AC_CHECK_HEADER(mozilla/jsapi.h,
                                                [no_js=""
                                                 AC_DEFINE(OPENVRML_JSAPI_H,
                                                           [<mozilla/jsapi.h>],
                                                           [Include mozilla/jsapi.h (primary header for SpiderMonkey)])])])
                CPPFLAGS="${ov_save_CPPFLAGS}"])
LIBS="${ov_save_LIBS}"
AC_LANG_POP(C)

if test "X${no_js}" = "Xyes"; then
  JS_CFLAGS=""
  JS_LIBS=""
fi
AC_SUBST(JS_CFLAGS)
AC_SUBST(JS_LIBS)
])
