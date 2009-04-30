// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2003, 2004, 2005, 2006, 2007  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

/**
 * @file openvrml/viewer.h
 *
 * @brief Definition of @c openvrml::viewer.
 */

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <private.h>
# include "viewer.h"

/**
 * @class openvrml::viewer openvrml/viewer.h
 *
 * @brief Map the %scene graph to the underlying graphics library.
 */

/**
 * @var openvrml::browser * openvrml::viewer::browser_
 *
 * @brief A pointer to the @c browser with which the @c viewer is currently
 *        associated.
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
 * @brief A pointer to the @c browser with which the @c viewer is currently
 *        associated.
 *
 * @return a pointer to the @c browser with which the @c viewer is currently
 *         associated, or 0 if the @c viewer is not associated with a
 *         @c browser.
 */
openvrml::browser * openvrml::viewer::browser() const OPENVRML_NOTHROW
{
    return this->browser_;
}

/**
 * @brief Get the rendering mode.
 *
 * This function delegates to @c viewer::do_mode.
 *
 * @return the rendering mode.
 */
openvrml::viewer::rendering_mode openvrml::viewer::mode()
{
    return this->do_mode();
}

/**
 * @fn openvrml::viewer::rendering_mode openvrml::viewer::do_mode()
 *
 * @brief Get the rendering mode.
 *
 * @return the rendering mode.
 */

/**
 * @brief Get the frame rate.
 *
 * This function delegates to @c viewer::do_frame_rate.
 *
 * @return the frame rate.
 */
double openvrml::viewer::frame_rate()
{
    return this->do_frame_rate();
}

/**
 * @fn double openvrml::viewer::do_frame_rate()
 *
 * @brief Get the frame rate.
 *
 * @return the frame rate.
 */

/**
 * @brief Return view to the last bound Viewpoint.
 *
 * This function delegates to @c viewer::do_reset_user_navigation.
 */
void openvrml::viewer::reset_user_navigation()
{
    this->do_reset_user_navigation();
}

/**
 * @fn void openvrml::viewer::do_reset_user_navigation()
 *
 * @brief Return view to the last bound Viewpoint.
 */

/**
 * @brief Begin a display list.
 *
 * @param[in] id        object identifier.
 * @param[in] retain    whether the object should be retained for reuse.
 *
 * This function delegates to @c viewer::do_begin_object.
 */
void openvrml::viewer::begin_object(const char * id, bool retain)
{
    return this->do_begin_object(id, retain);
}

/**
 * @fn void openvrml::viewer::do_begin_object(const char * id, bool retain)
 *
 * @brief Begin a display list.
 *
 * @param[in] id        object identifier.
 * @param[in] retain    whether the object should be retained for reuse.
 */

/**
 * @brief End a display list.
 *
 * This function delegates to @c viewer::do_end_object.
 */
void openvrml::viewer::end_object()
{
    this->do_end_object();
}

/**
 * @fn void openvrml::viewer::do_end_object()
 *
 * @brief End a display list.
 */

/**
 * @brief Insert a background into a display list.
 *
 * This function delegates to @c viewer::do_insert_background.
 *
 * @param[in] n a @c background_node.
 */
void openvrml::viewer::insert_background(const background_node & n)
{
    return this->do_insert_background(n);
}

/**
 * @fn void openvrml::viewer::do_insert_background(const background_node & n)
 *
 * @brief Insert a background into a display list.
 *
 * @param[in] n a @c background_node.
 */

/**
 * @brief Insert a box into a display list.
 *
 * This function delegates to @c viewer::do_insert_box.
 *
 * @param[in] n     the @c geometry_node corresponding to the box.
 * @param[in] size  box dimensions.
 */
void openvrml::viewer::insert_box(const geometry_node & n, const vec3f & size)
{
    this->do_insert_box(n, size);
}

