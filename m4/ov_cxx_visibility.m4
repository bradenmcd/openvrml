AC_DEFUN([OV_CXX_VISIBILITY],dnl
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK([if the compiler supports the gcc visibility attribute],
               [ov_cv_visibility],
[AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
                  [class __attribute__((visibility("default"))) test {};],
                  [test t;])],
                  [ov_cv_visibility=yes],
                  [ov_cv_visibility=no])
AC_LANG_POP])
AS_IF([test X$ov_cv_visibility = Xyes], [$1], [$2])
])

