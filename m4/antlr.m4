AC_DEFUN([OV_PROG_ANTLR],
[AC_REQUIRE([OV_PROG_JAVA])
AC_ARG_VAR([ANTLR])
AC_PATH_PROG([ANTLR], [antlr])
if test -z "$ANTLR"; then
  cat <<EOF > conftest.g
class TestParser extends Parser;
rule:;
EOF
  AS_IF([$JAVA antlr.Tool conftest.g],
        [ANTLR="$JAVA antlr.Tool"],
        [])
  rm -rf conftest.g TestParser.java TestParserTokenTypes.java TestParserTokenTypes.txt
fi
])
