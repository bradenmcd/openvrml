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


/**
 * @class VrmlNodeProto
 *
 * @brief A VrmlNodeProto object represents an instance of a PROTOd node.
 *
 * The definition of the PROTO is stored in a VrmlNodeType object;
 * the VrmlNodeProto object stores a local copy of the implementation
 * nodes.
 */
#include "VrmlNodeProto.h"
#include "VrmlNamespace.h"
#include "VrmlNodeVisitor.h"
#include "Route.h"
#include "VrmlBSphere.h"

#ifndef NDEBUG
#define VRML_NODE_PROTO_DEBUG
#endif

//
// Instances of PROTOs clone the implementation nodes stored
// in a VrmlNodeType object. The only tricky parts are to
// make sure ROUTEs are properly copied (the DEF name map is
// not available) and that fields are copied properly (the
// MF* guys currently share data & need to be made copy-on-
// write for this to be correct). Flags are set as each node
// is cloned so that USEd nodes are referenced rather than
// duplicated.
//
// ROUTEs: Build a temp namespace as each (named) implementation
// node is cloned, then traverse the implementation nodes again,
// reproducing the routes in the cloned nodes using the temp ns.
// I think that the addToScene() method is the right place to
// download EXTERNPROTO implementations. Should also check that
// the first node matches the expected type (need to store the
// expected type if first node is null when one of the type
// tests is run).
//
// Events between nodes in the PROTO implementation are handled
// by the ROUTE copying described above. For eventIns coming into
// the proto, when the implementation nodes are copied, a list
// of eventIns/exposedFields along with their IS mappings should
// be constructed.
// EventOuts from an implementation node to a node outside the
// PROTO can be directly replaced at copy time.
//

VrmlNodeType & VrmlNodeProto::nodeType() const
{
    return *d_nodeType;
}


VrmlNodeProto::VrmlNodeProto(VrmlNodeType *nodeDef, VrmlScene *scene) :
  VrmlNode(scene),
  d_nodeType(nodeDef->reference()),
  d_instantiated(false),
  d_scope(0),
  d_viewerObject(0)
{
  this->setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeProto::VrmlNodeProto(const VrmlNodeProto &n) :
  VrmlNode(0),
  d_nodeType(n.nodeType().reference()),
  d_instantiated(false),
  d_scope(0),
  d_viewerObject(0)
{
  // Copy fields.
  std::list<NameValueRec*>::const_iterator i;
  for (i = n.d_fields.begin(); i != n.d_fields.end(); i++)
    {
      VrmlField *value = (*i)->value;
      setField( (*i)->name, *value );
    }
  this->setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeProto::~VrmlNodeProto() {
    for (std::list<NameValueRec*>::iterator i(d_fields.begin());
            i != d_fields.end(); i++) {
        delete (*i)->value;
        delete *i;
    }

    for (EventDispatchList::iterator e(d_eventDispatch.begin());
            e != d_eventDispatch.end(); ++e) {
        for (VrmlNodeType::ISMap::iterator j((*e)->ismap.begin());
                j != (*e)->ismap.end(); ++j) {
            delete *j;
        }
        delete *e;
    }

    delete d_scope;
    d_nodeType->dereference();
}


bool VrmlNodeProto::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeProto::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        for (size_t i = 0; i < this->implNodes.getLength(); ++i) {
            if (this->implNodes.getElement(i)) {
                this->implNodes.getElement(i)->resetVisitedFlag();
            }
        }
    }
}

VrmlNodeProto* VrmlNodeProto::toProto() const
{ return (VrmlNodeProto*) this; }


// Instantiate a local copy of the implementation nodes.
// EXTERNPROTOs are actually loaded here. We don't want
// *references* (DEF/USE) to the nodes in the PROTO definition,
// we need to actually clone new nodes...

