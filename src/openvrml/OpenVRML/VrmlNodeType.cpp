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

#include "VrmlNodeType.h"
#include "VrmlNode.h"
#include "VrmlNamespace.h"

#include "doc2.hpp"
#include "VrmlNodeProto.h"	// Instances of PROTOtyped nodes
#include "VrmlScene.h"

#include <stdio.h>
#include <string.h>

#ifdef macintosh
extern char* strdup( const char* );
#endif

/**
 * @class VrmlNodeType
 *
 * @brief The VrmlNodeType class is responsible for storing information
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
VrmlNodeType::VrmlNodeType(const char * name,
                           VrmlNode * (*creator)(VrmlScene *)):
        d_refCount(0), d_name(strdup(name ? name : "<unnamed type>")),
        d_namespace(0), d_url(0), d_actualUrl(0), d_relative(0),
        d_creator(creator), d_fieldsInitialized(false) {}

namespace {
    void destructFieldList( VrmlNodeType::FieldList &f )
    {
      VrmlNodeType::FieldList::iterator i;
      for (i = f.begin(); i != f.end(); ++i) {
        VrmlNodeType::ProtoField *r = *i;
        free(r->name);
        if (r->defaultValue) delete r->defaultValue;

        // free NodeFieldRec* s in r->thisIS;
        VrmlNodeType::ISMap::const_iterator j;
        for (j = r->thisIS.begin(); j != r->thisIS.end(); ++j)
          {
	    VrmlNodeType::NodeFieldRec *nf = *j;
	    free(nf->fieldName);
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
VrmlNodeType::~VrmlNodeType()
{
  free(d_name);

  delete d_namespace;
  delete d_url;
  delete d_actualUrl;
  delete d_relative;

  // Free strings & defaults duplicated when fields/eventIns/eventOuts added:
  destructFieldList( d_eventIns );
  destructFieldList( d_eventOuts );
  destructFieldList( d_fields );
}

/**
 * @brief Add to the refcount.
 *
 * @return a pointer to this object.
 */
VrmlNodeType * VrmlNodeType::reference() {
    ++this->d_refCount;
    return this;
}

/**
 * @brief Subtract from the refcount;
 */
