# Autoconf macro to check for the availability of a C++ hash_map
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
# OV_CXX_HASH_MAP
# ---------------
# By convention, a C++ hash_map uses a hash table to implement the STL pair
# associative container concept. There are two commonly encountered variants of
# the interface: the SGI STL hash_map and the Dinkumware hash_map. This macro
# defines `OPENVRML_HAVE_SGI_HASH_MAP' if the SGI STL hash_map is found, and
# `OPENVRML_HAVE_DINKUM_HASH_MAP' if the Dinkumware hash_map is found.
#
AC_DEFUN(OV_CXX_HASH_MAP,
[AC_LANG_PUSH(C++)
dnl
dnl It's unlikely that someone will be using the Dinkumware library with the
dnl GNU build, so punt for now and assume hash_map is always the SGI STL
dnl version.
dnl
AC_CHECK_HEADER(hash_map,
                [AC_DEFINE(HAVE_SGI_HASH_MAP, ,
                           [defined if the C++ library includes hash_map])])
AC_LANG_POP(C++)
])
