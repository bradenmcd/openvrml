SUBDIRS = script

lib_LTLIBRARIES = libopenvrml/libopenvrml.la
if ENABLE_GL_RENDERER
lib_LTLIBRARIES += libopenvrml-gl/libopenvrml-gl.la
endif

if ENABLE_XEMBED
libexec_PROGRAMS = openvrml-xembed/openvrml-xembed
endif

if ENABLE_PLAYER
bin_PROGRAMS = openvrml-player/openvrml-player
endif

pkginclude_HEADERS = \
        libopenvrml/openvrml-config.h \
        libopenvrml/openvrml-common.h

if ENABLE_GL_RENDERER
pkginclude_HEADERS += \
        libopenvrml-gl/openvrml-gl-config.h \
        libopenvrml-gl/openvrml-gl-common.h
endif

openvrml_includedir = $(pkgincludedir)/openvrml
openvrml_include_HEADERS = \
        libopenvrml/openvrml/basetypes.h \
        libopenvrml/openvrml/vrml97_grammar.h \
        libopenvrml/openvrml/x3d_vrml_grammar.h \
        libopenvrml/openvrml/read_write_mutex.h \
        libopenvrml/openvrml/field_value.h \
        libopenvrml/openvrml/event.h \
        libopenvrml/openvrml/exposedfield.h \
        libopenvrml/openvrml/scope.h \
        libopenvrml/openvrml/node.h \
        libopenvrml/openvrml/bounding_volume.h \
        libopenvrml/openvrml/script.h \
        libopenvrml/openvrml/browser.h \
        libopenvrml/openvrml/viewer.h \
        libopenvrml/openvrml/rendering_context.h \
        libopenvrml/openvrml/frustum.h \
        libopenvrml/openvrml/node_impl_util.h \
        libopenvrml/openvrml/vrml97node.h \
        libopenvrml/openvrml/x3d_core.h \
        libopenvrml/openvrml/x3d_networking.h \
        libopenvrml/openvrml/x3d_grouping.h \
        libopenvrml/openvrml/x3d_rendering.h \
        libopenvrml/openvrml/x3d_shape.h \
        libopenvrml/openvrml/x3d_geometry2d.h \
        libopenvrml/openvrml/x3d_texturing.h \
        libopenvrml/openvrml/x3d_interpolation.h \
        libopenvrml/openvrml/x3d_key_device_sensor.h \
        libopenvrml/openvrml/x3d_event_utilities.h \
        libopenvrml/openvrml/x3d_dis.h \
        libopenvrml/openvrml/x3d_environmental_effects.h \
        libopenvrml/openvrml/x3d_geospatial.h \
        libopenvrml/openvrml/x3d_hanim.h \
        libopenvrml/openvrml/x3d_nurbs.h \
        libopenvrml/openvrml/x3d_cad_geometry.h

if ENABLE_GL_RENDERER
openvrml_gl_includedir = $(pkgincludedir)/openvrml/gl
openvrml_gl_include_HEADERS =  libopenvrml-gl/openvrml/gl/viewer.h
endif

noinst_HEADERS = \
        libopenvrml/private.h \
        openvrml-xembed/bounded_buffer.h \
        openvrml-xembed/command_istream.h \
        openvrml-xembed/plugin_streambuf.h \
        openvrml-xembed/request_channel.h \
        openvrml-xembed/gtkvrmlbrowser.h \
        openvrml-player/filechooserdialog.h

LIBOPENVRML_LIBRARY_VERSION =    8:6:0
LIBOPENVRML_GL_LIBRARY_VERSION = 7:2:0
#                                | | |
#                         +------+ | +---+
#                         |        |     |
#                      current:revision:age
#                         |        |     |
#                         |        |     +- Increment if interfaces have been
#                         |        |        added.  Set to zero if interfaces
#                         |        |        have been removed or changed.
#                         |        +- Increment if source code has changed
#                         |           Set to zero if current is incremented
#                         +- Increment if interfaces have been added, removed
#                            or changed

libopenvrml_libopenvrml_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(srcdir)/node \
        -I$(srcdir)/node/vrml97 \
        -DOPENVRML_LIBDIR_=\"$(libdir)\" \
        -DOPENVRML_PKGDATADIR_=\"$(pkgdatadir)\" \
        -DOPENVRML_PKGLIBDIR_=\"$(pkglibdir)\" \
        -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS \
        -DBOOST_MPL_LIMIT_VECTOR_SIZE=30 \
        -DBOOST_SPIRIT_THREADSAFE \
        -DBOOST_SPIRIT_CLOSURE_LIMIT=6 \
        -DPHOENIX_LIMIT=6