/**
 * @fn void openvrml::viewer::do_insert_box(const geometry_node & n, const vec3f & size)
 *
 * @brief Insert a box into a display list.
 *
 * @param[in] n     the @c geometry_node corresponding to the box.
 * @param[in] size  box dimensions.
 */

/**
 * @brief Insert a cone into a display list.
 *
 * This function delegates to @c viewer::do_insert_cone.
 *
 * @param[in] n         the @c geometry_node corresponding to the cone.
 * @param[in] height    height.
 * @param[in] radius    radius at base.
 * @param[in] bottom    show the bottom.
 * @param[in] side      show the side.
 */
void openvrml::viewer::insert_cone(const geometry_node & n,
                                   const float height,
                                   const float radius,
                                   const bool bottom,
                                   const bool side)
{
    this->do_insert_cone(n, height, radius, bottom, side);
}

/**
 * @fn void openvrml::viewer::do_insert_cone(const geometry_node & n, float height, float radius, bool bottom, bool side)
 *
 * @brief Insert a cone into a display list.
 *
 * @param[in] n         the @c geometry_node corresponding to the cone.
 * @param[in] height    height.
 * @param[in] radius    radius at base.
 * @param[in] bottom    show the bottom.
 * @param[in] side      show the side.
 */

/**
 * @brief Insert a cylinder into a display list.
 *
 * This function delegates to @c viewer::do_insert_cylinder.
 *
 * @param[in] n         the @c geometry_node corresponding to the cylinder.
 * @param[in] height    height.
 * @param[in] radius    radius.
 * @param[in] bottom    show the bottom.
 * @param[in] side      show the side.
 * @param[in] top       show the top.
 */
void openvrml::viewer::insert_cylinder(const geometry_node & n,
                                       const float height,
                                       const float radius,
                                       const bool bottom,
                                       const bool side,
                                       const bool top)
{
    this->do_insert_cylinder(n, height, radius, bottom, side, top);
}

/**
 * @fn void openvrml::viewer::do_insert_cylinder(const geometry_node & n, float height, float radius, bool bottom, bool side, bool top)
 *
 * @brief Insert a cylinder into a display list.
 *
 * @param[in] n         the @c geometry_node corresponding to the cylinder.
 * @param[in] height    height.
 * @param[in] radius    radius.
 * @param[in] bottom    show the bottom.
 * @param[in] side      show the side.
 * @param[in] top       show the top.
 */

/**
 * @brief Insert an elevation grid into a display list.
 *
 * This function delegates to @c viewer::do_insert_elevation_grid.
 *
 * @param[in] n             the @c geometry_node corresponding to the elevation
 *                          grid.
 * @param[in] mask
 * @param[in] height        height field.
 * @param[in] x_dimension   vertices in the x direction.
 * @param[in] z_dimension   vertices in the z direction.
 * @param[in] x_spacing     distance between vertices in the x direction.
 * @param[in] z_spacing     distance between vertices in the z direction.
 * @param[in] color         colors.
 * @param[in] normal        normals.
 * @param[in] tex_coord     texture coordinates.
 */
void
openvrml::viewer::insert_elevation_grid(const geometry_node & n,
                                        const unsigned int mask,
                                        const std::vector<float> & height,
                                        int32 x_dimension,
                                        int32 z_dimension,
                                        float x_spacing,
                                        float z_spacing,
                                        const std::vector<color> & color,
                                        const std::vector<vec3f> & normal,
                                        const std::vector<vec2f> & tex_coord)
{
    this->do_insert_elevation_grid(n, mask, height,
                                   x_dimension, z_dimension,
                                   x_spacing, z_spacing,
                                   color, normal, tex_coord);
}

