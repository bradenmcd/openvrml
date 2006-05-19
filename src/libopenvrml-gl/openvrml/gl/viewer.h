// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998-2000  Chris Morley
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

# ifndef OPENVRML_GL_VIEWER_H
#   define OPENVRML_GL_VIEWER_H

#   include <openvrml-gl-common.h>
#   include <stack>
#   include <openvrml/viewer.h>

// Use the stencil buffer to set the SHAPE mask.
#   define USE_STENCIL_SHAPE 0

extern "C" struct GLUtesselator;

namespace openvrml {

    namespace gl {

        class OPENVRML_GL_API viewer : public openvrml::viewer {
        public:
            enum { max_lights = 8 };

            enum light_type {
                light_unused,
                light_directional,
                light_positional
            };

            enum event_type {
                event_key_down,
                event_mouse_move,
                event_mouse_click,
                event_mouse_drag,
                event_mouse_release
            };

            enum {
                key_home,
                key_left,
                key_up,
                key_right,
                key_down,
                key_page_up,
                key_page_down
            };

            struct event_info {
                event_type event;
                int what;
                int x, y;
            };

        protected:
            enum cursor_style {
                cursor_inherit,
                cursor_info,
                cursor_cycle,
                cursor_up_down,
                cursor_crosshair
            };

            class OPENVRML_LOCAL modelview_matrix_stack {
                size_t size;
                std::stack<mat4f> spillover;

            public:
                modelview_matrix_stack();

                void push();
                void pop();
            };

            struct light_info {
                light_type type;
                int nesting_level;
                float radius_squared;
                vec3f location;
            };

            modelview_matrix_stack modelview_matrix_stack_;

            bool gl_initialized;
            bool blend;
            bool lit;
            bool texture;
            bool wireframe;

            size_t win_width;
            size_t win_height;
            color background;

            size_t objects, nested_objects;

            GLUtesselator * tesselator;

            size_t sensitive;
            size_t active_sensitive;
            size_t over_sensitive;

            enum { maxsensitive = 1000 };
            node * sensitive_object[maxsensitive];

            bool select_mode;
            double select_z;

            light_info light_info_[max_lights];

            int beginx, beginy;

            bool rotating, scaling, translating;
            bool draw_bounding_spheres;

            double render_time;
            double render_time1;

            // Window system specific methods

            virtual void post_redraw() = 0;
            virtual void set_cursor(cursor_style c) = 0;
            virtual void swap_buffers() = 0;
            virtual void set_timer(double interval) = 0;

            // Initialize OpenGL state
            void initialize();

            void begin_geometry();
            void end_geometry();

            void step(float, float, float);
            void zoom(float);
            void rotate(const openvrml::rotation & rot) OPENVRML_NOTHROW;

            void handleKey(int);
            void handleButton(event_info * e);
            void handleMouseDrag(int, int);

            // Check for pickable entity selection
            bool checkSensitive(int x, int y, event_type event);

        public:
            viewer();
            virtual ~viewer() OPENVRML_NOTHROW;

            virtual rendering_mode mode();
            virtual double frame_rate();

            virtual void reset_user_navigation();

            // Scope dirlights, open/close display lists
            virtual object_t begin_object(const char * id, bool retain);
            virtual void end_object();

            // Insert objects into the display list
            virtual object_t
            insert_background(const std::vector<float> & groundAngle,
                              const std::vector<color> & groundColor,
                              const std::vector<float> & skyAngle,
                              const std::vector<color> & skyColor,
                              const image & front,
                              const image & back,
                              const image & left,
                              const image & right,
                              const image & top,
                              const image & bottom);


            virtual object_t insert_box(const vec3f & size);
            virtual object_t insert_cone(float height, float radius,
                                         bool bottom, bool side);
            virtual object_t insert_cylinder(float height, float radius,
                                             bool bottom, bool side, bool top);

