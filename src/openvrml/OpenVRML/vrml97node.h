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
#   include "VrmlNode.h"
#   include "Viewer.h"
#   include "Image.h"
#   include "VrmlBSphere.h"

class OPENVRML_SCOPE VrmlNodeChild : public VrmlNode {
public:
    static VrmlNodeType * defineType(VrmlNodeType *);

    virtual const VrmlNodeChild * toChild() const;
    virtual VrmlNodeChild * toChild();

protected:
    VrmlNodeChild(VrmlScene *);
    VrmlNodeChild(const VrmlNodeChild & node);
};


class OPENVRML_SCOPE VrmlNodeGeometry : public VrmlNode {
public:
    static VrmlNodeType *defineType(VrmlNodeType *t);

    VrmlNodeGeometry(VrmlScene *);
    virtual ~VrmlNodeGeometry();

    virtual void render(Viewer *, VrmlRenderContext rc);
    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc) = 0;
    virtual VrmlNodeColor *color();
    virtual const VrmlNodeGeometry * toGeometry() const;
    virtual VrmlNodeGeometry * toGeometry();

protected:
    Viewer::Object d_viewerObject; // move to VrmlNode? ...
};


class OPENVRML_SCOPE VrmlNodeLight : public VrmlNodeChild {
public:
    static VrmlNodeType * defineType(VrmlNodeType * = 0);

    VrmlNodeLight(VrmlScene *);

    virtual VrmlNodeType & nodeType() const;
    virtual const VrmlField * getField(const char * fieldName) const;
    virtual void setField(const char * fieldName, const VrmlField & fieldValue);
    virtual void renderScoped(Viewer *);
    virtual ostream& printFields(ostream& os, int indent);
    virtual VrmlNodeLight* toLight() const;

    virtual float getAmbientIntensity() const { return d_ambientIntensity.get(); } //LarryD Mar 04/99
    virtual float getIntensity() const { return d_intensity.get(); } //LarryD Mar 04/99
    virtual bool  getOn() const { return d_on.get(); } //LarryD Mar 04/99
    virtual const float *getColor() const { return d_color.get(); }  //LarryD Mar 04/99

protected:
    VrmlSFFloat d_ambientIntensity;
    VrmlSFColor d_color;
    VrmlSFFloat d_intensity;
    VrmlSFBool d_on;
};


class OPENVRML_SCOPE VrmlNodeGroup : public VrmlNodeChild {
public:
    // Define the fields of all built in group nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeGroup(VrmlScene *s = 0);
    virtual ~VrmlNodeGroup();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual VrmlNodeGroup* toGroup() const;

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path, int flags);
    virtual void clearFlags();

    virtual void addToScene( VrmlScene *s, const char *relativeUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);
    virtual void renderNoCull(Viewer *, VrmlRenderContext rc);

    virtual void accumulateTransform(VrmlNode*);

    void activate( double timeStamp, bool isOver, bool isActive, double *p );

    const VrmlMFNode & getChildren() const;
    void setChildren(const VrmlMFNode & children);
    void addChildren(const VrmlMFNode & children);
    void removeChildren(const VrmlMFNode & children);
    void removeChildren();

    virtual void eventIn(double timeStamp, const char * eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNode* getParentTransform();

    const VrmlBVolume* getBVolume() const;

protected:
    VrmlSFVec3f d_bboxCenter;
    VrmlSFVec3f d_bboxSize;
    VrmlMFNode d_children;
    VrmlSFString d_relative;
    VrmlNode *d_parentTransform;
    Viewer::Object d_viewerObject;

    /**
     * Cached copy of the bsphere enclosing this node's children.
     */
    VrmlBSphere d_bsphere;

    /**
     * Construct a bounding sphere around this node's children. Store it
     * in d_bsphere.
     */
    virtual void recalcBSphere();
};


class OPENVRML_SCOPE VrmlNodeAnchor : public VrmlNodeGroup {
public:
    // Define the built in VrmlNodeType:: "Anchor"
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeAnchor(VrmlScene*);
    VrmlNodeAnchor(const VrmlNodeAnchor&);
    virtual ~VrmlNodeAnchor();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeAnchor *toAnchor() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    void activate();

    virtual const VrmlField *getField(const char *fieldName) const;

    virtual void setField(const char *fieldName,
			  const VrmlField &fieldValue);

    const char *description() { return d_description.get(); }
    const char *url() { return d_url.getLength() > 0 ? d_url.getElement(0) : 0; }

    const VrmlBVolume* getBVolume() const;

protected:
    VrmlSFString d_description;
    VrmlMFString d_parameter;
    VrmlMFString d_url;
};


class OPENVRML_SCOPE VrmlNodeAppearance : public VrmlNode {
public:
    // Define the built in VrmlNodeType:: "Appearance"
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeAppearance(VrmlScene *);
    virtual ~VrmlNodeAppearance();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual VrmlNodeAppearance* toAppearance() const;

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path);
    virtual void clearFlags();	// Clear childrens flags too.

    virtual void addToScene( VrmlScene *s, const char *relativeUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    // Get a field or eventOut by name.
    virtual const VrmlField *getField(const char *fieldName) const;

    virtual void setField(const char *fieldName,
			  const VrmlField &fieldValue);

    const VrmlSFNode & getMaterial() const;
    void setMaterial(const VrmlSFNode & material);

    const VrmlSFNode & getTexture() const;
    void setTexture(const VrmlSFNode & texture);

    const VrmlSFNode & getTextureTransform() const;
    void setTextureTransform(const VrmlSFNode & textureTransform);

protected:
    VrmlSFNode d_material;
    VrmlSFNode d_texture;
    VrmlSFNode d_textureTransform;
};

class OPENVRML_SCOPE VrmlNodeAudioClip : public VrmlNode {
    VrmlSFString d_description;
    VrmlSFBool d_loop;
    VrmlSFFloat d_pitch;
    VrmlSFTime d_startTime;
    VrmlSFTime d_stopTime;
    VrmlMFString d_url;

    VrmlSFString d_relativeUrl;

    VrmlSFTime d_duration;
    VrmlSFBool d_isActive;

    class Audio * d_audio;
    bool   d_url_modified;
    int    _audio_index;
    double _audio_intensity;
    int    _audio_fd;

public:
    // Define the fields of AudioClip nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeAudioClip(VrmlScene *);
    VrmlNodeAudioClip(const VrmlNodeAudioClip&);
    virtual ~VrmlNodeAudioClip();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual void addToScene(VrmlScene *s, const char *relativeUrl);

