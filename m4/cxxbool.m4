AC_DEFUN(VRML_CXX_BOOL,
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
