// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 1999  Kumaran Santhanam
// Copyright (C) 2001  Braden McDaniel
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

            virtual void do_process_event(const std::string & id,
                                          const field_value & value,
                                          double timestamp)
                throw (unsupported_interface, std::bad_cast, std::bad_alloc);

            virtual const field_value &
            do_eventout(const std::string & id) const
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

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

        protected:
            abstract_geometry_node(const node_type & type,
                                   const scope_ptr & scope);

            viewer::object_t viewerObject;
        };


        class abstract_indexed_set_node : public abstract_geometry_node {
        protected:
            sfnode color_;
            mfint32 colorIndex;
            sfbool colorPerVertex;
            sfnode coord;
            mfint32 coordIndex;

        public:
            virtual ~abstract_indexed_set_node() throw () = 0;

            virtual bool modified() const;

            virtual const color_node * color() const throw ();

        protected:
            abstract_indexed_set_node(const node_type & type,
                                      const scope_ptr & scope);

            //
            // eventIn handlers
            //
            void process_set_color(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_colorIndex(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_coord(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_coordIndex(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast, std::bad_alloc);
        };


        class abstract_light_node : public abstract_child_node {
        protected:
            sffloat ambientIntensity;
            sfcolor color_;
            sffloat intensity_;
            sfbool on_;

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

            //
            // eventIn handlers
            //
            void process_set_ambientIntensity(const field_value & value,
                                              double timestamp)
                throw (std::bad_cast);
            void process_set_color(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast);
            void process_set_intensity(const field_value & value,
                                       double timestamp) throw (std::bad_cast);
            void process_set_on(const field_value & value, double timestamp)
                throw (std::bad_cast);
        };


        class abstract_texture_node : public abstract_base,
                                      public texture_node {
        protected:
            sfbool repeatS;
            sfbool repeatT;

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
            sfvec3f bboxCenter;
            sfvec3f bboxSize;
            mfnode children_;

            viewer::object_t viewerObject;

            bounding_sphere bsphere;

        public:
            group_node(const node_type & type, const scope_ptr & scope);
            virtual ~group_node() throw ();

            virtual const openvrml::bounding_volume & bounding_volume() const;
            virtual bool modified() const;
            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timeStamp, bool isOver, bool isActive,
                                  double *p);

            void render_nocull(openvrml::viewer & viewer,
                               rendering_context context);

        protected:
            //
            // eventIn handlers
            //
            void process_addChildren(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_removeChildren(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_children(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);

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

            sfstring description;
            mfstring parameter;
            mfstring url;

        public:
            anchor_node(const node_type & type,
                        const scope_ptr & scope);
            virtual ~anchor_node() throw ();

            virtual anchor_node * to_anchor() const;

            virtual const openvrml::bounding_volume & bounding_volume() const;
            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

            void activate();

        private:
            //
            // eventIn handlers
            //
            void process_set_description(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_parameter(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_url(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfnode material_;
            sfnode texture_;
            sfnode textureTransform;

        public:
            appearance_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~appearance_node() throw ();

            virtual bool modified() const;

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

            //
            // appearance_node implementation
            //
            virtual const node_ptr & material() const throw ();
            virtual const node_ptr & texture() const throw ();
            virtual const node_ptr & texture_transform() const throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_material(const field_value & value,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_texture(const field_value & value,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_textureTransform(const field_value & value,
                                              double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
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

            sfstring description;
            sfbool loop;
            sffloat pitch;
            sftime startTime;
            sftime stopTime;
            mfstring url;
            sftime duration;
            sfbool active;

        public:
            audio_clip_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~audio_clip_node() throw ();

            void update(double time);

            virtual audio_clip_node * to_audio_clip() const;

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void process_set_description(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_loop(const field_value & value, double timestamp)
                throw (std::bad_cast);
            void process_set_pitch(const field_value & value,
                                  double timestamp)
                throw (std::bad_cast);
            void process_set_startTime(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_stopTime(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_url(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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
            virtual void render(openvrml::viewer & viewer) throw ();
            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class background_node : public abstract_child_node {
            friend class background_class;

            mffloat groundAngle;
            mfcolor groundColor;
            mfstring backUrl;
            mfstring bottomUrl;
            mfstring frontUrl;
            mfstring leftUrl;
            mfstring rightUrl;
            mfstring topUrl;
            mffloat skyAngle;
            mfcolor skyColor;
            sfbool bound;

            // Texture caches
            img * texPtr[6];
            img tex[6];

            // Display list object for background
            viewer::object_t viewerObject;

        public:
            background_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~background_node() throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void process_set_bind(const field_value & value, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_groundAngle(const field_value & value,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_groundColor(const field_value & value,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_backUrl(const field_value & value,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_bottomUrl(const field_value & value,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_frontUrl(const field_value & value,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_leftUrl(const field_value & value,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_rightUrl(const field_value & value,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_topUrl(const field_value & value,
                                   double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_skyAngle(const field_value & value,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_skyColor(const field_value & value,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
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

            sfvec3f axisOfRotation;

            viewer::object_t xformObject;

        public:
            static const mat4f billboard_to_matrix(const billboard_node & node,
                                                   const mat4f & modelview);

            billboard_node(const node_type & type,
                           const scope_ptr & scope);
            virtual ~billboard_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_axisOfRotation(const field_value & value,
                                           double timestamp)
                    throw (std::bad_cast);
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
            box_node(const node_type & type,
                     const scope_ptr & scope);
            virtual ~box_node() throw ();

            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
                            rendering_context context);
            virtual const openvrml::bounding_volume & bounding_volume() const;
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

            sfbool collide;
            sfnode proxy;
            sftime collideTime;

        public:
            collision_node(const node_type & type,
                           const scope_ptr & scope);
            virtual ~collision_node() throw ();

            virtual bool modified() const;

        private:
            //
            // eventIn handlers
            //
            void process_set_collide(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
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

            mfcolor color_;

        public:
            color_node(const node_type & type,
                       const scope_ptr & scope);
            virtual ~color_node() throw ();

            //
            // color_node implementation
            //
            virtual const std::vector<openvrml::color> & color() const
                throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_color(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            mffloat key;
            mfcolor keyValue;
            sfcolor value;

        public:
            color_interpolator_node(const node_type & type,
                                    const scope_ptr & scope);
            virtual ~color_interpolator_node() throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_fraction(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_key(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_keyValue(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
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

            mfvec3f point_;

        public:
            coordinate_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~coordinate_node() throw ();

            //
            // coordinate_node implementation
            //
            virtual const std::vector<vec3f> & point() const throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_point(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            mffloat key;
            mfvec3f keyValue;
            mfvec3f value;

        public:
            coordinate_interpolator_node(const node_type & type,
                                         const scope_ptr & scope);
            virtual ~coordinate_interpolator_node() throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_fraction(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_key(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_keyValue(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
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

            sfbool autoOffset;
            sffloat diskAngle;
            sfbool enabled_;
            sffloat maxAngle;
            sffloat minAngle;
            sffloat offset;
            sfbool active;
            sfrotation rotation;
            sfvec3f trackPoint;

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

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);
            void activate(double timeStamp, bool isActive, double *p);

            bool enabled() const;

        private:
            //
            // eventIn handlers
            //
            void process_set_autoOffset(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast);
            void process_set_diskAngle(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_enabled(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_maxAngle(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_minAngle(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_offset(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast);
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

            sfvec3f direction;

        public:
            directional_light_node(const node_type & type,
                                   const scope_ptr & scope);
            virtual ~directional_light_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_direction(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
        };


        class elevation_grid_class : public node_class {
        public:
            explicit elevation_grid_class(openvrml::browser & browser);
            virtual ~elevation_grid_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class elevation_grid_node :
            public abstract_geometry_node {

            friend class elevation_grid_class;

            sfnode color;
            sfnode normal;
            sfnode texCoord;
            sfbool ccw;
            sfbool colorPerVertex;
            sffloat creaseAngle;
            mffloat height;
            sfbool normalPerVertex;
            sfbool solid;
            sfint32 xDimension;
            sffloat xSpacing;
            sfint32 zDimension;
            sffloat zSpacing;

        public:
            elevation_grid_node(const node_type & type,
                                const scope_ptr & scope);
            virtual ~elevation_grid_node() throw ();

            virtual bool modified() const;
            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
                            rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_color(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_height(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_normal(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_texCoord(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfbool beginCap;
            sfbool ccw;
            sfbool convex;
            sffloat creaseAngle;
            mfvec2f crossSection;
            sfbool endCap;
            mfrotation orientation;
            mfvec2f scale;
            sfbool solid;
            mfvec3f spine;

        public:
            extrusion_node(const node_type & type,
                           const scope_ptr & scope);
            virtual ~extrusion_node() throw ();

            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
                            rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_crossSection(const field_value & value,
                                          double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_orientation(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_scale(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_spine(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfcolor color;
            sfstring fogType;
            sffloat visibilityRange;
            sfbool bound;

        public:
            fog_node(const node_type & type,
                     const scope_ptr & scope);
            virtual ~fog_node() throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void process_set_bind(const field_value & value, double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_color(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast);
            void process_set_fogType(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_visibilityRange(const field_value & value,
                                             double timestamp)
                throw (std::bad_cast);
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

            mfstring url;

            img * image;

            viewer::texture_object_t texObject;

        public:
            image_texture_node(const node_type & type,
                               const scope_ptr & scope);
            virtual ~image_texture_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

            virtual size_t components() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t frames() const throw ();
            virtual const unsigned char * pixels() const throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_url(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfbool ccw;
            sfbool convex;
            sffloat creaseAngle;
            sfnode normal;
            mfint32 normalIndex;
            sfbool normalPerVertex;
            sfbool solid;
            sfnode texCoord;
            mfint32 texCoordIndex;

            bounding_sphere bsphere;

        public:
            indexed_face_set_node(const node_type & type,
                                  const scope_ptr & scope);
            virtual ~indexed_face_set_node() throw ();

            virtual bool modified() const;
            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
                            rendering_context context);
            virtual const openvrml::bounding_volume & bounding_volume() const;

        private:
            //
            // eventIn handlers
            //
            void process_set_normal(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_normalIndex(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_texCoord(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_texCoordIndex(const field_value & value,
                                          double timestamp)
                throw (std::bad_cast, std::bad_alloc);

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

            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
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

        class inline_node : public abstract_base,
                                           public grouping_node {
            friend class inline_class;

            sfvec3f bboxCenter;
            sfvec3f bboxSize;
            mfstring url;

            openvrml::scene * inlineScene;
            bool hasLoaded;

        public:
            inline_node(const node_type & type,
                        const scope_ptr & scope);
            virtual ~inline_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);
            virtual inline_node * to_inline() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            void load();

            //
            // eventIn handlers
            //
            void process_set_url(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
        };


        class lod_class : public node_class {
        public:
            explicit lod_class(openvrml::browser & browser);
            virtual ~lod_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class lod_node : public abstract_base,
                         public grouping_node {
            friend class lod_class;

            mfnode level;
            sfvec3f center;
            mffloat range;

            mfnode children_;
            bounding_sphere bsphere;

        public:
            lod_node(const node_type & type,
                     const scope_ptr & scope);
            virtual ~lod_node() throw ();

            virtual bool modified() const;
            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);
            virtual const openvrml::bounding_volume & bounding_volume() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            //
            // eventIn handlers
            //
            void process_set_level(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);

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

            sffloat ambientIntensity;
            sfcolor diffuseColor;
            sfcolor emissiveColor;
            sffloat shininess_;
            sfcolor specularColor;
            sffloat transparency_;

        public:
            material_node(const node_type & type,
                          const scope_ptr & scope);
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

        private:
            //
            // eventIn handlers
            //
            void process_set_ambientIntensity(const field_value & value,
                                              double timestamp)
                throw (std::bad_cast);
            void process_set_diffuseColor(const field_value & value,
                                          double timestamp)
                throw (std::bad_cast);
            void process_set_emissiveColor(const field_value & value,
                                           double timestamp)
                throw (std::bad_cast);
            void process_set_shininess(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_specularColor(const field_value & value,
                                           double timestamp)
                throw (std::bad_cast);
            void process_set_transparency(const field_value & value,
                                          double timestamp)
                throw (std::bad_cast);
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

            sfbool loop;
            sffloat speed;
            sftime startTime;
            sftime stopTime;
            mfstring url;
            sftime duration;
            sfbool active;

            img * image;
            int frame, lastFrame;
            double lastFrameTime;

            viewer::texture_object_t texObject;

        public:
            movie_texture_node(const node_type & type,
                               const scope_ptr & scope);
            virtual ~movie_texture_node() throw ();

            virtual movie_texture_node * to_movie_texture() const;

            void update(double time);

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

            virtual size_t components() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t frames() const throw ();
            virtual const unsigned char * pixels() const throw ();

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void process_set_loop(const field_value & value, double timestamp)
                throw (std::bad_cast);
            void process_set_speed(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast);
            void process_set_startTime(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_stopTime(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_url(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            mffloat avatarSize;
            sfbool headlight_;
            sffloat speed_;
            mfstring type;
            sffloat visibilityLimit;
            sfbool bound;

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

            //
            // eventIn handlers
            //
            void process_set_avatarSize(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_bind(const field_value & value, double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_headlight(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_speed(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast);
            void process_set_type(const field_value & value,
                                  double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_visibilityLimit(const field_value & value,
                                             double timestamp)
                throw (std::bad_cast);
        };


        class normal_class : public node_class {
        public:
            explicit normal_class(openvrml::browser & browser);
            virtual ~normal_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class normal_node : public abstract_base,
                            public openvrml::normal_node {
            friend class normal_class;

            mfvec3f vector_;

        public:
            normal_node(const node_type & type,
                        const scope_ptr & scope);
            virtual ~normal_node() throw ();

            //
            // normal_node implementation.
            //
            virtual const std::vector<vec3f> & vector() const throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_vector(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            mffloat key;
            mfvec3f keyValue;
            mfvec3f value;

        public:
            normal_interpolator_node(const node_type & type,
                                     const scope_ptr & scope);
            virtual ~normal_interpolator_node() throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_fraction(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_key(const field_value & value, double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_keyValue(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            mffloat key;
            mfrotation keyValue;
            sfrotation value;

        public:
            orientation_interpolator_node(const node_type & type,
                                          const scope_ptr & scope);
            virtual ~orientation_interpolator_node() throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_fraction(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_key(const field_value & value, double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_keyValue(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfimage image;

            viewer::texture_object_t texObject;

        public:
            pixel_texture_node(const node_type & type,
                               const scope_ptr & scope);
            virtual ~pixel_texture_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

            virtual size_t components() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t frames() const throw ();
            virtual const unsigned char * pixels() const throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_image(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfbool autoOffset;
            sfbool enabled_;
            sfvec2f maxPosition;
            sfvec2f minPosition;
            sfvec3f offset;
            sfbool active;
            sfvec3f translation;
            sfvec3f trackPoint;

            sfvec3f activationPoint;

            mat4f activationMatrix;
            mat4f modelview;

        public:
            plane_sensor_node(const node_type & type,
                              const scope_ptr & scope);
            virtual ~plane_sensor_node() throw ();

            virtual plane_sensor_node * to_plane_sensor() const;

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);
            void activate( double timeStamp, bool isActive, double *p );

            bool enabled() const;

        private:
            //
            // eventIn handlers
            //
            void process_set_autoOffset(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast);
            void process_set_enabled(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_maxPosition(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast);
            void process_set_minPosition(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast);
            void process_set_offset(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast);
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

            sfvec3f attenuation;
            sfvec3f location;
            sffloat radius;

        public:
            point_light_node(const node_type & type,
                             const scope_ptr & scope);
            virtual ~point_light_node() throw ();

            virtual point_light_node * to_point_light() const;

            virtual void renderScoped(openvrml::viewer & viewer);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void process_set_attenuation(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast);
            void process_set_location(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_radius(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast);
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

            sfnode color;
            sfnode coord;

            bounding_sphere bsphere;

        public:
            point_set_node(const node_type & type,
                           const scope_ptr & scope);
            virtual ~point_set_node() throw ();

            virtual bool modified() const;
            virtual viewer::object_t insert_geometry(openvrml::viewer & viewer,
                                                  rendering_context context);
            virtual const openvrml::bounding_volume & bounding_volume() const;

        private:
            //
            // eventIn handlers
            //
            void process_set_color(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_coord(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);

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

            mffloat key;
            mfvec3f keyValue;
            sfvec3f value;

        public:
            position_interpolator_node(const node_type & type,
                                       const scope_ptr & scope);
            virtual ~position_interpolator_node() throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_fraction(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_key(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_keyValue(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfvec3f center;
            sfbool enabled;
            sfvec3f size;
            sfbool active;
            sfvec3f position;
            sfrotation orientation;
            sftime enterTime;
            sftime exitTime;

        public:
            proximity_sensor_node(const node_type & type,
                                  const scope_ptr & scope);
            virtual ~proximity_sensor_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_center(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast);
            void process_set_size(const field_value & value,
                                  double timestamp)
                throw (std::bad_cast);
            void process_set_enabled(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
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

            mffloat key;
            mffloat keyValue;
            sffloat value;

        public:
            scalar_interpolator_node(const node_type & type,
                                     const scope_ptr & scope);
            virtual ~scalar_interpolator_node() throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_fraction(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_key(const field_value & value,
                                 double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_keyValue(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfnode appearance;
            sfnode geometry;

            viewer::object_t viewerObject; // move to Node.h ? ...

        public:
            shape_node(const node_type & type,
                       const scope_ptr & scope);
            virtual ~shape_node() throw ();

            virtual bool modified() const;
            virtual const openvrml::bounding_volume & bounding_volume() const;
            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_appearance(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_geometry(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            sfvec3f direction;
            sffloat intensity;
            sfvec3f location;
            sffloat maxBack;
            sffloat maxFront;
            sffloat minBack;
            sffloat minFront;
            sffloat priority;
            sfnode source;
            sfbool spatialize;

        public:
            sound_node(const node_type & type,
                       const scope_ptr & scope);
            virtual ~sound_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_direction(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_intensity(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_location(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_maxBack(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_maxFront(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_minBack(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_minFront(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_priority(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_source(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast, std::bad_alloc);
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

            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
                            rendering_context context);
            virtual const openvrml::bounding_volume & bounding_volume() const;
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

            sfbool autoOffset;
            sfbool enabled;
            sfrotation offset;
            sfbool active;
            sfrotation rotation;
            sfvec3f trackPoint;

            sfvec3f activationPoint;
            sfvec3f centerPoint;
            mat4f modelview;

        public:
            sphere_sensor_node(const node_type & type,
                               const scope_ptr & scope);
            virtual ~sphere_sensor_node() throw ();

            virtual sphere_sensor_node * to_sphere_sensor() const;

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

            void activate(double timeStamp, bool isActive, double * p);
            bool isEnabled() const throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_autoOffset(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast);
            void process_set_enabled(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_offset(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast);
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

            sfvec3f attenuation;
            sffloat beamWidth;
            sffloat cutOffAngle;
            sfvec3f direction;
            sfvec3f location;
            sffloat radius;

        public:
            spot_light_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~spot_light_node() throw ();

            virtual spot_light_node * to_spot_light() const;

            virtual void renderScoped(openvrml::viewer & viewer);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void process_set_attenuation(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast);
            void process_set_beamWidth(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_cutOffAngle(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast);
            void process_set_direction(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_location(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_radius(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast);
        };


        class switch_class : public node_class {
        public:
            explicit switch_class(openvrml::browser & browser);
            virtual ~switch_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class switch_node : public abstract_base,
                            public grouping_node {
            friend class switch_class;

            mfnode choice;
            sfint32 whichChoice;

            mfnode children_;
            bounding_sphere bsphere;

        public:
            switch_node(const node_type & type,
                        const scope_ptr & scope);
            virtual ~switch_node() throw ();

            virtual bool modified() const;
            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);
            virtual const openvrml::bounding_volume & bounding_volume() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            //
            // eventIn handlers
            //
            void process_set_choice(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_whichChoice(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast);

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

            mfstring string;
            sfnode fontStyle;
            mffloat length;
            sffloat maxExtent;

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
            text_node(const node_type & type,
                      const scope_ptr & scope);
            virtual ~text_node() throw ();

            virtual bool modified() const;
            virtual viewer::object_t
            insert_geometry(openvrml::viewer & viewer,
                            rendering_context context);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void process_set_string(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_fontStyle(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_length(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void process_set_maxExtent(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);

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

            mfvec2f point_;

        public:
            texture_coordinate_node(const node_type & type,
                                    const scope_ptr & scope);
            virtual ~texture_coordinate_node() throw ();

            //
            // texture_coordinate_node implementation.
            //
            virtual const std::vector<vec2f> & point() const throw();

        private:
            //
            // eventIn handlers
            //
            void process_set_point(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
        };


        class texture_transform_class : public node_class {
        public:
            explicit texture_transform_class(openvrml::browser & browser);
            virtual ~texture_transform_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class texture_transform_node :
            public abstract_base,
            public openvrml::texture_transform_node {

            friend class texture_transform_class;

            sfvec2f center;
            sffloat rotation;
            sfvec2f scale;
            sfvec2f translation;

        public:
            texture_transform_node(const node_type & type,
                                   const scope_ptr & scope);
            virtual ~texture_transform_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_center(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast);
            void process_set_rotation(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
            void process_set_scale(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast);
            void process_set_translation(const field_value & value,
                                         double timestamp)
                throw (std::bad_cast);
        };


        class time_sensor_class : public node_class {
        public:
            explicit time_sensor_class(openvrml::browser & browser);
            virtual ~time_sensor_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class time_sensor_node : public abstract_child_node {
            friend class time_sensor_class;

            sftime cycleInterval;
            sfbool enabled;
            sfbool loop;
            sftime startTime;
            sftime stopTime;
            sftime cycleTime;
            sffloat fraction;
            sfbool active;
            sftime time;

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

            //
            // eventIn handlers
            //
            void process_set_cycleInterval(const field_value & value,
                                           double timestamp)
                throw (std::bad_cast);
            void process_set_enabled(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_loop(const field_value & value, double timestamp)
                throw (std::bad_cast);
            void process_set_startTime(const field_value & value,
                                       double timestamp)
                throw (std::bad_cast);
            void process_set_stopTime(const field_value & value,
                                      double timestamp)
                throw (std::bad_cast);
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

            sfbool enabled_;
            sfvec3f hitNormal;
            sfvec3f hitPoint;
            sfvec2f hitTexCoord;
            sfbool active;
            sfbool over;
            sftime touchTime;

        public:
            touch_sensor_node(const node_type & type,
                              const scope_ptr & scope);
            virtual ~touch_sensor_node() throw ();

            virtual touch_sensor_node * to_touch_sensor() const;

            void activate(double timeStamp, bool isOver, bool isActive,
                          double *p );

            bool enabled() const;

        private:
            //
            // eventIn handlers
            //
            void process_set_enabled(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
        };


        class transform_class : public node_class {
        public:
            explicit transform_class(openvrml::browser & browser);
            virtual ~transform_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class transform_node : public group_node,
                               public openvrml::transform_node {
            friend class transform_class;

            sfvec3f center;
            sfrotation rotation;
            sfvec3f scale;
            sfrotation scaleOrientation;
            sfvec3f translation;

            mutable mat4f transform_;
            mutable bool transform_dirty;
            viewer::object_t xformObject;

        public:
            transform_node(const node_type & type,
                           const scope_ptr & scope);
            virtual ~transform_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

            virtual const openvrml::bounding_volume & bounding_volume() const;

            virtual const mat4f & transform() const throw ();

        private:
            //
            // eventIn handlers
            //
            void process_set_center(const field_value & value,
                                   double timestamp)
                throw (std::bad_cast);
            void process_set_rotation(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_scale(const field_value & value,
                                  double timestamp)
                throw (std::bad_cast);
            void process_set_scaleOrientation(const field_value & value,
                                             double timestamp)
                throw (std::bad_cast);
            void process_set_translation(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast);

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

            sffloat fieldOfView;
            sfbool jump;
            sfrotation orientation_;
            sfvec3f position_;
            sfstring description_;
            sfbool bound;
            sftime bindTime;

            mat4f parent_transform;
            mutable mat4f final_transformation;
            mutable bool final_transformation_dirty;
            mat4f user_view_transform_;

        public:
            viewpoint_node(const node_type & type,
                           const scope_ptr & scope);
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

            //
            // eventIn handlers
            //
            void process_set_bind(const field_value & value, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void process_set_fieldOfView(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast);
            void process_set_jump(const field_value & value, double timestamp)
                throw (std::bad_cast);
            void process_set_orientation(const field_value & value,
                                        double timestamp)
                throw (std::bad_cast);
            void process_set_position(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);

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

            sfvec3f center;
            sfbool enabled;
            sfvec3f size;
            sfbool active;
            sftime enterTime;
            sftime exitTime;

        public:
            visibility_sensor_node(const node_type & type,
                                   const scope_ptr & scope);
            virtual ~visibility_sensor_node() throw ();

            virtual void render(openvrml::viewer & viewer,
                                rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void process_set_center(const field_value & value,
                                    double timestamp)
                throw (std::bad_cast);
            void process_set_enabled(const field_value & value,
                                     double timestamp)
                throw (std::bad_cast);
            void process_set_size(const field_value & value,
                                  double timestamp)
                throw (std::bad_cast);
        };


        class world_info_class : public node_class {
        public:
            explicit world_info_class(openvrml::browser & browser);
            virtual ~world_info_class() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class world_info_node : public abstract_child_node {
            friend class world_info_class;

            mfstring info;
            sfstring title;

        public:
            world_info_node(const node_type & type,
                            const scope_ptr & scope);
            virtual ~world_info_node() throw ();
        };
    }
}

# endif