libopenvrml_libopenvrml_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS) \
        $(XML_CFLAGS) \
        $(FONTCONFIG_CFLAGS) \
        $(FREETYPE_CFLAGS)

libopenvrml_libopenvrml_la_SOURCES = \
        node/vrml97/image_stream_listener.h \
        node/vrml97/image_stream_listener.cpp \
        node/vrml97/abstract_light.h \
        node/vrml97/abstract_texture.h \
        node/vrml97/abstract_indexed_set.h \
        node/vrml97/grouping_node_base.h \
        node/vrml97/anchor.h \
        node/vrml97/anchor.cpp \
        node/vrml97/appearance.h \
        node/vrml97/appearance.cpp \
        node/vrml97/audio_clip.h \
        node/vrml97/audio_clip.cpp \
        node/vrml97/background.h \
        node/vrml97/background.cpp \
        node/vrml97/billboard.h \
        node/vrml97/billboard.cpp \
        node/vrml97/box.h \
        node/vrml97/box.cpp \
        node/vrml97/collision.h \
        node/vrml97/collision.cpp \
        node/vrml97/color.h \
        node/vrml97/color.cpp \
        node/vrml97/color_interpolator.h \
        node/vrml97/color_interpolator.cpp \
        node/vrml97/cone.h \
        node/vrml97/cone.cpp \
        node/vrml97/coordinate.h \
        node/vrml97/coordinate.cpp \
        node/vrml97/coordinate_interpolator.h \
        node/vrml97/coordinate_interpolator.cpp \
        node/vrml97/cylinder.h \
        node/vrml97/cylinder.cpp \
        node/vrml97/cylinder_sensor.h \
        node/vrml97/cylinder_sensor.cpp \
        node/vrml97/directional_light.h \
        node/vrml97/directional_light.cpp \
        node/vrml97/elevation_grid.h \
        node/vrml97/elevation_grid.cpp \
        node/vrml97/extrusion.h \
        node/vrml97/extrusion.cpp \
        node/vrml97/fog.h \
        node/vrml97/fog.cpp \
        node/vrml97/font_style.h \
        node/vrml97/font_style.cpp \
        node/vrml97/group.h \
        node/vrml97/group.cpp \
        node/vrml97/image_texture.h \
        node/vrml97/image_texture.cpp \
        node/vrml97/indexed_face_set.h \
        node/vrml97/indexed_face_set.cpp \
        node/vrml97/indexed_line_set.h \
        node/vrml97/indexed_line_set.cpp \
        node/vrml97/inline.h \
        node/vrml97/inline.cpp \
        node/vrml97/lod.h \
        node/vrml97/lod.cpp \
        node/vrml97/material.h \
        node/vrml97/material.cpp \
        node/vrml97/movie_texture.h \
        node/vrml97/movie_texture.cpp \
        node/vrml97/navigation_info.h \
        node/vrml97/navigation_info.cpp \
        node/vrml97/normal.h \
        node/vrml97/normal.cpp \
        node/vrml97/normal_interpolator.h \
        node/vrml97/normal_interpolator.cpp \
        node/vrml97/orientation_interpolator.h \
        node/vrml97/orientation_interpolator.cpp \
        node/vrml97/pixel_texture.h \
        node/vrml97/pixel_texture.cpp \
        node/vrml97/plane_sensor.h \
        node/vrml97/plane_sensor.cpp \
        node/vrml97/point_light.h \
        node/vrml97/point_light.cpp \
        node/vrml97/point_set.h \
        node/vrml97/point_set.cpp \
        node/vrml97/position_interpolator.h \
        node/vrml97/position_interpolator.cpp \
        node/vrml97/proximity_sensor.h \
        node/vrml97/proximity_sensor.cpp \
        node/vrml97/scalar_interpolator.h \
        node/vrml97/scalar_interpolator.cpp \
        node/vrml97/shape.h \
        node/vrml97/shape.cpp \
        node/vrml97/sound.h \
        node/vrml97/sound.cpp \
        node/vrml97/sphere.h \
        node/vrml97/sphere.cpp \
        node/vrml97/sphere_sensor.h \
        node/vrml97/sphere_sensor.cpp \
        node/vrml97/spot_light.h \
        node/vrml97/spot_light.cpp \
        node/vrml97/switch.h \
        node/vrml97/switch.cpp \
        node/vrml97/text.h \
        node/vrml97/text.cpp \
        node/vrml97/texture_coordinate.h \
        node/vrml97/texture_coordinate.cpp \
        node/vrml97/texture_transform.h \
        node/vrml97/texture_transform.cpp \
        node/vrml97/time_sensor.h \
        node/vrml97/time_sensor.cpp \
        node/vrml97/touch_sensor.h \
        node/vrml97/touch_sensor.cpp \
        node/vrml97/transform.h \
        node/vrml97/transform.cpp \
        node/vrml97/viewpoint.h \
        node/vrml97/viewpoint.cpp \
        node/vrml97/visibility_sensor.h \
        node/vrml97/visibility_sensor.cpp \
        node/vrml97/world_info.h \
        node/vrml97/world_info.cpp \
        node/vrml97/cad_layer.h \
        node/vrml97/cad_layer.cpp \
        node/vrml97/cad_assembly.h \
        node/vrml97/cad_assembly.cpp \
        libopenvrml/openvrml/vrml97_grammar.cpp \
        libopenvrml/openvrml/x3d_vrml_grammar.cpp \
        libopenvrml/openvrml/read_write_mutex.cpp \
        libopenvrml/openvrml/basetypes.cpp \
        libopenvrml/openvrml/field_value.cpp \
        libopenvrml/openvrml/event.cpp \
        libopenvrml/openvrml/exposedfield.cpp \
        libopenvrml/openvrml/scope.cpp \
        libopenvrml/openvrml/node.cpp \
        libopenvrml/openvrml/bounding_volume.cpp \
        libopenvrml/openvrml/browser.cpp \
        libopenvrml/openvrml/viewer.cpp \
        libopenvrml/openvrml/rendering_context.cpp \
        libopenvrml/openvrml/frustum.cpp \
        libopenvrml/openvrml/node_impl_util.cpp \
        libopenvrml/openvrml/vrml97node.cpp \
        libopenvrml/openvrml/x3d_core.cpp \
        libopenvrml/openvrml/x3d_networking.cpp \
        libopenvrml/openvrml/x3d_grouping.cpp \
        libopenvrml/openvrml/x3d_rendering.cpp \
        libopenvrml/openvrml/x3d_shape.cpp \
        libopenvrml/openvrml/x3d_geometry2d.cpp \
        libopenvrml/openvrml/x3d_texturing.cpp \
        libopenvrml/openvrml/x3d_interpolation.cpp \
        libopenvrml/openvrml/x3d_key_device_sensor.cpp \
        libopenvrml/openvrml/x3d_event_utilities.cpp \
        libopenvrml/openvrml/x3d_dis.cpp \
        libopenvrml/openvrml/x3d_environmental_effects.cpp \
        libopenvrml/openvrml/x3d_geospatial.cpp \
        libopenvrml/openvrml/x3d_hanim.cpp \
        libopenvrml/openvrml/x3d_nurbs.cpp \
        libopenvrml/openvrml/x3d_cad_geometry.cpp

