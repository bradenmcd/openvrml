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
# include "System.h"
# include "script.h"

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


/**
 * @class Vrml97RootNamespace
 *
 * @brief Root namespace for VRML97 scenes.
 *
 * Vrml97RootNamespace is initialized with the VRML97 node types.
 */

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

/**
 * @brief Constructor.
 *
 * @param nodeClassMap  a map containing @link NodeClass NodeClasses@endlink
 *                      for the VRML97 nodes.
 */
Vrml97RootNamespace::Vrml97RootNamespace(const NodeClassMap & nodeClassMap) {
    NodeClassMap::const_iterator pos;
    
    //
    // Anchor node
    //
    static const NodeInterface anchorInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfstring, "description"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "parameter"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
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
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "material"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "texture"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "textureTransform")
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
        NodeInterface(NodeInterface::exposedField, FieldValue::sfstring, "description"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "loop"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "pitch"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "startTime"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "stopTime"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "duration_changed"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive")
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
        NodeInterface(NodeInterface::eventIn, FieldValue::sfbool, "set_bind"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "groundAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfcolor, "groundColor"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "backUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "bottomUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "frontUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "leftUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "rightUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "topUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "skyAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfcolor, "skyColor"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isBound")
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
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "axisOfRotation"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
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
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "size");
    static const Vrml97NodeInterfaceSet_
            boxInterfaceSet(&boxInterface, &boxInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Box");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Box", boxInterfaceSet));
    
    //
    // Collision node
    //
    static const NodeInterface collisionInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "collide"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize"),
        NodeInterface(NodeInterface::field, FieldValue::sfnode, "proxy"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "collideTime")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::mfcolor, "color");
    static const Vrml97NodeInterfaceSet_
            colorInterfaceSet(&colorInterface, &colorInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Color");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Color", colorInterfaceSet));
    
    //
    // ColorInterpolator node
    //
    static const NodeInterface colorInterpolatorInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfcolor, "keyValue"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfcolor, "value_changed")
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
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "bottomRadius"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "height"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "side"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "bottom")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "point");
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
        NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue"),
        NodeInterface(NodeInterface::eventOut, FieldValue::mfvec3f, "value_changed")
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
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "bottom"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "height"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "radius"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "side"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "top")
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
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "autoOffset"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "diskAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "maxAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "minAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "offset"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfrotation, "rotation_changed"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed")
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
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "color"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "direction"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "intensity"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "on")
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
        NodeInterface(NodeInterface::eventIn, FieldValue::mffloat, "set_height"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "color"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "normal"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "texCoord"),
        NodeInterface(NodeInterface::field, FieldValue::mffloat, "height"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "ccw"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "colorPerVertex"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "creaseAngle"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "normalPerVertex"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "solid"),
        NodeInterface(NodeInterface::field, FieldValue::sfint32, "xDimension"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "xSpacing"),
        NodeInterface(NodeInterface::field, FieldValue::sfint32, "zDimension"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "zSpacing")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::mfvec2f, "set_crossSection"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfrotation, "set_orientation"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfvec2f, "set_scale"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfvec3f, "set_spine"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "beginCap"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "ccw"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "convex"),
            NodeInterface(NodeInterface::field, FieldValue::sffloat, "creaseAngle"),
            NodeInterface(NodeInterface::field, FieldValue::mfvec2f, "crossSection"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "endCap"),
            NodeInterface(NodeInterface::field, FieldValue::mfrotation, "orientation"),
            NodeInterface(NodeInterface::field, FieldValue::mfvec2f, "scale"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "solid"),
            NodeInterface(NodeInterface::field, FieldValue::mfvec3f, "spine")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::sfbool, "set_bind"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfstring, "fogType"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "visibilityRange"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isBound")
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
            NodeInterface(NodeInterface::field, FieldValue::mfstring, "family"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "horizontal"),
            NodeInterface(NodeInterface::field, FieldValue::mfstring, "justify"),
            NodeInterface(NodeInterface::field, FieldValue::sfstring, "language"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "leftToRight"),
            NodeInterface(NodeInterface::field, FieldValue::sffloat, "size"),
            NodeInterface(NodeInterface::field, FieldValue::sffloat, "spacing"),
            NodeInterface(NodeInterface::field, FieldValue::sfstring, "style"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "topToBottom")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatS"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatT")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_colorIndex"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_coordIndex"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_normalIndex"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_texCoordIndex"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "coord"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "normal"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "texCoord"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "ccw"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "colorIndex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "colorPerVertex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "convex"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "coordIndex"),
            NodeInterface(NodeInterface::field, FieldValue::sffloat, "creaseAngle"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "normalIndex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "normalPerVertex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "solid"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "texCoordIndex")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_colorIndex"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_coordIndex"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "coord"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "colorIndex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "colorPerVertex"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "coordIndex")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "level"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "center"),
            NodeInterface(NodeInterface::field, FieldValue::mffloat, "range")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "diffuseColor"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "emissiveColor"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "shininess"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "specularColor"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "transparency")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "loop"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "speed"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "startTime"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "stopTime"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatS"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatT"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "duration_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::sfbool, "set_bind"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "avatarSize"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "headlight"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "speed"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "type"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "visibilityLimit"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isBound")
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
                NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "vector");
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
            NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue"),
            NodeInterface(NodeInterface::eventOut, FieldValue::mfvec3f, "value_changed")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfrotation, "keyValue"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfrotation, "value_changed")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfimage, "image"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatS"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatT")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "autoOffset"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "maxPosition"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "minPosition"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "offset"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "translation_changed")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "attenuation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "intensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "location"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "on"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "radius")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "coord")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "value_changed")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "center"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "size"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "position_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfrotation, "orientation_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "enterTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "exitTime")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "keyValue"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sffloat, "value_changed")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "appearance"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "geometry")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "direction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "intensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "location"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "maxBack"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "maxFront"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "minBack"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "minFront"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "priority"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "source"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "spatialize")
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
                NodeInterface(NodeInterface::field, FieldValue::sffloat, "radius");
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "autoOffset"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "offset"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfrotation, "rotation_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "attenuation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "beamWidth"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "cutOffAngle"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "direction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "intensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "location"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "on"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "radius")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "choice"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfint32, "whichChoice")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "string"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "fontStyle"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "length"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "maxExtent")
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
                NodeInterface(NodeInterface::exposedField, FieldValue::mfvec2f, "point");
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "center"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "rotation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "scale"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "translation")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "cycleInterval"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "loop"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "startTime"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "stopTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "cycleTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sffloat, "fraction_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "time")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "hitNormal_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "hitPoint_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec2f, "hitTexCoord_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isOver"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "touchTime")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "center"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfrotation, "rotation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "scale"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfrotation, "scaleOrientation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "translation"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
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
            NodeInterface(NodeInterface::eventIn, FieldValue::sfbool, "set_bind"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "fieldOfView"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "jump"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfrotation, "orientation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "position"),
            NodeInterface(NodeInterface::field, FieldValue::sfstring, "description"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "bindTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isBound")
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
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "center"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "size"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "enterTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "exitTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive")
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
            NodeInterface(NodeInterface::field, FieldValue::mfstring, "info"),
            NodeInterface(NodeInterface::field, FieldValue::sfstring, "title")
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