void VrmlNodeProto::instantiate()
{
  //theSystem->debug("%s::%s instantiate\n", d_nodeType->getName(),
  //name());

  if (this->implNodes.getLength() == 0)
    {
      d_scope = new VrmlNamespace();

      // Clone nodes
      this->implNodes =
              d_scope->cloneNodes(this->d_nodeType->getImplementationNodes());
      
      // Collect eventIns coming from outside the PROTO.
      // A list of eventIns along with their maps to local
      // nodes/eventIns is constructed for each instance.
      VrmlNodeType::FieldList &eventIns = d_nodeType->eventIns();
      VrmlNodeType::FieldList::iterator ev;
      const VrmlNodeType::ISMap * ismap;
      VrmlNodeType::ISMap::const_iterator j;

      for (ev = eventIns.begin(); ev != eventIns.end(); ++ev)
	{
	  EventDispatch *ed = new EventDispatch;
          ed->name = (*ev)->name;
	  ismap = &(*ev)->thisIS;
	  d_eventDispatch.push_front(ed);

	  for (j = ismap->begin(); j != ismap->end(); ++j)
	    {
	      VrmlNodeType::NodeFieldRec *nf = new VrmlNodeType::NodeFieldRec;
	      nf->node = d_scope->findNode((*j)->node->getName());
	      nf->fieldName = (*j)->fieldName;
	      ed->ismap.push_front(nf);
	    }
	}

      // Do the same for fields. Code duplication -- messy!
      VrmlNodeType::FieldList &fields = d_nodeType->fields();
      for (ev = fields.begin(); ev != fields.end(); ++ev )
        {
          EventDispatch *ed = new EventDispatch;
          ed->name = "set_" + (*ev)->name;
          ismap = &(*ev)->thisIS;
          d_eventDispatch.push_front(ed);
          for (j = ismap->begin(); j != ismap->end(); ++j)
            {
              VrmlNodeType::NodeFieldRec *nf = new VrmlNodeType::NodeFieldRec;
              nf->node = d_scope->findNode((*j)->node->getName());
              nf->fieldName = (*j)->fieldName;
              ed->ismap.push_front(nf);
            }
        }

      // Distribute eventOuts. Each eventOut ROUTE is added
      // directly to the local nodes that have IS'd the PROTO
      // eventOut.
      VrmlNodeType::FieldList &eventOuts = d_nodeType->eventOuts();
      for (Route *r = d_routes; r; r = r->getNext())
	for (ev = eventOuts.begin(); ev != eventOuts.end(); ++ev)
	  if ((*ev)->name == r->fromEventOut)
	    {
	      ismap = &(*ev)->thisIS;
	      for (j = ismap->begin(); j != ismap->end(); ++j)
		{
		  const VrmlNodePtr & n(d_scope->findNode((*j)->node->getName()));
		  if (n)
		    n->addRoute((*j)->fieldName, r->toNode, r->toEventIn);
		}
	    }

      // Set IS'd field values in the implementation nodes to
      // the values specified in the instantiation.

      std::list<NameValueRec*>::iterator ifld;
      for (ifld = d_fields.begin(); ifld != d_fields.end(); ++ifld)
	{
	  VrmlField *value = (*ifld)->value;
#ifdef VRML_NODE_PROTO_DEBUG
	  cerr << d_nodeType->getName() << "::" << this->getName()
	       << " setting IS field " << (*ifld)->name;
	  // Too much stuff...
	  //if (value) cerr << " to " << *value << endl;
	  //else cerr << " to null\n";
	  cerr << endl;
#endif
	  if (! value) continue;
	  if ((ismap = d_nodeType->getFieldISMap( (*ifld)->name )) != 0)
	    {
	      for (j = ismap->begin(); j != ismap->end(); ++j)
		{
		  const VrmlNodePtr & n(d_scope->findNode((*j)->node->getName()));
#ifdef VRML_NODE_PROTO_DEBUG
		  cerr << " on " << n->getName() << "::" << (*j)->fieldName << endl;
#endif
		  if (n) n->setField( (*j)->fieldName, *value );
		}
	    }
	}
    }

  d_instantiated = true;
  this->setBVolumeDirty(true); // lazy calc of bvolume
}


void VrmlNodeProto::addToScene(VrmlScene * scene, const std::string & relUrl) {
    //theSystem->debug("VrmlNodeProto::%s addToScene\n", name());
    this->d_scene = scene;

    // Make sure my nodes are here
    if (! d_instantiated) instantiate();
    //theSystem->debug("VrmlNodeProto::%s addToScene(%d nodes)\n",
    //	   name(), d_nodes ? d_nodes->size() : 0);

    // ... and add the implementation nodes to the scene.
    if (this->implNodes.getLength() > 0) {
        const std::string & rel = d_nodeType->getActualUrl();
        for (size_t i(0); i<this->implNodes.getLength(); ++i) {
            this->implNodes.getElement(i)
                  ->addToScene(scene, (rel.length() > 0) ? rel : relUrl);
        }
    }
}