libopenvrml_libopenvrml_la_LDFLAGS = \
        -version-info $(LIBOPENVRML_LIBRARY_VERSION) \
        $(XML_LIBS) \
        $(JPEG_LIBS) \
        $(PNG_LIBS) \
        $(FONTCONFIG_LIBS) \
        $(FREETYPE_LIBS) \
        $(PTHREAD_LIBS)

libopenvrml_libopenvrml_la_LIBADD = \
        -lboost_thread$(BOOST_LIB_SUFFIX) \
        -lboost_filesystem$(BOOST_LIB_SUFFIX) \
	-lltdl

openvrmlscriptdir = $(pkglibdir)/script

openvrmlscript_LTLIBRARIES =
if ENABLE_SCRIPT_NODE_JAVASCRIPT
openvrmlscript_LTLIBRARIES += script/javascript.la
endif
if ENABLE_SCRIPT_NODE_JAVA
openvrmlscript_LTLIBRARIES += script/java.la
endif

script_javascript_la_SOURCES = script/javascript.cpp
script_javascript_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS \
        -DBOOST_MPL_LIMIT_VECTOR_SIZE=30
script_javascript_la_CXXFLAGS = $(JS_CFLAGS)
script_javascript_la_LDFLAGS = \
        -module \
        -avoid-version \
        $(OPENVRML_JAVASCRIPT_RPATH) \
        $(JS_LIBS)

