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
# include "VrmlNodeType.h"
# include "VrmlNodeVisitor.h"
# include "VrmlNode.h"
# include "Route.h"
# include "System.h"
# include "VrmlNodeScript.h"
# include "VrmlNodeProto.h"

# include <string.h>

# ifndef NDEBUG
#   include <typeinfo>
# endif

// This should at least be a sorted vector...
std::list< VrmlNodeType* > VrmlNamespace::builtInList;

// Ref count of namespaces so builtins can be freed
int VrmlNamespace::s_nNamespaces;

VrmlNamespace::VrmlNamespace( VrmlNamespace *parent ) :
  d_parent(parent)
{
  ++s_nNamespaces;

  // Initialize typeList with built in nodes
  if (builtInList.size() == 0) defineBuiltIns();
}

VrmlNamespace::~VrmlNamespace()
{
  // Free typeList
  std::list<VrmlNodeType*>::iterator i;
  for (i = d_typeList.begin(); i != d_typeList.end(); ++i)
    (*i)->dereference();

  // Free builtins
  if ( --s_nNamespaces == 0 )
    {
      for (i = builtInList.begin(); i != builtInList.end(); ++i)
	(*i)->dereference();
      builtInList.clear();
    }
}


//
//  Built in nodes.
//  This code replaces the reading of the "standardNodes.wrl" file
//  of empty PROTOs so I don't need to carry that file around.
//

void
VrmlNamespace::addBuiltIn( VrmlNodeType *type)
{
  builtInList.push_front( type->reference() );
}




void VrmlNamespace::defineBuiltIns()
{
  addBuiltIn( VrmlNodeAnchor::defineType() );
  addBuiltIn( VrmlNodeAppearance::defineType() );
  addBuiltIn( VrmlNodeAudioClip::defineType() );
  addBuiltIn( VrmlNodeBackground::defineType() );
  addBuiltIn( VrmlNodeBillboard::defineType() );
  addBuiltIn( VrmlNodeBox::defineType() );
  addBuiltIn( VrmlNodeCollision::defineType() );
  addBuiltIn( VrmlNodeColor::defineType() );
  addBuiltIn( VrmlNodeColorInt::defineType() );
  addBuiltIn( VrmlNodeCone::defineType() );
  addBuiltIn( VrmlNodeCoordinate::defineType() );
  addBuiltIn( VrmlNodeCoordinateInt::defineType() );
  addBuiltIn( VrmlNodeCylinder::defineType() );
  addBuiltIn( VrmlNodeCylinderSensor::defineType() );
  addBuiltIn( VrmlNodeDirLight::defineType() );
  addBuiltIn( VrmlNodeElevationGrid::defineType() );
  addBuiltIn( VrmlNodeExtrusion::defineType() );
  addBuiltIn( VrmlNodeFog::defineType() );
  addBuiltIn( VrmlNodeFontStyle::defineType() );
  addBuiltIn( VrmlNodeGroup::defineType() );
  addBuiltIn( VrmlNodeIFaceSet::defineType() );
  addBuiltIn( VrmlNodeILineSet::defineType() );
  addBuiltIn( VrmlNodeImageTexture::defineType() );
  addBuiltIn( VrmlNodeInline::defineType() );
  addBuiltIn( VrmlNodeLOD::defineType() );
  addBuiltIn( VrmlNodeMaterial::defineType() );
  addBuiltIn( VrmlNodeMovieTexture::defineType() );
  addBuiltIn( VrmlNodeNavigationInfo::defineType() );
  addBuiltIn( VrmlNodeNormal::defineType() );
  addBuiltIn( VrmlNodeNormalInt::defineType() );
  addBuiltIn( VrmlNodeOrientationInt::defineType() );
  addBuiltIn( VrmlNodePixelTexture::defineType() );
  addBuiltIn( VrmlNodePlaneSensor::defineType() );
  addBuiltIn( VrmlNodePointLight::defineType() );
  addBuiltIn( VrmlNodePointSet::defineType() );
  addBuiltIn( VrmlNodePositionInt::defineType() );
  addBuiltIn( VrmlNodeProximitySensor::defineType() );
  addBuiltIn( VrmlNodeScalarInt::defineType() );
  addBuiltIn( VrmlNodeScript::defineType() );
  addBuiltIn( VrmlNodeShape::defineType() );
  addBuiltIn( VrmlNodeSound::defineType() );
  addBuiltIn( VrmlNodeSphere::defineType() );
  addBuiltIn( VrmlNodeSphereSensor::defineType() );
  addBuiltIn( VrmlNodeSpotLight::defineType() );
  addBuiltIn( VrmlNodeSwitch::defineType() );
  addBuiltIn( VrmlNodeText::defineType() );
  addBuiltIn( VrmlNodeTextureCoordinate::defineType() );
  addBuiltIn( VrmlNodeTextureTransform::defineType() );
  addBuiltIn( VrmlNodeTimeSensor::defineType() );
  addBuiltIn( VrmlNodeTouchSensor::defineType() );
  addBuiltIn( VrmlNodeTransform::defineType() );
  addBuiltIn( VrmlNodeViewpoint::defineType() );
  addBuiltIn( VrmlNodeVisibilitySensor::defineType() );
  addBuiltIn( VrmlNodeWorldInfo::defineType() );
}