    void update(VrmlSFTime & now);

    virtual VrmlNodeAudioClip* toAudioClip() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);
};


class OPENVRML_SCOPE VrmlNodeBackground : public VrmlNodeChild {
    VrmlMFFloat d_groundAngle;
    VrmlMFColor d_groundColor;
    VrmlMFString d_backUrl;
    VrmlMFString d_bottomUrl;
    VrmlMFString d_frontUrl;
    VrmlMFString d_leftUrl;
    VrmlMFString d_rightUrl;
    VrmlMFString d_topUrl;
    VrmlMFFloat d_skyAngle;
    VrmlMFColor d_skyColor;
    VrmlSFBool d_isBound;

    VrmlSFString d_relativeUrl;

    // Texture caches
    Image *d_texPtr[6];
    Image d_tex[6];

    // Display list object for background
    Viewer::Object d_viewerObject;

public:
    // Define the fields of Background nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);
    virtual VrmlNodeType & nodeType() const;

    VrmlNodeBackground(VrmlScene *);
    virtual ~VrmlNodeBackground();

    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeBackground* toBackground() const;

    virtual void addToScene( VrmlScene *s, const char *relativeUrl );

    virtual ostream& printFields(ostream& os, int indent);

    // render backgrounds once per scene, not via the render() method
    void renderBindable(Viewer *);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    size_t nGroundAngles()	{ return d_groundAngle.getLength(); }
    const float * groundAngle()	{ return d_groundAngle.get(); }
    const float * groundColor()	{ return d_groundColor.get(); }

    size_t nSkyAngles()		{ return d_skyAngle.getLength(); }
    const float * skyAngle()	{ return d_skyAngle.get(); }
    const float * skyColor()	{ return d_skyColor.get(); }
};


class OPENVRML_SCOPE VrmlNodeBillboard : public VrmlNodeGroup {
    VrmlSFVec3f d_axisOfRotation;
    VrmlNode *d_parentTransform;
    Viewer::Object d_xformObject;

public:
    // Define the fields of Billboard nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeBillboard(VrmlScene *);
    virtual ~VrmlNodeBillboard();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual void accumulateTransform(VrmlNode*);
    virtual VrmlNode* getParentTransform();
    virtual void inverseTransform(Viewer *);
    virtual void inverseTransform(double [4][4]);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual const VrmlNodeBillboard * toBillboard() const;
    virtual VrmlNodeBillboard * toBillboard();
};


class OPENVRML_SCOPE VrmlNodeBox : public VrmlNodeGeometry {
public:
    // Define the fields of box nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeBox(VrmlScene *);
    virtual ~VrmlNodeBox();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeBox* toBox() const; //LarryD Mar 08/99
    virtual const VrmlSFVec3f& getSize() const;  //LarryD Mar 08/99

    const VrmlBVolume* getBVolume() const;

protected:
    VrmlSFVec3f d_size;
    VrmlBSphere d_bsphere;
};


class OPENVRML_SCOPE VrmlNodeCollision : public VrmlNodeGroup {
    VrmlSFBool d_collide;
    VrmlSFNode d_proxy;
    VrmlSFTime d_collideTime;  // eventOut

public:
    // Define the fields of Collision nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeCollision(VrmlScene *);
    virtual ~VrmlNodeCollision();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual bool isModified() const;
    virtual void clearFlags();	// Clear childrens flags too.

    virtual void addToScene( VrmlScene *s, const char *rel );

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const VrmlSFNode & getProxy() const;
    void setProxy(const VrmlSFNode & proxy);

    virtual const VrmlNodeCollision * toCollision() const;
    virtual VrmlNodeCollision * toCollision();
};


class OPENVRML_SCOPE VrmlNodeColor : public VrmlNode {
    VrmlMFColor d_color;

public:
    // Define the fields of Color nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeColor(VrmlScene *);
    virtual ~VrmlNodeColor();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeColor* toColor() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    VrmlMFColor &color()	{ return d_color; }
};


class OPENVRML_SCOPE VrmlNodeColorInt : public VrmlNodeChild {
    // Fields
    VrmlMFFloat d_key;
    VrmlMFColor d_keyValue;

    // State
    VrmlSFColor d_value;

public:
    // Define the fields of ColorInt nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeColorInt( VrmlScene *scene = 0 );
    virtual ~VrmlNodeColorInt();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);
};


class OPENVRML_SCOPE VrmlNodeCone : public VrmlNodeGeometry {
public:
    // Define the fields of cone nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeCone(VrmlScene *);
    virtual ~VrmlNodeCone();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeCone* toCone() const; //LarryD Mar 08/99
    virtual bool getBottom() { return d_bottom.get(); } //LarryD Mar 08/99
    virtual bool getSide() { return d_side.get(); } //LarryD Mar 08/99
    virtual float getBottomRadius() { return d_bottomRadius.get(); } //LarryD Mar 08/99
    virtual float getHeight() { return d_height.get(); }  //LarryD Mar 08/99

protected:
    VrmlSFBool d_bottom;
    VrmlSFFloat d_bottomRadius;
    VrmlSFFloat d_height;
    VrmlSFBool d_side;
};


class OPENVRML_SCOPE VrmlNodeCoordinate : public VrmlNode {
    VrmlMFVec3f d_point;

public:
    // Define the fields of Coordinate nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeCoordinate(VrmlScene *);
    virtual ~VrmlNodeCoordinate();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeCoordinate* toCoordinate() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    VrmlMFVec3f &coordinate()	{ return d_point; }
};


class OPENVRML_SCOPE VrmlNodeCoordinateInt : public VrmlNodeChild {
    // Fields
    VrmlMFFloat d_key;
    VrmlMFVec3f d_keyValue;

    // State
    VrmlMFVec3f d_value;

public:
    // Define the fields of CoordinateInt nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeCoordinateInt( VrmlScene *scene = 0);
    virtual ~VrmlNodeCoordinateInt();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);
};


