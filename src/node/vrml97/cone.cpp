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
# include "cone.h"

namespace {

    class OPENVRML_LOCAL cone_node :
        public openvrml::node_impl_util::abstract_node<cone_node>,
        public openvrml::geometry_node {

        friend class openvrml_node_vrml97::cone_metatype;

        openvrml::sfbool bottom;
        openvrml::sffloat bottomRadius;
        openvrml::sffloat height;
        openvrml::sfbool side;
        openvrml::sfbool solid_;

    public:
        cone_node(const openvrml::node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~cone_node() OPENVRML_NOTHROW;

    private:
        virtual openvrml::viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               openvrml::rendering_context context);
    };

    /**
     * @class cone_node
     *
     * @brief Cone node instances.
     */

    /**
     * @var class cone_node::cone_metatype
     *
     * @brief Class object for Cone nodes.
     */

    /**
     * @var openvrml::sfbool cone_node::bottom
     *
     * @brief bottom field.
     */

    /**
     * @var openvrml::sffloat cone_node::bottomRadius
     *
     * @brief bottomRadius field.
     */

    /**
     * @var openvrml::sffloat cone_node::height
     *
     * @brief height field.
     */

    /**
     * @var openvrml::sfbool cone_node::side
     *
     * @brief side field.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope     the scope to which the node belongs.
     */
    cone_node::
    cone_node(const openvrml::node_type & type,
              const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<cone_node>(type, scope),
        geometry_node(type, scope),
        bottom(true),
        bottomRadius(1.0),
        height(2.0),
        side(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    cone_node::~cone_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a viewer.
     * @param context   the rendering context.
     */
    openvrml::viewer::object_t
    cone_node::
    do_render_geometry(openvrml::viewer & viewer, openvrml::rendering_context)
    {
        const openvrml::viewer::object_t object_ref =
            viewer.insert_cone(this->height.value(),
                               this->bottomRadius.value(),
                               this->bottom.value(),
                               this->side.value());
        return object_ref;
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::cone_metatype::id =
    "urn:X-openvrml:node:Cone";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::cone_metatype::
cone_metatype(openvrml::browser & browser):
    node_metatype(cone_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::cone_metatype::~cone_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Cone nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by cone_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::cone_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 6> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "bottomRadius"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "height"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "side"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "bottom"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid")
    };

    typedef node_impl_util::node_type_impl<cone_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & coneNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            coneNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &cone_node::bottomRadius)));
        } else if (*interface == *++supported_interface) {
            coneNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &cone_node::height)));
        } else if (*interface == *++supported_interface) {
            coneNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &cone_node::side)));
        } else if (*interface == *++supported_interface) {
            coneNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &cone_node::bottom)));
        } else if (*interface == *++supported_interface) {
            coneNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<cone_node>::exposedfield<sfnode> >(
                        &cone_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<cone_node>::exposedfield<sfnode> >(
                        &cone_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<cone_node>::exposedfield<sfnode> >(
                        &cone_node::metadata)));
        } else if (*interface == *++supported_interface) {
            coneNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &cone_node::solid_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}
