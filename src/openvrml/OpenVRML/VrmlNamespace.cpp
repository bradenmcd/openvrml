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

# include <stack>
# include "VrmlNamespace.h"
# include "node.h"
# include "Route.h"
# include "System.h"
# include "script.h"
# include "VrmlNodeProto.h"

# ifndef NDEBUG
#   include <typeinfo>
# endif

using namespace OpenVRML;

/**
 * @class OpenVRML::VrmlNamespace
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

// This should at least be a sorted vector...
std::list<OpenVRML::NodeTypePtr> OpenVRML::VrmlNamespace::builtInList;

// Ref count of namespaces so builtins can be freed
int OpenVRML::VrmlNamespace::s_nNamespaces;

OpenVRML::VrmlNamespace::VrmlNamespace(VrmlNamespace * parent):
        d_parent(parent) {
    ++s_nNamespaces;

    // Initialize typeList with built in nodes
    if (builtInList.empty()) {
        defineBuiltIns();
    }
}

OpenVRML::VrmlNamespace::~VrmlNamespace() {
    // Free builtins
    if ( --s_nNamespaces == 0 ) {
        builtInList.clear();
    }
}

void OpenVRML::VrmlNamespace::addBuiltIn(const NodeTypePtr & nodeType) {
    builtInList.push_front(nodeType);
}

void OpenVRML::VrmlNamespace::defineBuiltIns()
{
  addBuiltIn( NodeAnchor::defineType() );
  addBuiltIn( NodeAppearance::defineType() );
  addBuiltIn( NodeAudioClip::defineType() );
  addBuiltIn( NodeBackground::defineType() );
  addBuiltIn( NodeBillboard::defineType() );
  addBuiltIn( NodeBox::defineType() );
  addBuiltIn( NodeCollision::defineType() );
  addBuiltIn( NodeColor::defineType() );
  addBuiltIn( NodeColorInt::defineType() );
  addBuiltIn( NodeCone::defineType() );
  addBuiltIn( NodeCoordinate::defineType() );
  addBuiltIn( NodeCoordinateInt::defineType() );
  addBuiltIn( NodeCylinder::defineType() );
  addBuiltIn( NodeCylinderSensor::defineType() );
  addBuiltIn( NodeDirLight::defineType() );
  addBuiltIn( NodeElevationGrid::defineType() );
  addBuiltIn( NodeExtrusion::defineType() );
  addBuiltIn( NodeFog::defineType() );
  addBuiltIn( NodeFontStyle::defineType() );
  addBuiltIn( NodeGroup::defineType() );
  addBuiltIn( NodeIFaceSet::defineType() );
  addBuiltIn( NodeILineSet::defineType() );
  addBuiltIn( NodeImageTexture::defineType() );
  addBuiltIn( NodeInline::defineType() );
  addBuiltIn( NodeLOD::defineType() );
  addBuiltIn( NodeMaterial::defineType() );
  addBuiltIn( NodeMovieTexture::defineType() );
  addBuiltIn( NodeNavigationInfo::defineType() );
  addBuiltIn( NodeNormal::defineType() );
  addBuiltIn( NodeNormalInt::defineType() );
  addBuiltIn( NodeOrientationInt::defineType() );
  addBuiltIn( NodePixelTexture::defineType() );
  addBuiltIn( NodePlaneSensor::defineType() );
  addBuiltIn( NodePointLight::defineType() );
  addBuiltIn( NodePointSet::defineType() );
  addBuiltIn( NodePositionInt::defineType() );
  addBuiltIn( NodeProximitySensor::defineType() );
  addBuiltIn( NodeScalarInt::defineType() );
  addBuiltIn(ScriptNode::defineType());
  addBuiltIn( NodeShape::defineType() );
  addBuiltIn( NodeSound::defineType() );
  addBuiltIn( NodeSphere::defineType() );
  addBuiltIn( NodeSphereSensor::defineType() );
  addBuiltIn( NodeSpotLight::defineType() );
  addBuiltIn( NodeSwitch::defineType() );
  addBuiltIn( NodeText::defineType() );
  addBuiltIn( NodeTextureCoordinate::defineType() );
  addBuiltIn( NodeTextureTransform::defineType() );
  addBuiltIn( NodeTimeSensor::defineType() );
  addBuiltIn( NodeTouchSensor::defineType() );
  addBuiltIn( NodeTransform::defineType() );
  addBuiltIn( NodeViewpoint::defineType() );
  addBuiltIn( NodeVisibilitySensor::defineType() );
  addBuiltIn( NodeWorldInfo::defineType() );
}


/**
 * @brief Add a node type.
 *
 * Print an error message if the argument type is already defined.
 *
 * @param nodeType a NodeTypePtr
 *
 * @todo Throw std::invalid_argument if the argument type is already defined.
 */
