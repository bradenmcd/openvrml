dnl Autoconf macro to check for the availability of Gtkgl--
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

AC_DEFUN(OV_PATH_GTKGLMM,
  [
    AC_REQUIRE([AM_PATH_GTKMM])
    AC_REQUIRE([OV_PATH_GTKGL])
    
    AC_MSG_CHECKING([for GtkGLArea--])
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    ac_save_CXXFLAGS="${CXXFLAGS}"
    ac_save_LIBS="${LIBS}"
    CXXFLAGS="${GTKGL_CFLAGS} ${GTKMM_CFLAGS}"
    LIBS="-lgtkglmm ${GTKGL_LIBS} ${GTKMM_LIBS}"
    AC_TRY_LINK(
      [
        # include <gtkgl--/glarea.h>
      ],
      [
        Gtk::GLArea glArea;
      ],
      have_gtkglmm=yes,
      have_gtkglmm=no
    )
    CXXFLAGS="${ac_save_CXXFLAGS}"
    LIBS="${ac_save_LIBS}"
    AC_LANG_RESTORE
    
    if test "X${have_gtkglmm}" = Xyes; then
      AC_MSG_RESULT([yes])
      GTKGLMM_CXXFLAGS="${GTKGL_CFLAGS} ${GTKMM_CFLAGS}"
      GTKGLMM_LIBS="-lgtkglmm ${GTKGL_LIBS} ${GTKMM_LIBS}"
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      GTKGLMM_CXXFLAGS=""
      GTKGLMM_LIBS=""
      ifelse([$2], , :, [$2])
    fi
    
    AC_SUBST(GTKGLMM_CXXFLAGS)
    AC_SUBST(GTKGLMM_LIBS)
  ]
)
