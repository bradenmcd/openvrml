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

# include <algorithm>
# include "proto.h"
# include "VrmlNamespace.h"
# include "Route.h"
# include "bvolume.h"

/**
 * @class OpenVRML::ProtoNode
 *
 * @brief A ProtoNode object represents an instance of a PROTOd node.
 *
 * The definition of the PROTO is stored in a NodeType object;
 * the ProtoNode object stores a local copy of the implementation
 * nodes.
 *
 * Instances of PROTOs clone the implementation nodes stored
 * in a NodeType object. The only tricky parts are to
 * make sure ROUTEs are properly copied (the DEF name map is
 * not available) and that fields are copied properly (the
 * MF* guys currently share data & need to be made copy-on-
 * write for this to be correct). Flags are set as each node
 * is cloned so that USEd nodes are referenced rather than
 * duplicated.
 *
 * ROUTEs: Build a temp namespace as each (named) implementation
 * node is cloned, then traverse the implementation nodes again,
 * reproducing the routes in the cloned nodes using the temp ns.
 * I think that the addToScene() method is the right place to
 * download EXTERNPROTO implementations. Should also check that
 * the first node matches the expected type (need to store the
 * expected type if first node is null when one of the type
 * tests is run).
 *
 * Events between nodes in the PROTO implementation are handled
 * by the ROUTE copying described above. For eventIns coming into
 * the proto, when the implementation nodes are copied, a list
 * of eventIns/exposedFields along with their IS mappings should
 * be constructed.
 * EventOuts from an implementation node to a node outside the
 * PROTO can be directly replaced at copy time.
 */

// Field name/value pairs specified in PROTO instantiation
struct OpenVRML::ProtoNode::NameValueRec {
    std::string name;
    FieldValue * value;
};

struct OpenVRML::ProtoNode::EventDispatch {
    std::string name;
    NodeType::ISMap ismap;
};

OpenVRML::ProtoNode::ProtoNode(const NodeType & nodeDef, VrmlScene *scene) :
  Node(nodeDef, scene),
  d_instantiated(false),
  d_scope(0),
  d_viewerObject(0)
{
  this->setBVolumeDirty(true); // lazy calc of bvolume
}

OpenVRML::ProtoNode::ProtoNode(const ProtoNode &n) :
  Node(n),
  d_instantiated(false),
  d_scope(0),
  d_viewerObject(0)
{
  // Copy fields.
  std::list<NameValueRec*>::const_iterator i;
  for (i = n.d_fields.begin(); i != n.d_fields.end(); i++)
    {
      FieldValue *value = (*i)->value;
      setField( (*i)->name, *value );
    }
  this->setBVolumeDirty(true); // lazy calc of bvolume
}

OpenVRML::ProtoNode::~ProtoNode() {
    for (std::list<NameValueRec*>::iterator i(d_fields.begin());
            i != d_fields.end(); i++) {
        delete (*i)->value;
        delete *i;
    }

    typedef std::list<EventDispatch *> EventDispatchList;
    for (EventDispatchList::iterator e(d_eventDispatch.begin());
            e != d_eventDispatch.end(); ++e) {
        for (NodeType::ISMap::iterator j((*e)->ismap.begin());
                j != (*e)->ismap.end(); ++j) {
            delete *j;
        }
        delete *e;
    }

    delete d_scope;
}

bool OpenVRML::ProtoNode::accept(NodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void OpenVRML::ProtoNode::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        for (size_t i = 0; i < this->implNodes.getLength(); ++i) {
            if (this->implNodes.getElement(i)) {
                this->implNodes.getElement(i)->resetVisitedFlag();
            }
        }
    }
}

namespace {
    struct AccumulateNodes_ :
            std::unary_function<OpenVRML::ProtoNode::NameValueRec *, void> {
        explicit AccumulateNodes_(OpenVRML::MFNode & children):
                children(children) {}
        
        result_type operator()(argument_type protoFieldRec) {
            using OpenVRML::FieldValue;
            using OpenVRML::SFNode;
            using OpenVRML::MFNode;
            assert(protoFieldRec);
            if (protoFieldRec->value->type() == FieldValue::sfnode) {
                assert(dynamic_cast<SFNode *>(protoFieldRec->value));
                this->children.setLength(this->children.getLength() + 1);
                this->children.setElement(this->children.getLength() - 1,
                        static_cast<SFNode *>(protoFieldRec->value)->get());
            } else if (protoFieldRec->value->type() == FieldValue::mfnode) {
                assert(dynamic_cast<MFNode *>(protoFieldRec->value));
                MFNode & nodes =
                        *static_cast<MFNode *>(protoFieldRec->value);
                this->children.setLength(this->children.getLength()
                                         + nodes.getLength());
                for (size_t i = 0; i < nodes.getLength(); ++i) {
                    this->children.setElement(this->children.getLength() + i,
                                              nodes.getElement(i));
                }
            }
        };
    
    private:
        OpenVRML::MFNode & children;
    };
}