void OpenVRML::VrmlNamespace::addNodeType(const NodeTypePtr & nodeType) {
    assert(!this->findType(nodeType->getId()));
    this->d_typeList.push_front(nodeType);
}

/**
 * @brief Find a node type, given a type name. Returns NULL if type is
 *      not defined.
 */
const OpenVRML::NodeTypePtr
        OpenVRML::VrmlNamespace::findType(const std::string & name) const {
    // Look through the PROTO stack:
    NodeTypePtr nt(findPROTO(name));
    if (nt) {
        return nt;
    }
    
    // Look in parent scope for the type
    if (d_parent) {
        return d_parent->findType(name);
    }
    
    // Look through the built ins
    for (std::list<NodeTypePtr>::iterator i = builtInList.begin();
            i != builtInList.end(); ++i) {
        nt = *i;
        assert(nt);
        if (name == nt->getId()) {
            return nt;
        }
    }
    
    return NodeTypePtr(0);
}

/**
 * @brief Find a nodeType, given a PROTO name.
 */
const OpenVRML::NodeTypePtr
        OpenVRML::VrmlNamespace::findPROTO(const std::string & name) const {
    for (std::list<NodeTypePtr>::const_iterator i(d_typeList.begin());
          i != d_typeList.end(); ++i) {
        assert(*i);
        if (name == (*i)->getId()) {
	    return *i;
        }
    }
    return NodeTypePtr(0);
}


const OpenVRML::NodeTypePtr OpenVRML::VrmlNamespace::firstType() const {
    // Top of the PROTO stack (should make sure it has an implementation...)
    if (!this->d_typeList.empty()) {
        return d_typeList.front();
    }
    return NodeTypePtr(0);
}

void OpenVRML::VrmlNamespace::addNodeName(Node & namedNode) {
  // We could remove any existing node with this name, but
  // since we are just pushing this one onto the front of
  // the list, the other name won't be found. If we do
  // something smart with this list (like sorting), this
  // will need to change.
  d_nameList.push_front(&namedNode);
}

void OpenVRML::VrmlNamespace::removeNodeName(Node & namedNode) {
    for (std::list<Node *>::iterator i(this->d_nameList.begin());
            i != this->d_nameList.end(); ++i) {
        if (*i == &namedNode) {
            this->d_nameList.erase(i);
            return;
        }
    }
}


OpenVRML::Node *
        OpenVRML::VrmlNamespace::findNode(const std::string & nodeId) const {
    for (std::list<Node *>::const_iterator nodeItr(d_nameList.begin());
            nodeItr != d_nameList.end(); ++nodeItr) {
        if (nodeId == (*nodeItr)->getId()) {
            return *nodeItr;
        }
    }
    return 0;
}

namespace {
    
    class NodeCloneVisitor : public NodeVisitor {
    public:
        NodeCloneVisitor(VrmlNamespace & ns): ns(ns) {}
        
        virtual ~NodeCloneVisitor() {}
        
        const MFNode clone(const MFNode & mfnode) {
            MFNode clonedNodes(this->visitChildren(mfnode));
            assert(this->rootNodeStack.size() == 0);
            
            for (size_t i = 0; i < mfnode.getLength(); ++i) {
                mfnode.getElement(i)->resetVisitedFlag();
            }
            
            return clonedNodes;
        }
        
        virtual void visit(NodeAnchor & node) {
            const NodePtr clonedNode(new NodeAnchor(node));
            this->rootNodeStack.push(clonedNode);
            clonedNode->toAnchor()
                    ->setChildren(this->visitChildren(node.getChildren()));
        }
        