/**
 * @fn void openvrml::viewer::do_insert_elevation_grid(const geometry_node & n, unsigned int mask, const std::vector<float> & height, int32 x_dimension, int32 z_dimension, float x_spacing, float z_spacing, const std::vector<color> & color, const std::vector<vec3f> & normal, const std::vector<vec2f> & tex_coord)
 *
 * @brief Insert an elevation grid into a display list.
 *
 * @param[in] n             the @c geometry_node corresponding to the elevation
 *                          grid.
 * @param[in] mask
 * @param[in] height        height field.
 * @param[in] x_dimension   vertices in the x direction.
 * @param[in] z_dimension   vertices in the z direction.
 * @param[in] x_spacing     distance between vertices in the x direction.
 * @param[in] z_spacing     distance between vertices in the z direction.
 * @param[in] color         colors.
 * @param[in] normal        normals.
 * @param[in] tex_coord     texture coordinates.
 */

/**
 * @brief Insert an extrusion into a display list.
 *
 * This function delegates to @c viewer::do_insert_extrusion.
 *
 * @param[in] n             the @c geometry_node corresponding to the extrusion.
 * @param[in] mask
 * @param[in] spine         spine points.
 * @param[in] cross_section cross-sections.
 * @param[in] orientation   cross-section orientations.
 * @param[in] scale         cross-section scales.
 */
void
openvrml::viewer::insert_extrusion(const geometry_node & n,
                                   unsigned int mask,
                                   const std::vector<vec3f> & spine,
                                   const std::vector<vec2f> & cross_section,
                                   const std::vector<rotation> & orientation,
                                   const std::vector<vec2f> & scale)
{
    this->do_insert_extrusion(n, mask,
                              spine, cross_section, orientation, scale);
}

/**
 * @fn void openvrml::viewer::do_insert_extrusion(const geometry_node & n, unsigned int mask, const std::vector<vec3f> & spine, const std::vector<vec2f> & cross_section, const std::vector<rotation> & orientation, const std::vector<vec2f> & scale)
 *
 * @brief Insert an extrusion into a display list.
 *
 * @param[in] n             the @c geometry_node corresponding to the extrusion.
 * @param[in] mask
 * @param[in] spine         spine points.
 * @param[in] cross_section cross-sections.
 * @param[in] orientation   cross-section orientations.
 * @param[in] scale         cross-section scales.
 */

/**
 * @brief Insert a line set into a display list.
 *
 * This function delegates to @c viewer::do_insert_line_set.
 *
 * @param[in] n                 the @c geometry_node corresponding to the line
 *                              set.
 * @param[in] coord             coordinates.
 * @param[in] coord_index       coordinate indices.
 * @param[in] color_per_vertex  whether colors are applied per-vertex or
 *                              per-face.
 * @param[in] color             colors.
 * @param[in] color_index       color indices.
 */
void
openvrml::viewer::insert_line_set(const geometry_node & n,
                                  const std::vector<vec3f> & coord,
                                  const std::vector<int32> & coord_index,
                                  const bool color_per_vertex,
                                  const std::vector<color> & color,
                                  const std::vector<int32> & color_index)
{
    this->do_insert_line_set(n, coord, coord_index,
                             color_per_vertex, color, color_index);
}

/**
 * @fn void openvrml::viewer::do_insert_line_set(const geometry_node & n, const std::vector<vec3f> & coord, const std::vector<int32> & coord_index, bool color_per_vertex, const std::vector<color> & color, const std::vector<int32> & color_index)
 *
 * @brief Insert a line set into a display list.
 *
 * @param[in] n                 the @c geometry_node corresponding to the line
 *                              set.
 * @param[in] coord             coordinates.
 * @param[in] coord_index       coordinate indices.
 * @param[in] color_per_vertex  whether colors are applied per-vertex or per-face.
 * @param[in] color             colors.
 * @param[in] color_index       color indices.
 */

/**
 * @brief Insert a point set into a display list.
 *
 * This function delegates to @c viewer::insert_point_set.
 *
 * @param[in] n         the @c geometry_node corresponding to the point set.
 * @param[in] coord     points.
 * @param[in] color     colors.
 */
void
openvrml::viewer::insert_point_set(const geometry_node & n,
                                   const std::vector<vec3f> & coord,
                                   const std::vector<color> & color)
{
    this->do_insert_point_set(n, coord, color);
}

