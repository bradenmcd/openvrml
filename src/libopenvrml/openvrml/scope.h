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
#   include <openvrml/node_type_ptr.h>
#   include <openvrml/scope_ptr.h>

namespace openvrml {

    class scope {
        friend class node;

        std::list<node_type_ptr> node_type_list;
        std::map<std::string, node *> named_node_map;

    public:
        const std::string id;
        const scope_ptr parent;

        explicit scope(const std::string & id,
                       const scope_ptr & parent = scope_ptr());
        virtual ~scope();

        bool add_type(const node_type_ptr & type) throw (std::bad_alloc);
        const node_type_ptr & find_type(const std::string & id) const;
        const node_type_ptr & first_type() const;
        node * find_node(const std::string & id) const;

    private:
        // Not copyable.
        scope(const scope &);
        scope & operator=(const scope &);
    };
}

#endif
