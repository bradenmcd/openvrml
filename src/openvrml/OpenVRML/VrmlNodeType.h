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

#include <list>
#include "common.h"
#include "field.h"
#include "VrmlNodePtr.h"

class Doc2;
class VrmlNamespace;
class VrmlScene;

class OPENVRML_SCOPE VrmlNodeType {
public:
    struct NodeFieldRec {
        VrmlNodePtr node;
        char * fieldName;
    };
    typedef std::list<NodeFieldRec *> ISMap;
    struct ProtoField {
        char * name;
        VrmlField::VrmlFieldType type;
        VrmlField * defaultValue;
        ISMap thisIS;
    };
    typedef std::list<ProtoField *> FieldList;

private:
    int d_refCount;
    char * d_name;
    VrmlNamespace * d_namespace;
    const VrmlMFString * d_url; // Where the EXTERNPROTO could be.
    VrmlSFString * d_actualUrl;	// The URL actually used.
    Doc2 *d_relative;
    VrmlMFNode implNodes;
    // Pointer to function to create instances
    VrmlNode* (*d_creator)( VrmlScene* );	
    // Fields defined for this node type
    FieldList d_eventIns;
    FieldList d_eventOuts;
    FieldList d_fields;
    bool d_fieldsInitialized;

public:
    VrmlNodeType(const char * name,
	         VrmlNode * (*creator)(VrmlScene *) = 0);
    ~VrmlNodeType();

    VrmlNodeType * reference();
    void dereference();
    VrmlNode * newNode(VrmlScene * scene = 0) const;

    VrmlNamespace * getScope() const;
    void setScope(VrmlNamespace & scope);

    // Routines for adding/getting eventIns/Outs/fields to this type
    void addEventIn(const char *name, VrmlField::VrmlFieldType type);
    void addEventOut(const char *name, VrmlField::VrmlFieldType type);
    void addField(const char * id, VrmlField::VrmlFieldType type,
		  const VrmlField * defaultValue = 0);
    void addExposedField(const char * id, VrmlField::VrmlFieldType type,
		         const VrmlField * defaultValue = 0);

    void setFieldDefault(const char * name, const VrmlField * defaultValue);

    VrmlField::VrmlFieldType hasEventIn(const char *name) const;
    VrmlField::VrmlFieldType hasEventOut(const char *name) const;
    VrmlField::VrmlFieldType hasField(const char *name) const;
    VrmlField::VrmlFieldType hasExposedField(const char *name) const;
    VrmlField::VrmlFieldType hasInterface(char const *) const;

    VrmlField *fieldDefault(const char *name) const;

    void setUrl(const VrmlMFString & url, const Doc2 * relative = 0);
    void setActualUrl(const char *url);
    const char * getActualUrl() const;

    // Add a node to a PROTO implementation
    void addNode(VrmlNode & implNode);

    // Add an IS linkage to one of the PROTO interface fields/events.
    void addIS(const char *isFieldName,
	       VrmlNode & implNode,
	       const char *implFieldName);

    const char *getName() const { return d_name; }

    const VrmlMFNode & getImplementationNodes();

    const VrmlNodePtr firstNode() const;

    ISMap *getFieldISMap( const char *fieldName );

    FieldList &fields() { return d_fields; }
    FieldList &eventIns() { return d_eventIns; }
    FieldList &eventOuts() { return d_eventOuts; }

private:
    void fetchImplementation();
    VrmlField::VrmlFieldType has(const FieldList &, const char *) const;
};

#endif // VRMLNODETYPE_H
