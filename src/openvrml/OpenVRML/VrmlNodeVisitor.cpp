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

# include "VrmlNodeVisitor.h"

using namespace OpenVRML;

/**
 * @class OpenVRML::NodeVisitor
 *
 * @brief An interface to be implemented by visitors for nodes in a
 *        scene graph.
 */

/**
 * @brief Destructor.
 */
NodeVisitor::~NodeVisitor() {}

/**
 * @fn void NodeVisitor::visit(NodeAnchor & node)
 *
 * @brief Visit an Anchor node.
 *
 * @param node an Anchor node.
 */

/**
 * @fn void NodeVisitor::visit(NodeAppearance & node)
 *
 * @brief Visit an Appearance node.
 *
 * @param node an Appearance node.
 */

/**
 * @fn void NodeVisitor::visit(NodeAudioClip & node)
 *
 * @brief Visit an AudioClip node.
 *
 * @param node an AudioClip node.
 */

/**
 * @fn void NodeVisitor::visit(NodeBackground & node)
 *
 * @brief Visit a Background node.
 *
 * @param node a Background node.
 */

/**
 * @fn void NodeVisitor::visit(NodeBillboard & node)
 *
 * @brief Visit a Billboard node.
 *
 * @param node a Billboard node.
 */

/**
 * @fn void NodeVisitor::visit(NodeBox & node)
 *
 * @brief Visit a Box node.
 *
 * @param node a Box node.
 */

/**
 * @fn void NodeVisitor::visit(NodeCollision & node)
 *
 * @brief Visit a Collision node.
 *
 * @param node a Collision node.
 */

/**
 * @fn void NodeVisitor::visit(NodeColor & node)
 *
 * @brief Visit a Color node.
 *
 * @param node a Color node.
 */

/**
 * @fn void NodeVisitor::visit(NodeColorInt & node)
 *
 * @brief Visit a ColorInterpolator node.
 *
 * @param node a ColorInterpolator node.
 */

/**
 * @fn void NodeVisitor::visit(NodeCone & node)
 *
 * @brief Visit a Cone node.
 *
 * @param node a Cone node.
 */

/**
 * @fn void NodeVisitor::visit(NodeCoordinate & node)
 *
 * @brief Visit a Coordinate node.
 *
 * @param node a Coordinate node.
 */

/**
 * @fn void NodeVisitor::visit(NodeCoordinateInt & node)
 *
 * @brief Visit a CoordinateInterpolator node.
 *
 * @param node a CoordinateInterpolator node.
 */

/**
 * @fn void NodeVisitor::visit(NodeCylinder & node)
 *
 * @brief Visit a Cylinder node.
 *
 * @param node a Cylinder node.
 */

/**
 * @fn void NodeVisitor::visit(NodeCylinderSensor & node)
 *
 * @brief Visit a CylinderSensor node.
 *
 * @param node a CylinderSensor node.
 */

/**
 * @fn void NodeVisitor::visit(NodeDirLight & node)
 *
 * @brief Visit a DirectionalLight node.
 *
 * @param node a DirectionalLight node.
 */

/**
 * @fn void NodeVisitor::visit(NodeElevationGrid & node)
 *
 * @brief Visit an ElevationGrid node.
 *
 * @param node an ElevationGrid node.
 */

/**
 * @fn void NodeVisitor::visit(NodeExtrusion & node)
 *
 * @brief Visit an Extrusion node.
 *
 * @param node an Extrusion node.
 */

/**
 * @fn void NodeVisitor::visit(NodeFog & node)
 *
 * @brief Visit a Fog node.
 *
 * @param node a Fog node.
 */

/**
 * @fn void NodeVisitor::visit(NodeFontStyle & node)
 *
 * @brief Visit a FontStyle node.
 *
 * @param node a FontStyle node.
 */

/**
 * @fn void NodeVisitor::visit(NodeGroup & node)
 *
 * @brief Visit a Group node.
 *
 * @param node a Group node.
 */

/**
 * @fn void NodeVisitor::visit(NodeIFaceSet & node)
 *
 * @brief Visit an IndexedFaceSet node.
 *
 * @param node an IndexedFaceSet node.
 */

/**
 * @fn void NodeVisitor::visit(NodeILineSet & node)
 *
 * @brief Visit an IndexedLineSet node.
 *
 * @param node an IndexedLineSet node.
 */

/**
 * @fn void NodeVisitor::visit(NodeImageTexture & node)
 *
 * @brief Visit an ImageTexture node.
 *
 * @param node an ImageTexture node.
 */

/**
 * @fn void NodeVisitor::visit(NodeInline & node)
 *
 * @brief Visit an Inline node.
 *
 * @param node an Inline node.
 */

/**
 * @fn void NodeVisitor::visit(NodeLOD & node)
 *
 * @brief Visit a LOD node.
 *
 * @param node a LOD node.
 */

/**
 * @fn void NodeVisitor::visit(NodeMaterial & node)
 *
 * @brief Visit a Material node.
 *
 * @param node a Material node.
 */

/**
 * @fn void NodeVisitor::visit(NodeMovieTexture & node)
 *
 * @brief Visit a MovieTexture node.
 *
 * @param node a MovieTexture node.
 */

