dnl Autoconf macro to check for the availability of the C++ auto_ptr
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
dnl OV_CXX_AUTO_PTR(ACTION-IF_FOUND, [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN(OV_CXX_AUTO_PTR,
  [
    AC_MSG_CHECKING([if C++ compiler has auto_ptr])
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    have_auto_ptr=yes
    AC_TRY_COMPILE(
      [ #include<memory> ],
      [ std::auto_ptr<int> p(new int(0)); ],
      ,
      have_auto_ptr=no
    )
    
    AC_LANG_RESTORE
    
    if test "X$have_auto_ptr" = "Xyes"; then
      AC_MSG_RESULT([yes])
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      ifelse([$2], , :, [$2])
    fi
  ]
)