class OPENVRML_SCOPE VrmlNodeCylinder : public VrmlNodeGeometry {
public:
    // Define the fields of cylinder nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeCylinder(VrmlScene *);
    virtual ~VrmlNodeCylinder();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeCylinder* toCylinder() const; //LarryD Mar 08/99
    virtual bool getBottom() { return d_bottom.get(); }  //LarryD Mar 08/99
    virtual bool getSide() { return d_side.get(); }  //LarryD Mar 08/99
    virtual bool getTop() { return d_top.get(); }  //LarryD Mar 08/99
    virtual float getHeight() { return d_height.get(); }  //LarryD Mar 08/99
    virtual float getRadius() { return d_radius.get(); }  //LarryD Mar 08/99

protected:
    VrmlSFBool d_bottom;
    VrmlSFFloat d_height;
    VrmlSFFloat d_radius;
    VrmlSFBool d_side;
    VrmlSFBool d_top;
};


class OPENVRML_SCOPE VrmlNodeCylinderSensor : public VrmlNodeChild {
    // Fields
    VrmlSFBool d_autoOffset;
    VrmlSFFloat d_diskAngle;
    VrmlSFBool d_enabled;
    VrmlSFFloat d_maxAngle;
    VrmlSFFloat d_minAngle;
    VrmlSFFloat d_offset;

    VrmlSFBool d_isActive;
    VrmlSFRotation d_rotation;
    VrmlSFVec3f d_trackPoint;

public:
    // Define the fields of CylinderSensor nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeCylinderSensor( VrmlScene *scene = 0);
    virtual ~VrmlNodeCylinderSensor();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeCylinderSensor* toCylinderSensor() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    void activate( double timeStamp, bool isActive, double *p );

    bool isEnabled() { return d_enabled.get(); }
};


class OPENVRML_SCOPE VrmlNodeDirLight : public VrmlNodeLight {
public:
    // Define the fields of dirLight nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeDirLight(VrmlScene *);
    virtual ~VrmlNodeDirLight();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName,
			  const VrmlField &fieldValue);

    virtual const VrmlSFVec3f& getDirection() const;  //LarryD Mar 04/99
    virtual VrmlNodeDirLight* toDirLight() const;  //LarryD Mar 04/99

protected:
    VrmlSFVec3f d_direction;
};


class OPENVRML_SCOPE VrmlNodeElevationGrid : public VrmlNodeGeometry {
public:
    // Define the fields of elevationGrid nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);
    virtual VrmlNodeType & nodeType() const;

    VrmlNodeElevationGrid(VrmlScene *);
    virtual ~VrmlNodeElevationGrid();

    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path);
    virtual void clearFlags();

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField * getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeElevationGrid * toElevationGrid() const; //LarryD Mar 09/99

    const VrmlSFNode & getColor() const;
    void setColor(const VrmlSFNode & color);

    const VrmlSFNode & getNormal() const;
    void setNormal(const VrmlSFNode & normal);

    const VrmlSFNode & getTexCoord() const;
    void setTexCoord(const VrmlSFNode & texCoord);

    const VrmlSFBool & getCcw() const;
    void setCcw(const VrmlSFBool & ccw);

    const VrmlSFBool & getColorPerVertex() const;
    void setColorPerVertex(const VrmlSFBool & colorPerVertex);

    const VrmlSFFloat & getCreaseAngle() const;
    void setCreaseAngle(const VrmlSFFloat & creaseAngle);

    const VrmlMFFloat & getHeight() const;
    void setHeight(const VrmlMFFloat & height);

    const VrmlSFBool & getNormalPerVertex() const;
    void setNormalPerVertex(const VrmlSFBool & normalPerVertex);

    const VrmlSFBool & getSolid() const;
    void setSolid(const VrmlSFBool & solid);

    const VrmlSFInt32 & getXDimension() const;
    void setXDimension(const VrmlSFInt32 & xDimension);

    const VrmlSFFloat & getXSpacing() const;
    void setXSpacing(const VrmlSFFloat & xSpacing);

    const VrmlSFInt32 & getZDimension() const;
    void setZDimension(const VrmlSFInt32 & zDimension);

    const VrmlSFFloat & getZSpacing() const;
    void setZSpacing(const VrmlSFFloat & zSpacing);

protected:
    VrmlSFNode d_color;
    VrmlSFNode d_normal;
    VrmlSFNode d_texCoord;
    VrmlSFBool d_ccw;
    VrmlSFBool d_colorPerVertex;
    VrmlSFFloat d_creaseAngle;
    VrmlMFFloat d_height;
    VrmlSFBool d_normalPerVertex;
    VrmlSFBool d_solid;
    VrmlSFInt32 d_xDimension;
    VrmlSFFloat d_xSpacing;
    VrmlSFInt32 d_zDimension;
    VrmlSFFloat d_zSpacing;
};


class OPENVRML_SCOPE VrmlNodeExtrusion : public VrmlNodeGeometry {
public:
    // Define the fields of extrusion nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeExtrusion(VrmlScene *);
    virtual ~VrmlNodeExtrusion();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeExtrusion* toExtrusion() const;
    virtual bool getBeginCap(){ return d_beginCap.get(); }
    virtual bool getCcw(){ return d_ccw.get(); }
    virtual bool getConvex(){ return d_convex.get(); }  
    virtual float getCreaseAngle(){return d_creaseAngle.get();}
    virtual VrmlMFVec2f &getCrossSection() { return d_crossSection; }
    virtual bool getEndCap(){ return d_endCap.get(); }
    virtual VrmlMFRotation &getOrientation() { return d_orientation; } 
    virtual VrmlMFVec2f &getScale() { return d_scale; } 
    virtual bool getSolid(){ return d_solid.get();} 
    virtual VrmlMFVec3f &getSpine() { return d_spine; } 

protected:
    VrmlSFBool d_beginCap;
    VrmlSFBool d_ccw;
    VrmlSFBool d_convex;
    VrmlSFFloat d_creaseAngle;
    VrmlMFVec2f d_crossSection;
    VrmlSFBool d_endCap;
    VrmlMFRotation d_orientation;
    VrmlMFVec2f d_scale;
    VrmlSFBool d_solid;
    VrmlMFVec3f d_spine;
};


class OPENVRML_SCOPE VrmlNodeFog : public VrmlNodeChild {
    VrmlSFColor d_color;
    VrmlSFString d_fogType;
    VrmlSFFloat d_visibilityRange;
    VrmlSFBool d_isBound; // eventOut

public:
    // Define the fields of Fog nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeFog(VrmlScene *);
    virtual ~VrmlNodeFog();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeFog* toFog() const;

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    // Get a field or eventOut by name.
    virtual const VrmlField *getField(const char *fieldName) const;

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const float *color() const	{ return d_color.get(); }
    const char *fogType()	const	{ return d_fogType.get(); }
    float visibilityRange() const	{ return d_visibilityRange.get(); }
};


