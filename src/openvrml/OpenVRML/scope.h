// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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
#   include <OpenVRML/node_type_ptr.h>
#   include <OpenVRML/scopeptr.h>

namespace OpenVRML {

    class OPENVRML_SCOPE Scope {
        friend class node;

        std::list<node_type_ptr> nodeTypeList;
        std::map<std::string, node *> namedNodeMap;

    public:
        const std::string id;
        const ScopePtr parent;

        explicit Scope(const std::string & id,
                       const ScopePtr & scope = ScopePtr());
        virtual ~Scope();

        void addNodeType(const node_type_ptr & nodeType);
        const node_type_ptr findType(const std::string & name) const;
        const node_type_ptr firstType() const;
        node * findNode(const std::string & name) const;

    private:
        // Not copyable.
        Scope(const Scope &);
        Scope & operator=(const Scope &);
    };
}

#endif