void VrmlNodeType::dereference() {
    if (--this->d_refCount == 0) {
        delete this;
    }
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
VrmlNode * VrmlNodeType::newNode(VrmlScene * scene) const
{
  if (d_creator)
    return (*d_creator)( scene );

  return new VrmlNodeProto( (VrmlNodeType *)this, scene );
}

/**
 * @brief Get the scope of this type.
 *
 * @return a pointer to the VrmlNamespace corresponding to this node type's
 *      scope, or 0 for built-in node types.
 */
VrmlNamespace * VrmlNodeType::getScope() const {
    return this->d_namespace;
}

/**
 * @brief Set the scope of this type.
 *
 * @param scope
 */
void VrmlNodeType::setScope(VrmlNamespace & scope)
{
  d_namespace = new VrmlNamespace(&scope);
}

/**
 * @brief Set the URL from which an EXTERNPROTO's implementation was
 *      successfully retrieved.
 *
 * @param url
 */
void VrmlNodeType::setActualUrl(const char *url)
{
  if (d_actualUrl) delete d_actualUrl;
  d_actualUrl = url ? new VrmlSFString(url): 0;
}

/**
 * @brief Retrieve the actual URL the PROTO was retrieved from.
 */
const char * VrmlNodeType::getActualUrl() const {
    return d_actualUrl ? d_actualUrl->get() : 0;
}

namespace {
    //
    // Helper method to add a field or event.
    //
    void add(VrmlNodeType::FieldList & recs, const char * id, VrmlField::VrmlFieldType type) {
        VrmlNodeType::ProtoField * const protoField =
                new VrmlNodeType::ProtoField;
        protoField->name = strdup(id);
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
void VrmlNodeType::addEventIn(const char * id, VrmlField::VrmlFieldType type) {
    add(d_eventIns, id, type);
}

/**
 * @brief Add an eventOut to this type.
 *
 * @param id
 * @param type
 */
void VrmlNodeType::addEventOut(const char * id, VrmlField::VrmlFieldType type) {
    add(d_eventOuts, id, type);
}

/**
 * @brief Add a field to this type.
 *
 * @param id
 * @param type
 * @param defaultValue
 */
void VrmlNodeType::addField(const char * id, VrmlField::VrmlFieldType type,
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
void VrmlNodeType::addExposedField(const char * id,
                                   VrmlField::VrmlFieldType type,
                                   const VrmlField * defaultValue) {
    char tmp[1000];
    add(d_fields, id, type);
    if (defaultValue) {
        this->setFieldDefault(id, defaultValue);
    }

    sprintf(tmp, "set_%s", id);
    add(d_eventIns, tmp, type);
    sprintf(tmp, "%s_changed", id);
    add(d_eventOuts, tmp, type);
}

/**
 * @brief Set the default value for a field or exposedField.
 *
 * @param fname field name
 * @param defaultValue default value
 */
void VrmlNodeType::setFieldDefault(const char * fname,
                                   const VrmlField * defaultValue) {
  FieldList::const_iterator i;

  for (i = d_fields.begin(); i != d_fields.end(); ++i)
    if (strcmp((*i)->name, fname) == 0)
      {
	if ((*i)->defaultValue)
	  {
	    theSystem->error("Default for field %s of %s already set.",
			     fname, getName());
	    delete (*i)->defaultValue;
	  }
	(*i)->defaultValue = defaultValue ? defaultValue->clone() : 0;
	return;
      }

  theSystem->error("setFieldDefault for field %s of %s failed: no such field.",
		   fname, getName());
}


/**
 * @brief Download the EXTERNPROTO definition.
 */
void VrmlNodeType::fetchImplementation()
{
  // Get the PROTO def from the url (relative to original scene url).
  VrmlNodeType * const proto =
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

      setActualUrl(proto->getActualUrl());

      delete proto;
    }
  else
    {
      theSystem->warn("Couldn't read EXTERNPROTO %s\n", getName());
    }
}

/**
 * @brief Get the implementation nodes; download them if necessary.
 *
 * @return the implementation nodes
 */
const VrmlMFNode & VrmlNodeType::getImplementationNodes() {
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
	      if (strcmp(n->name(),"") == 0)
		{
		  sprintf(buf,"#%lx", (unsigned long) n);
		  n->setName( buf );
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
	      if (strcmp(n->name(),"") == 0)
		{
		  sprintf(buf,"#%lx", (unsigned long) n);
		  n->setName( buf );
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
	      if (strcmp(n->name(),"") == 0)
		{
		  sprintf(buf,"#%lx", (unsigned long) n);
		  n->setName( buf );
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
const VrmlNodePtr VrmlNodeType::firstNode() const {
    return (this->implNodes.getLength() > 0)
            ? this->implNodes.getElement(0)
            : VrmlNodePtr(0);
}
  
VrmlField::VrmlFieldType
VrmlNodeType::hasEventIn(const char *ename) const
{
  return has(d_eventIns, ename);
}

VrmlField::VrmlFieldType
VrmlNodeType::hasEventOut(const char *ename) const
{
  return has(d_eventOuts, ename);
}

VrmlField::VrmlFieldType
VrmlNodeType::hasField(const char *ename) const
{
  return has(d_fields, ename);
}

VrmlField::VrmlFieldType
VrmlNodeType::hasExposedField(const char *ename) const
{
  // Must have field "name", eventIn "set_name", and eventOut
  // "name_changed", all with same type:
  char tmp[1000];
  VrmlField::VrmlFieldType type;

  if ( (type = has(d_fields, ename)) == VrmlField::NO_FIELD)
    return VrmlField::NO_FIELD;

  sprintf(tmp, "set_%s", ename);
  if (type != has(d_eventIns, tmp)) return VrmlField::NO_FIELD;

  sprintf(tmp, "%s_changed", ename);
  if (type != has(d_eventOuts, tmp)) return VrmlField::NO_FIELD;

  return type;
}

VrmlField::VrmlFieldType VrmlNodeType::hasInterface(char const * id) const
{
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

VrmlField::VrmlFieldType
VrmlNodeType::has(const FieldList &recs, const char *ename) const
{
  FieldList::const_iterator i;
  for (i = recs.begin(); i != recs.end(); ++i) {
    if (strcmp((*i)->name, ename) == 0)
      return (*i)->type;
  }
  return VrmlField::NO_FIELD;
}

VrmlField*
VrmlNodeType::fieldDefault(const char *fname) const
{
  FieldList::const_iterator i;
  for (i = d_fields.begin(); i != d_fields.end(); ++i) {
    if (strcmp((*i)->name, fname) == 0)
      return (*i)->defaultValue;
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
void VrmlNodeType::setUrl(const VrmlMFString & url, const Doc2 * relative) {
    assert(this->implNodes.getLength() == 0);
    
    delete this->d_url;
    this->d_url = new VrmlMFString(url);
    
    delete this->d_relative;
    this->d_relative = relative ? new Doc2(relative) : 0;
}

void VrmlNodeType::addNode(VrmlNode & node) {
    // add node to list of implementation nodes
    this->implNodes.addNode(node);
}

void VrmlNodeType::addIS(const char *isFieldName,
			 VrmlNode & implNode,
			 const char *implFieldName)
{
  FieldList::iterator i;

  theSystem->debug("%s::addIS(%s, %s::%s.%s)\n",
		   getName(),
		   isFieldName,
		   implNode.nodeType().getName(),
		   implNode.name(),
		   implFieldName);

  // Fields
  for (i = d_fields.begin(); i != d_fields.end(); ++i)
    {
      if (strcmp((*i)->name, isFieldName) == 0)
	{
	  NodeFieldRec *nf = new NodeFieldRec;
	  nf->node.reset(&implNode);
	  nf->fieldName = strdup(implFieldName);
	  (*i)->thisIS.push_front(nf);
	  break;
	}
    }

  // EventIns
  for (i = d_eventIns.begin(); i != d_eventIns.end(); ++i)
    {
      if (strcmp((*i)->name, isFieldName) == 0)
	{
	  NodeFieldRec *nf = new NodeFieldRec;
	  nf->node.reset(&implNode);
	  nf->fieldName = strdup(implFieldName);
	  (*i)->thisIS.push_front(nf);
	  break;
	}
    }

  // EventOuts
  for (i = d_eventOuts.begin(); i != d_eventOuts.end(); ++i)
    {
      if (strcmp((*i)->name, isFieldName) == 0)
	{
	  NodeFieldRec *nf = new NodeFieldRec;
	  nf->node.reset(&implNode);
	  nf->fieldName = strdup(implFieldName);
	  (*i)->thisIS.push_front(nf);
	  break;
	}
    }

}


VrmlNodeType::ISMap *VrmlNodeType::getFieldISMap( const char *fieldName )
{
  FieldList::iterator i;
  for (i = d_fields.begin(); i != d_fields.end(); ++i)
    if (strcmp((*i)->name, fieldName) == 0)
      return & ((*i)->thisIS);
  return 0;
}