const OpenVRML::MFNode OpenVRML::ProtoNode::getChildren() const {
    MFNode children;
    std::for_each(this->d_fields.begin(), this->d_fields.end(),
                  AccumulateNodes_(children));
    return children;
}

/**
 * Instantiate a local copy of the implementation nodes. EXTERNPROTOs are
 * actually loaded here. We don't want <em>references</em> (DEF/USE) to the
 * nodes in the PROTO definition, we need to actually clone new nodes.
 */
void OpenVRML::ProtoNode::instantiate()
{
  if (this->implNodes.getLength() == 0)
    {
      d_scope = new VrmlNamespace();

      // Clone nodes
      this->implNodes =
              d_scope->cloneNodes(this->type.getImplementationNodes());
      
      // Collect eventIns coming from outside the PROTO.
      // A list of eventIns along with their maps to local
      // nodes/eventIns is constructed for each instance.
      const NodeType::FieldList & eventIns = this->type.eventIns();
      NodeType::FieldList::const_iterator ev;
      const NodeType::ISMap * ismap;
      NodeType::ISMap::const_iterator j;

      for (ev = eventIns.begin(); ev != eventIns.end(); ++ev)
	{
	  EventDispatch *ed = new EventDispatch;
          ed->name = (*ev)->name;
	  ismap = &(*ev)->thisIS;
	  d_eventDispatch.push_front(ed);

	  for (j = ismap->begin(); j != ismap->end(); ++j)
	    {
	      NodeType::NodeFieldRec *nf = new NodeType::NodeFieldRec;
	      nf->node.reset(this->d_scope->findNode((*j)->node->getId()));
	      nf->fieldName = (*j)->fieldName;
	      ed->ismap.push_front(nf);
	    }
	}

      // Do the same for fields. Code duplication -- messy!
      const NodeType::FieldList & fields = this->type.fields();
      for (ev = fields.begin(); ev != fields.end(); ++ev )
        {
          EventDispatch *ed = new EventDispatch;
          ed->name = "set_" + (*ev)->name;
          ismap = &(*ev)->thisIS;
          d_eventDispatch.push_front(ed);
          for (j = ismap->begin(); j != ismap->end(); ++j)
            {
              NodeType::NodeFieldRec *nf = new NodeType::NodeFieldRec;
              nf->node.reset(this->d_scope->findNode((*j)->node->getId()));
              nf->fieldName = (*j)->fieldName;
              ed->ismap.push_front(nf);
            }
        }

      // Distribute eventOuts. Each eventOut ROUTE is added
      // directly to the local nodes that have IS'd the PROTO
      // eventOut.
      const NodeType::FieldList & eventOuts = this->type.eventOuts();

      RouteList::iterator i;

      for (i = d_routes.begin(); i != d_routes.end(); ++i)
      {
	for (ev = eventOuts.begin(); ev != eventOuts.end(); ++ev)
	{
	  if ((*ev)->name == (*i)->fromEventOut)
	  {
	    ismap = &(*ev)->thisIS;
	    for (j = ismap->begin(); j != ismap->end(); ++j)
	    {
	      Node * const n = this->d_scope->findNode((*j)->node->getId());
	      if (n)
		n->addRoute((*j)->fieldName, (*i)->toNode, (*i)->toEventIn);
	    }
	  }
	}
      }

      // Set IS'd field values in the implementation nodes to
      // the values specified in the instantiation.

      std::list<NameValueRec*>::iterator ifld;
      for (ifld = d_fields.begin(); ifld != d_fields.end(); ++ifld)
	{
	  FieldValue *value = (*ifld)->value;
	  if (! value) continue;
          ismap = this->type.getFieldISMap((*ifld)->name);
	  if (ismap) {
	      for (j = ismap->begin(); j != ismap->end(); ++j)
		{
		  Node * const n = this->d_scope->findNode((*j)->node->getId());
		  if (n) n->setField( (*j)->fieldName, *value );
		}
	    }
	}
    }

  d_instantiated = true;
  this->setBVolumeDirty(true); // lazy calc of bvolume
}


