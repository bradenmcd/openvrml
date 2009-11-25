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
# include "world_info.h"

namespace {

    class OPENVRML_LOCAL world_info_node :
        public openvrml::node_impl_util::abstract_node<world_info_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::world_info_metatype;

        openvrml::mfstring info;
        openvrml::sfstring title;

    public:
        world_info_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~world_info_node() OPENVRML_NOTHROW;
    };


    /**
     * @class world_info_node
     *
     * @brief WorldInfo node instances.
     */

    /**
     * @var class world_info_node::world_info_metatype
     *
     * @brief Class object for WorldInfo instances.
     */

    /**
     * @var openvrml::mfstring world_info_node::info
     *
     * @brief info field.
     */

    /**
     * @var openvrml::sfstring world_info_node::title
     *
     * @brief title field.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope that the new node will belong to.
     */
    world_info_node::
    world_info_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<world_info_node>(type, scope),
        child_node(type, scope)
    {}

    /**
     * @brief Destroy.
     */
    world_info_node::~world_info_node() OPENVRML_NOTHROW
    {}
}

 
/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::world_info_metatype::id =
    "urn:X-openvrml:node:WorldInfo";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::world_info_metatype::
world_info_metatype(openvrml::browser & browser):
    node_metatype(world_info_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::world_info_metatype::~world_info_metatype()
    OPENVRML_NOTHROW
{}

# define WORLD_INFO_INTERFACE_SEQ                    \
    ((field,        mfstring, "info",     info))     \
    ((field,        sfstring, "title",    title))    \
    ((exposedfield, sfnode,   "metadata", metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              world_info_metatype,
                                              world_info_node,
                                              WORLD_INFO_INTERFACE_SEQ)
