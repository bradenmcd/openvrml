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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "node.h"
#include "Route.h"
#include "VrmlNamespace.h"
#include "VrmlScene.h"
#include "MathUtils.h"
#include "bvolume.h"
#include "VrmlRenderContext.h"
#include "doc2.hpp"
#include "proto.h" // Instances of PROTOtyped nodes

# ifndef NDEBUG
#   define VRML_NODE_DEBUG
# endif

namespace OpenVRML {

ostream & operator<<(ostream & os, const Node & f)
{ return f.print(os, 0); }

/**
 * @class NodeType
 *
 * @brief The NodeType class is responsible for storing information
 *      about node or prototype types.
 */

/**
 * @brief Constructor.
 *
 * Takes name of new type (e.g. "Transform" or "Box"). Copies the string
 * given as name.
 *
 * @param name the name of the node.
 * @param creator a factory function for creating nodes of this type.
 */
NodeType::NodeType(const std::string & id,
                   const NodePtr (*creator)(VrmlScene * const scene)):
        id(id), d_namespace(0), d_url(0), d_relative(0), d_creator(creator),
        d_fieldsInitialized(false) {}

namespace {
    void destructFieldList(NodeType::FieldList & f) {
        for (NodeType::FieldList::iterator i = f.begin(); i != f.end(); ++i) {
        
            if ((*i)->defaultValue) { delete (*i)->defaultValue; }
            
            // free NodeFieldRec* s in r->thisIS;
            for (NodeType::ISMap::const_iterator j = (*i)->thisIS.begin();
                    j != (*i)->thisIS.end(); ++j) {
                delete *j;
            }
            delete *i;
        }
    }
}

/**
 * @brief Destructor.
 *
 * Deallocate storage for name and PROTO implementations.
 */
NodeType::~NodeType() {
    delete d_namespace;
    delete d_url;
    delete d_relative;

    // Free strings & defaults duplicated when fields/eventIns/eventOuts added:
    destructFieldList( d_eventIns );
    destructFieldList( d_eventOuts );
    destructFieldList( d_fields );
}

/**
 * @brief Create a new instance of a node of this type.
 *
 * Built in nodes have a creator function specified, while instances of
 * PROTOs are constructed by NodeProto.
 *
 * @param scene the VrmlScene to which this new node will belong
 *
 * @return a new Node
 */
const NodePtr NodeType::newNode(VrmlScene * scene) const {
    if (this->d_creator) {
        return (*this->d_creator)(scene);
    }
    return NodePtr(new ProtoNode(*this, scene));
}

/**
 * @brief Get the node name defined by this type.
 */
const std::string & NodeType::getId() const {
    return this->id;
}

/**
 * @brief Get the scope of this type.
 *
 * @return a pointer to the VrmlNamespace corresponding to this node type's
 *      scope, or 0 for built-in node types.
 */
VrmlNamespace * NodeType::getScope() const { return this->d_namespace; }

/**
 * @brief Set the scope of this type.
 *
 * @param scope
 */
void NodeType::setScope(VrmlNamespace & scope) {
    this->d_namespace = new VrmlNamespace(&scope);
}

/**
 * @brief Set the URL from which an EXTERNPROTO's implementation was
 *      successfully retrieved.
 *
 * @param url
 */
void NodeType::setActualUrl(const std::string & url) {
    this->actualUrl = url;
}

/**
 * @brief Retrieve the actual URL the PROTO was retrieved from.
 */
const std::string & NodeType::getActualUrl() const {
    return this->actualUrl;
}

namespace {
    //
    // Helper method to add a field or event.
    //
    void add(NodeType::FieldList & recs, const std::string & id,
             FieldValue::Type type) {
        NodeType::ProtoField * const protoField =
                new NodeType::ProtoField;
        protoField->name = id;
        protoField->type = type;
        protoField->defaultValue = 0;
        recs.push_front(protoField);
    }
}

/**
 * @brief Add an eventIn to this type.
 *
 * @param id
 * @param type
 */
void NodeType::addEventIn(const std::string & id, FieldValue::Type type) {
    add(d_eventIns, id, type);
}

/**
 * @brief Add an eventOut to this type.
 *
 * @param id
 * @param type
 */
void NodeType::addEventOut(const std::string & id, FieldValue::Type type) {
    add(d_eventOuts, id, type);
}

/**
 * @brief Add a field to this type.
 *
 * @param id
 * @param type
 * @param defaultValue
 */
void NodeType::addField(const std::string & id, const FieldValue::Type type,
                        const FieldValue * defaultValue) {
    add(d_fields, id, type);
    if (defaultValue) {
        this->setFieldDefault(id, defaultValue);
    }
}

/**
 * @brief Add an exposedField to this type.
 *
 * @param id
 * @param type
 * @param defaultValue
 */
void NodeType::addExposedField(const std::string & id,
                               const FieldValue::Type type,
                               const FieldValue * const defaultValue) {
    add(d_fields, id, type);
    if (defaultValue) {
        this->setFieldDefault(id, defaultValue);
    }

    add(d_eventIns, "set_" + id, type);
    add(d_eventOuts, id + "_changed", type);
}

/**
 * @brief Set the default value for a field or exposedField.
 *
 * @param fname field name
 * @param defaultValue default value
 */
void NodeType::setFieldDefault(const std::string & fname,
                               const FieldValue * defaultValue) {
    for (FieldList::const_iterator i(d_fields.begin()); i != d_fields.end();
            ++i) {
        if (fname == (*i)->name) {
            if ((*i)->defaultValue) {
                theSystem->error("Default for field %s of %s already set.",
                                 fname.c_str(), this->id.c_str());
	        delete (*i)->defaultValue;
            }
	    (*i)->defaultValue = defaultValue ? defaultValue->clone() : 0;
	    return;
        }
    }      
    theSystem->error("setFieldDefault for field %s of %s failed: no such field.",
                     fname.c_str(), this->id.c_str());
}


/**
 * @brief Download the EXTERNPROTO definition.
 */
void NodeType::fetchImplementation() const {
  // Get the PROTO def from the url (relative to original scene url).
  const NodeTypePtr proto =
        VrmlScene::readPROTO(*this->d_url, this->d_relative);
  if (proto)
    {
      // check type of first node...
      // steal nodes without cloning
      this->implNodes = proto->implNodes;

      // Make sure we get all the IS maps by using the field
      // lists from the implementation rather than just the
      // interface.
      destructFieldList( d_eventIns );
      destructFieldList( d_eventOuts );
      destructFieldList( d_fields );

      d_eventIns = proto->d_eventIns;
      d_eventOuts = proto->d_eventOuts;
      d_fields = proto->d_fields;

      proto->d_eventIns.erase(proto->d_eventIns.begin(),
			      proto->d_eventIns.end());
      proto->d_eventOuts.erase(proto->d_eventOuts.begin(),
			      proto->d_eventOuts.end());
      proto->d_fields.erase(proto->d_fields.begin(),
			    proto->d_fields.end());

      this->actualUrl = proto->getActualUrl();
    }
  else
    {
      theSystem->warn("Couldn't read EXTERNPROTO %s\n",
                      this->id.c_str());
    }
}

/**
 * @brief Get the implementation nodes; download them if necessary.
 *
 * @return the implementation nodes
 */
const MFNode & NodeType::getImplementationNodes() const {
  if ((this->implNodes.getLength() == 0) && d_url)
    fetchImplementation();

  // Now that the nodes are here, initialize any IS'd fields
  // to the default values (could do it at instantiation...)
  // Also, any IS'd field needs a name because of the stupid
  // IS mapping implementation (when I copy the implementation
  // nodes for each instance of the PROTO, the IS mappings
  // need to copied too). So make up a unique name for those
  // without.
  if (! d_fieldsInitialized)
    {
      FieldList::iterator i;
      char buf[32];

      for (i = d_fields.begin(); i != d_fields.end(); ++i)
	{
	  ISMap &ismap = (*i)->thisIS;
	  ISMap::iterator j;
	  for (j = ismap.begin(); j != ismap.end(); ++j)
	    {
	      const NodePtr & n = (*j)->node;
	      if (n->getId().empty())
		{
		  sprintf(buf,"#%lx", (unsigned long) n);
		  n->setId(buf);
		}

	      if ((*i)->defaultValue)
		n->setField( (*j)->fieldName, *((*i)->defaultValue) );

	    }
	}

      // Set names on IS'd eventIns/Outs, too.
      for (i = d_eventIns.begin(); i != d_eventIns.end(); ++i)
	{
	  ISMap &ismap = (*i)->thisIS;
	  ISMap::iterator j;
	  for (j = ismap.begin(); j != ismap.end(); ++j)
	    {
	      const NodePtr & n = (*j)->node;
	      if (n->getId().empty())
		{
		  sprintf(buf,"#%lx", (unsigned long) n);
		  n->setId(buf);
		}
	    }
	}
      for (i = d_eventOuts.begin(); i != d_eventOuts.end(); ++i)
	{
	  ISMap &ismap = (*i)->thisIS;
	  ISMap::iterator j;
	  for (j = ismap.begin(); j != ismap.end(); ++j)
	    {
	      const NodePtr & n = (*j)->node;
	      if (n->getId().empty())
		{
		  sprintf(buf,"#%lx", (unsigned long) n);
		  n->setId(buf);
		}
	    }
	}

      d_fieldsInitialized = true;
    }
    
  return this->implNodes;
}


/**
 * @brief Get the first node in the implementation.
 *
 * This will NOT fetch the implementation of an EXTERNPROTO.
 * This method is used in NodeProto to check the type of
 * SFNode fields in toXXX() node downcasts. Type checking
 * of EXTERNPROTOs is deferred until the implementation is
 * actually downloaded. (not actually done yet...)
 */
const NodePtr NodeType::firstNode() const {
    return (this->implNodes.getLength() > 0)
            ? this->implNodes.getElement(0)
            : NodePtr(0);
}

/**
 * @brief Check whether the node type has a particular eventIn.
 *
 * @param id the name of an eventIn to check for
 *
 * @return the Type of the eventIn if it exists, or
 *      FieldValue::invalidType otherwise.
 */
FieldValue::Type NodeType::hasEventIn(const std::string & id) const {
    return has(this->d_eventIns, id);
}

/**
 * @brief Check whether the node type has a particular eventOut.
 *
 * @param id the name of an eventOut to check for
 *
 * @return the Type of the eventOut if it exists, or
 *      FieldValue::invalidType otherwise
 */
FieldValue::Type NodeType::hasEventOut(const std::string & id) const {
    return has(this->d_eventOuts, id);
}

/**
 * @brief Check whether the node type has a particular field.
 *
 * @param id the name of an field to check for
 *
 * @return the Type of the field if it exists, or
 *      FieldValue::invalidType otherwise.
 */
FieldValue::Type NodeType::hasField(const std::string & id) const {
    return has(this->d_fields, id);
}

/**
 * @brief Check whether the node type has a particular exposedField.
 *
 * @param id the name of an exposedField to check for
 *
 * @return the Type of the exposedField if it exists, or
 *      FieldValue::invalidType otherwise.
 */
FieldValue::Type NodeType::hasExposedField(const std::string & id) const {
    // Must have field "name", eventIn "set_name", and eventOut
    // "name_changed", all with same type:
    FieldValue::Type type;

    if ((type = this->has(this->d_fields, id)) == FieldValue::invalidType) {
        return FieldValue::invalidType;
    }

    if (type != this->has(this->d_eventIns, "set_" + id)) {
        return FieldValue::invalidType;
    }

    if (type != this->has(this->d_eventOuts, id + "_changed")) {
        return FieldValue::invalidType;
    }

    return type;
}

/**
 * @brief Check whether the node type has a particular interface.
 *
 * Check to see whether a eventIn, eventOut, field, or exposedField with the
 * given name exists.
 *
 * @param id the name of an interface to check for
 *
 * @return the Type of the interface if it exists, or
 *      FieldValue::invalidType otherwise.
 */
FieldValue::Type NodeType::hasInterface(const std::string & id) const {
    FieldValue::Type fieldType = FieldValue::invalidType;
    
    if ((fieldType = this->hasField(id)) != FieldValue::invalidType) {
        return fieldType;
    }
    
    if ((fieldType = this->hasEventIn(id)) != FieldValue::invalidType) {
        return fieldType;
    }
    
    if ((fieldType = this->hasEventOut(id)) != FieldValue::invalidType) {
        return fieldType;
    }
    
    return fieldType;
}

FieldValue::Type NodeType::has(const FieldList & recs,
                               const std::string & id) const {
    for (FieldList::const_iterator i(recs.begin()); i != recs.end(); ++i) {
        if ((*i)->name == id) {
            return (*i)->type;
        }
    }
    return FieldValue::invalidType;
}

const FieldValue * NodeType::fieldDefault(const std::string & fname) const {
    for (FieldList::const_iterator i(d_fields.begin()); i != d_fields.end();
            ++i) {
        if ((*i)->name == fname) {
            return (*i)->defaultValue;
        }
    }
    return 0;
}

/**
 * @brief Set the URL to retrieve an EXTERNPROTO implementation from.
 *
 * @param url An MFString list of URIs at which the implementation may
 *      reside.
 * @param relative the resource to which the URIs in the first parameter
 *      are relative.
 */
void NodeType::setUrl(const MFString & url, const Doc2 * relative) {
    assert(this->implNodes.getLength() == 0);
    
    delete this->d_url;
    this->d_url = new MFString(url);
    
    delete this->d_relative;
    this->d_relative = relative ? new Doc2(relative) : 0;
}

void NodeType::addNode(Node & node) {
    // add node to list of implementation nodes
    this->implNodes.addNode(node);
}

void NodeType::addIS(const std::string & isFieldName, Node & implNode,
                     const std::string & implFieldName) {
  FieldList::iterator i;

  // Fields
  for (i = d_fields.begin(); i != d_fields.end(); ++i)
    {
      if ((*i)->name == isFieldName)
	{
	  NodeFieldRec *nf = new NodeFieldRec;
	  nf->node.reset(&implNode);
	  nf->fieldName = implFieldName;
	  (*i)->thisIS.push_front(nf);
	  break;
	}
    }

  // EventIns
  for (i = d_eventIns.begin(); i != d_eventIns.end(); ++i)
    {
      if ((*i)->name == isFieldName)
	{
	  NodeFieldRec *nf = new NodeFieldRec;
	  nf->node.reset(&implNode);
	  nf->fieldName = implFieldName;
	  (*i)->thisIS.push_front(nf);
	  break;
	}
    }

  // EventOuts
  for (i = d_eventOuts.begin(); i != d_eventOuts.end(); ++i)
    {
      if ((*i)->name == isFieldName)
	{
	  NodeFieldRec *nf = new NodeFieldRec;
	  nf->node.reset(&implNode);
	  nf->fieldName = implFieldName;
	  (*i)->thisIS.push_front(nf);
	  break;
	}
    }
}


const NodeType::ISMap *
        NodeType::getFieldISMap(const std::string & fieldName) const {
    for (FieldList::const_iterator i(d_fields.begin()); i != d_fields.end();
            ++i) {
        if ((*i)->name == fieldName) {
            return & ((*i)->thisIS);
        }
    }
    return 0;
}

/**
 * @struct NodeType::NodeFieldRec
 *
 * @brief A pointer to a node and the name of an interface of that node.
 *
 * This struct is used to identify nodes in the implementation as part of the
 * <code>IS</code> mapping.
 */

/**
 * @typedef NodeType::ISMap
 *
 * @brief An <code>IS</code> mapping.
 *
 * A list comprising pointers to NodeFieldRec's which correspond to nodes in
 * a PROTO implementation.
 */

/**
 * @struct NodeType::ProtoField
 *
 * @brief A field in a <code>PROTO</code>.
 */

/**
 * @typedef NodeType::FieldList
 *
 * @brief A list of fields in a <code>PROTO</code>.
 */


/**
 * @class Node
 *
 * @brief A node in the scene graph.
 */

/*
 * Given a NodeType, add in the fields, exposedFields, eventIns
 * and eventOuts defined by the particular node implementation.
 * There's a great big method in VrmlNamespace that just calls
 * defineType for every built in node. Nodes that inherit from other
 * nodes (eg NodeIFaceSet) must call their parent classe's
 * defineType, it doesn't happen automatically. The defineType for
 * Node doesn't actually do anything, since there (currently)
 * aren't any base events to be defined. It's just here for the sake
 * of symmetry (and makes a nice place to park a comment)
 *
 * @param type to get the predefined cached type, pass a null,
 *          otherwise the fields and events will be added to the
 *          passed in vrmlnodetype object
 *
 * @see VrmlNamespace::defineBuiltins()
 */
Node::Node(const NodeType & type, VrmlScene * scene):
        type(type), d_scene(scene), d_modified(false),
        visited(false), d_routes(0) {
  this->setBVolumeDirty(true);
}

Node::Node(const Node & node): type(node.type), id(node.id),
        d_scene(0), d_modified(true), d_routes(0) {
    this->setBVolumeDirty(true);
}


/**
 * @brief Destructor.
 *
 * Free name (if any) and route info.
 */
Node::~Node() {
    // Remove the node's name (if any) from the map...
    if (!this->id.empty()) {
        if (this->d_scene && d_scene->scope()) {
            this->d_scene->scope()->removeNodeName(*this);
        }
    }

    // Remove all routes from this node
    this->d_routes.resize(0);
}

/**
 * @fn void Node::accept(NodeVisitor & visitor);
 *
 * @brief Accept a visitor.
 *
 * If the node has not been visited, set the <var>visited</var> flag to
 * <code>true</code> and call <code>NodeVisitor::visit()</code> for this object.
 * Otherwise (if the <var>visited</var> flag is already
 * <code>true</code>), this method has no effect.
 *
 * <p>The fact that the <var>visited</var> flag is set <em>before</em> the
 * node is actually visited is an important detail. Even though scene
 * graphs should not have cycles, nodes can be self-referencing: a field
 * of a <code>Script</code> node can legally <code>USE</code> the
 * <code>Script</code> node. (This does not pose a problem for rendering
 * since nodes in a <code>Script</code> node's fields are not part of
 * the transformation hierarchy.)
 *
 * @param visitor
 *
 * @return <code>true</code> if the visitor is accepted (the node
 *         <em>has not</em> been visited during this traversal),
 *         <code>false</code> otherwise (the node <em>has</em> been
 *         visited during this traversal.
 */

/**
 * @brief Recursively set the <var>visited</var> flag to
 *        <code>false</code> for this node and its children.
 *
 * Typically used by a visitor (a class that implements NodeVisitor)
 * after traversal is complete. The default implementation is only
 * appropriate for nodes with no child nodes.
 */
void Node::resetVisitedFlag() {
    this->visited = false;
}

/**
 * @brief Set the nodeId of the node.
 *
 * Some one else (the parser) needs to tell the scene about the name for
 * use in USE/ROUTEs.
 *
 * @param nodeName a string
 * @param ns a pointer to the VrmlNamespace to which this node should
 *           belong
 */
void Node::setId(const std::string & nodeId, VrmlNamespace * const ns) {
    this->id = nodeId;
    if (!nodeId.empty() && ns) {
        ns->addNodeName(*this);
    }
}

/**
 * @brief Retrieve the nodeId of this node.
 *
 * @return the nodeId
 */
const std::string & Node::getId() const { return this->id; }

/**
 * @brief Add to scene.
 *
 * A node can belong to at most one scene for now. If it doesn't belong
 * to a scene, it can't be rendered.
 *
 * @param scene
 * @param relativeUrl
 */
void Node::addToScene(VrmlScene * scene, const std::string &) {
    this->d_scene = scene;
}


// Safe node downcasts. These avoid the dangerous casts of Node* (esp in
// presence of protos), but are ugly in that this class must know about all
// the subclasses. These return 0 if the typecast is invalid.
// Remember to also add new ones to NodeProto. Protos should
// return their first implementation node (except toProto()).

NodeAnchor * Node::toAnchor() const { return 0; }

NodeAppearance * Node::toAppearance() const { return 0; }

NodeAudioClip * Node::toAudioClip() const { return 0; }

NodeBackground * Node::toBackground() const { return 0; }

NodeBillboard * Node::toBillboard() const { return 0; }

NodeBox * Node::toBox() const { return 0; }

NodeChild * Node::toChild() const { return 0; }

NodeCollision * Node::toCollision() const { return 0; }

NodeColor * Node::toColor() const { return 0; }

NodeCone * Node::toCone() const { return 0; }

NodeCoordinate * Node::toCoordinate() const { return 0; }

NodeCylinder * Node::toCylinder() const { return 0; }

NodeDirLight * Node::toDirLight() const { return 0; }

NodeElevationGrid * Node::toElevationGrid() const { return 0; }

NodeExtrusion * Node::toExtrusion() const { return 0; }

NodeFog * Node::toFog() const { return 0; }

NodeFontStyle * Node::toFontStyle() const { return 0; }

NodeGeometry * Node::toGeometry() const { return 0; }

NodeGroup * Node::toGroup() const { return 0; }

NodeIFaceSet * Node::toIFaceSet() const { return 0; }

NodeImageTexture * Node::toImageTexture() const { return 0; }

NodeInline * Node::toInline() const { return 0; }

NodeLight * Node::toLight() const { return 0; }

NodeLOD * Node::toLOD() const { return 0; }

NodeMaterial * Node::toMaterial() const { return 0; }

NodeMovieTexture * Node::toMovieTexture() const { return 0; }

NodeNavigationInfo * Node::toNavigationInfo() const { return 0; }

NodeNormal * Node::toNormal() const { return 0; }

NodeOrientationInt * Node::toOrientationInt() const { return 0; }

NodePlaneSensor * Node::toPlaneSensor() const { return 0; }

NodePositionInt * Node::toPositionInt() const { return 0; }

NodeSphereSensor * Node::toSphereSensor() const { return 0; }

NodeCylinderSensor * Node::toCylinderSensor() const { return 0; }

NodePixelTexture * Node::toPixelTexture() const { return 0; }

NodePointLight * Node::toPointLight() const { return 0; }

NodePointSet * Node::toPointSet() const { return 0; }

NodeScalarInt * Node::toScalarInt() const { return 0; }

ScriptNode * Node::toScript() const { return 0; }

NodeShape * Node::toShape() const { return 0; }

NodeSound * Node::toSound() const { return 0; }

NodeSphere * Node::toSphere() const { return 0; }

NodeSpotLight * Node::toSpotLight() const { return 0; }

NodeSwitch * Node::toSwitch() const { return 0; }

NodeTexture * Node::toTexture() const { return 0; }

NodeTextureCoordinate * Node::toTextureCoordinate() const { return 0; }

NodeTextureTransform * Node::toTextureTransform() const { return 0; }

NodeTimeSensor * Node::toTimeSensor() const { return 0; }

NodeTouchSensor * Node::toTouchSensor() const { return 0; }

NodeTransform * Node::toTransform() const { return 0; }

NodeViewpoint * Node::toViewpoint() const { return 0; }


/**
 * @brief Add a route from an eventOut of this node to an eventIn of another
 *      node.
 */
void Node::addRoute(const std::string & fromEventOut,
                    const NodePtr & toNode,
                    const std::string & toEventIn) {
  // Check to make sure fromEventOut and toEventIn are valid names...
  
  // Is this route already here?
  RouteList::iterator i;

  for (i = d_routes.begin(); i != d_routes.end(); ++i)
  {
    if (toNode == (*i)->toNode
	&& fromEventOut == (*i)->fromEventOut
	&& toEventIn == (*i)->toEventIn)
      return;       // Ignore duplicate routes
  }

  // Add route
  Route* r = new Route(fromEventOut, toNode, toEventIn);

  d_routes.push_back(r);
}


/**
 * @brief Remove a route from an eventOut of this node to an eventIn of another
 *      node.
 */
void Node::deleteRoute(const std::string & fromEventOut,
                       const NodePtr & toNode,
                       const std::string & toEventIn) {
  RouteList::iterator i;

  for (i = d_routes.begin(); i != d_routes.end(); ++i)
  {
    if (toNode == (*i)->toNode
	&& fromEventOut == (*i)->fromEventOut
	&& toEventIn == (*i)->toEventIn)
    {
      i = d_routes.erase(i);
    }
  }
}

/**
 * @brief Get the routes from this node.
 *
 * @return an std::vector of Routes from this node.
 */
std::list<Route *> Node::getRoutes() {
    return this->d_routes;
}

// Dirty bit - indicates node needs to be revisited for rendering.

void Node::setModified()
{
  d_modified = true;
  if (d_scene) d_scene->setModified(); 
}

bool Node::isModified() const
{
  return d_modified; 
}


/**
 * @brief Mark all the nodes in the path as (not) modified.
 *
 * Convenience function used by updateModified.
 *
 * @param path
 * @param mod
 * @param flags
 */
void Node::markPathModified(NodePath& path, bool mod, int flags) {
  NodePath::iterator i;
  NodePath::iterator end = path.end();
  if (flags & 0x001) {
    for (i = path.begin(); i != end; ++i) {
      Node *c = *i;
      if (mod) {
	// do the proof that our invarient shows that this short
	// circuit is legal...
	c->setModified();
      } else
	c->clearModified();
    }
  }
  if (flags & 0x002) {
    for (i = path.begin(); i != end; ++i) {
      Node *c = *i;
      if (mod) {
	c->setBVolumeDirty(true);
      } else
	c->setBVolumeDirty(false);
    }
  }
}


void Node::updateModified(NodePath& path, int flags) {
  if (this->d_modified||this->d_bvol_dirty) markPathModified(path, true, flags);
}


/**
 * Propagate the bvolume dirty flag from children to parents. I
 * don't like this at all, but it's not worth making it pretty
 * because the need for it will go away when parent pointers are
 * implemented.
 *
 * @param path stack of ancestor nodes
 * @param mod set modified flag to this value
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both
 */
// note not virtual
//
void Node::updateModified(int flags)
{
  NodePath path;
  updateModified(path, flags);
}


void Node::clearFlags()
{
  d_flag = false;
}

// Render

//bool Node::cull(Viewer *v, RenderContext* c)
//{
//if (c && c->getCullFlag()) {
//VrmlBVolume* bv = this->getBVolume();
//int r = v->isectViewVolume(*bv); // better not be null...
//if (r == VrmlBVolume::BV_OUTSIDE) {
//cout << "Node::render():OUTSIDE:culled" << endl;
//return true;
//} else if (r == VrmlBVolume::BV_INSIDE) {
//cout << "Node::render():INSIDE:no more cull tests" << endl;
//c->setCullFlag(false);
//return false;
//} else {
//cout << "Node::render():PARTIAL:continue cull tests" << endl;
//return false;
//}
//}
//return false;
//}


/**
 * @brief Get this node's bounding volume.
 *
 * Nodes that have no bounding volume, or have a difficult to calculate
 * bvolume (like, say, Extrusion or Billboard) can just return an infinite
 * bsphere. Note that returning an infinite bvolume means that all the node's
 * ancestors will also end up with an infinite bvolume, and will never be
 * culled.
 *
 * @return this node's bounding volume
 */
const BVolume * Node::getBVolume() const {
  static BSphere * inf_bsphere = 0;
  if (!inf_bsphere) {
    inf_bsphere = new BSphere();
    inf_bsphere->setMAX();
  }
  const_cast<Node *>(this)->setBVolumeDirty(false);
  return inf_bsphere;
}


/**
 * Override a node's calculated bounding volume. Not implemented.
 *
 * @todo Implement me!
 */
void Node::setBVolume(const BVolume & v) {
    // XXX Implement me!
}

/** 
 * Indicate that a node's bounding volume needs to be recalculated
 * (or not). If a node's bvolume is invalid, then the bvolumes of
 * all that node's ancestors are also invalid. Normally, the node
 * itself will determine when its bvolume needs updating.
 */
void Node::setBVolumeDirty(bool f)
{
  this->d_bvol_dirty = f;
  if (f && this->d_scene) // only if dirtying, not clearing
    this->d_scene->d_flags_need_updating = true;
}

/**
 * Return true if the node's bounding volume needs to be
 * recalculated.
 */
bool Node::isBVolumeDirty() const
{
  if (d_scene && d_scene->d_flags_need_updating) {
    d_scene->updateFlags();
    d_scene->d_flags_need_updating = false;
  }
  return this->d_bvol_dirty;
}

/**
 * Render this node. Actually, most of the rendering work is
 * delegated to the viewer, but this method is responsible for
 * traversal to the node's renderable children, including
 * culling. Each node class needs to implement this routine
 * appropriately. It's not abstract since it doesn't make sense to
 * call render on some nodes. Alternative would be to break render
 * out into a seperate mixins class, but that's probably overkill.
 *
 * @param v viewer implementation responsible for actually doing the
 *          drawing
 * @param rc generic context argument, holds things like the
 *          accumulated modelview transform.
 */
void Node::render(Viewer* v, VrmlRenderContext rc)
{
  //if (cull(v, c)) return;
  clearModified();
}

/**
 * @brief Accumulate transformations for proper rendering of bindable
 *        nodes.
 *
 * Cache a pointer to one of the parent transforms. The resulting
 * pointer is used by getParentTransform. Grouping nodes need to
 * redefine this, the default implementation does nothing.
 *
 * @param p parent node. can be null.
 *
 * @deprecated This routine will go away once parent pointers
 * are implemented.
 */
void Node::accumulateTransform(Node *)
{
  ;
}

/**
 * Return the nearest ancestor node that affects the modelview
 * transform. Doesn't work for nodes with more than one parent.
 */
Node * Node::getParentTransform() { return 0; }

/**
 * Compute the inverse of the transform above a viewpoint node. Just
 * like the version that takes a matrix, but instead calls
 * Viewer::setTransform at each level. The idea is to call this
 * routine right before the start of a render traversal.
 *
 * @see getParentTransform
 *
 * @deprecated This method is (gradually) being replaces by
 * inverseTranform(double[4][4]) and should no longer be used.
 */
void Node::inverseTransform(Viewer *v)
{
  Node *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(v);
}

/**
 * Compute the inverse of the transform stack above a Viewpoint
 * node. This is safe since the behavior of multi-parented
 * Viewpoint nodes is undefined. May be called at any time.
 *
 * @param M return the accumulated inverse
 *
 * @see accumulateTransform
 * @see getParentTransform
 */
void Node::inverseTransform(VrmlMatrix & M) {
    Node * parentTransform = getParentTransform();
    if (parentTransform) {
        parentTransform->inverseTransform(M);
    } else {
        M = VrmlMatrix(); // Set to identity.
    }
}

/**
 * @brief Pass a named event to this node.
 *
 * This method needs to be overridden to support any node-specific eventIns
 * behaviors, but exposedFields (should be) handled here...
 */
void Node::eventIn(double timeStamp, const std::string & eventName,
                   const FieldValue & fieldValue) {
    // Strip set_ prefix
    static const char * eventInPrefix = "set_";
    std::string basicEventName;
    if (std::equal(eventInPrefix, eventInPrefix + 4, eventName.begin())) {
        basicEventName = eventName.substr(4);
    } else {
        basicEventName = eventName;
    }

    // Handle exposedFields 
    if (this->type.hasExposedField(basicEventName)) {
        this->setField(basicEventName, fieldValue);
        std::string eventOutName = basicEventName + "_changed";
        this->eventOut(timeStamp, eventOutName, fieldValue);
        setModified();
    }
    
    // Handle set_field eventIn/field
    else if (this->type.hasEventIn(eventName)
            && this->type.hasField(basicEventName)) {
        this->setField(basicEventName, fieldValue);
        this->setModified();
    } else
        cerr << "Error: unhandled eventIn " << this->type.getId().c_str()
		<< "::" << this->id.c_str() << "." << eventName.c_str() << endl;
}

/**
 * @fn const MFNode Node::getChildren() const
 *
 * @brief Get this node's child nodes as an MFNode.
 *
 * This method is intended to provide generalized access to a node's child
 * nodes. The default implementation returns an empty MFNode. Node
 * implementations that include child nodes should override this method to
 * return an appropriate MFNode.
 *
 * <p>The returned MFNode should include <strong>all</strong> of the node's
 * child nodes, from all of the node's SFNode or MFNode fields. Since fields
 * do not have a defined order, no ordering is defined for the nodes that
 * occur in the returned MFNode. Therefore, traversals that depend on any
 * such ordering should not use this method.</p>
 *
 * @return an MFNode containing any children of this node.
 */
const MFNode Node::getChildren() const {
    return MFNode();
}

/**
 * @brief Send an event from this node.
 */
void Node::eventOut(double timeStamp, const std::string & id,
                    const FieldValue & fieldValue) {

  RouteList::const_iterator i;

  for (i = d_routes.begin(); i != d_routes.end(); ++i)
  {
    if (id == (*i)->fromEventOut)
    {
      FieldValue * eventValue = fieldValue.clone();
      assert(this->d_scene);
      this->d_scene->queueEvent(timeStamp, eventValue, (*i)->toNode,
				(*i)->toEventIn);
    }
  }
}

ostream & Node::print(ostream & os, int indent) const {
  for (int i=0; i<indent; ++i)
    os << ' ';

  if (!this->id.empty()) {
    os << "DEF " << this->id.c_str() << " ";
  }

  os << this->type.getId().c_str() << " { ";

  // cast away const-ness for now...
  Node *n = (Node*)this;
  n->printFields(os, indent+INDENT_INCREMENT);

  os << " }";

  return os; 
}

// This should probably generate an error...
// Might be nice to make this non-virtual (each node would have
// to provide a getField(const char* name) method and specify
// default values in the addField(). The NodeType class would 
// have to make the fields list public.

ostream & Node::printFields(ostream & os, int /*indent*/) {
  os << "# Error: " << this->type.getId().c_str()
     << "::printFields unimplemented.\n";
  return os; 
}


ostream & Node::printField(ostream & os, int indent,
                           const char *name, const FieldValue & f) {
  os << endl;
  for (int i=0; i<indent; ++i)
    os << ' ';
  os << name << ' ' << f;
  return os; 
}


/**
 * @brief Set a field by name (used by the parser, not for external
 *        consumption).
 *
 * Set the value of one of the node fields. No fields exist at the
 * top level, so reaching this indicates an error.
 *
 * @todo Make this method pure virtual.
 */
void Node::setField(const std::string & fieldId, const FieldValue &) {
    theSystem->error("%s::setField: no such field (%s)",
                     this->type.getId().c_str(), fieldId.c_str());
}

/**
 * @brief Get a field or eventOut by name.
 *
 * getField is used by Script nodes to access exposedFields. It does not
 * allow access to private fields (there tend to be specific access
 * functions for each field for programmatic access).
 *
 * @todo Make this method pure virtual.
 */
const FieldValue * Node::getField(const std::string & fieldId) const {
    theSystem->error("%s::getField: no such field (%s)\n",
                     this->type.getId().c_str(), fieldId.c_str());
    return 0;
}


/**
 * @brief Retrieve a named eventOut/exposedField value.
 *
 * Used by the script node to access the node fields. This just strips
 * the _changed suffix and tries to access the field using getField.
 */
const FieldValue * Node::getEventOut(const std::string & fieldName) const {
    static const char * eventOutSuffix = "_changed";
    std::string basicFieldName;
    if (fieldName.length() > 8
            && std::equal(fieldName.end() - 8, fieldName.end(),
                          eventOutSuffix)) {
        basicFieldName = fieldName.substr(0, fieldName.length() - 8);
    } else {
        basicFieldName = fieldName;
    }
    
    // Handle exposedFields 
    if (this->type.hasExposedField(basicFieldName)) {
        return getField(basicFieldName);
    } else if (this->type.hasEventOut(fieldName)) {
        return getField(fieldName);
    }
    return 0;
}


/**
 * @class NodeVisitor
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
 * @fn void NodeVisitor::visit(ProtoNode & node)
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

} // namespace OpenVRML
