dnl Autoconf macro to check for the availability of a C++ hash_map
dnl Copyright (C) 2001  Braden McDaniel
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

dnl
dnl OV_CXX_HASH_MAP
dnl
AC_DEFUN(OV_CXX_HASH_MAP,
  [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    AC_CHECK_HEADER(hash_map,
      [
        dnl
        dnl There are two variants of the hash_map interface: the STL
        dnl version and the Dinkumware version. Both support the STL
        dnl concept of a pair associative container, but the templates
        dnl take different arguments for instantiation. We ought to
        dnl differentiate between the two here, but since it's pretty
        dnl unlikely that someone will be using the Dinkumware library
        dnl with the GNU build, punt for now and assume hash_map is
        dnl always the STL version.
        dnl
        AC_DEFINE(HAVE_STL_HASH_MAP, , [defined if the C++ library includes hash_map])
      ]
    )
    
    AC_LANG_RESTORE
  ]
)
