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

#ifndef VRMLNAMESPACE_H
#define VRMLNAMESPACE_H

# include <list>
# include <map>
# include "common.h"
# include "field.h"
# include "nodeclassptr.h"
# include "nodetypeptr.h"

namespace OpenVRML {

    class OPENVRML_SCOPE VrmlNamespace {
        std::list<NodeTypePtr> typeList;
        std::list<Node *> nameList;
    
    public:
        VrmlNamespace * const parent;
        
        VrmlNamespace(VrmlNamespace * parent = 0);
        virtual ~VrmlNamespace();

        void addNodeType(const NodeTypePtr & nodeType);

        // DEFd nodes add node names to the namespace.
        // Node names are only defined in the current name space. They
        // are not available outside of the PROTO they are defined in,
        // nor are they available inside of nested PROTOs.

        void addNodeName(Node & namedNode);
        void removeNodeName(Node & namedNode);

        const NodeTypePtr findType(const std::string & name) const;

        // Return the first node type in scope (default EXTERNPROTO implementation)
        const NodeTypePtr firstType() const;

        // Find a node by name.
        Node * findNode(const std::string & name) const;
    
    private:
        // Not copyable.
        VrmlNamespace(const VrmlNamespace &);
        VrmlNamespace & operator=(const VrmlNamespace &);
    };
    
    typedef std::map<std::string, NodeClassPtr> NodeClassMap;
        
    class OPENVRML_SCOPE Vrml97RootNamespace : public VrmlNamespace {
    public:
        Vrml97RootNamespace(const NodeClassMap & nodeClassMap);
        virtual ~Vrml97RootNamespace();
    };
}

#endif
