// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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

# include "viewer.h"
# include "private.h"

namespace openvrml {

/**
 * @class viewer
 *
 * @brief Map the scene graph to the underlying graphics library.
 */

/**
 * @var frustum viewer::frustum_
 *
 * @brief The frustum.
 */

/**
 * @var viewer::mask_none
 */

/**
 * @var viewer::mask_ccw
 */

/**
 * @var viewer::mask_convex
 */

/**
 * @var viewer::mask_solid
 */

/**
 * @var viewer::mask_bottom
 */

/**
 * @var viewer::mask_top
 */

/**
 * @var viewer::mask_side
 */

/**
 * @var viewer::mask_color_per_vertex
 */

/**
 * @var viewer::mask_normal_per_vertex
 */

/**
 * @enum viewer::rendering_mode
 *
 * @brief The rendering mode.
 */

/**
 * @var viewer::rendering_mode viewer::draw_mode
 *
 * @brief Draw mode.
 */

/**
 * @var viewer::rendering_mode viewer::pick_mode
 *
 * @brief Pick mode.
 */

/**
 * @typedef viewer::object_t
 *
 * @brief An object handle.
 */

/**
 * @typedef viewer::texture_object_t
 *
 * @brief An texture object handle.
 */

/**
 * @var openvrml::browser & viewer::browser
 *
 * @brief The browser associated with the viewer.
 */

/**
 * @brief Construct.
 *
 * @param browser   a browser.
 */
viewer::viewer(openvrml::browser & browser):
    browser(browser)
{}

/**
 * @fn viewer::viewer(const viewer &)
 *
 * @brief Not implemented.
 *
 * viewer is not copyable.
 */

/**
 * @brief Destroy.
 */
viewer::~viewer() {}

/**
 * @fn viewer & viewer::operator=(const viewer &)
 *
 * @brief Not implemented.
 *
 * viewer is not copyable.
 */

/**
 * @fn viewer::rendering_mode viewer::mode()
 *
 * @brief Get the rendering mode.
 *
 * @return the rendering mode.
 */

/**
 * @fn double viewer::frame_rate()
 *
 * @brief Get the frame rate.
 *
 * @return the frame rate.
 */

/**
 * @fn void viewer::reset_user_navigation()
 *
 * @brief Return view to the last bound Viewpoint.
 */

/**
 * @fn viewer::object_t viewer::begin_object(const char * id, bool retain)
 *
 * @brief Begin a display list.
 *
 * @param id        object identifier.
 * @param retain    whether the object should be retained for reuse.
 *
 * @return the display object identifier.
 */

/**
 * @fn void viewer::end_object()
 *
 * @brief End a display list.
 */

/**
 * @fn viewer::object_t viewer::insert_background(const std::vector<float> & ground_angle, const std::vector<color> & ground_color, const std::vector<float> & sky_angle, const std::vector<color> & sky_color, size_t * whc, unsigned char ** pixels)
 *
 * @brief Insert a background into a display list.
 *
 * @param ground_angle  ground angles.
 * @param ground_color  ground colors.
 * @param sky_angle     sky angles.
 * @param sky_color     sky colors.
 * @param whc           texture width, height, and number of components.
 * @param pixels        texture pixel data.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_box(const vec3f & size)
 *
 * @brief Insert a box into a display list.
 *
 * @param size  box dimensions.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_cone(float height, float radius, bool bottom, bool side)
 *
 * @brief Insert a cone into a display list.
 *
 * @param height    height.
 * @param radius    radius at base.
 * @param bottom    show the bottom.
 * @param side      show the side.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_cylinder(float height, float radius, bool bottom, bool side, bool top)
 *
 * @brief Insert a cylinder into a display list.
 *
 * @param height    height.
 * @param radius    radius.
 * @param bottom    show the bottom.
 * @param side      show the side.
 * @param top       show the top.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_elevation_grid(unsigned int mask, const std::vector<float> & height, int32 x_dimension, int32 z_dimension, float x_spacing, float z_spacing, const std::vector<color> & color, const std::vector<vec3f> & normal, const std::vector<vec2f> & tex_coord)
 *
 * @brief Insert an elevation grid into a display list.
 *
 * @param mask
 * @param height        height field.
 * @param x_dimension   vertices in the x direction.
 * @param z_dimension   vertices in the z direction.
 * @param x_spacing     distance between vertices in the x direction.
 * @param z_spacing     distance between vertices in the z direction.
 * @param color         colors.
 * @param normal        normals.
 * @param tex_coord     texture coordinates.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_extrusion(unsigned int mask, const std::vector<vec3f> & spine, const std::vector<vec2f> & cross_section, const std::vector<rotation> & orientation, const std::vector<vec2f> & scale)
 *
 * @brief Insert an extrusion into a display list.
 *
 * @param mask
 * @param spine         spine points.
 * @param cross_section cross-sections.
 * @param orientation   cross-section orientations.
 * @param scale         cross-section scales.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_line_set(const std::vector<vec3f> & coord, const std::vector<int32> & coord_index, bool color_per_vertex, const std::vector<color> & color, const std::vector<int32> & color_index)
 *
 * @brief Insert a line set into a display list.
 *
 * @param coord             coordinates.
 * @param coord_index       coordinate indices.
 * @param color_per_vertex  whether colors are applied per-vertex or per-face.
 * @param color             colors.
 * @param color_index       color indices.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_point_set(const std::vector<vec3f> & coord, const std::vector<color> & color)
 *
 * @brief Insert a point set into a display list.
 *
 * @param coord     points.
 * @param color     colors.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_shell(unsigned int mask, const std::vector<vec3f> & coord, const std::vector<int32> & coord_index, const std::vector<color> & color, const std::vector<int32> & color_index, const std::vector<vec3f> & normal, const std::vector<int32> & normal_index, const std::vector<vec2f> & tex_coord, const std::vector<int32> & tex_coord_index)
 *
 * @brief Insert a shell into a display list.
 *
 * @param mask
 * @param coord           coordinates.
 * @param coord_index     coordinate indices.
 * @param color           colors.
 * @param color_index     color indices.
 * @param normal          normals.
 * @param normal_index    normal indices.
 * @param tex_coord       texture coordinates.
 * @param tex_coord_index texture coordinate indices.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_sphere(float radius)
 *
 * @brief Insert a sphere into a display list.
 *
 * @param radius    sphere radius.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_dir_light(float ambient_intensity, float intensity , const color & color, const vec3f & direction)
 *
 * @brief Insert a directional light into a display list.
 *
 * @param ambient_intensity ambient intensity.
 * @param intensity         intensity.
 * @param color             color.
 * @param direction         direction.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_point_light(float ambient_intensity, const vec3f & attenuation, const color & color, float intensity, const vec3f & location, float radius)
 *
 * @brief Insert a point light into a display list.
 *
 * @param ambient_intensity ambient intensity.
 * @param attenuation       attenuation.
 * @param color             color.
 * @param intensity         intensity.
 * @param location          location.
 * @param radius            radius.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_spot_light(float ambient_intensity, const vec3f & attenuation, float beam_width, const color & color, float cut_off_angle, const vec3f & direction, float intensity, const vec3f & location, float radius)
 *
 * @brief Insert a point light into a display list.
 *
 * @param ambient_intensity ambient intensity.
 * @param attenuation       attenuation.
 * @param beam_width        beam width.
 * @param color             color.
 * @param cut_off_angle     cut-off angle.
 * @param direction         direction.
 * @param intensity         intensity.
 * @param location          location.
 * @param radius            radius.
 *
 * @return display object identifier.
 */

/**
 * @fn viewer::object_t viewer::insert_reference(object_t existing_object)
 *
 * @brief Insert a reference to an existing object into a display list.
 *
 * @return display object identifier.
 */

/**
 * @fn void viewer::remove_object(object_t ref)
 *
 * @brief Remove an object from the display list.
 *
 * @param ref   object handle.
 */

/**
 * @fn void viewer::enable_lighting(bool val)
 *
 * @brief Enable/disable lighting.
 *
 * @param val whether lighting should be enabled.
 */

/**
 * @fn void viewer::set_fog(const color & color, float visibility_range, const char * type)
 *
 * @brief Set the fog.
 *
 * @param color             fog color.
 * @param visibility_range  the distance at which objects are fully obscured by
 *                          fog.
 * @param type              fog type.
 */

/**
 * @fn void viewer::set_color(const color & rgb, float a)
 *
 * @brief Set the color.
 *
 * @param rgb   red, green, and blue components.
 * @param a     alpha (transparency) component.
 */

/**
 * @fn void viewer::set_material(float ambient_intensity, const color & diffuse_color, const color & emissive_color, float shininess, const color & specular_color, float transparency)
 *
 * @brief Set the material.
 *
 * @param ambient_intensity ambient intensity.
 * @param diffuse_color     diffuse color.
 * @param emissive_color    emissive color.
 * @param shininess         shininess.
 * @param specular_color    specular color.
 * @param transparency      transparency.
 */

/**
 * @fn void viewer::set_material_mode(size_t tex_components, bool geometry_color)
 *
 * @brief Set the material mode.
 *
 * @param tex_components    texture components.
 * @param geometry_color    geometry color.
 */

/**
 * @fn void viewer::set_sensitive(node * object)
 *
 * @brief Indicate that a node should be sensitive to the pointing device.
 *
 * @param object    a node.
 */

/**
 * @fn void viewer::scale_texture(size_t w, size_t h, size_t newW, size_t newH, size_t nc, unsigned char * pixels)
 *
 * @brief Scale a texture.
 *
 * @param w         current width.
 * @param h         current height,
 * @param newW      desired width.
 * @param newH      desired height.
 * @param nc        number of components.
 * @param pixels    pixel data.
 */

/**
 * @fn viewer::texture_object_t viewer::insert_texture(size_t w, size_t h, size_t nc, bool repeat_s, bool repeat_t, const unsigned char * pixels, bool retainHint)
 *
 * @brief Create a texture object.
 *
 * @param w             width.
 * @param h             height.
 * @param nc            number of components.
 * @param repeat_s      repeat in the S direction.
 * @param repeat_t      repeat in the T direction.
 * @param pixels        pixel data.
 * @param retainHint    whether the texture is likely to be reused.
 *
 * @return a handle to the inserted texture.
 */

/**
 * @fn void viewer::insert_texture_reference(texture_object_t ref, size_t components)
 *
 * @brief Insert a texture into the display list from an existing handle.
 *
 * @param ref           texture handle.
 * @param components    number of components.
 */

/**
 * @fn void viewer::remove_texture_object(texture_object_t ref)
 *
 * @brief Remove a texture from the display list.
 *
 * @param ref   texture handle.
 */

/**
 * @fn void viewer::set_texture_transform(const vec2f & center, float rotation, const vec2f & scale, const vec2f & translation)
 *
 * @brief Set the texture transform.
 *
 * @param center        center.
 * @param rotation      rotation.
 * @param scale         scale.
 * @param translation   translation.
 */

/**
 * @fn void viewer::set_viewpoint(const vec3f & position, const rotation & orientation, float field_of_view, float avatar_size, float visibility_limit)
 *
 * @brief Set the viewpoint.
 *
 * @param position          position.
 * @param orientation       orientation.
 * @param field_of_view     field of view.
 * @param avatar_size       avatar size.
 * @param visibility_limit  visiblity limit.
 */

/**
 * @fn void viewer::transform(const mat4f & mat)
 *
 * @brief Transform the modelview.
 *
 * Make the modelview matrix the result of multiplying @p mat by the current
 * modelview matrix.
 *
 * @param mat   transformation matrix.
 */

/**
 * @fn void viewer::transform_points(size_t nPoints, vec3f * point) const
 *
 * @brief Transform @p points by the current modelview matrix.
 *
 * @param nPoints   number of points.
 * @param point     pointer to the first point in an array.
 */

/**
 * Intersect the given bounding volume with the view volume. This
 * goes into the viewer to provide a hook for systems that use
 * non-standard view volumes. Most subclasses should be able to use
 * the default implementation provided here. If your view volume is
 * so strange that there's no way to cull to is, then reimplement to
 * always return bounding_volume::inside.
 *
 * @param bvolume   the bounding volume to intersect with the view volume.
 *
 * @return bounding_volume::inside, bounding_volume::outside, or
 *         bounding_volume::partial.
 */
bounding_volume::intersection
viewer::intersect_view_volume(const bounding_volume & bvolume) const
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
 * @fn void viewer::draw_bounding_sphere(const bounding_sphere & bs, bounding_volume::intersection intersection)
 *
 * @brief Draw a bounding sphere.
 *
 * Used for debugging view culling. Probably should be draw_bounding_volume and
 * handle axis_aligned_bounding_boxes as well.
 *
 * @param bs            a bounding sphere; if max, will not be drawn
 * @param intersection  one of the bvolume intersection test constants, or 4
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
const frustum & viewer::frustum() const
{
    return this->frustum_;
}

/**
 * @brief Set the frustum.
 *
 * @param f new frustum value.
 */
void viewer::frustum(const openvrml::frustum & f)
{
    this->frustum_ = f;
}

} // namespace openvrml
