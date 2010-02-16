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

# include "coordinate_double.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Represents CoordinateDouble node instances.
     */
    class OPENVRML_LOCAL coordinate_double_node :
        public abstract_node<coordinate_double_node> {

        friend class openvrml_node_x3d_nurbs::coordinate_double_metatype;

        exposedfield<mfvec3d> point_;

    public:
        coordinate_double_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~coordinate_double_node() OPENVRML_NOTHROW;
    };


    /**
     * @var coordinate_double_node::coordinate_double_metatype
     *
     * @brief Class object for CoordinateDouble nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<coordinate_double_node>::exposedfield<openvrml::mfvec3d> coordinate_double_node::point_
     *
     * @brief point exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    coordinate_double_node::
    coordinate_double_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        point_(*this)
    {}

    /**
     * @brief Destroy.
     */
    coordinate_double_node::~coordinate_double_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_nurbs::coordinate_double_metatype::id =
    "urn:X-openvrml:node:CoordinateDouble";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c coordinate_double_metatype.
 */
openvrml_node_x3d_nurbs::coordinate_double_metatype::
coordinate_double_metatype(openvrml::browser & browser):
    node_metatype(coordinate_double_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::coordinate_double_metatype::
~coordinate_double_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating CoordinateDouble nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c coordinate_double_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_nurbs::coordinate_double_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 2> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3d_id,
                       "point")
    };
    typedef node_type_impl<coordinate_double_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & the_node_type = static_cast<node_type_t &>(*type);

    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &coordinate_double_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &coordinate_double_node::point_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
