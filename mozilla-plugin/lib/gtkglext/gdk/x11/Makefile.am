## -*- Makefile -*-
## Makefile.am for gtkglext/gdk/x11

INCLUDES = \
	-DG_LOG_DOMAIN=\"GdkGLExt\"	\
	-DGDK_GL_COMPILATION		\
	-DINSIDE_GDK_GL_X11		\
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

gdkglext_x11_public_h_sources = \
	gdkglx.h		\
	gdkglglxext.h

gdkglext_x11_private_h_sources = \
	gdkglconfig-x11.h	\
	gdkgloverlay-x11.h	\
	gdkglcontext-x11.h	\
	gdkglpixmap-x11.h	\
	gdkglwindow-x11.h	\
	gdkglprivate-x11.h

gdkglext_x11_c_sources = \
	gdkglquery-x11.c	\
	gdkglconfig-x11.c	\
	gdkgloverlay-x11.c	\
	gdkglcontext-x11.c	\
	gdkgldrawable-x11.c	\
	gdkglpixmap-x11.c	\
	gdkglwindow-x11.c	\
	gdkglfont-x11.c		\
	gdkglglxext.c

gdkglext_x11_headers = \
	$(gdkglext_x11_public_h_sources)

gdkglext_x11_sources = \
	$(gdkglext_x11_private_h_sources)	\
	$(gdkglext_x11_c_sources)

gdkglextx11includedir = $(includedir)/gtkglext-@GTKGLEXT_API_VERSION@/gdk/x11
noinst_HEADERS = $(gdkglext_x11_headers)

noinst_LTLIBRARIES = libgdkglext-x11.la

libgdkglext_x11_la_SOURCES = $(gdkglext_x11_sources)

