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

typedef unsigned int FT_UInt;
typedef struct FT_LibraryRec_ * FT_Library;
typedef struct FT_FaceRec_ * FT_Face;
typedef unsigned int FcChar32;

namespace OpenVRML {

    class Audio;

    namespace Vrml97Node {

        class OPENVRML_SCOPE AbstractBase : public virtual Node {
        public:
            virtual ~AbstractBase() throw () = 0;

        protected:
            AbstractBase(const NodeType & nodeType, const ScopePtr & scope);
        
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
            AbstractChild(const NodeType & nodeType, const ScopePtr & scope);
        };


        class OPENVRML_SCOPE AbstractGeometry :
                public AbstractBase, public GeometryNode {
        public:
            virtual ~AbstractGeometry() throw () = 0;

            virtual void render(Viewer *, VrmlRenderContext rc);
            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc) = 0;

        protected:
            AbstractGeometry(const NodeType & nodeType, const ScopePtr & scope);

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
            AbstractIndexedSet(const NodeType & nodeType,
                               const ScopePtr & scope);

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
            AbstractLight(const NodeType & nodeType, const ScopePtr & scope);

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
        
        public:
            virtual ~AbstractTexture() throw () = 0;

            //
            // TextureNode implementation.
            //
            virtual const SFBool & getRepeatS() const throw ();
            virtual const SFBool & getRepeatT() const throw ();
            
        protected:
            AbstractTexture(const NodeType & nodeType, const ScopePtr & scope);
        };


        class OPENVRML_SCOPE GroupClass : public NodeClass {
        public:
            explicit GroupClass(Browser & browser);
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
            Group(const NodeType & nodeType,
                  const ScopePtr & scope);
            virtual ~Group() throw ();
            
            virtual Group * toGroup() const;

            virtual bool isModified() const;
            virtual void updateModified(NodePath& path, int flags);
            virtual void clearFlags();

            virtual void render(Viewer *, VrmlRenderContext rc);
            virtual void renderNoCull(Viewer *, VrmlRenderContext rc);

            virtual void accumulateTransform(Node*);

            void activate( double timeStamp, bool isOver, bool isActive, double *p );

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
            explicit AnchorClass(Browser & browser);
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
            Anchor(const NodeType & nodeType,
                   const ScopePtr & scope);
            virtual ~Anchor() throw ();
            
            virtual Anchor * toAnchor() const;

            virtual void render(Viewer *, VrmlRenderContext rc);

            void activate();

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
            explicit AppearanceClass(Browser & browser);
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
            Appearance(const NodeType & nodeType,
                       const ScopePtr & scope);
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
            explicit AudioClipClass(Browser & browser);
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
            AudioClip(const NodeType & nodeType,
                      const ScopePtr & scope);
            virtual ~AudioClip() throw ();
            
            void update(double time);

            virtual AudioClip * toAudioClip() const;
        
        private:
            virtual void initializeImpl(double timestamp) throw ();
                
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


        class Background;

        class OPENVRML_SCOPE BackgroundClass : public NodeClass {
            Background * first;
            std::vector<NodePtr> boundNodes;

        public:
            explicit BackgroundClass(Browser & browser);
            virtual ~BackgroundClass() throw ();

            void setFirst(Background & background) throw ();
            bool hasFirst() const throw ();
            void bind(Background & background, double timestamp)
                throw (std::bad_alloc);
            void unbind(Background & background, double timestamp) throw ();

            virtual void initialize(double timestamp) throw ();
            virtual void render(Viewer & viewer) throw ();
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
            Background(const NodeType & nodeType,
                       const ScopePtr & scope);
            virtual ~Background() throw ();
            
        private:
            virtual void initializeImpl(double timestamp) throw ();
            
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
            explicit BillboardClass(Browser & browser);
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

            Billboard(const NodeType & nodeType,
                      const ScopePtr & scope);
            virtual ~Billboard() throw ();
            
            virtual void render(Viewer *, VrmlRenderContext rc);