class OPENVRML_SCOPE VrmlNodeFontStyle : public VrmlNode {
    VrmlMFString d_family;
    VrmlSFBool d_horizontal;
    VrmlMFString d_justify;
    VrmlSFString d_language;
    VrmlSFBool d_leftToRight;
    VrmlSFFloat d_size;
    VrmlSFFloat d_spacing;
    VrmlSFString d_style;
    VrmlSFBool d_topToBottom;

public:
    // Define the fields of FontStyle nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeFontStyle(VrmlScene *);
    virtual ~VrmlNodeFontStyle();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeFontStyle* toFontStyle() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    VrmlMFString &justify() { return d_justify; }
    float size() { return d_size.get(); }
};


class OPENVRML_SCOPE VrmlNodeIndexedSet : public VrmlNodeGeometry {
public:
    // Define the fields of indexed face set nodes
    static VrmlNodeType *defineType(VrmlNodeType *t);

    VrmlNodeIndexedSet(VrmlScene *);
    virtual ~VrmlNodeIndexedSet();

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path);
    virtual void clearFlags();

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const VrmlSFNode & getColor() const;
    void setColor(const VrmlSFNode & color);

    const VrmlSFNode & getCoord() const;
    void setCoord(const VrmlSFNode & coord);

    virtual VrmlNodeColor *color();

    virtual const VrmlMFInt32 & getCoordIndex() const;
    virtual bool getColorPerVertex(){ return d_colorPerVertex.get(); } // LarryD  Feb18/99
    virtual const VrmlMFInt32 & getColorIndex() const; // LarryD  Feb18/99

protected:
    VrmlSFNode d_color;
    VrmlMFInt32 d_colorIndex;
    VrmlSFBool d_colorPerVertex;

    VrmlSFNode d_coord;
    VrmlMFInt32 d_coordIndex;
};


class OPENVRML_SCOPE VrmlNodeIFaceSet : public VrmlNodeIndexedSet {
public:
    // Define the fields of indexed face set nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeIFaceSet(VrmlScene *);
    virtual ~VrmlNodeIFaceSet();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path, int flags);
    virtual void clearFlags();

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *v, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeIFaceSet* toIFaceSet() const;

    const VrmlSFNode & getNormal() const;
    void setNormal(const VrmlSFNode & normal);

    virtual const VrmlMFInt32 & getNormalIndex() const;

    const VrmlSFNode & getTexCoord() const;
    void setTexCoord(const VrmlSFNode & texCoord);

    virtual const VrmlMFInt32 & getTexCoordIndex() const;

    virtual bool getCcw(){ return d_ccw.get(); }  // LarryD  Feb18/99
    virtual bool getConvex(){ return d_convex.get(); }   // LarryD Feb18/99
    virtual float getCreaseAngle(){ return d_creaseAngle.get();} // LarryD  Feb18/99
    virtual bool getNormalPerVertex(){ return d_normalPerVertex.get();} // LarryD  Feb18/99
    virtual bool getSolid(){ return d_solid.get();} // LarryD  Feb18/99

    const VrmlBVolume* getBVolume() const;

protected:
    VrmlSFBool d_ccw;
    VrmlSFBool d_convex;
    VrmlSFFloat d_creaseAngle;
    VrmlSFNode d_normal;
    VrmlMFInt32 d_normalIndex;
    VrmlSFBool d_normalPerVertex;
    VrmlSFBool d_solid;
    VrmlSFNode d_texCoord;
    VrmlMFInt32 d_texCoordIndex;

    VrmlBSphere d_bsphere;
    void recalcBSphere();
};


class OPENVRML_SCOPE VrmlNodeILineSet : public VrmlNodeIndexedSet {
public:
    // Define the fields of indexed line set nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeILineSet(VrmlScene *);
    virtual ~VrmlNodeILineSet();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual Viewer::Object insertGeometry(Viewer *v, VrmlRenderContext rc);
};


class OPENVRML_SCOPE VrmlNodeTexture : public VrmlNode {
public:
    // Define the fields of Texture nodes
    static VrmlNodeType * defineType(VrmlNodeType *t);

    VrmlNodeTexture(VrmlScene *s);
    virtual ~VrmlNodeTexture();

    virtual VrmlNodeTexture* toTexture() const;

    virtual size_t nComponents() = 0;
    virtual size_t width() = 0;
    virtual size_t height() = 0;
    virtual size_t nFrames() = 0;
    virtual const unsigned char * pixels() = 0;

    //similar to the VrmlNode calls, but placed here, cause they make more sense here.
    virtual VrmlNodePixelTexture*	toPixelTexture() const { return NULL; }
    virtual VrmlNodeImageTexture*	toImageTexture() const { return NULL; }

    void addToScene( VrmlScene *scene, const char *relativeUrl )
      { d_scene = scene; d_relativeUrl.set( relativeUrl ); }

protected:
    VrmlSFString d_relativeUrl;
};


class OPENVRML_SCOPE VrmlNodeImageTexture : public VrmlNodeTexture {
    VrmlMFString d_url;
    VrmlSFBool d_repeatS;
    VrmlSFBool d_repeatT;

    Image *d_image;

    Viewer::TextureObject d_texObject;

public:
    // Define the fields of ImageTexture nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeImageTexture(VrmlScene *);
    virtual ~VrmlNodeImageTexture();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual size_t nComponents();
    virtual size_t width();
    virtual size_t height();
    virtual size_t nFrames();
    virtual const unsigned char * pixels();

    virtual const VrmlMFString& getUrl() const;

    virtual VrmlNodeImageTexture*	toImageTexture() const;

    //
    // Aren't repeatS and repeatT shared by all texture nodes? Perhaps these
    // methods should be moved to the base class, VrmlNodeTexture. (Also, these
    // should probably return VrmlSFBool const references to be consistent with
    // the getUrl method.
    // -- Braden McDaniel, <braden@endoframe.com>, 11 Apr, 2000
    //
    virtual bool getRepeatS() const; // LarryD Feb18/99
    virtual bool getRepeatT() const; // LarryD Feb18/99
};


class OPENVRML_SCOPE VrmlNodeInline : public VrmlNodeGroup {
public:
    // Define the built in VrmlNodeType:: "Inline"
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeInline(VrmlScene*);
    virtual ~VrmlNodeInline();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeInline* toInline() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual void addToScene( VrmlScene *s, const char* relativeUrl );

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName,
			  const VrmlField &fieldValue);

    void load(const char *relativeUrl);