/**
 * @fn void openvrml::viewer::do_insert_point_set(const geometry_node & n, const std::vector<vec3f> & coord, const std::vector<color> & color)
 *
 * @brief Insert a point set into a display list.
 *
 * @param[in] n         the @c geometry_node corresponding to the point set.
 * @param[in] coord     points.
 * @param[in] color     colors.
 */

/**
 * @brief Insert a shell into a display list.
 *
 * This function delegates to @c viewer::do_insert_shell.
 *
 * @param[in] n               the @c geometry_node corresponding to the shell.
 * @param[in] mask
 * @param[in] coord           coordinates.
 * @param[in] coord_index     coordinate indices.
 * @param[in] color           colors.
 * @param[in] color_index     color indices.
 * @param[in] normal          normals.
 * @param[in] normal_index    normal indices.
 * @param[in] tex_coord       texture coordinates.
 * @param[in] tex_coord_index texture coordinate indices.
 */
void
openvrml::viewer::insert_shell(const geometry_node & n,
                               unsigned int mask,
                               const std::vector<vec3f> & coord,
                               const std::vector<int32> & coord_index,
                               const std::vector<color> & color,
                               const std::vector<int32> & color_index,
                               const std::vector<vec3f> & normal,
                               const std::vector<int32> & normal_index,
                               const std::vector<vec2f> & tex_coord,
                               const std::vector<int32> & tex_coord_index)
{
    this->do_insert_shell(n, mask,
                          coord, coord_index,
                          color, color_index,
                          normal, normal_index,
                          tex_coord, tex_coord_index);
}

/**
 * @fn void openvrml::viewer::do_insert_shell(const geometry_node & n, unsigned int mask, const std::vector<vec3f> & coord, const std::vector<int32> & coord_index, const std::vector<color> & color, const std::vector<int32> & color_index, const std::vector<vec3f> & normal, const std::vector<int32> & normal_index, const std::vector<vec2f> & tex_coord, const std::vector<int32> & tex_coord_index)
 *
 * @brief Insert a shell into a display list.
 *
 * @param[in] n               the @c geometry_node corresponding to the shell.
 * @param[in] mask
 * @param[in] coord           coordinates.
 * @param[in] coord_index     coordinate indices.
 * @param[in] color           colors.
 * @param[in] color_index     color indices.
 * @param[in] normal          normals.
 * @param[in] normal_index    normal indices.
 * @param[in] tex_coord       texture coordinates.
 * @param[in] tex_coord_index texture coordinate indices.
 */

/**
 * @brief Insert a sphere into a display list.
 *
 * This function delegates to @c viewer::do_insert_sphere.
 *
 * @param[in] n         the @c geometry_node corresponding to the sphere.
 * @param[in] radius    sphere radius.
 */
void openvrml::viewer::insert_sphere(const geometry_node & n,
                                     const float radius)
{
    this->do_insert_sphere(n, radius);
}

/**
 * @fn void openvrml::viewer::do_insert_sphere(const geometry_node & n, float radius)
 *
 * @brief Insert a sphere into a display list.
 *
 * @param[in] n         the @c geometry_node corresponding to the sphere.
 * @param[in] radius    sphere radius.
 */

/**
 * @brief Insert a directional light into a display list.
 *
 * This function delegates to @c viewer::do_insert_dir_light.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] intensity         intensity.
 * @param[in] color             color.
 * @param[in] direction         direction.
 */
void openvrml::viewer::insert_dir_light(const float ambient_intensity,
                                        const float intensity,
                                        const color & color,
                                        const vec3f & direction)
{
    return this->do_insert_dir_light(ambient_intensity,
                                     intensity,
                                     color,
                                     direction);
}

/**
 * @fn void openvrml::viewer::do_insert_dir_light(float ambient_intensity, float intensity , const color & color, const vec3f & direction)
 *
 * @brief Insert a directional light into a display list.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] intensity         intensity.
 * @param[in] color             color.
 * @param[in] direction         direction.
 */

