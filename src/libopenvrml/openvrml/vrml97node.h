// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 1999  Kumaran Santhanam
// Copyright 2001, 2002, 2003, 2004  Braden McDaniel
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

# ifndef OPENVRML_VRML97NODE_H
#   define OPENVRML_VRML97NODE_H

#   include <openvrml/common.h>
#   include <openvrml/node.h>
#   include <openvrml/img.h>
#   include <openvrml/bounding_volume.h>

typedef unsigned int FT_UInt;
typedef struct FT_LibraryRec_ * FT_Library;
typedef struct FT_FaceRec_ * FT_Face;
typedef unsigned int FcChar32;

namespace openvrml {

    namespace vrml97_node {

        class abstract_base : public virtual node {
        public:
            virtual ~abstract_base() throw () = 0;

        protected:
            abstract_base(const node_type & type, const scope_ptr & scope);

        private:
            virtual void do_field(const std::string & id,
                                  const field_value & value)
                throw (unsupported_interface, std::bad_cast, std::bad_alloc);

            virtual const field_value & do_field(const std::string & id) const
                throw (unsupported_interface);

            virtual openvrml::event_listener &
            do_event_listener(const std::string & id)
                throw (unsupported_interface);

            virtual openvrml::event_emitter &
            do_event_emitter(const std::string & id)
                throw (unsupported_interface);
        };


        class abstract_child_node : public abstract_base,
                                    public child_node {
        public:
            virtual ~abstract_child_node() throw () = 0;

        protected:
            abstract_child_node(const node_type & type,
                                const scope_ptr & scope);
        };


        class abstract_geometry_node : public abstract_base,
                                       public geometry_node {
        public:
            virtual ~abstract_geometry_node() throw () = 0;

        protected:
            abstract_geometry_node(const node_type & type,
                                   const scope_ptr & scope);
        };


        class abstract_indexed_set_node : public abstract_geometry_node {
        protected:
            class set_color_index_listener : public mfint32_listener {
            public:
                explicit set_color_index_listener(
                    abstract_indexed_set_node & node);
                virtual ~set_color_index_listener() throw ();

