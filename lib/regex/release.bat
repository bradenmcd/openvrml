rem create hs_regex distribution.
deltree hs_regex_dist
mkdir hs_regex_dist
rem copy documentation
copy hs_regex.html hs_regex_dist
copy regex.doc hs_regex_dist
copy re_format.doc hs_regex_dist
copy copyright.txt hs_regex_dist
rem copy binaries and test files
copy Release\hs_regex.lib hs_regex_dist
copy Release\hs_regex.dll hs_regex_dist
copy Release\test_hs_regex.exe hs_regex_dist
copy regex_test.txt hs_regex_dist
rem copy sources
copy cclass.h hs_regex_dist
copy cname.h hs_regex_dist
copy regex.h hs_regex_dist
copy regex2.h hs_regex_dist
copy utils.h hs_regex_dist
copy regerror.c hs_regex_dist
copy regexec.c hs_regex_dist
copy regfree.c hs_regex_dist
copy regcomp.c hs_regex_dist
copy engine.c hs_regex_dist
copy regex_test.c hs_regex_dist
copy hs_regex.def hs_regex_dist
rem copy build and maint files
copy test_hs_regex.mak hs_regex_dist
copy release.bat hs_regex_dist
copy install.bat hs_regex_dist
rem end of file copy
\zip\zip -r \Distribute\hs_regex hs_regex_dist