// A safer version for reading PROTOs from files.

void
VrmlNamespace::addNodeType( VrmlNodeType *type )
{
  if ( findType( type->getName() ) != NULL)
    theSystem->warn("PROTO %s already defined\n",
		    type->getName() );
  else
    d_typeList.push_front( type->reference() );
}


const VrmlNodeType *
VrmlNamespace::findType( const char *name )
{
  // Look through the PROTO stack:
  const VrmlNodeType *nt = findPROTO(name);
  if (nt) return nt;

  // Look in parent scope for the type
  if (d_parent)
    return d_parent->findType( name );

  // Look through the built ins
  std::list<VrmlNodeType*>::iterator i;
  for (i = builtInList.begin(); i != builtInList.end(); ++i)
    {
      nt = *i;
      if (nt != NULL && strcmp(nt->getName(),name) == 0)
	return nt;
    }

  return NULL;
}

const VrmlNodeType *    // LarryD
VrmlNamespace::findPROTO(const char *name)
{
  // Look through the PROTO list ONLY:
  std::list<VrmlNodeType*>::iterator i;
  for (i = d_typeList.begin(); i != d_typeList.end(); ++i)
    {
      const VrmlNodeType *nt = *i;
      if (nt != NULL && strcmp(nt->getName(),name) == 0)
	return nt;
    }
  return NULL;
}


const VrmlNodeType *
VrmlNamespace::firstType()
{
  // Top of the PROTO stack (should make sure it has an implementation...)
  if (d_typeList.size() > 0)
    return d_typeList.front()->reference();
  return NULL;
}

void
VrmlNamespace::addNodeName(const VrmlNodePtr & namedNode)
{
  // We could remove any existing node with this name, but
  // since we are just pushing this one onto the front of
  // the list, the other name won't be found. If we do
  // something smart with this list (like sorting), this
  // will need to change.
  d_nameList.push_front(namedNode);
}

void VrmlNamespace::removeNodeName(const VrmlNode & namedNode) {
    for (std::list<VrmlNodePtr>::iterator i(this->d_nameList.begin());
            i != this->d_nameList.end(); ++i) {
        if (i->get() == &namedNode) {
            this->d_nameList.erase(i);
            return;
        }
    }
}


const VrmlNodePtr VrmlNamespace::findNode(const char * name) const {
    for (std::list<VrmlNodePtr>::const_iterator n = d_nameList.begin();
            n != d_nameList.end(); ++n) {
        if (strcmp((*n)->name(), name) == 0) {
            return *n;
        }
    }
    
    return VrmlNodePtr(0);
}

namespace {
    
    class NodeCloneVisitor : public VrmlNodeVisitor {
    public:
        NodeCloneVisitor(VrmlNamespace & ns): ns(ns) {}
        
        virtual ~NodeCloneVisitor() {}
        
        const VrmlMFNode clone(const VrmlMFNode & mfnode) {
            VrmlMFNode clonedNodes(this->visitChildren(mfnode));
            assert(this->rootNodeStack.size() == 0);
            
            for (size_t i = 0; i < mfnode.getLength(); ++i) {
                mfnode.getElement(i)->resetVisitedFlag();
            }
            
            return clonedNodes;
        }
        
        virtual void visit(VrmlNodeAnchor & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeAnchor(node));
            this->rootNodeStack.push(clonedNode);
            clonedNode->toAnchor()
                    ->setChildren(this->visitChildren(node.getChildren()));
        }
        
