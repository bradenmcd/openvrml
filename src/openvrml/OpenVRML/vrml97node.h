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

#   include <OpenVRML/common.h>
#   include <OpenVRML/node.h>
#   include <OpenVRML/img.h>
#   include <OpenVRML/bounding_volume.h>

typedef unsigned int FT_UInt;
typedef struct FT_LibraryRec_ * FT_Library;
typedef struct FT_FaceRec_ * FT_Face;
typedef unsigned int FcChar32;

namespace OpenVRML {

    namespace Vrml97Node {

        class OPENVRML_SCOPE AbstractBase : public virtual node {
        public:
            virtual ~AbstractBase() throw () = 0;

        protected:
            AbstractBase(const node_type & type, const scope_ptr & scope);

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


        class OPENVRML_SCOPE AbstractChild : public AbstractBase,
                                             public child_node {
        public:
            virtual ~AbstractChild() throw () = 0;

        protected:
            AbstractChild(const node_type & type, const scope_ptr & scope);
        };


        class OPENVRML_SCOPE AbstractGeometry : public AbstractBase,
                                                public geometry_node {
        public:
            virtual ~AbstractGeometry() throw () = 0;

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

        protected:
            AbstractGeometry(const node_type & type, const scope_ptr & scope);

            viewer::object_t d_viewerObject; // move to Node? ...
        };


        class OPENVRML_SCOPE AbstractIndexedSet : public AbstractGeometry {
        protected:
            sfnode color_;
            mfint32 colorIndex;
            sfbool colorPerVertex;
            sfnode coord;
            mfint32 coordIndex;

        public:
            virtual ~AbstractIndexedSet() throw () = 0;

            virtual bool modified() const;
            virtual void update_modified(node_path & path, int flags = 0x003);

            virtual const color_node * color() const throw ();

        protected:
            AbstractIndexedSet(const node_type & type,
                               const scope_ptr & scope);

