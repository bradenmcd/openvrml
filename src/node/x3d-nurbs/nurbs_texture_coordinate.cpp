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

# include "nurbs_texture_coordinate.h"
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
     * @brief Represents NurbsTextureCoordinate node instances.
     */
    class OPENVRML_LOCAL nurbs_texture_coordinate_node :
        public abstract_node<nurbs_texture_coordinate_node> {

        friend class openvrml_node_x3d_nurbs::nurbs_texture_coordinate_metatype;

        exposedfield<mfvec2f> control_point_;
        exposedfield<mffloat> weight_;
        sfint32 u_dimension_;
        mfdouble u_knot_;
        sfint32 u_order_;
        sfint32 v_dimension_;
        mfdouble v_knot_;
        sfint32 v_order_;

    public:
        nurbs_texture_coordinate_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_texture_coordinate_node() OPENVRML_NOTHROW;
    };


    /**
     * @var nurbs_texture_coordinate_node::nurbs_texture_coordinate_metatype
     *
     * @brief Class object for NurbsTextureCoordinate nodes.
     */

    /**
     * @var nurbs_texture_coordinate_node::control_point_
     *
     * @brief control_point exposedField
     */

    /**
     * @var nurbs_texture_coordinate_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_texture_coordinate_node::u_dimension_
     *
     * @brief u_dimension field
     */

    /**
     * @var nurbs_texture_coordinate_node::u_knot_
     *
     * @brief u_knot field
     */

    /**
     * @var nurbs_texture_coordinate_node::u_order_
     *
     * @brief u_order field
     */

    /**
     * @var nurbs_texture_coordinate_node::v_dimension_
     *
     * @brief v_dimension field
     */

    /**
     * @var nurbs_texture_coordinate_node::v_knot_
     *
     * @brief v_knot field
     */

    /**
     * @var nurbs_texture_coordinate_node::v_order_
     *
     * @brief v_order field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    nurbs_texture_coordinate_node::
    nurbs_texture_coordinate_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        control_point_(*this),
        weight_(*this),
        u_order_(3),
        v_order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_texture_coordinate_node::~nurbs_texture_coordinate_node()
        OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_nurbs::nurbs_texture_coordinate_metatype::id =
    "urn:X-openvrml:node:NurbsTextureCoordinate";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c nurbs_texture_coordinate_metatype.
 */
openvrml_node_x3d_nurbs::nurbs_texture_coordinate_metatype::
nurbs_texture_coordinate_metatype(openvrml::browser & browser):
    node_metatype(nurbs_texture_coordinate_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::nurbs_texture_coordinate_metatype::
~nurbs_texture_coordinate_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating NurbsTextureCoordinate nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c nurbs_texture_coordinate_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_nurbs::nurbs_texture_coordinate_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 9> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec2f_id,
                       "controlPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "weight"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "uKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uOrder"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "vKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vOrder")
    };
    typedef node_type_impl<nurbs_texture_coordinate_node> node_type_t;

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
                &nurbs_texture_coordinate_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_texture_coordinate_node::control_point_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_texture_coordinate_node::weight_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_texture_coordinate_node::u_dimension_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_texture_coordinate_node::u_knot_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_texture_coordinate_node::u_order_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_texture_coordinate_node::v_dimension_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_texture_coordinate_node::v_knot_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_texture_coordinate_node::v_order_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