                virtual void process_event(const mfint32 & color_index,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class set_coord_index_listener : public mfint32_listener {
            public:
                explicit set_coord_index_listener(
                    abstract_indexed_set_node & node);
                virtual ~set_coord_index_listener() throw ();

                virtual void process_event(const mfint32 & coord_index,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_color_index_listener set_color_index_;
            set_coord_index_listener set_coord_index_;
            exposedfield<sfnode> color_;
            exposedfield<sfnode> coord_;
            mfint32 color_index_;
            sfbool color_per_vertex_;
            mfint32 coord_index_;

        public:
            virtual ~abstract_indexed_set_node() throw () = 0;

            virtual bool modified() const;

            virtual const color_node * color() const throw ();

        protected:
            abstract_indexed_set_node(const node_type & type,
                                      const scope_ptr & scope);
        };


        class abstract_light_node : public abstract_child_node {
        protected:
            exposedfield<sffloat> ambient_intensity_;
            exposedfield<sfcolor> color_;
            exposedfield<sffloat> intensity_;
            exposedfield<sfbool> on_;

        public:
            virtual ~abstract_light_node() throw () = 0;

            virtual void renderScoped(openvrml::viewer & viewer);
            virtual abstract_light_node * to_light() const;

            float ambient_intensity() const throw ();
            float intensity() const throw ();
            bool on() const throw ();
            const openvrml::color & color() const throw ();

        protected:
            abstract_light_node(const node_type & type,
                                const scope_ptr & scope);
        };


        class abstract_texture_node : public abstract_base,
                                      public texture_node {
        protected:
            sfbool repeat_s_;
            sfbool repeat_t_;

        public:
            virtual ~abstract_texture_node() throw () = 0;

            //
            // texture_node implementation.
            //
            virtual bool repeat_s() const throw ();
            virtual bool repeat_t() const throw ();

        protected:
            abstract_texture_node(const node_type & type,
                                  const scope_ptr & scope);
        };


        class group_class : public node_class {
        public:
            explicit group_class(openvrml::browser & browser);
            virtual ~group_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class group_node : public abstract_base,
                           public virtual grouping_node {
            friend class group_class;

        protected:
            class add_children_listener : public mfnode_listener {
            public:
                explicit add_children_listener(group_node & node);
                virtual ~add_children_listener() throw ();

                virtual void process_event(const mfnode & value,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class remove_children_listener : public mfnode_listener {
            public:
                explicit remove_children_listener(group_node & node);
                virtual ~remove_children_listener() throw ();

                virtual void process_event(const mfnode & value,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class children_exposedfield : public exposedfield<mfnode> {
            public:
                explicit children_exposedfield(openvrml::node & node) throw ();
                virtual ~children_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfnode & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            sfvec3f bbox_center_;
            sfvec3f bbox_size_;
            add_children_listener add_children_listener_;
            remove_children_listener remove_children_listener_;
            children_exposedfield children_;

            viewer::object_t viewerObject;

            bounding_sphere bsphere;

        public:
            group_node(const node_type & type, const scope_ptr & scope);
            virtual ~group_node() throw ();

            virtual const openvrml::bounding_volume & bounding_volume() const;
            virtual bool modified() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timeStamp, bool isOver, bool isActive,
                                  double *p);

            void render_nocull(openvrml::viewer & viewer,
                               rendering_context context);

        protected:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            void recalc_bsphere();
        };


        class anchor_class : public node_class {
        public:
            explicit anchor_class(openvrml::browser & browser);
            virtual ~anchor_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class anchor_node : public group_node {
            friend class anchor_class;

            exposedfield<sfstring> description_;
            exposedfield<mfstring> parameter_;
            exposedfield<mfstring> url_;

        public:
            anchor_node(const node_type & type,
                        const scope_ptr & scope);
            virtual ~anchor_node() throw ();

            virtual anchor_node * to_anchor() const;

            virtual const openvrml::bounding_volume & bounding_volume() const;

            void activate();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class appearance_class : public node_class {
        public:
            explicit appearance_class(openvrml::browser & browser);
            virtual ~appearance_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class appearance_node : public abstract_base,
                                public openvrml::appearance_node {
            friend class appearance_class;

            exposedfield<sfnode> material_;
            exposedfield<sfnode> texture_;
            exposedfield<sfnode> texture_transform_;

        public:
            appearance_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~appearance_node() throw ();

            virtual bool modified() const;

            //
            // appearance_node implementation
            //
            virtual const node_ptr & material() const throw ();
            virtual const node_ptr & texture() const throw ();
            virtual const node_ptr & texture_transform() const throw ();

        private:
            virtual void do_render_appearance(viewer & v,
                                              rendering_context context);
        };


        class audio_clip_class : public node_class {
        public:
            explicit audio_clip_class(openvrml::browser & browser);
            virtual ~audio_clip_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class audio_clip_node : public abstract_base {
            friend class audio_clip_class;

            exposedfield<sfstring> description_;
            exposedfield<sfbool> loop_;
            exposedfield<sffloat> pitch_;
            exposedfield<sftime> start_time_;
            exposedfield<sftime> stop_time_;
            exposedfield<mfstring> url_;
            sftime duration_changed_;
            sftime_emitter duration_changed_emitter_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;

        public:
            audio_clip_node(const node_type & type, const scope_ptr & scope);
            virtual ~audio_clip_node() throw ();

            void update(double time);

            virtual audio_clip_node * to_audio_clip() const;

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
        };


        class background_node;

        class background_class : public node_class {
            typedef std::vector<background_node *> bound_nodes_t;

            background_node * first;
            bound_nodes_t bound_nodes;

        public:
            explicit background_class(openvrml::browser & browser);
            virtual ~background_class() throw ();

            void set_first(background_node & background) throw ();
            bool has_first() const throw ();
            void bind(background_node & background, double timestamp)
                throw (std::bad_alloc);
            void unbind(background_node & background, double timestamp)
                throw ();

            virtual void initialize(viewpoint_node * initialViewpoint,
                                    double timestamp) throw ();
            virtual void render(viewer & v) throw ();
            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class background_node : public abstract_child_node {
            friend class background_class;

            class set_bind_listener : public sfbool_listener {
            public:
                explicit set_bind_listener(background_node & node);
                virtual ~set_bind_listener() throw ();

                virtual void process_event(const sfbool & value,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class back_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit back_url_exposedfield(background_node & node)
                    throw ();
                virtual ~back_url_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfstring & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class bottom_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit bottom_url_exposedfield(background_node & node)
                    throw ();
                virtual ~bottom_url_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfstring & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class front_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit front_url_exposedfield(background_node & node)
                    throw ();
                virtual ~front_url_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfstring & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class left_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit left_url_exposedfield(background_node & node)
                    throw ();
                virtual ~left_url_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfstring & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class right_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit right_url_exposedfield(background_node & node)
                    throw ();
                virtual ~right_url_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfstring & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class top_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit top_url_exposedfield(background_node & node) throw ();
                virtual ~top_url_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfstring & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_bind_listener set_bind_listener_;
            exposedfield<mffloat> ground_angle_;
            exposedfield<mfcolor> ground_color_;
            back_url_exposedfield back_url_;
            bottom_url_exposedfield bottom_url_;
            front_url_exposedfield front_url_;
            left_url_exposedfield left_url_;
            right_url_exposedfield right_url_;
            top_url_exposedfield top_url_;
            exposedfield<mffloat> sky_angle_;
            exposedfield<mfcolor> sky_color_;
            sfbool is_bound_;
            sfbool_emitter is_bound_emitter_;

            image front;
            bool front_needs_update;
            image back;
            bool back_needs_update;
            image left;
            bool left_needs_update;
            image right;
            bool right_needs_update;
            image top;
            bool top_needs_update;
            image bottom;
            bool bottom_needs_update;

            // Display list object for background
            viewer::object_t viewerObject;

        public:
            background_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~background_node() throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();

            void update_textures();
        };


        class billboard_class : public node_class {
        public:
            explicit billboard_class(openvrml::browser & browser);
            virtual ~billboard_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class billboard_node : public group_node {
            friend class billboard_class;

            exposedfield<sfvec3f> axis_of_rotation_;

            viewer::object_t xformObject;

        public:
            static const mat4f billboard_to_matrix(const billboard_node & node,
                                                   const mat4f & modelview);

            billboard_node(const node_type & type, const scope_ptr & scope);
            virtual ~billboard_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class box_class : public node_class {
        public:
            explicit box_class(openvrml::browser & browser);
            virtual ~box_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class box_node : public abstract_geometry_node {
            friend class box_class;

            sfvec3f size;

            bounding_sphere bsphere;

        public:
            box_node(const node_type & type, const scope_ptr & scope);
            virtual ~box_node() throw ();

            virtual const openvrml::bounding_volume & bounding_volume() const;

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class collision_class : public node_class {
        public:
            explicit collision_class(openvrml::browser & browser);
            virtual ~collision_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class collision_node : public group_node {
            friend class collision_class;

            exposedfield<sfbool> collide_;
            sfnode proxy_;
            sftime collide_time_;
            sftime_emitter collide_time_emitter_;

        public:
            collision_node(const node_type & type, const scope_ptr & scope);
            virtual ~collision_node() throw ();

            virtual bool modified() const;
        };


        class color_class : public node_class {
        public:
            explicit color_class(openvrml::browser & browser);
            virtual ~color_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class color_node : public abstract_base,
                           public openvrml::color_node {
            friend class color_class;

            exposedfield<mfcolor> color_;

        public:
            color_node(const node_type & type,
                       const scope_ptr & scope);
            virtual ~color_node() throw ();

            //
            // color_node implementation
            //
            virtual const std::vector<openvrml::color> & color() const
                throw ();
        };


        class color_interpolator_class : public node_class {
        public:
            explicit color_interpolator_class(openvrml::browser & browser);
            virtual ~color_interpolator_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class color_interpolator_node : public abstract_child_node {
            friend class color_interpolator_class;

            class set_fraction_listener : public sffloat_listener {
            public:
                explicit set_fraction_listener(color_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

                virtual void process_event(const sffloat & value,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfcolor> key_value_;
            sfcolor value_;
            sfcolor_emitter value_changed_;

        public:
            color_interpolator_node(const node_type & type,
                                    const scope_ptr & scope);
            virtual ~color_interpolator_node() throw ();
        };


        class cone_class : public node_class {
        public:
            explicit cone_class(openvrml::browser & browser);
            virtual ~cone_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class cone_node : public abstract_geometry_node {
            friend class cone_class;

            sfbool bottom;
            sffloat bottomRadius;
            sffloat height;
            sfbool side;

        public:
            cone_node(const node_type & type,
                      const scope_ptr & scope);
            virtual ~cone_node() throw ();

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class coordinate_class : public node_class {
        public:
            explicit coordinate_class(openvrml::browser & browser);
            virtual ~coordinate_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class coordinate_node : public abstract_base,
                                public openvrml::coordinate_node {
            friend class coordinate_class;

            exposedfield<mfvec3f> point_;

        public:
            coordinate_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~coordinate_node() throw ();

            //
            // coordinate_node implementation
            //
            virtual const std::vector<vec3f> & point() const throw ();
        };


        class coordinate_interpolator_class :
            public node_class {
        public:
            explicit coordinate_interpolator_class(
                openvrml::browser & browser);
            virtual ~coordinate_interpolator_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class coordinate_interpolator_node : public abstract_child_node {
            friend class coordinate_interpolator_class;

            class set_fraction_listener : public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    coordinate_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

                virtual void process_event(const sffloat & value,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfvec3f> key_value_;
            mfvec3f value_;
            mfvec3f_emitter value_changed_;

        public:
            coordinate_interpolator_node(const node_type & type,
                                         const scope_ptr & scope);
            virtual ~coordinate_interpolator_node() throw ();
        };


        class cylinder_class : public node_class {
        public:
            explicit cylinder_class(openvrml::browser & browser);
            virtual ~cylinder_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class cylinder_node : public abstract_geometry_node {
            friend class cylinder_class;

            sfbool bottom;
            sffloat height;
            sffloat radius;
            sfbool side;
            sfbool top;

        public:
            cylinder_node(const node_type & type,
                          const scope_ptr & scope);
            virtual ~cylinder_node() throw ();

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class cylinder_sensor_class : public node_class {
        public:
            explicit cylinder_sensor_class(openvrml::browser & browser);
            virtual ~cylinder_sensor_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class cylinder_sensor_node : public abstract_child_node {

            friend class cylinder_sensor_class;

            exposedfield<sfbool> auto_offset_;
            exposedfield<sffloat> disk_angle_;
            exposedfield<sfbool> enabled_;
            exposedfield<sffloat> max_angle_;
            exposedfield<sffloat> min_angle_;
            exposedfield<sffloat> offset_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfrotation rotation_changed_;
            sfrotation_emitter rotation_changed_emitter_;
            sfvec3f track_point_changed_;
            sfvec3f_emitter track_point_changed_emitter_;

            float rotation_val;
            vec3f activationPoint;
            bool disk;
            mat4f activationMatrix;
            mat4f modelview;

        public:
            cylinder_sensor_node(const node_type & type,
                                 const scope_ptr & scope);
            virtual ~cylinder_sensor_node() throw ();

            virtual cylinder_sensor_node * to_cylinder_sensor() const;

            void activate(double timeStamp, bool isActive, double *p);

            bool enabled() const;

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class directional_light_class : public node_class {
        public:
            explicit directional_light_class(openvrml::browser & browser);
            virtual ~directional_light_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class directional_light_node : public abstract_light_node {
            friend class directional_light_class;

            exposedfield<sfvec3f> direction_;

        public:
            directional_light_node(const node_type & type,
                                   const scope_ptr & scope);
            virtual ~directional_light_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class elevation_grid_class : public node_class {
        public:
            explicit elevation_grid_class(openvrml::browser & browser);
            virtual ~elevation_grid_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class elevation_grid_node : public abstract_geometry_node {
            friend class elevation_grid_class;

            class set_height_listener : public mffloat_listener {
            public:
                explicit set_height_listener(elevation_grid_node & node);
                virtual ~set_height_listener() throw ();

                virtual void process_event(const mffloat & height,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_height_listener set_height_listener_;
            exposedfield<sfnode> color_;
            exposedfield<sfnode> normal_;
            exposedfield<sfnode> tex_coord_;
            mffloat height_;
            sfbool ccw_;
            sfbool color_per_vertex_;
            sffloat crease_angle_;
            sfbool normal_per_vertex_;
            sfbool solid_;
            sfint32 x_dimension_;
            sffloat x_spacing_;
            sfint32 z_dimension_;
            sffloat z_spacing_;

        public:
            elevation_grid_node(const node_type & type,
                                const scope_ptr & scope);
            virtual ~elevation_grid_node() throw ();

            virtual bool modified() const;

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class extrusion_class : public node_class {
        public:
            explicit extrusion_class(openvrml::browser & browser);
            virtual ~extrusion_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class extrusion_node : public abstract_geometry_node {
            friend class extrusion_class;

            class set_cross_section_listener : public mfvec2f_listener {
            public:
                explicit set_cross_section_listener(extrusion_node & node);
                virtual ~set_cross_section_listener() throw ();

                virtual void process_event(const mfvec2f & cross_section,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class set_orientation_listener : public mfrotation_listener {
            public:
                explicit set_orientation_listener(extrusion_node & node);
                virtual ~set_orientation_listener() throw ();

                virtual void process_event(const mfrotation & orientation,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class set_scale_listener : public mfvec2f_listener {
            public:
                explicit set_scale_listener(extrusion_node & node);
                virtual ~set_scale_listener() throw ();

                virtual void process_event(const mfvec2f & scale,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class set_spine_listener : public mfvec3f_listener {
            public:
                explicit set_spine_listener(extrusion_node & node);
                virtual ~set_spine_listener() throw ();

                virtual void process_event(const mfvec3f & spine,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_cross_section_listener set_cross_section_listener_;
            set_orientation_listener set_orientation_listener_;
            set_scale_listener set_scale_listener_;
            set_spine_listener set_spine_listener_;
            sfbool begin_cap_;
            sfbool ccw_;
            sfbool convex_;
            sffloat crease_angle_;
            mfvec2f cross_section_;
            sfbool end_cap_;
            mfrotation orientation_;
            mfvec2f scale_;
            sfbool solid_;
            mfvec3f spine_;

        public:
            extrusion_node(const node_type & type, const scope_ptr & scope);
            virtual ~extrusion_node() throw ();

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class fog_node;

        class fog_class : public node_class {
            typedef std::vector<fog_node *> bound_nodes_t;

            fog_node * first;
            bound_nodes_t bound_nodes;

        public:
            explicit fog_class(openvrml::browser & browser);
            virtual ~fog_class() throw ();

            void set_first(fog_node & fog) throw ();
            bool has_first() const throw ();
            void bind(fog_node & fog, double timestamp) throw (std::bad_alloc);
            void unbind(fog_node & fog, double timestamp) throw ();

            virtual void initialize(viewpoint_node * initialViewpoint,
                                    double timestamp) throw ();
            virtual void render(openvrml::viewer & viewer) throw ();
            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class fog_node : public abstract_child_node {
            friend class fog_class;

            class set_bind_listener : public sfbool_listener {
            public:
                explicit set_bind_listener(fog_node & node);
                virtual ~set_bind_listener() throw ();

                virtual void process_event(const sfbool & bind,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_bind_listener set_bind_listener_;
            exposedfield<sfcolor> color_;
            exposedfield<sfstring> fog_type_;
            exposedfield<sffloat> visibility_range_;
            sfbool is_bound_;
            sfbool_emitter is_bound_emitter_;

        public:
            fog_node(const node_type & type, const scope_ptr & scope);
            virtual ~fog_node() throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();
        };


        class font_style_class : public node_class {
        public:
            explicit font_style_class(openvrml::browser & browser);
            virtual ~font_style_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class font_style_node : public abstract_base,
                                public openvrml::font_style_node {

            friend class font_style_class;

            mfstring family_;
            sfbool horizontal_;
            mfstring justify_;
            sfstring language_;
            sfbool leftToRight;
            sffloat size_;
            sffloat spacing_;
            sfstring style_;
            sfbool topToBottom;

        public:
            font_style_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~font_style_node() throw ();

            //
            // font_style_node implementation
            //
            virtual const std::vector<std::string> & family() const throw ();
            virtual bool horizontal() const throw ();
            virtual const std::vector<std::string> & justify() const throw ();
            virtual const std::string & language() const throw ();
            virtual bool left_to_right() const throw ();
            virtual float size() const throw ();
            virtual float spacing() const throw ();
            virtual const std::string & style() const throw ();
            virtual bool top_to_bottom() const throw ();
        };


        class image_texture_class : public node_class {
        public:
            explicit image_texture_class(openvrml::browser & browser);
            virtual ~image_texture_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class image_texture_node : public abstract_texture_node {
            friend class image_texture_class;

            class url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit url_exposedfield(image_texture_node & node);
                virtual ~url_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfstring & url,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            url_exposedfield url_;

            openvrml::image image_;
            bool texture_needs_update;

        public:
            image_texture_node(const node_type & type,
                               const scope_ptr & scope);
            virtual ~image_texture_node() throw ();

            virtual const openvrml::image & image() const throw ();
            virtual size_t frames() const throw ();

        private:
            virtual viewer::texture_object_t
            do_render_texture(viewer & v, rendering_context context);

            void update_texture();
        };


        class indexed_face_set_class : public node_class {
        public:
            explicit indexed_face_set_class(openvrml::browser & browser);
            virtual ~indexed_face_set_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class indexed_face_set_node : public abstract_indexed_set_node {
            friend class indexed_face_set_class;

            class set_normal_index_listener : public mfint32_listener {
            public:
                explicit set_normal_index_listener(
                    indexed_face_set_node & node);
                virtual ~set_normal_index_listener() throw ();

                virtual void process_event(const mfint32 & normal_index,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class set_tex_coord_index_listener : public mfint32_listener {
            public:
                explicit set_tex_coord_index_listener(
                    indexed_face_set_node & node);
                virtual ~set_tex_coord_index_listener() throw ();

                virtual void process_event(const mfint32 & tex_coord_index,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_normal_index_listener set_normal_index_listener_;
            set_tex_coord_index_listener set_tex_coord_index_listener_;
            exposedfield<sfnode> normal_;
            exposedfield<sfnode> tex_coord_;
            sfbool ccw_;
            sfbool convex_;
            sffloat crease_angle_;
            mfint32 normal_index_;
            sfbool normal_per_vertex_;
            sfbool solid_;
            mfint32 tex_coord_index_;

            bounding_sphere bsphere;

        public:
            indexed_face_set_node(const node_type & type,
                                  const scope_ptr & scope);
            virtual ~indexed_face_set_node() throw ();

            virtual bool modified() const;
            virtual const openvrml::bounding_volume & bounding_volume() const;

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);

            void recalcBSphere();
        };


        class indexed_line_set_class : public node_class {
        public:
            explicit indexed_line_set_class(openvrml::browser & browser);
            virtual ~indexed_line_set_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class indexed_line_set_node : public abstract_indexed_set_node {

            friend class indexed_line_set_class;

        public:
            indexed_line_set_node(const node_type & type,
                                  const scope_ptr & scope);
            virtual ~indexed_line_set_node() throw ();

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class inline_class : public node_class {
        public:
            explicit inline_class(openvrml::browser & browser);
            virtual ~inline_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class inline_node : public abstract_base, public grouping_node {
            friend class inline_class;

            exposedfield<mfstring> url_;
            sfvec3f bbox_center_;
            sfvec3f bbox_size_;

            openvrml::scene * inlineScene;
            bool hasLoaded;

        public:
            inline_node(const node_type & type, const scope_ptr & scope);
            virtual ~inline_node() throw ();

            virtual inline_node * to_inline() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            void load();
        };


        class lod_class : public node_class {
        public:
            explicit lod_class(openvrml::browser & browser);
            virtual ~lod_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class lod_node : public abstract_base, public grouping_node {
            friend class lod_class;

            exposedfield<mfnode> level_;
            sfvec3f center_;
            mffloat range_;

            mfnode children_;
            bounding_sphere bsphere;

        public:
            lod_node(const node_type & type, const scope_ptr & scope);
            virtual ~lod_node() throw ();

            virtual bool modified() const;
            virtual const openvrml::bounding_volume & bounding_volume() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            virtual void recalcBSphere();
        };


        class material_class : public node_class {
        public:
            explicit material_class(openvrml::browser & browser);
            virtual ~material_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class material_node : public abstract_base,
                              public openvrml::material_node {
            friend class material_class;

            exposedfield<sffloat> ambient_intensity_;
            exposedfield<sfcolor> diffuse_color_;
            exposedfield<sfcolor> emissive_color_;
            exposedfield<sffloat> shininess_;
            exposedfield<sfcolor> specular_color_;
            exposedfield<sffloat> transparency_;

        public:
            material_node(const node_type & type, const scope_ptr & scope);
            virtual ~material_node() throw ();

            //
            // material_node implementation
            //
            virtual float ambient_intensity() const throw ();
            virtual const color & diffuse_color() const throw ();
            virtual const color & emissive_color() const throw ();
            virtual float shininess() const throw ();
            virtual const color & specular_color() const throw ();
            virtual float transparency() const throw ();
        };


        class movie_texture_class : public node_class {
        public:
            explicit movie_texture_class(openvrml::browser & browser);
            virtual ~movie_texture_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class movie_texture_node : public abstract_texture_node {
            friend class movie_texture_class;

            class set_speed_listener : public sffloat_listener {
            public:
                explicit set_speed_listener(movie_texture_node & node);
                virtual ~set_speed_listener() throw ();

                virtual void process_event(const sffloat & speed,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            exposedfield<sfbool> loop_;
            set_speed_listener set_speed_;
            sffloat speed_;
            sffloat_emitter speed_changed_;
            exposedfield<sftime> start_time_;
            exposedfield<sftime> stop_time_;
            exposedfield<mfstring> url_;
            sftime duration_;
            sftime_emitter duration_changed_;
            sfbool active_;
            sfbool_emitter is_active_;

            img * img_;
            openvrml::image image_;
            int frame, lastFrame;
            double lastFrameTime;

        public:
            movie_texture_node(const node_type & type,
                               const scope_ptr & scope);
            virtual ~movie_texture_node() throw ();

            virtual movie_texture_node * to_movie_texture() const;

            void update(double time);

            virtual const openvrml::image & image() const throw ();
            virtual size_t frames() const throw ();

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
            virtual viewer::texture_object_t
            do_render_texture(viewer & v, rendering_context context);
        };


        class navigation_info_class : public node_class {
        public:
            explicit navigation_info_class(openvrml::browser & browser);
            virtual ~navigation_info_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class navigation_info_node : public abstract_child_node {
            friend class navigation_info_class;

            class set_bind_listener : public sfbool_listener {
            public:
                explicit set_bind_listener(navigation_info_node & node);
                virtual ~set_bind_listener() throw ();

                virtual void process_event(const sfbool & bind,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_bind_listener set_bind_listener_;
            exposedfield<mffloat> avatar_size_;
            exposedfield<sfbool> headlight_;
            exposedfield<sffloat> speed_;
            exposedfield<mfstring> type_;
            exposedfield<sffloat> visibility_limit_;
            sfbool is_bound_;
            sfbool_emitter is_bound_emitter_;

        public:
            navigation_info_node(const node_type & type,
                                 const scope_ptr & scope);
            virtual ~navigation_info_node() throw ();

            virtual navigation_info_node * to_navigation_info() const;

            const float * avatar_size() const;
            bool headlight() const;
            float speed() const;
            float visibility_limit() const;

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
        };


        class normal_class : public node_class {
        public:
            explicit normal_class(openvrml::browser & browser);
            virtual ~normal_class() throw ();

            virtual const node_type_ptr
            create_type(const std::string & id,
                        const node_interface_set & interfaces)
                throw (unsupported_interface, std::bad_alloc);
        };

        class normal_node : public abstract_base,
                            public openvrml::normal_node {
            friend class normal_class;

            exposedfield<mfvec3f> vector_;

        public:
            normal_node(const node_type & type, const scope_ptr & scope);
            virtual ~normal_node() throw ();

            //
            // normal_node implementation.
            //
            virtual const std::vector<vec3f> & vector() const throw ();
        };


        class normal_interpolator_class : public node_class {
        public:
            explicit normal_interpolator_class(openvrml::browser & browser);
            virtual ~normal_interpolator_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class normal_interpolator_node : public abstract_child_node {
            friend class normal_interpolator_class;

            class set_fraction_listener : public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    normal_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

                virtual void process_event(const sffloat & fraction,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfvec3f> key_value_;
            mfvec3f value_changed_;
            mfvec3f_emitter value_changed_emitter_;

        public:
            normal_interpolator_node(const node_type & type,
                                     const scope_ptr & scope);
            virtual ~normal_interpolator_node() throw ();
        };


        class orientation_interpolator_class : public node_class {
        public:
            explicit orientation_interpolator_class(
                openvrml::browser & browser);
            virtual ~orientation_interpolator_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class orientation_interpolator_node : public abstract_child_node {
            friend class orientation_interpolator_class;

            class set_fraction_listener : public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    orientation_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

                virtual void process_event(const sffloat & fraction,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfrotation> key_value_;
            sfrotation value_changed_;
            sfrotation_emitter value_changed_emitter_;

        public:
            orientation_interpolator_node(const node_type & type,
                                          const scope_ptr & scope);
            virtual ~orientation_interpolator_node() throw ();
        };


        class pixel_texture_class : public node_class {
        public:
            explicit pixel_texture_class(openvrml::browser & browser);
            virtual ~pixel_texture_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class pixel_texture_node : public abstract_texture_node {
            friend class pixel_texture_class;

            exposedfield<sfimage> image_;

        public:
            pixel_texture_node(const node_type & type,
                               const scope_ptr & scope);
            virtual ~pixel_texture_node() throw ();

            virtual const openvrml::image & image() const throw ();
            virtual size_t frames() const throw ();

        private:
            virtual viewer::texture_object_t
            do_render_texture(viewer & v, rendering_context context);
        };


        class plane_sensor_class : public node_class {
        public:
            explicit plane_sensor_class(openvrml::browser & browser);
            virtual ~plane_sensor_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class plane_sensor_node : public abstract_child_node {
            friend class plane_sensor_class;

            exposedfield<sfbool> auto_offset_;
            exposedfield<sfbool> enabled_;
            exposedfield<sfvec2f> max_position_;
            exposedfield<sfvec2f> min_position_;
            exposedfield<sfvec3f> offset_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfvec3f track_point_changed_;
            sfvec3f_emitter track_point_changed_emitter_;
            sfvec3f translation_changed_;
            sfvec3f_emitter translation_changed_emitter_;

            sfvec3f activationPoint;

            mat4f activationMatrix;
            mat4f modelview;

        public:
            plane_sensor_node(const node_type & type,
                              const scope_ptr & scope);
            virtual ~plane_sensor_node() throw ();

            virtual plane_sensor_node * to_plane_sensor() const;

            void activate( double timeStamp, bool isActive, double *p );

            bool enabled() const;

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class point_light_class : public node_class {
        public:
            explicit point_light_class(openvrml::browser & browser);
            virtual ~point_light_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class point_light_node : public abstract_light_node {
            friend class point_light_class;

            exposedfield<sfvec3f> attenuation_;
            exposedfield<sfvec3f> location_;
            exposedfield<sffloat> radius_;

        public:
            point_light_node(const node_type & type, const scope_ptr & scope);
            virtual ~point_light_node() throw ();

            virtual point_light_node * to_point_light() const;

            virtual void renderScoped(openvrml::viewer & viewer);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
        };


        class point_set_class : public node_class {
        public:
            explicit point_set_class(openvrml::browser & browser);
            virtual ~point_set_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class point_set_node : public abstract_geometry_node {
            friend class point_set_class;

            exposedfield<sfnode> color_;
            exposedfield<sfnode> coord_;

            bounding_sphere bsphere;

        public:
            point_set_node(const node_type & type, const scope_ptr & scope);
            virtual ~point_set_node() throw ();

            virtual bool modified() const;
            virtual const openvrml::bounding_volume & bounding_volume() const;

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);

            void recalcBSphere();
        };


        class position_interpolator_class : public node_class {
        public:
            explicit position_interpolator_class(openvrml::browser & browser);
            virtual ~position_interpolator_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class position_interpolator_node : public abstract_child_node {
            friend class position_interpolator_class;

            class set_fraction_listener : public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    position_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

                virtual void process_event(const sffloat & fraction,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfvec3f> key_value_;
            sfvec3f value_changed_;
            sfvec3f_emitter value_changed_emitter_;

        public:
            position_interpolator_node(const node_type & type,
                                       const scope_ptr & scope);
            virtual ~position_interpolator_node() throw ();
        };


        class proximity_sensor_class : public node_class {
        public:
            explicit proximity_sensor_class(openvrml::browser & browser);
            virtual ~proximity_sensor_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class proximity_sensor_node : public abstract_child_node {

            friend class proximity_sensor_class;

            exposedfield<sfvec3f> center_;
            exposedfield<sfbool> enabled_;
            exposedfield<sfvec3f> size_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfvec3f position_changed_;
            sfvec3f_emitter position_changed_emitter_;
            sfrotation orientation_changed_;
            sfrotation_emitter orientation_changed_emitter_;
            sftime enter_time_;
            sftime_emitter enter_time_emitter_;
            sftime exit_time_;
            sftime_emitter exit_time_emitter_;

        public:
            proximity_sensor_node(const node_type & type,
                                  const scope_ptr & scope);
            virtual ~proximity_sensor_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class scalar_interpolator_class : public node_class {
        public:
            explicit scalar_interpolator_class(openvrml::browser & browser);
            virtual ~scalar_interpolator_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class scalar_interpolator_node : public abstract_child_node {
            friend class scalar_interpolator_class;

            class set_fraction_listener : public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    scalar_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

                virtual void process_event(const sffloat & fraction,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mffloat> key_value_;
            sffloat value_changed_;
            sffloat_emitter value_changed_emitter_;

        public:
            scalar_interpolator_node(const node_type & type,
                                     const scope_ptr & scope);
            virtual ~scalar_interpolator_node() throw ();
        };


        class shape_class : public node_class {
        public:
            explicit shape_class(openvrml::browser & browser);
            virtual ~shape_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class shape_node : public abstract_child_node {
            friend class shape_class;

            exposedfield<sfnode> appearance_;
            exposedfield<sfnode> geometry_;

            viewer::object_t viewerObject;

        public:
            shape_node(const node_type & type, const scope_ptr & scope);
            virtual ~shape_node() throw ();

            virtual bool modified() const;
            virtual const openvrml::bounding_volume & bounding_volume() const;

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class sound_class : public node_class {
        public:
            explicit sound_class(openvrml::browser & browser);
            virtual ~sound_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class sound_node : public abstract_child_node {
            friend class sound_class;

            exposedfield<sfvec3f> direction_;
            exposedfield<sffloat> intensity_;
            exposedfield<sfvec3f> location_;
            exposedfield<sffloat> max_back_;
            exposedfield<sffloat> max_front_;
            exposedfield<sffloat> min_back_;
            exposedfield<sffloat> min_front_;
            exposedfield<sffloat> priority_;
            exposedfield<sfnode> source_;
            sfbool spatialize_;

        public:
            sound_node(const node_type & type, const scope_ptr & scope);
            virtual ~sound_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class sphere_class : public node_class {
        public:
            explicit sphere_class(openvrml::browser & browser);
            virtual ~sphere_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class sphere_node : public abstract_geometry_node {
            friend class sphere_class;

            sffloat radius;
            bounding_sphere bsphere;

        public:
            sphere_node(const node_type & type,
                        const scope_ptr & scope);
            virtual ~sphere_node() throw ();

            virtual const openvrml::bounding_volume & bounding_volume() const;

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class sphere_sensor_class : public node_class {
        public:
            explicit sphere_sensor_class(openvrml::browser & browser);
            virtual ~sphere_sensor_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class sphere_sensor_node : public abstract_child_node {
            friend class sphere_sensor_class;

            exposedfield<sfbool> auto_offset_;
            exposedfield<sfbool> enabled_;
            exposedfield<sfrotation> offset_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfrotation rotation_changed_;
            sfrotation_emitter rotation_changed_emitter_;
            sfvec3f track_point_changed_;
            sfvec3f_emitter track_point_changed_emitter_;

            sfvec3f activationPoint;
            sfvec3f centerPoint;
            mat4f modelview;

        public:
            sphere_sensor_node(const node_type & type,
                               const scope_ptr & scope);
            virtual ~sphere_sensor_node() throw ();

            virtual sphere_sensor_node * to_sphere_sensor() const;

            void activate(double timeStamp, bool isActive, double * p);
            bool isEnabled() const throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class spot_light_class : public node_class {
        public:
            explicit spot_light_class(openvrml::browser & browser);
            virtual ~spot_light_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class spot_light_node : public abstract_light_node {
            friend class spot_light_class;

            exposedfield<sfvec3f> attenuation_;
            exposedfield<sffloat> beam_width_;
            exposedfield<sffloat> cut_off_angle_;
            exposedfield<sfvec3f> direction_;
            exposedfield<sfvec3f> location_;
            exposedfield<sffloat> radius_;

        public:
            spot_light_node(const node_type & type, const scope_ptr & scope);
            virtual ~spot_light_node() throw ();

            virtual spot_light_node * to_spot_light() const;

            virtual void renderScoped(openvrml::viewer & viewer);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
        };


        class switch_class : public node_class {
        public:
            explicit switch_class(openvrml::browser & browser);
            virtual ~switch_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class switch_node : public abstract_base, public grouping_node {
            friend class switch_class;

            class choice_exposedfield : public exposedfield<mfnode> {
            public:
                explicit choice_exposedfield(switch_node & node);
                virtual ~choice_exposedfield() throw ();

            private:
                virtual void do_process_event(const mfnode & choice,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class which_choice_exposedfield : public exposedfield<sfint32> {
            public:
                explicit which_choice_exposedfield(switch_node & node);
                virtual ~which_choice_exposedfield() throw ();

            private:
                virtual void do_process_event(const sfint32 & which_choice,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            choice_exposedfield choice_;
            which_choice_exposedfield which_choice_;

            mfnode children_;
            bounding_sphere bsphere;

        public:
            switch_node(const node_type & type, const scope_ptr & scope);
            virtual ~switch_node() throw ();

            virtual bool modified() const;
            virtual const openvrml::bounding_volume & bounding_volume() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            virtual void recalcBSphere();
        };


        class text_class : public node_class {
        public:
            FT_Library freeTypeLibrary;

            explicit text_class(openvrml::browser & browser);
            virtual ~text_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class text_node : public abstract_geometry_node {
            friend class text_class;

            class string_exposedfield : public exposedfield<mfstring> {
            public:
                explicit string_exposedfield(text_node & node);
                virtual ~string_exposedfield() throw ();

                virtual void do_process_event(const mfstring & string,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class font_style_exposedfield : public exposedfield<sfnode> {
            public:
                explicit font_style_exposedfield(text_node & node);
                virtual ~font_style_exposedfield() throw ();

                virtual void do_process_event(const sfnode & font_style,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class length_exposedfield : public exposedfield<mffloat> {
            public:
                explicit length_exposedfield(text_node & node);
                virtual ~length_exposedfield() throw ();

                virtual void do_process_event(const mffloat & length,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class max_extent_exposedfield : public exposedfield<sffloat> {
            public:
                explicit max_extent_exposedfield(text_node & node);
                virtual ~max_extent_exposedfield() throw ();

                virtual void do_process_event(const sffloat & max_extent,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            string_exposedfield string_;
            font_style_exposedfield font_style_;
            length_exposedfield length_;
            max_extent_exposedfield max_extent_;

            struct glyph_geometry {
                std::vector<vec2f> coord;
                std::vector<int32> coord_index;
                float advance_width;
                float advance_height;

                glyph_geometry(
                    const std::vector<std::vector<vec2f> > & contours,
                    float advance_width, float advance_height)
                    throw (std::bad_alloc);
            };

            struct text_geometry {
                std::vector<vec3f> coord;
                std::vector<int32> coord_index;
                std::vector<vec3f> normal;
                std::vector<vec2f> tex_coord;
            };

            typedef std::vector<std::vector<FcChar32> > ucs4_string_t;
            typedef std::map<FT_UInt, glyph_geometry> glyph_geometry_map_t;

            ucs4_string_t ucs4_string;
            FT_Face face;
            glyph_geometry_map_t glyph_geometry_map;
            text_geometry text_geometry_;

        public:
            text_node(const node_type & type, const scope_ptr & scope);
            virtual ~text_node() throw ();

            virtual bool modified() const;

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);

            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            void update_ucs4() throw (std::bad_alloc);
            void update_face() throw (std::bad_alloc);
            void update_geometry() throw (std::bad_alloc);
        };


        class texture_coordinate_class : public node_class {
        public:
            explicit texture_coordinate_class(openvrml::browser & browser);
            virtual ~texture_coordinate_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class texture_coordinate_node :
            public abstract_base,
            public openvrml::texture_coordinate_node {

            friend class texture_coordinate_class;

            exposedfield<mfvec2f> point_;

        public:
            texture_coordinate_node(const node_type & type,
                                    const scope_ptr & scope);
            virtual ~texture_coordinate_node() throw ();

            //
            // texture_coordinate_node implementation.
            //
            virtual const std::vector<vec2f> & point() const throw();
        };


        class texture_transform_class : public node_class {
        public:
            explicit texture_transform_class(openvrml::browser & browser);
            virtual ~texture_transform_class() throw ();

            virtual const node_type_ptr
            create_type(const std::string & id,
                        const node_interface_set & interfaces)
                throw (unsupported_interface, std::bad_alloc);
        };

        class texture_transform_node :
            public abstract_base,
            public openvrml::texture_transform_node {

            friend class texture_transform_class;

            exposedfield<sfvec2f> center_;
            exposedfield<sffloat> rotation_;
            exposedfield<sfvec2f> scale_;
            exposedfield<sfvec2f> translation_;

        public:
            texture_transform_node(const node_type & type,
                                   const scope_ptr & scope);
            virtual ~texture_transform_node() throw ();

        private:
            virtual void
            do_render_texture_transform(viewer & v, rendering_context context);
        };


        class time_sensor_class : public node_class {
        public:
            explicit time_sensor_class(openvrml::browser & browser);
            virtual ~time_sensor_class() throw ();

            virtual const node_type_ptr
            create_type(const std::string & id,
                        const node_interface_set & interfaces)
                throw (unsupported_interface, std::bad_alloc);
        };

        class time_sensor_node : public abstract_child_node {
            friend class time_sensor_class;

            class set_cycle_interval_listener : public sftime_listener {
            public:
                explicit set_cycle_interval_listener(time_sensor_node & node);
                virtual ~set_cycle_interval_listener() throw ();

                virtual void process_event(const sftime & cycle_interval,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class enabled_exposedfield : public exposedfield<sfbool> {
            public:
                explicit enabled_exposedfield(time_sensor_node & node);
                virtual ~enabled_exposedfield() throw ();

            private:
                virtual void do_process_event(const sfbool & enabled,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class set_start_time_listener : public sftime_listener {
            public:
                explicit set_start_time_listener(time_sensor_node & node);
                virtual ~set_start_time_listener() throw ();

                virtual void process_event(const sftime & start_time,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            set_cycle_interval_listener set_cycle_interval_listener_;
            sftime cycle_interval_;
            sftime_emitter cycle_interval_changed_emitter_;
            enabled_exposedfield enabled_;
            exposedfield<sfbool> loop_;
            set_start_time_listener set_start_time_listener_;
            sftime start_time_;
            sftime_emitter start_time_changed_emitter_;
            exposedfield<sftime> stop_time_;
            sftime cycle_time_;
            sftime_emitter cycle_time_emitter_;
            sffloat fraction_changed_;
            sffloat_emitter fraction_changed_emitter_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sftime time_;
            sftime_emitter time_emitter_;

            double lastTime;

        public:
            time_sensor_node(const node_type & type,
                             const scope_ptr & scope);
            virtual ~time_sensor_node() throw ();

            virtual time_sensor_node * to_time_sensor() const;

            void update(double time);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
        };


        class touch_sensor_class : public node_class {
        public:
            explicit touch_sensor_class(openvrml::browser & browser);
            virtual ~touch_sensor_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class touch_sensor_node : public abstract_child_node {
            friend class touch_sensor_class;

            exposedfield<sfbool> enabled_;
            sfvec3f hit_normal_changed_;
            sfvec3f_emitter hit_normal_changed_emitter_;
            sfvec3f hit_point_changed_;
            sfvec3f_emitter hit_point_changed_emitter_;
            sfvec2f hit_tex_coord_changed_;
            sfvec2f_emitter hit_tex_coord_changed_emitter_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfbool is_over_;
            sfbool_emitter is_over_emitter_;
            sftime touch_time_;
            sftime_emitter touch_time_emitter_;

        public:
            touch_sensor_node(const node_type & type, const scope_ptr & scope);
            virtual ~touch_sensor_node() throw ();

            virtual touch_sensor_node * to_touch_sensor() const;

            void activate(double timeStamp, bool isOver, bool isActive,
                          double *p );

            bool enabled() const;
        };


        class transform_class : public node_class {
        public:
            explicit transform_class(openvrml::browser & browser);
            virtual ~transform_class() throw ();

            virtual const node_type_ptr
            create_type(const std::string & id,
                        const node_interface_set & interfaces)
                throw (unsupported_interface, std::bad_alloc);
        };

        class transform_node : public group_node,
                               public openvrml::transform_node {
            friend class transform_class;

            class center_exposedfield : public exposedfield<sfvec3f> {
            public:
                explicit center_exposedfield(transform_node & node);
                virtual ~center_exposedfield() throw ();

            private:
                virtual void do_process_event(const sfvec3f & center,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class rotation_exposedfield : public exposedfield<sfrotation> {
            public:
                explicit rotation_exposedfield(transform_node & node);
                virtual ~rotation_exposedfield() throw ();

            private:
                virtual void do_process_event(const sfrotation & rotation,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class scale_exposedfield : public exposedfield<sfvec3f> {
            public:
                explicit scale_exposedfield(transform_node & node);
                virtual ~scale_exposedfield() throw ();

            private:
                virtual void do_process_event(const sfvec3f & scale,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class scale_orientation_exposedfield :
                public exposedfield<sfrotation> {
            public:
                explicit scale_orientation_exposedfield(transform_node & node);
                virtual ~scale_orientation_exposedfield() throw ();

            private:
                virtual void
                do_process_event(const sfrotation & scale_orientation,
                                 double timestamp)
                    throw (std::bad_alloc);
            };

            class translation_exposedfield : public exposedfield<sfvec3f> {
            public:
                explicit translation_exposedfield(transform_node & node);
                virtual ~translation_exposedfield() throw ();

            private:
                virtual void do_process_event(const sfvec3f & translation,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            center_exposedfield center_;
            rotation_exposedfield rotation_;
            scale_exposedfield scale_;
            scale_orientation_exposedfield scale_orientation_;
            translation_exposedfield translation_;

            mutable mat4f transform_;
            mutable bool transform_dirty;
            viewer::object_t xformObject;

        public:
            transform_node(const node_type & type,
                           const scope_ptr & scope);
            virtual ~transform_node() throw ();

            virtual const openvrml::bounding_volume & bounding_volume() const;

            virtual const mat4f & transform() const throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            void recalc_bsphere();
            void update_transform() const throw ();
        };


        class viewpoint_node;

        class viewpoint_class : public node_class {
            typedef std::vector<viewpoint_node *> bound_nodes_t;

            viewpoint_node * first;
            bound_nodes_t bound_nodes;

        public:
            explicit viewpoint_class(openvrml::browser & browser);
            virtual ~viewpoint_class() throw ();

            void set_first(viewpoint_node & viewpoint) throw ();
            bool has_first() const throw ();
            void bind(viewpoint_node & viewpoint, double timestamp)
                throw (std::bad_alloc);
            void unbind(viewpoint_node & viewpoint, double timestamp) throw ();

            virtual void
            initialize(openvrml::viewpoint_node * initial_viewpoint,
                       double timestamp) throw ();
            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class viewpoint_node : public abstract_base,
                               public openvrml::viewpoint_node {
            friend class viewpoint_class;

            class set_bind_listener : public sfbool_listener {
            public:
                explicit set_bind_listener(viewpoint_node & node);
                virtual ~set_bind_listener() throw ();

                virtual void process_event(const sfbool & value,
                                           double timestamp)
                    throw (std::bad_alloc);
            };

            class orientation_exposedfield : public exposedfield<sfrotation> {
            public:
                explicit orientation_exposedfield(viewpoint_node & node);
                virtual ~orientation_exposedfield() throw ();

            private:
                virtual void do_process_event(const sfrotation & orientation,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class position_exposedfield : public exposedfield<sfvec3f> {
            public:
                explicit position_exposedfield(viewpoint_node & node);
                virtual ~position_exposedfield() throw ();

            private:
                virtual void do_process_event(const sfvec3f & position,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_bind_listener set_bind_listener_;
            exposedfield<sffloat> field_of_view_;
            exposedfield<sfbool> jump_;
            orientation_exposedfield orientation_;
            position_exposedfield position_;
            sfstring description_;
            sfbool is_bound_;
            sfbool_emitter is_bound_emitter_;
            sftime bind_time_;
            sftime_emitter bind_time_emitter_;

            mat4f parent_transform;
            mutable mat4f final_transformation;
            mutable bool final_transformation_dirty;
            mat4f user_view_transform_;

        public:
            viewpoint_node(const node_type & type, const scope_ptr & scope);
            virtual ~viewpoint_node() throw ();

            virtual const mat4f & transformation() const throw ();
            virtual const mat4f & user_view_transform() const throw ();
            virtual void user_view_transform(const mat4f & transform)
                throw ();
            virtual const std::string & description() const throw ();
            virtual float field_of_view() const throw ();

            const sfrotation & orientation() const;
            const sfvec3f & position() const;

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_relocate() throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            void update_final_transformation() const throw ();
        };


        class visibility_sensor_class : public node_class {
        public:
            explicit visibility_sensor_class(openvrml::browser & browser);
            virtual ~visibility_sensor_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class visibility_sensor_node : public abstract_child_node {
            friend class visibility_sensor_class;

            exposedfield<sfvec3f> center_;
            exposedfield<sfbool> enabled_;
            exposedfield<sfvec3f> size_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sftime enter_time_;
            sftime_emitter enter_time_emitter_;
            sftime exit_time_;
            sftime_emitter exit_time_emitter_;

        public:
            visibility_sensor_node(const node_type & type,
                                   const scope_ptr & scope);
            virtual ~visibility_sensor_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class world_info_class : public node_class {
        public:
            explicit world_info_class(openvrml::browser & browser);
            virtual ~world_info_class() throw ();

            virtual const node_type_ptr
            create_type(const std::string & id,
                        const node_interface_set & interfaces)
                throw (unsupported_interface, std::bad_alloc);
        };

        class world_info_node : public abstract_child_node {
            friend class world_info_class;

            mfstring info;
            sfstring title;

        public:
            world_info_node(const node_type & type, const scope_ptr & scope);
            virtual ~world_info_node() throw ();
        };
    }
}

# endif
