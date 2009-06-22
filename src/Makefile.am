SUBDIRS = script

BUILT_SOURCES =

lib_LTLIBRARIES = libopenvrml/libopenvrml.la
if ENABLE_GL_RENDERER
lib_LTLIBRARIES += libopenvrml-gl/libopenvrml-gl.la
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
        libopenvrml/openvrml/bad_url.h \
        libopenvrml/openvrml/basetypes.h \
        libopenvrml/openvrml/vrml97_grammar.h \
        libopenvrml/openvrml/x3d_vrml_grammar.h \
        libopenvrml/openvrml/field_value.h \
        libopenvrml/openvrml/event.h \
        libopenvrml/openvrml/exposedfield.h \
        libopenvrml/openvrml/scope.h \
        libopenvrml/openvrml/node.h \
        libopenvrml/openvrml/bounding_volume.h \
        libopenvrml/openvrml/script.h \
        libopenvrml/openvrml/scene.h \
        libopenvrml/openvrml/browser.h \
        libopenvrml/openvrml/viewer.h \
        libopenvrml/openvrml/rendering_context.h \
        libopenvrml/openvrml/frustum.h \
        libopenvrml/openvrml/node_impl_util.h

if ENABLE_GL_RENDERER
openvrml_gl_includedir = $(pkgincludedir)/openvrml/gl
openvrml_gl_include_HEADERS =  libopenvrml-gl/openvrml/gl/viewer.h
endif

noinst_HEADERS = \
        libopenvrml/private.h \
        openvrml-player/filechooserdialog.h

LIBOPENVRML_LIBRARY_VERSION =    8:8:0
LIBOPENVRML_GL_LIBRARY_VERSION = 7:4:0
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
        -DBOOST_MPL_LIMIT_VECTOR_SIZE=30
libopenvrml_libopenvrml_la_CXXFLAGS = \
        $(FREETYPE_CFLAGS) \
        $(PTHREAD_CFLAGS) \
        $(XML_CFLAGS)

libopenvrml_libopenvrml_la_SOURCES = \
        libopenvrml/openvrml/bad_url.cpp \
        libopenvrml/openvrml/vrml97_grammar.cpp \
        libopenvrml/openvrml/x3d_vrml_grammar.cpp \
        libopenvrml/openvrml/basetypes.cpp \
        libopenvrml/openvrml/field_value.cpp \
        libopenvrml/openvrml/event.cpp \
        libopenvrml/openvrml/exposedfield.cpp \
        libopenvrml/openvrml/scope.cpp \
        libopenvrml/openvrml/node.cpp \
        libopenvrml/openvrml/script.cpp \
        libopenvrml/openvrml/bounding_volume.cpp \
        libopenvrml/openvrml/scene.cpp \
        libopenvrml/openvrml/browser.cpp \
        libopenvrml/openvrml/viewer.cpp \
        libopenvrml/openvrml/rendering_context.cpp \
        libopenvrml/openvrml/frustum.cpp \
        libopenvrml/openvrml/node_impl_util.cpp \
        libopenvrml/openvrml/local/conf.cpp \
        libopenvrml/openvrml/local/conf.h \
        libopenvrml/openvrml/local/dl.cpp \
        libopenvrml/openvrml/local/dl.h \
        libopenvrml/openvrml/local/uri.cpp \
        libopenvrml/openvrml/local/uri.h \
        libopenvrml/openvrml/local/xml_reader.cpp \
        libopenvrml/openvrml/local/xml_reader.h \
        libopenvrml/openvrml/local/parse_vrml.cpp \
        libopenvrml/openvrml/local/parse_vrml.h \
        libopenvrml/openvrml/local/component.cpp \
        libopenvrml/openvrml/local/component.h \
        libopenvrml/openvrml/local/proto.cpp \
        libopenvrml/openvrml/local/proto.h \
        libopenvrml/openvrml/local/externproto.cpp \
        libopenvrml/openvrml/local/externproto.h \
        libopenvrml/openvrml/local/field_value_types.h \
        libopenvrml/openvrml/local/float.h \
        libopenvrml/openvrml/local/node_metatype_registry_impl.cpp \
        libopenvrml/openvrml/local/node_metatype_registry_impl.h

libopenvrml_libopenvrml_la_LDFLAGS = \
        -version-info $(LIBOPENVRML_LIBRARY_VERSION) \
        -no-undefined \
        $(XML_LIBS) \
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
        -no-undefined \
        -avoid-version \
        $(OPENVRML_JAVASCRIPT_RPATH) \
        $(JS_LIBS)