protected:
    VrmlMFString d_url;
    VrmlNamespace *d_namespace;
    bool d_hasLoaded;
};


class OPENVRML_SCOPE VrmlNodeLOD : public VrmlNodeChild {
public:
    // Define the fields of all built in LOD nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeLOD(VrmlScene *);
    virtual ~VrmlNodeLOD();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path);
    virtual void clearFlags();

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeLOD* toLOD() const;
    const VrmlMFNode & getLevel() const;
    void setLevel(const VrmlMFNode & level);
    virtual const VrmlMFFloat& getRange() const;
    virtual const VrmlSFVec3f& getCenter() const;

    const VrmlBVolume* getBVolume() const;

protected:
    VrmlMFNode d_level;
    VrmlSFVec3f d_center;
    VrmlMFFloat d_range;

    /**
     * Cached copy of the bsphere enclosing this node's children.
     */
    VrmlBSphere d_bsphere;

    /**
     * Construct a bounding sphere around this node's children. Store it
     * in d_bsphere.
     */
    virtual void recalcBSphere();
};


class OPENVRML_SCOPE VrmlNodeMaterial : public VrmlNode {
    VrmlSFFloat d_ambientIntensity;
    VrmlSFColor d_diffuseColor;
    VrmlSFColor d_emissiveColor;
    VrmlSFFloat d_shininess;
    VrmlSFColor d_specularColor;
    VrmlSFFloat d_transparency;

public:
    // Define the fields of Material nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeMaterial(VrmlScene *);
    virtual ~VrmlNodeMaterial();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeMaterial* toMaterial()	const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

          float  ambientIntensity() const { return d_ambientIntensity.get(); }
    const float* diffuseColor() const { return d_diffuseColor.get(); }
    const float* emissiveColor() const { return d_emissiveColor.get(); }
          float  shininess() const { return d_shininess.get(); }
    const float* specularColor() const { return d_specularColor.get(); }
          float  transparency() const { return d_transparency.get(); }
};


class OPENVRML_SCOPE VrmlNodeMovieTexture : public VrmlNodeTexture {
    VrmlSFBool d_loop;
    VrmlSFFloat d_speed;
    VrmlSFTime d_startTime;
    VrmlSFTime d_stopTime;

    VrmlMFString d_url;
    VrmlSFBool d_repeatS;
    VrmlSFBool d_repeatT;

    VrmlSFTime d_duration;
    VrmlSFBool d_isActive;

    Image *d_image;
    int d_frame, d_lastFrame;
    double d_lastFrameTime;

    Viewer::TextureObject d_texObject;

public:
    // Define the fields of MovieTexture nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeMovieTexture(VrmlScene *);
    virtual ~VrmlNodeMovieTexture();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeMovieTexture* toMovieTexture() const;

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    void update( VrmlSFTime &now );

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual void eventIn(double timeStamp, const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual size_t nComponents();
    virtual size_t width();
    virtual size_t height();
    virtual size_t nFrames();
    virtual const unsigned char * pixels();
};


class OPENVRML_SCOPE VrmlNodeNavigationInfo : public VrmlNodeChild {
    VrmlMFFloat d_avatarSize;
    VrmlSFBool d_headlight;
    VrmlSFFloat d_speed;
    VrmlMFString d_type;
    VrmlSFFloat d_visibilityLimit;

    VrmlSFBool d_isBound;		// eventOut

public:
    // Define the built in VrmlNodeType:: "NavigationInfo"
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeNavigationInfo(VrmlScene *scene);
    virtual ~VrmlNodeNavigationInfo();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeNavigationInfo* toNavigationInfo() const;

    // Bindable nodes must notify the scene of their existence.
    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    // Get a field or eventOut by name.
    virtual const VrmlField *getField(const char *fieldName) const;

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const float * avatarSize()		{ return d_avatarSize.get(); }
    bool headlightOn()		{ return d_headlight.get(); }
    float speed()			{ return d_speed.get(); }
    float visibilityLimit()	{ return d_visibilityLimit.get(); }
};


class OPENVRML_SCOPE VrmlNodeNormal : public VrmlNode {
    VrmlMFVec3f d_vector;

public:
    // Define the fields of Normal nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeNormal(VrmlScene *);
    virtual ~VrmlNodeNormal();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeNormal* toNormal() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    VrmlMFVec3f &normal()	{ return d_vector; }
};


class VrmlScene;

class OPENVRML_SCOPE VrmlNodeNormalInt : public VrmlNodeChild {
    // Fields
    VrmlMFFloat d_key;
    VrmlMFVec3f d_keyValue;

    // State
    VrmlMFVec3f d_value;

public:
    // Define the fields of NormalInt nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeNormalInt( VrmlScene *scene = 0);
    virtual ~VrmlNodeNormalInt();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);
};


class OPENVRML_SCOPE VrmlNodeOrientationInt : public VrmlNodeChild {
    // Fields
    VrmlMFFloat d_key;
    VrmlMFRotation d_keyValue;

    // State
    VrmlSFRotation d_value;

public:
    // Define the fields of OrientationInt nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeOrientationInt( VrmlScene *scene = 0);
    virtual ~VrmlNodeOrientationInt();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeOrientationInt* toOrientationInt() const;  
    virtual const VrmlMFFloat& getKey() const;   
    virtual const VrmlMFRotation& getKeyValue() const;   
};


class OPENVRML_SCOPE VrmlNodePixelTexture : public VrmlNodeTexture {
    VrmlSFImage d_image;
    VrmlSFBool d_repeatS;
    VrmlSFBool d_repeatT;

    Viewer::TextureObject d_texObject;

public:
    // Define the fields of PixelTexture nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodePixelTexture(VrmlScene *);
    virtual ~VrmlNodePixelTexture();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual size_t nComponents();
    virtual size_t width();
    virtual size_t height();
    virtual size_t nFrames();
    virtual const unsigned char * pixels();

    virtual VrmlNodePixelTexture*	toPixelTexture() const;

    const VrmlSFImage & getImage() const;
    const VrmlSFBool & getRepeatS() const;
    const VrmlSFBool & getRepeatT() const;
};


/**
 * The PlaneSensor node maps pointing device motion into
 * two-dimensional translation in a plane parallel to the Z=0 plane of
 * the local coordinate system. The PlaneSensor node uses the
 * descendent geometry of its parent node to determine whether it is
 * liable to generate events
 */