            //
            // eventIn handlers
            //
            void processSet_color(const field_value & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_colorIndex(const field_value & mfint32,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_coord(const field_value & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_coordIndex(const field_value & mfint32,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE AbstractLight : public AbstractChild {
        protected:
            sffloat ambientIntensity;
            sfcolor color;
            sffloat intensity;
            sfbool on;

        public:
            virtual ~AbstractLight() throw () = 0;

            virtual void renderScoped(OpenVRML::viewer & viewer);
            virtual AbstractLight * to_light() const;

            float getAmbientIntensity() const
            {
                return this->ambientIntensity.value;
            }

            float getIntensity() const
            {
                return this->intensity.value;
            }

            bool getOn() const
            {
                return this->on.value;
            }

            const OpenVRML::color & getColor() const
            {
                return this->color.value;
            }

        protected:
            AbstractLight(const node_type & type, const scope_ptr & scope);

            //
            // eventIn handlers
            //
            void processSet_ambientIntensity(const field_value & sffloat,
                                             double timestamp)
                    throw (std::bad_cast);
            void processSet_color(const field_value & sfcolor, double timestamp)
                    throw (std::bad_cast);
            void processSet_intensity(const field_value & sffloat,
                                      double timestamp) throw (std::bad_cast);
            void processSet_on(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE AbstractTexture : public AbstractBase,
                                               public texture_node {
        protected:
            sfbool repeatS;
            sfbool repeatT;

        public:
            virtual ~AbstractTexture() throw () = 0;

            //
            // texture_node implementation.
            //
            virtual bool repeat_s() const throw ();
            virtual bool repeat_t() const throw ();

        protected:
            AbstractTexture(const node_type & type, const scope_ptr & scope);
        };


        class OPENVRML_SCOPE GroupClass : public node_class {
        public:
            explicit GroupClass(OpenVRML::browser & browser);
            virtual ~GroupClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Group : public AbstractBase,
                                     public virtual grouping_node {
            friend class GroupClass;

        protected:
            sfvec3f bboxCenter;
            sfvec3f bboxSize;
            mfnode children_;
            sfstring relative;

            viewer::object_t viewerObject;

            /**
             * Cached copy of the bsphere enclosing this node's children.
             */
            bounding_sphere bsphere;

        public:
            Group(const node_type & type, const scope_ptr & scope);
            virtual ~Group() throw ();

            virtual const OpenVRML::bounding_volume & bounding_volume() const;
            virtual bool modified() const;
            virtual void update_modified(node_path& path, int flags);
            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timeStamp, bool isOver, bool isActive, double *p);

            void renderNoCull(OpenVRML::viewer & viewer, rendering_context context);

        protected:
            //
            // eventIn handlers
            //
            void processAddChildren(const field_value & mfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processRemoveChildren(const field_value & mfnode,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_children(const field_value & mfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);

            /**
             * Construct a bounding sphere around this node's children. Store it
             * in d_bsphere.
             */
            virtual void recalcBSphere();
        };


        class OPENVRML_SCOPE AnchorClass : public node_class {
        public:
            explicit AnchorClass(OpenVRML::browser & browser);
            virtual ~AnchorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Anchor : public Group {
            friend class AnchorClass;

            sfstring description;
            mfstring parameter;
            mfstring url;

        public:
            Anchor(const node_type & type,
                   const scope_ptr & scope);
            virtual ~Anchor() throw ();

            virtual Anchor * to_anchor() const;

            virtual const OpenVRML::bounding_volume & bounding_volume() const;
            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

            void activate();

        private:
            //
            // field mutators
            //
            void setDescription(const field_value & sfstring)
                    throw (std::bad_cast, std::bad_alloc);
            void setParameter(const field_value & mfstring)
                    throw (std::bad_cast, std::bad_alloc);
            void setUrl(const field_value & mfstring)
                    throw (std::bad_cast, std::bad_alloc);

            //
            // eventIn handlers
            //
            void processSet_description(const field_value & sfstring,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_parameter(const field_value & mfstring,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_url(const field_value & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE AppearanceClass : public node_class {
        public:
            explicit AppearanceClass(OpenVRML::browser & browser);
            virtual ~AppearanceClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Appearance : public AbstractBase,
                                          public appearance_node {
            friend class AppearanceClass;

            sfnode material_;
            sfnode texture_;
            sfnode textureTransform;

        public:
            Appearance(const node_type & type,
                       const scope_ptr & scope);
            virtual ~Appearance() throw ();

            virtual bool modified() const;
            virtual void update_modified(node_path & path, int flags = 0x003);

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

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
            void processSet_material(const field_value & sfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_texture(const field_value & sfnode,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_textureTransform(const field_value & sfnode,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE AudioClipClass : public node_class {
        public:
            explicit AudioClipClass(OpenVRML::browser & browser);
            virtual ~AudioClipClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE AudioClip : public AbstractBase {
            friend class AudioClipClass;

            sfstring description;
            sfbool loop;
            sffloat pitch;
            sftime startTime;
            sftime stopTime;
            mfstring url;
            sftime duration;
            sfbool active;

            sfstring relativeUrl;

        public:
            AudioClip(const node_type & type,
                      const scope_ptr & scope);
            virtual ~AudioClip() throw ();

            void update(double time);

            virtual AudioClip * to_audio_clip() const;

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_description(const field_value & sfstring,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_loop(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_pitch(const field_value & sffloat,
                                  double timestamp)
                    throw (std::bad_cast);
            void processSet_startTime(const field_value & sftime,
                                      double timestamp)
                    throw (std::bad_cast);
            void processSet_stopTime(const field_value & sftime,
                                     double timestamp)
                    throw (std::bad_cast);
            void processSet_url(const field_value & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class Background;

        class OPENVRML_SCOPE BackgroundClass : public node_class {
            typedef std::vector<Background *> BoundNodes;

            Background * first;
            BoundNodes boundNodes;

        public:
            explicit BackgroundClass(OpenVRML::browser & browser);
            virtual ~BackgroundClass() throw ();

            void setFirst(Background & background) throw ();
            bool hasFirst() const throw ();
            void bind(Background & background, double timestamp)
                throw (std::bad_alloc);
            void unbind(Background & background, double timestamp) throw ();

            virtual void initialize(viewpoint_node * initialViewpoint,
                                    double timestamp) throw ();
            virtual void render(OpenVRML::viewer & viewer) throw ();
            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Background : public AbstractChild {
            friend class BackgroundClass;

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

            sfstring relativeUrl;

            // Texture caches
            img * texPtr[6];
            img tex[6];

            // Display list object for background
            viewer::object_t viewerObject;

        public:
            Background(const node_type & type,
                       const scope_ptr & scope);
            virtual ~Background() throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_bind(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_groundAngle(const field_value & mffloat,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_groundColor(const field_value & mfcolor,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_backUrl(const field_value & mfstring,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_bottomUrl(const field_value & mfstring,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_frontUrl(const field_value & mfstring,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_leftUrl(const field_value & mfstring,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_rightUrl(const field_value & mfstring,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_topUrl(const field_value & mfstring,
                                   double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_skyAngle(const field_value & mffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_skyColor(const field_value & mfcolor,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE BillboardClass : public node_class {
        public:
            explicit BillboardClass(OpenVRML::browser & browser);
            virtual ~BillboardClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Billboard : public Group {
            friend class BillboardClass;

            sfvec3f axisOfRotation;

            viewer::object_t xformObject;

        public:
            static void billboard_to_matrix(const Billboard * t_arg,
                                            const mat4f & MV,
                                            mat4f & M);

            Billboard(const node_type & type,
                      const scope_ptr & scope);
            virtual ~Billboard() throw ();

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

        private:
            //
            // field mutators
            //
            void setAxisOfRotation(const field_value & sfvec3f)
                    throw (std::bad_cast);

            //
            // eventIn handlers
            //
            void processSet_axisOfRotation(const field_value & sfvec3f,
                                           double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE BoxClass : public node_class {
        public:
            explicit BoxClass(OpenVRML::browser & browser);
            virtual ~BoxClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Box : public AbstractGeometry {
            friend class BoxClass;

            sfvec3f size;

            bounding_sphere bsphere;

        public:
            Box(const node_type & type,
                const scope_ptr & scope);
            virtual ~Box() throw ();

            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);
            virtual const OpenVRML::bounding_volume & bounding_volume() const;
        };


        class OPENVRML_SCOPE CollisionClass : public node_class {
        public:
            explicit CollisionClass(OpenVRML::browser & browser);
            virtual ~CollisionClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Collision : public Group {
            friend class CollisionClass;

            sfbool collide;
            sfnode proxy;
            sftime collideTime;  // eventOut

        public:
            Collision(const node_type & type,
                      const scope_ptr & scope);
            virtual ~Collision() throw ();

            virtual bool modified() const;

        private:
            //
            // eventIn handlers
            //
            void processSet_collide(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE ColorClass : public node_class {
        public:
            explicit ColorClass(OpenVRML::browser & browser);
            virtual ~ColorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Color : public AbstractBase, public color_node {
            friend class ColorClass;

            mfcolor color_;

        public:
            Color(const node_type & type,
                  const scope_ptr & scope);
            virtual ~Color() throw ();

            //
            // color_node implementation
            //
            virtual const std::vector<OpenVRML::color> & color() const
                throw ();

        private:
            //
            // field mutators
            //
            void setColor(const field_value & mfcolor)
                    throw (std::bad_cast, std::bad_alloc);

            //
            // eventIn handlers
            //
            void processSet_color(const field_value & mfcolor, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ColorInterpolatorClass : public node_class {
        public:
            explicit ColorInterpolatorClass(OpenVRML::browser & browser);
            virtual ~ColorInterpolatorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ColorInterpolator : public AbstractChild {
            friend class ColorInterpolatorClass;

            mffloat key;
            mfcolor keyValue;
            sfcolor value;

        public:
            ColorInterpolator(const node_type & type,
                              const scope_ptr & scope);
            virtual ~ColorInterpolator() throw ();

        private:
            //
            // field mutators
            //
            void setKey(const field_value & mffloat)
                    throw (std::bad_cast, std::bad_alloc);
            void setKeyValue(const field_value & mfcolor)
                    throw (std::bad_cast, std::bad_alloc);

            //
            // eventIn handlers
            //
            void processSet_fraction(const field_value & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const field_value & mffloat, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const field_value & mfcolor,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ConeClass : public node_class {
        public:
            explicit ConeClass(OpenVRML::browser & browser);
            virtual ~ConeClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Cone : public AbstractGeometry {
            friend class ConeClass;

            sfbool bottom;
            sffloat bottomRadius;
            sffloat height;
            sfbool side;

        public:
            Cone(const node_type & type,
                 const scope_ptr & scope);
            virtual ~Cone() throw ();

            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);
        };


        class OPENVRML_SCOPE CoordinateClass : public node_class {
        public:
            explicit CoordinateClass(OpenVRML::browser & browser);
            virtual ~CoordinateClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Coordinate : public AbstractBase,
                                          public coordinate_node {
            friend class CoordinateClass;

            mfvec3f point_;

        public:
            Coordinate(const node_type & type,
                       const scope_ptr & scope);
            virtual ~Coordinate() throw ();

            //
            // coordinate_node implementation
            //
            virtual const std::vector<vec3f> & point() const throw ();

        private:
            //
            // field mutators
            //
            void setPoint(const field_value & mfvec3f)
                throw (std::bad_cast, std::bad_alloc);

            //
            // eventIn handlers
            //
            void processSet_point(const field_value & mfvec3f, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE CoordinateInterpolatorClass : public node_class {
        public:
            explicit CoordinateInterpolatorClass(OpenVRML::browser & browser);
            virtual ~CoordinateInterpolatorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE CoordinateInterpolator : public AbstractChild {
            friend class CoordinateInterpolatorClass;

            mffloat key;
            mfvec3f keyValue;
            mfvec3f value;

        public:
            CoordinateInterpolator(const node_type & type,
                                   const scope_ptr & scope);
            virtual ~CoordinateInterpolator() throw ();

        private:
            //
            // field mutators
            //
            void setKey(const field_value & mffloat)
                    throw (std::bad_cast, std::bad_alloc);
            void setKeyValue(const field_value & mfvec3f)
                    throw (std::bad_cast, std::bad_alloc);

            //
            // eventIn handlers
            //
            void processSet_fraction(const field_value & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const field_value & mffloat,
                                double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const field_value & mfvec3f,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE CylinderClass : public node_class {
        public:
            explicit CylinderClass(OpenVRML::browser & browser);
            virtual ~CylinderClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Cylinder : public AbstractGeometry {
            friend class CylinderClass;

            sfbool bottom;
            sffloat height;
            sffloat radius;
            sfbool side;
            sfbool top;

        public:
            Cylinder(const node_type & type,
                     const scope_ptr & scope);
            virtual ~Cylinder() throw ();

            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);
        };


        class OPENVRML_SCOPE CylinderSensorClass : public node_class {
        public:
            explicit CylinderSensorClass(OpenVRML::browser & browser);
            virtual ~CylinderSensorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE CylinderSensor : public AbstractChild {
            friend class CylinderSensorClass;

            sfbool autoOffset;
            sffloat diskAngle;
            sfbool enabled;
            sffloat maxAngle;
            sffloat minAngle;
            sffloat offset;
            sfbool active;
            sfrotation rotation;
            sfvec3f trackPoint;

            sffloat rotation_val;
            sfvec3f activationPoint;
            sfbool disk;
            mat4f activationMatrix;
            mat4f modelview;

        public:
            CylinderSensor(const node_type & type,
                           const scope_ptr & scope);
            virtual ~CylinderSensor() throw ();

            virtual CylinderSensor * to_cylinder_sensor() const;

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);
            void activate( double timeStamp, bool isActive, double *p );

            bool isEnabled() { return this->enabled.value; }

        private:
            //
            // eventIn handlers
            //
            void processSet_autoOffset(const field_value & sfbool,
                                       double timestamp) throw (std::bad_cast);
            void processSet_diskAngle(const field_value & sffloat,
                                      double timestamp) throw (std::bad_cast);
            void processSet_enabled(const field_value & sfbool,
                                    double timestamp) throw (std::bad_cast);
            void processSet_maxAngle(const field_value & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_minAngle(const field_value & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_offset(const field_value & sffloat,
                                   double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE DirectionalLightClass : public node_class {
        public:
            explicit DirectionalLightClass(OpenVRML::browser & browser);
            virtual ~DirectionalLightClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE DirectionalLight : public AbstractLight {
            friend class DirectionalLightClass;

            sfvec3f direction;

        public:
            DirectionalLight(const node_type & type,
                             const scope_ptr & scope);
            virtual ~DirectionalLight() throw ();

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void processSet_direction(const field_value & sfvec3f,
                                      double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE ElevationGridClass : public node_class {
        public:
            explicit ElevationGridClass(OpenVRML::browser & browser);
            virtual ~ElevationGridClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ElevationGrid : public AbstractGeometry {
            friend class ElevationGridClass;

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
            ElevationGrid(const node_type & type,
                          const scope_ptr & scope);
            virtual ~ElevationGrid() throw ();

            virtual bool modified() const;
            virtual void update_modified(node_path & path, int flags = 0x003);
            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void processSet_color(const field_value & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_height(const field_value & mffloat, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_normal(const field_value & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_texCoord(const field_value & sfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ExtrusionClass : public node_class {
        public:
            explicit ExtrusionClass(OpenVRML::browser & browser);
            virtual ~ExtrusionClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Extrusion : public AbstractGeometry {
            friend class ExtrusionClass;

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
            Extrusion(const node_type & type,
                      const scope_ptr & scope);
            virtual ~Extrusion() throw ();

            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void processSet_crossSection(const field_value & mfvec2f,
                                         double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_orientation(const field_value & mfrotation,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_scale(const field_value & mfvec2f,
                                  double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_spine(const field_value & mfvec3f,
                                  double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class Fog;

        class OPENVRML_SCOPE FogClass : public node_class {
            typedef std::vector<Fog *> BoundNodes;

            Fog * first;
            BoundNodes boundNodes;

        public:
            explicit FogClass(OpenVRML::browser & browser);
            virtual ~FogClass() throw ();

            void setFirst(Fog & fog) throw ();
            bool hasFirst() const throw ();
            void bind(Fog & fog, double timestamp) throw (std::bad_alloc);
            void unbind(Fog & fog, double timestamp) throw ();

            virtual void initialize(viewpoint_node * initialViewpoint,
                                    double timestamp) throw ();
            virtual void render(OpenVRML::viewer & viewer) throw ();
            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Fog : public AbstractChild {
            friend class FogClass;

            sfcolor color;
            sfstring fogType;
            sffloat visibilityRange;
            sfbool bound;

        public:
            Fog(const node_type & type,
                const scope_ptr & scope);
            virtual ~Fog() throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_bind(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_color(const field_value & sfcolor, double timestamp)
                    throw (std::bad_cast);
            void processSet_fogType(const field_value & sfstring,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_visibilityRange(const field_value & sffloat,
                                            double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE FontStyleClass : public node_class {
        public:
            explicit FontStyleClass(OpenVRML::browser & browser);
            virtual ~FontStyleClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE FontStyle : public AbstractBase,
                                         public font_style_node {
            friend class FontStyleClass;

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
            FontStyle(const node_type & type,
                      const scope_ptr & scope);
            virtual ~FontStyle() throw ();

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


        class OPENVRML_SCOPE ImageTextureClass : public node_class {
        public:
            explicit ImageTextureClass(OpenVRML::browser & browser);
            virtual ~ImageTextureClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ImageTexture : public AbstractTexture {
            friend class ImageTextureClass;

            mfstring url;

            img * image;

            viewer::texture_object_t texObject;

        public:
            ImageTexture(const node_type & type,
                         const scope_ptr & scope);
            virtual ~ImageTexture() throw ();

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

            virtual size_t nComponents() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t nFrames() const throw ();
            virtual const unsigned char * pixels() const throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_url(const field_value & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE IndexedFaceSetClass : public node_class {
        public:
            explicit IndexedFaceSetClass(OpenVRML::browser & browser);
            virtual ~IndexedFaceSetClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE IndexedFaceSet : public AbstractIndexedSet {
            friend class IndexedFaceSetClass;

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
            IndexedFaceSet(const node_type & type,
                           const scope_ptr & scope);
            virtual ~IndexedFaceSet() throw ();

            virtual bool modified() const;
            virtual void update_modified(node_path & path, int flags = 0x003);
            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);
            virtual const OpenVRML::bounding_volume & bounding_volume() const;

        private:
            //
            // eventIn handlers
            //
            void processSet_normal(const field_value & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_normalIndex(const field_value & mfint32,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_texCoord(const field_value & sfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_texCoordIndex(const field_value & mfint32,
                                          double timestamp)
                     throw (std::bad_cast, std::bad_alloc);

            void recalcBSphere();
        };


        class OPENVRML_SCOPE IndexedLineSetClass : public node_class {
        public:
            explicit IndexedLineSetClass(OpenVRML::browser & browser);
            virtual ~IndexedLineSetClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE IndexedLineSet : public AbstractIndexedSet {
            friend class IndexedLineSetClass;

        public:
            IndexedLineSet(const node_type & type,
                           const scope_ptr & scope);
            virtual ~IndexedLineSet() throw ();

            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);
        };


        class OPENVRML_SCOPE InlineClass : public node_class {
        public:
            explicit InlineClass(OpenVRML::browser & browser);
            virtual ~InlineClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Inline : public AbstractBase,
                                      public grouping_node {
            friend class InlineClass;

            sfvec3f bboxCenter;
            sfvec3f bboxSize;
            mfstring url;

            OpenVRML::scene * inlineScene;
            bool hasLoaded;

        public:
            Inline(const node_type & type,
                   const scope_ptr & scope);
            virtual ~Inline() throw ();

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);
            virtual Inline * to_inline() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            void load();

            //
            // eventIn handlers
            //
            void processSet_url(const field_value & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE LODClass : public node_class {
        public:
            explicit LODClass(OpenVRML::browser & browser);
            virtual ~LODClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE LOD : public AbstractBase,
                                   public grouping_node {
            friend class LODClass;

            mfnode level;
            sfvec3f center;
            mffloat range;

            mfnode children_;
            bounding_sphere bsphere;

        public:
            LOD(const node_type & type,
                const scope_ptr & scope);
            virtual ~LOD() throw ();

            virtual bool modified() const;
            virtual void update_modified(node_path & path, int flags = 0x003);
            virtual void render(OpenVRML::viewer & viewer, rendering_context context);
            virtual const OpenVRML::bounding_volume & bounding_volume() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            //
            // eventIn handlers
            //
            void processSet_level(const field_value & mfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);

            virtual void recalcBSphere();
        };


        class OPENVRML_SCOPE MaterialClass : public node_class {
        public:
            explicit MaterialClass(OpenVRML::browser & browser);
            virtual ~MaterialClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Material : public AbstractBase,
                                        public material_node {
            friend class MaterialClass;

            sffloat ambientIntensity;
            sfcolor diffuseColor;
            sfcolor emissiveColor;
            sffloat shininess_;
            sfcolor specularColor;
            sffloat transparency_;

        public:
            Material(const node_type & type,
                     const scope_ptr & scope);
            virtual ~Material() throw ();

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
            void processSet_ambientIntensity(const field_value & sffloat,
                                             double timestamp)
                    throw (std::bad_cast);
            void processSet_diffuseColor(const field_value & sfcolor,
                                         double timestamp)
                    throw (std::bad_cast);
            void processSet_emissiveColor(const field_value & sfcolor,
                                          double timestamp)
                    throw (std::bad_cast);
            void processSet_shininess(const field_value & sffloat,
                                      double timestamp)
                    throw (std::bad_cast);
            void processSet_specularColor(const field_value & sfcolor,
                                          double timestamp)
                    throw (std::bad_cast);
            void processSet_transparency(const field_value & sffloat,
                                         double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE MovieTextureClass : public node_class {
        public:
            explicit MovieTextureClass(OpenVRML::browser & browser);
            virtual ~MovieTextureClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE MovieTexture : public AbstractTexture {
            friend class MovieTextureClass;

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
            MovieTexture(const node_type & type,
                         const scope_ptr & scope);
            virtual ~MovieTexture() throw ();

            virtual MovieTexture * to_movie_texture() const;

            void update(double time);

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

            virtual size_t nComponents() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t nFrames() const throw ();
            virtual const unsigned char * pixels() const throw ();

        private:
            virtual void do_initialize(double timestamp) throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_loop(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_speed(const field_value & sffloat, double timestamp)
                    throw (std::bad_cast);
            void processSet_startTime(const field_value & sftime,
                                      double timestamp) throw (std::bad_cast);
            void processSet_stopTime(const field_value & sftime,
                                     double timestamp) throw (std::bad_cast);
            void processSet_url(const field_value & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE NavigationInfoClass : public node_class {
        public:
            explicit NavigationInfoClass(OpenVRML::browser & browser);
            virtual ~NavigationInfoClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE NavigationInfo : public AbstractChild {
            friend class NavigationInfoClass;

            mffloat avatarSize;
            sfbool headlight;
            sffloat speed;
            mfstring type;
            sffloat visibilityLimit;
            sfbool bound;

        public:
            NavigationInfo(const node_type & type,
                           const scope_ptr & scope);
            virtual ~NavigationInfo() throw ();

            virtual NavigationInfo * to_navigation_info() const;

            const float * getAvatarSize()
            {
                return !this->avatarSize.value.empty()
                        ? &this->avatarSize.value[0]
                        : 0;
            }

            bool getHeadlightOn()
            {
                return this->headlight.value;
            }

            float getSpeed()
            {
                return this->speed.value;
            }

            float getVisibilityLimit()
            {
                return this->visibilityLimit.value;
            }

        private:
            virtual void do_initialize(double timestamp) throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_avatarSize(const field_value & mffloat,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_bind(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_headlight(const field_value & sfbool,
                                      double timestamp) throw (std::bad_cast);
            void processSet_speed(const field_value & sffloat, double timestamp)
                    throw (std::bad_cast);
            void processSet_type(const field_value & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_visibilityLimit(const field_value & sffloat,
                                            double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE NormalClass : public node_class {
        public:
            explicit NormalClass(OpenVRML::browser & browser);
            virtual ~NormalClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Normal : public AbstractBase, public normal_node {
            friend class NormalClass;

            mfvec3f vector_;

        public:
            Normal(const node_type & type,
                   const scope_ptr & scope);
            virtual ~Normal() throw ();

            //
            // normal_node implementation.
            //
            virtual const std::vector<vec3f> & vector() const throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_vector(const field_value & mfvec3f, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE NormalInterpolatorClass : public node_class {
        public:
            explicit NormalInterpolatorClass(OpenVRML::browser & browser);
            virtual ~NormalInterpolatorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE NormalInterpolator : public AbstractChild {
            friend class NormalInterpolatorClass;

            mffloat key;
            mfvec3f keyValue;
            mfvec3f value;

        public:
            NormalInterpolator(const node_type & type,
                               const scope_ptr & scope);
            virtual ~NormalInterpolator() throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_fraction(const field_value & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const field_value & mffloat, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const field_value & mfvec3f,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE OrientationInterpolatorClass : public node_class {
        public:
            explicit OrientationInterpolatorClass(OpenVRML::browser & browser);
            virtual ~OrientationInterpolatorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE OrientationInterpolator : public AbstractChild {
            friend class OrientationInterpolatorClass;

            mffloat key;
            mfrotation keyValue;
            sfrotation value;

        public:
            OrientationInterpolator(const node_type & type,
                                    const scope_ptr & scope);
            virtual ~OrientationInterpolator() throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_fraction(const field_value & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const field_value & mffloat, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const field_value & mfrotation,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE PixelTextureClass : public node_class {
        public:
            explicit PixelTextureClass(OpenVRML::browser & browser);
            virtual ~PixelTextureClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PixelTexture : public AbstractTexture {
            friend class PixelTextureClass;

            sfimage image;

            viewer::texture_object_t texObject;

        public:
            PixelTexture(const node_type & type,
                         const scope_ptr & scope);
            virtual ~PixelTexture() throw ();

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

            virtual size_t nComponents() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t nFrames() const throw ();
            virtual const unsigned char * pixels() const throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_image(const field_value & sfimage, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE PlaneSensorClass : public node_class {
        public:
            explicit PlaneSensorClass(OpenVRML::browser & browser);
            virtual ~PlaneSensorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PlaneSensor : public AbstractChild {
            friend class PlaneSensorClass;

            sfbool autoOffset;
            sfbool enabled;
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
            PlaneSensor(const node_type & type,
                        const scope_ptr & scope);
            virtual ~PlaneSensor() throw ();

            virtual PlaneSensor * to_plane_sensor() const;

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);
            void activate( double timeStamp, bool isActive, double *p );

            bool isEnabled() { return this->enabled.value; }

        private:
            //
            // eventIn handlers
            //
            void processSet_autoOffset(const field_value & sfbool,
                                       double timestamp) throw (std::bad_cast);
            void processSet_enabled(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_maxPosition(const field_value & sfvec2f,
                                        double timestamp) throw (std::bad_cast);
            void processSet_minPosition(const field_value & sfvec2f,
                                        double timestamp) throw (std::bad_cast);
            void processSet_offset(const field_value & sfvec3f, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE PointLightClass : public node_class {
        public:
            explicit PointLightClass(OpenVRML::browser & browser);
            virtual ~PointLightClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PointLight : public AbstractLight {
            friend class PointLightClass;

            sfvec3f attenuation;
            sfvec3f location;
            sffloat radius;

        public:
            PointLight(const node_type & type,
                       const scope_ptr & scope);
            virtual ~PointLight() throw ();

            virtual PointLight * to_point_light() const;

            virtual void renderScoped(OpenVRML::viewer & viewer);

        private:
            virtual void do_initialize(double timestamp) throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_attenuation(const field_value & sfvec3f,
                                        double timestamp) throw (std::bad_cast);
            void processSet_location(const field_value & sfvec3f,
                                     double timestamp) throw (std::bad_cast);
            void processSet_radius(const field_value & sffloat, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE PointSetClass : public node_class {
        public:
            explicit PointSetClass(OpenVRML::browser & browser);
            virtual ~PointSetClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PointSet : public AbstractGeometry {
            friend class PointSetClass;

            sfnode color;
            sfnode coord;

            bounding_sphere bsphere;

        public:
            PointSet(const node_type & type,
                     const scope_ptr & scope);
            virtual ~PointSet() throw ();

            virtual void update_modified(node_path & path, int flags = 0x003);
            virtual bool modified() const;
            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);
            virtual const OpenVRML::bounding_volume & bounding_volume() const;

        private:
            //
            // eventIn handlers
            //
            void processSet_color(const field_value & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_coord(const field_value & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);

            void recalcBSphere();
        };


        class OPENVRML_SCOPE PositionInterpolatorClass : public node_class {
        public:
            explicit PositionInterpolatorClass(OpenVRML::browser & browser);
            virtual ~PositionInterpolatorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PositionInterpolator : public AbstractChild {
            friend class PositionInterpolatorClass;

            mffloat key;
            mfvec3f keyValue;
            sfvec3f value;

        public:
            PositionInterpolator(const node_type & type,
                                 const scope_ptr & scope);
            virtual ~PositionInterpolator() throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_fraction(const field_value & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const field_value & mffloat,
                                double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const field_value & mfvec3f,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ProximitySensorClass : public node_class {
        public:
            explicit ProximitySensorClass(OpenVRML::browser & browser);
            virtual ~ProximitySensorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ProximitySensor : public AbstractChild {
            friend class ProximitySensorClass;

            sfvec3f center;
            sfbool enabled;
            sfvec3f size;
            sfbool active;
            sfvec3f position;
            sfrotation orientation;
            sftime enterTime;
            sftime exitTime;

        public:
            ProximitySensor(const node_type & type,
                            const scope_ptr & scope);
            virtual ~ProximitySensor() throw ();

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void processSet_center(const field_value & sfvec3f, double timestamp)
                    throw (std::bad_cast);
            void processSet_size(const field_value & sfvec3f, double timestamp)
                    throw (std::bad_cast);
            void processSet_enabled(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE ScalarInterpolatorClass : public node_class {
        public:
            explicit ScalarInterpolatorClass(OpenVRML::browser & browser);
            virtual ~ScalarInterpolatorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ScalarInterpolator : public AbstractChild {
            friend class ScalarInterpolatorClass;

            mffloat key;
            mffloat keyValue;
            sffloat value;

        public:
            ScalarInterpolator(const node_type & type,
                               const scope_ptr & scope);
            virtual ~ScalarInterpolator() throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_fraction(const field_value & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const field_value & mffloat,
                                double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const field_value & mffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ShapeClass : public node_class {
        public:
            explicit ShapeClass(OpenVRML::browser & browser);
            virtual ~ShapeClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Shape : public AbstractChild {
            friend class ShapeClass;

            sfnode appearance;
            sfnode geometry;

            viewer::object_t viewerObject; // move to Node.h ? ...

        public:
            Shape(const node_type & type,
                  const scope_ptr & scope);
            virtual ~Shape() throw ();

            virtual bool modified() const;
            virtual void update_modified(node_path& path, int flags);
            virtual const OpenVRML::bounding_volume & bounding_volume() const;
            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void processSet_appearance(const field_value & sfnode,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_geometry(const field_value & sfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE SoundClass : public node_class {
        public:
            explicit SoundClass(OpenVRML::browser & browser);
            virtual ~SoundClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Sound : public AbstractChild {
            friend class SoundClass;

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
            Sound(const node_type & type,
                  const scope_ptr & scope);
            virtual ~Sound() throw ();

            virtual void update_modified(node_path & path, int flags = 0x003);
            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void processSet_direction(const field_value & sfvec3f,
                                      double timestamp) throw (std::bad_cast);
            void processSet_intensity(const field_value & sffloat,
                                      double timestamp) throw (std::bad_cast);
            void processSet_location(const field_value & sfvec3f,
                                     double timestamp) throw (std::bad_cast);
            void processSet_maxBack(const field_value & sffloat,
                                    double timestamp) throw (std::bad_cast);
            void processSet_maxFront(const field_value & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_minBack(const field_value & sffloat,
                                    double timestamp) throw (std::bad_cast);
            void processSet_minFront(const field_value & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_priority(const field_value & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_source(const field_value & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE SphereClass : public node_class {
        public:
            explicit SphereClass(OpenVRML::browser & browser);
            virtual ~SphereClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Sphere : public AbstractGeometry {
            friend class SphereClass;

            sffloat radius;
            bounding_sphere bsphere;

        public:
            Sphere(const node_type & type,
                   const scope_ptr & scope);
            virtual ~Sphere() throw ();

            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                  rendering_context context);
            virtual const OpenVRML::bounding_volume & bounding_volume() const;
        };


        class OPENVRML_SCOPE SphereSensorClass : public node_class {
        public:
            explicit SphereSensorClass(OpenVRML::browser & browser);
            virtual ~SphereSensorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE SphereSensor : public AbstractChild {
            friend class SphereSensorClass;

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
            SphereSensor(const node_type & type,
                         const scope_ptr & scope);
            virtual ~SphereSensor() throw ();

            virtual SphereSensor * to_sphere_sensor() const;

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

            void activate(double timeStamp, bool isActive, double * p);
            bool isEnabled() const throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_autoOffset(const field_value & sfbool,
                                       double timestamp)
                throw (std::bad_cast);
            void processSet_enabled(const field_value & sfbool,
                                    double timestamp)
                throw (std::bad_cast);
            void processSet_offset(const field_value & sfrotation,
                                   double timestamp)
                throw (std::bad_cast);
        };


        class OPENVRML_SCOPE SpotLightClass : public node_class {
        public:
            explicit SpotLightClass(OpenVRML::browser & browser);
            virtual ~SpotLightClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE SpotLight : public AbstractLight {
            friend class SpotLightClass;

            sfvec3f attenuation;
            sffloat beamWidth;
            sffloat cutOffAngle;
            sfvec3f direction;
            sfvec3f location;
            sffloat radius;

        public:
            SpotLight(const node_type & type,
                      const scope_ptr & scope);
            virtual ~SpotLight() throw ();

            virtual SpotLight * to_spot_light() const;

            virtual void renderScoped(OpenVRML::viewer & viewer);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_attenuation(const field_value & sfvec3f,
                                        double timestamp)
                throw (std::bad_cast);
            void processSet_beamWidth(const field_value & sffloat,
                                      double timestamp) throw (std::bad_cast);
            void processSet_cutOffAngle(const field_value & sffloat,
                                        double timestamp)
                throw (std::bad_cast);
            void processSet_direction(const field_value & sfvec3f,
                                      double timestamp) throw (std::bad_cast);
            void processSet_location(const field_value & sfvec3f,
                                     double timestamp) throw (std::bad_cast);
            void processSet_radius(const field_value & sffloat,
                                   double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE SwitchClass : public node_class {
        public:
            explicit SwitchClass(OpenVRML::browser & browser);
            virtual ~SwitchClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Switch : public AbstractBase,
                                      public grouping_node {
            friend class SwitchClass;

            mfnode choice;
            sfint32 whichChoice;

            mfnode children_;
            bounding_sphere bsphere;

        public:
            Switch(const node_type & type,
                   const scope_ptr & scope);
            virtual ~Switch() throw ();

            virtual bool modified() const;
            virtual void update_modified(node_path & path, int flags = 0x003);
            virtual void render(OpenVRML::viewer & viewer, rendering_context context);
            virtual const OpenVRML::bounding_volume & bounding_volume() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            //
            // eventIn handlers
            //
            void processSet_choice(const field_value & mfnode,
                                   double timestamp)
                throw (std::bad_cast, std::bad_alloc);
            void processSet_whichChoice(const field_value & sfint32,
                                        double timestamp)
                throw (std::bad_cast);

            virtual void recalcBSphere();
        };


        class OPENVRML_SCOPE TextClass : public node_class {
        public:
            FT_Library freeTypeLibrary;

            explicit TextClass(OpenVRML::browser & browser);
            virtual ~TextClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Text : public AbstractGeometry {
            friend class TextClass;

            mfstring string;
            sfnode fontStyle;
            mffloat length;
            sffloat maxExtent;

            struct GlyphGeometry {
                std::vector<vec2f> coord;
                std::vector<int32> coordIndex;
                float advanceWidth;
                float advanceHeight;

                GlyphGeometry(
                    const std::vector<std::vector<vec2f> > & contours,
                    float advanceWidth, float advanceHeight)
                    throw (std::bad_alloc);
            };

            struct TextGeometry {
                std::vector<vec3f> coord;
                std::vector<int32> coordIndex;
                std::vector<vec3f> normal;
                std::vector<vec2f> texCoord;
            };

            typedef std::vector<std::vector<FcChar32> > Ucs4String;
            typedef std::map<FT_UInt, GlyphGeometry> GlyphGeometryMap;

            Ucs4String ucs4String;
            FT_Face face;
            GlyphGeometryMap glyphGeometryMap;
            TextGeometry textGeometry;

        public:
            Text(const node_type & type,
                 const scope_ptr & scope);
            virtual ~Text() throw ();

            virtual bool modified() const;
            virtual void update_modified(node_path & path, int flags = 0x003);
            virtual viewer::object_t insert_geometry(OpenVRML::viewer & viewer,
                                                     rendering_context context);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_string(const field_value & mfstring,
                                   double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_fontStyle(const field_value & sfnode,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_length(const field_value & mffloat,
                                   double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_maxExtent(const field_value & sffloat,
                                      double timestamp) throw (std::bad_cast);

            void updateUcs4() throw (std::bad_alloc);
            void updateFace() throw (std::bad_alloc);
            void updateGeometry() throw (std::bad_alloc);
        };


        class OPENVRML_SCOPE TextureCoordinateClass : public node_class {
        public:
            explicit TextureCoordinateClass(OpenVRML::browser & browser);
            virtual ~TextureCoordinateClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE TextureCoordinate :
                public AbstractBase, public texture_coordinate_node {
            friend class TextureCoordinateClass;

            mfvec2f point_;

        public:
            TextureCoordinate(const node_type & type,
                              const scope_ptr & scope);
            virtual ~TextureCoordinate() throw ();

            //
            // texture_coordinate_node implementation.
            //
            virtual const std::vector<vec2f> & point() const throw();

        private:
            //
            // eventIn handlers
            //
            void processSet_point(const field_value & mfvec2f,
                                  double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE TextureTransformClass : public node_class {
        public:
            explicit TextureTransformClass(OpenVRML::browser & browser);
            virtual ~TextureTransformClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE TextureTransform : public AbstractBase,
                                                public texture_transform_node {
            friend class TextureTransformClass;

            sfvec2f center;
            sffloat rotation;
            sfvec2f scale;
            sfvec2f translation;

        public:
            TextureTransform(const node_type & type,
                             const scope_ptr & scope);
            virtual ~TextureTransform() throw ();

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void processSet_center(const field_value & sfvec2f,
                                   double timestamp)
                throw (std::bad_cast);
            void processSet_rotation(const field_value & sffloat,
                                     double timestamp)
                throw (std::bad_cast);
            void processSet_scale(const field_value & sfvec2f,
                                  double timestamp)
                throw (std::bad_cast);
            void processSet_translation(const field_value & sfvec2f,
                                        double timestamp)
                throw (std::bad_cast);
        };


        class OPENVRML_SCOPE TimeSensorClass : public node_class {
        public:
            explicit TimeSensorClass(OpenVRML::browser & browser);
            virtual ~TimeSensorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE TimeSensor : public AbstractChild {
            friend class TimeSensorClass;

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
            TimeSensor(const node_type & type,
                       const scope_ptr & scope);
            virtual ~TimeSensor() throw ();

            virtual TimeSensor * to_time_sensor() const;

            void update(double time);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            //
            // eventIn handlers
            //
            void processSet_cycleInterval(const field_value & sftime,
                                          double timestamp)
                throw (std::bad_cast);
            void processSet_enabled(const field_value & sfbool,
                                    double timestamp)
                throw (std::bad_cast);
            void processSet_loop(const field_value & sfbool, double timestamp)
                throw (std::bad_cast);
            void processSet_startTime(const field_value & sftime,
                                      double timestamp) throw (std::bad_cast);
            void processSet_stopTime(const field_value & sftime,
                                     double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE TouchSensorClass : public node_class {
        public:
            explicit TouchSensorClass(OpenVRML::browser & browser);
            virtual ~TouchSensorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE TouchSensor : public AbstractChild {
            friend class TouchSensorClass;

            sfbool enabled;
            sfvec3f hitNormal;
            sfvec3f hitPoint;
            sfvec2f hitTexCoord;
            sfbool active;
            sfbool over;
            sftime touchTime;

        public:
            TouchSensor(const node_type & type,
                        const scope_ptr & scope);
            virtual ~TouchSensor() throw ();

            virtual TouchSensor * to_touch_sensor() const;

            void activate( double timeStamp, bool isOver, bool isActive, double *p );

            bool isEnabled() const;

        private:
            //
            // eventIn handlers
            //
            void processSet_enabled(const field_value & sfbool,
                                    double timestamp)
                throw (std::bad_cast);
        };


        class OPENVRML_SCOPE TransformClass : public node_class {
        public:
            explicit TransformClass(OpenVRML::browser & browser);
            virtual ~TransformClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Transform : public Group,
                                         public transform_node {
            friend class TransformClass;

            sfvec3f center;
            sfrotation rotation;
            sfvec3f scale;
            sfrotation scaleOrientation;
            sfvec3f translation;

            mutable mat4f transform_;
            mutable bool transformDirty;
            viewer::object_t xformObject;

        public:
            Transform(const node_type & type,
                      const scope_ptr & scope);
            virtual ~Transform() throw ();

            virtual void render(OpenVRML::viewer & viewer, rendering_context context);

            virtual const OpenVRML::bounding_volume & bounding_volume() const;

            virtual const mat4f & transform() const throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_center(const field_value & sfvec3f,
                                   double timestamp)
                throw (std::bad_cast);
            void processSet_rotation(const field_value & sfrotation,
                                     double timestamp)
                throw (std::bad_cast);
            void processSet_scale(const field_value & sfvec3f,
                                  double timestamp)
                throw (std::bad_cast);
            void processSet_scaleOrientation(const field_value & sfrotation,
                                             double timestamp)
                throw (std::bad_cast);
            void processSet_translation(const field_value & sfvec3f,
                                        double timestamp)
                throw (std::bad_cast);

            void recalcBSphere();
            void updateTransform() const throw ();
        };


        class Viewpoint;

        class OPENVRML_SCOPE ViewpointClass : public node_class {
            typedef std::vector<Viewpoint *> BoundNodes;

            Viewpoint * first;
            BoundNodes boundNodes;

        public:
            explicit ViewpointClass(OpenVRML::browser & browser);
            virtual ~ViewpointClass() throw ();

            void setFirst(Viewpoint & viewpoint) throw ();
            bool hasFirst() const throw ();
            void bind(Viewpoint & viewpoint, double timestamp)
                throw (std::bad_alloc);
            void unbind(Viewpoint & viewpoint, double timestamp) throw ();

            virtual void initialize(viewpoint_node * initialViewpoint,
                                    double timestamp) throw ();
            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Viewpoint : public AbstractBase,
                                         public viewpoint_node {
            friend class ViewpointClass;

            sffloat fieldOfView;
            sfbool jump;
            sfrotation orientation_;
            sfvec3f position_;
            sfstring description_;
            sfbool bound;
            sftime bindTime;

            mat4f parentTransform;
            mutable bool finalTransformationDirty;
            mutable mat4f finalTransformation;
            mat4f userViewTransform;

        public:
            Viewpoint(const node_type & type,
                      const scope_ptr & scope);
            virtual ~Viewpoint() throw ();

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
            void processSet_bind(const field_value & sfbool, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_fieldOfView(const field_value & sffloat,
                                        double timestamp)
                throw (std::bad_cast);
            void processSet_jump(const field_value & sfbool, double timestamp)
                throw (std::bad_cast);
            void processSet_orientation(const field_value & sfrotation,
                                        double timestamp)
                throw (std::bad_cast);
            void processSet_position(const field_value & sfvec3f,
                                     double timestamp)
                throw (std::bad_cast);

            void updateFinalTransformation() const throw ();
        };


        class OPENVRML_SCOPE VisibilitySensorClass : public node_class {
        public:
            explicit VisibilitySensorClass(OpenVRML::browser & browser);
            virtual ~VisibilitySensorClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                 const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE VisibilitySensor : public AbstractChild {
            friend class VisibilitySensorClass;

            sfvec3f center;
            sfbool enabled;
            sfvec3f size;
            sfbool active;
            sftime enterTime;
            sftime exitTime;

        public:
            VisibilitySensor(const node_type & type,
                             const scope_ptr & scope);
            virtual ~VisibilitySensor() throw ();

            virtual void render(OpenVRML::viewer & viewer,
                                rendering_context context);

        private:
            //
            // eventIn handlers
            //
            void processSet_center(const field_value & sfvec3f,
                                   double timestamp)
                    throw (std::bad_cast);
            void processSet_enabled(const field_value & sfbool,
                                    double timestamp)
                    throw (std::bad_cast);
            void processSet_size(const field_value & sfvec3f, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE WorldInfoClass : public node_class {
        public:
            explicit WorldInfoClass(OpenVRML::browser & browser);
            virtual ~WorldInfoClass() throw ();

            virtual const node_type_ptr create_type(const std::string & id,
                                                    const node_interface_set &)
                    throw (unsupported_interface, std::bad_alloc);
        };

        class OPENVRML_SCOPE WorldInfo : public AbstractChild {
            friend class WorldInfoClass;

            mfstring info;
            sfstring title;

        public:
            WorldInfo(const node_type & type,
                      const scope_ptr & scope);
            virtual ~WorldInfo() throw ();
        };
    }
}

# endif
