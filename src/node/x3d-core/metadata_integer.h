// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifndef OPENVRML_NODE_X3D_CORE_METADATA_INTEGER_H
#   define OPENVRML_NODE_X3D_CORE_METADATA_INTEGER_H

#   include <openvrml/node.h>

namespace openvrml_node_x3d_core {

    /**
     * @brief Class object for MetadataInteger nodes.
     */
    class OPENVRML_LOCAL metadata_integer_metatype :
        public openvrml::node_metatype {
    public:
        static const char * const id;

        explicit metadata_integer_metatype(openvrml::browser & browser);
        virtual ~metadata_integer_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<openvrml::node_type>
        do_create_type(const std::string & id,
                       const openvrml::node_interface_set & interfaces) const
            OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);
    };
}

# endif // ifndef OPENVRML_NODE_X3D_CORE_METADATA_INTEGER_H