void VrmlNodeProto::accumulateTransform( VrmlNode *n )
{
  // Make sure my nodes are here
  if (! d_instantiated)
    {
      theSystem->debug("VrmlNodeProto::%s accumTrans before instantiation\n",
		       this->getName().c_str());
      instantiate();
    }

  // ... and process the implementation nodes.
  if (this->implNodes.getLength() > 0)
    {
      int i, j = this->implNodes.getLength();
      for (i=0; i<j; ++i)
	this->implNodes.getElement(i)->accumulateTransform(n);
    }
}


// Print the node type, instance vars

ostream& VrmlNodeProto::printFields(ostream& os, int )
{
  os << "#VrmlNodeProto::printFields not implemented yet...\n";
  return os;
}


// Use the first node to check the type

const VrmlNodePtr VrmlNodeProto::firstNode() const {
    return (this->implNodes.getLength() > 0)
            ? this->implNodes.getElement(0)
            : d_nodeType->firstNode();
}


// These are passed along to the first implementation node of the proto.
// If the first node is not present (EXTERNPROTO prior to retrieving the
// the implementation), all tests fail.

VrmlNodeAnchor* VrmlNodeProto::toAnchor() const
{ return firstNode() ? firstNode()->toAnchor() : 0; }

VrmlNodeAppearance* VrmlNodeProto::toAppearance() const
{ return firstNode() ? firstNode()->toAppearance() : 0; }

VrmlNodeAudioClip* VrmlNodeProto::toAudioClip() const
{ return firstNode() ? firstNode()->toAudioClip() : 0; }

const VrmlNodeChild* VrmlNodeProto::toChild() const
{ return firstNode() ? firstNode()->toChild() : 0; }

VrmlNodeBackground* VrmlNodeProto::toBackground() const
{ return firstNode() ? firstNode()->toBackground() : 0; }

VrmlNodeColor* VrmlNodeProto::toColor() const
{ return firstNode() ? firstNode()->toColor() : 0; }

VrmlNodeCoordinate* VrmlNodeProto::toCoordinate() const
{ return firstNode() ? firstNode()->toCoordinate() : 0; }

VrmlNodeFog* VrmlNodeProto::toFog() const
{ return firstNode() ? firstNode()->toFog() : 0; }

VrmlNodeFontStyle* VrmlNodeProto::toFontStyle() const
{ return firstNode() ? firstNode()->toFontStyle() : 0; }

const VrmlNodeGeometry* VrmlNodeProto::toGeometry() const
{ return firstNode() ? firstNode()->toGeometry() : 0; }

VrmlNodeGroup* VrmlNodeProto::toGroup() const
{ return firstNode() ? firstNode()->toGroup() : 0; }

VrmlNodeInline* VrmlNodeProto::toInline() const
{ return firstNode() ? firstNode()->toInline() : 0; }

VrmlNodeLight* VrmlNodeProto::toLight() const
{ return firstNode() ? firstNode()->toLight() : 0; }

VrmlNodeMaterial* VrmlNodeProto::toMaterial() const
{ return firstNode() ? firstNode()->toMaterial() : 0; }

VrmlNodeMovieTexture* VrmlNodeProto::toMovieTexture() const
{ return firstNode() ? firstNode()->toMovieTexture() : 0; }

VrmlNodeNavigationInfo* VrmlNodeProto::toNavigationInfo() const
{ return firstNode() ? firstNode()->toNavigationInfo() : 0; }

VrmlNodeNormal* VrmlNodeProto::toNormal() const
{ return firstNode() ? firstNode()->toNormal() : 0; }

VrmlNodePlaneSensor* VrmlNodeProto::toPlaneSensor() const
{ return firstNode() ? firstNode()->toPlaneSensor() : 0; }

VrmlNodeSphereSensor* VrmlNodeProto::toSphereSensor() const
{ return firstNode() ? firstNode()->toSphereSensor() : 0; }

VrmlNodeCylinderSensor* VrmlNodeProto::toCylinderSensor() const
{ return firstNode() ? firstNode()->toCylinderSensor() : 0; }

VrmlNodePointLight* VrmlNodeProto::toPointLight() const
{ return firstNode() ? firstNode()->toPointLight() : 0; }

VrmlNodeScript* VrmlNodeProto::toScript() const
{ return firstNode() ? firstNode()->toScript() : 0; }

