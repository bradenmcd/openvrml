dnl Autoconf macro to check for the availability of the C++ bool keyword
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

AC_DEFUN(OV_CXX_BOOL,
  [
    AC_MSG_CHECKING([if C++ compiler supports bool])
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    AC_TRY_COMPILE(
      ,
      [
        bool x = true;
        bool y = false;
      ],
      have_bool=yes,
      have_bool=no
    )
    
    AC_LANG_RESTORE
    
    if test "X$have_bool" = "Xyes" ; then
      AC_MSG_RESULT([yes])
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      ifelse([$2], , :, [$2])
    fi
  ]
)
