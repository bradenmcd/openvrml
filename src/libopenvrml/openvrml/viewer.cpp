// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2003, 2004  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Viewer.cpp
//  Abstract base class for display of VRML models
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <private.h>
# include "viewer.h"

/**
 * @class openvrml::viewer
 *
 * @brief Map the scene graph to the underlying graphics library.
 */

/**
 * @var openvrml::browser * openvrml::viewer::browser_
 *
 * @brief A pointer to the <code>browser</code> with which the
 * <code>viewer</code> is currently associated.
 */

/**
 * @var openvrml::frustum openvrml::viewer::frustum_
 *
 * @brief The frustum.
 */

/**
 * @var openvrml::viewer::mask_none
 */

/**
 * @var openvrml::viewer::mask_ccw
 */

/**
 * @var openvrml::viewer::mask_convex
 */

/**
 * @var openvrml::viewer::mask_solid
 */

/**
 * @var openvrml::viewer::mask_bottom
 */

/**
 * @var openvrml::viewer::mask_top
 */

/**
 * @var openvrml::viewer::mask_side
 */

/**
 * @var openvrml::viewer::mask_color_per_vertex
 */

/**
 * @var openvrml::viewer::mask_normal_per_vertex
 */

/**
 * @enum openvrml::viewer::rendering_mode
 *
 * @brief The rendering mode.
 */

/**
 * @var openvrml::viewer::rendering_mode openvrml::viewer::draw_mode
 *
 * @brief Draw mode.
 */

/**
 * @var openvrml::viewer::rendering_mode openvrml::viewer::pick_mode
 *
 * @brief Pick mode.
 */

/**
 * @typedef openvrml::viewer::object_t
 *
 * @brief An object handle.
 */

/**
 * @typedef openvrml::viewer::texture_object_t
 *
 * @brief An texture object handle.
 */

/**
 * @brief Construct.
 */
openvrml::viewer::viewer() OPENVRML_NOTHROW:
    browser_(0)
{}

/**
 * @brief Destroy.
 */
openvrml::viewer::~viewer() OPENVRML_NOTHROW
{}

/**
 * @brief A pointer to the <code>browser</code> with which the
 *        <code>viewer</code> is currently associated.
 *
 * @return a pointer to the <code>browser</code> with which the
 *         <code>viewer</code> is currently associated, or 0 if the
 *         <code>viewer</code> is not associated with a <code>browser</code>.
 */
openvrml::browser * openvrml::viewer::browser() const OPENVRML_NOTHROW
{
    return this->browser_;
}

/**
 * @fn openvrml::viewer::rendering_mode openvrml::viewer::mode()
 *
 * @brief Get the rendering mode.
 *
 * @return the rendering mode.
 */

/**
 * @fn double openvrml::viewer::frame_rate()
 *
 * @brief Get the frame rate.
 *
 * @return the frame rate.
 */

/**
 * @fn void openvrml::viewer::reset_user_navigation()
 *
 * @brief Return view to the last bound Viewpoint.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::begin_object(const char * id, bool retain)
 *
 * @brief Begin a display list.
 *
 * @param[in] id        object identifier.
 * @param[in] retain    whether the object should be retained for reuse.
 *
 * @return the display object identifier.
 */