            virtual object_t
            insert_elevation_grid(unsigned int mask,
                                  const std::vector<float> & height,
                                  int32 xDimension,
                                  int32 zDimension,
                                  float xSpacing,
                                  float zSpacing,
                                  const std::vector<color> & color,
                                  const std::vector<vec3f> & normal,
                                  const std::vector<vec2f> & texCoord);
            virtual object_t
            insert_extrusion(
                unsigned int mask,
                const std::vector<vec3f> & spine,
                const std::vector<vec2f> & crossSection,
                const std::vector<openvrml::rotation> & orientation,
                const std::vector<vec2f> & scale);
            virtual object_t
            insert_line_set(const std::vector<vec3f> & coord,
                            const std::vector<int32> & coordIndex,
                            bool colorPerVertex,
                            const std::vector<color> & color,
                            const std::vector<int32> & colorIndex);
            virtual object_t
            insert_point_set(const std::vector<vec3f> & coord,
                             const std::vector<color> & color);
            virtual object_t
            insert_shell(unsigned int mask,
                         const std::vector<vec3f> & coord,
                         const std::vector<int32> & coordIndex,
                         const std::vector<color> & color,
                         const std::vector<int32> & colorIndex,
                         const std::vector<vec3f> & normal,
                         const std::vector<int32> & normalIndex,
                         const std::vector<vec2f> & texCoord,
                         const std::vector<int32> & texCoordIndex);
            virtual object_t insert_sphere(float radius);

            // Lights
            virtual object_t insert_dir_light(float ambientIntensity,
                                              float intensity,
                                              const color & color,
                                              const vec3f & direction);
            virtual object_t insert_point_light(float ambientIntensity,
                                                const vec3f & attenuation,
                                                const color & color,
                                                float intensity,
                                                const vec3f & location,
                                                float radius);
            virtual object_t insert_spot_light(float ambientIntensity,
                                               const vec3f & attenuation,
                                               float beamWidth,
                                               const color & color,
                                               float cutOffAngle,
                                               const vec3f & direction,
                                               float intensity,
                                               const vec3f & location,
                                               float radius);


            virtual object_t insert_reference(object_t existing_object);

            virtual void remove_object(object_t ref);

            virtual void enable_lighting(bool);

            // Set attributes
            virtual void set_color(const color & rgb, float a = 1.0);

            virtual void set_fog(const color & color,
                                 float visibilityRange,
                                 const char * type);

            virtual void set_material(float ambientIntensity,
                                      const color & diffuseColor,
                                      const color & emissiveColor,
                                      float shininess,
                                      const color & specularColor,
                                      float transparency);

            virtual void set_material_mode(size_t tex_components,
                                           bool geometry_color);

            virtual void set_sensitive(node * object);

            virtual texture_object_t insert_texture(const image & img,
                                                    bool repeat_s,
                                                    bool repeat_t,
                                                    bool retainHint = false);

            // Reference/remove a texture object
            virtual void insert_texture_reference(texture_object_t ref,
                                                  size_t components);
            virtual void remove_texture_object(texture_object_t);

            virtual void set_texture_transform(const vec2f & center,
                                               float rotation,
                                               const vec2f & scale,
                                               const vec2f & translation);

            virtual void set_viewpoint(const vec3f & position,
                                       const openvrml::rotation & orientation,
                                       float fieldOfView,
                                       float avatarSize,
                                       float visibilityLimit);

            virtual void transform(const mat4f & mat);

            virtual void transform_points(size_t nPoints, vec3f * point) const;

            virtual void
            draw_bounding_sphere(const bounding_sphere & bs,
                                 bounding_volume::intersection intersection);



            //
            // Viewer callbacks (not for public consumption)

            // Update the model.
            void update( double time = 0.0 );

            // Redraw the screen.
            virtual void redraw();
            void resize(size_t width, size_t height);

            // user interaction

            void input(event_info * e);
        };
    }
}

# endif // OPENVRML_GL_VIEWER_H