void OpenVRML::ProtoNode::addToScene(VrmlScene * scene,
                                     const std::string & relUrl) {
    //theSystem->debug("ProtoNode::%s addToScene\n", name());
    this->d_scene = scene;

    // Make sure my nodes are here
    if (! d_instantiated) instantiate();
    //theSystem->debug("ProtoNode::%s addToScene(%d nodes)\n",
    //	   name(), d_nodes ? d_nodes->size() : 0);

    // ... and add the implementation nodes to the scene.
    if (this->implNodes.getLength() > 0) {
        const std::string & rel = this->type.getActualUrl();
        for (size_t i(0); i<this->implNodes.getLength(); ++i) {
            this->implNodes.getElement(i)
                  ->addToScene(scene, (rel.length() > 0) ? rel : relUrl);
        }
    }
}

void OpenVRML::ProtoNode::accumulateTransform( Node *n )
{
  // Make sure my nodes are here
  if (! d_instantiated)
    {
      theSystem->debug("ProtoNode::%s accumTrans before instantiation\n",
		       this->getId().c_str());
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

ostream & OpenVRML::ProtoNode::printFields(ostream& os, int )
{
  os << "#ProtoNode::printFields not implemented yet...\n";
  return os;
}


// Use the first node to check the type

const OpenVRML::NodePtr OpenVRML::ProtoNode::firstNode() const {
    return (this->implNodes.getLength() > 0)
            ? this->implNodes.getElement(0)
            : this->type.firstNode();
}


// These are passed along to the first implementation node of the proto.
// If the first node is not present (EXTERNPROTO prior to retrieving the
// the implementation), all tests fail.

OpenVRML::NodeAnchor * OpenVRML::ProtoNode::toAnchor() const
{ return firstNode() ? firstNode()->toAnchor() : 0; }

OpenVRML::NodeAppearance * OpenVRML::ProtoNode::toAppearance() const
{ return firstNode() ? firstNode()->toAppearance() : 0; }

OpenVRML::NodeAudioClip * OpenVRML::ProtoNode::toAudioClip() const
{ return firstNode() ? firstNode()->toAudioClip() : 0; }

OpenVRML::NodeChild * OpenVRML::ProtoNode::toChild() const
{ return firstNode() ? firstNode()->toChild() : 0; }

OpenVRML::NodeBackground * OpenVRML::ProtoNode::toBackground() const
{ return firstNode() ? firstNode()->toBackground() : 0; }

OpenVRML::NodeBillboard * OpenVRML::ProtoNode::toBillboard() const
{ return firstNode() ? firstNode()->toBillboard() : 0; }

OpenVRML::NodeBox * OpenVRML::ProtoNode::toBox() const       
{ return firstNode() ? firstNode()->toBox() : 0; }
 
OpenVRML::NodeCollision * OpenVRML::ProtoNode::toCollision() const
{ return firstNode() ? firstNode()->toCollision() : 0; }

OpenVRML::NodeColor * OpenVRML::ProtoNode::toColor() const
{ return firstNode() ? firstNode()->toColor() : 0; }

OpenVRML::NodeCone * OpenVRML::ProtoNode::toCone() const       
{ return firstNode() ? firstNode()->toCone() : 0; }

OpenVRML::NodeCoordinate * OpenVRML::ProtoNode::toCoordinate() const
{ return firstNode() ? firstNode()->toCoordinate() : 0; }

OpenVRML::NodeCylinder * OpenVRML::ProtoNode::toCylinder() const       
{ return firstNode() ? firstNode()->toCylinder() : 0; }

OpenVRML::NodeCylinderSensor * OpenVRML::ProtoNode::toCylinderSensor() const
{ return firstNode() ? firstNode()->toCylinderSensor() : 0; }

OpenVRML::NodeDirLight * OpenVRML::ProtoNode::toDirLight() const       
{ return firstNode() ? firstNode()->toDirLight() : 0; }

OpenVRML::NodeElevationGrid * OpenVRML::ProtoNode::toElevationGrid() const       
{ return firstNode() ? firstNode()->toElevationGrid() : 0; }

OpenVRML::NodeExtrusion * OpenVRML::ProtoNode::toExtrusion() const       
{ return firstNode() ? firstNode()->toExtrusion() : 0; }

OpenVRML::NodeFog * OpenVRML::ProtoNode::toFog() const
{ return firstNode() ? firstNode()->toFog() : 0; }

OpenVRML::NodeFontStyle * OpenVRML::ProtoNode::toFontStyle() const
{ return firstNode() ? firstNode()->toFontStyle() : 0; }

OpenVRML::NodeGeometry * OpenVRML::ProtoNode::toGeometry() const
{ return firstNode() ? firstNode()->toGeometry() : 0; }

OpenVRML::NodeGroup * OpenVRML::ProtoNode::toGroup() const
{ return firstNode() ? firstNode()->toGroup() : 0; }

OpenVRML::NodeImageTexture * OpenVRML::ProtoNode::toImageTexture() const
{ return firstNode() ? firstNode()->toImageTexture() : 0; }

OpenVRML::NodeIFaceSet * OpenVRML::ProtoNode::toIFaceSet() const  
{ return firstNode() ? firstNode()->toIFaceSet() : 0; }

OpenVRML::NodeInline * OpenVRML::ProtoNode::toInline() const
{ return firstNode() ? firstNode()->toInline() : 0; }

OpenVRML::NodeLight * OpenVRML::ProtoNode::toLight() const
{ return firstNode() ? firstNode()->toLight() : 0; }

OpenVRML::NodeLOD * OpenVRML::ProtoNode::toLOD() const
{ return firstNode() ? firstNode()->toLOD() : 0; }

OpenVRML::NodeMaterial * OpenVRML::ProtoNode::toMaterial() const
{ return firstNode() ? firstNode()->toMaterial() : 0; }

OpenVRML::NodeMovieTexture * OpenVRML::ProtoNode::toMovieTexture() const
{ return firstNode() ? firstNode()->toMovieTexture() : 0; }

OpenVRML::NodeNavigationInfo * OpenVRML::ProtoNode::toNavigationInfo() const
{ return firstNode() ? firstNode()->toNavigationInfo() : 0; }

OpenVRML::NodeNormal * OpenVRML::ProtoNode::toNormal() const
{ return firstNode() ? firstNode()->toNormal() : 0; }

OpenVRML::NodeOrientationInt * OpenVRML::ProtoNode::toOrientationInt() const
{ return firstNode() ? firstNode()->toOrientationInt() : 0; }

OpenVRML::NodePixelTexture * OpenVRML::ProtoNode::toPixelTexture() const
{ return firstNode() ? firstNode()->toPixelTexture() : 0; }
 
OpenVRML::NodePlaneSensor * OpenVRML::ProtoNode::toPlaneSensor() const
{ return firstNode() ? firstNode()->toPlaneSensor() : 0; }

OpenVRML::NodePointLight * OpenVRML::ProtoNode::toPointLight() const
{ return firstNode() ? firstNode()->toPointLight() : 0; }

OpenVRML::NodePositionInt * OpenVRML::ProtoNode::toPositionInt() const    
{ return firstNode() ? firstNode()->toPositionInt() : 0; }

OpenVRML::NodeShape * OpenVRML::ProtoNode::toShape() const  
{ return firstNode() ? firstNode()->toShape() : 0; }

OpenVRML::NodeSphereSensor * OpenVRML::ProtoNode::toSphereSensor() const
{ return firstNode() ? firstNode()->toSphereSensor() : 0; }

OpenVRML::NodeScalarInt * OpenVRML::ProtoNode::toScalarInt() const    
{ return firstNode() ? firstNode()->toScalarInt() : 0; }

OpenVRML::ScriptNode * OpenVRML::ProtoNode::toScript() const
{ return firstNode() ? firstNode()->toScript() : 0; }

OpenVRML::NodeSound * OpenVRML::ProtoNode::toSound() const
{ return firstNode() ? firstNode()->toSound() : 0; }

OpenVRML::NodeSphere * OpenVRML::ProtoNode::toSphere() const       
{ return firstNode() ? firstNode()->toSphere() : 0; }

OpenVRML::NodeSpotLight * OpenVRML::ProtoNode::toSpotLight() const
{ return firstNode() ? firstNode()->toSpotLight() : 0; }

OpenVRML::NodeSwitch * OpenVRML::ProtoNode::toSwitch() const       
{ return firstNode() ? firstNode()->toSwitch() : 0; }

OpenVRML::NodeTexture * OpenVRML::ProtoNode::toTexture() const
{ return firstNode() ? firstNode()->toTexture() : 0; }

OpenVRML::NodeTextureCoordinate * OpenVRML::ProtoNode::toTextureCoordinate() const
{ return firstNode() ? firstNode()->toTextureCoordinate() : 0; }

OpenVRML::NodeTextureTransform * OpenVRML::ProtoNode::toTextureTransform() const
{ return firstNode() ? firstNode()->toTextureTransform() : 0; }

OpenVRML::NodeTimeSensor * OpenVRML::ProtoNode::toTimeSensor() const
{ return firstNode() ? firstNode()->toTimeSensor() : 0; }

OpenVRML::NodeTouchSensor * OpenVRML::ProtoNode::toTouchSensor() const
{ return firstNode() ? firstNode()->toTouchSensor() : 0; }

OpenVRML::NodeTransform * OpenVRML::ProtoNode::toTransform() const       
{ return firstNode() ? firstNode()->toTransform() : 0; }

OpenVRML::NodeViewpoint * OpenVRML::ProtoNode::toViewpoint() const
{ return firstNode() ? firstNode()->toViewpoint() : 0; }

void OpenVRML::ProtoNode::render(Viewer *viewer, VrmlRenderContext rc)
{
  if (! d_instantiated)
    {
      theSystem->debug("ProtoNode::%s render before instantiation\n",
		       this->getId().c_str());
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
      d_viewerObject = viewer->beginObject(this->getId().c_str());

      // render the nodes with the new values
      int n = this->implNodes.getLength();
      for (int j = 0; j<n; ++j)
	this->implNodes.getElement(j)->render(viewer, rc);

      viewer->endObject();
    }

  clearModified();
}


void OpenVRML::ProtoNode::eventIn(double timeStamp,
                                  const std::string & eventName,
                                  const FieldValue & fieldValue) {
    if (!d_instantiated) {
        theSystem->debug("ProtoNode::%s eventIn before instantiation\n",
                         this->getId().c_str());
        instantiate();
    }

    static const char * eventInPrefix = "set_";
    std::string basicEventName;
    if (std::equal(eventInPrefix, eventInPrefix + 4, eventName.begin())) {
        basicEventName = eventName.substr(4);
    } else {
        basicEventName = eventName;
    }

    typedef std::list<EventDispatch *> EventDispatchList;
    for (EventDispatchList::iterator i(d_eventDispatch.begin());
            i != d_eventDispatch.end(); ++i) {
//        cout << "basicEventName = " << basicEventName
//             << ", eventName = " << eventName
//             << ", (*i)->name = " << (*i)->name << endl;
        if (basicEventName == (*i)->name || eventName == (*i)->name) {
            NodeType::ISMap * ismap = &(*i)->ismap;
            for (NodeType::ISMap::iterator j(ismap->begin());
                    j != ismap->end(); ++j) {
//                cout << "Relaying event." << endl;
                (*j)->node->eventIn(timeStamp, (*j)->fieldName, fieldValue);
            }
	    return;
        }
    }
    // Let the generic code handle errors.
    Node::eventIn(timeStamp, eventName, fieldValue);
    this->setBVolumeDirty(true); // lazy calc of bvolume
}

/**
 * Find a field by name.
 */
OpenVRML::ProtoNode::NameValueRec *
        OpenVRML::ProtoNode::findField(const std::string & fieldName) const {
    for (std::list<NameValueRec *>::const_iterator i(d_fields.begin());
            i != d_fields.end(); ++i) {
        if (*i && (*i)->name == fieldName) {
            return *i;
        }
    }
    return 0;
}

/**
 * Set the value of one of the node fields (creates the field if
 * it doesn't exist).
 *
 * @todo Is it necessary to create the field if it doesn't exist?
 */
void OpenVRML::ProtoNode::setField(const std::string & fieldName,
                                   const FieldValue & fieldValue) {
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

const OpenVRML::FieldValue *
        OpenVRML::ProtoNode::getField(const std::string & fieldName) const {
    NameValueRec * nv = findField(fieldName);
    if (nv) {
        return nv->value;
    }
    return this->type.fieldDefault(fieldName);
}

const OpenVRML::MFNode & OpenVRML::ProtoNode::getImplNodes() const {
    return this->implNodes;
}

const OpenVRML::BVolume * OpenVRML::ProtoNode::getBVolume() const
{
  //cout << "ProtoNode::getBVolume() {" << endl;
  if (!d_instantiated) {
    cout << "ProtoNode::getBVolume():WARNING:not instantiated" << endl;
    return 0; // shouldn't happen
  }
  const NodePtr base(firstNode());
  if (!base) {
    cout << "ProtoNode::getBVolume():WARNING:!base" << endl;
    return 0; // shouldn't happen
  }
  const BVolume* bv = base->getBVolume();
  //cout << "}";
  //bv->dump(cout) << endl;
  return bv;
}

/**
 * @brief determine whether or not implementation is modified
 */
bool OpenVRML::ProtoNode::isModified() const {
  if ( d_modified ) return 1;
  int n = implNodes.getLength();
  for ( int i=0; i<n; ++i )
    if ( implNodes.getElement(i)->isModified() ) return 1;
  return 0;
}

void OpenVRML::ProtoNode::updateModified(NodePath& path, int flags)
{
  //cout << "ProtoNode::updateModified()" << endl;
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  const NodePtr base(firstNode());
  if (base) base->updateModified(path, flags);    
  path.pop_front();
}