script_javascript_la_LIBADD = \
        libopenvrml/libopenvrml.la

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
        -no-undefined \
        -avoid-version
script_java_la_LIBADD = \
        libopenvrml/libopenvrml.la

openvrmlnodedir = $(pkglibdir)/node
openvrmlnode_LTLIBRARIES = \
        node/vrml97.la \
        node/x3d-core.la \
        node/x3d-networking.la \
        node/x3d-grouping.la \
        node/x3d-rendering.la \
        node/x3d-shape.la \
        node/x3d-geometry2d.la \
        node/x3d-texturing.la \
        node/x3d-interpolation.la \
        node/x3d-key-device-sensor.la \
        node/x3d-event-utilities.la \
        node/x3d-dis.la \
        node/x3d-environmental-effects.la \
        node/x3d-geospatial.la \
        node/x3d-h-anim.la \
        node/x3d-nurbs.la \
        node/x3d-cad-geometry.la

node_vrml97_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS \
        -DBOOST_MPL_LIMIT_VECTOR_SIZE=30
node_vrml97_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS) \
        $(FONTCONFIG_CFLAGS) \
        $(FREETYPE_CFLAGS)
node_vrml97_la_SOURCES = \
        node/vrml97/register_node_metatypes.cpp \
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
        node/vrml97/cad_assembly.cpp
node_vrml97_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version \
        $(JPEG_LIBS) \
        $(PNG_LIBS) \
        $(FONTCONFIG_LIBS) \
        $(FREETYPE_LIBS) \
        $(PTHREAD_LIBS)
node_vrml97_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_core_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS \
        -DBOOST_MPL_LIMIT_VECTOR_SIZE=30
node_x3d_core_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_core_la_SOURCES = \
        node/x3d-core/register_node_metatypes.cpp \
        node/x3d-core/metadata_double.cpp \
        node/x3d-core/metadata_double.h \
        node/x3d-core/metadata_float.cpp \
        node/x3d-core/metadata_float.h \
        node/x3d-core/metadata_integer.cpp \
        node/x3d-core/metadata_integer.h \
        node/x3d-core/metadata_set.cpp \
        node/x3d-core/metadata_set.h \
        node/x3d-core/metadata_string.cpp \
        node/x3d-core/metadata_string.h
node_x3d_core_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_core_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_networking_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_networking_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_networking_la_SOURCES = \
        node/x3d-networking/register_node_metatypes.cpp \
        node/x3d-networking/load_sensor.cpp \
        node/x3d-networking/load_sensor.h
node_x3d_networking_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_networking_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_grouping_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_grouping_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_grouping_la_SOURCES = \
        node/x3d-grouping/register_node_metatypes.cpp \
        node/x3d-grouping/static_group.cpp \
        node/x3d-grouping/static_group.h
node_x3d_grouping_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_grouping_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_rendering_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_rendering_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_rendering_la_SOURCES = \
        node/x3d-rendering/register_node_metatypes.cpp \
        node/x3d-rendering/color_rgba.cpp \
        node/x3d-rendering/color_rgba.h \
        node/x3d-rendering/indexed_triangle_fan_set.cpp \
        node/x3d-rendering/indexed_triangle_fan_set.h \
        node/x3d-rendering/indexed_triangle_set.cpp \
        node/x3d-rendering/indexed_triangle_set.h \
        node/x3d-rendering/indexed_triangle_strip_set.cpp \
        node/x3d-rendering/indexed_triangle_strip_set.h \
        node/x3d-rendering/triangle_fan_set.cpp \
        node/x3d-rendering/triangle_fan_set.h \
        node/x3d-rendering/triangle_set.cpp \
        node/x3d-rendering/triangle_set.h \
        node/x3d-rendering/triangle_strip_set.cpp \
        node/x3d-rendering/triangle_strip_set.h
node_x3d_rendering_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_rendering_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_shape_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_shape_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_shape_la_SOURCES = \
        node/x3d-shape/register_node_metatypes.cpp \
        node/x3d-shape/fill_properties.cpp \
        node/x3d-shape/fill_properties.h \
        node/x3d-shape/line_properties.cpp \
        node/x3d-shape/line_properties.h
