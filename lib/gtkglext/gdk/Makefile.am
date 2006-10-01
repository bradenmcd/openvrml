## -*- Makefile -*-
## Makefile.am for gtkglext/gdk

SUBDIRS = glext $(gdktarget)
DIST_SUBDIRS = glext x11 win32

EXTRA_DIST = \
	gdkglversion.h.in	\
	gdkglext.def		\
	gdkglext-gtk20.def

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

if OS_WIN32
if MULTIHEAD_SUPPORT
gdkglext_def = gdkglext.def
else
gdkglext_def = gdkglext-gtk20.def
endif
gdkglext_win32_symbols = -export-symbols $(gdkglext_def)
endif

if MS_LIB_AVAILABLE
noinst_DATA = gdkglext-win32-@GTKGLEXT_API_VERSION@.lib

gdkglext-win32-@GTKGLEXT_API_VERSION@.lib: libgdkglext-win32-@GTKGLEXT_API_VERSION@.la $(gdkglext_def)
	lib -name:libgdkglext-win32-@GTKGLEXT_API_VERSION@-@LT_CURRENT_MINUS_AGE@.dll -def:$(gdkglext_def) -out:$@

install-ms-lib:
	$(INSTALL) gdkglext-win32-@GTKGLEXT_API_VERSION@.lib $(DESTDIR)$(libdir)

uninstall-ms-lib:
	-rm $(DESTDIR)$(libdir)/gdkglext-win32-@GTKGLEXT_API_VERSION@.lib
else
install-ms-lib:
uninstall-ms-lib:
endif

common_includes = \
	-DG_LOG_DOMAIN=\"GdkGLExt\"	\
	-DGDK_GL_COMPILATION		\
	-I$(top_srcdir)			\
	$(GTKGLEXT_DEBUG_FLAGS)		\
	$(GDKGLEXT_DEP_CFLAGS)		\
	-DG_DISABLE_DEPRECATED		\
	-DGDK_DISABLE_DEPRECATED	\
	-DGDK_PIXBUF_DISABLE_DEPRECATED

common_ldflags = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) 	\
	-export-dynamic 					\
	-rpath $(libdir) 					\
	$(no_undefined)						\
	@LIBTOOL_EXPORT_OPTIONS@				\
	$(gdkglext_win32_symbols)

common_libadd = \
	$(GDKGLEXT_DEP_LIBS)

#
# setup source file variables
#
gdkglext_configure_generated_public_h_source = gdkglversion.h

gdkglext_public_h_sources = \
	gdkgl.h			\
	gdkgldefs.h		\
	gdkgltokens.h		\
	gdkgltypes.h		\
	gdkglinit.h		\
	gdkglquery.h		\
	gdkglconfig.h		\
	gdkglcontext.h		\
	gdkgldrawable.h		\
	gdkglpixmap.h		\
	gdkglwindow.h		\
	gdkglfont.h		\
	gdkglshapes.h		\
	gdkglglext.h

gdkglext_private_h_sources = \
	gdkglprivate.h

gdkglext_built_public_h_sources = \
	gdkglenumtypes.h

gdkglext_c_sources = \
	gdkglversion.c		\
	gdkglinit.c		\
	gdkglquery.c		\
	gdkglconfig.c		\
	gdkglcontext.c		\
	gdkgldrawable.c		\
	gdkglpixmap.c		\
	gdkglwindow.c		\
	gdkglshapes.c		\
	gdkglglext.c

gdkglext_built_c_sources = \
	gdkglenumtypes.c

gdkglext_headers = \
	$(gdkglext_public_h_sources)			\
	$(gdkglext_built_public_h_sources)		\
	$(gdkglext_configure_generated_public_h_source)	\
	gdkgldebug.h

gdkglext_sources = \
	$(gdkglext_private_h_sources)		\
	$(gdkglext_c_sources)			\
	$(gdkglext_built_c_sources)

gdkglext_built_sources = \
	$(gdkglext_built_public_h_sources)	\
	$(gdkglext_built_c_sources)