VrmlNodeSound* VrmlNodeProto::toSound() const
{ return firstNode() ? firstNode()->toSound() : 0; }

VrmlNodeSpotLight* VrmlNodeProto::toSpotLight() const
{ return firstNode() ? firstNode()->toSpotLight() : 0; }

VrmlNodeTexture* VrmlNodeProto::toTexture() const
{ return firstNode() ? firstNode()->toTexture() : 0; }

VrmlNodeTextureCoordinate* VrmlNodeProto::toTextureCoordinate() const
{ return firstNode() ? firstNode()->toTextureCoordinate() : 0; }

VrmlNodeTextureTransform* VrmlNodeProto::toTextureTransform() const
{ return firstNode() ? firstNode()->toTextureTransform() : 0; }

VrmlNodeTimeSensor* VrmlNodeProto::toTimeSensor() const
{ return firstNode() ? firstNode()->toTimeSensor() : 0; }

VrmlNodeTouchSensor* VrmlNodeProto::toTouchSensor() const
{ return firstNode() ? firstNode()->toTouchSensor() : 0; }

VrmlNodeViewpoint* VrmlNodeProto::toViewpoint() const
{ return firstNode() ? firstNode()->toViewpoint() : 0; }

// Larry
VrmlNodeBox* VrmlNodeProto::toBox() const       
{ return firstNode() ? firstNode()->toBox() : 0; }
 
VrmlNodeCone* VrmlNodeProto::toCone() const       
{ return firstNode() ? firstNode()->toCone() : 0; }

VrmlNodeCylinder* VrmlNodeProto::toCylinder() const       
{ return firstNode() ? firstNode()->toCylinder() : 0; }

VrmlNodeDirLight* VrmlNodeProto::toDirLight() const       
{ return firstNode() ? firstNode()->toDirLight() : 0; }

VrmlNodeElevationGrid* VrmlNodeProto::toElevationGrid() const       
{ return firstNode() ? firstNode()->toElevationGrid() : 0; }
VrmlNodeExtrusion* VrmlNodeProto::toExtrusion() const       
{ return firstNode() ? firstNode()->toExtrusion() : 0; }

VrmlNodeIFaceSet* VrmlNodeProto::toIFaceSet() const  
{ return firstNode() ? firstNode()->toIFaceSet() : 0; }
VrmlNodeShape* VrmlNodeProto::toShape() const  
{ return firstNode() ? firstNode()->toShape() : 0; }

VrmlNodeSphere* VrmlNodeProto::toSphere() const       
{ return firstNode() ? firstNode()->toSphere() : 0; }

VrmlNodeSwitch* VrmlNodeProto::toSwitch() const       
{ return firstNode() ? firstNode()->toSwitch() : 0; }

VrmlNodeTransform* VrmlNodeProto::toTransform() const       
{ return firstNode() ? firstNode()->toTransform() : 0; }

VrmlNodeImageTexture* VrmlNodeProto::toImageTexture() const
{ return firstNode() ? firstNode()->toImageTexture() : 0; }
VrmlNodePixelTexture* VrmlNodeProto::toPixelTexture() const
{ return firstNode() ? firstNode()->toPixelTexture() : 0; }
 
//

void VrmlNodeProto::render(Viewer *viewer, VrmlRenderContext rc)
{
  if (! d_instantiated)
    {
      theSystem->debug("VrmlNodeProto::%s render before instantiation\n",
		       this->getName().c_str());
      instantiate();
    }

  // If the PROTO has been modified it is necessary to rebuild the viewer object
  if ( d_viewerObject && isModified() )
    {
      viewer->removeObject(d_viewerObject);
      d_viewerObject = 0;
    }


  if (d_viewerObject)
    viewer->insertReference(d_viewerObject);

  else if (this->implNodes.getLength() > 0)
    {
      d_viewerObject = viewer->beginObject(this->getName().c_str());

      // render the nodes with the new values
      int n = this->implNodes.getLength();
      for (int j = 0; j<n; ++j)
	this->implNodes.getElement(j)->render(viewer, rc);

      viewer->endObject();
    }

  clearModified();
}