node_x3d_shape_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_shape_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_geometry2d_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_geometry2d_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_geometry2d_la_SOURCES = \
        node/x3d-geometry2d/register_node_metatypes.cpp \
        node/x3d-geometry2d/arc2d.cpp \
        node/x3d-geometry2d/arc2d.h \
        node/x3d-geometry2d/arc_close2d.cpp \
        node/x3d-geometry2d/arc_close2d.h \
        node/x3d-geometry2d/circle2d.cpp \
        node/x3d-geometry2d/circle2d.h \
        node/x3d-geometry2d/disk2d.cpp \
        node/x3d-geometry2d/disk2d.h \
        node/x3d-geometry2d/polyline2d.cpp \
        node/x3d-geometry2d/polyline2d.h \
        node/x3d-geometry2d/polypoint2d.cpp \
        node/x3d-geometry2d/polypoint2d.h \
        node/x3d-geometry2d/rectangle2d.cpp \
        node/x3d-geometry2d/rectangle2d.h \
        node/x3d-geometry2d/triangle_set2d.cpp \
        node/x3d-geometry2d/triangle_set2d.h
node_x3d_geometry2d_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_geometry2d_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_texturing_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_texturing_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_texturing_la_SOURCES = \
        node/x3d-texturing/register_node_metatypes.cpp \
        node/x3d-texturing/multi_texture.cpp \
        node/x3d-texturing/multi_texture.h \
        node/x3d-texturing/multi_texture_coordinate.cpp \
        node/x3d-texturing/multi_texture_coordinate.h \
        node/x3d-texturing/multi_texture_transform.cpp \
        node/x3d-texturing/multi_texture_transform.h \
        node/x3d-texturing/texture_coordinate_generator.cpp \
        node/x3d-texturing/texture_coordinate_generator.h
node_x3d_texturing_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_texturing_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_interpolation_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_interpolation_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_interpolation_la_SOURCES = \
        node/x3d-interpolation/register_node_metatypes.cpp \
        node/x3d-interpolation/coordinate_interpolator2d.cpp \
        node/x3d-interpolation/coordinate_interpolator2d.h \
        node/x3d-interpolation/position_interpolator2d.cpp \
        node/x3d-interpolation/position_interpolator2d.h
node_x3d_interpolation_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_interpolation_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_key_device_sensor_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_key_device_sensor_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_key_device_sensor_la_SOURCES = \
        node/x3d-key-device-sensor/register_node_metatypes.cpp \
        node/x3d-key-device-sensor/key_sensor.cpp \
        node/x3d-key-device-sensor/key_sensor.h \
        node/x3d-key-device-sensor/string_sensor.cpp \
        node/x3d-key-device-sensor/string_sensor.h
node_x3d_key_device_sensor_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_key_device_sensor_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_event_utilities_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_event_utilities_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_event_utilities_la_SOURCES = \
        node/x3d-event-utilities/register_node_metatypes.cpp \
        node/x3d-event-utilities/boolean_filter.cpp \
        node/x3d-event-utilities/boolean_filter.h \
        node/x3d-event-utilities/boolean_sequencer.cpp \
        node/x3d-event-utilities/boolean_sequencer.h \
        node/x3d-event-utilities/boolean_toggle.cpp \
        node/x3d-event-utilities/boolean_toggle.h \
        node/x3d-event-utilities/boolean_trigger.cpp \
        node/x3d-event-utilities/boolean_trigger.h \
        node/x3d-event-utilities/integer_sequencer.cpp \
        node/x3d-event-utilities/integer_sequencer.h \
        node/x3d-event-utilities/integer_trigger.cpp \
        node/x3d-event-utilities/integer_trigger.h \
        node/x3d-event-utilities/time_trigger.cpp \
        node/x3d-event-utilities/time_trigger.h
node_x3d_event_utilities_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_event_utilities_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_dis_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_dis_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_dis_la_SOURCES = \
        node/x3d-dis/register_node_metatypes.cpp \
        node/x3d-dis/espdu_transform.cpp \
        node/x3d-dis/espdu_transform.h \
        node/x3d-dis/receiver_pdu.cpp \
        node/x3d-dis/receiver_pdu.h \
        node/x3d-dis/signal_pdu.cpp \
        node/x3d-dis/signal_pdu.h \
        node/x3d-dis/transmitter_pdu.cpp \
        node/x3d-dis/transmitter_pdu.h
node_x3d_dis_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_dis_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_environmental_effects_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_environmental_effects_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_environmental_effects_la_SOURCES = \
        node/x3d-environmental-effects/register_node_metatypes.cpp \
        node/x3d-environmental-effects/texture_background.cpp \
        node/x3d-environmental-effects/texture_background.h
