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

#   include "common.h"
#   include "field.h"
#   include "nodetypeptr.h"
#   include "node.h"
#   include "Viewer.h"
#   include "Image.h"
#   include "bvolume.h"
#   include "VrmlMatrix.h"

namespace OpenVRML {

    class Audio;

    namespace Vrml97Node {

        class OPENVRML_SCOPE AbstractBase : public virtual Node {
        public:
            virtual ~AbstractBase() throw () = 0;

        protected:
            AbstractBase(const NodeType & type);
        
        private:
            virtual void setFieldImpl(const std::string & id,
                                      const FieldValue & value)
                    throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);

            virtual const FieldValue &
                    getFieldImpl(const std::string & id) const
                    throw (UnsupportedInterface);

            virtual void processEventImpl(const std::string & id,
                                          const FieldValue & value,
                                          double timestamp)
                    throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);

            virtual const FieldValue &
                    getEventOutImpl(const std::string & id) const
                    throw (UnsupportedInterface);
        };


        class OPENVRML_SCOPE AbstractChild : public AbstractBase,
                                             public ChildNode {
        public:
            virtual ~AbstractChild() throw () = 0;
        
        protected:
            explicit AbstractChild(const NodeType & type);
        };


        class OPENVRML_SCOPE AbstractGeometry :
                public AbstractBase, public GeometryNode {
        public:
            virtual ~AbstractGeometry() throw () = 0;

            virtual void render(Viewer *, VrmlRenderContext rc);
            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc) = 0;

        protected:
            explicit AbstractGeometry(const NodeType & type);

            Viewer::Object d_viewerObject; // move to Node? ...
        };


        class OPENVRML_SCOPE AbstractIndexedSet : public AbstractGeometry {
        protected:
            SFNode color;
            MFInt32 colorIndex;
            SFBool colorPerVertex;
            SFNode coord;
            MFInt32 coordIndex;

        public:
            virtual ~AbstractIndexedSet() throw () = 0;

            virtual bool isModified() const;
            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();

            virtual const ColorNode * getColor() const throw ();

        protected:
            explicit AbstractIndexedSet(const NodeType & type);

            //
            // eventIn handlers
            //
            void processSet_color(const FieldValue & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_colorIndex(const FieldValue & mfint32,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_coord(const FieldValue & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_coordIndex(const FieldValue & mfint32,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE AbstractLight : public AbstractChild {
        protected:
            SFFloat ambientIntensity;
            SFColor color;
            SFFloat intensity;
            SFBool on;

        public:
            virtual ~AbstractLight() throw () = 0;

            virtual void renderScoped(Viewer *);
            virtual AbstractLight * toLight() const;

            float getAmbientIntensity() const { return this->ambientIntensity.get(); }
            float getIntensity() const { return this->intensity.get(); }
            bool getOn() const { return this->on.get(); }
            SFColor::ConstArrayReference getColor() const { return this->color.get(); }

        protected:
            explicit AbstractLight(const NodeType & type);

            //
            // eventIn handlers
            //
            void processSet_ambientIntensity(const FieldValue & sffloat,
                                             double timestamp)
                    throw (std::bad_cast);
            void processSet_color(const FieldValue & sfcolor, double timestamp)
                    throw (std::bad_cast);
            void processSet_intensity(const FieldValue & sffloat,
                                      double timestamp) throw (std::bad_cast);
            void processSet_on(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE AbstractTexture : public AbstractBase,
                                               public TextureNode {
        protected:
            SFBool repeatS;
            SFBool repeatT;

            SFString d_relativeUrl;
        
        public:
            virtual ~AbstractTexture() throw () = 0;

            //
            // TextureNode implementation.
            //
            virtual const SFBool & getRepeatS() const throw ();
            virtual const SFBool & getRepeatT() const throw ();
            
        protected:
            explicit AbstractTexture(const NodeType & type);
        };


        class OPENVRML_SCOPE GroupClass : public NodeClass {
        public:
            explicit GroupClass(VrmlScene & scene);
            virtual ~GroupClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Group : public AbstractChild {
            friend class GroupClass;

        protected:
            SFVec3f bboxCenter;
            SFVec3f bboxSize;
            MFNode children;
            SFString relative;
            
            Node * parentTransform;
            Viewer::Object viewerObject;

            /**
             * Cached copy of the bsphere enclosing this node's children.
             */
            BSphere bsphere;
        
        public:
            explicit Group(const NodeType & type);
            virtual ~Group() throw ();
            
            virtual Group * toGroup() const;

            virtual bool isModified() const;
            virtual void updateModified(NodePath& path, int flags);
            virtual void clearFlags();

            virtual void render(Viewer *, VrmlRenderContext rc);
            virtual void renderNoCull(Viewer *, VrmlRenderContext rc);

            virtual void accumulateTransform(Node*);

            void activate( double timeStamp, bool isOver, bool isActive, double *p );

            void setChildren(const MFNode & children);
            void addChildren(const MFNode & children);
            void removeChildren(const MFNode & children);
            void removeChildren();

            virtual Node* getParentTransform();

            const BVolume * getBVolume() const;

        protected:
            //
            // eventIn handlers
            //
            void processAddChildren(const FieldValue & mfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processRemoveChildren(const FieldValue & mfnode,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_children(const FieldValue & mfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);

            /**
             * Construct a bounding sphere around this node's children. Store it
             * in d_bsphere.
             */
            virtual void recalcBSphere();
        };


        class OPENVRML_SCOPE AnchorClass : public NodeClass {
        public:
            explicit AnchorClass(VrmlScene & scene);
            virtual ~AnchorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Anchor : public Group {
            friend class AnchorClass;

            SFString description;
            MFString parameter;
            MFString url;

        public:
            explicit Anchor(const NodeType & type);
            virtual ~Anchor() throw ();
            
            virtual Anchor * toAnchor() const;

            virtual void render(Viewer *, VrmlRenderContext rc);

            void activate();

            const std::string & getDescription() { return description.get(); }
            const std::string & getUrl() { assert(url.getLength() > 0); return url.getElement(0); }

            const BVolume * getBVolume() const;
        
        private:
            //
            // field mutators
            //
            void setDescription(const FieldValue & sfstring)
                    throw (std::bad_cast, std::bad_alloc);
            void setParameter(const FieldValue & mfstring)
                    throw (std::bad_cast, std::bad_alloc);
            void setUrl(const FieldValue & mfstring)
                    throw (std::bad_cast, std::bad_alloc);
            
            //
            // eventIn handlers
            //
            void processSet_description(const FieldValue & sfstring,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_parameter(const FieldValue & mfstring,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_url(const FieldValue & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE AppearanceClass : public NodeClass {
        public:
            explicit AppearanceClass(VrmlScene & scene);
            virtual ~AppearanceClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Appearance : public AbstractBase,
                                          public AppearanceNode {
            friend class AppearanceClass;
            
            SFNode material;
            SFNode texture;
            SFNode textureTransform;

        public:
            explicit Appearance(const NodeType & type);
            virtual ~Appearance() throw ();
            
            virtual bool isModified() const;
            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();	// Clear childrens flags too.

            virtual void render(Viewer *, VrmlRenderContext rc);

            //
            // AppearanceNode implementation
            //
            virtual const SFNode & getMaterial() const throw ();
            virtual const SFNode & getTexture() const throw ();
            virtual const SFNode & getTextureTransform() const throw ();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_material(const FieldValue & sfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_texture(const FieldValue & sfnode,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_textureTransform(const FieldValue & sfnode,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE AudioClipClass : public NodeClass {
        public:
            explicit AudioClipClass(VrmlScene & scene);
            virtual ~AudioClipClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE AudioClip : public AbstractBase {
            friend class AudioClipClass;

            SFString description;
            SFBool loop;
            SFFloat pitch;
            SFTime startTime;
            SFTime stopTime;
            MFString url;
            SFTime duration;
            SFBool active;

            SFString relativeUrl;

            Audio * audio;
            bool url_modified;
            int audio_index;
            double audio_intensity;
            int audio_fd;

        public:
            explicit AudioClip(const NodeType & type);
            virtual ~AudioClip() throw ();
            
            void update(double time);

            virtual AudioClip * toAudioClip() const;
        
        private:
            //
            // eventIn handlers
            //
            void processSet_description(const FieldValue & sfstring,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_loop(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_pitch(const FieldValue & sffloat, double timestamp)
                    throw (std::bad_cast);
            void processSet_startTime(const FieldValue & sftime,
                                      double timestamp)
                    throw (std::bad_cast);
            void processSet_stopTime(const FieldValue & sftime,
                                     double timestamp)
                    throw (std::bad_cast);
            void processSet_url(const FieldValue & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE BackgroundClass : public NodeClass {
        public:
            explicit BackgroundClass(VrmlScene & scene);
            virtual ~BackgroundClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Background : public AbstractChild {
            friend class BackgroundClass;

            MFFloat groundAngle;
            MFColor groundColor;
            MFString backUrl;
            MFString bottomUrl;
            MFString frontUrl;
            MFString leftUrl;
            MFString rightUrl;
            MFString topUrl;
            MFFloat skyAngle;
            MFColor skyColor;
            SFBool bound;

            SFString relativeUrl;

            // Texture caches
            Image * texPtr[6];
            Image tex[6];

            // Display list object for background
            Viewer::Object viewerObject;

        public:
            explicit Background(const NodeType & type);
            virtual ~Background() throw ();
            
            virtual Background * toBackground() const;

            // render backgrounds once per scene, not via the render() method
            void renderBindable(Viewer *);

            size_t nGroundAngles() { return this->groundAngle.getLength(); }
            const float * getGroundAngle() { return this->groundAngle.get(); }
            const float * getGroundColor() { return this->groundColor.get(); }

            size_t nSkyAngles() { return this->skyAngle.getLength(); }
            const float * getSkyAngle() { return this->skyAngle.get(); }
            const float * getSkyColor() { return this->skyColor.get(); }
        
        private:
            //
            // eventIn handlers
            //
            void processSet_bind(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_groundAngle(const FieldValue & mffloat,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_groundColor(const FieldValue & mfcolor,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_backUrl(const FieldValue & mfstring,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_bottomUrl(const FieldValue & mfstring,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_frontUrl(const FieldValue & mfstring,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_leftUrl(const FieldValue & mfstring,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_rightUrl(const FieldValue & mfstring,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_topUrl(const FieldValue & mfstring,
                                   double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_skyAngle(const FieldValue & mffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_skyColor(const FieldValue & mfcolor,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE BillboardClass : public NodeClass {
        public:
            explicit BillboardClass(VrmlScene & scene);
            virtual ~BillboardClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Billboard : public Group {
            friend class BillboardClass;

            SFVec3f axisOfRotation;
            
            Node * parentTransform;
            Viewer::Object xformObject;

        public:
            static void billboard_to_matrix(const Billboard * t_arg,
                                            const VrmlMatrix & MV,
                                            VrmlMatrix & M);

            explicit Billboard(const NodeType & type);
            virtual ~Billboard() throw ();
            
            virtual void render(Viewer *, VrmlRenderContext rc);

            virtual void accumulateTransform(Node*);
            virtual Node* getParentTransform();
            virtual void inverseTransform(Viewer *);
            virtual void inverseTransform(VrmlMatrix &);
        
        private:
            //
            // field mutators
            //
            void setAxisOfRotation(const FieldValue & sfvec3f)
                    throw (std::bad_cast);
            
            //
            // eventIn handlers
            //
            void processSet_axisOfRotation(const FieldValue & sfvec3f,
                                           double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE BoxClass : public NodeClass {
        public:
            explicit BoxClass(VrmlScene & scene);
            virtual ~BoxClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Box : public AbstractGeometry {
            friend class BoxClass;

            SFVec3f size;
            
            BSphere bsphere;

        public:
            explicit Box(const NodeType & type);
            virtual ~Box() throw ();
            
            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
            virtual const BVolume * getBVolume() const;
        };


        class OPENVRML_SCOPE CollisionClass : public NodeClass {
        public:
            explicit CollisionClass(VrmlScene & scene);
            virtual ~CollisionClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Collision : public Group {
            friend class CollisionClass;

            SFBool collide;
            SFNode proxy;
            SFTime collideTime;  // eventOut

        public:
            explicit Collision(const NodeType & type);
            virtual ~Collision() throw ();


            virtual bool isModified() const;
            virtual void clearFlags();	// Clear childrens flags too.
        
        private:
            //
            // eventIn handlers
            //
            void processSet_collide(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE ColorClass : public NodeClass {
        public:
            explicit ColorClass(VrmlScene & scene);
            virtual ~ColorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Color : public AbstractBase, public ColorNode {
            friend class ColorClass;

            MFColor color;

        public:
            explicit Color(const NodeType & type);
            virtual ~Color() throw ();

            //
            // ColorNode implementation
            //
            virtual const MFColor & getColor() const throw ();
        
        private:
            //
            // field mutators
            //
            void setColor(const FieldValue & mfcolor)
                    throw (std::bad_cast, std::bad_alloc);
            
            //
            // eventIn handlers
            //
            void processSet_color(const FieldValue & mfcolor, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ColorInterpolatorClass : public NodeClass {
        public:
            explicit ColorInterpolatorClass(VrmlScene & scene);
            virtual ~ColorInterpolatorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ColorInterpolator : public AbstractChild {
            friend class ColorInterpolatorClass;

            MFFloat key;
            MFColor keyValue;
            SFColor value;

        public:
            explicit ColorInterpolator(const NodeType & type);
            virtual ~ColorInterpolator() throw ();

        private:
            //
            // field mutators
            //
            void setKey(const FieldValue & mffloat)
                    throw (std::bad_cast, std::bad_alloc);
            void setKeyValue(const FieldValue & mfcolor)
                    throw (std::bad_cast, std::bad_alloc);

            //
            // eventIn handlers
            //
            void processSet_fraction(const FieldValue & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const FieldValue & mffloat, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const FieldValue & mfcolor,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ConeClass : public NodeClass {
        public:
            explicit ConeClass(VrmlScene & scene);
            virtual ~ConeClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Cone : public AbstractGeometry {
            friend class ConeClass;

            SFBool bottom;
            SFFloat bottomRadius;
            SFFloat height;
            SFBool side;

        public:
            explicit Cone(const NodeType & type);
            virtual ~Cone() throw ();

            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
        };


        class OPENVRML_SCOPE CoordinateClass : public NodeClass {
        public:
            explicit CoordinateClass(VrmlScene & scene);
            virtual ~CoordinateClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Coordinate : public AbstractBase,
                                          public CoordinateNode {
            friend class CoordinateClass;

            MFVec3f point;

        public:
            explicit Coordinate(const NodeType & type);
            virtual ~Coordinate() throw ();

            //
            // CoordinateNode implementation
            //
            virtual const MFVec3f & getPoint() const throw ();
        
        private:
            //
            // field mutators
            //
            void setPoint(const FieldValue & mfvec3f)
                throw (std::bad_cast, std::bad_alloc);
            
            //
            // eventIn handlers
            //
            void processSet_point(const FieldValue & mfvec3f, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE CoordinateInterpolatorClass : public NodeClass {
        public:
            explicit CoordinateInterpolatorClass(VrmlScene & scene);
            virtual ~CoordinateInterpolatorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE CoordinateInterpolator : public AbstractChild {
            friend class CoordinateInterpolatorClass;

            MFFloat key;
            MFVec3f keyValue;
            MFVec3f value;

        public:
            explicit CoordinateInterpolator(const NodeType & type);
            virtual ~CoordinateInterpolator() throw ();

        private:
            //
            // field mutators
            //
            void setKey(const FieldValue & mffloat)
                    throw (std::bad_cast, std::bad_alloc);
            void setKeyValue(const FieldValue & mfvec3f)
                    throw (std::bad_cast, std::bad_alloc);
            
            //
            // eventIn handlers
            //
            void processSet_fraction(const FieldValue & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const FieldValue & mffloat,
                                double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const FieldValue & mfvec3f,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE CylinderClass : public NodeClass {
        public:
            explicit CylinderClass(VrmlScene & scene);
            virtual ~CylinderClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Cylinder : public AbstractGeometry {
            friend class CylinderClass;

            SFBool bottom;
            SFFloat height;
            SFFloat radius;
            SFBool side;
            SFBool top;

        public:
            explicit Cylinder(const NodeType & type);
            virtual ~Cylinder() throw ();
            
            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
        };


        class OPENVRML_SCOPE CylinderSensorClass : public NodeClass {
        public:
            explicit CylinderSensorClass(VrmlScene & scene);
            virtual ~CylinderSensorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE CylinderSensor : public AbstractChild {
            friend class CylinderSensorClass;

            SFBool autoOffset;
            SFFloat diskAngle;
            SFBool enabled;
            SFFloat maxAngle;
            SFFloat minAngle;
            SFFloat offset;
            SFBool active;
            SFRotation rotation;
            SFVec3f trackPoint;

            SFFloat rotation_val;
            SFVec3f activationPoint;
            SFBool disk;
            VrmlMatrix activationMatrix;
            VrmlMatrix M;

        public:
            explicit CylinderSensor(const NodeType & type);
            virtual ~CylinderSensor() throw ();

            virtual CylinderSensor * toCylinderSensor() const;

            virtual void render(Viewer* v, VrmlRenderContext rc);
            void activate( double timeStamp, bool isActive, double *p );

            bool isEnabled() { return this->enabled.get(); }

        private:
            //
            // eventIn handlers
            //
            void processSet_autoOffset(const FieldValue & sfbool,
                                       double timestamp) throw (std::bad_cast);
            void processSet_diskAngle(const FieldValue & sffloat,
                                      double timestamp) throw (std::bad_cast);
            void processSet_enabled(const FieldValue & sfbool,
                                    double timestamp) throw (std::bad_cast);
            void processSet_maxAngle(const FieldValue & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_minAngle(const FieldValue & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_offset(const FieldValue & sffloat,
                                   double timestamp) throw (std::bad_cast);
            
            void setMVMatrix(const VrmlMatrix & M_in);
            const VrmlMatrix & getMVMatrix()const;
        };


        class OPENVRML_SCOPE DirectionalLightClass : public NodeClass {
        public:
            explicit DirectionalLightClass(VrmlScene & scene);
            virtual ~DirectionalLightClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE DirectionalLight : public AbstractLight {
            friend class DirectionalLightClass;

            SFVec3f direction;

        public:
            explicit DirectionalLight(const NodeType & type);
            virtual ~DirectionalLight() throw ();

            virtual void render(Viewer *, VrmlRenderContext rc);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_direction(const FieldValue & sfvec3f,
                                      double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE ElevationGridClass : public NodeClass {
        public:
            explicit ElevationGridClass(VrmlScene & scene);
            virtual ~ElevationGridClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ElevationGrid : public AbstractGeometry {
            friend class ElevationGridClass;

            SFNode color;
            SFNode normal;
            SFNode texCoord;
            SFBool ccw;
            SFBool colorPerVertex;
            SFFloat creaseAngle;
            MFFloat height;
            SFBool normalPerVertex;
            SFBool solid;
            SFInt32 xDimension;
            SFFloat xSpacing;
            SFInt32 zDimension;
            SFFloat zSpacing;

        public:
            explicit ElevationGrid(const NodeType & type);
            virtual ~ElevationGrid() throw ();

            virtual bool isModified() const;
            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();
            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_color(const FieldValue & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_height(const FieldValue & mffloat, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_normal(const FieldValue & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_texCoord(const FieldValue & sfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ExtrusionClass : public NodeClass {
        public:
            explicit ExtrusionClass(VrmlScene & scene);
            virtual ~ExtrusionClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Extrusion : public AbstractGeometry {
            friend class ExtrusionClass;

            SFBool beginCap;
            SFBool ccw;
            SFBool convex;
            SFFloat creaseAngle;
            MFVec2f crossSection;
            SFBool endCap;
            MFRotation orientation;
            MFVec2f scale;
            SFBool solid;
            MFVec3f spine;

        public:
            explicit Extrusion(const NodeType & type);
            virtual ~Extrusion() throw ();

            virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        private:
            //
            // eventIn handlers
            //
            void processSet_crossSection(const FieldValue & mfvec2f,
                                         double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_orientation(const FieldValue & mfrotation,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_scale(const FieldValue & mfvec2f,
                                  double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_spine(const FieldValue & mfvec3f,
                                  double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE FogClass : public NodeClass {
        public:
            explicit FogClass(VrmlScene & scene);
            virtual ~FogClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Fog : public AbstractChild {
            friend class FogClass;

            SFColor color;
            SFString fogType;
            SFFloat visibilityRange;
            SFBool bound;

        public:
            explicit Fog(const NodeType & type);
            virtual ~Fog() throw ();

            virtual Fog * toFog() const;

            SFColor::ConstArrayReference getColor() const { return this->color.get(); }
            const std::string & getFogType() const { return this->fogType.get(); }
            float getVisibilityRange() const { return this->visibilityRange.get(); }
        
        private:
            //
            // eventIn handlers
            //
            void processSet_bind(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_color(const FieldValue & sfcolor, double timestamp)
                    throw (std::bad_cast);
            void processSet_fogType(const FieldValue & sfstring,
                                    double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_visibilityRange(const FieldValue & sffloat,
                                            double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE FontStyleClass : public NodeClass {
        public:
            explicit FontStyleClass(VrmlScene & scene);
            virtual ~FontStyleClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE FontStyle : public AbstractBase,
                                         public FontStyleNode {
            friend class FontStyleClass;

            MFString family;
            SFBool horizontal;
            MFString justify;
            SFString language;
            SFBool leftToRight;
            SFFloat size;
            SFFloat spacing;
            SFString style;
            SFBool topToBottom;

        public:
            explicit FontStyle(const NodeType & type);
            virtual ~FontStyle() throw ();

            //
            // FontStyleNode implementation
            //
            virtual const MFString & getFamily() const throw ();
            virtual const SFBool & getHorizontal() const throw ();
            virtual const MFString & getJustify() const throw ();
            virtual const SFString & getLanguage() const throw ();
            virtual const SFBool & getLeftToRight() const throw ();
            virtual const SFFloat & getSize() const throw ();
            virtual const SFFloat & getSpacing() const throw ();
            virtual const SFString & getStyle() const throw ();
            virtual const SFBool & getTopToBottom() const throw ();
        };


        class OPENVRML_SCOPE ImageTextureClass : public NodeClass {
        public:
            explicit ImageTextureClass(VrmlScene & scene);
            virtual ~ImageTextureClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ImageTexture : public AbstractTexture {
            friend class ImageTextureClass;

            MFString url;

            Image * image;

            Viewer::TextureObject texObject;

        public:
            explicit ImageTexture(const NodeType & type);
            virtual ~ImageTexture() throw ();

            virtual void render(Viewer *, VrmlRenderContext rc);

            virtual size_t nComponents() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t nFrames() const throw ();
            virtual const unsigned char * pixels() const throw ();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_url(const FieldValue & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE IndexedFaceSetClass : public NodeClass {
        public:
            explicit IndexedFaceSetClass(VrmlScene & scene);
            virtual ~IndexedFaceSetClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE IndexedFaceSet : public AbstractIndexedSet {
            friend class IndexedFaceSetClass;

            SFBool ccw;
            SFBool convex;
            SFFloat creaseAngle;
            SFNode normal;
            MFInt32 normalIndex;
            SFBool normalPerVertex;
            SFBool solid;
            SFNode texCoord;
            MFInt32 texCoordIndex;

            BSphere bsphere;

        public:
            explicit IndexedFaceSet(const NodeType & type);
            virtual ~IndexedFaceSet() throw ();

            virtual bool isModified() const;
            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();

            virtual Viewer::Object insertGeometry(Viewer * v,
                                                  VrmlRenderContext rc);
            virtual const BVolume * getBVolume() const;

        private:
            //
            // eventIn handlers
            //
            void processSet_normal(const FieldValue & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_normalIndex(const FieldValue & mfint32,
                                        double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_texCoord(const FieldValue & sfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_texCoordIndex(const FieldValue & mfint32,
                                          double timestamp)
                     throw (std::bad_cast, std::bad_alloc);

            void recalcBSphere();
        };


        class OPENVRML_SCOPE IndexedLineSetClass : public NodeClass {
        public:
            explicit IndexedLineSetClass(VrmlScene & scene);
            virtual ~IndexedLineSetClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE IndexedLineSet : public AbstractIndexedSet {
            friend class IndexedLineSetClass;

        public:
            explicit IndexedLineSet(const NodeType & type);
            virtual ~IndexedLineSet() throw ();

            virtual Viewer::Object insertGeometry(Viewer * v,
                                                  VrmlRenderContext rc);
        };


        class OPENVRML_SCOPE InlineClass : public NodeClass {
        public:
            explicit InlineClass(VrmlScene & scene);
            virtual ~InlineClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Inline : public Group {
            friend class InlineClass;

            MFString url;
            
            VrmlNamespace * scope;
            bool hasLoaded;

        public:
            explicit Inline(const NodeType & type);
            virtual ~Inline() throw ();

            virtual void render(Viewer *, VrmlRenderContext rc);
            virtual Inline * toInline() const;

        private:
            void load();
        
            //
            // eventIn handlers
            //
            void processSet_url(const FieldValue & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE LODClass : public NodeClass {
        public:
            explicit LODClass(VrmlScene & scene);
            virtual ~LODClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE LOD : public AbstractChild {
            friend class LODClass;

            MFNode level;
            SFVec3f center;
            MFFloat range;

            BSphere bsphere;

        public:
            explicit LOD(const NodeType & type);
            virtual ~LOD() throw ();

            virtual bool isModified() const;
            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();
            virtual void render(Viewer *, VrmlRenderContext rc);
            virtual const BVolume * getBVolume() const;

        private:
            //
            // eventIn handlers
            //
            void processSet_level(const FieldValue & mfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);

            virtual void recalcBSphere();
        };


        class OPENVRML_SCOPE MaterialClass : public NodeClass {
        public:
            explicit MaterialClass(VrmlScene & scene);
            virtual ~MaterialClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Material : public AbstractBase,
                                        public MaterialNode {
            friend class MaterialClass;

            SFFloat ambientIntensity;
            SFColor diffuseColor;
            SFColor emissiveColor;
            SFFloat shininess;
            SFColor specularColor;
            SFFloat transparency;

        public:
            explicit Material(const NodeType & type);
            virtual ~Material() throw ();

            //
            // MaterialNode implementation
            //
            virtual const SFFloat & getAmbientIntensity() const throw ();
            virtual const SFColor & getDiffuseColor() const throw ();
            virtual const SFColor & getEmissiveColor() const throw ();
            virtual const SFFloat & getShininess() const throw ();
            virtual const SFColor & getSpecularColor() const throw ();
            virtual const SFFloat & getTransparency() const throw ();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_ambientIntensity(const FieldValue & sffloat,
                                             double timestamp)
                    throw (std::bad_cast);
            void processSet_diffuseColor(const FieldValue & sfcolor,
                                         double timestamp)
                    throw (std::bad_cast);
            void processSet_emissiveColor(const FieldValue & sfcolor,
                                          double timestamp)
                    throw (std::bad_cast);
            void processSet_shininess(const FieldValue & sffloat,
                                      double timestamp)
                    throw (std::bad_cast);
            void processSet_specularColor(const FieldValue & sfcolor,
                                          double timestamp)
                    throw (std::bad_cast);
            void processSet_transparency(const FieldValue & sffloat,
                                         double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE MovieTextureClass : public NodeClass {
        public:
            explicit MovieTextureClass(VrmlScene & scene);
            virtual ~MovieTextureClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE MovieTexture : public AbstractTexture {
            friend class MovieTextureClass;

            SFBool loop;
            SFFloat speed;
            SFTime startTime;
            SFTime stopTime;
            MFString url;
            SFTime duration;
            SFBool active;

            Image * image;
            int frame, lastFrame;
            double lastFrameTime;

            Viewer::TextureObject texObject;

        public:
            explicit MovieTexture(const NodeType & type);
            virtual ~MovieTexture() throw ();

            virtual MovieTexture * toMovieTexture() const;

            void update(double time);

            virtual void render(Viewer *, VrmlRenderContext rc);

            virtual size_t nComponents() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t nFrames() const throw ();
            virtual const unsigned char * pixels() const throw ();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_loop(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_speed(const FieldValue & sffloat, double timestamp)
                    throw (std::bad_cast);
            void processSet_startTime(const FieldValue & sftime,
                                      double timestamp) throw (std::bad_cast);
            void processSet_stopTime(const FieldValue & sftime,
                                     double timestamp) throw (std::bad_cast);
            void processSet_url(const FieldValue & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE NavigationInfoClass : public NodeClass {
        public:
            explicit NavigationInfoClass(VrmlScene & scene);
            virtual ~NavigationInfoClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE NavigationInfo : public AbstractChild {
            friend class NavigationInfoClass;

            MFFloat avatarSize;
            SFBool headlight;
            SFFloat speed;
            MFString type;
            SFFloat visibilityLimit;
            SFBool bound;

        public:
            explicit NavigationInfo(const NodeType & type);
            virtual ~NavigationInfo() throw ();

            virtual NavigationInfo * toNavigationInfo() const;

            const float * getAvatarSize() { return this->avatarSize.get(); }
            bool getHeadlightOn() { return this->headlight.get(); }
            float getSpeed() { return this->speed.get(); }
            float getVisibilityLimit() { return this->visibilityLimit.get(); }
        
        private:
            //
            // eventIn handlers
            //
            void processSet_avatarSize(const FieldValue & mffloat,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_bind(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_headlight(const FieldValue & sfbool,
                                      double timestamp) throw (std::bad_cast);
            void processSet_speed(const FieldValue & sffloat, double timestamp)
                    throw (std::bad_cast);
            void processSet_type(const FieldValue & mfstring, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_visibilityLimit(const FieldValue & sffloat,
                                            double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE NormalClass : public NodeClass {
        public:
            explicit NormalClass(VrmlScene & scene);
            virtual ~NormalClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Normal : public AbstractBase, public NormalNode {
            friend class NormalClass;

            MFVec3f vector;

        public:
            explicit Normal(const NodeType & type);
            virtual ~Normal() throw ();

            //
            // NormalNode implementation.
            //
            virtual const MFVec3f & getVector() const throw ();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_vector(const FieldValue & mfvec3f, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE NormalInterpolatorClass : public NodeClass {
        public:
            explicit NormalInterpolatorClass(VrmlScene & scene);
            virtual ~NormalInterpolatorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE NormalInterpolator : public AbstractChild {
            friend class NormalInterpolatorClass;

            MFFloat key;
            MFVec3f keyValue;
            MFVec3f value;

        public:
            explicit NormalInterpolator(const NodeType & type);
            virtual ~NormalInterpolator() throw ();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_fraction(const FieldValue & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const FieldValue & mffloat, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const FieldValue & mfvec3f,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE OrientationInterpolatorClass : public NodeClass {
        public:
            explicit OrientationInterpolatorClass(VrmlScene & scene);
            virtual ~OrientationInterpolatorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE OrientationInterpolator : public AbstractChild {
            friend class OrientationInterpolatorClass;

            MFFloat key;
            MFRotation keyValue;
            SFRotation value;

        public:
            explicit OrientationInterpolator(const NodeType & type);
            virtual ~OrientationInterpolator() throw ();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_fraction(const FieldValue & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const FieldValue & mffloat, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const FieldValue & mfrotation,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE PixelTextureClass : public NodeClass {
        public:
            explicit PixelTextureClass(VrmlScene & scene);
            virtual ~PixelTextureClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PixelTexture : public AbstractTexture {
            friend class PixelTextureClass;

            SFImage image;

            Viewer::TextureObject texObject;

        public:
            explicit PixelTexture(const NodeType & type);
            virtual ~PixelTexture() throw ();

            virtual void render(Viewer *, VrmlRenderContext rc);

            virtual size_t nComponents() const throw ();
            virtual size_t width() const throw ();
            virtual size_t height() const throw ();
            virtual size_t nFrames() const throw ();
            virtual const unsigned char * pixels() const throw ();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_image(const FieldValue & sfimage, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE PlaneSensorClass : public NodeClass {
        public:
            explicit PlaneSensorClass(VrmlScene & scene);
            virtual ~PlaneSensorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PlaneSensor : public AbstractChild {
            friend class PlaneSensorClass;

            SFBool autoOffset;
            SFBool enabled;
            SFVec2f maxPosition;
            SFVec2f minPosition;
            SFVec3f offset;
            SFBool active;
            SFVec3f translation;
            SFVec3f trackPoint;

            SFVec3f activationPoint;

            Node * parentTransform;
            VrmlMatrix activationMatrix;
            VrmlMatrix M;

        public:
            explicit PlaneSensor(const NodeType & type);
            virtual ~PlaneSensor() throw ();

            virtual PlaneSensor * toPlaneSensor() const;

            virtual void render(Viewer* v, VrmlRenderContext rc);
            void activate( double timeStamp, bool isActive, double *p );

            virtual void accumulateTransform( Node* );
            virtual Node* getParentTransform();

            bool isEnabled() { return this->enabled.get(); }

        private:
            //
            // eventIn handlers
            //
            void processSet_autoOffset(const FieldValue & sfbool,
                                       double timestamp) throw (std::bad_cast);
            void processSet_enabled(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_maxPosition(const FieldValue & sfvec2f,
                                        double timestamp) throw (std::bad_cast);
            void processSet_minPosition(const FieldValue & sfvec2f,
                                        double timestamp) throw (std::bad_cast);
            void processSet_offset(const FieldValue & sfvec3f, double timestamp)
                    throw (std::bad_cast);

            void setMVMatrix(const VrmlMatrix & M_in);
            const VrmlMatrix & getMVMatrix()const;
        };


        class OPENVRML_SCOPE PointLightClass : public NodeClass {
        public:
            explicit PointLightClass(VrmlScene & scene);
            virtual ~PointLightClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PointLight : public AbstractLight {
            friend class PointLightClass;

            SFVec3f attenuation;
            SFVec3f location;
            SFFloat radius;

        public:
            explicit PointLight(const NodeType & type);
            virtual ~PointLight() throw ();

            virtual PointLight * toPointLight() const;

            virtual void renderScoped(Viewer *);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_attenuation(const FieldValue & sfvec3f,
                                        double timestamp) throw (std::bad_cast);
            void processSet_location(const FieldValue & sfvec3f,
                                     double timestamp) throw (std::bad_cast);
            void processSet_radius(const FieldValue & sffloat, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE PointSetClass : public NodeClass {
        public:
            explicit PointSetClass(VrmlScene & scene);
            virtual ~PointSetClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PointSet : public AbstractGeometry {
            friend class PointSetClass;

            SFNode color;
            SFNode coord;

            BSphere bsphere;

        public:
            explicit PointSet(const NodeType & type);
            virtual ~PointSet() throw ();

            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual bool isModified() const;
            virtual void clearFlags();
            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
            virtual const BVolume * getBVolume() const;

        private:
            //
            // eventIn handlers
            //
            void processSet_color(const FieldValue & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_coord(const FieldValue & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);

            void recalcBSphere();
        };


        class OPENVRML_SCOPE PositionInterpolatorClass : public NodeClass {
        public:
            explicit PositionInterpolatorClass(VrmlScene & scene);
            virtual ~PositionInterpolatorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE PositionInterpolator : public AbstractChild {
            friend class PositionInterpolatorClass;

            MFFloat key;
            MFVec3f keyValue;
            SFVec3f value;

        public:
            explicit PositionInterpolator(const NodeType & type);
            virtual ~PositionInterpolator() throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_fraction(const FieldValue & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const FieldValue & mffloat,
                                double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const FieldValue & mfvec3f,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ProximitySensorClass : public NodeClass {
        public:
            explicit ProximitySensorClass(VrmlScene & scene);
            virtual ~ProximitySensorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ProximitySensor : public AbstractChild {
            friend class ProximitySensorClass;

            SFVec3f center;
            SFBool enabled;
            SFVec3f size;
            SFBool active;
            SFVec3f position;
            SFRotation orientation;
            SFTime enterTime;
            SFTime exitTime;

        public:
            explicit ProximitySensor(const NodeType & type);
            virtual ~ProximitySensor() throw ();

            virtual void render(Viewer *, VrmlRenderContext rc);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_center(const FieldValue & sfvec3f, double timestamp)
                    throw (std::bad_cast);
            void processSet_size(const FieldValue & sfvec3f, double timestamp)
                    throw (std::bad_cast);
            void processSet_enabled(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE ScalarInterpolatorClass : public NodeClass {
        public:
            explicit ScalarInterpolatorClass(VrmlScene & scene);
            virtual ~ScalarInterpolatorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE ScalarInterpolator : public AbstractChild {
            friend class ScalarInterpolatorClass;

            MFFloat key;
            MFFloat keyValue;
            SFFloat value;

        public:
            explicit ScalarInterpolator(const NodeType & type);
            virtual ~ScalarInterpolator() throw ();

        private:
            //
            // eventIn handlers
            //
            void processSet_fraction(const FieldValue & sffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_key(const FieldValue & mffloat,
                                double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_keyValue(const FieldValue & mffloat,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE ShapeClass : public NodeClass {
        public:
            explicit ShapeClass(VrmlScene & scene);
            virtual ~ShapeClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Shape : public AbstractChild {
            friend class ShapeClass;

            SFNode appearance;
            SFNode geometry;

            Viewer::Object viewerObject; // move to Node.h ? ...

        public:
            explicit Shape(const NodeType & type);
            virtual ~Shape() throw ();

            virtual bool isModified() const;
            virtual void updateModified(NodePath& path, int flags);
            virtual void clearFlags();
            virtual const BVolume * getBVolume() const;
            virtual void render(Viewer *, VrmlRenderContext rc);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_appearance(const FieldValue & sfnode,
                                       double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_geometry(const FieldValue & sfnode,
                                     double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE SoundClass : public NodeClass {
        public:
            explicit SoundClass(VrmlScene & scene);
            virtual ~SoundClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Sound : public AbstractChild {
            friend class SoundClass;

            SFVec3f direction;
            SFFloat intensity;
            SFVec3f location;
            SFFloat maxBack;
            SFFloat maxFront;
            SFFloat minBack;
            SFFloat minFront;
            SFFloat priority;
            SFNode source;
            SFBool spatialize;

        public:
            explicit Sound(const NodeType & type);
            virtual ~Sound() throw ();

            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();
            virtual void render(Viewer *, VrmlRenderContext rc);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_direction(const FieldValue & sfvec3f,
                                      double timestamp) throw (std::bad_cast);
            void processSet_intensity(const FieldValue & sffloat,
                                      double timestamp) throw (std::bad_cast);
            void processSet_location(const FieldValue & sfvec3f,
                                     double timestamp) throw (std::bad_cast);
            void processSet_maxBack(const FieldValue & sffloat,
                                    double timestamp) throw (std::bad_cast);
            void processSet_maxFront(const FieldValue & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_minBack(const FieldValue & sffloat,
                                    double timestamp) throw (std::bad_cast);
            void processSet_minFront(const FieldValue & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_priority(const FieldValue & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_source(const FieldValue & sfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE SphereClass : public NodeClass {
        public:
            explicit SphereClass(VrmlScene & scene);
            virtual ~SphereClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Sphere : public AbstractGeometry {
            friend class SphereClass;

            SFFloat radius;
            BSphere bsphere;

        public:
            explicit Sphere(const NodeType & type);
            virtual ~Sphere() throw ();

            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
            virtual const BVolume * getBVolume() const;
        };


        class OPENVRML_SCOPE SphereSensorClass : public NodeClass {
        public:
            explicit SphereSensorClass(VrmlScene & scene);
            virtual ~SphereSensorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE SphereSensor : public AbstractChild {
            friend class SphereSensorClass;

            SFBool autoOffset;
            SFBool enabled;
            SFRotation offset;
            SFBool active;
            SFRotation rotation;
            SFVec3f trackPoint;

            SFVec3f activationPoint;
            SFVec3f centerPoint;
            VrmlMatrix M;

        public:
            explicit SphereSensor(const NodeType & type);
            virtual ~SphereSensor() throw ();

            virtual SphereSensor * toSphereSensor() const;

            virtual void render(Viewer* v, VrmlRenderContext rc);
            void activate( double timeStamp, bool isActive, double *p );

            bool isEnabled() { return this->enabled.get(); }

        private:
            //
            // eventIn handlers
            //
            void processSet_autoOffset(const FieldValue & sfbool,
                                       double timestamp) throw (std::bad_cast);
            void processSet_enabled(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_offset(const FieldValue & sfrotation,
                                   double timestamp) throw (std::bad_cast);

            void setMVMatrix(const VrmlMatrix & M_in);
            const VrmlMatrix & getMVMatrix()const;
        };


        class OPENVRML_SCOPE SpotLightClass : public NodeClass {
        public:
            explicit SpotLightClass(VrmlScene & scene);
            virtual ~SpotLightClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE SpotLight : public AbstractLight {
            friend class SpotLightClass;

            SFVec3f attenuation;
            SFFloat beamWidth;
            SFFloat cutOffAngle;
            SFVec3f direction;
            SFVec3f location;
            SFFloat radius;

        public:
            explicit SpotLight(const NodeType & type);
            virtual ~SpotLight() throw ();

            virtual SpotLight * toSpotLight() const;

            virtual void renderScoped(Viewer *);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_attenuation(const FieldValue & sfvec3f,
                                        double timestamp) throw (std::bad_cast);
            void processSet_beamWidth(const FieldValue & sffloat,
                                      double timestamp) throw (std::bad_cast);
            void processSet_cutOffAngle(const FieldValue & sffloat,
                                        double timestamp) throw (std::bad_cast);
            void processSet_direction(const FieldValue & sfvec3f,
                                      double timestamp) throw (std::bad_cast);
            void processSet_location(const FieldValue & sfvec3f,
                                     double timestamp) throw (std::bad_cast);
            void processSet_radius(const FieldValue & sffloat,
                                   double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE SwitchClass : public NodeClass {
        public:
            explicit SwitchClass(VrmlScene & scene);
            virtual ~SwitchClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Switch : public AbstractChild {
            friend class SwitchClass;

            MFNode choice;
            SFInt32 whichChoice;

            BSphere bsphere;

        public:
            explicit Switch(const NodeType & type);
            virtual ~Switch() throw ();

            virtual bool isModified() const;
            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();

            virtual void render(Viewer *, VrmlRenderContext rc);

            virtual const BVolume * getBVolume() const;

        private:
            //
            // eventIn handlers
            //
            void processSet_choice(const FieldValue & mfnode, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_whichChoice(const FieldValue & sfint32,
                                        double timestamp) throw (std::bad_cast);

            virtual void recalcBSphere();
        };


        class OPENVRML_SCOPE TextClass : public NodeClass {
        public:
            explicit TextClass(VrmlScene & scene);
            virtual ~TextClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Text : public AbstractGeometry {
            friend class TextClass;

            MFString string;
            SFNode fontStyle;
            MFFloat length;
            SFFloat maxExtent;

        public:
            explicit Text(const NodeType & type);
            virtual ~Text() throw ();

            virtual bool isModified() const;
            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();

            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_string(const FieldValue & mfstring,
                                   double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_fontStyle(const FieldValue & sfnode,
                                      double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_length(const FieldValue & mffloat,
                                   double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_maxExtent(const FieldValue & sffloat,
                                      double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE TextureCoordinateClass : public NodeClass {
        public:
            explicit TextureCoordinateClass(VrmlScene & scene);
            virtual ~TextureCoordinateClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE TextureCoordinate : public AbstractBase,
                                                 public TextureCoordinateNode {
            friend class TextureCoordinateClass;

            MFVec2f point;

        public:
            explicit TextureCoordinate(const NodeType & type);
            virtual ~TextureCoordinate() throw ();

            //
            // TextureCoordinateNode implementation.
            //
            virtual const MFVec2f & getPoint() const throw();
        
        private:
            //
            // eventIn handlers
            //
            void processSet_point(const FieldValue & mfvec2f, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
        };


        class OPENVRML_SCOPE TextureTransformClass : public NodeClass {
        public:
            explicit TextureTransformClass(VrmlScene & scene);
            virtual ~TextureTransformClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE TextureTransform : public AbstractBase,
                                                public TextureTransformNode {
            friend class TextureTransformClass;

            SFVec2f center;
            SFFloat rotation;
            SFVec2f scale;
            SFVec2f translation;

        public:
            explicit TextureTransform(const NodeType & type);
            virtual ~TextureTransform() throw ();

            virtual void render(Viewer *, VrmlRenderContext rc);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_center(const FieldValue & sfvec2f, double timestamp)
                    throw (std::bad_cast);
            void processSet_rotation(const FieldValue & sffloat,
                                     double timestamp) throw (std::bad_cast);
            void processSet_scale(const FieldValue & sfvec2f, double timestamp)
                    throw (std::bad_cast);
            void processSet_translation(const FieldValue & sfvec2f,
                                        double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE TimeSensorClass : public NodeClass {
        public:
            explicit TimeSensorClass(VrmlScene & scene);
            virtual ~TimeSensorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE TimeSensor : public AbstractChild {
            friend class TimeSensorClass;

            SFTime cycleInterval;
            SFBool enabled;
            SFBool loop;
            SFTime startTime;
            SFTime stopTime;
            SFTime cycleTime;
            SFFloat fraction;
            SFBool active;
            SFTime time;
            
            double lastTime;

        public:
            explicit TimeSensor(const NodeType & type);
            virtual ~TimeSensor() throw ();

            virtual TimeSensor * toTimeSensor() const;
            virtual const BVolume * getBVolume() const;

            void update(double time);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_cycleInterval(const FieldValue & sftime,
                                          double timestamp)
                    throw (std::bad_cast);
            void processSet_enabled(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_loop(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_startTime(const FieldValue & sftime,
                                      double timestamp) throw (std::bad_cast);
            void processSet_stopTime(const FieldValue & sftime,
                                     double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE TouchSensorClass : public NodeClass {
        public:
            explicit TouchSensorClass(VrmlScene & scene);
            virtual ~TouchSensorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE TouchSensor : public AbstractChild {
            friend class TouchSensorClass;

            SFBool enabled;
            SFVec3f hitNormal;
            SFVec3f hitPoint;
            SFVec2f hitTexCoord;
            SFBool active;
            SFBool over;
            SFTime touchTime;

        public:
            explicit TouchSensor(const NodeType & type);
            virtual ~TouchSensor() throw ();

            virtual TouchSensor * toTouchSensor() const;

            void activate( double timeStamp, bool isOver, bool isActive, double *p );

            bool isEnabled() const;
        
        private:
            //
            // eventIn handlers
            //
            void processSet_enabled(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE TransformClass : public NodeClass {
        public:
            explicit TransformClass(VrmlScene & scene);
            virtual ~TransformClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Transform : public Group {
            friend class TransformClass;

            SFVec3f center;
            SFRotation rotation;
            SFVec3f scale;
            SFRotation scaleOrientation;
            SFVec3f translation;
            
            Viewer::Object xformObject;
            VrmlMatrix M;
            bool M_dirty;

        public:
            explicit Transform(const NodeType & type);
            virtual ~Transform() throw ();

            virtual void render(Viewer * viewer, VrmlRenderContext rc);

            virtual void accumulateTransform(Node*);
            virtual void inverseTransform(Viewer *);
            virtual void inverseTransform(VrmlMatrix &);
            virtual const BVolume * getBVolume() const;

            void getMatrix(VrmlMatrix & M) const;

        private:
            //
            // eventIn handlers
            //
            void processSet_center(const FieldValue & sfvec3f, double timestamp)
                    throw (std::bad_cast);
            void processSet_rotation(const FieldValue & sfrotation,
                                     double timestamp) throw (std::bad_cast);
            void processSet_scale(const FieldValue & sfvec3f, double timestamp)
                    throw (std::bad_cast);
            void processSet_scaleOrientation(const FieldValue & sfrotation,
                                             double timestamp)
                    throw (std::bad_cast);
            void processSet_translation(const FieldValue & sfvec3f,
                                        double timestamp) throw (std::bad_cast);

            void recalcBSphere();
            void synch_cached_matrix();
        };


        class OPENVRML_SCOPE ViewpointClass : public NodeClass {
        public:
            explicit ViewpointClass(VrmlScene & scene);
            virtual ~ViewpointClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Viewpoint : public AbstractChild {
            friend class ViewpointClass;

            SFFloat fieldOfView;
            SFBool jump;
            SFRotation orientation;
            SFVec3f position;
            SFString description;
            SFBool bound;
            SFTime bindTime;

            Node * parentTransform;

        public:
            explicit Viewpoint(const NodeType & type);
            virtual ~Viewpoint() throw ();

            virtual Viewpoint * toViewpoint() const;

            virtual void accumulateTransform( Node* );
            virtual Node* getParentTransform();

            const SFFloat & getFieldOfView() const;
            const SFRotation & getOrientation() const;
            const SFVec3f & getPosition() const;
            const SFString & getDescription() const;

            const BVolume * getBVolume() const;

            void getInverseMatrix(VrmlMatrix & M) const;

            void getFrustum(VrmlFrustum& frust) const; // get a copy
        
        private:
            //
            // eventIn handlers
            //
            void processSet_bind(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast, std::bad_alloc);
            void processSet_fieldOfView(const FieldValue & sffloat,
                                        double timestamp) throw (std::bad_cast);
            void processSet_jump(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_orientation(const FieldValue & sfrotation,
                                        double timestamp) throw (std::bad_cast);
            void processSet_position(const FieldValue & sfvec3f,
                                     double timestamp) throw (std::bad_cast);
        };


        class OPENVRML_SCOPE VisibilitySensorClass : public NodeClass {
        public:
            explicit VisibilitySensorClass(VrmlScene & scene);
            virtual ~VisibilitySensorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE VisibilitySensor : public AbstractChild {
            friend class VisibilitySensorClass;

            SFVec3f center;
            SFBool enabled;
            SFVec3f size;
            SFBool active;
            SFTime enterTime;
            SFTime exitTime;

        public:
            explicit VisibilitySensor(const NodeType & type);
            virtual ~VisibilitySensor() throw ();

            virtual void render(Viewer *, VrmlRenderContext rc);
        
        private:
            //
            // eventIn handlers
            //
            void processSet_center(const FieldValue & sfvec3f, double timestamp)
                    throw (std::bad_cast);
            void processSet_enabled(const FieldValue & sfbool, double timestamp)
                    throw (std::bad_cast);
            void processSet_size(const FieldValue & sfvec3f, double timestamp)
                    throw (std::bad_cast);
        };


        class OPENVRML_SCOPE WorldInfoClass : public NodeClass {
        public:
            explicit WorldInfoClass(VrmlScene & scene);
            virtual ~WorldInfoClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE WorldInfo : public AbstractChild {
            friend class WorldInfoClass;

            MFString info;
            SFString title;

        public:
            explicit WorldInfo(const NodeType & type);
            virtual ~WorldInfo() throw ();
        };
    }
}

# endif
