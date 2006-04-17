// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2002, 2005  Braden McDaniel
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

#   include <openvrml-common.h>
#   include <list>
#   include <map>
#   include <string>
#   include <boost/shared_ptr.hpp>
#   include <boost/utility.hpp>

namespace openvrml {

    class node;
    class node_type;

    class OPENVRML_API scope : boost::noncopyable {
        friend class node;

        std::list<boost::shared_ptr<node_type> > node_type_list;
        std::map<std::string, node *> named_node_map;
        const std::string id_;
        const boost::shared_ptr<const scope> parent_;

    public:
        explicit scope(const std::string & id,
                       const boost::shared_ptr<const scope> & parent =
                       boost::shared_ptr<scope>());

        const std::string & id() const OPENVRML_NOTHROW;

        const boost::shared_ptr<const scope> & parent() const OPENVRML_NOTHROW;

        const std::pair<boost::shared_ptr<node_type>, bool>
            add_type(const boost::shared_ptr<node_type> & type)
            OPENVRML_THROW1(std::bad_alloc);

        const boost::shared_ptr<node_type> &
        find_type(const std::string & id) const;

        const boost::shared_ptr<node_type> & first_type() const;

        node * find_node(const std::string & id) const;
    };

    const std::string path(const scope & s) OPENVRML_THROW1(std::bad_alloc);
}

#endif