node_x3d_environmental_effects_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_environmental_effects_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_geospatial_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_geospatial_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_geospatial_la_SOURCES = \
        node/x3d-geospatial/register_node_metatypes.cpp \
        node/x3d-geospatial/geospatial-common.h \
        node/x3d-geospatial/geospatial-common.cpp \
        node/x3d-geospatial/geo_coordinate.cpp \
        node/x3d-geospatial/geo_coordinate.h \
        node/x3d-geospatial/geo_elevation_grid.cpp \
        node/x3d-geospatial/geo_elevation_grid.h \
        node/x3d-geospatial/geo_location.cpp \
        node/x3d-geospatial/geo_location.h \
        node/x3d-geospatial/geo_lod.cpp \
        node/x3d-geospatial/geo_lod.h \
        node/x3d-geospatial/geo_metadata.cpp \
        node/x3d-geospatial/geo_metadata.h \
        node/x3d-geospatial/geo_origin.cpp \
        node/x3d-geospatial/geo_origin.h \
        node/x3d-geospatial/geo_position_interpolator.cpp \
        node/x3d-geospatial/geo_position_interpolator.h \
        node/x3d-geospatial/geo_touch_sensor.cpp \
        node/x3d-geospatial/geo_touch_sensor.h \
        node/x3d-geospatial/geo_viewpoint.cpp \
        node/x3d-geospatial/geo_viewpoint.h
node_x3d_geospatial_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_geospatial_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_h_anim_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_h_anim_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_h_anim_la_SOURCES = \
        node/x3d-h-anim/register_node_metatypes.cpp \
        node/x3d-h-anim/h_anim_displacer.cpp \
        node/x3d-h-anim/h_anim_displacer.h \
        node/x3d-h-anim/h_anim_humanoid.cpp \
        node/x3d-h-anim/h_anim_humanoid.h \
        node/x3d-h-anim/h_anim_joint.cpp \
        node/x3d-h-anim/h_anim_joint.h \
        node/x3d-h-anim/h_anim_segment.cpp \
        node/x3d-h-anim/h_anim_segment.h \
        node/x3d-h-anim/h_anim_site.cpp \
        node/x3d-h-anim/h_anim_site.h
node_x3d_h_anim_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_h_anim_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_nurbs_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_nurbs_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_nurbs_la_SOURCES = \
        node/x3d-nurbs/register_node_metatypes.cpp \
        node/x3d-nurbs/contour2d.cpp \
        node/x3d-nurbs/contour2d.h \
        node/x3d-nurbs/contour_polyline2d.cpp \
        node/x3d-nurbs/contour_polyline2d.h \
        node/x3d-nurbs/coordinate_double.cpp \
        node/x3d-nurbs/coordinate_double.h \
        node/x3d-nurbs/nurbs_curve.cpp \
        node/x3d-nurbs/nurbs_curve.h \
        node/x3d-nurbs/nurbs_curve2d.cpp \
        node/x3d-nurbs/nurbs_curve2d.h \
        node/x3d-nurbs/nurbs_orientation_interpolator.cpp \
        node/x3d-nurbs/nurbs_orientation_interpolator.h \
        node/x3d-nurbs/nurbs_patch_surface.cpp \
        node/x3d-nurbs/nurbs_patch_surface.h \
        node/x3d-nurbs/nurbs_position_interpolator.cpp \
        node/x3d-nurbs/nurbs_position_interpolator.h \
        node/x3d-nurbs/nurbs_set.cpp \
        node/x3d-nurbs/nurbs_set.h \
        node/x3d-nurbs/nurbs_surface_interpolator.cpp \
        node/x3d-nurbs/nurbs_surface_interpolator.h \
        node/x3d-nurbs/nurbs_swept_surface.cpp \
        node/x3d-nurbs/nurbs_swept_surface.h \
        node/x3d-nurbs/nurbs_swung_surface.cpp \
        node/x3d-nurbs/nurbs_swung_surface.h \
        node/x3d-nurbs/nurbs_texture_coordinate.cpp \
        node/x3d-nurbs/nurbs_texture_coordinate.h \
        node/x3d-nurbs/nurbs_trimmed_surface.cpp \
        node/x3d-nurbs/nurbs_trimmed_surface.h
node_x3d_nurbs_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_nurbs_la_LIBADD = \
        libopenvrml/libopenvrml.la

node_x3d_cad_geometry_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
node_x3d_cad_geometry_la_CXXFLAGS = \
        $(PTHREAD_CFLAGS)
