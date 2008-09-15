// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating WorldInfo nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by world_info_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::world_info_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 3> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "info"),
        node_interface(node_interface::field_id,
                       field_value::sfstring_id,
                       "title"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<world_info_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & worldInfoNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            worldInfoNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &world_info_node::info);
        } else if (*interface_ == *++supported_interface) {
            worldInfoNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &world_info_node::title);
        } else if (*interface_ == *++supported_interface) {
            worldInfoNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &world_info_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
