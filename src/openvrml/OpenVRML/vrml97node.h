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

    class OPENVRML_SCOPE NodeChild : public Node {
    public:
        virtual NodeChild * toChild() const;

    protected:
        NodeChild(const NodeType & nodeType, VrmlScene *);
        NodeChild(const NodeChild & node);
    };


    class OPENVRML_SCOPE NodeGeometry : public Node {
    public:
        virtual ~NodeGeometry();

        virtual void render(Viewer *, VrmlRenderContext rc);
        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc) = 0;
        virtual NodeColor *color();
        virtual NodeGeometry * toGeometry() const;

    protected:
        NodeGeometry(const NodeType & nodeType, VrmlScene *);

        Viewer::Object d_viewerObject; // move to Node? ...
    };


    class OPENVRML_SCOPE NodeIndexedSet : public NodeGeometry {
    public:

        virtual ~NodeIndexedSet();

        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path);
        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        const SFNode & getColor() const;
        void setColor(const SFNode & color);

        const SFNode & getCoord() const;
        void setCoord(const SFNode & coord);

        virtual NodeColor *color();

        const MFInt32 & getCoordIndex() const;
        bool getColorPerVertex(){ return d_colorPerVertex.get(); }
        const MFInt32 & getColorIndex() const;

    protected:
        NodeIndexedSet(const NodeType & nodeType, VrmlScene *);

        SFNode d_color;
        MFInt32 d_colorIndex;
        SFBool d_colorPerVertex;

        SFNode d_coord;
        MFInt32 d_coordIndex;
    };


    class OPENVRML_SCOPE NodeLight : public NodeChild {
    public:
        virtual ~NodeLight();

        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual void renderScoped(Viewer *);
        virtual ostream& printFields(ostream& os, int indent);
        virtual NodeLight* toLight() const;

        float getAmbientIntensity() const { return d_ambientIntensity.get(); }
        float getIntensity() const { return d_intensity.get(); }
        bool getOn() const { return d_on.get(); }
        const float *getColor() const { return d_color.get(); }

    protected:
        NodeLight(const NodeType & nodeType, VrmlScene *);

        SFFloat d_ambientIntensity;
        SFColor d_color;
        SFFloat d_intensity;
        SFBool d_on;
    };


    class OPENVRML_SCOPE NodeTexture : public Node {
    public:
        virtual ~NodeTexture();

        virtual NodeTexture* toTexture() const;

        virtual size_t nComponents() = 0;
        virtual size_t width() = 0;
        virtual size_t height() = 0;
        virtual size_t nFrames() = 0;
        virtual const unsigned char * pixels() = 0;

        void addToScene(VrmlScene * scene, const std::string & relativeUrl)
          { d_scene = scene; d_relativeUrl.set( relativeUrl ); }

    protected:
        NodeTexture(const NodeType & nodeType, VrmlScene *s);

        SFString d_relativeUrl;
    };


    class OPENVRML_SCOPE NodeGroup : public NodeChild {
    public:
        // Define the fields of all built in group nodes
        static const NodeTypePtr defineType();

        NodeGroup(VrmlScene *s = 0);
        virtual ~NodeGroup();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
		             const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual NodeGroup* toGroup() const;

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path, int flags);
        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string & relativeUrl);

        virtual ostream& printFields(ostream& os, int indent);

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
        NodeGroup(const NodeType & nodeType, VrmlScene * scene);

        SFVec3f d_bboxCenter;
        SFVec3f d_bboxSize;
        MFNode d_children;
        SFString d_relative;
        Node *d_parentTransform;
        Viewer::Object d_viewerObject;

        /**
         * Cached copy of the bsphere enclosing this node's children.
         */
        BSphere d_bsphere;

        /**
         * Construct a bounding sphere around this node's children. Store it
         * in d_bsphere.
         */
        virtual void recalcBSphere();
    };


    class OPENVRML_SCOPE NodeAnchor : public NodeGroup {
        SFString d_description;
        MFString d_parameter;
        MFString d_url;

    public:
        // Define the built in NodeType:: "Anchor"
        static const NodeTypePtr defineType();

        NodeAnchor(VrmlScene*);
        NodeAnchor(const NodeAnchor&);
        virtual ~NodeAnchor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldName) const;
        virtual void setField(const std::string & fieldId,
			      const FieldValue & fieldValue);

        virtual NodeAnchor *toAnchor() const;

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        void activate();

        const std::string & description() { return d_description.get(); }
        const std::string & url() { assert(d_url.getLength() > 0); return d_url.getElement(0); }

        const BVolume * getBVolume() const;
    };


    class OPENVRML_SCOPE NodeAppearance : public Node {
    public:
        // Define the built in NodeType:: "Appearance"
        static const NodeTypePtr defineType();

        NodeAppearance(VrmlScene *);
        virtual ~NodeAppearance();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
			      const FieldValue & fieldValue);

        virtual NodeAppearance* toAppearance() const;

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path);
        virtual void clearFlags();	// Clear childrens flags too.

        virtual void addToScene(VrmlScene * scene, const std::string & relativeUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        const SFNode & getMaterial() const;
        void setMaterial(const SFNode & material);

        const SFNode & getTexture() const;
        void setTexture(const SFNode & texture);

        const SFNode & getTextureTransform() const;
        void setTextureTransform(const SFNode & textureTransform);

    protected:
        SFNode d_material;
        SFNode d_texture;
        SFNode d_textureTransform;
    };

    class OPENVRML_SCOPE NodeAudioClip : public Node {
        SFString d_description;
        SFBool d_loop;
        SFFloat d_pitch;
        SFTime d_startTime;
        SFTime d_stopTime;
        MFString d_url;

        SFString d_relativeUrl;

        SFTime d_duration;
        SFBool d_isActive;

        class Audio * d_audio;
        bool   d_url_modified;
        int    _audio_index;
        double _audio_intensity;
        int    _audio_fd;

    public:
        // Define the fields of AudioClip nodes
        static const NodeTypePtr defineType();

        NodeAudioClip(VrmlScene *);
        NodeAudioClip(const NodeAudioClip&);
        virtual ~NodeAudioClip();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual void addToScene(VrmlScene * scene, const std::string & relativeUrl);

        void update(SFTime & now);

        virtual NodeAudioClip* toAudioClip() const;

        virtual ostream& printFields(ostream& os, int indent);

    };


    class OPENVRML_SCOPE NodeBackground : public NodeChild {
        MFFloat d_groundAngle;
        MFColor d_groundColor;
        MFString d_backUrl;
        MFString d_bottomUrl;
        MFString d_frontUrl;
        MFString d_leftUrl;
        MFString d_rightUrl;
        MFString d_topUrl;
        MFFloat d_skyAngle;
        MFColor d_skyColor;
        SFBool d_isBound;

        SFString d_relativeUrl;

        // Texture caches
        Image *d_texPtr[6];
        Image d_tex[6];

        // Display list object for background
        Viewer::Object d_viewerObject;

    public:
        // Define the fields of Background nodes
        static const NodeTypePtr defineType();

        NodeBackground(VrmlScene *);
        virtual ~NodeBackground();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual NodeBackground* toBackground() const;

        virtual void addToScene(VrmlScene * scene, const std::string & relativeUrl);

        virtual ostream& printFields(ostream& os, int indent);

        // render backgrounds once per scene, not via the render() method
        void renderBindable(Viewer *);

        size_t nGroundAngles()	{ return d_groundAngle.getLength(); }
        const float * groundAngle()	{ return d_groundAngle.get(); }
        const float * groundColor()	{ return d_groundColor.get(); }

        size_t nSkyAngles()		{ return d_skyAngle.getLength(); }
        const float * skyAngle()	{ return d_skyAngle.get(); }
        const float * skyColor()	{ return d_skyColor.get(); }
    };


    class OPENVRML_SCOPE NodeBillboard : public NodeGroup {
        SFVec3f d_axisOfRotation;
        Node *d_parentTransform;
        Viewer::Object d_xformObject;

    public:
        // Define the fields of Billboard nodes
        static const NodeTypePtr defineType();
        static void billboard_to_matrix(const NodeBillboard* t_arg,
		                                const VrmlMatrix & MV, VrmlMatrix & M);

        NodeBillboard(VrmlScene *);
        virtual ~NodeBillboard();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual void accumulateTransform(Node*);
        virtual Node* getParentTransform();
        virtual void inverseTransform(Viewer *);
        virtual void inverseTransform(VrmlMatrix &);

        virtual NodeBillboard * toBillboard() const;

    };


    class OPENVRML_SCOPE NodeBox : public NodeGeometry {
        SFVec3f d_size;
        BSphere d_bsphere;

    public:
        // Define the fields of box nodes
        static const NodeTypePtr defineType();

        NodeBox(VrmlScene *);
        virtual ~NodeBox();

        virtual bool accept(NodeVisitor & visitor);
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        virtual NodeBox* toBox() const;
        const SFVec3f& getSize() const;

        const BVolume * getBVolume() const;

    };


    class OPENVRML_SCOPE NodeCollision : public NodeGroup {
        SFBool d_collide;
        SFNode d_proxy;
        SFTime d_collideTime;  // eventOut

    public:
        // Define the fields of Collision nodes
        static const NodeTypePtr defineType();

        NodeCollision(VrmlScene *);
        virtual ~NodeCollision();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual bool isModified() const;
        virtual void clearFlags();	// Clear childrens flags too.

        virtual void addToScene(VrmlScene * scene, const std::string & rel);

        virtual ostream& printFields(ostream& os, int indent);

        const SFNode & getProxy() const;
        void setProxy(const SFNode & proxy);

        virtual NodeCollision * toCollision() const;
    };


    class OPENVRML_SCOPE NodeColor : public Node {
        MFColor d_color;

    public:
        // Define the fields of Color nodes
        static const NodeTypePtr defineType();

        NodeColor(VrmlScene *);
        virtual ~NodeColor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeColor* toColor() const;

        virtual ostream& printFields(ostream& os, int indent);

        MFColor &color()	{ return d_color; }
    };


    class OPENVRML_SCOPE NodeColorInt : public NodeChild {
        // Fields
        MFFloat d_key;
        MFColor d_keyValue;

        // State
        SFColor d_value;

    public:
        // Define the fields of ColorInt nodes
        static const NodeTypePtr defineType();

        NodeColorInt( VrmlScene *scene = 0 );
        virtual ~NodeColorInt();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);
    };


    class OPENVRML_SCOPE NodeCone : public NodeGeometry {
        SFBool d_bottom;
        SFFloat d_bottomRadius;
        SFFloat d_height;
        SFBool d_side;

    public:
        // Define the fields of cone nodes
        static const NodeTypePtr defineType();

        NodeCone(VrmlScene *);
        virtual ~NodeCone();

        virtual bool accept(NodeVisitor & visitor);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeCone* toCone() const;
        bool getBottom() { return d_bottom.get(); }
        bool getSide() { return d_side.get(); }
        float getBottomRadius() { return d_bottomRadius.get(); }
        float getHeight() { return d_height.get(); }
    };


    class OPENVRML_SCOPE NodeCoordinate : public Node {
        MFVec3f d_point;

    public:
        // Define the fields of Coordinate nodes
        static const NodeTypePtr defineType();

        NodeCoordinate(VrmlScene *);
        virtual ~NodeCoordinate();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeCoordinate* toCoordinate() const;

        virtual ostream& printFields(ostream& os, int indent);

        MFVec3f &coordinate() { return d_point; }
    };


    class OPENVRML_SCOPE NodeCoordinateInt : public NodeChild {
        // Fields
        MFFloat d_key;
        MFVec3f d_keyValue;

        // State
        MFVec3f d_value;

    public:
        // Define the fields of CoordinateInt nodes
        static const NodeTypePtr defineType();

        NodeCoordinateInt( VrmlScene *scene = 0);
        virtual ~NodeCoordinateInt();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);
    };


    class OPENVRML_SCOPE NodeCylinder : public NodeGeometry {
        SFBool d_bottom;
        SFFloat d_height;
        SFFloat d_radius;
        SFBool d_side;
        SFBool d_top;

    public:
        // Define the fields of cylinder nodes
        static const NodeTypePtr defineType();

        NodeCylinder(VrmlScene *);
        virtual ~NodeCylinder();

        virtual bool accept(NodeVisitor & visitor);
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        virtual NodeCylinder* toCylinder() const;
        bool getBottom() { return d_bottom.get(); }
        bool getSide() { return d_side.get(); }
        bool getTop() { return d_top.get(); }
        float getHeight() { return d_height.get(); }
        float getRadius() { return d_radius.get(); }
    };


    class OPENVRML_SCOPE NodeCylinderSensor : public NodeChild {
        // Fields
        SFBool d_autoOffset;
        SFFloat d_diskAngle;
        SFBool d_enabled;
        SFFloat d_maxAngle;
        SFFloat d_minAngle;
        SFFloat d_offset;

        SFBool d_isActive;
        SFRotation d_rotation;
        SFVec3f d_trackPoint;
	    VrmlMatrix M;

    public:
        // Define the fields of CylinderSensor nodes
        static const NodeTypePtr defineType();

        NodeCylinderSensor( VrmlScene *scene = 0);
        virtual ~NodeCylinderSensor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeCylinderSensor* toCylinderSensor() const;

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer* v, VrmlRenderContext rc);
        void activate( double timeStamp, bool isActive, double *p );

        bool isEnabled() { return d_enabled.get(); }

    private:
        void setMVMatrix(const VrmlMatrix & M_in);
        const VrmlMatrix & getMVMatrix()const;
    };


    class OPENVRML_SCOPE NodeDirLight : public NodeLight {
        SFVec3f d_direction;

    public:
        // Define the fields of dirLight nodes
        static const NodeTypePtr defineType();

        NodeDirLight(VrmlScene *);
        virtual ~NodeDirLight();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual const SFVec3f& getDirection() const;  //LarryD Mar 04/99
        virtual NodeDirLight* toDirLight() const;  //LarryD Mar 04/99
    };


    class OPENVRML_SCOPE NodeElevationGrid : public NodeGeometry {
        SFNode d_color;
        SFNode d_normal;
        SFNode d_texCoord;
        SFBool d_ccw;
        SFBool d_colorPerVertex;
        SFFloat d_creaseAngle;
        MFFloat d_height;
        SFBool d_normalPerVertex;
        SFBool d_solid;
        SFInt32 d_xDimension;
        SFFloat d_xSpacing;
        SFInt32 d_zDimension;
        SFFloat d_zSpacing;

    public:
        // Define the fields of elevationGrid nodes
        static const NodeTypePtr defineType();

        NodeElevationGrid(VrmlScene *);
        virtual ~NodeElevationGrid();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path);
        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        virtual NodeElevationGrid * toElevationGrid() const; //LarryD Mar 09/99

        const SFNode & getColor() const;
        void setColor(const SFNode & color);

        const SFNode & getNormal() const;
        void setNormal(const SFNode & normal);

        const SFNode & getTexCoord() const;
        void setTexCoord(const SFNode & texCoord);

        const SFBool & getCcw() const;
        void setCcw(const SFBool & ccw);

        const SFBool & getColorPerVertex() const;
        void setColorPerVertex(const SFBool & colorPerVertex);

        const SFFloat & getCreaseAngle() const;
        void setCreaseAngle(const SFFloat & creaseAngle);

        const MFFloat & getHeight() const;
        void setHeight(const MFFloat & height);

        const SFBool & getNormalPerVertex() const;
        void setNormalPerVertex(const SFBool & normalPerVertex);

        const SFBool & getSolid() const;
        void setSolid(const SFBool & solid);

        const SFInt32 & getXDimension() const;
        void setXDimension(const SFInt32 & xDimension);

        const SFFloat & getXSpacing() const;
        void setXSpacing(const SFFloat & xSpacing);

        const SFInt32 & getZDimension() const;
        void setZDimension(const SFInt32 & zDimension);

        const SFFloat & getZSpacing() const;
        void setZSpacing(const SFFloat & zSpacing);
    };


    class OPENVRML_SCOPE NodeExtrusion : public NodeGeometry {
        SFBool d_beginCap;
        SFBool d_ccw;
        SFBool d_convex;
        SFFloat d_creaseAngle;
        MFVec2f d_crossSection;
        SFBool d_endCap;
        MFRotation d_orientation;
        MFVec2f d_scale;
        SFBool d_solid;
        MFVec3f d_spine;

    public:
        // Define the fields of extrusion nodes
        static const NodeTypePtr defineType();

        NodeExtrusion(VrmlScene *);
        virtual ~NodeExtrusion();

        virtual bool accept(NodeVisitor & visitor);
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        virtual NodeExtrusion* toExtrusion() const;

        bool getBeginCap(){ return d_beginCap.get(); }
        bool getCcw(){ return d_ccw.get(); }
        bool getConvex(){ return d_convex.get(); }  
        float getCreaseAngle(){return d_creaseAngle.get();}
        MFVec2f &getCrossSection() { return d_crossSection; }
        bool getEndCap(){ return d_endCap.get(); }
        MFRotation &getOrientation() { return d_orientation; } 
        MFVec2f &getScale() { return d_scale; } 
        bool getSolid(){ return d_solid.get();} 
        MFVec3f &getSpine() { return d_spine; } 
    };


    class OPENVRML_SCOPE NodeFog : public NodeChild {
        SFColor d_color;
        SFString d_fogType;
        SFFloat d_visibilityRange;
        SFBool d_isBound; // eventOut

    public:
        // Define the fields of Fog nodes
        static const NodeTypePtr defineType();

        NodeFog(VrmlScene *);
        virtual ~NodeFog();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual NodeFog* toFog() const;

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        const float *color() const	{ return d_color.get(); }
        const std::string & fogType() const	{ return d_fogType.get(); }
        float visibilityRange() const	{ return d_visibilityRange.get(); }
    };


    class OPENVRML_SCOPE NodeFontStyle : public Node {
        MFString d_family;
        SFBool d_horizontal;
        MFString d_justify;
        SFString d_language;
        SFBool d_leftToRight;
        SFFloat d_size;
        SFFloat d_spacing;
        SFString d_style;
        SFBool d_topToBottom;

    public:
        // Define the fields of FontStyle nodes
        static const NodeTypePtr defineType();

        NodeFontStyle(VrmlScene *);
        virtual ~NodeFontStyle();

        virtual bool accept(NodeVisitor & visitor);
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeFontStyle* toFontStyle() const;

        virtual ostream& printFields(ostream& os, int indent);

        MFString &justify() { return d_justify; }
        float size() { return d_size.get(); }
    };


    class OPENVRML_SCOPE NodeIFaceSet : public NodeIndexedSet {
        SFBool d_ccw;
        SFBool d_convex;
        SFFloat d_creaseAngle;
        SFNode d_normal;
        MFInt32 d_normalIndex;
        SFBool d_normalPerVertex;
        SFBool d_solid;
        SFNode d_texCoord;
        MFInt32 d_texCoordIndex;

        BSphere d_bsphere;

    public:
        // Define the fields of indexed face set nodes
        static const NodeTypePtr defineType();

        NodeIFaceSet(VrmlScene *);
        virtual ~NodeIFaceSet();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path, int flags);
        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *v, VrmlRenderContext rc);

        virtual NodeIFaceSet* toIFaceSet() const;

        const SFNode & getNormal() const;
        void setNormal(const SFNode & normal);

        virtual const MFInt32 & getNormalIndex() const;

        const SFNode & getTexCoord() const;
        void setTexCoord(const SFNode & texCoord);

        virtual const MFInt32 & getTexCoordIndex() const;

        bool getCcw(){ return d_ccw.get(); }
        bool getConvex(){ return d_convex.get(); }
        float getCreaseAngle(){ return d_creaseAngle.get();}
        bool getNormalPerVertex(){ return d_normalPerVertex.get();}
        bool getSolid(){ return d_solid.get();}

        const BVolume* getBVolume() const;

    private:
        void recalcBSphere();
    };


    class OPENVRML_SCOPE NodeILineSet : public NodeIndexedSet {
    public:
        // Define the fields of indexed line set nodes
        static const NodeTypePtr defineType();

        NodeILineSet(VrmlScene *);
        virtual ~NodeILineSet();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();

        virtual Viewer::Object insertGeometry(Viewer *v, VrmlRenderContext rc);
    };


    class OPENVRML_SCOPE NodeImageTexture : public NodeTexture {
        MFString d_url;
        SFBool d_repeatS;
        SFBool d_repeatT;

        Image *d_image;

        Viewer::TextureObject d_texObject;

    public:
        // Define the fields of ImageTexture nodes
        static const NodeTypePtr defineType();

        NodeImageTexture(VrmlScene *);
        virtual ~NodeImageTexture();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual size_t nComponents();
        virtual size_t width();
        virtual size_t height();
        virtual size_t nFrames();
        virtual const unsigned char * pixels();

        virtual const MFString& getUrl() const;

        virtual NodeImageTexture*	toImageTexture() const;

        bool getRepeatS() const;
        bool getRepeatT() const;
    };


    class OPENVRML_SCOPE NodeInline : public NodeGroup {
        MFString d_url;
        VrmlNamespace *d_namespace;
        bool d_hasLoaded;

    public:
        // Define the built in NodeType:: "Inline"
        static const NodeTypePtr defineType();

        NodeInline(VrmlScene*);
        virtual ~NodeInline();

        virtual bool accept(NodeVisitor & visitor);

        virtual NodeInline* toInline() const;

        virtual ostream& printFields(ostream& os, int indent);

        virtual void addToScene(VrmlScene * scene, const std::string & relativeUrl);

        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        void load(const std::string & relativeUrl);
    };


    class OPENVRML_SCOPE NodeLOD : public NodeChild {
        MFNode d_level;
        SFVec3f d_center;
        MFFloat d_range;

        /**
         * Cached copy of the bsphere enclosing this node's children.
         */
        BSphere d_bsphere;

    public:
        // Define the fields of all built in LOD nodes
        static const NodeTypePtr defineType();

        NodeLOD(VrmlScene *);
        virtual ~NodeLOD();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path);
        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual NodeLOD* toLOD() const;
        const MFNode & getLevel() const;
        void setLevel(const MFNode & level);
        virtual const MFFloat& getRange() const;
        virtual const SFVec3f& getCenter() const;

        const BVolume* getBVolume() const;

    private:
        /**
         * Construct a bounding sphere around this node's children. Store it
         * in d_bsphere.
         */
        virtual void recalcBSphere();
    };


    class OPENVRML_SCOPE NodeMaterial : public Node {
        SFFloat d_ambientIntensity;
        SFColor d_diffuseColor;
        SFColor d_emissiveColor;
        SFFloat d_shininess;
        SFColor d_specularColor;
        SFFloat d_transparency;

    public:
        // Define the fields of Material nodes
        static const NodeTypePtr defineType();

        NodeMaterial(VrmlScene *);
        virtual ~NodeMaterial();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeMaterial* toMaterial() const;

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        float ambientIntensity() const { return d_ambientIntensity.get(); }
        const float* diffuseColor() const { return d_diffuseColor.get(); }
        const float* emissiveColor() const { return d_emissiveColor.get(); }
        float shininess() const { return d_shininess.get(); }
        const float* specularColor() const { return d_specularColor.get(); }
        float  transparency() const { return d_transparency.get(); }
    };


    class OPENVRML_SCOPE NodeMovieTexture : public NodeTexture {
        SFBool d_loop;
        SFFloat d_speed;
        SFTime d_startTime;
        SFTime d_stopTime;

        MFString d_url;
        SFBool d_repeatS;
        SFBool d_repeatT;

        SFTime d_duration;
        SFBool d_isActive;

        Image *d_image;
        int d_frame, d_lastFrame;
        double d_lastFrameTime;

        Viewer::TextureObject d_texObject;

    public:
        // Define the fields of MovieTexture nodes
        static const NodeTypePtr defineType();

        NodeMovieTexture(VrmlScene *);
        virtual ~NodeMovieTexture();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual NodeMovieTexture* toMovieTexture() const;

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        void update( SFTime &now );

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual size_t nComponents();
        virtual size_t width();
        virtual size_t height();
        virtual size_t nFrames();
        virtual const unsigned char * pixels();
    };


    class OPENVRML_SCOPE NodeNavigationInfo : public NodeChild {
        MFFloat d_avatarSize;
        SFBool d_headlight;
        SFFloat d_speed;
        MFString d_type;
        SFFloat d_visibilityLimit;

        SFBool d_isBound;		// eventOut

    public:
        // Define the built in NodeType:: "NavigationInfo"
        static const NodeTypePtr defineType();

        NodeNavigationInfo(VrmlScene *scene);
        virtual ~NodeNavigationInfo();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual NodeNavigationInfo* toNavigationInfo() const;

        // Bindable nodes must notify the scene of their existence.
        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        const float * avatarSize()		{ return d_avatarSize.get(); }
        bool headlightOn()		{ return d_headlight.get(); }
        float speed()			{ return d_speed.get(); }
        float visibilityLimit()	{ return d_visibilityLimit.get(); }
    };


    class OPENVRML_SCOPE NodeNormal : public Node {
        MFVec3f d_vector;

    public:
        // Define the fields of Normal nodes
        static const NodeTypePtr defineType();

        NodeNormal(VrmlScene *);
        virtual ~NodeNormal();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeNormal* toNormal() const;

        virtual ostream& printFields(ostream& os, int indent);

        MFVec3f &normal()	{ return d_vector; }
    };


    class OPENVRML_SCOPE NodeNormalInt : public NodeChild {
        // Fields
        MFFloat d_key;
        MFVec3f d_keyValue;

        // State
        MFVec3f d_value;

    public:
        // Define the fields of NormalInt nodes
        static const NodeTypePtr defineType();

        NodeNormalInt( VrmlScene *scene = 0);
        virtual ~NodeNormalInt();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);
    };


    class OPENVRML_SCOPE NodeOrientationInt : public NodeChild {
        // Fields
        MFFloat d_key;
        MFRotation d_keyValue;

        // State
        SFRotation d_value;

    public:
        // Define the fields of OrientationInt nodes
        static const NodeTypePtr defineType();

        NodeOrientationInt( VrmlScene *scene = 0);
        virtual ~NodeOrientationInt();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual NodeOrientationInt* toOrientationInt() const;  
        const MFFloat& getKey() const;   
        const MFRotation& getKeyValue() const;   
    };


    class OPENVRML_SCOPE NodePixelTexture : public NodeTexture {
        SFImage d_image;
        SFBool d_repeatS;
        SFBool d_repeatT;

        Viewer::TextureObject d_texObject;

    public:
        // Define the fields of PixelTexture nodes
        static const NodeTypePtr defineType();

        NodePixelTexture(VrmlScene *);
        virtual ~NodePixelTexture();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual size_t nComponents();
        virtual size_t width();
        virtual size_t height();
        virtual size_t nFrames();
        virtual const unsigned char * pixels();

        virtual NodePixelTexture*	toPixelTexture() const;

        const SFImage & getImage() const;
        const SFBool & getRepeatS() const;
        const SFBool & getRepeatT() const;
    };


    /**
     * The PlaneSensor node maps pointing device motion into
     * two-dimensional translation in a plane parallel to the Z=0 plane of
     * the local coordinate system. The PlaneSensor node uses the
     * descendent geometry of its parent node to determine whether it is
     * liable to generate events
     */
    class OPENVRML_SCOPE NodePlaneSensor : public NodeChild {
        // Fields
        SFBool d_autoOffset;
        SFBool d_enabled;
        SFVec2f d_maxPosition;
        SFVec2f d_minPosition;
        SFVec3f d_offset;

        SFBool d_isActive;
        SFVec3f d_translation;
        SFVec3f d_trackPoint;

        SFVec3f d_activationPoint;

        Node *d_parentTransform;
        VrmlMatrix d_activationMatrix;
        VrmlMatrix M;

    public:
        // Define the fields of PlaneSensor nodes
        static const NodeTypePtr defineType();

        NodePlaneSensor( VrmlScene *scene = 0);
        virtual ~NodePlaneSensor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodePlaneSensor* toPlaneSensor() const;

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer* v, VrmlRenderContext rc);
        void activate( double timeStamp, bool isActive, double *p );

        virtual void accumulateTransform( Node* );
        virtual Node* getParentTransform();

        bool isEnabled() { return d_enabled.get(); }

    private:
        void setMVMatrix(const VrmlMatrix & M_in);
        const VrmlMatrix & getMVMatrix()const;
    };


    class OPENVRML_SCOPE NodePointLight : public NodeLight {
        SFVec3f d_attenuation;
        SFVec3f d_location;
        SFFloat d_radius;

    public:
        // Define the fields of pointLight nodes
        static const NodeTypePtr defineType();

        NodePointLight(VrmlScene *);
        virtual ~NodePointLight();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodePointLight* toPointLight() const;

        // Bindable/scoped nodes must notify the scene of their existence.
        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void renderScoped(Viewer *);

        const SFVec3f& getAttenuation() const;
        const SFVec3f& getLocation() const;
        float getRadius(){ return d_radius.get(); }
    };


    class OPENVRML_SCOPE NodePointSet : public NodeGeometry {
        SFNode d_color;
        SFNode d_coord;

        BSphere d_bsphere;

    public:
        // Define the fields of pointSet nodes
        static const NodeTypePtr defineType();

        NodePointSet(VrmlScene *);
        virtual ~NodePointSet();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual void updateModified(NodePath& path);
        virtual bool isModified() const;
        
        virtual NodePointSet * toPointSet() const;

        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        const SFNode & getColor() const;
        void setColor(const SFNode & color);

        const SFNode & getCoord() const;
        void setCoord(const SFNode & coord);

        const BVolume* getBVolume() const;

    private:
        void recalcBSphere();
    };


    class OPENVRML_SCOPE NodePositionInt : public NodeChild {
        // Fields
        MFFloat d_key;
        MFVec3f d_keyValue;

        // State
        SFVec3f d_value;

    public:
        // Define the fields of PositionInt nodes
        static const NodeTypePtr defineType();

        NodePositionInt( VrmlScene *scene = 0);
        virtual ~NodePositionInt();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual NodePositionInt* toPositionInt() const;  
        const MFFloat& getKey() const;   
        const MFVec3f& getKeyValue() const;   
    };


    class OPENVRML_SCOPE NodeProximitySensor : public NodeChild {
        // Fields
        SFVec3f d_center;
        SFBool d_enabled;
        SFVec3f d_size;

        // Internal state
        SFBool d_isActive;
        SFVec3f d_position;
        SFRotation d_orientation;
        SFTime d_enterTime;
        SFTime d_exitTime;

    public:
        // Define the fields of ProximitySensor nodes
        static const NodeTypePtr defineType();

        NodeProximitySensor( VrmlScene *scene = 0);
        virtual ~NodeProximitySensor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);
    };


    class OPENVRML_SCOPE NodeScalarInt : public NodeChild {
        // Fields
        MFFloat d_key;
        MFFloat d_keyValue;

        // State
        SFFloat d_value;

    public:
        // Define the fields of ScalarInt nodes
        static const NodeTypePtr defineType();

        NodeScalarInt( VrmlScene *scene = 0 );
        virtual ~NodeScalarInt();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual NodeScalarInt* toScalarInt() const;
        const MFFloat& getKey() const;
        const MFFloat& getKeyValue() const;
    };


    class OPENVRML_SCOPE NodeShape : public NodeChild {
        SFNode d_appearance;
        SFNode d_geometry;

        Viewer::Object d_viewerObject; // move to Node.h ? ...

    public:
        // Define the fields of Shape nodes
        static const NodeTypePtr defineType();

        NodeShape(VrmlScene *);
        virtual ~NodeShape();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path, int flags);
        virtual void clearFlags();
        const BVolume* getBVolume() const;

        virtual NodeShape* toShape()	const;

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        const SFNode & getAppearance() const;
        void setAppearance(const SFNode & appearance);

        const SFNode & getGeometry() const;
        void setGeometry(const SFNode & geometry);
    };


    class OPENVRML_SCOPE NodeSound : public NodeChild {
        SFVec3f d_direction;
        SFFloat d_intensity;
        SFVec3f d_location;
        SFFloat d_maxBack;
        SFFloat d_maxFront;
        SFFloat d_minBack;
        SFFloat d_minFront;
        SFFloat d_priority;
        SFNode d_source;
        SFBool d_spatialize;

    public:
        // Define the fields of Sound nodes
        static const NodeTypePtr defineType();

        NodeSound( VrmlScene *scene = 0 );
        virtual ~NodeSound();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual void updateModified(NodePath& path);
        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string &);

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual NodeSound* toSound() const;

        virtual ostream& printFields(ostream& os, int indent);

        const SFNode & getSource() const;
        void setSource(const SFNode & source);
    };


    class OPENVRML_SCOPE NodeSphere : public NodeGeometry {
    public:
        // Define the fields of sphere nodes
        static const NodeTypePtr defineType();

        NodeSphere(VrmlScene *);
        virtual ~NodeSphere();

        virtual bool accept(NodeVisitor & visitor);
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        virtual NodeSphere* toSphere() const;
        float getRadius() { return d_radius.get(); }

        const BVolume* getBVolume() const;

    protected:
        SFFloat d_radius;
        BSphere d_bsphere;
    };


    class OPENVRML_SCOPE NodeSphereSensor : public NodeChild {
        // Fields
        SFBool d_autoOffset;
        SFBool d_enabled;
        SFRotation d_offset;

        SFBool d_isActive;
        SFRotation d_rotation;
        SFVec3f d_trackPoint;

        SFVec3f d_activationPoint;
        SFVec3f d_centerPoint;
	    VrmlMatrix M;

    public:
        // Define the fields of SphereSensor nodes
        static const NodeTypePtr defineType();

        NodeSphereSensor(VrmlScene *scene = 0);
        virtual ~NodeSphereSensor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeSphereSensor * toSphereSensor() const;

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer* v, VrmlRenderContext rc);
        void activate( double timeStamp, bool isActive, double *p );

        bool isEnabled() { return d_enabled.get(); }

    private:
        void setMVMatrix(const VrmlMatrix & M_in);
        const VrmlMatrix & getMVMatrix()const;
    };


    class OPENVRML_SCOPE NodeSpotLight : public NodeLight {
        SFVec3f d_attenuation;
        SFFloat d_beamWidth;
        SFFloat d_cutOffAngle;
        SFVec3f d_direction;
        SFVec3f d_location;
        SFFloat d_radius;

    public:
        // Define the fields of spotLight nodes
        static const NodeTypePtr defineType();

        NodeSpotLight(VrmlScene *);
        virtual ~NodeSpotLight();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeSpotLight* toSpotLight() const;

        virtual void addToScene(VrmlScene * scene, const std::string &);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void renderScoped(Viewer *);

        const SFVec3f& getAttenuation() const;
        const SFVec3f& getDirection() const;
        const SFVec3f& getLocation() const;
        float getBeamWidth() { return d_beamWidth.get(); }
        float getCutOffAngle() { return d_cutOffAngle.get(); }
        float getRadius() { return d_radius.get(); }
    };


    class OPENVRML_SCOPE NodeSwitch : public NodeChild {
        MFNode d_choice;
        SFInt32 d_whichChoice;

        /**
         * Cached copy of the bsphere enclosing this node's children.
         */
        BSphere d_bsphere;

    public:
        // Define the fields of all built in switch nodes
        static const NodeTypePtr defineType();

        NodeSwitch(VrmlScene *);
        virtual ~NodeSwitch();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual NodeSwitch* toSwitch() const; //LarryD

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path);
        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        const MFNode & getChoice() const;
        void setChoice(const MFNode & choice);

        const SFInt32 & getWhichChoice() const;
        void setWhichChoice(const SFInt32 & choice);

        const BVolume* getBVolume() const;

    private:
        /**
         * Construct a bounding sphere around this node's children. Store it
         * in d_bsphere.
         */
        virtual void recalcBSphere();
    };


    class OPENVRML_SCOPE NodeText : public NodeGeometry {
        MFString d_string;
        SFNode d_fontStyle;
        MFFloat d_length;
        SFFloat d_maxExtent;

    public:
        // Define the fields of text nodes
        static const NodeTypePtr defineType();

        NodeText(VrmlScene *);
        virtual ~NodeText();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path);
        virtual void clearFlags();

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

        const SFNode & getFontStyle() const;
        void setFontStyle(const SFNode & fontStyle);
    };


    class OPENVRML_SCOPE NodeTextureCoordinate : public Node {
        MFVec2f d_point;

    public:
        // Define the fields of TextureCoordinate nodes
        static const NodeTypePtr defineType();

        NodeTextureCoordinate(VrmlScene *);
        virtual ~NodeTextureCoordinate();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeTextureCoordinate* toTextureCoordinate() const;

        virtual ostream& printFields(ostream& os, int indent);

        MFVec2f &coordinate()	{ return d_point; }
    };


    class OPENVRML_SCOPE NodeTextureTransform : public Node {
        SFVec2f d_center;
        SFFloat d_rotation;
        SFVec2f d_scale;
        SFVec2f d_translation;

    public:
        // Define the fields of TextureTransform nodes
        static const NodeTypePtr defineType();

        NodeTextureTransform(VrmlScene *);
        virtual ~NodeTextureTransform();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeTextureTransform* toTextureTransform() const;

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        const SFVec2f & getCenter() const;
        const SFFloat & getRotation() const;
        const SFVec2f & getScale() const;
        const SFVec2f & getTranslation() const;
    };


    class OPENVRML_SCOPE NodeTimeSensor : public NodeChild {
        // Fields
        SFTime d_cycleInterval;
        SFBool d_enabled;
        SFBool d_loop;
        SFTime d_startTime;
        SFTime d_stopTime;

        // Internal state
        SFBool d_isActive;
        double d_lastTime;

    public:
        // Define the fields of TimeSensor nodes
        static const NodeTypePtr defineType();

        NodeTimeSensor( VrmlScene *scene = 0);
        virtual ~NodeTimeSensor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual NodeTimeSensor* toTimeSensor() const;

        virtual void addToScene(VrmlScene * scene, const std::string &);

        virtual ostream& printFields(ostream& os, int indent);

        void update( SFTime &now );

        double getCycleInterval(){ return d_cycleInterval.get();} 
        bool getEnabled(){ return d_enabled.get();} 
        bool getLoop(){ return d_loop.get();} 
        double getStartTime(){ return d_startTime.get();} 
        double getStopTime(){ return d_stopTime.get();} 
        const BVolume* getBVolume() const;
    };


    class OPENVRML_SCOPE NodeTouchSensor : public NodeChild {
        // Fields
        SFBool d_enabled;

        // Internal state
        SFVec3f d_hitNormal;
        SFVec3f d_hitPoint;
        SFVec2f d_hitTexCoord;
        SFBool d_isActive;
        SFBool d_isOver;
        SFTime d_touchTime;

    public:
        // Define the fields of TouchSensor nodes
        static const NodeTypePtr defineType();

        NodeTouchSensor( VrmlScene *scene = 0);
        virtual ~NodeTouchSensor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual NodeTouchSensor* toTouchSensor() const;

        virtual ostream& printFields(ostream& os, int indent);

        void activate( double timeStamp, bool isOver, bool isActive, double *p );

        bool isEnabled() { return d_enabled.get(); }
    };


    class OPENVRML_SCOPE NodeTransform : public NodeGroup {
        SFVec3f d_center;
        SFRotation d_rotation;
        SFVec3f d_scale;
        SFRotation d_scaleOrientation;
        SFVec3f d_translation;
        Viewer::Object d_xformObject;
        VrmlMatrix M;
        bool M_dirty;

    public:
        // Define the fields of Transform nodes
        static const NodeTypePtr defineType();

        NodeTransform(VrmlScene *);
        virtual ~NodeTransform();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual void accumulateTransform(Node*);
        virtual void inverseTransform(Viewer *);
        virtual void inverseTransform(VrmlMatrix &);

        virtual NodeTransform* toTransform() const;
        const SFVec3f& getCenter() const;
        const SFRotation& getRotation() const;
        const SFVec3f& getScale() const;
        const SFRotation& getScaleOrientation() const;
        const SFVec3f& getTranslation() const;

        const BVolume * getBVolume() const;

        void getMatrix(VrmlMatrix & M) const;

    private:
        void recalcBSphere();
        void synch_cached_matrix();
    };


    class OPENVRML_SCOPE NodeViewpoint : public NodeChild {
        SFFloat d_fieldOfView;
        SFBool d_jump;
        SFRotation d_orientation;
        SFVec3f d_position;
        SFString d_description;

        SFBool d_isBound;
        SFTime d_bindTime;

        Node *d_parentTransform;

    public:
        // Define the fields of Viewpoint nodes
        static const NodeTypePtr defineType();

        NodeViewpoint(VrmlScene *);
        virtual ~NodeViewpoint();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
                             const FieldValue & fieldValue);

        virtual NodeViewpoint* toViewpoint() const;

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void accumulateTransform( Node* );
        virtual Node* getParentTransform();

        const SFFloat & getFieldOfView() const;
        const SFRotation & getOrientation() const;
        const SFVec3f & getPosition() const;
        const SFString & getDescription() const;

        const BVolume * getBVolume() const;

        void getInverseMatrix(VrmlMatrix & M) const;

        // XXX not implemented
        void getFrustum(VrmlFrustum& frust) const; // get a copy

        const std::string & description() { return d_description.get(); }
    };


    class OPENVRML_SCOPE NodeVisibilitySensor : public NodeChild {
        // Fields
        SFVec3f d_center;
        SFBool d_enabled;
        SFVec3f d_size;

        // Internal state
        SFBool d_isActive;
        SFTime d_enterTime;
        SFTime d_exitTime;

    public:
        // Define the fields of VisibilitySensor nodes
        static const NodeTypePtr defineType();

        NodeVisibilitySensor( VrmlScene *scene = 0);
        virtual ~NodeVisibilitySensor();

        virtual bool accept(NodeVisitor & visitor);
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        virtual void render(Viewer *, VrmlRenderContext rc);
    };


    class OPENVRML_SCOPE NodeWorldInfo : public NodeChild {
        MFString d_info;
        SFString d_title;

    public:
        // Define the built in NodeType:: "WorldInfo"
        static const NodeTypePtr defineType();

        NodeWorldInfo(VrmlScene *);
        virtual ~NodeWorldInfo();

        virtual bool accept(NodeVisitor & visitor);
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);

        virtual ostream& printFields(ostream& os, int indent);

        const std::string & title() { return d_title.get(); }
    };
}

# endif
