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

#ifndef VRMLNODEPROTO_H
#define VRMLNODEPROTO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "VrmlNode.h"
#include "nodetype.h"
#include "Viewer.h"
#include "field.h"

class OPENVRML_SCOPE VrmlNodeProto : public VrmlNode {
public:
    class NameValueRec;

private:
    bool d_instantiated;
    VrmlNamespace * d_scope;
    VrmlMFNode implNodes;

    std::list<NameValueRec *> d_fields;

    class EventDispatch;
    std::list<EventDispatch *> d_eventDispatch;

    Viewer::Object d_viewerObject;

public:
    VrmlNodeProto(const NodeType & nodeDef, VrmlScene *scene);
    VrmlNodeProto(const VrmlNodeProto&);
    virtual ~VrmlNodeProto();

    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();
    virtual const VrmlField * getField(const std::string & fieldName) const;
    virtual void setField(const std::string & fieldName, const VrmlField &fieldValue);
    virtual void eventIn(double timeStamp, const std::string & eventName,
		         const VrmlField & fieldValue);
    virtual const VrmlMFNode getChildren() const;

    virtual void addToScene(VrmlScene * scene, const std::string & relUrl);
    virtual ostream & printFields(ostream & os, int indent);

    virtual VrmlNodeAnchor * toAnchor() const;
    virtual VrmlNodeAppearance * toAppearance() const;
    virtual VrmlNodeAudioClip * toAudioClip() const;
    virtual VrmlNodeBackground * toBackground() const;
    virtual VrmlNodeBillboard * toBillboard() const;
    virtual VrmlNodeBox * toBox() const;
    virtual VrmlNodeChild * toChild() const;
    virtual VrmlNodeCollision * toCollision() const;
    virtual VrmlNodeColor * toColor() const;
    virtual VrmlNodeCone * toCone() const;
    virtual VrmlNodeCoordinate * toCoordinate() const;
    virtual VrmlNodeCylinder * toCylinder() const;
    virtual VrmlNodeDirLight * toDirLight() const;
    virtual VrmlNodeElevationGrid * toElevationGrid() const;
    virtual VrmlNodeExtrusion * toExtrusion() const;
    virtual VrmlNodeFog * toFog() const;
    virtual VrmlNodeFontStyle * toFontStyle() const;
    virtual VrmlNodeGeometry * toGeometry() const;
    virtual VrmlNodeGroup * toGroup() const;
    virtual VrmlNodeIFaceSet * toIFaceSet() const;
    virtual VrmlNodeImageTexture * toImageTexture() const;
    virtual VrmlNodePixelTexture * toPixelTexture() const;
    virtual VrmlNodeInline * toInline() const;
    virtual VrmlNodeLOD * toLOD() const;
    virtual VrmlNodeLight * toLight() const;
    virtual VrmlNodeMaterial * toMaterial() const;
    virtual VrmlNodeMovieTexture * toMovieTexture() const;
    virtual VrmlNodeNavigationInfo * toNavigationInfo() const;
    virtual VrmlNodeNormal * toNormal() const;
    virtual VrmlNodeOrientationInt * toOrientationInt() const;
    virtual VrmlNodePlaneSensor * toPlaneSensor() const;
    virtual VrmlNodePositionInt * toPositionInt() const;
    virtual VrmlNodeSphereSensor * toSphereSensor() const;
    virtual VrmlNodeCylinderSensor * toCylinderSensor() const;
    virtual VrmlNodePointLight * toPointLight() const;
    virtual VrmlNodeScalarInt * toScalarInt() const;
    virtual VrmlNodeScript * toScript() const;
    virtual VrmlNodeShape * toShape() const;
    virtual VrmlNodeSphere * toSphere() const;
    virtual VrmlNodeSound * toSound() const;
    virtual VrmlNodeSpotLight * toSpotLight() const;
    virtual VrmlNodeSwitch * toSwitch() const;
    virtual VrmlNodeTexture * toTexture() const;
    virtual VrmlNodeTextureCoordinate * toTextureCoordinate() const;
    virtual VrmlNodeTextureTransform * toTextureTransform() const;
    virtual VrmlNodeTimeSensor * toTimeSensor() const;
    virtual VrmlNodeTouchSensor * toTouchSensor() const;
    virtual VrmlNodeTransform * toTransform() const;
    virtual VrmlNodeViewpoint * toViewpoint() const;
    virtual VrmlNodeProto * toProto() const;

    virtual void render(Viewer *, VrmlRenderContext rc);

    virtual bool isModified() const;
    virtual void updateModified(VrmlNodePath& path, int flags);

    virtual void accumulateTransform( VrmlNode* );

    const VrmlMFNode & getImplNodes() const;

    const VrmlBVolume* getBVolume() const;

private:
    const VrmlNodePtr firstNode() const;
    void instantiate();
    NameValueRec * findField(const std::string & fieldName) const;
};

#endif