            virtual void accumulateTransform(Node*);
            virtual Node* getParentTransform();
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
            explicit BoxClass(Browser & browser);
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
            Box(const NodeType & nodeType,
                const ScopePtr & scope);
            virtual ~Box() throw ();
            
            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
            virtual const BVolume * getBVolume() const;
        };


        class OPENVRML_SCOPE CollisionClass : public NodeClass {
        public:
            explicit CollisionClass(Browser & browser);
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
            Collision(const NodeType & nodeType,
                      const ScopePtr & scope);
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
            explicit ColorClass(Browser & browser);
            virtual ~ColorClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Color : public AbstractBase, public ColorNode {
            friend class ColorClass;

            MFColor color;

        public:
            Color(const NodeType & nodeType,
                  const ScopePtr & scope);
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
            explicit ColorInterpolatorClass(Browser & browser);
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
            ColorInterpolator(const NodeType & nodeType,
                              const ScopePtr & scope);
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
            explicit ConeClass(Browser & browser);
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
            Cone(const NodeType & nodeType,
                 const ScopePtr & scope);
            virtual ~Cone() throw ();

            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
        };


        class OPENVRML_SCOPE CoordinateClass : public NodeClass {
        public:
            explicit CoordinateClass(Browser & browser);
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
            Coordinate(const NodeType & nodeType,
                       const ScopePtr & scope);
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
            explicit CoordinateInterpolatorClass(Browser & browser);
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
            CoordinateInterpolator(const NodeType & nodeType,
                                   const ScopePtr & scope);
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
            explicit CylinderClass(Browser & browser);
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
            Cylinder(const NodeType & nodeType,
                     const ScopePtr & scope);
            virtual ~Cylinder() throw ();
            
            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
        };