        virtual void visit(VrmlNodeAppearance & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeAppearance(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getMaterial().get()) {
                this->visitNode(*node.getMaterial().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toMaterial());
                clonedNode->toAppearance()
                        ->setMaterial(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getTexture().get()) {
                this->visitNode(*node.getTexture().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toTexture());
                clonedNode->toAppearance()
                        ->setTexture(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getTextureTransform().get()) {
                this->visitNode(*node.getTextureTransform().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toTextureTransform());
                clonedNode->toAppearance()
                        ->setTextureTransform(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(VrmlNodeAudioClip & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeAudioClip(node)));
        }
        
        virtual void visit(VrmlNodeBackground & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeBackground(node)));
        }
        
        virtual void visit(VrmlNodeBillboard & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeBillboard(node));
            this->rootNodeStack.push(clonedNode);
            clonedNode->toBillboard()
                    ->setChildren(this->visitChildren(node.getChildren()));
        }
        
        virtual void visit(VrmlNodeBox & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeBox(node)));
        }
        
        virtual void visit(VrmlNodeCollision & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeCollision(node));
            this->rootNodeStack.push(clonedNode);
            clonedNode->toCollision()
                    ->setChildren(this->visitChildren(node.getChildren()));
            if (node.getProxy().get()) {
                this->visitNode(*node.getProxy().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toChild());
                clonedNode->toCollision()
                        ->setProxy(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(VrmlNodeColor & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeColor(node)));
        }
        
        virtual void visit(VrmlNodeColorInt & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeColorInt(node)));
        }
        
        virtual void visit(VrmlNodeCone & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeCone(node)));
        }
        
        virtual void visit(VrmlNodeCoordinate & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeCoordinate(node)));
        }
        
        virtual void visit(VrmlNodeCoordinateInt & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeCoordinateInt(node)));
        }
        
        virtual void visit(VrmlNodeCylinder & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeCylinder(node)));
        }
        
        virtual void visit(VrmlNodeCylinderSensor & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeCylinderSensor(node)));
        }
        
        virtual void visit(VrmlNodeDirLight & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeDirLight(node)));
        }
        
        virtual void visit(VrmlNodeElevationGrid & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeElevationGrid(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getColor().get()) {
                this->visitNode(*node.getColor().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toColor());
                clonedNode->toElevationGrid()
                        ->setColor(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getNormal().get()) {
                this->visitNode(*node.getNormal().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toNormal());
                clonedNode->toElevationGrid()
                        ->setNormal(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getTexCoord().get()) {
                this->visitNode(*node.getTexCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toTextureCoordinate());
                clonedNode->toElevationGrid()
                        ->setTexCoord(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(VrmlNodeExtrusion & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeExtrusion(node)));
        }
        
        virtual void visit(VrmlNodeFog & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeFog(node)));
        }
        
        virtual void visit(VrmlNodeFontStyle & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeFontStyle(node)));
        }
        
        virtual void visit(VrmlNodeGroup & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeGroup(node));
            this->rootNodeStack.push(clonedNode);
            static_cast<VrmlNodeGroup *>(clonedNode.get())
                    ->setChildren(this->visitChildren(node.getChildren()));
        }
        
        virtual void visit(VrmlNodeIFaceSet & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeIFaceSet(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getColor().get()) {
                this->visitNode(*node.getColor().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toColor());
                static_cast<VrmlNodeIFaceSet *>(clonedNode.get())
                        ->setColor(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getCoord().get()) {
                this->visitNode(*node.getCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toCoordinate());
                static_cast<VrmlNodeIFaceSet *>(clonedNode.get())
                        ->setCoord(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getNormal().get()) {
                this->visitNode(*node.getNormal().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toNormal());
                static_cast<VrmlNodeIFaceSet *>(clonedNode.get())
                        ->setNormal(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getTexCoord().get()) {
                this->visitNode(*node.getTexCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toTextureCoordinate());
                static_cast<VrmlNodeIFaceSet *>(clonedNode.get())
                        ->setTexCoord(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(VrmlNodeILineSet & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeILineSet(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getColor().get()) {
                this->visitNode(*node.getColor().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toColor());
                static_cast<VrmlNodeILineSet *>(clonedNode.get())
                        ->setColor(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getCoord().get()) {
                this->visitNode(*node.getCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toCoordinate());
                static_cast<VrmlNodeILineSet *>(clonedNode.get())
                        ->setCoord(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(VrmlNodeImageTexture & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeImageTexture(node)));
        }
        
        virtual void visit(VrmlNodeInline & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeInline(node)));
        }
        
        virtual void visit(VrmlNodeLOD & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeLOD(node));
            this->rootNodeStack.push(clonedNode);
            static_cast<VrmlNodeLOD *>(clonedNode.get())
                    ->setLevel(this->visitChildren(node.getLevel()));
        }
        
        virtual void visit(VrmlNodeMaterial & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeMaterial(node)));
        }
        
        virtual void visit(VrmlNodeMovieTexture & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeMovieTexture(node)));
        }
        
        virtual void visit(VrmlNodeNavigationInfo & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeNavigationInfo(node)));
        }
        
        virtual void visit(VrmlNodeNormal & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeNormal(node)));
        }
        
        virtual void visit(VrmlNodeNormalInt & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeNormalInt(node)));
        }
        
        virtual void visit(VrmlNodeOrientationInt & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeOrientationInt(node)));
        }
        
        virtual void visit(VrmlNodePixelTexture & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodePixelTexture(node)));
        }
        
        virtual void visit(VrmlNodePlaneSensor & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodePlaneSensor(node)));
        }
        
        virtual void visit(VrmlNodePointLight & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodePointLight(node)));
        }
        
        virtual void visit(VrmlNodePointSet & node) {
            const VrmlNodePtr clonedNode(new VrmlNodePointSet(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getColor().get()) {
                this->visitNode(*node.getColor().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toColor());
                static_cast<VrmlNodePointSet *>(clonedNode.get())
                        ->setColor(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getCoord().get()) {
                this->visitNode(*node.getCoord().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toCoordinate());
                static_cast<VrmlNodePointSet *>(clonedNode.get())
                        ->setCoord(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(VrmlNodePositionInt & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodePositionInt(node)));
        }
        
        virtual void visit(VrmlNodeProto & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeProto(node));
            clonedNode->addToScene(node.scene(), clonedNode->nodeType().url());
            this->rootNodeStack.push(clonedNode);
        }
        
        virtual void visit(VrmlNodeProximitySensor & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeProximitySensor(node)));
        }
        
        virtual void visit(VrmlNodeScalarInt & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeScalarInt(node)));
        }
        
        virtual void visit(VrmlNodeScript & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeScript(node));
            this->rootNodeStack.push(clonedNode);
            for (VrmlNodeScript::FieldList::const_iterator 
                    i = node.fields().begin(),
                    j = static_cast<VrmlNodeScript *>(clonedNode.get())
                            ->fields().begin();
                    i != node.fields().end(); ++i, ++j) {
                assert((*i)->value);
                if ((*i)->type == VrmlField::SFNODE) {
                    assert(dynamic_cast<VrmlSFNode *>((*i)->value));
                    if (static_cast<VrmlSFNode *>((*i)->value)->get()) {
                        visitNode(*static_cast<VrmlSFNode *>((*i)->value)->get());
                        static_cast<VrmlSFNode *>((*j)->value)
                                ->set(this->rootNodeStack.top());
                        this->rootNodeStack.pop();
                    }
                } else if ((*i)->type == VrmlField::MFNODE) {
                    assert(dynamic_cast<VrmlMFNode *>((*i)->value));
                    static_cast<VrmlMFNode &>(*(*j)->value) =
                        visitChildren(static_cast<VrmlMFNode &>(*(*i)->value));
                }
            }
        }
        
        virtual void visit(VrmlNodeShape & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeShape(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getAppearance().get()) {
                this->visitNode(*node.getAppearance().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toAppearance());
                static_cast<VrmlNodeShape *>(clonedNode.get())
                        ->setAppearance(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
            
            if (node.getGeometry().get()) {
                this->visitNode(*node.getGeometry().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toGeometry());
                static_cast<VrmlNodeShape *>(clonedNode.get())
                        ->setGeometry(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        /**
         * @todo The assertion here will fail if a MovieTexture node is used
         *       as a sound source.
         */
        virtual void visit(VrmlNodeSound & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeSound(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getSource().get()) {
                this->visitNode(*node.getSource().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toAudioClip());
                static_cast<VrmlNodeSound *>(clonedNode.get())
                        ->setSource(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(VrmlNodeSphere & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeSphere(node)));
        }
        
        virtual void visit(VrmlNodeSphereSensor & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeSphereSensor(node)));
        }
        
        virtual void visit(VrmlNodeSpotLight & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeSpotLight(node)));
        }
        
        virtual void visit(VrmlNodeSwitch & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeSwitch(node));
            this->rootNodeStack.push(clonedNode);
            static_cast<VrmlNodeSwitch *>(clonedNode.get())
                    ->setChoice(this->visitChildren(node.getChoice()));
        }
        
        virtual void visit(VrmlNodeText & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeText(node));
            this->rootNodeStack.push(clonedNode);
            
            if (node.getFontStyle().get()) {
                this->visitNode(*node.getFontStyle().get());
                assert(this->rootNodeStack.top());
                assert(this->rootNodeStack.top()->toFontStyle());
                static_cast<VrmlNodeText *>(clonedNode.get())
                        ->setFontStyle(VrmlSFNode(this->rootNodeStack.top()));
                this->rootNodeStack.pop();
            }
        }
        
        virtual void visit(VrmlNodeTextureCoordinate & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeTextureCoordinate(node)));
        }
        
        virtual void visit(VrmlNodeTextureTransform & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeTextureTransform(node)));
        }
        
        virtual void visit(VrmlNodeTimeSensor & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeTimeSensor(node)));
        }
        
        virtual void visit(VrmlNodeTouchSensor & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeTouchSensor(node)));
        }
        
        virtual void visit(VrmlNodeTransform & node) {
            const VrmlNodePtr clonedNode(new VrmlNodeTransform(node));
            this->rootNodeStack.push(clonedNode);
            static_cast<VrmlNodeTransform *>(clonedNode.get())
                    ->setChildren(this->visitChildren(node.getChildren()));
        }
        
        virtual void visit(VrmlNodeViewpoint & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeViewpoint(node)));
        }
        
        virtual void visit(VrmlNodeVisibilitySensor & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeVisibilitySensor(node)));
        }
        
        virtual void visit(VrmlNodeWorldInfo & node) {
            this->rootNodeStack.push(VrmlNodePtr(new VrmlNodeWorldInfo(node)));
        }
        
    private:
        //
        // Not copyable.
        //
        NodeCloneVisitor(const NodeCloneVisitor &);
        NodeCloneVisitor & operator=(const NodeCloneVisitor &);
        
        const VrmlMFNode visitChildren(const VrmlMFNode & children) {
            VrmlMFNode clonedChildren(children.getLength());
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
        
        void visitNode(VrmlNode & node) {
            if (node.accept(*this)) {
                assert(this->rootNodeStack.size() > 0);
                assert(typeid(node) == typeid(*this->rootNodeStack.top()));
                const char * const name = this->rootNodeStack.top()->name();
                if (*this->rootNodeStack.top()->name()) {
                    this->ns.addNodeName(this->rootNodeStack.top());
                }
            } else {
                assert(this->ns.findNode(node.name()));
                this->rootNodeStack.push(this->ns.findNode(node.name()));
            }
        }
        
        VrmlNamespace & ns;
        std::stack<VrmlNodePtr> rootNodeStack;
    };
    
    class NodeRouteCopyVisitor : public VrmlNodeVisitor {
    public:
        NodeRouteCopyVisitor(VrmlNamespace & ns): ns(ns) {}
        
        virtual ~NodeRouteCopyVisitor() {}
        
        void copyRoutes(const VrmlMFNode & mfnode) {
            this->visitChildren(mfnode);
            
            for (size_t i = 0; i < mfnode.getLength(); ++i) {
                mfnode.getElement(i)->resetVisitedFlag();
            }
        }
        
        virtual void visit(VrmlNodeAnchor & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(VrmlNodeAppearance & node) {
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
        
        virtual void visit(VrmlNodeAudioClip & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeBackground & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeBillboard & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(VrmlNodeBox & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeCollision & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(VrmlNodeColor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeColorInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeCone & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeCoordinate & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeCoordinateInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeCylinder & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeCylinderSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeDirLight & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeElevationGrid & node) {
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
        
        virtual void visit(VrmlNodeExtrusion & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeFog & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeFontStyle & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeGroup & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(VrmlNodeIFaceSet & node) {
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
        
        virtual void visit(VrmlNodeILineSet & node) {
            this->copyRoutesFromNode(node);
            if (node.getColor().get()) {
                node.getColor().get()->accept(*this);
            }
            if (node.getCoord().get()) {
                node.getCoord().get()->accept(*this);
            }
        }
        
        virtual void visit(VrmlNodeImageTexture & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeInline & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeLOD & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getLevel());
        }
        
        virtual void visit(VrmlNodeMaterial & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeMovieTexture & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeNavigationInfo & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeNormal & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeNormalInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeOrientationInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodePixelTexture & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodePlaneSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodePointLight & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodePointSet & node) {
            this->copyRoutesFromNode(node);
            if (node.getColor().get()) {
                node.getColor().get()->accept(*this);
            }
            if (node.getCoord().get()) {
                node.getCoord().get()->accept(*this);
            }
       }
        
        virtual void visit(VrmlNodePositionInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeProto & node) {
            this->copyRoutesFromNode(node);
            //
            // What about the fields of a PROTO'd node?
            //
        }
        
        virtual void visit(VrmlNodeProximitySensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeScalarInt & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeScript & node) {
            this->copyRoutesFromNode(node);
            for (VrmlNodeScript::FieldList::const_iterator
                    i = node.fields().begin(); i != node.fields().end(); ++i) {
                assert((*i)->value);
                if ((*i)->type == VrmlField::SFNODE) {
                    assert(dynamic_cast<VrmlSFNode *>((*i)->value));
                    if (static_cast<VrmlSFNode *>((*i)->value)->get()) {
                        static_cast<VrmlSFNode *>((*i)->value)->get()
                                ->accept(*this);
                    }
                } else if ((*i)->type == VrmlField::MFNODE) {
                    assert(dynamic_cast<VrmlMFNode *>((*i)->value));
                    this->visitChildren(static_cast<VrmlMFNode &>(*(*i)->value));
                }
            }
        }
        
        virtual void visit(VrmlNodeShape & node) {
            this->copyRoutesFromNode(node);
            if (node.getAppearance().get()) {
                node.getAppearance().get()->accept(*this);
            }
            if (node.getGeometry().get()) {
                node.getGeometry().get()->accept(*this);
            }
        }
        
        virtual void visit(VrmlNodeSound & node) {
            this->copyRoutesFromNode(node);
            if (node.getSource().get()) {
                node.getSource().get()->accept(*this);
            }
        }
        
        virtual void visit(VrmlNodeSphere & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeSphereSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeSpotLight & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeSwitch & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChoice());
        }
        
        virtual void visit(VrmlNodeText & node) {
            this->copyRoutesFromNode(node);
            if (node.getFontStyle().get()) {
                node.getFontStyle().get()->accept(*this);
            }
        }
        
        virtual void visit(VrmlNodeTextureCoordinate & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeTextureTransform & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeTimeSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeTouchSensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeTransform & node) {
            this->copyRoutesFromNode(node);
            this->visitChildren(node.getChildren());
        }
        
        virtual void visit(VrmlNodeViewpoint & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeVisibilitySensor & node) {
            this->copyRoutesFromNode(node);
        }
        
        virtual void visit(VrmlNodeWorldInfo & node) {
            this->copyRoutesFromNode(node);
        }
        
    private:
        //
        // Not copyable.
        //
        NodeRouteCopyVisitor(const NodeRouteCopyVisitor &);
        NodeRouteCopyVisitor & operator=(const NodeRouteCopyVisitor &);
        
        void copyRoutesFromNode(VrmlNode & node) {
            const char * fromName = 0;
            if (*(fromName = node.name())) {
                const VrmlNodePtr fromNode = this->ns.findNode(fromName);
                assert(fromNode);
                for (Route * route = node.getRoutes(); route;
                        route = route->next()) {
                    const char * toName = route->toNode()->name();
                    assert(this->ns.findNode(toName));
                    fromNode->addRoute(route->fromEventOut(),
                            this->ns.findNode(toName), route->toEventIn());
                }
            }
        }
        
        void visitChildren(const VrmlMFNode & children) {
            for (size_t i = 0; i < children.getLength(); ++i) {
                if (children.getElement(i)) {
                    children.getElement(i)->accept(*this);
                }
            }
        }
        
        VrmlNamespace & ns;
    };
}

const VrmlMFNode VrmlNamespace::cloneNodes(const VrmlMFNode & mfnode) {
    
    VrmlMFNode clonedNodes(NodeCloneVisitor(*this).clone(mfnode));
    NodeRouteCopyVisitor(*this).copyRoutes(mfnode);
    
    return clonedNodes;
}
