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

# include "line_properties.h"
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
     * @brief Represents LineProperties node instances.
     */
    class OPENVRML_LOCAL line_properties_node :
        public abstract_node<line_properties_node> {

        friend class openvrml_node_x3d_shape::line_properties_metatype;

        exposedfield<sfbool> applied_;
        exposedfield<sfint32> line_type_;
        exposedfield<sffloat> linewidth_scale_factor_;

    public:
        line_properties_node(const node_type & type, 
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~line_properties_node() OPENVRML_NOTHROW;
    };


    /**
     * @var line_properties_node::line_properties_metatype
     *
     * @brief Class object for LineProperties nodes.
     */

    /**
     * @var line_properties_node::applied_
     *
     * @brief applied exposedField
     */

    /**
     * @var line_properties_node::line_type_
     *
     * @brief line_type exposedField
     */

    /**
     * @var line_properties_node::linewidth_scale_factor_
     *
     * @brief linewidth_scale_factor exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    line_properties_node::
    line_properties_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        applied_(*this, true),
        line_type_(*this, 1),
        linewidth_scale_factor_(*this)
    {}

    /**
     * @brief Destroy.
     */
    line_properties_node::~line_properties_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_shape::line_properties_metatype::id =
    "urn:X-openvrml:node:LineProperties";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c line_properties_metatype.
 */
openvrml_node_x3d_shape::line_properties_metatype::
line_properties_metatype(openvrml::browser & browser):
    node_metatype(line_properties_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_shape::line_properties_metatype::~line_properties_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the @c new node_type.
 *
 * @return a @c boost::shared_ptr to a @c node_type capable of creating
 *         LineProperties nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c line_properties_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_shape::line_properties_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 4> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "applied"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "lineType"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "linewidthScaleFactor")
    };
    typedef node_type_impl<line_properties_node> node_type_t;

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
                &line_properties_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &line_properties_node::applied_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &line_properties_node::line_type_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &line_properties_node::linewidth_scale_factor_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
