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

# ifndef OPENVRML_VIEWER_H
#   define OPENVRML_VIEWER_H

#   include <cstddef>
#   include <vector>
#   include <boost/utility.hpp>
#   include <openvrml/bounding_volume.h>
#   include <openvrml/frustum.h>

namespace openvrml {

    class node;
    class browser;

    class OPENVRML_API viewer : boost::noncopyable {
        friend class browser;

        openvrml::browser * browser_;

    protected:
        openvrml::frustum frustum_;

    public:
        enum {
            mask_none                 = 0,
            mask_ccw                  = 1,
            mask_convex               = 2,
            mask_solid                = 4,
            mask_bottom               = 8,
            mask_top                  = 16,
            mask_side                 = 32,
            mask_color_per_vertex     = 64,
            mask_normal_per_vertex    = 128
        };

        enum rendering_mode {
            draw_mode,
            pick_mode
        };

        typedef long object_t;
        typedef long texture_object_t;

        virtual ~viewer() OPENVRML_NOTHROW = 0;

        openvrml::browser * browser() const OPENVRML_NOTHROW;

        virtual rendering_mode mode() = 0;
        virtual double frame_rate() = 0;
        virtual void reset_user_navigation() = 0;

        virtual object_t begin_object(const char * id,
                                      bool retain = false) = 0;
        virtual void end_object() = 0;

        virtual object_t
        insert_background(const std::vector<float> & ground_angle,
                          const std::vector<color> & ground_color,
                          const std::vector<float> & sky_angle,
                          const std::vector<color> & sky_color,
                          const image & front,
                          const image & back,
                          const image & left,
                          const image & right,
                          const image & top,
                          const image & bottom) = 0;

        virtual object_t insert_box(const vec3f & size) = 0;
        virtual object_t insert_cone(float height, float radius, bool bottom,
                                     bool side) = 0;
        virtual object_t
        insert_cylinder(float height, float radius, bool bottom, bool side,
                        bool top) = 0;
        virtual object_t
        insert_elevation_grid(unsigned int mask,
                              const std::vector<float> & height,
                              int32 x_dimension, int32 z_dimension,
                              float x_spacing, float z_spacing,
                              const std::vector<color> & color,
                              const std::vector<vec3f> & normal,
                              const std::vector<vec2f> & tex_coord) = 0;
        virtual object_t
        insert_extrusion(unsigned int mask,
                         const std::vector<vec3f> & spine,
                         const std::vector<vec2f> & cross_section,
                         const std::vector<rotation> & orientation,
                         const std::vector<vec2f> & scale) = 0;
        virtual object_t
        insert_line_set(const std::vector<vec3f> & coord,
                        const std::vector<int32> & coord_index,
                        bool color_per_vertex,
                        const std::vector<color> & color,
                        const std::vector<int32> & color_index) = 0;
        virtual object_t
        insert_point_set(const std::vector<vec3f> & coord,
                         const std::vector<color> & color) = 0;
        virtual object_t
        insert_shell(unsigned int mask,
                     const std::vector<vec3f> & coord,
                     const std::vector<int32> & coord_index,
                     const std::vector<color> & color,
                     const std::vector<int32> & color_index,
                     const std::vector<vec3f> & normal,
                     const std::vector<int32> & normal_index,
                     const std::vector<vec2f> & tex_coord,
                     const std::vector<int32> & tex_coord_index) = 0;
        virtual object_t insert_sphere(float radius) = 0;
        virtual object_t insert_dir_light(float ambient_intensity,
                                          float intensity,
                                          const color & color,
                                          const vec3f & direction) = 0;
        virtual object_t insert_point_light(float ambient_intensity,
                                            const vec3f & attenuation,
                                            const color & color,
                                            float intensity,
                                            const vec3f & location,
                                            float radius) = 0;
        virtual object_t insert_spot_light(float ambient_intensity,
                                           const vec3f & attenuation,
                                           float beam_width,
                                           const color & color,
                                           float cut_off_angle,
                                           const vec3f & direction,
                                           float intensity,
                                           const vec3f & location,
                                           float radius) = 0;
        virtual object_t insert_reference(object_t existing_object) = 0;

        virtual void remove_object(object_t ref) = 0;

        virtual void enable_lighting(bool val) = 0;

        virtual void set_fog(const color & color, float visibility_range,
                             const char * type) = 0;

        virtual void set_color(const color & rgb, float a = 1.0) = 0;

        virtual void set_material(float ambient_intensity,
                                  const color & diffuse_color,
                                  const color & emissive_color,
                                  float shininess,
                                  const color & specular_color,
                                  float transparency) = 0;

        virtual void set_material_mode(size_t tex_components,
                                       bool geometry_color) = 0;

        virtual void set_sensitive(node * object) = 0;

        virtual texture_object_t insert_texture(const image & img,
                                                bool repeat_s,
                                                bool repeat_t,
                                                bool retainHint = false) = 0;

        virtual void insert_texture_reference(texture_object_t ref,
                                              size_t components) = 0;
        virtual void remove_texture_object(texture_object_t ref) = 0;

        virtual void set_texture_transform(const vec2f & center,
                                           float rotation,
                                           const vec2f & scale,
                                           const vec2f & translation) = 0;

        virtual void set_viewpoint(const vec3f & position,
                                   const rotation & orientation,
                                   float field_of_view,
                                   float avatar_size,
                                   float visibility_limit) = 0;

        virtual void transform(const mat4f & mat) = 0;

        virtual void transform_points(size_t nPoints, vec3f * point) const = 0;

        // still working on some navigation api issues, so don't depend on
        // thses yet. there's a default implementation in any case, so you
        // shouldn't have to worry about it.
        //
        virtual const openvrml::frustum& frustum() const;
        virtual void frustum(const openvrml::frustum & f);

        virtual bounding_volume::intersection
        intersect_view_volume(const bounding_volume & bvolume) const;

        virtual void
        draw_bounding_sphere(const bounding_sphere & bs,
                             bounding_volume::intersection intersection) = 0;

    protected:
        viewer() OPENVRML_NOTHROW;
    };
}

# endif // OPENVRML_VIEWER_H
