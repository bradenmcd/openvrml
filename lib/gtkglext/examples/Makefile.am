## -*- Makefile -*-
## Makefile.am for gtkglext/examples

SUBDIRS = 
DIST_SUBDIRS = glade

EXTRA_DIST = \
	Makefile.mingw	\
	Makefile.msc	\
	alien.lwo	\
	penguin.lwo	\
	girl.rgb	\
	reflect.rgb

INCLUDES = \
	-I$(top_srcdir) 		\
	-I$(top_builddir)/gdk		\
	-I$(top_builddir)/gtk		\
	$(GTKGLEXT_DEBUG_FLAGS)		\
	$(EXAMPLES_DEP_CFLAGS)		\
	-DG_DISABLE_DEPRECATED		\
	-DGDK_DISABLE_DEPRECATED	\
	-DGDK_PIXBUF_DISABLE_DEPRECATED	\
	-DGTK_DISABLE_DEPRECATED

DEPS = \
	$(top_builddir)/gdk/$(gdkglext_targetlib)	\
	$(top_builddir)/gtk/$(gtkglext_targetlib)

LDADDS = \
	$(top_builddir)/gdk/$(gdkglext_targetlib)	\
	$(top_builddir)/gtk/$(gtkglext_targetlib)	\
	$(GTKGLEXT_DEP_LIBS)

LDADDS_WITH_PANGOFT2 = \
	$(top_builddir)/gdk/$(gdkglext_targetlib)	\
	$(top_builddir)/gtk/$(gtkglext_targetlib)	\
	$(GTKGLEXT_DEP_LIBS_WITH_PANGOFT2)

common_programs = \
	low-level		\
	simple			\
	simple-mixed		\
	pixmap			\
	pixmap-mixed		\
	share-lists		\
	color			\
	font			\
	button			\
	shapes			\
	logo			\
	gears			\
	multiarb		\
	viewlw			\
	rotating-square		\
	coolwave		\
	coolwave2		\
	template		\
	scribble-gl		\
	font-pangoft2		\
	font-pangoft2-tex

x11_programs = \
	simple-pbuffer-sgix	\
	glxinfo

win32_programs = \
	wglinfo

if PLATFORM_WIN32
EXTRA_PROGRAMS = $(common_programs) $(win32_programs)
else
EXTRA_PROGRAMS = $(common_programs) $(x11_programs)
endif

low_level_SOURCES = low-level.c
low_level_DEPENDENCIES = $(DEPS)
low_level_LDADD = $(LDADDS)

simple_SOURCES = simple.c
simple_DEPENDENCIES = $(DEPS)
simple_LDADD = $(LDADDS)

simple_mixed_SOURCES = simple-mixed.c
simple_mixed_DEPENDENCIES = $(DEPS)
simple_mixed_LDADD = $(LDADDS)

pixmap_SOURCES = pixmap.c
pixmap_DEPENDENCIES = $(DEPS)
pixmap_LDADD = $(LDADDS)

pixmap_mixed_SOURCES = pixmap-mixed.c
pixmap_mixed_DEPENDENCIES = $(DEPS)
pixmap_mixed_LDADD = $(LDADDS)

share_lists_SOURCES = share-lists.c
share_lists_DEPENDENCIES = $(DEPS)
share_lists_LDADD = $(LDADDS)

color_SOURCES = color.c
color_DEPENDENCIES = $(DEPS)
color_LDADD = $(LDADDS)

font_SOURCES = font.c
font_DEPENDENCIES = $(DEPS)
font_LDADD = $(LDADDS)

button_SOURCES = button.c
button_DEPENDENCIES = $(DEPS)
button_LDADD = $(LDADDS)

shapes_SOURCES = trackball.h trackball.c shapes.c
shapes_DEPENDENCIES = $(DEPS)
shapes_LDADD = $(LDADDS)

logo_SOURCES = trackball.h trackball.c logo-model.h logo-model.c logo.c
EXTRA_logo_SOURCES = logo-g.c logo-t.c logo-k.c
logo_DEPENDENCIES = $(DEPS)
logo_LDADD = $(LDADDS)

gears_SOURCES = gears.c
gears_DEPENDENCIES = $(DEPS)
gears_LDADD = $(LDADDS)

multiarb_SOURCES = readtex.h readtex.c multiarb.c
multiarb_DEPENDENCIES = $(DEPS)
multiarb_LDADD = $(LDADDS)

viewlw_SOURCES = viewlw.c lw.h lw.c trackball.h trackball.c
viewlw_DEPENDENCIES = $(DEPS)
viewlw_LDADD = $(LDADDS)

rotating_square_SOURCES = rotating-square.c
rotating_square_DEPENDENCIES = $(DEPS)
rotating_square_LDADD = $(LDADDS)

coolwave_SOURCES = coolwave.c
coolwave_DEPENDENCIES = $(DEPS)
coolwave_LDADD = $(LDADDS)

coolwave2_SOURCES = coolwave2.c
coolwave2_DEPENDENCIES = $(DEPS)
coolwave2_LDADD = $(LDADDS)

template_SOURCES = template.c
template_DEPENDENCIES = $(DEPS)
template_LDADD = $(LDADDS)

scribble_gl_SOURCES = scribble-gl.c
scribble_gl_DEPENDENCIES = $(DEPS)
scribble_gl_LDADD = $(LDADDS)

font_pangoft2_SOURCES = font-pangoft2.c
font_pangoft2_DEPENDENCIES = $(DEPS)
font_pangoft2_LDADD = $(LDADDS_WITH_PANGOFT2)

font_pangoft2_tex_SOURCES = font-pangoft2-tex.c
font_pangoft2_tex_DEPENDENCIES = $(DEPS)
font_pangoft2_tex_LDADD = $(LDADDS_WITH_PANGOFT2)

simple_pbuffer_sgix_SOURCES = simple-pbuffer-sgix.c
simple_pbuffer_sgix_DEPENDENCIES = $(DEPS)
simple_pbuffer_sgix_LDADD = $(LDADDS)

glxinfo_SOURCES = glxinfo.c
glxinfo_DEPENDENCIES = $(DEPS)
glxinfo_LDADD = $(GL_LIBS) $(WINDOWING_LIBS) $(MATH_LIB)

wglinfo_SOURCES = wglinfo.c
wglinfo_DEPENDENCIES = $(DEPS)
wglinfo_LDADD = $(GL_LIBS) $(WINDOWING_LIBS)


all-local: $(EXTRA_PROGRAMS)

clean-local:
	-rm -f $(EXTRA_PROGRAMS)

