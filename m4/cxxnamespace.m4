AC_DEFUN(VRML_CXX_BASIC_NAMESPACE,
  [
    AC_MSG_CHECKING([if C++ compiler has basic namespace support])
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    dnl
    dnl Check for basic namespace support
    dnl
    have_basic_namespaces=yes
    AC_TRY_COMPILE(
      [
        namespace foo {
          int x = 69;
        }
      ],
      [
        int y = foo::x;
      ],
      ,
      have_basic_namespaces=no
    )
    
    if test "$have_basic_namespaces" = yes ; then
      AC_MSG_RESULT([yes])
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
  ]
)

dnl
dnl VRML_CXX_GLOBAL_USING([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRML_CXX_GLOBAL_USING,
  [
    AC_MSG_CHECKING([if C++ compiler supports global-scope using declarations])
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    dnl
    dnl Check for global-scope using support
    dnl
    have_global_using=yes
    AC_TRY_COMPILE(
      [
        namespace foo {
          int x = 69;
        }
        using namespace foo;
      ],
      [
        int y = x;
      ],
      ,
      have_global_using=no
    )
    
    if test "$have_global_using" = yes ; then
      AC_MSG_RESULT([yes])
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
  ]
)

dnl
dnl VRML_CXX_BLOCK_USING([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRML_CXX_BLOCK_USING,
  [
    AC_MSG_CHECKING([if C++ compiler supports block-scope using declarations])
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    dnl
    dnl Check for block-scope using support
    dnl
    have_block_using=yes
    AC_TRY_COMPILE(
      [
        namespace foo {
          int x = 69;
        }
      ],
      [
        {
          using namespace foo;
          int y = x;
        }
      ],
      ,
      have_block_using=no
    )
    
    AC_TRY_COMPILE(
      [
        namespace foo {
          int x = 69;
        }
      ],
      [
        {
          using namespace foo;
        }
        int y = x;
      ],
      have_block_using=no
    )
    
    if test "$have_block_using" = yes ; then
      AC_MSG_RESULT([yes])
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
  ]
)

dnl
dnl VRML_CXX_UNNAMED_NAMESPACE([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRML_CXX_UNNAMED_NAMESPACE,
  [
    AC_MSG_CHECKING([if C++ compiler supports unnamed namespaces])
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    
    dnl
    dnl Check for unnamed namespace support
    dnl
    have_unnamed_namespaces=yes
    AC_TRY_COMPILE(
      [
        namespace {
          int x = 69;
        }
      ],
      [
        int y = x;
      ],
      ,
      have_unnamed_namespaces=no
    )
    
    if test "$have_unnamed_namespaces" = yes ; then
      AC_MSG_RESULT([yes])
      ifelse([$1], , :, [$1])
    else
      AC_MSG_RESULT([no])
      ifelse([$2], , :, [$2])
    fi
    
    AC_LANG_RESTORE
  ]
)

dnl
dnl VRML_CXX_NAMESPACE([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(VRML_CXX_NAMESPACE,
  [
    have_namespaces=yes
    VRML_CXX_BASIC_NAMESPACE(, have_namespaces=no)
    VRML_CXX_GLOBAL_USING(, have_namespaces=no)
    VRML_CXX_BLOCK_USING(, have_namespaces=no)
    VRML_CXX_UNNAMED_NAMESPACE(, have_namespaces=no)
    
    if test "$have_namespaces" = yes ; then
      ifelse([$1], , :, [$1])
    else
      ifelse([$2], , :, [$2])
    fi
  ]
)
