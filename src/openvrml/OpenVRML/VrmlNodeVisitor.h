//
// OpenVRML
//
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

# ifndef VRMLNODEVISITOR_H
#   define VRMLNODEVISITOR_H

#   include "common.h"

class VrmlNodeAnchor;
class VrmlNodeAppearance;
class VrmlNodeAudioClip;
class VrmlNodeBackground;
class VrmlNodeBillboard;
class VrmlNodeBox;
class VrmlNodeCollision;
class VrmlNodeColor;
class VrmlNodeColorInt;
class VrmlNodeCone;
class VrmlNodeCoordinate;
class VrmlNodeCoordinateInt;
class VrmlNodeCylinder;
class VrmlNodeCylinderSensor;
class VrmlNodeDirLight;
class VrmlNodeElevationGrid;
class VrmlNodeExtrusion;
class VrmlNodeFog;
class VrmlNodeFontStyle;
class VrmlNodeGroup;
class VrmlNodeIFaceSet;
class VrmlNodeILineSet;
class VrmlNodeImageTexture;
class VrmlNodeInline;
class VrmlNodeLOD;
class VrmlNodeMaterial;
class VrmlNodeMovieTexture;
class VrmlNodeNavigationInfo;
class VrmlNodeNormal;
class VrmlNodeNormalInt;
class VrmlNodeOrientationInt;
class VrmlNodePixelTexture;
class VrmlNodePlaneSensor;
class VrmlNodePointLight;
class VrmlNodePointSet;
class VrmlNodePositionInt;
class VrmlNodeProto;
class VrmlNodeProximitySensor;
class VrmlNodeScalarInt;
class VrmlNodeScript;
class VrmlNodeShape;
class VrmlNodeSound;
class VrmlNodeSphere;
class VrmlNodeSphereSensor;
class VrmlNodeSpotLight;
class VrmlNodeSwitch;
class VrmlNodeText;
class VrmlNodeTextureCoordinate;
class VrmlNodeTextureTransform;
class VrmlNodeTimeSensor;
class VrmlNodeTouchSensor;
class VrmlNodeTransform;
class VrmlNodeViewpoint;
class VrmlNodeVisibilitySensor;
class VrmlNodeWorldInfo;

OPENVRML_SCOPE class VrmlNodeVisitor {
public:
    virtual ~VrmlNodeVisitor() = 0;
    
    virtual void visit(VrmlNodeAnchor & node) = 0;
    virtual void visit(VrmlNodeAppearance & node) = 0;
    virtual void visit(VrmlNodeAudioClip & node) = 0;
    virtual void visit(VrmlNodeBackground & node) = 0;
    virtual void visit(VrmlNodeBillboard & node) = 0;
    virtual void visit(VrmlNodeBox & node) = 0;
    virtual void visit(VrmlNodeCollision & node) = 0;
    virtual void visit(VrmlNodeColor & node) = 0;
    virtual void visit(VrmlNodeColorInt & node) = 0;
    virtual void visit(VrmlNodeCone & node) = 0;
    virtual void visit(VrmlNodeCoordinate & node) = 0;
    virtual void visit(VrmlNodeCoordinateInt & node) = 0;
    virtual void visit(VrmlNodeCylinder & node) = 0;
    virtual void visit(VrmlNodeCylinderSensor & node) = 0;
    virtual void visit(VrmlNodeDirLight & node) = 0;
    virtual void visit(VrmlNodeElevationGrid & node) = 0;
    virtual void visit(VrmlNodeExtrusion & node) = 0;
    virtual void visit(VrmlNodeFog & node) = 0;
    virtual void visit(VrmlNodeFontStyle & node) = 0;
    virtual void visit(VrmlNodeGroup & node) = 0;
    virtual void visit(VrmlNodeIFaceSet & node) = 0;
    virtual void visit(VrmlNodeILineSet & node) = 0;
    virtual void visit(VrmlNodeImageTexture & node) = 0;
    virtual void visit(VrmlNodeInline & node) = 0;
    virtual void visit(VrmlNodeLOD & node) = 0;
    virtual void visit(VrmlNodeMaterial & node) = 0;
    virtual void visit(VrmlNodeMovieTexture & node) = 0;
    virtual void visit(VrmlNodeNavigationInfo & node) = 0;
    virtual void visit(VrmlNodeNormal & node) = 0;
    virtual void visit(VrmlNodeNormalInt & node) = 0;
    virtual void visit(VrmlNodeOrientationInt & node) = 0;
    virtual void visit(VrmlNodePixelTexture & node) = 0;
    virtual void visit(VrmlNodePlaneSensor & node) = 0;
    virtual void visit(VrmlNodePointLight & node) = 0;
    virtual void visit(VrmlNodePointSet & node) = 0;
    virtual void visit(VrmlNodePositionInt & node) = 0;
    virtual void visit(VrmlNodeProto & node) = 0;
    virtual void visit(VrmlNodeProximitySensor & node) = 0;
    virtual void visit(VrmlNodeScalarInt & node) = 0;
    virtual void visit(VrmlNodeScript & node) = 0;
    virtual void visit(VrmlNodeShape & node) = 0;
    virtual void visit(VrmlNodeSound & node) = 0;
    virtual void visit(VrmlNodeSphere & node) = 0;
    virtual void visit(VrmlNodeSphereSensor & node) = 0;
    virtual void visit(VrmlNodeSpotLight & node) = 0;
    virtual void visit(VrmlNodeSwitch & node) = 0;
    virtual void visit(VrmlNodeText & node) = 0;
    virtual void visit(VrmlNodeTextureCoordinate & node) = 0;
    virtual void visit(VrmlNodeTextureTransform & node) = 0;
    virtual void visit(VrmlNodeTimeSensor & node) = 0;
    virtual void visit(VrmlNodeTouchSensor & node) = 0;
    virtual void visit(VrmlNodeTransform & node) = 0;
    virtual void visit(VrmlNodeViewpoint & node) = 0;
    virtual void visit(VrmlNodeVisibilitySensor & node) = 0;
    virtual void visit(VrmlNodeWorldInfo & node) = 0;
};

# endif