void VrmlNodeProto::eventIn(double timeStamp,
			    const std::string & eventName,
			    const VrmlField & fieldValue) {
//    cout << "eventIn " << this->nodeType().getName()
//         << "::" << this->getName() << "." << eventName
//         << " " << fieldValue << endl;

    if (!d_instantiated) {
        theSystem->debug("VrmlNodeProto::%s eventIn before instantiation\n",
                         this->getName().c_str());
        instantiate();
    }

    static const char * eventInPrefix = "set_";
    std::string basicEventName;
    if (std::equal(eventInPrefix, eventInPrefix + 4, eventName.begin())) {
        basicEventName = eventName.substr(4);
    } else {
        basicEventName = eventName;
    }

    for (EventDispatchList::iterator i(d_eventDispatch.begin());
            i != d_eventDispatch.end(); ++i) {
//        cout << "basicEventName = " << basicEventName
//             << ", eventName = " << eventName
//             << ", (*i)->name = " << (*i)->name << endl;
        if (basicEventName == (*i)->name || eventName == (*i)->name) {
            VrmlNodeType::ISMap * ismap = &(*i)->ismap;
            for (VrmlNodeType::ISMap::iterator j(ismap->begin());
                    j != ismap->end(); ++j) {
//                cout << "Relaying event." << endl;
                (*j)->node->eventIn(timeStamp, (*j)->fieldName, fieldValue);
            }
	    return;
        }
    }
    // Let the generic code handle errors.
    VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    this->setBVolumeDirty(true); // lazy calc of bvolume
}


VrmlNodeProto::NameValueRec * VrmlNodeProto::findField(
                                                const std::string & fieldName)
        const {
    for (std::list<NameValueRec *>::const_iterator i(d_fields.begin());
            i != d_fields.end(); ++i) {
        if (*i && (*i)->name == fieldName) {
            return *i;
        }
    }
    return 0;
}

// Set the value of one of the node fields (creates the field if
// it doesn't exist - is that necessary?...)

void VrmlNodeProto::setField(const std::string & fieldName,
			     const VrmlField & fieldValue)
{
  NameValueRec *nv = findField(fieldName);

  if (! nv)
    {
      nv = new NameValueRec;
      nv->name = fieldName;
      d_fields.push_front(nv);
    }
  else
    {
      delete nv->value;
    }

  nv->value = fieldValue.clone();
  this->setBVolumeDirty(true); // lazy calc of bvolume
}

const VrmlField *VrmlNodeProto::getField(const std::string & fieldName) const {
    NameValueRec * nv = findField(fieldName);
    if (nv) {
        return nv->value;
    }
    return d_nodeType->fieldDefault(fieldName);
}

VrmlNodeLOD* VrmlNodeProto::toLOD() const
{ return firstNode() ? firstNode()->toLOD() : 0; }

VrmlNodeOrientationInt* VrmlNodeProto::toOrientationInt() const
{ return firstNode() ? firstNode()->toOrientationInt() : 0; }

VrmlNodePositionInt* VrmlNodeProto::toPositionInt() const    
{ return firstNode() ? firstNode()->toPositionInt() : 0; }

VrmlNodeScalarInt* VrmlNodeProto::toScalarInt() const    
{ return firstNode() ? firstNode()->toScalarInt() : 0; }

const VrmlMFNode & VrmlNodeProto::getImplNodes() const {
    return this->implNodes;
}

const VrmlBVolume*
VrmlNodeProto::getBVolume() const
{
  //cout << "VrmlNodeProto::getBVolume() {" << endl;
  if (!d_instantiated) {
    cout << "VrmlNodeProto::getBVolume():WARNING:not instantiated" << endl;
    return (VrmlBVolume*)0; // shouldn't happen
  }
  const VrmlNodePtr base(firstNode());
  if (!base) {
    cout << "VrmlNodeProto::getBVolume():WARNING:!base" << endl;
    return (VrmlBVolume*)0; // shouldn't happen
  }
  const VrmlBVolume* bv = base->getBVolume();
  //cout << "}";
  //bv->dump(cout) << endl;
  return bv;
}

/**
 * @brief determine whether or not implementation is modified
 */
bool VrmlNodeProto::isModified() const {
  if ( d_modified ) return 1;
  int n = implNodes.getLength();
  for ( int i=0; i<n; ++i )
    if ( implNodes.getElement(i)->isModified() ) return 1;
  return 0;
}

void
VrmlNodeProto::updateModified(VrmlNodePath& path, int flags)
{
  //cout << "VrmlNodeProto::updateModified()" << endl;
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  const VrmlNodePtr base(firstNode());
  if (base) base->updateModified(path, flags);    
  path.pop_front();
}

