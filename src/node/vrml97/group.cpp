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

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Group nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by group_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::group_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 6> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<group_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & groupNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            groupNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &group_node::add_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            groupNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &group_node::remove_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            groupNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &group_node::children_);
        } else if (*interface_ == *++supported_interface) {
            groupNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &group_node::bbox_center_);
        } else if (*interface_ == *++supported_interface) {
            groupNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &group_node::bbox_size_);
        } else if (*interface_ == *++supported_interface) {
            groupNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &group_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
