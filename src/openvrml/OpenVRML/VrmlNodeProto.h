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

#include "common.h"
#include "VrmlNode.h"
#include "nodetype.h"
#include "Viewer.h"
#include "field.h"

namespace OpenVRML {

    class OPENVRML_SCOPE ProtoNode : public Node {
    public:
        class NameValueRec;

    private:
        bool d_instantiated;
        VrmlNamespace * d_scope;
        MFNode implNodes;

        std::list<NameValueRec *> d_fields;

        class EventDispatch;
        std::list<EventDispatch *> d_eventDispatch;

        Viewer::Object d_viewerObject;

    public:
        ProtoNode(const NodeType & nodeDef, VrmlScene *scene);
        ProtoNode(const ProtoNode &);
        virtual ~ProtoNode();

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue *
                getField(const std::string & fieldName) const;
        virtual void setField(const std::string & fieldName,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
		             const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;

        virtual void addToScene(VrmlScene * scene, const std::string & relUrl);
        virtual ostream & printFields(ostream & os, int indent);

        virtual ScriptNode * toScript() const;
        virtual NodeAnchor * toAnchor() const;
        virtual NodeAppearance * toAppearance() const;
        virtual NodeAudioClip * toAudioClip() const;
        virtual NodeBackground * toBackground() const;
        virtual NodeBillboard * toBillboard() const;
        virtual NodeBox * toBox() const;
        virtual NodeChild * toChild() const;
        virtual NodeCollision * toCollision() const;
        virtual NodeColor * toColor() const;
        virtual NodeCone * toCone() const;
        virtual NodeCoordinate * toCoordinate() const;
        virtual NodeCylinder * toCylinder() const;
        virtual NodeDirLight * toDirLight() const;
        virtual NodeElevationGrid * toElevationGrid() const;
        virtual NodeExtrusion * toExtrusion() const;
        virtual NodeFog * toFog() const;
        virtual NodeFontStyle * toFontStyle() const;
        virtual NodeGeometry * toGeometry() const;
        virtual NodeGroup * toGroup() const;
        virtual NodeIFaceSet * toIFaceSet() const;
        virtual NodeImageTexture * toImageTexture() const;
        virtual NodePixelTexture * toPixelTexture() const;
        virtual NodeInline * toInline() const;
        virtual NodeLOD * toLOD() const;
        virtual NodeLight * toLight() const;
        virtual NodeMaterial * toMaterial() const;
        virtual NodeMovieTexture * toMovieTexture() const;
        virtual NodeNavigationInfo * toNavigationInfo() const;
        virtual NodeNormal * toNormal() const;
        virtual NodeOrientationInt * toOrientationInt() const;
        virtual NodePlaneSensor * toPlaneSensor() const;
        virtual NodePositionInt * toPositionInt() const;
        virtual NodeSphereSensor * toSphereSensor() const;
        virtual NodeCylinderSensor * toCylinderSensor() const;
        virtual NodePointLight * toPointLight() const;
        virtual NodeScalarInt * toScalarInt() const;
        virtual NodeShape * toShape() const;
        virtual NodeSphere * toSphere() const;
        virtual NodeSound * toSound() const;
        virtual NodeSpotLight * toSpotLight() const;
        virtual NodeSwitch * toSwitch() const;
        virtual NodeTexture * toTexture() const;
        virtual NodeTextureCoordinate * toTextureCoordinate() const;
        virtual NodeTextureTransform * toTextureTransform() const;
        virtual NodeTimeSensor * toTimeSensor() const;
        virtual NodeTouchSensor * toTouchSensor() const;
        virtual NodeTransform * toTransform() const;
        virtual NodeViewpoint * toViewpoint() const;

        virtual void render(Viewer *, VrmlRenderContext rc);

        virtual bool isModified() const;
        virtual void updateModified(NodePath& path, int flags);

        virtual void accumulateTransform( Node* );

        const MFNode & getImplNodes() const;

        const VrmlBVolume* getBVolume() const;

    private:
        const NodePtr firstNode() const;
        void instantiate();
        NameValueRec * findField(const std::string & fieldName) const;
    };
}

#endif