class OPENVRML_SCOPE VrmlNodePlaneSensor : public VrmlNodeChild {
public:
    // Define the fields of PlaneSensor nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodePlaneSensor( VrmlScene *scene = 0);
    virtual ~VrmlNodePlaneSensor();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodePlaneSensor* toPlaneSensor() const;

    virtual ostream& printFields(ostream& os, int indent);

    void activate( double timeStamp, bool isActive, double *p );

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual void accumulateTransform( VrmlNode* );
    virtual VrmlNode* getParentTransform();

    bool isEnabled() { return d_enabled.get(); }

protected:
    // Fields
    VrmlSFBool d_autoOffset;
    VrmlSFBool d_enabled;
    VrmlSFVec2f d_maxPosition;
    VrmlSFVec2f d_minPosition;
    VrmlSFVec3f d_offset;

    VrmlSFBool d_isActive;
    VrmlSFVec3f d_translation;
    VrmlSFVec3f d_trackPoint;

    VrmlSFVec3f d_activationPoint;

    VrmlNode *d_parentTransform;

    /**
     * Track the inverse transform matrix at the time of sensor
     * activation. Mouse deltas are relative to the original
     * transform. This matters because the transform stack on top of the
     * sensor may be modified by the output of the sensor.
     */
    double d_activationMatrix[4][4];
};


class OPENVRML_SCOPE VrmlNodePointLight : public VrmlNodeLight {
public:
    // Define the fields of pointLight nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodePointLight(VrmlScene *);
    virtual ~VrmlNodePointLight();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodePointLight* toPointLight() const;

    // Bindable/scoped nodes must notify the scene of their existence.
    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void renderScoped(Viewer *);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual const VrmlSFVec3f& getAttenuation() const;  //LarryD Mar 04/99
    virtual const VrmlSFVec3f& getLocation() const;  //LarryD Mar 04/99
    virtual float getRadius(){ return d_radius.get(); }    //LarryD Mar 04/99

protected:
    VrmlSFVec3f d_attenuation;
    VrmlSFVec3f d_location;
    VrmlSFFloat d_radius;
};


class OPENVRML_SCOPE VrmlNodePointSet : public VrmlNodeGeometry {
public:
    // Define the fields of pointSet nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodePointSet(VrmlScene *);
    virtual ~VrmlNodePointSet();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual void updateModified(VrmlNodePath& path);
    virtual bool isModified() const;

    virtual void clearFlags();

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const VrmlSFNode & getColor() const;
    void setColor(const VrmlSFNode & color);

    const VrmlSFNode & getCoord() const;
    void setCoord(const VrmlSFNode & coord);

    const VrmlBVolume* getBVolume() const;

protected:
    VrmlSFNode d_color;
    VrmlSFNode d_coord;

    VrmlBSphere d_bsphere;
    void recalcBSphere();
};


class OPENVRML_SCOPE VrmlNodePositionInt : public VrmlNodeChild {
    // Fields
    VrmlMFFloat d_key;
    VrmlMFVec3f d_keyValue;

    // State
    VrmlSFVec3f d_value;

public:
    // Define the fields of PositionInt nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodePositionInt( VrmlScene *scene = 0);
    virtual ~VrmlNodePositionInt();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodePositionInt* toPositionInt() const;  
    virtual const VrmlMFFloat& getKey() const;   
    virtual const VrmlMFVec3f& getKeyValue() const;   
};


class OPENVRML_SCOPE VrmlNodeProximitySensor : public VrmlNodeChild {
    // Fields
    VrmlSFVec3f d_center;
    VrmlSFBool d_enabled;
    VrmlSFVec3f d_size;

    // Internal state
    VrmlSFBool d_isActive;
    VrmlSFVec3f d_position;
    VrmlSFRotation d_orientation;
    VrmlSFTime d_enterTime;
    VrmlSFTime d_exitTime;

public:
    // Define the fields of ProximitySensor nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeProximitySensor( VrmlScene *scene = 0);
    virtual ~VrmlNodeProximitySensor();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);
};


class OPENVRML_SCOPE VrmlNodeScalarInt : public VrmlNodeChild {
    // Fields
    VrmlMFFloat d_key;
    VrmlMFFloat d_keyValue;

    // State
    VrmlSFFloat d_value;

public:
    // Define the fields of ScalarInt nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeScalarInt( VrmlScene *scene = 0 );
    virtual ~VrmlNodeScalarInt();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp, const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeScalarInt* toScalarInt() const;
    virtual const VrmlMFFloat& getKey() const;
    virtual const VrmlMFFloat& getKeyValue() const;
};


class OPENVRML_SCOPE VrmlNodeShape : public VrmlNodeChild {
    VrmlSFNode d_appearance;
    VrmlSFNode d_geometry;

    Viewer::Object d_viewerObject; // move to VrmlNode.h ? ...

public:
    // Define the fields of Shape nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeShape(VrmlScene *);
    virtual ~VrmlNodeShape();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path, int flags);
    virtual void clearFlags();
    const VrmlBVolume* getBVolume() const;

    virtual VrmlNodeShape* toShape()	const;

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const VrmlSFNode & getAppearance() const;
    void setAppearance(const VrmlSFNode & appearance);

    const VrmlSFNode & getGeometry() const;
    void setGeometry(const VrmlSFNode & geometry);
};


class OPENVRML_SCOPE VrmlNodeSound : public VrmlNodeChild {
    VrmlSFVec3f d_direction;
    VrmlSFFloat d_intensity;
    VrmlSFVec3f d_location;
    VrmlSFFloat d_maxBack;
    VrmlSFFloat d_maxFront;
    VrmlSFFloat d_minBack;
    VrmlSFFloat d_minFront;
    VrmlSFFloat d_priority;
    VrmlSFNode d_source;
    VrmlSFBool d_spatialize;

public:
    // Define the fields of Sound nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeSound( VrmlScene *scene = 0 );
    virtual ~VrmlNodeSound();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual void updateModified(VrmlNodePath& path);
    virtual void clearFlags();

    virtual void addToScene(VrmlScene *s, const char *);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual VrmlNodeSound* toSound() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const VrmlSFNode & getSource() const;
    void setSource(const VrmlSFNode & source);
};