/**
 * @brief Insert a point light into a display list.
 *
 * This function delegates to @c viewer::do_insert_point_light.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] attenuation       attenuation.
 * @param[in] color             color.
 * @param[in] intensity         intensity.
 * @param[in] location          location.
 * @param[in] radius            radius.
 */
void openvrml::viewer::insert_point_light(const float ambient_intensity,
                                          const vec3f & attenuation,
                                          const color & color,
                                          const float intensity,
                                          const vec3f & location,
                                          const float radius)
{
    return this->do_insert_point_light(ambient_intensity,
                                       attenuation,
                                       color,
                                       intensity,
                                       location,
                                       radius);
}

/**
 * @fn void openvrml::viewer::do_insert_point_light(float ambient_intensity, const vec3f & attenuation, const color & color, float intensity, const vec3f & location, float radius)
 *
 * @brief Insert a point light into a display list.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] attenuation       attenuation.
 * @param[in] color             color.
 * @param[in] intensity         intensity.
 * @param[in] location          location.
 * @param[in] radius            radius.
 */

/**
 * @brief Insert a point light into a display list.
 *
 * This function delegates to @c viewer::do_insert_spot_light.
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
 */
void openvrml::viewer::insert_spot_light(const float ambient_intensity,
                                         const vec3f & attenuation,
                                         const float beam_width,
                                         const color & color,
                                         const float cut_off_angle,
                                         const vec3f & direction,
                                         const float intensity,
                                         const vec3f & location,
                                         const float radius)
{
    return this->do_insert_spot_light(ambient_intensity,
                                      attenuation,
                                      beam_width,
                                      color,
                                      cut_off_angle,
                                      direction,
                                      intensity,
                                      location,
                                      radius);
}

/**
 * @fn void openvrml::viewer::do_insert_spot_light(float ambient_intensity, const vec3f & attenuation, float beam_width, const color & color, float cut_off_angle, const vec3f & direction, float intensity, const vec3f & location, float radius)
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
 */

/**
 * @brief Remove an object from the display list.
 *
 * This function delegates to @c viewer::do_remove_object.
 *
 * @param[in] ref   object handle.
 */
void openvrml::viewer::remove_object(const node & ref)
{
    this->do_remove_object(ref);
}

/**
 * @fn void openvrml::viewer::do_remove_object(const node & ref)
 *
 * @brief Remove an object from the display list.
 *
 * @param[in] ref   object handle.
 */

/**
 * @brief Enable/disable lighting.
 *
 * This function delegates to @c viewer::do_enable_lighting.
 *
 * @param[in] val whether lighting should be enabled.
 */
void openvrml::viewer::enable_lighting(const bool val)
{
    this->do_enable_lighting(val);
}

/**
 * @fn void openvrml::viewer::do_enable_lighting(bool val)
 *
 * @brief Enable/disable lighting.
 *
 * @param[in] val whether lighting should be enabled.
 */

/**
 * @brief Set the fog.
 *
 * This function delegates to @c viewer::do_set_fog.
 *
 * @param[in] color             fog color.
 * @param[in] visibility_range  the distance at which objects are fully obscured by
 *                              fog.
 * @param[in] type              fog type.
 */
void openvrml::viewer::set_fog(const color & color,
                               const float visibility_range,
                               const char * const type)
{
    this->do_set_fog(color, visibility_range, type);
}

/**
 * @fn void openvrml::viewer::do_set_fog(const color & color, float visibility_range, const char * type)
 *
 * @brief Set the fog.
 *
 * @param[in] color             fog color.
 * @param[in] visibility_range  the distance at which objects are fully obscured by
 *                              fog.
 * @param[in] type              fog type.
 */

/**
 * @brief Set the color.
 *
 * This function delegates to @c viewer::do_set_color.
 *
 * @param[in] rgb   red, green, and blue components.
 * @param[in] a     alpha (transparency) component.
 */