script_java_la_SOURCES = script/java.cpp
script_java_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_builddir)/src/script/java \
        -DOPENVRML_LIBDIR_=\"$(libdir)\" \
        -DOPENVRML_PKGDATADIR_=\"$(pkgdatadir)\" \
        -DJAVA_HOME=\"$(JAVA_HOME)\" \
        -DOPENVRML_JVM_ARCH=\"$(OPENVRML_JVM_ARCH)\" \
        -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS \
        -DBOOST_MPL_LIMIT_VECTOR_SIZE=30
script_java_la_LDFLAGS = \
        -module \
        -avoid-version

libopenvrml_gl_libopenvrml_gl_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_builddir)/src/libopenvrml-gl \
        -I$(top_srcdir)/src/libopenvrml-gl
libopenvrml_gl_libopenvrml_gl_la_CXXFLAGS = $(GLU_CFLAGS)
libopenvrml_gl_libopenvrml_gl_la_SOURCES = \
        libopenvrml-gl/openvrml/gl/viewer.cpp
libopenvrml_gl_libopenvrml_gl_la_LDFLAGS = \
        -version-info $(LIBOPENVRML_GL_LIBRARY_VERSION) \
        $(GLU_LIBS)
libopenvrml_gl_libopenvrml_gl_la_LIBADD = libopenvrml/libopenvrml.la

openvrml_xembed_openvrml_xembed_CPPFLAGS = \
        -I$(top_srcdir)/lib/gtkglext \
        -I$(top_builddir)/lib/gtkglext \
        -I$(top_builddir)/lib/gtkglext/gdk \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_builddir)/src/libopenvrml-gl \
        -I$(top_srcdir)/src/libopenvrml-gl \
        -I$(mozincludedir) \
        -DGTK_DISABLE_DEPRECATED
openvrml_xembed_openvrml_xembed_CXXFLAGS = \
        $(PTHREAD_CFLAGS) \
        $(GTK_CFLAGS) \
        $(GL_CFLAGS)
openvrml_xembed_openvrml_xembed_SOURCES = \
        openvrml-xembed/main.cpp \
        openvrml-xembed/command_istream.cpp \
        openvrml-xembed/plugin_streambuf.cpp \
        openvrml-xembed/gtkvrmlbrowser.cpp
openvrml_xembed_openvrml_xembed_LDFLAGS = \
        $(GTK_LIBS) \
        $(GL_LIBS)
openvrml_xembed_openvrml_xembed_LDADD = \
        libopenvrml-gl/libopenvrml-gl.la \
        $(top_builddir)/lib/gtkglext/gtk/libgtkglext-x11-1.0.la \
        $(top_builddir)/lib/gtkglext/gdk/libgdkglext-x11-1.0.la


openvrml_player_openvrml_player_CPPFLAGS = \
        -DG_DISABLE_DEPRECATED \
        -DGTK_DISABLE_DEPRECATED \
        -DGNOME_DISABLE_DEPRECATED \
        -DOPENVRML_PLAYER_PKGDATADIR_=\"$(datadir)/openvrml-player\" \
        -DOPENVRML_LIBEXECDIR_=\"$(libexecdir)\" \
        -I$(builddir)/libopenvrml \
        -I$(srcdir)/libopenvrml
openvrml_player_openvrml_player_CXXFLAGS = \
        $(GNOMEUI_CFLAGS) \
        $(CURL_CFLAGS)
openvrml_player_openvrml_player_LDFLAGS = \
        -export-dynamic \
        $(GNOMEUI_LIBS) \
        $(CURL_LIBS)

openvrml_player_uidir = $(datadir)/openvrml-player/ui
dist_openvrml_player_ui_DATA = openvrml-player/openvrml-player.ui

openvrml_player_openvrml_player_SOURCES = \
        openvrml-player/filechooserdialog.cpp \
        openvrml-player/player.cpp


EXTRA_DIST = \
        libopenvrml/openvrml-config.h.in \
        libopenvrml-gl/openvrml-gl-config.h.in

#
# Automake 1.10 seems to need this for distcheck to work.
#
CLEANFILES = \
        openvrml-xembed/.libs/openvrml-xembed
