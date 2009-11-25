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
# include <private.h>
# include "grouping_node_base.h"
# include "group.h"

namespace {

    class OPENVRML_LOCAL group_node :
        public openvrml_node_vrml97::grouping_node_base<group_node> {

        friend class openvrml_node_vrml97::group_metatype;

    public:
        group_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~group_node() OPENVRML_NOTHROW;
    };

    /**
     * @class group_node
     *
     * @brief Represents Group node instances.
     */

    /**
     * @var class group_node::group_metatype
     *
     * @brief Class object for Group nodes.
     */

    /**
     * @brief Construct.
     */
    group_node::group_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<group_node>(type, scope)
    {}

    /**
     * @brief Destroy.
     */
    group_node::~group_node() OPENVRML_NOTHROW
    {}
}

 
/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::group_metatype::id =
    "urn:X-openvrml:node:Group";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::group_metatype::
group_metatype(openvrml::browser & browser):
    node_metatype(group_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::group_metatype::~group_metatype() OPENVRML_NOTHROW
{}

# define GROUP_INTERFACE_SEQ                                            \
    ((eventin,      mfnode,  "addChildren",    add_children_listener_)) \
    ((eventin,      mfnode,  "removeChildren", remove_children_listener_)) \
    ((exposedfield, mfnode,  "children",       children_))              \
    ((field,        sfvec3f, "bboxCenter",     bbox_center_))           \
    ((field,        sfvec3f, "bboxSize",       bbox_size_))             \
    ((exposedfield, sfnode,  "metadata",       metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              group_metatype,
                                              group_node,
                                              GROUP_INTERFACE_SEQ)
