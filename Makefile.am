# Makefile.am
#
# toplevel Makefile.am
#
#
#
# cks Fri Dec 10 00:34:35 CST 1999
#


# normally, automake requires a README, ChangeLog, and a
# bunch of other stuff. we'll have toplevel versions of
# those eventually, but for now we can do without them
# "foreign" says not to require that stuff...
#
AUTOMAKE_OPTIONS=foreign

# we don't have the rules to build gtklookat, so don't
# build it by default. we should really have configure.in
# check for the proper libs and enable the build
# conditionally.
#
SUBDIRS = libvrml97core libvrml97gl lookat