class OPENVRML_SCOPE VrmlNodeSphere : public VrmlNodeGeometry {
public:
    // Define the fields of sphere nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeSphere(VrmlScene *);
    virtual ~VrmlNodeSphere();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual VrmlNodeSphere* toSphere() const; //LarryD Mar 08/99
    virtual float getRadius() { return d_radius.get(); }   //LarryD Mar 08/99

    const VrmlBVolume* getBVolume() const;

protected:
    VrmlSFFloat d_radius;
    VrmlBSphere d_bsphere;
};


class OPENVRML_SCOPE VrmlNodeSphereSensor : public VrmlNodeChild {
    // Fields
    VrmlSFBool d_autoOffset;
    VrmlSFBool d_enabled;
    VrmlSFRotation d_offset;

    VrmlSFBool d_isActive;
    VrmlSFRotation d_rotation;
    VrmlSFVec3f d_trackPoint;

    VrmlSFVec3f d_activationPoint;
    VrmlSFVec3f d_centerPoint;

public:
    // Define the fields of SphereSensor nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);
    virtual VrmlNodeType & nodeType() const;

    VrmlNodeSphereSensor(VrmlScene *scene = 0);
    virtual ~VrmlNodeSphereSensor();

    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeSphereSensor * toSphereSensor() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    void activate( double timeStamp, bool isActive, double *p );

    bool isEnabled() { return d_enabled.get(); }
};


class OPENVRML_SCOPE VrmlNodeSpotLight : public VrmlNodeLight {
public:
    // Define the fields of spotLight nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeSpotLight(VrmlScene *);
    virtual ~VrmlNodeSpotLight();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeSpotLight* toSpotLight() const;

    virtual void addToScene( VrmlScene *s, const char * );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void renderScoped(Viewer *);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual const VrmlSFVec3f& getAttenuation() const;  //LarryD Mar 04/99
    virtual const VrmlSFVec3f& getDirection() const;  //LarryD Mar 04/99
    virtual const VrmlSFVec3f& getLocation() const;  //LarryD Mar 04/99
    virtual float getBeamWidth() { return d_beamWidth.get(); }  //LarryD Mar 04/99
    virtual float getCutOffAngle() { return d_cutOffAngle.get(); } //LarryD Mar 04/99
    virtual float getRadius() { return d_radius.get(); }  //LarryD Mar 04/99

protected:
    VrmlSFVec3f d_attenuation;
    VrmlSFFloat d_beamWidth;
    VrmlSFFloat d_cutOffAngle;
    VrmlSFVec3f d_direction;
    VrmlSFVec3f d_location;
    VrmlSFFloat d_radius;
};


class OPENVRML_SCOPE VrmlNodeSwitch : public VrmlNodeChild {
public:
    // Define the fields of all built in switch nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeSwitch(VrmlScene *);
    virtual ~VrmlNodeSwitch();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual VrmlNodeSwitch* toSwitch() const; //LarryD

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path);
    virtual void clearFlags();

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const VrmlMFNode & getChoice() const;
    void setChoice(const VrmlMFNode & choice);

    const VrmlSFInt32 & getWhichChoice() const;
    void setWhichChoice(const VrmlSFInt32 & choice);

    const VrmlBVolume* getBVolume() const;

protected:
    VrmlMFNode d_choice;
    VrmlSFInt32 d_whichChoice;

    /**
     * Cached copy of the bsphere enclosing this node's children.
     */
    VrmlBSphere d_bsphere;

    /**
     * Construct a bounding sphere around this node's children. Store it
     * in d_bsphere.
     */
    virtual void recalcBSphere();
};


class OPENVRML_SCOPE VrmlNodeText : public VrmlNodeGeometry {
public:
    // Define the fields of text nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeText(VrmlScene *);
    virtual ~VrmlNodeText();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path);
    virtual void clearFlags();

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const VrmlSFNode & getFontStyle() const;
    void setFontStyle(const VrmlSFNode & fontStyle);

protected:
    VrmlMFString d_string;
    VrmlSFNode d_fontStyle;
    VrmlMFFloat d_length;
    VrmlSFFloat d_maxExtent;
};


class OPENVRML_SCOPE VrmlNodeTextureCoordinate : public VrmlNode {
    VrmlMFVec2f d_point;

public:
    // Define the fields of TextureCoordinate nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeTextureCoordinate(VrmlScene *);
    virtual ~VrmlNodeTextureCoordinate();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeTextureCoordinate* toTextureCoordinate() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    VrmlMFVec2f &coordinate()	{ return d_point; }
};


class OPENVRML_SCOPE VrmlNodeTextureTransform : public VrmlNode {
    VrmlSFVec2f d_center;
    VrmlSFFloat d_rotation;
    VrmlSFVec2f d_scale;
    VrmlSFVec2f d_translation;

public:
    // Define the fields of TextureTransform nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeTextureTransform(VrmlScene *);
    virtual ~VrmlNodeTextureTransform();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeTextureTransform* toTextureTransform() const;

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const VrmlSFVec2f & getCenter() const;
    const VrmlSFFloat & getRotation() const;
    const VrmlSFVec2f & getScale() const;
    const VrmlSFVec2f & getTranslation() const;
};


class OPENVRML_SCOPE VrmlNodeTimeSensor : public VrmlNodeChild {
    // Fields
    VrmlSFTime d_cycleInterval;
    VrmlSFBool d_enabled;
    VrmlSFBool d_loop;
    VrmlSFTime d_startTime;
    VrmlSFTime d_stopTime;

    // Internal state
    VrmlSFBool d_isActive;
    double d_lastTime;

public:
    // Define the fields of TimeSensor nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeTimeSensor( VrmlScene *scene = 0);
    virtual ~VrmlNodeTimeSensor();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeTimeSensor* toTimeSensor() const;

    virtual void addToScene( VrmlScene *s, const char* );

    virtual ostream& printFields(ostream& os, int indent);

    void update( VrmlSFTime &now );

    virtual void eventIn(double timeStamp, const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual double getCycleInterval(){ return d_cycleInterval.get();} 
    virtual bool getEnabled(){ return d_enabled.get();} 
    virtual bool getLoop(){ return d_loop.get();} 
    virtual double getStartTime(){ return d_startTime.get();} 
    virtual double getStopTime(){ return d_stopTime.get();} 
    const VrmlBVolume* getBVolume() const;
};


class OPENVRML_SCOPE VrmlNodeTouchSensor : public VrmlNodeChild {
    // Fields
    VrmlSFBool d_enabled;

