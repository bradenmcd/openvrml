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

#include <list>
#include "common.h"
#include "field.h"
#include "nodetypeptr.h"

class OPENVRML_SCOPE VrmlNamespace {
    static std::list<NodeTypePtr> builtInList;
    static int s_nNamespaces;

    // Defined node types (PROTOs) for this namespace
    std::list<NodeTypePtr> d_typeList;

    // Defined node names for this namespace
    std::list<VrmlNodePtr> d_nameList;

    // Parent namespace
    VrmlNamespace *d_parent;

public:
    VrmlNamespace(VrmlNamespace * parent = 0);
    ~VrmlNamespace();

    void addNodeType(const NodeTypePtr & nodeType);

    // DEFd nodes add node names to the namespace.
    // Node names are only defined in the current name space. They
    // are not available outside of the PROTO they are defined in,
    // nor are they available inside of nested PROTOs.

    void addNodeName(const VrmlNodePtr &);
    void removeNodeName(const VrmlNode &);

    const NodeTypePtr findType(const std::string & name) const;
    const NodeTypePtr findPROTO(const std::string & name) const;

    // Return the first node type in scope (default EXTERNPROTO implementation)
    const NodeTypePtr firstType() const;

    // Find a node by name.
    const VrmlNodePtr findNode(const std::string & name) const;

    const VrmlMFNode cloneNodes(const VrmlMFNode & mfnode);

private:
    void addBuiltIn(const NodeTypePtr & nodeType);
    void defineBuiltIns();
};

#endif
