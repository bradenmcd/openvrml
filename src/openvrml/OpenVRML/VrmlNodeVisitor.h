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

namespace OpenVRML {

    class ScriptNode;
    class ProtoNode;
    class NodeAnchor;
    class NodeAppearance;
    class NodeAudioClip;
    class NodeBackground;
    class NodeBillboard;
    class NodeBox;
    class NodeCollision;
    class NodeColor;
    class NodeColorInt;
    class NodeCone;
    class NodeCoordinate;
    class NodeCoordinateInt;
    class NodeCylinder;
    class NodeCylinderSensor;
    class NodeDirLight;
    class NodeElevationGrid;
    class NodeExtrusion;
    class NodeFog;
    class NodeFontStyle;
    class NodeGroup;
    class NodeIFaceSet;
    class NodeILineSet;
    class NodeImageTexture;
    class NodeInline;
    class NodeLOD;
    class NodeMaterial;
    class NodeMovieTexture;
    class NodeNavigationInfo;
    class NodeNormal;
    class NodeNormalInt;
    class NodeOrientationInt;
    class NodePixelTexture;
    class NodePlaneSensor;
    class NodePointLight;
    class NodePointSet;
    class NodePositionInt;
    class NodeProximitySensor;
    class NodeScalarInt;
    class NodeShape;
    class NodeSound;
    class NodeSphere;
    class NodeSphereSensor;
    class NodeSpotLight;
    class NodeSwitch;
    class NodeText;
    class NodeTextureCoordinate;
    class NodeTextureTransform;
    class NodeTimeSensor;
    class NodeTouchSensor;
    class NodeTransform;
    class NodeViewpoint;
    class NodeVisibilitySensor;
    class NodeWorldInfo;

    class OPENVRML_SCOPE NodeVisitor {
    public:
        virtual ~NodeVisitor() = 0;

        virtual void visit(ScriptNode & node) = 0;
        virtual void visit(ProtoNode & node) = 0;
        virtual void visit(NodeAnchor & node) = 0;
        virtual void visit(NodeAppearance & node) = 0;
        virtual void visit(NodeAudioClip & node) = 0;
        virtual void visit(NodeBackground & node) = 0;
        virtual void visit(NodeBillboard & node) = 0;
        virtual void visit(NodeBox & node) = 0;
        virtual void visit(NodeCollision & node) = 0;
        virtual void visit(NodeColor & node) = 0;
        virtual void visit(NodeColorInt & node) = 0;
        virtual void visit(NodeCone & node) = 0;
        virtual void visit(NodeCoordinate & node) = 0;
        virtual void visit(NodeCoordinateInt & node) = 0;
        virtual void visit(NodeCylinder & node) = 0;
        virtual void visit(NodeCylinderSensor & node) = 0;
        virtual void visit(NodeDirLight & node) = 0;
        virtual void visit(NodeElevationGrid & node) = 0;
        virtual void visit(NodeExtrusion & node) = 0;
        virtual void visit(NodeFog & node) = 0;
        virtual void visit(NodeFontStyle & node) = 0;
        virtual void visit(NodeGroup & node) = 0;
        virtual void visit(NodeIFaceSet & node) = 0;
        virtual void visit(NodeILineSet & node) = 0;
        virtual void visit(NodeImageTexture & node) = 0;
        virtual void visit(NodeInline & node) = 0;
        virtual void visit(NodeLOD & node) = 0;
        virtual void visit(NodeMaterial & node) = 0;
        virtual void visit(NodeMovieTexture & node) = 0;
        virtual void visit(NodeNavigationInfo & node) = 0;
        virtual void visit(NodeNormal & node) = 0;
        virtual void visit(NodeNormalInt & node) = 0;
        virtual void visit(NodeOrientationInt & node) = 0;
        virtual void visit(NodePixelTexture & node) = 0;
        virtual void visit(NodePlaneSensor & node) = 0;
        virtual void visit(NodePointLight & node) = 0;
        virtual void visit(NodePointSet & node) = 0;
        virtual void visit(NodePositionInt & node) = 0;
        virtual void visit(NodeProximitySensor & node) = 0;
        virtual void visit(NodeScalarInt & node) = 0;
        virtual void visit(NodeShape & node) = 0;
        virtual void visit(NodeSound & node) = 0;
        virtual void visit(NodeSphere & node) = 0;
        virtual void visit(NodeSphereSensor & node) = 0;
        virtual void visit(NodeSpotLight & node) = 0;
        virtual void visit(NodeSwitch & node) = 0;
        virtual void visit(NodeText & node) = 0;
        virtual void visit(NodeTextureCoordinate & node) = 0;
        virtual void visit(NodeTextureTransform & node) = 0;
        virtual void visit(NodeTimeSensor & node) = 0;
        virtual void visit(NodeTouchSensor & node) = 0;
        virtual void visit(NodeTransform & node) = 0;
        virtual void visit(NodeViewpoint & node) = 0;
        virtual void visit(NodeVisibilitySensor & node) = 0;
        virtual void visit(NodeWorldInfo & node) = 0;
    };
}

# endif