stamp_files = \
	stamp-gdkglenumtypes-h

#
# setup GdkGLExt sources and their dependancies
#

gdkglextincludedir = $(includedir)/gtkglext-@GTKGLEXT_API_VERSION@/gdk
noinst_HEADERS = $(gdkglext_headers)

INCLUDES = $(common_includes)

noinst_LTLIBRARIES = $(gdkglext_targetlib)

EXTRA_LTLIBRARIES = \
	libgdkglext-x11-@API_MJ@.@API_MI@.la	\
	libgdkglext-win32-@API_MJ@.@API_MI@.la

libgdkglext_x11_@API_MJ@_@API_MI@_la_SOURCES = $(gdkglext_sources)
libgdkglext_x11_@API_MJ@_@API_MI@_la_LDFLAGS = $(common_ldflags)
libgdkglext_x11_@API_MJ@_@API_MI@_la_LIBADD = x11/libgdkglext-x11.la $(common_libadd)

libgdkglext_win32_@API_MJ@_@API_MI@_la_SOURCES = $(gdkglext_sources)
libgdkglext_win32_@API_MJ@_@API_MI@_la_LDFLAGS = $(common_ldflags)
libgdkglext_win32_@API_MJ@_@API_MI@_la_LIBADD = win32/libgdkglext-win32.la $(common_libadd)
libgdkglext_win32_@API_MJ@_@API_MI@_la_DEPENDENCIES = $(gdkglext_def)

BUILT_SOURCES = \
	@REBUILD@			\
	$(gdkglext_built_sources)

DISTCLEANFILES = $(stamp_files) $(gdkglext_built_sources)

# Generate built header without using automake BUILT_SOURCES
$(libgdkglext_x11_@API_MJ@_@API_MI@_la_OBJECTS) $(libgdkglext_win32_@API_MJ@_@API_MI@_la_OBJECTS): $(gdkglext_built_public_h_sources)

gdkglenumtypes.h: stamp-gdkglenumtypes-h
	@true
stamp-gdkglenumtypes-h: @REBUILD@ $(gdkglext_public_h_sources) Makefile
	builddir=`pwd`; ( cd $(srcdir) && glib-mkenums \
			--fhead "#ifndef __GDK_GL_ENUM_TYPES_H__\n#define __GDK_GL_ENUM_TYPES_H__\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void);\n#define GDK_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
			--ftail "G_END_DECLS\n\n#endif /* __GDK_GL_ENUM_TYPES_H__ */" \
		$(gdkglext_public_h_sources) $${builddir}/$(gdkglext_configure_generated_public_h_source) ) >> xgen-geth \
	&& (cmp -s xgen-geth gdkglenumtypes.h || cp xgen-geth gdkglenumtypes.h ) \
	&& rm -f xgen-geth \
	&& echo timestamp > $(@F)
gdkglenumtypes.c: @REBUILD@ $(gdkglext_public_h_sources) Makefile
	builddir=`pwd`; ( cd $(srcdir) && glib-mkenums \
			--fhead "#include \"gdkgl.h\"" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(gdkglext_public_h_sources) $${builddir}/$(gdkglext_configure_generated_public_h_source) ) > xgen-getc \
	&& cp xgen-getc $@  \
	&& rm -f xgen-getc

#
# Rule to install gdkglext-config.h header file
#
configexecincludedir = $(libdir)/gtkglext-@GTKGLEXT_API_VERSION@/include
#configexecinclude_DATA = gdkglext-config.h

gdkglext-config.h: stamp-gdkglext-config-h
	@if test -f gdkglext-config.h; then :; \
	else rm -f stamp-gdkglext-config-h; $(MAKE) stamp-gdkglext-config-h; fi

stamp-gdkglext-config-h: $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status gdk/gdkglext-config.h
	echo timestamp > stamp-gdkglext-config-h

DISTCLEANFILES += gdkglext-config.h stamp-gdkglext-config-h