node_x3d_cad_geometry_la_SOURCES = \
        node/x3d-cad-geometry/cad_face.cpp \
        node/x3d-cad-geometry/cad_face.h \
        node/x3d-cad-geometry/indexed_quad_set.cpp \
        node/x3d-cad-geometry/indexed_quad_set.h
node_x3d_cad_geometry_la_LDFLAGS = \
        -module \
        -no-undefined \
        -avoid-version
node_x3d_cad_geometry_la_LIBADD = \
        libopenvrml/libopenvrml.la

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
        -no-undefined \
        $(GLU_LIBS)
libopenvrml_gl_libopenvrml_gl_la_LIBADD = libopenvrml/libopenvrml.la

if ENABLE_XEMBED
noinst_LTLIBRARIES = libopenvrml-control/libopenvrml-control.la
libexec_PROGRAMS = openvrml-xembed/openvrml-xembed
session_bus_servicesdir = $(datadir)/dbus-1/services
session_bus_services_DATA = openvrml-xembed/org.openvrml.BrowserControl.service
BUILT_SOURCES += \
        openvrml-xembed/browser-server-glue.h \
        openvrml-xembed/browser-factory-server-glue.h \
        openvrml-xembed/browser-host-client-glue.h
endif
libopenvrml_control_libopenvrml_control_la_SOURCES = \
        libopenvrml-control/openvrml_control/browser.cpp \
        libopenvrml-control/openvrml_control/browser.h
libopenvrml_control_libopenvrml_control_la_CPPFLAGS = \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml
libopenvrml_control_libopenvrml_control_la_LIBADD = \
        libopenvrml/libopenvrml.la

openvrml_xembed_openvrml_xembed_CPPFLAGS = \
        -I$(top_builddir)/src/openvrml-xembed \
        -I$(top_srcdir)/src/libopenvrml-control \
        -I$(top_builddir)/src/libopenvrml \
        -I$(top_srcdir)/src/libopenvrml \
        -I$(top_builddir)/src/libopenvrml-gl \
        -I$(top_srcdir)/src/libopenvrml-gl \
        -DGTK_DISABLE_DEPRECATED
openvrml_xembed_openvrml_xembed_CXXFLAGS = \
        $(PTHREAD_CFLAGS) \
        $(DBUS_G_CFLAGS) \
        $(GTKGL_CFLAGS)
openvrml_xembed_openvrml_xembed_SOURCES = \
        openvrml-xembed/main.cpp \
        openvrml-xembed/browserfactory.cpp \
        openvrml-xembed/browserfactory.h \
        openvrml-xembed/browser.cpp \
        openvrml-xembed/browser.h \
        openvrml-xembed/streamclient.cpp \
        openvrml-xembed/streamclient.h
openvrml_xembed_openvrml_xembed_LDFLAGS = \
        $(DBUS_G_LIBS) \
        $(GTKGL_LIBS)
openvrml_xembed_openvrml_xembed_LDADD = \
        libopenvrml-control/libopenvrml-control.la \
        libopenvrml-gl/libopenvrml-gl.la

openvrml_xembed_datadir = $(datadir)/openvrml-xembed
openvrml_xembed_dbusdir = $(openvrml_xembed_datadir)/dbus-1
dist_openvrml_xembed_dbus_DATA = \
        openvrml-xembed/browser.xml \
        openvrml-xembed/browser-factory.xml \
        openvrml-xembed/browser-host.xml

openvrml-xembed/browser-server-glue.h: $(srcdir)/openvrml-xembed/browser.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL) \
                   --prefix=openvrml_xembed_browser    \
                   --mode=glib-server                  \
                   --output=$@                         \
                   $?

openvrml-xembed/browser-factory-server-glue.h: $(srcdir)/openvrml-xembed/browser-factory.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL)      \
                   --prefix=openvrml_xembed_browser_factory \
                   --mode=glib-server                       \
                   --output=$@                              \
                   $?

openvrml-xembed/browser-host-client-glue.h: $(srcdir)/openvrml-xembed/browser-host.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL)   \
                   --prefix=openvrml_xembed_browser_host \
                   --mode=glib-client                    \
                   --output=$@                           \
                   $?

edit = sed -e 's|@libexecdir[@]|$(libexecdir)|g'

openvrml-xembed/org.openvrml.BrowserControl.service: Makefile
	rm -f $@ $@.tmp
	$(edit) '$(srcdir)/$@.in' > $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

openvrml-xembed/org.openvrml.BrowserControl.service: $(srcdir)/openvrml-xembed/org.openvrml.BrowserControl.service.in

