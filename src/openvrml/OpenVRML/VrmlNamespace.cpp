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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <algorithm>
# include <stack>
# include "VrmlNamespace.h"
# include "node.h"
# include "Route.h"
# include "System.h"
# include "script.h"
# include "proto.h"

# ifndef NDEBUG
#   include <typeinfo>
# endif

namespace OpenVRML {

/**
 * @class VrmlNamespace
 *
 * @brief The VrmlNamespace class keeps track of defined nodes and
 *      prototypes.
 *
 * PROTO definitions add node types to the namespace.
 * PROTO implementations are a separate node type namespace,
 * and require that any nested PROTOs NOT be available outside
 * the PROTO implementation. PROTOs defined outside the current
 * namespace are available.
 */

typedef std::list<NodeTypePtr> NodeTypeList;

VrmlNamespace::VrmlNamespace(VrmlNamespace * parent):
        parent(parent) {}

VrmlNamespace::~VrmlNamespace() {}

/**
 * @brief Add a node type.
 *
 * Print an error message if the argument type is already defined.
 *
 * @param nodeType a NodeTypePtr
 *
 * @todo Throw std::invalid_argument if the argument type is already defined.
 */
void VrmlNamespace::addNodeType(const NodeTypePtr & nodeType) {
    assert(nodeType);
    assert(!this->findType(nodeType->id));
    this->typeList.push_front(nodeType);
}

/**
 * @brief Find a node type, given a type name. Returns NULL if type is
 *      not defined.
 */
const NodeTypePtr VrmlNamespace::findType(const std::string & id) const {
    //
    // Look through the types unique to this scope.
    //
    for (NodeTypeList::const_iterator itr(this->typeList.begin());
            itr != this->typeList.end(); ++itr) {
        assert(*itr);
        if (id == (*itr)->id) { return *itr; }
    }
    
    //
    // Look in the parent scope for the type.
    //
    if (this->parent) { return this->parent->findType(id); }
    
    return NodeTypePtr(0);
}

const NodeTypePtr VrmlNamespace::firstType() const {
    if (!this->typeList.empty()) { return this->typeList.front(); }
    return NodeTypePtr(0);
}

void VrmlNamespace::addNodeName(Node & namedNode) {
    // We could remove any existing node with this name, but
    // since we are just pushing this one onto the front of
    // the list, the other name won't be found. If we do
    // something smart with this list (like sorting), this
    // will need to change.
    this->nameList.push_front(&namedNode);
}

void VrmlNamespace::removeNodeName(Node & namedNode) {
    for (std::list<Node *>::iterator i(this->nameList.begin());
            i != this->nameList.end(); ++i) {
        if (*i == &namedNode) {
            this->nameList.erase(i);
            return;
        }
    }
}

Node * VrmlNamespace::findNode(const std::string & id) const {
    for (std::list<Node *>::const_iterator itr(this->nameList.begin());
            itr != this->nameList.end(); ++itr) {
        if (id == (*itr)->getId()) { return *itr; }
    }
    return 0;
}


namespace {
    class AddInterfaceToSet_ : std::unary_function<NodeInterface, void> {
        NodeInterfaceSet & nodeInterfaceSet;
        
    public:
        AddInterfaceToSet_(NodeInterfaceSet & nodeInterfaceSet):
                nodeInterfaceSet(nodeInterfaceSet) {}
        
        void operator()(const NodeInterface & nodeInterface) const
                throw (std::invalid_argument, std::bad_alloc) {
            this->nodeInterfaceSet.add(nodeInterface);
        }
    };
    
    class Vrml97NodeInterfaceSet_ : public NodeInterfaceSet {
    public:
        Vrml97NodeInterfaceSet_(const NodeInterface * const begin,
                                const NodeInterface * const end) {
            std::for_each(begin, end, AddInterfaceToSet_(*this));
        }
    };
}

Vrml97RootNamespace::Vrml97RootNamespace(const NodeClassMap & nodeClassMap) {
    NodeClassMap::const_iterator pos;
    
    //
    // Anchor node
    //
    static const NodeInterface anchorInterfaces[] = {
        { NodeInterface::eventIn, FieldValue::mfnode, "addChildren" },
        { NodeInterface::eventIn, FieldValue::mfnode, "removeChildren" },
        { NodeInterface::exposedField, FieldValue::mfnode, "children" },
        { NodeInterface::exposedField, FieldValue::sfstring, "description" },
        { NodeInterface::exposedField, FieldValue::mfstring, "parameter" },
        { NodeInterface::exposedField, FieldValue::mfstring, "url" },
        { NodeInterface::field, FieldValue::sfvec3f, "bboxCenter" },
        { NodeInterface::field, FieldValue::sfvec3f, "bboxSize" }
    };
    static const Vrml97NodeInterfaceSet_
            anchorInterfaceSet(anchorInterfaces, anchorInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:Anchor");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Anchor", anchorInterfaceSet));
    