void openvrml::viewer::set_color(const color & rgb, float a)
{
    this->do_set_color(rgb, a);
}

/**
 * @fn void openvrml::viewer::do_set_color(const color & rgb, float a)
 *
 * @brief Set the color.
 *
 * @param[in] rgb   red, green, and blue components.
 * @param[in] a     alpha (transparency) component.
 */

/**
 * @brief Set the material.
 *
 * This function delegates to @c viewer::do_set_material.
 *
 * @param[in] ambient_intensity ambient intensity.
 * @param[in] diffuse_color     diffuse color.
 * @param[in] emissive_color    emissive color.
 * @param[in] shininess         shininess.
 * @param[in] specular_color    specular color.
 * @param[in] transparency      transparency.
 */
void openvrml::viewer::set_material(const float ambient_intensity,
                                    const color & diffuse_color,
                                    const color & emissive_color,
                                    const float shininess,
                                    const color & specular_color,
                                    const float transparency)
{
    this->do_set_material(ambient_intensity,
                          diffuse_color,
                          emissive_color,
                          shininess,
                          specular_color,
                          transparency);
}

/**
 * @fn void openvrml::viewer::do_set_material(float ambient_intensity, const color & diffuse_color, const color & emissive_color, float shininess, const color & specular_color, float transparency)
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
 * @brief Set the material mode.
 *
 * This function delegates to @c viewer::do_set_material_mode.
 *
 * @param[in] tex_components    texture components.
 * @param[in] geometry_color    geometry color.
 */
void openvrml::viewer::set_material_mode(const size_t tex_components,
                                         const bool geometry_color)
{
    this->do_set_material_mode(tex_components, geometry_color);
}

/**
 * @fn void openvrml::viewer::do_set_material_mode(size_t tex_components, bool geometry_color)
 *
 * @brief Set the material mode.
 *
 * @param[in] tex_components    texture components.
 * @param[in] geometry_color    geometry color.
 */

/**
 * @brief Indicate that a node should be sensitive to the pointing device.
 *
 * This function delegates to @c viewer::do_set_sensitive.
 *
 * @param[in] object    a node.
 */
void openvrml::viewer::set_sensitive(node * const object)
{
    this->do_set_sensitive(object);
}

/**
 * @fn void openvrml::viewer::do_set_sensitive(node * object)
 *
 * @brief Indicate that a node should be sensitive to the pointing device.
 *
 * @param[in] object    a node.
 */

/**
 * @brief Create a texture object.
 *
 * This function delegates to @c viewer::do_insert_texture.
 *
 * @param[in] n             a @c texture_node.
 * @param[in] retainHint    whether the texture is likely to be reused.
 */
void openvrml::viewer::insert_texture(const texture_node & n,
                                      const bool retainHint)
{
    return this->do_insert_texture(n, retainHint);
}

/**
 * @fn void openvrml::viewer::do_insert_texture(const texture_node & n, bool retainHint)
 *
 * @brief Create a texture object.
 *
 * @param[in] n             a @c texture_node.
 * @param[in] retainHint    whether the texture is likely to be reused.
 */

/**
 * @brief Remove a texture from the display list.
 *
 * This function delegates to @c viewer::do_remove_texture_object.
 *
 * @param[in] ref   texture handle.
 */
void openvrml::viewer::remove_texture_object(const texture_node & ref)
{
    this->do_remove_texture_object(ref);
}

/**
 * @fn void openvrml::viewer::do_remove_texture_object(const texture_node & ref)
 *
 * @brief Remove a texture from the display list.
 *
 * @param[in] ref   a @c texture_node.
 */

/**
 * @brief Set the texture transform.
 *
 * This function delegates to @c viewer::do_set_texture_transform.
 *
 * @param[in] center        center.
 * @param[in] rotation      rotation.
 * @param[in] scale         scale.
 * @param[in] translation   translation.
 */
