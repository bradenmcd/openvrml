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

#ifndef VRMLNODETYPE_H
#define VRMLNODETYPE_H

//
// The VrmlNodeType class is responsible for storing information about node
// or prototype types.
//

#include <list>

#include "VrmlNodePtr.h"
#include "VrmlField.h"
#include "VrmlMFNode.h"
#include "VrmlSFString.h"

class Doc2;
class VrmlNamespace;
class VrmlNode;
class VrmlScene;


class VrmlNodeType {
public:
  // Constructor.  Takes name of new type (e.g. "Transform" or "Box")
  // Copies the string given as name.
  VrmlNodeType(const char *nm,
	       VrmlNode* (*creator)(VrmlScene *scene) = 0);

  // Deallocate storage for name and PROTO implementations
  ~VrmlNodeType();

  VrmlNodeType *reference() { ++d_refCount; return this; }
  void dereference() { if (--d_refCount==0) delete this; }

  // Create a node of this type in the specified scene
  VrmlNode *newNode( VrmlScene* s = 0 ) const;

  // Set/get scope of this type (namespace is NULL for builtins).
  VrmlNamespace *scope() { return d_namespace; }
  void setScope(VrmlNamespace &);

  // Routines for adding/getting eventIns/Outs/fields to this type
  void addEventIn(const char *name, VrmlField::VrmlFieldType type);
  void addEventOut(const char *name, VrmlField::VrmlFieldType type);
  void addField(const char *name, VrmlField::VrmlFieldType type,
		VrmlField const * defaultVal = 0);
  void addExposedField(const char *name, VrmlField::VrmlFieldType type,
		       VrmlField const * defaultVal = 0);

  void setFieldDefault(const char *name, VrmlField const * value);

  VrmlField::VrmlFieldType hasEventIn(const char *name) const;
  VrmlField::VrmlFieldType hasEventOut(const char *name) const;
  VrmlField::VrmlFieldType hasField(const char *name) const;
  VrmlField::VrmlFieldType hasExposedField(const char *name) const;
  VrmlField::VrmlFieldType hasInterface(char const *) const;

  VrmlField *fieldDefault(const char *name) const;

  // Set the URL to retrieve an EXTERNPROTO implementation from.
  void setUrl(VrmlMFString * url, Doc2 const * relative = 0);

  void setActualUrl(const char *url);

  // Retrieve the actual URL the PROTO was retrieved from.
  const char *url() { return d_actualUrl ? d_actualUrl->get() : 0; }

  // Add a node to a PROTO implementation
  void addNode(VrmlNode & implNode);
  
  // Add an IS linkage to one of the PROTO interface fields/events.
  void addIS(const char *isFieldName,
	     VrmlNode & implNode,
	     const char *implFieldName);

  const char *getName() const { return d_name; }

  const VrmlMFNode & getImplementationNodes();

  const VrmlNodePtr firstNode() const;

  struct NodeFieldRec {
    VrmlNodePtr node;
    char *fieldName;
  };

  typedef std::list<NodeFieldRec*> ISMap;

  struct ProtoField {
    char *name;
    VrmlField::VrmlFieldType type;
    VrmlField *defaultValue;
    ISMap thisIS;
  };

  typedef std::list<ProtoField*> FieldList;

  ISMap *getFieldISMap( const char *fieldName );

  FieldList &eventIns() { return d_eventIns; }
  FieldList &eventOuts() { return d_eventOuts; }


private:

  // Grab the implementation of an EXTERNPROTO
  void fetchImplementation();

  void add(FieldList &, const char *, VrmlField::VrmlFieldType);
  VrmlField::VrmlFieldType has(const FieldList &, const char *) const;

  int d_refCount;

  char *d_name;

  VrmlNamespace *d_namespace;

  VrmlMFString *d_url;		// Where the EXTERNPROTO could be.
  VrmlSFString *d_actualUrl;	// The URL actually used.
  Doc2 *d_relative;

  VrmlMFNode implNodes;

  // Pointer to function to create instances
  VrmlNode* (*d_creator)( VrmlScene* );	

  // Fields defined for this node type
  FieldList d_eventIns;
  FieldList d_eventOuts;
  FieldList d_fields;

  bool d_fieldsInitialized;

};
#endif // VRMLNODETYPE_H
