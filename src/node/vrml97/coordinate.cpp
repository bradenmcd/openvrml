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
# include "coordinate.h"

namespace {

    class coordinate_node :
        public openvrml::node_impl_util::abstract_node<coordinate_node>,
        public openvrml::coordinate_node {

        friend class openvrml_node_vrml97::coordinate_metatype;

        exposedfield<openvrml::mfvec3f> point_;

    public:
        coordinate_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~coordinate_node() OPENVRML_NOTHROW;

    private:
        //
        // coordinate_node implementation
        //
        virtual const std::vector<openvrml::vec3f> & do_point() const
            OPENVRML_NOTHROW;
    };

    /**
     * @class coordinate_node
     *
     * @brief Coordinate node instances.
     */

    /**
     * @var class coordinate_node::coordinate_metatype
     *
     * @brief Class object for Coordinate nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<coordinate_node>::exposedfield<openvrml::mfvec3f> coordinate_node::point_
     *
     * @brief point exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope     the scope to which the node belongs.
     */
    coordinate_node::
    coordinate_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml::node_impl_util::abstract_node<coordinate_node>(type, scope),
        openvrml::coordinate_node(type, scope),
        point_(*this)
    {}

    /**
     * @brief Destroy.
     */
    coordinate_node::~coordinate_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the points encapsulated by this node.
     *
     * @return the array of points for this node.
     */
    const std::vector<openvrml::vec3f> & coordinate_node::do_point() const
        OPENVRML_NOTHROW
    {
        return this->point_.mfvec3f::value();
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::coordinate_metatype::id =
    "urn:X-openvrml:node:Coordinate";

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml_node_vrml97::coordinate_metatype::
coordinate_metatype(openvrml::browser & browser):
    node_metatype(coordinate_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::coordinate_metatype::~coordinate_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c boost::shared_ptr to a @c node_type capable of creating
 *         Coordinate nodes.
 *
 * @exception openvrml::unsupported_interface   if @p interfaces includes an
 *                                              interface not supported by
 *                                              @c coordinate_metatype.
 * @exception std::bad_alloc                    if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::coordinate_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using openvrml::field_value;
    using openvrml::sfnode;
    using openvrml::mfvec3f;
    using openvrml::node_interface;
    using openvrml::node_interface_set;
    using openvrml::node_type;
    using openvrml::unsupported_interface;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 2> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "point"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef openvrml::node_impl_util::node_type_impl<coordinate_node>
        node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & coordinateNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            coordinateNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &coordinate_node::point_);
        } else if (*interface_ == *++supported_interface) {
            coordinateNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &coordinate_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
