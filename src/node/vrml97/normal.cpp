// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "normal.h"

namespace {

    class OPENVRML_LOCAL normal_node :
        public openvrml::node_impl_util::abstract_node<normal_node>,
        public openvrml::normal_node {

        friend class openvrml_node_vrml97::normal_metatype;

        exposedfield<openvrml::mfvec3f> vector_;

    public:
        normal_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~normal_node() OPENVRML_NOTHROW;

    private:
        //
        // normal_node implementation.
        //
        virtual const std::vector<openvrml::vec3f> & do_vector() const
            OPENVRML_NOTHROW;
    };

    /**
     * @class normal_node
     *
     * @brief Normal node instances.
     */

    /**
     * @var class normal_node::normal_metatype
     *
     * @brief Class object for Normal nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<normal_node>::exposedfield<openvrml::mfvec3f> normal_node::vector_
     *
     * @brief vector exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope     the scope to which the node belongs.
     */
    normal_node::
    normal_node(const openvrml::node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml::node_impl_util::abstract_node<normal_node>(type, scope),
        openvrml::normal_node(type, scope),
        vector_(*this)
    {}

    /**
     * @brief Destroy.
     */
    normal_node::~normal_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the array of normal vectors.
     *
     * @return the array of normal vectors.
     */
    const std::vector<openvrml::vec3f> & normal_node::do_vector() const
        OPENVRML_NOTHROW
    {
        return this->vector_.mfvec3f::value();
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::normal_metatype::id =
    "urn:X-openvrml:node:Normal";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::normal_metatype::
normal_metatype(openvrml::browser & browser):
    node_metatype(normal_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::normal_metatype::~normal_metatype() OPENVRML_NOTHROW
{}

# define NORMAL_INTERFACE_SEQ                           \
    ((exposedfield, mfvec3f, "vector",   vector_))      \
    ((exposedfield, sfnode,  "metadata", metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              normal_metatype,
                                              normal_node,
                                              NORMAL_INTERFACE_SEQ)