/**
 * @fn void NodeVisitor::visit(NodeNavigationInfo & node)
 *
 * @brief Visit a NavigationInfo node.
 *
 * @param node a NavigationInfo node.
 */

/**
 * @fn void NodeVisitor::visit(NodeNormal & node)
 *
 * @brief Visit a Normal node.
 *
 * @param node a Normal node.
 */

/**
 * @fn void NodeVisitor::visit(NodeNormalInt & node)
 *
 * @brief Visit a NormalInterpolator node.
 *
 * @param node a NormalInterpolator node.
 */

/**
 * @fn void NodeVisitor::visit(NodeOrientationInt & node)
 *
 * @brief Visit an OrientationInterpolator node.
 *
 * @param node an OrientationInterpolator node.
 */

/**
 * @fn void NodeVisitor::visit(NodePixelTexture & node)
 *
 * @brief Visit a PixelTexture node.
 *
 * @param node a PixelTexture node.
 */

/**
 * @fn void NodeVisitor::visit(NodePlaneSensor & node)
 *
 * @brief Visit a PlaneSensor node.
 *
 * @param node a PlaneSensor node.
 */

/**
 * @fn void NodeVisitor::visit(NodePointLight & node)
 *
 * @brief Visit a PointLight node.
 *
 * @param node a PointLight node.
 */

/**
 * @fn void NodeVisitor::visit(NodePointSet & node)
 *
 * @brief Visit a PointSet node.
 *
 * @param node a PointSet node.
 */

/**
 * @fn void NodeVisitor::visit(NodePositionInt & node)
 *
 * @brief Visit a PositionInterpolator node.
 *
 * @param node a PositionInterpolator node.
 */

/**
 * @fn void NodeVisitor::visit(NodeProto & node)
 *
 * @brief Visit a PROTO instantiation node.
 *
 * @param node a PROTO instantiation node.
 */

/**
 * @fn void NodeVisitor::visit(NodeProximitySensor & node)
 *
 * @brief Visit a ProximitySensor node.
 *
 * @param node a ProximitySensor node.
 */

/**
 * @fn void NodeVisitor::visit(NodeScalarInt & node)
 *
 * @brief Visit a ScalarInterpolator node.
 *
 * @param node a ScalarInterpolator node.
 */

/**
 * @fn void NodeVisitor::visit(ScriptNode & node)
 *
 * @brief Visit a Script node.
 *
 * @param node a Script node.
 */

/**
 * @fn void NodeVisitor::visit(NodeShape & node)
 *
 * @brief Visit a Shape node.
 *
 * @param node a Shape node.
 */

/**
 * @fn void NodeVisitor::visit(NodeSound & node)
 *
 * @brief Visit a Sound node.
 *
 * @param node a Sound node.
 */

/**
 * @fn void NodeVisitor::visit(NodeSphere & node)
 *
 * @brief Visit a Sphere node.
 *
 * @param node a Sphere node.
 */

/**
 * @fn void NodeVisitor::visit(NodeSphereSensor & node)
 *
 * @brief Visit a SphereSensor node.
 *
 * @param node a SphereSensor node.
 */

/**
 * @fn void NodeVisitor::visit(NodeSpotLight & node)
 *
 * @brief Visit a SpotLight node.
 *
 * @param node a SpotLight node.
 */

/**
 * @fn void NodeVisitor::visit(NodeSwitch & node)
 *
 * @brief Visit a Switch node.
 *
 * @param node a Switch node.
 */

/**
 * @fn void NodeVisitor::visit(NodeText & node)
 *
 * @brief Visit a Text node.
 *
 * @param node a Text node.
 */

/**
 * @fn void NodeVisitor::visit(NodeTextureCoordinate & node)
 *
 * @brief Visit a TextureCoordinate node.
 *
 * @param node a TextureCoordinate node.
 */

/**
 * @fn void NodeVisitor::visit(NodeTextureTransform & node)
 *
 * @brief Visit a TextureTransform node.
 *
 * @param node a TextureTransform node.
 */

/**
 * @fn void NodeVisitor::visit(NodeTimeSensor & node)
 *
 * @brief Visit a TimeSensor node.
 *
 * @param node a TimeSensor node.
 */

/**
 * @fn void NodeVisitor::visit(NodeTouchSensor & node)
 *
 * @brief Visit a TouchSensor node.
 *
 * @param node a TouchSensor node.
 */

/**
 * @fn void NodeVisitor::visit(NodeTransform & node)
 *
 * @brief Visit a Transform node.
 *
 * @param node a Transform node.
 */

/**
 * @fn void NodeVisitor::visit(NodeViewpoint & node)
 *
 * @brief Visit a Viewpoint node.
 *
 * @param node a Viewpoint node.
 */

/**
 * @fn void NodeVisitor::visit(NodeVisibilitySensor & node)
 *
 * @brief Visit a VisibilitySensor node.
 *
 * @param node a VisibilitySensor node.
 */

/**
 * @fn void NodeVisitor::visit(NodeWorldInfo & node)
 *
 * @brief Visit a WorldInfo node.
 *
 * @param node a WorldInfo node.
 */
