dnl Autoconf macro to check for the JDK at a specified path
dnl Copyright (C) 2001  Thomas Flynn
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
dnl OV_PATH_JAVA([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl Adds `--with-java' option to the package. The argument to the option
dnl may be `yes', `no' (the default), or a directory that is the prefix where
dnl the JDK is installed. If the argument is `yes' or a
dnl directory, ACTION-IF-FOUND is executed if the JDK can be located;
dnl otherwise, ACTION-IF-NOT-FOUND is executed.
dnl
dnl If the JDK is found and should be used, OPENVRML_HAVE_JAVA is defined, and the
dnl autoconf substitutions JAVA_CFLAGS and JAVA_LIBS are respectively set to
dnl the compiler and linker flags required to use the library.  If the JDK
dnl is not available--either because `--without-java' has been specified, or
dnl because the library cannot be found--the shell variable `no_java' is set
dnl to `yes'; otherwise it is set to the empty string.
dnl
dnl
AC_DEFUN(OV_PATH_JAVA,
  [
    JAVA_CFLAGS=""
    JAVA_LIBS=""

    AC_MSG_CHECKING([for JDK])

    AC_ARG_WITH(java,
      [  --with-java[=PREFIX]   JDK installed under PREFIX [default=no]
      ],[

        dnl Test to be run if -with or -without has been specified

    	if test "X$with_java" == "Xno"; then
     	  no_java=yes
	  AC_MSG_RESULT([no])
    	else
      	  have_java=yes

      	  if test -n "$with_java"; then
            if test "X$with_java" != "Xyes"; then
              java__Idir="-I${with_java}/include -I${with_java}/include/linux"
              java__Ldir="-L${with_java}/jre/lib/i386/client -ljvm -L${with_java}/jre/lib/i386 -ljava -lverify"
              JAVA_CFLAGS="$java__Idir"
              JAVA_LIBS="$java__Ldir"
            else
	      JAVA_CFLAGS="-I/usr/local/java/include -I/usr/local/java/include/linux"
	      JAVA_LIBS="-L/usr/local/java/jre/lib/i386/client -ljvm -L/usr/local/java/jre/lib/i386 -ljava -lverify"
            fi
          fi

      	  if test "X$have_java" = "Xyes"; then
            AC_MSG_RESULT([yes])
            no_java=""
            AC_DEFINE(OPENVRML_HAVE_JAVA, , [defined if JDK is available and should be used])
            ifelse([$1], , :, [$1])
          else
            AC_MSG_RESULT([no])
            no_java=yes
            ifelse([$2], , :, [$2])
          fi
        fi

    AC_SUBST(JAVA_CFLAGS)
    AC_SUBST(JAVA_LIBS)
  ],[
    dnl Default case (no java support)
    no_java=yes
    AC_MSG_RESULT([no])
    AC_SUBST(JAVA_CFLAGS)
    AC_SUBST(JAVA_LIBS)
  ])
  ]
)