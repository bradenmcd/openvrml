## -*- Makefile -*-
## Makefile.am for gtkglext/gdk/win32

INCLUDES = \
	-DG_LOG_DOMAIN=\"GdkGLExt\"	\
	-DGDK_GL_COMPILATION		\
	-DINSIDE_GDK_GL_WIN32		\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/gdk		\
	-I$(top_builddir)/gdk		\
	$(GTKGLEXT_DEBUG_FLAGS)		\
	$(GDKGLEXT_DEP_CFLAGS)		\
	$(GDKGLEXT_WIN_EXTRA_DEFS)	\
	-DG_DISABLE_DEPRECATED		\
	-DGDK_DISABLE_DEPRECATED	\
	-DGDK_PIXBUF_DISABLE_DEPRECATED

LDADDS = \
	$(GDKGLEXT_DEP_LIBS)

gdkglext_win32_public_h_sources = \
	gdkglwin32.h		\
	gdkglwglext.h

gdkglext_win32_private_h_sources = \
	gdkglconfig-win32.h	\
	gdkglcontext-win32.h	\
	gdkglpixmap-win32.h	\
	gdkglwindow-win32.h	\
	gdkglprivate-win32.h

gdkglext_win32_c_sources = \
	gdkglquery-win32.c	\
	gdkglconfig-win32.c	\
	gdkglcontext-win32.c	\
	gdkgldrawable-win32.c	\
	gdkglpixmap-win32.c	\
	gdkglwindow-win32.c	\
	gdkglfont-win32.c	\
	gdkglwglext.c

gdkglext_win32_headers = \
	$(gdkglext_win32_public_h_sources)

gdkglext_win32_sources = \
	$(gdkglext_win32_private_h_sources)	\
	$(gdkglext_win32_c_sources)

gdkglextwin32includedir = $(includedir)/gtkglext-@GTKGLEXT_API_VERSION@/gdk/win32
noinst_HEADERS = $(gdkglext_win32_headers)

noinst_LTLIBRARIES = libgdkglext-win32.la

libgdkglext_win32_la_SOURCES = $(gdkglext_win32_sources)