/**
 * @fn void openvrml::viewer::end_object()
 *
 * @brief End a display list.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_background(const std::vector<float> & ground_angle, const std::vector<color> & ground_color, const std::vector<float> & sky_angle, const std::vector<color> & sky_color, const image & front, const image & back, const image & left, const image & right, const image & top, const image & bottom)
 *
 * @brief Insert a background into a display list.
 *
 * @param[in] ground_angle  ground angles.
 * @param[in] ground_color  ground colors.
 * @param[in] sky_angle     sky angles.
 * @param[in] sky_color     sky colors.
 * @param[in] front         front texture.
 * @param[in] back          back texture.
 * @param[in] left          left texture.
 * @param[in] right         right texture.
 * @param[in] top           top texture.
 * @param[in] bottom        bottom texture.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_box(const vec3f & size)
 *
 * @brief Insert a box into a display list.
 *
 * @param[in] size  box dimensions.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_cone(float height, float radius, bool bottom, bool side)
 *
 * @brief Insert a cone into a display list.
 *
 * @param[in] height    height.
 * @param[in] radius    radius at base.
 * @param[in] bottom    show the bottom.
 * @param[in] side      show the side.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_cylinder(float height, float radius, bool bottom, bool side, bool top)
 *
 * @brief Insert a cylinder into a display list.
 *
 * @param[in] height    height.
 * @param[in] radius    radius.
 * @param[in] bottom    show the bottom.
 * @param[in] side      show the side.
 * @param[in] top       show the top.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_elevation_grid(unsigned int mask, const std::vector<float> & height, int32 x_dimension, int32 z_dimension, float x_spacing, float z_spacing, const std::vector<color> & color, const std::vector<vec3f> & normal, const std::vector<vec2f> & tex_coord)
 *
 * @brief Insert an elevation grid into a display list.
 *
 * @param[in] mask
 * @param[in] height        height field.
 * @param[in] x_dimension   vertices in the x direction.
 * @param[in] z_dimension   vertices in the z direction.
 * @param[in] x_spacing     distance between vertices in the x direction.
 * @param[in] z_spacing     distance between vertices in the z direction.
 * @param[in] color         colors.
 * @param[in] normal        normals.
 * @param[in] tex_coord     texture coordinates.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_extrusion(unsigned int mask, const std::vector<vec3f> & spine, const std::vector<vec2f> & cross_section, const std::vector<rotation> & orientation, const std::vector<vec2f> & scale)
 *
 * @brief Insert an extrusion into a display list.
 *
 * @param[in] mask
 * @param[in] spine         spine points.
 * @param[in] cross_section cross-sections.
 * @param[in] orientation   cross-section orientations.
 * @param[in] scale         cross-section scales.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_line_set(const std::vector<vec3f> & coord, const std::vector<int32> & coord_index, bool color_per_vertex, const std::vector<color> & color, const std::vector<int32> & color_index)
 *
 * @brief Insert a line set into a display list.
 *
 * @param[in] coord             coordinates.
 * @param[in] coord_index       coordinate indices.
 * @param[in] color_per_vertex  whether colors are applied per-vertex or per-face.
 * @param[in] color             colors.
 * @param[in] color_index       color indices.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_point_set(const std::vector<vec3f> & coord, const std::vector<color> & color)
 *
 * @brief Insert a point set into a display list.
 *
 * @param[in] coord     points.
 * @param[in] color     colors.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_shell(unsigned int mask, const std::vector<vec3f> & coord, const std::vector<int32> & coord_index, const std::vector<color> & color, const std::vector<int32> & color_index, const std::vector<vec3f> & normal, const std::vector<int32> & normal_index, const std::vector<vec2f> & tex_coord, const std::vector<int32> & tex_coord_index)
 *
 * @brief Insert a shell into a display list.
 *
 * @param[in] mask
 * @param[in] coord           coordinates.
 * @param[in] coord_index     coordinate indices.
 * @param[in] color           colors.
 * @param[in] color_index     color indices.
 * @param[in] normal          normals.
 * @param[in] normal_index    normal indices.
 * @param[in] tex_coord       texture coordinates.
 * @param[in] tex_coord_index texture coordinate indices.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_sphere(float radius)
 *
 * @brief Insert a sphere into a display list.
 *
 * @param[in] radius    sphere radius.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_dir_light(float ambient_intensity, float intensity , const color & color, const vec3f & direction)
 *
 * @brief Insert a directional light into a display list.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] intensity         intensity.
 * @param[in] color             color.
 * @param[in] direction         direction.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_point_light(float ambient_intensity, const vec3f & attenuation, const color & color, float intensity, const vec3f & location, float radius)
 *
 * @brief Insert a point light into a display list.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] attenuation       attenuation.
 * @param[in] color             color.
 * @param[in] intensity         intensity.
 * @param[in] location          location.
 * @param[in] radius            radius.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_spot_light(float ambient_intensity, const vec3f & attenuation, float beam_width, const color & color, float cut_off_angle, const vec3f & direction, float intensity, const vec3f & location, float radius)
 *
 * @brief Insert a point light into a display list.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] attenuation       attenuation.
 * @param[in] beam_width        beam width.
 * @param[in] color             color.
 * @param[in] cut_off_angle     cut-off angle.
 * @param[in] direction         direction.
 * @param[in] intensity         intensity.
 * @param[in] location          location.
 * @param[in] radius            radius.
 *
 * @return display object identifier.
 */

/**
 * @fn openvrml::viewer::object_t openvrml::viewer::insert_reference(object_t existing_object)
 *
 * @brief Insert a reference to an existing object into a display list.
 *
 * @return display object identifier.
 */

/**
 * @fn void openvrml::viewer::remove_object(object_t ref)
 *
 * @brief Remove an object from the display list.
 *
 * @param[in] ref   object handle.
 */

/**
 * @fn void openvrml::viewer::enable_lighting(bool val)
 *
 * @brief Enable/disable lighting.
 *
 * @param[in] val whether lighting should be enabled.
 */

/**
 * @fn void openvrml::viewer::set_fog(const color & color, float visibility_range, const char * type)
 *
 * @brief Set the fog.
 *
 * @param[in] color             fog color.
 * @param[in] visibility_range  the distance at which objects are fully obscured by
 *                          fog.
 * @param[in] type              fog type.
 */

/**
 * @fn void openvrml::viewer::set_color(const color & rgb, float a)
 *
 * @brief Set the color.
 *
 * @param[in] rgb   red, green, and blue components.
 * @param[in] a     alpha (transparency) component.
 */

/**
 * @fn void openvrml::viewer::set_material(float ambient_intensity, const color & diffuse_color, const color & emissive_color, float shininess, const color & specular_color, float transparency)
 *
 * @brief Set the material.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] diffuse_color     diffuse color.
 * @param[in] emissive_color    emissive color.
 * @param[in] shininess         shininess.
 * @param[in] specular_color    specular color.
 * @param[in] transparency      transparency.
 */

