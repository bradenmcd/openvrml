AC_DEFUN(OV_PATH_GTKGL,
  [
    AC_REQUIRE([AM_PATH_GTK])
    AC_REQUIRE([OV_PATH_GL])
    
    AC_LANG_SAVE
    AC_LANG_C
    AC_CHECK_LIB(gtkgl, gdk_gl_query, have_gtkgl=yes, have_gtkgl=no, ${GTK_LIBS} ${GL_LIBS})
    AC_LANG_RESTORE
    
    if test "X${have_gtkgl}" = Xyes; then
      GTKGL_CFLAGS="${GTK_CFLAGS} ${GL_CFLAGS}"
      GTKGL_LIBS="-lgtkgl ${GTK_LIBS} ${GL_LIBS}"
      ifelse([$1], , :, [$1])
    else
      GTKGL_CFLAGS=""
      GTKGL_LIBS=""
      ifelse([$2], , :, [$2])
    fi
    
    AC_SUBST(GTKGL_CFLAGS)
    AC_SUBST(GTKGL_LIBS)
  ]
)
