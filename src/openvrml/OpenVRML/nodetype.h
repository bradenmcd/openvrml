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

# ifndef OPENVRML_NODETYPE_H
#   define OPENVRML_NODETYPE_H

#   include <list>
#   include "common.h"
#   include "field.h"
#   include "VrmlNodePtr.h"

namespace OpenVRML {

    class Doc2;
    class VrmlNamespace;
    class VrmlScene;

    class OPENVRML_SCOPE NodeType {
    public:
        struct NodeFieldRec {
            NodePtr node;
            std::string fieldName;
        };
        typedef std::list<NodeFieldRec *> ISMap;
        struct ProtoField {
            std::string name;
            FieldValue::FieldType type;
            FieldValue * defaultValue;
            ISMap thisIS;
        };
        typedef std::list<ProtoField *> FieldList;

    private:
        std::string id;
        VrmlNamespace * d_namespace;
        const MFString * d_url; // Where the EXTERNPROTO could be.
        mutable std::string actualUrl;	// The URL actually used.
        Doc2 *d_relative;
        mutable MFNode implNodes;
        // Pointer to function to create instances
        const NodePtr (*d_creator)(VrmlScene *const);	
        // Fields defined for this node type
        mutable FieldList d_eventIns;
        mutable FieldList d_eventOuts;
        mutable FieldList d_fields;
        mutable bool d_fieldsInitialized;

    public:
        NodeType(const std::string & id,
	         const NodePtr (*creator)(VrmlScene *const scene) = 0);
        ~NodeType();

        const NodePtr newNode(VrmlScene * scene = 0) const;

        const std::string & getId() const;
        VrmlNamespace * getScope() const;
        void setScope(VrmlNamespace & scope);

        // Routines for adding/getting eventIns/Outs/fields to this type
        void addEventIn(const std::string & name, FieldValue::FieldType type);
        void addEventOut(const std::string & name, FieldValue::FieldType type);
        void addField(const std::string & name, FieldValue::FieldType type,
		      const FieldValue * defaultVal = 0);
        void addExposedField(const std::string & name,
                             FieldValue::FieldType type,
		             const FieldValue * defaultVal = 0);

        const FieldValue * fieldDefault(const std::string & name) const;
        void setFieldDefault(const std::string & name, const FieldValue * value);

        FieldValue::FieldType hasEventIn(const std::string & id) const;
        FieldValue::FieldType hasEventOut(const std::string & id) const;
        FieldValue::FieldType hasField(const std::string & id) const;
        FieldValue::FieldType hasExposedField(const std::string & id) const;
        FieldValue::FieldType hasInterface(const std::string & id) const;

        void setUrl(const MFString & url, const Doc2 * relative = 0);
        void setActualUrl(const std::string & url);
        const std::string & getActualUrl() const;

        // Add a node to a PROTO implementation
        void addNode(Node & implNode);

        // Add an IS linkage to one of the PROTO interface fields/events.
        void addIS(const std::string & isFieldName, Node & implNode,
	           const std::string & implFieldName);


        const MFNode & getImplementationNodes() const;

        const NodePtr firstNode() const;

        const ISMap * getFieldISMap(const std::string & fieldName) const;

        const FieldList & fields() const { return d_fields; }
        const FieldList & eventIns() const { return d_eventIns; }
        const FieldList & eventOuts() const { return d_eventOuts; }

    private:
        void fetchImplementation() const;
        FieldValue::FieldType has(const FieldList &, const std::string & id) const;
    };
}

# endif