/**
 * @fn void openvrml::viewer::set_material_mode(size_t tex_components, bool geometry_color)
 *
 * @brief Set the material mode.
 *
 * @param[in] tex_components    texture components.
 * @param[in] geometry_color    geometry color.
 */

/**
 * @fn void openvrml::viewer::set_sensitive(node * object)
 *
 * @brief Indicate that a node should be sensitive to the pointing device.
 *
 * @param[in] object    a node.
 */

/**
 * @fn openvrml::viewer::texture_object_t openvrml::viewer::insert_texture(const image & img, bool repeat_s, bool repeat_t, bool retainHint)
 *
 * @brief Create a texture object.
 *
 * @param[in] img           image.
 * @param[in] repeat_s      repeat in the S direction.
 * @param[in] repeat_t      repeat in the T direction.
 * @param[in] retainHint    whether the texture is likely to be reused.
 *
 * @return a handle to the inserted texture.
 */

/**
 * @fn void openvrml::viewer::insert_texture_reference(texture_object_t ref, size_t components)
 *
 * @brief Insert a texture into the display list from an existing handle.
 *
 * @param[in] ref           texture handle.
 * @param[in] components    number of components.
 */

/**
 * @fn void openvrml::viewer::remove_texture_object(texture_object_t ref)
 *
 * @brief Remove a texture from the display list.
 *
 * @param[in] ref   texture handle.
 */

/**
 * @fn void openvrml::viewer::set_texture_transform(const vec2f & center, float rotation, const vec2f & scale, const vec2f & translation)
 *
 * @brief Set the texture transform.
 *
 * @param[in] center        center.
 * @param[in] rotation      rotation.
 * @param[in] scale         scale.
 * @param[in] translation   translation.
 */

/**
 * @fn void openvrml::viewer::set_viewpoint(const vec3f & position, const rotation & orientation, float field_of_view, float avatar_size, float visibility_limit)
 *
 * @brief Set the viewpoint.
 *
 * @param[in] position          position.
 * @param[in] orientation       orientation.
 * @param[in] field_of_view     field of view.
 * @param[in] avatar_size       avatar size.
 * @param[in] visibility_limit  visiblity limit.
 */

/**
 * @fn void openvrml::viewer::transform(const mat4f & mat)
 *
 * @brief Transform the modelview.
 *
 * Make the modelview matrix the result of multiplying @p mat by the current
 * modelview matrix.
 *
 * @param[in] mat   transformation matrix.
 */

/**
 * @fn void openvrml::viewer::transform_points(size_t nPoints, vec3f * point) const
 *
 * @brief Transform @p points by the current modelview matrix.
 *
 * @param[in] nPoints   number of points.
 * @param[in] point     pointer to the first point in an array.
 */

/**
 * Intersect the given bounding volume with the view volume. This
 * goes into the viewer to provide a hook for systems that use
 * non-standard view volumes. Most subclasses should be able to use
 * the default implementation provided here. If your view volume is
 * so strange that there's no way to cull to is, then reimplement to
 * always return bounding_volume::inside.
 *
 * @param[in] bvolume   the bounding volume to intersect with the view volume.
 *
 * @return bounding_volume::inside, bounding_volume::outside, or
 *         bounding_volume::partial.
 */
openvrml::bounding_volume::intersection
openvrml::viewer::intersect_view_volume(const bounding_volume & bvolume) const
{
    //
    // For normal VRML97 use, this won't need to be overridden, but for
    // systems with non-standard view volumes, this can be changed to
    // cull as appropriate. Note that culling can be disabled by setting
    // a flag in rendering_context. Since I don't have access to the
    // appropriate cave/boom/whichever api's, I can't be sure that this
    // is enough. If it isn't, please express any concerns to the
    // OpenVRML developer's list, and it can be fixed...
    //
    return bvolume.intersect_frustum(this->frustum_);
}

/**
 * @fn void openvrml::viewer::draw_bounding_sphere(const bounding_sphere & bs, bounding_volume::intersection intersection)
 *
 * @brief Draw a bounding sphere.
 *
 * Used for debugging view culling. Probably should be draw_bounding_volume and
 * handle axis_aligned_bounding_boxes as well.
 *
 * @param[in] bs            a bounding sphere; if max, will not be drawn
 * @param[in] intersection  one of the bvolume intersection test constants, or 4
 *                      to draw in unique way. (useful for debugging)
 */

/**
 * @todo We're forcing everybody to carry around a frustum
 *       whether they want it or not. It shouldn't be used except
 *       for debugging and stuff since it might not be valid in some
 *       implementations
 *
 * @return the frustum.
 */
const openvrml::frustum & openvrml::viewer::frustum() const
{
    return this->frustum_;
}

/**
 * @brief Set the frustum.
 *
 * @param[in] f new frustum value.
 */
void openvrml::viewer::frustum(const openvrml::frustum & f)
{
    this->frustum_ = f;
}
