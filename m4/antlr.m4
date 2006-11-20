dnl -*- mode: autoconf -*-
AC_DEFUN([OV_PROG_ANTLR],
[AC_REQUIRE([OV_PROG_JAVA])
AC_ARG_VAR([ANTLR], [antlr parser generator])
AC_CHECK_PROGS([ANTLR], [antlr antlr-java])
if test -z "$ANTLR"; then
  AC_MSG_CHECKING([if antlr is available to the Java runtime])
  cat <<EOF > conftest.g
class TestParser extends Parser;
rule:;
EOF
  AS_IF([test -n "$JAVA" && $JAVA $JAVAFLAGS antlr.Tool conftest.g > /dev/null 2> /dev/null],
        [ANTLR="$JAVA $JAVAFLAGS antlr.Tool"
         AC_MSG_RESULT([yes])],
        [AC_MSG_RESULT([no])])
  rm -rf conftest.g TestParser.java TestParserTokenTypes.java TestParserTokenTypes.txt
fi
if test -n "$1" -a -n "$ANTLR"; then
  AC_MSG_CHECKING([if antlr version is $1])
  version=`$ANTLR 2>&1 | head -n1 | cut -d' ' -f7`
  if test X$version = X$1; then
    AC_MSG_RESULT([yes])
  else
    ANTLR=
    AC_MSG_RESULT([no])
  fi
fi
AC_SUBST([ANTLR])
])