if ENABLE_PLAYER
bin_PROGRAMS = openvrml-player/openvrml-player
BUILT_SOURCES += \
        openvrml-player/browser-host-server-glue.h \
        openvrml-player/browser-factory-client-glue.h \
        openvrml-player/browser-client-glue.h
endif

openvrml_player_openvrml_player_CPPFLAGS = \
        -DG_DISABLE_DEPRECATED \
        -DGTK_DISABLE_DEPRECATED \
        -DGNOME_DISABLE_DEPRECATED \
        -DOPENVRML_PLAYER_PKGDATADIR_=\"$(datadir)/openvrml-player\" \
        -DOPENVRML_LIBEXECDIR_=\"$(libexecdir)\" \
        -Iopenvrml-player \
        -Ilibopenvrml \
        -I$(srcdir)/libopenvrml
openvrml_player_openvrml_player_CXXFLAGS = \
        $(DBUS_G_CFLAGS) \
        $(GNOMEUI_CFLAGS) \
        $(CURL_CFLAGS)
openvrml_player_openvrml_player_LDFLAGS = \
        -export-dynamic \
        $(GNOMEUI_LIBS) \
        $(DBUS_G_LIBS) \
        $(CURL_LIBS)

openvrml_player_uidir = $(datadir)/openvrml-player/ui
dist_openvrml_player_ui_DATA = openvrml-player/openvrml-player.ui

openvrml_player_openvrml_player_SOURCES = \
        openvrml-player/filechooserdialog.cpp \
        openvrml-player/player.cpp \
        openvrml-player/curlbrowserhost.cpp \
        openvrml-player/curlbrowserhost.h

openvrml-player/browser-host-server-glue.h: $(srcdir)/openvrml-xembed/browser-host.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL) \
                   --prefix=openvrml_player_curl_browser_host   \
                   --mode=glib-server                  \
                   --output=$@                         \
                   $?

openvrml-player/browser-factory-client-glue.h: $(srcdir)/openvrml-xembed/browser-factory.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL)   \
                   --mode=glib-client                    \
                   --output=$@                           \
                   $?

openvrml-player/browser-client-glue.h: $(srcdir)/openvrml-xembed/browser.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL)   \
                   --mode=glib-client                    \
                   --output=$@                           \
                   $?

mozpluginsdir = $(libdir)/mozilla/plugins
if ENABLE_MOZILLA_PLUGIN
mozplugins_LTLIBRARIES = mozilla-plugin/openvrml.la
BUILT_SOURCES += \
        mozilla-plugin/browser-host-server-glue.h \
        mozilla-plugin/browser-factory-client-glue.h \
        mozilla-plugin/browser-client-glue.h
endif

mozilla_plugin_openvrml_la_CPPFLAGS = \
        -DMOZ_X11 \
        -DGTK_DISABLE_DEPRECATED \
        -DOPENVRML_LIBEXECDIR_=\"$(libexecdir)\" \
        -Imozilla-plugin
mozilla_plugin_openvrml_la_CXXFLAGS = \
        $(MOZILLA_PLUGIN_CFLAGS) \
        $(DBUS_G_CFLAGS)
mozilla_plugin_openvrml_la_SOURCES = mozilla-plugin/openvrml.cpp
mozilla_plugin_openvrml_la_LDFLAGS = \
        -module -avoid-version \
        $(MOZILLA_PLUGIN_LIBS) \
        $(DBUS_G_LIBS)

mozilla-plugin/browser-host-server-glue.h: $(srcdir)/openvrml-xembed/browser-host.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL) \
                   --prefix=openvrml_np_browser_host   \
                   --mode=glib-server                  \
                   --output=$@                         \
                   $?

mozilla-plugin/browser-factory-client-glue.h: $(srcdir)/openvrml-xembed/browser-factory.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL) \
                   --mode=glib-client                  \
                   --output=$@                         \
                   $?

mozilla-plugin/browser-client-glue.h: $(srcdir)/openvrml-xembed/browser.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL) \
                   --mode=glib-client                  \
                   --output=$@                         \
                   $?

EXTRA_DIST = \
        libopenvrml/openvrml-config.h.in \
        libopenvrml-gl/openvrml-gl-config.h.in \
        openvrml-xembed/org.openvrml.BrowserControl.service.in

CLEANFILES = $(BUILT_SOURCES)

DISTCLEANFILES = \
        libopenvrml/openvrml-config.h \
        libopenvrml-gl/openvrml-gl-config.h \
        $(session_bus_services_DATA)