void openvrml::viewer::set_texture_transform(const vec2f & center,
                                             const float rotation,
                                             const vec2f & scale,
                                             const vec2f & translation)
{
    this->do_set_texture_transform(center, rotation, scale, translation);
}

/**
 * @fn void openvrml::viewer::do_set_texture_transform(const vec2f & center, float rotation, const vec2f & scale, const vec2f & translation)
 *
 * @brief Set the texture transform.
 *
 * @param[in] center        center.
 * @param[in] rotation      rotation.
 * @param[in] scale         scale.
 * @param[in] translation   translation.
 */

/**
 * @brief Set the frustum.
 *
 * This function delegates to @c viewer::do_set_frustum.
 *
 * @param[in] field_of_view     field of view.
 * @param[in] avatar_size       avatar size.
 * @param[in] visibility_limit  visibility limit.
 */
void openvrml::viewer::set_frustum(const float field_of_view,
                                   const float avatar_size,
                                   const float visibility_limit)
{
    this->do_set_frustum(field_of_view, avatar_size, visibility_limit);
}

/**
 * @fn void openvrml::viewer::do_set_frustum(float field_of_view, float avatar_size, float visibility_limit)
 *
 * @brief Set the frustum.
 *
 * @param[in] field_of_view     field of view.
 * @param[in] avatar_size       avatar size.
 * @param[in] visibility_limit  visibility limit.
 */

/**
 * @brief Set the viewpoint.
 *
 * This function delegates to @c viewer::do_set_viewpoint.
 *
 * @param[in] position          position.
 * @param[in] orientation       orientation.
 * @param[in] avatar_size       avatar size.
 * @param[in] visibility_limit  visiblity limit.
 */
void openvrml::viewer::set_viewpoint(const vec3f & position,
                                     const rotation & orientation,
                                     const float avatar_size,
                                     const float visibility_limit)
{
    this->do_set_viewpoint(position,
                           orientation,
                           avatar_size,
                           visibility_limit);
}

/**
 * @fn void openvrml::viewer::do_set_viewpoint(const vec3f & position, const rotation & orientation, float avatar_size, float visibility_limit)
 *
 * @brief Set the viewpoint.
 *
 * @param[in] position          position.
 * @param[in] orientation       orientation.
 * @param[in] avatar_size       avatar size.
 * @param[in] visibility_limit  visiblity limit.
 */

/**
 * @brief Transform the modelview.
 *
 * Make the modelview matrix the result of multiplying @p mat by the current
 * modelview matrix.
 *
 * This function delegates to @c viewer::do_transform.
 *
 * @param[in] mat   transformation matrix.
 */
void openvrml::viewer::transform(const mat4f & mat)
{
    this->do_transform(mat);
}

/**
 * @fn void openvrml::viewer::do_transform(const mat4f & mat)
 *
 * @brief Transform the modelview.
 *
 * Make the modelview matrix the result of multiplying @p mat by the current
 * modelview matrix.
 *
 * @param[in] mat   transformation matrix.
 */

/**
 * @brief Transform @p points by the current modelview matrix.
 *
 * This function delegates to @c viewer::do_transform_points.
 *
 * @param[in] nPoints   number of points.
 * @param[in] point     pointer to the first point in an array.
 */
void openvrml::viewer::transform_points(const size_t nPoints,
                                        vec3f * const point) const
{
    this->do_transform_points(nPoints, point);
}

/**
 * @fn void openvrml::viewer::do_transform_points(size_t nPoints, vec3f * point) const
 *
 * @brief Transform @p points by the current modelview matrix.
 *
 * @param[in] nPoints   number of points.
 * @param[in] point     pointer to the first point in an array.
 */

/**
 * @brief Intersect the given bounding volume with the view volume.
 *
 * This function delegates to @c viewer::do_intersect_view_volume.
 *
 * @param[in] bvolume   the bounding volume to intersect with the view volume.
 *
 * @return bounding_volume::inside, bounding_volume::outside, or
 *         bounding_volume::partial.
 */