    //
    // Appearance node
    //
    static const NodeInterface appearanceInterfaces[] = {
        { NodeInterface::exposedField, FieldValue::sfnode, "material" },
        { NodeInterface::exposedField, FieldValue::sfnode, "texture" },
        { NodeInterface::exposedField, FieldValue::sfnode, "textureTransform" }
    };
    static const Vrml97NodeInterfaceSet_
            appearanceInterfaceSet(appearanceInterfaces,
                                   appearanceInterfaces + 3);
    pos = nodeClassMap.find("urn:X-openvrml:node:Appearance");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Appearance",
                                              appearanceInterfaceSet));
    
    //
    // AudioClip node
    //
    static const NodeInterface audioClipInterfaces[] = {
        { NodeInterface::exposedField, FieldValue::sfstring, "description" },
        { NodeInterface::exposedField, FieldValue::sfbool, "loop" },
        { NodeInterface::exposedField, FieldValue::sffloat, "pitch" },
        { NodeInterface::exposedField, FieldValue::sftime, "startTime" },
        { NodeInterface::exposedField, FieldValue::sftime, "stopTime" },
        { NodeInterface::exposedField, FieldValue::mfstring, "url" },
        { NodeInterface::eventOut, FieldValue::sftime, "duration_changed" },
        { NodeInterface::eventOut, FieldValue::sfbool, "isActive" }
    };
    static const Vrml97NodeInterfaceSet_
            audioClipInterfaceSet(audioClipInterfaces,
                                  audioClipInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:AudioClip");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("AudioClip",
                                              audioClipInterfaceSet));
    
    //
    // Background node
    //
    static const NodeInterface backgroundInterfaces[] = {
        { NodeInterface::eventIn, FieldValue::sfbool, "set_bind" },
        { NodeInterface::exposedField, FieldValue::mffloat, "groundAngle" },
        { NodeInterface::exposedField, FieldValue::mfcolor, "groundColor" },
        { NodeInterface::exposedField, FieldValue::mfstring, "backUrl" },
        { NodeInterface::exposedField, FieldValue::mfstring, "bottomUrl" },
        { NodeInterface::exposedField, FieldValue::mfstring, "frontUrl" },
        { NodeInterface::exposedField, FieldValue::mfstring, "leftUrl" },
        { NodeInterface::exposedField, FieldValue::mfstring, "rightUrl" },
        { NodeInterface::exposedField, FieldValue::mfstring, "topUrl" },
        { NodeInterface::exposedField, FieldValue::mffloat, "skyAngle" },
        { NodeInterface::exposedField, FieldValue::mfcolor, "skyColor" },
        { NodeInterface::eventOut, FieldValue::sfbool, "isBound" }
    };
    static const Vrml97NodeInterfaceSet_
            backgroundInterfaceSet(backgroundInterfaces,
                                  backgroundInterfaces + 12);
    pos = nodeClassMap.find("urn:X-openvrml:node:Background");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Background",
                                              backgroundInterfaceSet));
    
    //
    // Billboard node
    //
    static const NodeInterface billboardInterfaces[] = {
        { NodeInterface::eventIn, FieldValue::mfnode, "addChildren" },
        { NodeInterface::eventIn, FieldValue::mfnode, "removeChildren" },
        { NodeInterface::exposedField, FieldValue::sfvec3f, "axisOfRotation" },
        { NodeInterface::exposedField, FieldValue::mfnode, "children" },
        { NodeInterface::field, FieldValue::sfvec3f, "bboxCenter" },
        { NodeInterface::field, FieldValue::sfvec3f, "bboxSize" }
    };
    static const Vrml97NodeInterfaceSet_
            billboardInterfaceSet(billboardInterfaces,
                                  billboardInterfaces + 6);
    pos = nodeClassMap.find("urn:X-openvrml:node:Billboard");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Billboard",
                                              billboardInterfaceSet));
    
    //
    // Box node
    //
    static const NodeInterface boxInterface =
            { NodeInterface::field, FieldValue::sfvec3f, "size" };
    static const Vrml97NodeInterfaceSet_
            boxInterfaceSet(&boxInterface, &boxInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Box");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Box", boxInterfaceSet));
    
    //
    // Collision node
    //
    static const NodeInterface collisionInterfaces[] = {
        { NodeInterface::eventIn, FieldValue::mfnode, "addChildren" },
        { NodeInterface::eventIn, FieldValue::mfnode, "removeChildren" },
        { NodeInterface::exposedField, FieldValue::mfnode, "children" },
        { NodeInterface::exposedField, FieldValue::sfbool, "collide" },
        { NodeInterface::field, FieldValue::sfvec3f, "bboxCenter" },
        { NodeInterface::field, FieldValue::sfvec3f, "bboxSize" },
        { NodeInterface::field, FieldValue::sfnode, "proxy" },
        { NodeInterface::eventOut, FieldValue::sftime, "collideTime" }
    };
    static const Vrml97NodeInterfaceSet_
            collisionInterfaceSet(collisionInterfaces, collisionInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:Collision");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Collision",
                                              collisionInterfaceSet));
    
    //
    // Color node
    //
    static const NodeInterface colorInterface =
            { NodeInterface::exposedField, FieldValue::mfcolor, "color" };
    static const Vrml97NodeInterfaceSet_
            colorInterfaceSet(&colorInterface, &colorInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Color");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Color", colorInterfaceSet));
    
    //
    // ColorInterpolator node
    //
    static const NodeInterface colorInterpolatorInterfaces[] = {
        { NodeInterface::eventIn, FieldValue::sffloat, "set_fraction" },
        { NodeInterface::exposedField, FieldValue::mffloat, "key" },
        { NodeInterface::exposedField, FieldValue::mfcolor, "keyValue" },
        { NodeInterface::eventOut, FieldValue::sfcolor, "value_changed" }
    };
    static const Vrml97NodeInterfaceSet_
            colorInterpolatorInterfaceSet(colorInterpolatorInterfaces,
                                          colorInterpolatorInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:ColorInterpolator");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("ColorInterpolator",
                                              colorInterpolatorInterfaceSet));
    
    //
    // Cone node
    //
    static const NodeInterface coneInterfaces[] = {
        { NodeInterface::field, FieldValue::sffloat, "bottomRadius" },
        { NodeInterface::field, FieldValue::sffloat, "height" },
        { NodeInterface::field, FieldValue::sfbool, "side" },
        { NodeInterface::field, FieldValue::sfbool, "bottom" }
    };
    static const Vrml97NodeInterfaceSet_
            coneInterfaceSet(coneInterfaces, coneInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:Cone");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Cone", coneInterfaceSet));
    
    //
    // Coordinate node
    //
    static const NodeInterface coordinateInterface =
            { NodeInterface::exposedField, FieldValue::mfvec3f, "point" };
    static const Vrml97NodeInterfaceSet_
            coordinateInterfaceSet(&coordinateInterface,
                                   &coordinateInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Coordinate");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Coordinate",
                                              coordinateInterfaceSet));
    
    //
    // CoordinateInterpolator node
    //
    static const NodeInterface coordinateInterpolatorInterfaces[] = {
        { NodeInterface::eventIn, FieldValue::sffloat, "set_fraction" },
        { NodeInterface::exposedField, FieldValue::mffloat, "key" },
        { NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue" },
        { NodeInterface::eventOut, FieldValue::mfvec3f, "value_changed" }
    };
    static const Vrml97NodeInterfaceSet_
            coordinateInterpolatorInterfaceSet(coordinateInterpolatorInterfaces,
                                               coordinateInterpolatorInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:CoordinateInterpolator");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("CoordinateInterpolator",
                                              coordinateInterpolatorInterfaceSet));
    
    //
    // Cylinder node
    //
    static const NodeInterface cylinderInterfaces[] = {
        { NodeInterface::field, FieldValue::sfbool, "bottom" },
        { NodeInterface::field, FieldValue::sffloat, "height" },
        { NodeInterface::field, FieldValue::sffloat, "radius" },
        { NodeInterface::field, FieldValue::sfbool, "side" },
        { NodeInterface::field, FieldValue::sfbool, "top" }
    };
    static const Vrml97NodeInterfaceSet_
            cylinderInterfaceSet(cylinderInterfaces, cylinderInterfaces + 5);
    pos = nodeClassMap.find("urn:X-openvrml:node:Cylinder");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Cylinder",
                                              cylinderInterfaceSet));
    
    //
    // CylinderSensor node
    //
    static const NodeInterface cylinderSensorInterfaces[] = {
        { NodeInterface::exposedField, FieldValue::sfbool, "autoOffset" },
        { NodeInterface::exposedField, FieldValue::sffloat, "diskAngle" },
        { NodeInterface::exposedField, FieldValue::sfbool, "enabled" },
        { NodeInterface::exposedField, FieldValue::sffloat, "maxAngle" },
        { NodeInterface::exposedField, FieldValue::sffloat, "minAngle" },
        { NodeInterface::exposedField, FieldValue::sffloat, "offset" },
        { NodeInterface::eventOut, FieldValue::sfbool, "isActive" },
        { NodeInterface::eventOut, FieldValue::sfrotation, "rotation_changed" },
        { NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed" }
    };
    static const Vrml97NodeInterfaceSet_
            cylinderSensorInterfaceSet(cylinderSensorInterfaces,
                                       cylinderSensorInterfaces + 9);
    pos = nodeClassMap.find("urn:X-openvrml:node:CylinderSensor");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("CylinderSensor",
                                              cylinderSensorInterfaceSet));
    
    //
    // DirectionalLight node
    //
    static const NodeInterface directionalLightInterfaces[] = {
        { NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity" },
        { NodeInterface::exposedField, FieldValue::sfcolor, "color" },
        { NodeInterface::exposedField, FieldValue::sfvec3f, "direction" },
        { NodeInterface::exposedField, FieldValue::sffloat, "intensity" },
        { NodeInterface::exposedField, FieldValue::sfbool, "on" }
    };
    static const Vrml97NodeInterfaceSet_
            directionalLightInterfaceSet(directionalLightInterfaces,
                                         directionalLightInterfaces + 5);
    pos = nodeClassMap.find("urn:X-openvrml:node:DirectionalLight");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("DirectionalLight",
                                              directionalLightInterfaceSet));
    
    //
    // ElevationGrid node
    //
    static const NodeInterface elevationGridInterfaces[] = {
        { NodeInterface::eventIn, FieldValue::mffloat, "set_height" },
        { NodeInterface::exposedField, FieldValue::sfnode, "color" },
        { NodeInterface::exposedField, FieldValue::sfnode, "normal" },
        { NodeInterface::exposedField, FieldValue::sfnode, "texCoord" },
        { NodeInterface::field, FieldValue::mffloat, "height" },
        { NodeInterface::field, FieldValue::sfbool, "ccw" },
        { NodeInterface::field, FieldValue::sfbool, "colorPerVertex" },
        { NodeInterface::field, FieldValue::sffloat, "creaseAngle" },
        { NodeInterface::field, FieldValue::sfbool, "normalPerVertex" },
        { NodeInterface::field, FieldValue::sfbool, "solid" },
        { NodeInterface::field, FieldValue::sfint32, "xDimension" },
        { NodeInterface::field, FieldValue::sffloat, "xSpacing" },
        { NodeInterface::field, FieldValue::sfint32, "zDimension" },
        { NodeInterface::field, FieldValue::sffloat, "zSpacing" }
    };
    static const Vrml97NodeInterfaceSet_
            elevationGridInterfaceSet(elevationGridInterfaces,
                                         elevationGridInterfaces + 14);
    pos = nodeClassMap.find("urn:X-openvrml:node:ElevationGrid");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("ElevationGrid",
                                              elevationGridInterfaceSet));
    
    //
    // Extrusion node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::mfvec2f, "set_crossSection" },
            { NodeInterface::eventIn, FieldValue::mfrotation, "set_orientation" },
            { NodeInterface::eventIn, FieldValue::mfvec2f, "set_scale" },
            { NodeInterface::eventIn, FieldValue::mfvec3f, "set_spine" },
            { NodeInterface::field, FieldValue::sfbool, "beginCap" },
            { NodeInterface::field, FieldValue::sfbool, "ccw" },
            { NodeInterface::field, FieldValue::sfbool, "convex" },
            { NodeInterface::field, FieldValue::sffloat, "creaseAngle" },
            { NodeInterface::field, FieldValue::mfvec2f, "crossSection" },
            { NodeInterface::field, FieldValue::sfbool, "endCap" },
            { NodeInterface::field, FieldValue::mfrotation, "orientation" },
            { NodeInterface::field, FieldValue::mfvec2f, "scale" },
            { NodeInterface::field, FieldValue::sfbool, "solid" },
            { NodeInterface::field, FieldValue::mfvec3f, "spine" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 14);
        pos = nodeClassMap.find("urn:X-openvrml:node:Extrusion");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Extrusion",
                                                  nodeInterfaceSet));
    }
    
    //
    // Fog node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::sfbool, "set_bind" },
            { NodeInterface::exposedField, FieldValue::sfcolor, "color" },
            { NodeInterface::exposedField, FieldValue::sfstring, "fogType" },
            { NodeInterface::exposedField, FieldValue::sffloat, "visibilityRange" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isBound" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 5);
        pos = nodeClassMap.find("urn:X-openvrml:node:Fog");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Fog", nodeInterfaceSet));
    }
    
    //
    // FontStyle node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::field, FieldValue::mfstring, "family" },
            { NodeInterface::field, FieldValue::sfbool, "horizontal" },
            { NodeInterface::field, FieldValue::mfstring, "justify" },
            { NodeInterface::field, FieldValue::sfstring, "language" },
            { NodeInterface::field, FieldValue::sfbool, "leftToRight" },
            { NodeInterface::field, FieldValue::sffloat, "size" },
            { NodeInterface::field, FieldValue::sffloat, "spacing" },
            { NodeInterface::field, FieldValue::sfstring, "style" },
            { NodeInterface::field, FieldValue::sfbool, "topToBottom" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:FontStyle");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("FontStyle",
                                                  nodeInterfaceSet));
    }
    
    //
    // Group node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::mfnode, "addChildren" },
            { NodeInterface::eventIn, FieldValue::mfnode, "removeChildren" },
            { NodeInterface::exposedField, FieldValue::mfnode, "children" },
            { NodeInterface::field, FieldValue::sfvec3f, "bboxCenter" },
            { NodeInterface::field, FieldValue::sfvec3f, "bboxSize" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 5);
        pos = nodeClassMap.find("urn:X-openvrml:node:Group");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Group",
                                                  nodeInterfaceSet));
    }
    
    //
    // ImageTexture node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::mfstring, "url" },
            { NodeInterface::field, FieldValue::sfbool, "repeatS" },
            { NodeInterface::field, FieldValue::sfbool, "repeatT" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:ImageTexture");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("ImageTexture",
                                                  nodeInterfaceSet));
    }
    
    //
    // IndexedFaceSet node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::mfint32, "set_colorIndex" },
            { NodeInterface::eventIn, FieldValue::mfint32, "set_coordIndex" },
            { NodeInterface::eventIn, FieldValue::mfint32, "set_normalIndex" },
            { NodeInterface::eventIn, FieldValue::mfint32, "set_texCoordIndex" },
            { NodeInterface::exposedField, FieldValue::sfnode, "color" },
            { NodeInterface::exposedField, FieldValue::sfnode, "coord" },
            { NodeInterface::exposedField, FieldValue::sfnode, "normal" },
            { NodeInterface::exposedField, FieldValue::sfnode, "texCoord" },
            { NodeInterface::field, FieldValue::sfbool, "ccw" },
            { NodeInterface::field, FieldValue::mfint32, "colorIndex" },
            { NodeInterface::field, FieldValue::sfbool, "colorPerVertex" },
            { NodeInterface::field, FieldValue::sfbool, "convex" },
            { NodeInterface::field, FieldValue::mfint32, "coordIndex" },
            { NodeInterface::field, FieldValue::sffloat, "creaseAngle" },
            { NodeInterface::field, FieldValue::mfint32, "normalIndex" },
            { NodeInterface::field, FieldValue::sfbool, "normalPerVertex" },
            { NodeInterface::field, FieldValue::sfbool, "solid" },
            { NodeInterface::field, FieldValue::mfint32, "texCoordIndex" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 18);
        pos = nodeClassMap.find("urn:X-openvrml:node:IndexedFaceSet");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("IndexedFaceSet",
                                                  nodeInterfaceSet));
    }
    
    //
    // IndexedLineSet node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::mfint32, "set_colorIndex" },
            { NodeInterface::eventIn, FieldValue::mfint32, "set_coordIndex" },
            { NodeInterface::exposedField, FieldValue::sfnode, "color" },
            { NodeInterface::exposedField, FieldValue::sfnode, "coord" },
            { NodeInterface::field, FieldValue::mfint32, "colorIndex" },
            { NodeInterface::field, FieldValue::sfbool, "colorPerVertex" },
            { NodeInterface::field, FieldValue::mfint32, "coordIndex" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:IndexedLineSet");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("IndexedLineSet",
                                                  nodeInterfaceSet));
    }
    
    //
    // Inline node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::mfstring, "url" },
            { NodeInterface::field, FieldValue::sfvec3f, "bboxCenter" },
            { NodeInterface::field, FieldValue::sfvec3f, "bboxSize" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:Inline");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Inline", nodeInterfaceSet));
    }
    
    //
    // LOD node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::mfnode, "level" },
            { NodeInterface::field, FieldValue::sfvec3f, "center" },
            { NodeInterface::field, FieldValue::mffloat, "range" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:LOD");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("LOD", nodeInterfaceSet));
    }
    
    //
    // Material node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity" },
            { NodeInterface::exposedField, FieldValue::sfcolor, "diffuseColor" },
            { NodeInterface::exposedField, FieldValue::sfcolor, "emissiveColor" },
            { NodeInterface::exposedField, FieldValue::sffloat, "shininess" },
            { NodeInterface::exposedField, FieldValue::sfcolor, "specularColor" },
            { NodeInterface::exposedField, FieldValue::sffloat, "transparency" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:Material");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Material",
                                                  nodeInterfaceSet));
    }
    
    //
    // MovieTexture node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfbool, "loop" },
            { NodeInterface::exposedField, FieldValue::sffloat, "speed" },
            { NodeInterface::exposedField, FieldValue::sftime, "startTime" },
            { NodeInterface::exposedField, FieldValue::sftime, "stopTime" },
            { NodeInterface::exposedField, FieldValue::mfstring, "url" },
            { NodeInterface::field, FieldValue::sfbool, "repeatS" },
            { NodeInterface::field, FieldValue::sfbool, "repeatT" },
            { NodeInterface::eventOut, FieldValue::sftime, "duration_changed" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isActive" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:MovieTexture");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("MovieTexture",
                                                  nodeInterfaceSet));
    }
    
    //
    // NavigationInfo node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::sfbool, "set_bind" },
            { NodeInterface::exposedField, FieldValue::mffloat, "avatarSize" },
            { NodeInterface::exposedField, FieldValue::sfbool, "headlight" },
            { NodeInterface::exposedField, FieldValue::sffloat, "speed" },
            { NodeInterface::exposedField, FieldValue::mfstring, "type" },
            { NodeInterface::exposedField, FieldValue::sffloat, "visibilityLimit" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isBound" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:NavigationInfo");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("NavigationInfo",
                                                  nodeInterfaceSet));
    }
    
    //
    // Normal node
    //
    {
        static const NodeInterface nodeInterface =
                { NodeInterface::exposedField, FieldValue::mfvec3f, "vector" };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:Normal");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Normal",
                                                  nodeInterfaceSet));
    }
    
    //
    // NormalInterpolator node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::sffloat, "set_fraction" },
            { NodeInterface::exposedField, FieldValue::mffloat, "key" },
            { NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue" },
            { NodeInterface::eventOut, FieldValue::mfvec3f, "value_changed" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:NormalInterpolator");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("NormalInterpolator",
                                                  nodeInterfaceSet));
    }
    
    //
    // OrientationInterpolator node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::sffloat, "set_fraction" },
            { NodeInterface::exposedField, FieldValue::mffloat, "key" },
            { NodeInterface::exposedField, FieldValue::mfrotation, "keyValue" },
            { NodeInterface::eventOut, FieldValue::sfrotation, "value_changed" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:OrientationInterpolator");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("OrientationInterpolator",
                                                  nodeInterfaceSet));
    }
    
    //
    // PixelTexture node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfimage, "image" },
            { NodeInterface::field, FieldValue::sfbool, "repeatS" },
            { NodeInterface::field, FieldValue::sfbool, "repeatT" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:PixelTexture");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PixelTexture",
                                                  nodeInterfaceSet));
    }
    
    //
    // PlaneSensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfbool, "autoOffset" },
            { NodeInterface::exposedField, FieldValue::sfbool, "enabled" },
            { NodeInterface::exposedField, FieldValue::sfvec2f, "maxPosition" },
            { NodeInterface::exposedField, FieldValue::sfvec2f, "minPosition" },
            { NodeInterface::exposedField, FieldValue::sffloat, "offset" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isActive" },
            { NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed" },
            { NodeInterface::eventOut, FieldValue::sfvec3f, "translation_changed" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:PlaneSensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PlaneSensor",
                                                  nodeInterfaceSet));
    }
    
    //
    // PointLight node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "attenuation" },
            { NodeInterface::exposedField, FieldValue::sfcolor, "color" },
            { NodeInterface::exposedField, FieldValue::sffloat, "intensity" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "location" },
            { NodeInterface::exposedField, FieldValue::sfbool, "on" },
            { NodeInterface::exposedField, FieldValue::sffloat, "radius" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:PointLight");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PointLight",
                                                  nodeInterfaceSet));
    }
    
    //
    // PointSet node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfnode, "color" },
            { NodeInterface::exposedField, FieldValue::sfnode, "coord" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:PointSet");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PointSet",
                                                  nodeInterfaceSet));
    }
    
    //
    // PositionInterpolator node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::sffloat, "set_fraction" },
            { NodeInterface::exposedField, FieldValue::mffloat, "key" },
            { NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue" },
            { NodeInterface::eventOut, FieldValue::sfvec3f, "value_changed" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:PositionInterpolator");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PositionInterpolator",
                                                  nodeInterfaceSet));
    }
    
    //
    // ProximitySensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfvec3f, "center" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "size" },
            { NodeInterface::exposedField, FieldValue::sfbool, "enabled" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isActive" },
            { NodeInterface::eventOut, FieldValue::sfvec3f, "position_changed" },
            { NodeInterface::eventOut, FieldValue::sfrotation, "orientation_changed" },
            { NodeInterface::eventOut, FieldValue::sftime, "enterTime" },
            { NodeInterface::eventOut, FieldValue::sftime, "exitTime" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:ProximitySensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("ProximitySensor",
                                                  nodeInterfaceSet));
    }
    
    //
    // ScalarInterpolator node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::sffloat, "set_fraction" },
            { NodeInterface::exposedField, FieldValue::mffloat, "key" },
            { NodeInterface::exposedField, FieldValue::mffloat, "keyValue" },
            { NodeInterface::eventOut, FieldValue::sffloat, "value_changed" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:ScalarInterpolator");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("ScalarInterpolator",
                                                  nodeInterfaceSet));
    }
    
    //
    // Shape node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfnode, "appearance" },
            { NodeInterface::exposedField, FieldValue::sfnode, "geometry" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:Shape");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Shape", nodeInterfaceSet));
    }
    
    //
    // Sound node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfvec3f, "direction" },
            { NodeInterface::exposedField, FieldValue::sffloat, "intensity" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "location" },
            { NodeInterface::exposedField, FieldValue::sffloat, "maxBack" },
            { NodeInterface::exposedField, FieldValue::sffloat, "maxFront" },
            { NodeInterface::exposedField, FieldValue::sffloat, "minBack" },
            { NodeInterface::exposedField, FieldValue::sffloat, "minFront" },
            { NodeInterface::exposedField, FieldValue::sffloat, "priority" },
            { NodeInterface::exposedField, FieldValue::sfnode, "source" },
            { NodeInterface::field, FieldValue::sfbool, "spatialize" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:Sound");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Sound", nodeInterfaceSet));
    }
    
    //
    // Sphere node
    //
    {
        static const NodeInterface nodeInterface =
                { NodeInterface::field, FieldValue::sffloat, "radius" };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:Sphere");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Sphere", nodeInterfaceSet));
    }
    
    //
    // SphereSensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfbool, "autoOffset" },
            { NodeInterface::exposedField, FieldValue::sfbool, "enabled" },
            { NodeInterface::exposedField, FieldValue::sffloat, "offset" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isActive" },
            { NodeInterface::eventOut, FieldValue::sfrotation, "rotation_changed" },
            { NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:SphereSensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("SphereSensor",
                                                  nodeInterfaceSet));
    }
    
    //
    // SpotLight node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "attenuation" },
            { NodeInterface::exposedField, FieldValue::sffloat, "beamWidth" },
            { NodeInterface::exposedField, FieldValue::sfcolor, "color" },
            { NodeInterface::exposedField, FieldValue::sffloat, "cutOffAngle" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "direction" },
            { NodeInterface::exposedField, FieldValue::sffloat, "intensity" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "location" },
            { NodeInterface::exposedField, FieldValue::sfbool, "on" },
            { NodeInterface::exposedField, FieldValue::sffloat, "radius" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:SpotLight");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("SpotLight",
                                                  nodeInterfaceSet));
    }
    
    //
    // Switch node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::mfnode, "choice" },
            { NodeInterface::exposedField, FieldValue::sfint32, "whichChoice" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:Switch");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Switch", nodeInterfaceSet));
    }
    
    //
    // Text node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::mfstring, "string" },
            { NodeInterface::exposedField, FieldValue::sfnode, "fontStyle" },
            { NodeInterface::exposedField, FieldValue::mffloat, "length" },
            { NodeInterface::exposedField, FieldValue::sffloat, "maxExtent" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:Text");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Text", nodeInterfaceSet));
    }
    
    //
    // TextureCoordinate node
    //
    {
        static const NodeInterface nodeInterface =
                { NodeInterface::exposedField, FieldValue::mfvec2f, "point" };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:TextureCoordinate");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("TextureCoordinate",
                                                  nodeInterfaceSet));
    }
    
    //
    // TextureTransform node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfvec2f, "center" },
            { NodeInterface::exposedField, FieldValue::sffloat, "rotation" },
            { NodeInterface::exposedField, FieldValue::sfvec2f, "scale" },
            { NodeInterface::exposedField, FieldValue::sfvec2f, "translation" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:TextureTransform");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("TextureTransform",
                                                  nodeInterfaceSet));
    }
    
    //
    // TimeSensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sftime, "cycleInterval" },
            { NodeInterface::exposedField, FieldValue::sfbool, "enabled" },
            { NodeInterface::exposedField, FieldValue::sfbool, "loop" },
            { NodeInterface::exposedField, FieldValue::sftime, "startTime" },
            { NodeInterface::exposedField, FieldValue::sftime, "stopTime" },
            { NodeInterface::eventOut, FieldValue::sftime, "cycleTime" },
            { NodeInterface::eventOut, FieldValue::sffloat, "fraction_changed" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isActive" },
            { NodeInterface::eventOut, FieldValue::sftime, "time" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:TimeSensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("TimeSensor",
                                                  nodeInterfaceSet));
    }
    
    //
    // TouchSensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfbool, "enabled" },
            { NodeInterface::eventOut, FieldValue::sfvec3f, "hitNormal_changed" },
            { NodeInterface::eventOut, FieldValue::sfvec3f, "hitPoint_changed" },
            { NodeInterface::eventOut, FieldValue::sfvec2f, "hitTexCoord_changed" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isActive" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isOver" },
            { NodeInterface::eventOut, FieldValue::sftime, "touchTime" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:TouchSensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("TouchSensor",
                                                  nodeInterfaceSet));
    }
    
    //
    // Transform node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::mfnode, "addChildren" },
            { NodeInterface::eventIn, FieldValue::mfnode, "removeChildren" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "center" },
            { NodeInterface::exposedField, FieldValue::mfnode, "children" },
            { NodeInterface::exposedField, FieldValue::sfrotation, "rotation" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "scale" },
            { NodeInterface::exposedField, FieldValue::sfrotation, "scaleOrientation" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "translation" },
            { NodeInterface::field, FieldValue::sfvec3f, "bboxCenter" },
            { NodeInterface::field, FieldValue::sfvec3f, "bboxSize" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:Transform");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Transform",
                                                  nodeInterfaceSet));
    }
    
    //
    // Viewpoint node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::eventIn, FieldValue::sfbool, "set_bind" },
            { NodeInterface::exposedField, FieldValue::sffloat, "fieldOfView" },
            { NodeInterface::exposedField, FieldValue::sfbool, "jump" },
            { NodeInterface::exposedField, FieldValue::sfrotation, "orientation" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "position" },
            { NodeInterface::field, FieldValue::sfstring, "description" },
            { NodeInterface::eventOut, FieldValue::sftime, "bindTime" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isBound" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:Viewpoint");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Viewpoint",
                                                  nodeInterfaceSet));
    }
    
    //
    // VisibilitySensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::exposedField, FieldValue::sfvec3f, "center" },
            { NodeInterface::exposedField, FieldValue::sfbool, "enabled" },
            { NodeInterface::exposedField, FieldValue::sfvec3f, "size" },
            { NodeInterface::eventOut, FieldValue::sftime, "enterTime" },
            { NodeInterface::eventOut, FieldValue::sftime, "exitTime" },
            { NodeInterface::eventOut, FieldValue::sfbool, "isActive" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:VisibilitySensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("VisibilitySensor",
                                                  nodeInterfaceSet));
    }
    
    //
    // WorldInfo node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            { NodeInterface::field, FieldValue::mfstring, "info" },
            { NodeInterface::field, FieldValue::sfstring, "title" }
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:WorldInfo");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("WorldInfo",
                                                  nodeInterfaceSet));
    }
}

Vrml97RootNamespace::~Vrml97RootNamespace() {}

} // namespace OpenVRML

