//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2002  Braden McDaniel
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

# ifndef OPENVRML_SCOPE_H
#   define OPENVRML_SCOPE_H

#   include <list>
#   include <map>
#   include <string>
#   include "common.h"
#   include "nodetypeptr.h"
#   include "scopeptr.h"

namespace OpenVRML {

    class OPENVRML_SCOPE Scope {
        friend class Node;
        
        std::list<NodeTypePtr> nodeTypeList;
        std::map<std::string, Node *> namedNodeMap;
    
    public:
        const std::string id;
        const ScopePtr parent;
        
        explicit Scope(const std::string & id,
                       const ScopePtr & scope = ScopePtr());
        virtual ~Scope();

        void addNodeType(const NodeTypePtr & nodeType);
        const NodeTypePtr findType(const std::string & name) const;
        const NodeTypePtr firstType() const;
        Node * findNode(const std::string & name) const;
    
    private:
        // Not copyable.
        Scope(const Scope &);
        Scope & operator=(const Scope &);
    };
}

#endif