        virtual void visit(NodeAppearance & node) {
            const NodePtr clonedNode(new NodeAppearance(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getMaterial().get()) {
                this->visitNode(*node.getMaterial().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toMaterial());
                clonedNode->toAppearance()
                        ->setMaterial(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getTexture().get()) {
                this->visitNode(*node.getTexture().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toTexture());
                clonedNode->toAppearance()
                        ->setTexture(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getTextureTransform().get()) {
                this->visitNode(*node.getTextureTransform().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toTextureTransform());
                clonedNode->toAppearance()
                        ->setTextureTransform(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(NodeAudioClip & node) {
            this->rootNodeStack.push(NodePtr(new NodeAudioClip(node)));
        }
        
        virtual void visit(NodeBackground & node) {
            this->rootNodeStack.push(NodePtr(new NodeBackground(node)));
        }
        
        virtual void visit(NodeBillboard & node) {
            const NodePtr clonedNode(new NodeBillboard(node));
            this->rootNodeStack.push(clonedNode);
            clonedNode->toBillboard()
                    ->setChildren(this->visitChildren(node.getChildren()));
        }
        
        virtual void visit(NodeBox & node) {
            this->rootNodeStack.push(NodePtr(new NodeBox(node)));
        }
        
        virtual void visit(NodeCollision & node) {
            const NodePtr clonedNode(new NodeCollision(node));
            this->rootNodeStack.push(clonedNode);
            clonedNode->toCollision()
                    ->setChildren(this->visitChildren(node.getChildren()));
            if (node.getProxy().get()) {
                this->visitNode(*node.getProxy().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toChild());
                clonedNode->toCollision()
                        ->setProxy(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(NodeColor & node) {
            this->rootNodeStack.push(NodePtr(new NodeColor(node)));
        }
        
        virtual void visit(NodeColorInt & node) {
            this->rootNodeStack.push(NodePtr(new NodeColorInt(node)));
        }
        
        virtual void visit(NodeCone & node) {
            this->rootNodeStack.push(NodePtr(new NodeCone(node)));
        }
        
        virtual void visit(NodeCoordinate & node) {
            this->rootNodeStack.push(NodePtr(new NodeCoordinate(node)));
        }
        
        virtual void visit(NodeCoordinateInt & node) {
            this->rootNodeStack.push(NodePtr(new NodeCoordinateInt(node)));
        }
        
        virtual void visit(NodeCylinder & node) {
            this->rootNodeStack.push(NodePtr(new NodeCylinder(node)));
        }
        
        virtual void visit(NodeCylinderSensor & node) {
            this->rootNodeStack.push(NodePtr(new NodeCylinderSensor(node)));
        }
        
        virtual void visit(NodeDirLight & node) {
            this->rootNodeStack.push(NodePtr(new NodeDirLight(node)));
        }
        
        virtual void visit(NodeElevationGrid & node) {
            const NodePtr clonedNode(new NodeElevationGrid(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getColor().get()) {
                this->visitNode(*node.getColor().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toColor());
                clonedNode->toElevationGrid()
                        ->setColor(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getNormal().get()) {
                this->visitNode(*node.getNormal().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toNormal());
                clonedNode->toElevationGrid()
                        ->setNormal(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getTexCoord().get()) {
                this->visitNode(*node.getTexCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toTextureCoordinate());
                clonedNode->toElevationGrid()
                        ->setTexCoord(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(NodeExtrusion & node) {
            this->rootNodeStack.push(NodePtr(new NodeExtrusion(node)));
        }
        
        virtual void visit(NodeFog & node) {
            this->rootNodeStack.push(NodePtr(new NodeFog(node)));
        }
        
        virtual void visit(NodeFontStyle & node) {
            this->rootNodeStack.push(NodePtr(new NodeFontStyle(node)));
        }
        
        virtual void visit(NodeGroup & node) {
            const NodePtr clonedNode(new NodeGroup(node));
            this->rootNodeStack.push(clonedNode);
            static_cast<NodeGroup *>(clonedNode.get())
                    ->setChildren(this->visitChildren(node.getChildren()));
        }
        
        virtual void visit(NodeIFaceSet & node) {
            const NodePtr clonedNode(new NodeIFaceSet(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getColor().get()) {
                this->visitNode(*node.getColor().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toColor());
                static_cast<NodeIFaceSet *>(clonedNode.get())
                        ->setColor(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getCoord().get()) {
                this->visitNode(*node.getCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toCoordinate());
                static_cast<NodeIFaceSet *>(clonedNode.get())
                        ->setCoord(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getNormal().get()) {
                this->visitNode(*node.getNormal().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toNormal());
                static_cast<NodeIFaceSet *>(clonedNode.get())
                        ->setNormal(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getTexCoord().get()) {
                this->visitNode(*node.getTexCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toTextureCoordinate());
                static_cast<NodeIFaceSet *>(clonedNode.get())
                        ->setTexCoord(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(NodeILineSet & node) {
            const NodePtr clonedNode(new NodeILineSet(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getColor().get()) {
                this->visitNode(*node.getColor().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toColor());
                static_cast<NodeILineSet *>(clonedNode.get())
                        ->setColor(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getCoord().get()) {
                this->visitNode(*node.getCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toCoordinate());
                static_cast<NodeILineSet *>(clonedNode.get())
                        ->setCoord(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(NodeImageTexture & node) {
            this->rootNodeStack.push(NodePtr(new NodeImageTexture(node)));
        }
        
        virtual void visit(NodeInline & node) {
            this->rootNodeStack.push(NodePtr(new NodeInline(node)));
        }
        
        virtual void visit(NodeLOD & node) {
            const NodePtr clonedNode(new NodeLOD(node));
            this->rootNodeStack.push(clonedNode);
            static_cast<NodeLOD *>(clonedNode.get())
                    ->setLevel(this->visitChildren(node.getLevel()));
        }
        
        virtual void visit(NodeMaterial & node) {
            this->rootNodeStack.push(NodePtr(new NodeMaterial(node)));
        }
        
        virtual void visit(NodeMovieTexture & node) {
            this->rootNodeStack.push(NodePtr(new NodeMovieTexture(node)));
        }
        
        virtual void visit(NodeNavigationInfo & node) {
            this->rootNodeStack.push(NodePtr(new NodeNavigationInfo(node)));
        }
        
        virtual void visit(NodeNormal & node) {
            this->rootNodeStack.push(NodePtr(new NodeNormal(node)));
        }
        
        virtual void visit(NodeNormalInt & node) {
            this->rootNodeStack.push(NodePtr(new NodeNormalInt(node)));
        }
        
        virtual void visit(NodeOrientationInt & node) {
            this->rootNodeStack.push(NodePtr(new NodeOrientationInt(node)));
        }
        
        virtual void visit(NodePixelTexture & node) {
            this->rootNodeStack.push(NodePtr(new NodePixelTexture(node)));
        }
        
        virtual void visit(NodePlaneSensor & node) {
            this->rootNodeStack.push(NodePtr(new NodePlaneSensor(node)));
        }
        
        virtual void visit(NodePointLight & node) {
            this->rootNodeStack.push(NodePtr(new NodePointLight(node)));
        }
        
        virtual void visit(NodePointSet & node) {
            const NodePtr clonedNode(new NodePointSet(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getColor().get()) {
                this->visitNode(*node.getColor().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toColor());
                static_cast<NodePointSet *>(clonedNode.get())
                        ->setColor(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getCoord().get()) {
                this->visitNode(*node.getCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toCoordinate());
                clonedNode.get()->toPointSet()
                        ->setCoord(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(NodePositionInt & node) {
            this->rootNodeStack.push(NodePtr(new NodePositionInt(node)));
        }
        
        virtual void visit(ProtoNode & node) {
            const NodePtr clonedNode(new ProtoNode(node));
            clonedNode->addToScene(node.scene(),
                                   clonedNode->type.getActualUrl());
            this->rootNodeStack.push(clonedNode);
        }
        
        virtual void visit(NodeProximitySensor & node) {
            this->rootNodeStack.push(NodePtr(new NodeProximitySensor(node)));
        }
        
        virtual void visit(NodeScalarInt & node) {
            this->rootNodeStack.push(NodePtr(new NodeScalarInt(node)));
        }
        
        virtual void visit(ScriptNode & node) {
            const NodePtr clonedNode(new ScriptNode(node));
            this->rootNodeStack.push(clonedNode);
            for (ScriptNode::FieldList::const_iterator 
                    i = node.fields().begin(),
                    j = static_cast<ScriptNode *>(clonedNode.get())
                            ->fields().begin();
                    i != node.fields().end(); ++i, ++j) {
                assert((*i)->value);
                if ((*i)->type == FieldValue::SFNODE) {
                    assert(dynamic_cast<SFNode *>((*i)->value));
                    if (static_cast<SFNode *>((*i)->value)->get()) {
                        visitNode(*static_cast<SFNode *>((*i)->value)->get());
                        static_cast<SFNode *>((*j)->value)
                                ->set(this->rootNodeStack.top());
                        this->rootNodeStack.pop();
                    }
                } else if ((*i)->type == FieldValue::MFNODE) {
                    assert(dynamic_cast<MFNode *>((*i)->value));
                    static_cast<MFNode &>(*(*j)->value) =
                        visitChildren(static_cast<MFNode &>(*(*i)->value));
                }
            }
        }
        
        virtual void visit(NodeShape & node) {
            const NodePtr clonedNode(new NodeShape(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getAppearance().get()) {
                this->visitNode(*node.getAppearance().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toAppearance());
                static_cast<NodeShape *>(clonedNode.get())
                        ->setAppearance(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getGeometry().get()) {
                this->visitNode(*node.getGeometry().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toGeometry());
                static_cast<NodeShape *>(clonedNode.get())
                        ->setGeometry(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        /**
         * @todo The assertion here will fail if a MovieTexture node is used
         *       as a sound source.
         */
        virtual void visit(NodeSound & node) {
            const NodePtr clonedNode(new NodeSound(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getSource().get()) {
                this->visitNode(*node.getSource().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toAudioClip());
                static_cast<NodeSound *>(clonedNode.get())
                        ->setSource(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(NodeSphere & node) {
            this->rootNodeStack.push(NodePtr(new NodeSphere(node)));
        }
        
        virtual void visit(NodeSphereSensor & node) {
            this->rootNodeStack.push(NodePtr(new NodeSphereSensor(node)));
        }
        
        virtual void visit(NodeSpotLight & node) {
            this->rootNodeStack.push(NodePtr(new NodeSpotLight(node)));
        }
        
        virtual void visit(NodeSwitch & node) {
            const NodePtr clonedNode(new NodeSwitch(node));
            this->rootNodeStack.push(clonedNode);
            static_cast<NodeSwitch *>(clonedNode.get())
                    ->setChoice(this->visitChildren(node.getChoice()));
        }
        
        virtual void visit(NodeText & node) {
            const NodePtr clonedNode(new NodeText(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getFontStyle().get()) {
                this->visitNode(*node.getFontStyle().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toFontStyle());
                static_cast<NodeText *>(clonedNode.get())
                        ->setFontStyle(SFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(NodeTextureCoordinate & node) {
            this->rootNodeStack.push(NodePtr(new NodeTextureCoordinate(node)));
        }
        
        virtual void visit(NodeTextureTransform & node) {
            this->rootNodeStack.push(NodePtr(new NodeTextureTransform(node)));
        }
        
        virtual void visit(NodeTimeSensor & node) {
            this->rootNodeStack.push(NodePtr(new NodeTimeSensor(node)));
        }
        
        virtual void visit(NodeTouchSensor & node) {
            this->rootNodeStack.push(NodePtr(new NodeTouchSensor(node)));
        }
        
        virtual void visit(NodeTransform & node) {
            const NodePtr clonedNode(new NodeTransform(node));
            this->rootNodeStack.push(clonedNode);
            static_cast<NodeTransform *>(clonedNode.get())
                    ->setChildren(this->visitChildren(node.getChildren()));
        }
        
        virtual void visit(NodeViewpoint & node) {
            this->rootNodeStack.push(NodePtr(new NodeViewpoint(node)));
        }
        
        virtual void visit(NodeVisibilitySensor & node) {
            this->rootNodeStack.push(NodePtr(new NodeVisibilitySensor(node)));
        }
        
        virtual void visit(NodeWorldInfo & node) {
            this->rootNodeStack.push(NodePtr(new NodeWorldInfo(node)));
        }
        
    private:
        //
        // Not copyable.
        //
        NodeCloneVisitor(const NodeCloneVisitor &);
        NodeCloneVisitor & operator=(const NodeCloneVisitor &);
        
        const MFNode visitChildren(const MFNode & children) {
            MFNode clonedChildren(children.getLength());
            for (size_t i = 0; i < clonedChildren.getLength(); ++i) {
                if (children.getElement(i)) {
                    this->visitNode(*children.getElement(i));
                    assert(this->rootNodeStack.top());
                    clonedChildren.setElement(i, this->rootNodeStack.top());
                    this->rootNodeStack.pop();
                }
            }
            return clonedChildren;
        }
        
        void visitNode(Node & node) {
            if (node.accept(*this)) {
                assert(this->rootNodeStack.size() > 0);
                assert(typeid(node) == typeid(*this->rootNodeStack.top()));
                const std::string & nodeId =
                        this->rootNodeStack.top()->getId();
                if (!this->rootNodeStack.top()->getId().empty()) {
                    this->ns.addNodeName(*this->rootNodeStack.top());
                }
            } else {
                assert(this->ns.findNode(node.getId()));
                this->rootNodeStack
                        .push(NodePtr(this->ns.findNode(node.getId())));
            }
        }
        
        VrmlNamespace & ns;
        std::stack<NodePtr> rootNodeStack;
    };
    
    class NodeRouteCopyVisitor : public NodeVisitor {
    public:
        NodeRouteCopyVisitor(VrmlNamespace & ns): ns(ns) {}
        
        virtual ~NodeRouteCopyVisitor() {}
        
        void copyRoutes(const MFNode & mfnode) {
            this->visitChildren(mfnode);
            
            for (size_t i = 0; i < mfnode.getLength(); ++i) {
                mfnode.getElement(i)->resetVisitedFlag();
            }
        }
        
        virtual void visit(NodeAnchor & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(NodeAppearance & node) {
            this->copyRoutesFromNode(node);
            if (node.getMaterial().get()) {
                node.getMaterial().get()->accept(*this);
            }
            if (node.getTexture().get()) {
                node.getTexture().get()->accept(*this);
            }
            if (node.getTextureTransform().get()) {
                node.getTextureTransform().get()->accept(*this);
            }
        }
        
        virtual void visit(NodeAudioClip & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeBackground & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeBillboard & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(NodeBox & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeCollision & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(NodeColor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeColorInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeCone & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeCoordinate & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeCoordinateInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeCylinder & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeCylinderSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeDirLight & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeElevationGrid & node) {
            this->copyRoutesFromNode(node);
            if (node.getColor().get()) {
                node.getColor().get()->accept(*this);
            }
            if (node.getNormal().get()) {
                node.getNormal().get()->accept(*this);
            }
            if (node.getTexCoord().get()) {
                node.getTexCoord().get()->accept(*this);
            }
        }
        
        virtual void visit(NodeExtrusion & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeFog & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeFontStyle & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeGroup & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(NodeIFaceSet & node) {
            this->copyRoutesFromNode(node);
            if (node.getColor().get()) {
                node.getColor().get()->accept(*this);
            }
            if (node.getCoord().get()) {
                node.getCoord().get()->accept(*this);
            }
            if (node.getNormal().get()) {
                node.getNormal().get()->accept(*this);
            }
            if (node.getTexCoord().get()) {
                node.getTexCoord().get()->accept(*this);
            }
        }
        
        virtual void visit(NodeILineSet & node) {
            this->copyRoutesFromNode(node);
            if (node.getColor().get()) {
                node.getColor().get()->accept(*this);
            }
            if (node.getCoord().get()) {
                node.getCoord().get()->accept(*this);
            }
        }
        
        virtual void visit(NodeImageTexture & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeInline & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeLOD & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getLevel());
        }
        
        virtual void visit(NodeMaterial & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeMovieTexture & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeNavigationInfo & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeNormal & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeNormalInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeOrientationInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodePixelTexture & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodePlaneSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodePointLight & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodePointSet & node) {
            this->copyRoutesFromNode(node);
            if (node.getColor().get()) {
                node.getColor().get()->accept(*this);
            }
            if (node.getCoord().get()) {
                node.getCoord().get()->accept(*this);
            }
       }
        
        virtual void visit(NodePositionInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(ProtoNode & node) {
            this->copyRoutesFromNode(node);
            //
            // What about the fields of a PROTO'd node?
            //
        }
        
        virtual void visit(NodeProximitySensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeScalarInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(ScriptNode & node) {
            this->copyRoutesFromNode(node);
            for (ScriptNode::FieldList::const_iterator
                    i = node.fields().begin(); i != node.fields().end(); ++i) {
                assert((*i)->value);
                if ((*i)->type == FieldValue::SFNODE) {
                    assert(dynamic_cast<SFNode *>((*i)->value));
                    if (static_cast<SFNode *>((*i)->value)->get()) {
                        static_cast<SFNode *>((*i)->value)->get()
                                ->accept(*this);
                    }
                } else if ((*i)->type == FieldValue::MFNODE) {
                    assert(dynamic_cast<MFNode *>((*i)->value));
                    this->visitChildren(static_cast<MFNode &>(*(*i)->value));
                }
            }
        }
        
        virtual void visit(NodeShape & node) {
            this->copyRoutesFromNode(node);
            if (node.getAppearance().get()) {
                node.getAppearance().get()->accept(*this);
            }
            if (node.getGeometry().get()) {
                node.getGeometry().get()->accept(*this);
            }
        }
        
        virtual void visit(NodeSound & node) {
            this->copyRoutesFromNode(node);
            if (node.getSource().get()) {
                node.getSource().get()->accept(*this);
            }
        }
        
        virtual void visit(NodeSphere & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeSphereSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeSpotLight & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeSwitch & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChoice());
        }
        
        virtual void visit(NodeText & node) {
            this->copyRoutesFromNode(node);
            if (node.getFontStyle().get()) {
                node.getFontStyle().get()->accept(*this);
            }
        }
        
        virtual void visit(NodeTextureCoordinate & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeTextureTransform & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeTimeSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeTouchSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeTransform & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(NodeViewpoint & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeVisibilitySensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(NodeWorldInfo & node) {
            this->copyRoutesFromNode(node);
        }
        
    private:
        //
        // Not copyable.
        //
        NodeRouteCopyVisitor(const NodeRouteCopyVisitor &);
        NodeRouteCopyVisitor & operator=(const NodeRouteCopyVisitor &);
        
        void copyRoutesFromNode(Node & node) {
            const std::string & fromNodeId = node.getId();
            if (!fromNodeId.empty()) {
                Node * fromNode = this->ns.findNode(fromNodeId);
                assert(fromNode);
		
		Node::RouteList routes = node.getRoutes();

		Node::RouteList::iterator i;
		
		for (i = routes.begin(); i != routes.end(); ++i)
		{
		  const std::string& toNodeId = (*i)->toNode->getId();
		  assert(this->ns.findNode(toNodeId));
		  fromNode->addRoute((*i)->fromEventOut,
				     NodePtr(this->ns.findNode(toNodeId)),
				     (*i)->toEventIn);
		}
            }
        }
        
        void visitChildren(const MFNode & children) {
            for (size_t i = 0; i < children.getLength(); ++i) {
                if (children.getElement(i)) {
                    children.getElement(i)->accept(*this);
                }
            }
        }
        
        VrmlNamespace & ns;
    };
}

const OpenVRML::MFNode
        OpenVRML::VrmlNamespace::cloneNodes(const MFNode & mfnode) {
    MFNode clonedNodes(NodeCloneVisitor(*this).clone(mfnode));
    NodeRouteCopyVisitor(*this).copyRoutes(mfnode);
    return clonedNodes;
}
