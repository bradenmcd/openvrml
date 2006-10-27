## -*- Makefile -*-
## Makefile.am for gtkglext/docs/reference/gtkglext

GTKDOC_CFLAGS = \
	-I$(top_srcdir)			\
	-I$(top_srcdir)/gdk		\
	-I$(top_srcdir)/gtk		\
	-I$(top_builddir)		\
	-I$(top_builddir)/gdk		\
	-I$(top_builddir)/gtk		\
	$(GTKGLEXT_DEBUG_FLAGS)		\
	$(GTKGLEXT_DEP_CFLAGS)

GTKDOC_LIBS = \
	$(top_builddir)/gdk/$(gdkglext_targetlib)	\
	$(top_builddir)/gtk/$(gtkglext_targetlib)

# The name of the module, e.g. 'glib'.
DOC_MODULE = gtkglext

# The top-level SGML file. Change it if you want.
DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.sgml

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting functions and macros.
DOC_SOURCE_DIR = ../../..

# Extra options to pass to gtkdoc-scanobj or gtkdoc-scangobj.
SCANOBJ_OPTIONS =

# Extra options to supply to gtkdoc-scan.
SCAN_OPTIONS =

# Extra options to supply to gtkdoc-mkdb.
MKDB_OPTIONS = --sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-fixref.
FIXXREF_OPTIONS =

# Used for dependencies.
HFILE_GLOB = $(top_srcdir)/gdk/*.h $(top_srcdir)/gdk/x11/gdkglx.h $(top_srcdir)/gtk/*.h
CFILE_GLOB = $(top_srcdir)/gdk/*.c $(top_srcdir)/gdk/x11/*.c $(top_srcdir)/gtk/*.c

# Header files to ignore when scanning.
IGNORE_HFILES = \
	config.h		\
	docs			\
	examples		\
	glext			\
	x11			\
	win32			\
	gdkglext-config.h	\
	gdkglprivate.h		\
	gdkgldebug.h		\
	gdkglglext.h		\
	gtkglprivate.h		\
	gtkgldebug.h

# Extra files to add when scanning (relative to $srcdir)
EXTRA_HFILES = \
	../../../gdk/x11/gdkglx.h

# Images to copy into HTML directory.
HTML_IMAGES =

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
content_files = \
	$(top_srcdir)/docs/reference/gtkglext/version.xml	\
	building.sgml	\
	running.sgml

# Other files to distribute.
extra_files = \
	version.xml.in

$(top_srcdir)/docs/reference/gtkglext/version.xml:
	cp $(top_builddir)/docs/reference/gtkglext/version.xml $@

include $(top_srcdir)/gtk-doc.make

#
# We don't want to install anything here; so override install-data-am to
# keep the automake-generated one from pulling in install-data-local
# (defined in gtk-doc.make).
#
install-data-am:
