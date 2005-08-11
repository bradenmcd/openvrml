AC_DEFUN([OV_CXX_VISIBILITY],dnl
[AC_REQUIRE([AC_PROG_CXX])
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[class __attribute__((visibility("default"))) test {};], [test t;])],
[$1],[$2])
AC_LANG_POP])