openvrml::bounding_volume::intersection
openvrml::viewer::intersect_view_volume(const bounding_volume & bvolume) const
{
    return this->do_intersect_view_volume(bvolume);
}

/**
 * @brief Intersect the given bounding volume with the view volume.
 *
 * This goes into the viewer to provide a hook for systems that use
 * nonstandard view volumes.  Most subclasses should be able to use the
 * default implementation provided here.  If your view volume is so strange
 * that there's no way to cull to it, then reimplement to return
 * @c bounding_volume::inside always.
 *
 * @param[in] bvolume   the bounding volume to intersect with the view volume.
 *
 * @return @c bounding_volume::inside, @c bounding_volume::outside, or
 *         @c bounding_volume::partial.
 */
openvrml::bounding_volume::intersection
openvrml::viewer::do_intersect_view_volume(const bounding_volume & bvolume) const
{
    //
    // For normal VRML97 use, this won't need to be overridden, but for
    // systems with non-standard view volumes, this can be changed to cull as
    // appropriate. Note that culling can be disabled by setting a flag in
    // rendering_context. Since I don't have access to the appropriate
    // cave/boom/whichever api's, I can't be sure that this is enough. If it
    // isn't, please express any concerns to the OpenVRML developer's list,
    // and it can be fixed...
    //
    return bvolume.intersect_frustum(this->frustum_);
}

/**
 * @brief Draw a bounding sphere.
 *
 * Used for debugging view culling.  Probably should be draw_bounding_volume
 * and handle axis_aligned_bounding_boxes as well.
 *
 * This function delegates to @c viewer::do_draw_bounding_sphere.
 *
 * @param[in] bs            a bounding sphere; if max, will not be drawn
 * @param[in] intersection  one of the bvolume intersection test constants, or 4
 *                          to draw in unique way. (useful for debugging)
 */
void
openvrml::viewer::
draw_bounding_sphere(const bounding_sphere & bs,
                     const bounding_volume::intersection intersection)
{
    this->do_draw_bounding_sphere(bs, intersection);
}

/**
 * @fn void openvrml::viewer::do_draw_bounding_sphere(const bounding_sphere & bs, bounding_volume::intersection intersection)
 *
 * @brief Draw a bounding sphere.
 *
 * Used for debugging view culling.  Probably should be draw_bounding_volume
 * and handle axis_aligned_bounding_boxes as well.
 *
 * @param[in] bs            a bounding sphere; if max, will not be drawn
 * @param[in] intersection  one of the bvolume intersection test constants, or 4
 *                          to draw in unique way. (useful for debugging)
 */

/**
 * @brief The frustum.
 *
 * This function delegates to @c viewer::do_frustum.
 *
 * @todo We're forcing everybody to carry around a frustum whether they want
 *       it or not.  It shouldn't be used except for debugging and stuff since
 *       it might not be valid in some implementations
 *
 * @return the frustum.
 */
const openvrml::frustum & openvrml::viewer::frustum() const
{
    return this->do_frustum();
}

/**
 * @todo We're forcing everybody to carry around a frustum whether they want
 *       it or not.  It shouldn't be used except for debugging and stuff since
 *       it might not be valid in some implementations
 *
 * @return the frustum.
 */
const openvrml::frustum & openvrml::viewer::do_frustum() const
{
    return this->frustum_;
}

/**
 * @brief Set the frustum.
 *
 * This function delegates to
 * <code>viewer::do_frustum(const openvrml::frustum &)</code>.
 *
 * @param[in] f new frustum value.
 */
void openvrml::viewer::frustum(const openvrml::frustum & f)
{
    this->do_frustum(f);
}

/**
 * @brief Set the frustum.
 *
 * @param[in] f new frustum value.
 */
void openvrml::viewer::do_frustum(const openvrml::frustum & f)
{
    this->frustum_ = f;
}