        class OPENVRML_SCOPE CylinderSensorClass : public NodeClass {
        public:
            explicit CylinderSensorClass(Browser & browser);
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
            CylinderSensor(const NodeType & nodeType,
                           const ScopePtr & scope);
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
            explicit DirectionalLightClass(Browser & browser);
            virtual ~DirectionalLightClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE DirectionalLight : public AbstractLight {
            friend class DirectionalLightClass;

            SFVec3f direction;

        public:
            DirectionalLight(const NodeType & nodeType,
                             const ScopePtr & scope);
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
            explicit ElevationGridClass(Browser & browser);
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
            ElevationGrid(const NodeType & nodeType,
                          const ScopePtr & scope);
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
            explicit ExtrusionClass(Browser & browser);
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
            Extrusion(const NodeType & nodeType,
                      const ScopePtr & scope);
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


        class Fog;

        class OPENVRML_SCOPE FogClass : public NodeClass {
            Fog * first;
            std::vector<NodePtr> boundNodes;

        public:
            explicit FogClass(Browser & browser);
            virtual ~FogClass() throw ();

            void setFirst(Fog & fog) throw ();
            bool hasFirst() const throw ();
            void bind(Fog & fog, double timestamp) throw (std::bad_alloc);
            void unbind(Fog & fog, double timestamp) throw ();

            virtual void initialize(double timestamp) throw ();
            virtual void render(Viewer & viewer) throw ();
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
            Fog(const NodeType & nodeType,
                const ScopePtr & scope);
            virtual ~Fog() throw ();

        private:
            virtual void initializeImpl(double timestamp) throw ();
            
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
            explicit FontStyleClass(Browser & browser);
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
            FontStyle(const NodeType & nodeType,
                      const ScopePtr & scope);
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
            explicit ImageTextureClass(Browser & browser);
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
            ImageTexture(const NodeType & nodeType,
                         const ScopePtr & scope);
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
            explicit IndexedFaceSetClass(Browser & browser);
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
            IndexedFaceSet(const NodeType & nodeType,
                           const ScopePtr & scope);
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
            explicit IndexedLineSetClass(Browser & browser);
            virtual ~IndexedLineSetClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE IndexedLineSet : public AbstractIndexedSet {
            friend class IndexedLineSetClass;

        public:
            IndexedLineSet(const NodeType & nodeType,
                           const ScopePtr & scope);
            virtual ~IndexedLineSet() throw ();

            virtual Viewer::Object insertGeometry(Viewer * v,
                                                  VrmlRenderContext rc);
        };


        class OPENVRML_SCOPE InlineClass : public NodeClass {
        public:
            explicit InlineClass(Browser & browser);
            virtual ~InlineClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Inline : public AbstractChild {
            friend class InlineClass;

            SFVec3f bboxCenter;
            SFVec3f bboxSize;
            MFString url;
            
            Scene * inlineScene;
            bool hasLoaded;

        public:
            Inline(const NodeType & nodeType,
                   const ScopePtr & scope);
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
            explicit LODClass(Browser & browser);
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
            LOD(const NodeType & nodeType,
                const ScopePtr & scope);
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
            explicit MaterialClass(Browser & browser);
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
            Material(const NodeType & nodeType,
                     const ScopePtr & scope);
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
            explicit MovieTextureClass(Browser & browser);
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
            MovieTexture(const NodeType & nodeType,
                         const ScopePtr & scope);
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
            virtual void initializeImpl(double timestamp) throw ();
            
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
            explicit NavigationInfoClass(Browser & browser);
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
            NavigationInfo(const NodeType & nodeType,
                           const ScopePtr & scope);
            virtual ~NavigationInfo() throw ();

            virtual NavigationInfo * toNavigationInfo() const;

            const float * getAvatarSize()
            {
                return this->avatarSize.getLength() > 0
                        ? &this->avatarSize.getElement(0)
                        : 0;
            }
            bool getHeadlightOn() { return this->headlight.get(); }
            float getSpeed() { return this->speed.get(); }
            float getVisibilityLimit() { return this->visibilityLimit.get(); }
        
        private:
            virtual void initializeImpl(double timestamp) throw ();
            
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
            explicit NormalClass(Browser & browser);
            virtual ~NormalClass() throw ();

            virtual const NodeTypePtr createType(const std::string & id,
                                                 const NodeInterfaceSet &)
                    throw (UnsupportedInterface, std::bad_alloc);
        };

        class OPENVRML_SCOPE Normal : public AbstractBase, public NormalNode {
            friend class NormalClass;

            MFVec3f vector;

        public:
            Normal(const NodeType & nodeType,
                   const ScopePtr & scope);
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
            explicit NormalInterpolatorClass(Browser & browser);
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
            NormalInterpolator(const NodeType & nodeType,
                               const ScopePtr & scope);
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
            explicit OrientationInterpolatorClass(Browser & browser);
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
            OrientationInterpolator(const NodeType & nodeType,
                                    const ScopePtr & scope);
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
            explicit PixelTextureClass(Browser & browser);
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
            PixelTexture(const NodeType & nodeType,
                         const ScopePtr & scope);
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
            explicit PlaneSensorClass(Browser & browser);
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
            PlaneSensor(const NodeType & nodeType,
                        const ScopePtr & scope);
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
            explicit PointLightClass(Browser & browser);
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
            PointLight(const NodeType & nodeType,
                       const ScopePtr & scope);
            virtual ~PointLight() throw ();

            virtual PointLight * toPointLight() const;

            virtual void renderScoped(Viewer *);
        
        private:
            virtual void initializeImpl(double timestamp) throw ();
            
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
            explicit PointSetClass(Browser & browser);
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
            PointSet(const NodeType & nodeType,
                     const ScopePtr & scope);
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
            explicit PositionInterpolatorClass(Browser & browser);
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
            PositionInterpolator(const NodeType & nodeType,
                                 const ScopePtr & scope);
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
            explicit ProximitySensorClass(Browser & browser);
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
            ProximitySensor(const NodeType & nodeType,
                            const ScopePtr & scope);
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
            explicit ScalarInterpolatorClass(Browser & browser);
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
            ScalarInterpolator(const NodeType & nodeType,
                               const ScopePtr & scope);
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
            explicit ShapeClass(Browser & browser);
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
            Shape(const NodeType & nodeType,
                  const ScopePtr & scope);
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
            explicit SoundClass(Browser & browser);
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
            Sound(const NodeType & nodeType,
                  const ScopePtr & scope);
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
            explicit SphereClass(Browser & browser);
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
            Sphere(const NodeType & nodeType,
                   const ScopePtr & scope);
            virtual ~Sphere() throw ();

            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
            virtual const BVolume * getBVolume() const;
        };


        class OPENVRML_SCOPE SphereSensorClass : public NodeClass {
        public:
            explicit SphereSensorClass(Browser & browser);
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
            SphereSensor(const NodeType & nodeType,
                         const ScopePtr & scope);
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
            explicit SpotLightClass(Browser & browser);
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
            SpotLight(const NodeType & nodeType,
                      const ScopePtr & scope);
            virtual ~SpotLight() throw ();

            virtual SpotLight * toSpotLight() const;

            virtual void renderScoped(Viewer *);
        
        private:
            virtual void initializeImpl(double timestamp) throw ();
            
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
            explicit SwitchClass(Browser & browser);
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
            Switch(const NodeType & nodeType,
                   const ScopePtr & scope);
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
            FT_Library freeTypeLibrary;

            explicit TextClass(Browser & browser);
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

            struct GlyphGeometry {
                MFVec2f coord;
                MFInt32 coordIndex;
                float advanceWidth;
                float advanceHeight;
                
                GlyphGeometry(const std::vector<MFVec2f> & contours,
                              float advanceWidth, float advanceHeight)
                    throw (std::bad_alloc);
            };
            
            struct TextGeometry {
                MFVec3f coord;
                MFInt32 coordIndex;
                MFVec3f normal;
            };
            
            typedef std::vector<std::vector<FcChar32> > Ucs4String;
            typedef std::map<FT_UInt, GlyphGeometry> GlyphGeometryMap;
            
            Ucs4String ucs4String;
            FT_Face face;
            GlyphGeometryMap glyphGeometryMap;
            TextGeometry textGeometry;

        public:
            Text(const NodeType & nodeType,
                 const ScopePtr & scope);
            virtual ~Text() throw ();

            virtual bool isModified() const;
            virtual void updateModified(NodePath & path, int flags = 0x003);
            virtual void clearFlags();

            virtual Viewer::Object insertGeometry(Viewer *,
                                                  VrmlRenderContext rc);
        
        private:
            virtual void initializeImpl(double timestamp) throw ();
            
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

            void updateUcs4() throw (std::bad_alloc);
            void updateFace() throw (std::bad_alloc);
            void updateGeometry() throw (std::bad_alloc);
        };


        class OPENVRML_SCOPE TextureCoordinateClass : public NodeClass {
        public:
            explicit TextureCoordinateClass(Browser & browser);
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
            TextureCoordinate(const NodeType & nodeType,
                              const ScopePtr & scope);
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
            explicit TextureTransformClass(Browser & browser);
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
            TextureTransform(const NodeType & nodeType,
                             const ScopePtr & scope);
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
            explicit TimeSensorClass(Browser & browser);
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
            TimeSensor(const NodeType & nodeType,
                       const ScopePtr & scope);
            virtual ~TimeSensor() throw ();

            virtual TimeSensor * toTimeSensor() const;
            virtual const BVolume * getBVolume() const;

            void update(double time);
        
        private:
            virtual void initializeImpl(double timestamp) throw ();
            
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
            explicit TouchSensorClass(Browser & browser);
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
            TouchSensor(const NodeType & nodeType,
                        const ScopePtr & scope);
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
            explicit TransformClass(Browser & browser);
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
            Transform(const NodeType & nodeType,
                      const ScopePtr & scope);
            virtual ~Transform() throw ();

            virtual void render(Viewer * viewer, VrmlRenderContext rc);

            virtual void accumulateTransform(Node*);
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
            explicit ViewpointClass(Browser & browser);
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
            Viewpoint(const NodeType & nodeType,
                      const ScopePtr & scope);
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
            virtual void initializeImpl(double timestamp) throw ();
            
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
            explicit VisibilitySensorClass(Browser & browser);
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
            VisibilitySensor(const NodeType & nodeType,
                             const ScopePtr & scope);
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
            explicit WorldInfoClass(Browser & browser);
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
            WorldInfo(const NodeType & nodeType,
                      const ScopePtr & scope);
            virtual ~WorldInfo() throw ();
        };
    }
}

# endif
