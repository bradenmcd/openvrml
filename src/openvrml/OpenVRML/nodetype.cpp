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

#include "nodetype.h"
#include "VrmlNode.h"
#include "VrmlNamespace.h"

#include "doc2.hpp"
#include "VrmlNodeProto.h"	// Instances of PROTOtyped nodes
#include "VrmlScene.h"

#include <stdio.h>

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
                   const VrmlNodePtr (*creator)(VrmlScene *const)):
        id(id), d_namespace(0), d_url(0), d_relative(0), d_creator(creator),
        d_fieldsInitialized(false) {}

namespace {
    void destructFieldList( NodeType::FieldList &f )
    {
      NodeType::FieldList::iterator i;
      for (i = f.begin(); i != f.end(); ++i) {
        NodeType::ProtoField *r = *i;
        if (r->defaultValue) delete r->defaultValue;

        // free NodeFieldRec* s in r->thisIS;
        NodeType::ISMap::const_iterator j;
        for (j = r->thisIS.begin(); j != r->thisIS.end(); ++j)
          {
	    NodeType::NodeFieldRec *nf = *j;
	    delete nf;
          }

        delete r;
      }
    }
}

/**
 * @brief Destructor.
 *
 * Deallocate storage for name and PROTO implementations.
 */
NodeType::~NodeType()
{
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
 * PROTOs are constructed by VrmlNodeProto.
 *
 * @param scene the VrmlScene to which this new node will belong
 *
 * @return a new VrmlNode
 */
const VrmlNodePtr NodeType::newNode(VrmlScene * scene) const
{
  if (d_creator)
    return (*d_creator)( scene );

  return VrmlNodePtr(new VrmlNodeProto(*this, scene));
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
VrmlNamespace * NodeType::getScope() const {
    return this->d_namespace;
}

/**
 * @brief Set the scope of this type.
 *
 * @param scope
 */
void NodeType::setScope(VrmlNamespace & scope)
{
  d_namespace = new VrmlNamespace(&scope);
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
             VrmlField::VrmlFieldType type) {
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
void NodeType::addEventIn(const std::string & id,
                              VrmlField::VrmlFieldType type) {
    add(d_eventIns, id, type);
}

/**
 * @brief Add an eventOut to this type.
 *
 * @param id
 * @param type
 */
void NodeType::addEventOut(const std::string & id,
                               VrmlField::VrmlFieldType type) {
    add(d_eventOuts, id, type);
}

/**
 * @brief Add a field to this type.
 *
 * @param id
 * @param type
 * @param defaultValue
 */
void NodeType::addField(const std::string & id,
                            const VrmlField::VrmlFieldType type,
                            const VrmlField * defaultValue) {
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
                                   const VrmlField::VrmlFieldType type,
                                   const VrmlField * const defaultValue) {
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
                                   const VrmlField * defaultValue) {
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
const VrmlMFNode & NodeType::getImplementationNodes() const {
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
	      const VrmlNodePtr & n = (*j)->node;
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
	      const VrmlNodePtr & n = (*j)->node;
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
	      const VrmlNodePtr & n = (*j)->node;
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
 * This method is used in VrmlNodeProto to check the type of
 * SFNode fields in toXXX() node downcasts. Type checking
 * of EXTERNPROTOs is deferred until the implementation is
 * actually downloaded. (not actually done yet...)
 */
const VrmlNodePtr NodeType::firstNode() const {
    return (this->implNodes.getLength() > 0)
            ? this->implNodes.getElement(0)
            : VrmlNodePtr(0);
}

/**
 * @brief Check whether the node type has a particular eventIn.
 *
 * @param id the name of an eventIn to check for
 *
 * @return the VrmlFieldType of the eventIn if it exists, or
 *      VrmlField::NO_FIELD otherwise.
 */
VrmlField::VrmlFieldType NodeType::hasEventIn(const std::string & id)
        const {
    return has(this->d_eventIns, id);
}

/**
 * @brief Check whether the node type has a particular eventOut.
 *
 * @param id the name of an eventOut to check for
 *
 * @return the VrmlFieldType of the eventOut if it exists, or
 *      VrmlField::NO_FIELD otherwise
 */
VrmlField::VrmlFieldType NodeType::hasEventOut(const std::string & id)
        const {
    return has(this->d_eventOuts, id);
}

/**
 * @brief Check whether the node type has a particular field.
 *
 * @param id the name of an field to check for
 *
 * @return the VrmlFieldType of the field if it exists, or
 *      VrmlField::NO_FIELD otherwise.
 */
VrmlField::VrmlFieldType NodeType::hasField(const std::string & id) const {
    return has(this->d_fields, id);
}

/**
 * @brief Check whether the node type has a particular exposedField.
 *
 * @param id the name of an exposedField to check for
 *
 * @return the VrmlFieldType of the exposedField if it exists, or
 *      VrmlField::NO_FIELD otherwise.
 */
VrmlField::VrmlFieldType NodeType::hasExposedField(const std::string & id)
        const {
    // Must have field "name", eventIn "set_name", and eventOut
    // "name_changed", all with same type:
    VrmlField::VrmlFieldType type;

    if ((type = this->has(this->d_fields, id)) == VrmlField::NO_FIELD) {
        return VrmlField::NO_FIELD;
    }

    if (type != this->has(this->d_eventIns, "set_" + id)) {
        return VrmlField::NO_FIELD;
    }

    if (type != this->has(this->d_eventOuts, id + "_changed")) {
        return VrmlField::NO_FIELD;
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
 * @return the VrmlFieldType of the interface if it exists, or
 *      VrmlField::NO_FIELD otherwise.
 */
VrmlField::VrmlFieldType NodeType::hasInterface(const std::string & id)
        const {
    VrmlField::VrmlFieldType fieldType = VrmlField::NO_FIELD;
    
    if ((fieldType = this->hasField(id)) != VrmlField::NO_FIELD) {
        return fieldType;
    }
    
    if ((fieldType = this->hasEventIn(id)) != VrmlField::NO_FIELD) {
        return fieldType;
    }
    
    if ((fieldType = this->hasEventOut(id)) != VrmlField::NO_FIELD) {
        return fieldType;
    }
    
    return fieldType;
}

VrmlField::VrmlFieldType NodeType::has(const FieldList & recs,
                                           const std::string & id) const {
    for (FieldList::const_iterator i(recs.begin()); i != recs.end(); ++i) {
        if ((*i)->name == id) {
            return (*i)->type;
        }
    }
    return VrmlField::NO_FIELD;
}

const VrmlField * NodeType::fieldDefault(const std::string & fname) const {
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
void NodeType::setUrl(const VrmlMFString & url, const Doc2 * relative) {
    assert(this->implNodes.getLength() == 0);
    
    delete this->d_url;
    this->d_url = new VrmlMFString(url);
    
    delete this->d_relative;
    this->d_relative = relative ? new Doc2(relative) : 0;
}

void NodeType::addNode(VrmlNode & node) {
    // add node to list of implementation nodes
    this->implNodes.addNode(node);
}

void NodeType::addIS(const std::string & isFieldName,
			 VrmlNode & implNode,
			 const std::string & implFieldName)
{
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


const NodeType::ISMap * NodeType::getFieldISMap(
                                                const std::string & fieldName)
        const {
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