    // Internal state
    VrmlSFVec3f d_hitNormal;
    VrmlSFVec3f d_hitPoint;
    VrmlSFVec2f d_hitTexCoord;
    VrmlSFBool d_isActive;
    VrmlSFBool d_isOver;
    VrmlSFTime d_touchTime;

public:
    // Define the fields of TouchSensor nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeTouchSensor( VrmlScene *scene = 0);
    virtual ~VrmlNodeTouchSensor();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeTouchSensor* toTouchSensor() const;

    virtual ostream& printFields(ostream& os, int indent);

    void activate( double timeStamp, bool isOver, bool isActive, double *p );

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    bool isEnabled() { return d_enabled.get(); }
};

/**
 * VrmlNodeTransform.
 */
class OPENVRML_SCOPE VrmlNodeTransform : public VrmlNodeGroup {
public:
    // Define the fields of Transform nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeTransform(VrmlScene *);
    virtual ~VrmlNodeTransform();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual void accumulateTransform(VrmlNode*);
    virtual void inverseTransform(Viewer *);
    virtual void inverseTransform(double [4][4]);

    virtual VrmlNodeTransform* toTransform() const;     //LarryD Feb 24/99
    virtual const VrmlSFVec3f& getCenter() const;  //LarryD Feb 24/99
    virtual const VrmlSFRotation& getRotation() const;  //LarryD Feb 24/99
    virtual const VrmlSFVec3f& getScale() const;  //LarryD Feb 24/99
    virtual const VrmlSFRotation& getScaleOrientation() const;  //LarryD Feb 24/99
    virtual const VrmlSFVec3f& getTranslation() const;  //LarryD Feb 

    const VrmlBVolume* getBVolume() const;

    /**
     * Get a matrix representation (in MathUtils format) of the
     * transformation stored in the node fields.
     *
     * @return a copy of the cached transformation matrix
     */
    void getMatrix(double M[4][4]) const;


    /**
     * Take the fields of this transform, and calculate the matching
     * transformation matrix. Store a copy in M. Should this be
     * protected?
     *
     * @param t a transformation node
     * @param flag 0 means calculate transform, 1 means calculate
     *               inverse transform
     * @param M gets a copy of the resulting transform
     */
    static void transform_to_matrix(const VrmlNodeTransform* t_arg, int flag, double M[4][4]);

protected:
    VrmlSFVec3f d_center;
    VrmlSFRotation d_rotation;
    VrmlSFVec3f d_scale;
    VrmlSFRotation d_scaleOrientation;
    VrmlSFVec3f d_translation;
    Viewer::Object d_xformObject;

    void recalcBSphere();

    /**
     * Resynchronize the cached matrix <code>M</code> with the node
     * fields, but only if M_dirty is true. Think logical const.
     */
    void synch_cached_matrix();

    /**
     * Cached copy (in MathUtils format) of this node's transformation.
     * Currently this is used only by the culling code, but eventually
     * all the matrix manipulation needs to be moved from the Viewer
     * side over into core.
     */
    double M[4][4];

    /**
     * If true, we need to recalculate M. Is this the same as
     * VrmlNode::d_modified? No, since it's entirely a core-side issue,
     * and has nothing to do with the viewer being out of date wrt the
     * core scene graph.
     */
    bool M_dirty;
};


class OPENVRML_SCOPE VrmlNodeViewpoint : public VrmlNodeChild {
    VrmlSFFloat d_fieldOfView;
    VrmlSFBool d_jump;
    VrmlSFRotation d_orientation;
    VrmlSFVec3f d_position;
    VrmlSFString d_description;


    VrmlSFBool d_isBound;
    VrmlSFTime d_bindTime;

    VrmlNode *d_parentTransform;

public:
    // Define the fields of Viewpoint nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeViewpoint(VrmlScene *);
    virtual ~VrmlNodeViewpoint();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual VrmlNodeViewpoint* toViewpoint() const;

    virtual void addToScene( VrmlScene *s, const char *relUrl );

    virtual ostream& printFields(ostream& os, int indent);

    virtual void eventIn(double timeStamp,
		         const char *eventName,
		         const VrmlField & fieldValue);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    virtual void accumulateTransform( VrmlNode* );
    virtual VrmlNode* getParentTransform();

    const VrmlSFFloat & getFieldOfView() const;
    const VrmlSFRotation & getOrientation() const;
    const VrmlSFVec3f & getPosition() const;
    const VrmlSFString & getDescription() const;

    const VrmlBVolume* getBVolume() const;

    /**
     * Determine the inverse of the transform represented by the
     * viewpoint's position and orientation fields. Return the matrix in
     * MathUtils format. Note that this method deals only with the
     * viewpoint node's transform, not with any ancestor transforms.
     *
     * @param IM inverse of the position/orientation transform
     *
     * @see VrmlNode::inverseTransform
     */
    void getInverseMatrix(double IM[4][4]) const;

    // not implemented
    void getFrustum(VrmlFrustum& frust) const; // get a copy

    const char *description() { return d_description.get() ? d_description.get() : ""; }
};


class OPENVRML_SCOPE VrmlNodeVisibilitySensor : public VrmlNodeChild {
    // Fields
    VrmlSFVec3f d_center;
    VrmlSFBool d_enabled;
    VrmlSFVec3f d_size;

    // Internal state
    VrmlSFBool d_isActive;
    VrmlSFTime d_enterTime;
    VrmlSFTime d_exitTime;

public:
    // Define the fields of VisibilitySensor nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeVisibilitySensor( VrmlScene *scene = 0);
    virtual ~VrmlNodeVisibilitySensor();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual const VrmlField *getField(const char *fieldName) const;
    virtual void setField(const char *fieldName, const VrmlField &fieldValue);
};


class OPENVRML_SCOPE VrmlNodeWorldInfo : public VrmlNodeChild {
    VrmlMFString d_info;
    VrmlSFString d_title;

public:
    // Define the built in VrmlNodeType:: "WorldInfo"
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);

    VrmlNodeWorldInfo(VrmlScene *);
    virtual ~VrmlNodeWorldInfo();

    virtual VrmlNodeType & nodeType() const;
    virtual bool accept(VrmlNodeVisitor & visitor);

    virtual ostream& printFields(ostream& os, int indent);

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);

    const char* title() { return d_title.get(); }

    const char * const * info() { return d_info.get(); }

    size_t size() { return d_info.getLength(); }
};

# endif
