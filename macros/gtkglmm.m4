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
