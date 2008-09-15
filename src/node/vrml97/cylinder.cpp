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
# include "cylinder.h"

namespace {

    class OPENVRML_LOCAL cylinder_node :
        public openvrml::node_impl_util::abstract_node<cylinder_node>,
        public openvrml::geometry_node {

        friend class openvrml_node_vrml97::cylinder_metatype;

        openvrml::sfbool bottom;
        openvrml::sffloat height;
        openvrml::sffloat radius;
        openvrml::sfbool side;
        openvrml::sfbool top;
        openvrml::sfbool solid_;

    public:
        cylinder_node(const openvrml::node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~cylinder_node() OPENVRML_NOTHROW;

    private:
        virtual openvrml::viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               openvrml::rendering_context context);
    };

    /**
     * @class cylinder_node
     *
     * @brief Cylinder node instances.
     */

    /**
     * @var class cylinder_node::cylinder_metatype
     *
     * @brief Class object for Cylinder nodes.
     */

    /**
     * @var openvrml::sfbool cylinder_node::bottom
     *
     * @brief bottom field.
     */

    /**
     * @var openvrml::sffloat cylinder_node::height
     *
     * @brief height field.
     */

    /**
     * @var openvrml::sffloat cylinder_node::radius
     *
     * @brief radius field.
     */

    /**
     * @var openvrml::sfbool cylinder_node::side
     *
     * @brief side field.
     */

    /**
     * @var openvrml::sfbool cylinder_node::top
     *
     * @brief top field.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    cylinder_node::
    cylinder_node(const openvrml::node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<cylinder_node>(type, scope),
        geometry_node(type, scope),
        bottom(true),
        height(2.0),
        radius(1.0),
        side(true),
        top(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    cylinder_node::~cylinder_node() OPENVRML_NOTHROW
    {
        // need access to viewer to remove d_viewerObject...
    }

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a viewer.
     * @param context   the rendering context.
     */
    openvrml::viewer::object_t
    cylinder_node::
    do_render_geometry(openvrml::viewer & viewer, openvrml::rendering_context)
    {
        const openvrml::viewer::object_t object_ref =
            viewer.insert_cylinder(this->height.value(),
                                   this->radius.value(),
                                   this->bottom.value(),
                                   this->side.value(),
                                   this->top.value());
        return object_ref;
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::cylinder_metatype::id =
    "urn:X-openvrml:node:Cylinder";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::cylinder_metatype::
cylinder_metatype(openvrml::browser & browser):
    node_metatype(cylinder_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::cylinder_metatype::~cylinder_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c boost::shared_ptr to a @c node_type capable of creating
 *         Cylinder nodes.
 *
 * @exception openvrml::unsupported_interface   if @p interfaces includes an
 *                                              interface not supported by
 *                                              @c cylinder_metatype.
 * @exception std::bad_alloc                    if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::cylinder_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 7> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "bottom"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "height"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "radius"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "side"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "top"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid")
    };

    typedef node_impl_util::node_type_impl<cylinder_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & cylinderNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_ = interfaces.begin();
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            cylinderNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &cylinder_node::bottom);
        } else if (*interface_ == *++supported_interface) {
            cylinderNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &cylinder_node::height);
        } else if (*interface_ == *++supported_interface) {
            cylinderNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &cylinder_node::radius);
        } else if (*interface_ == *++supported_interface) {
            cylinderNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &cylinder_node::side);
        } else if (*interface_ == *++supported_interface) {
            cylinderNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &cylinder_node::top);
        } else if (*interface_ == *++supported_interface) {
            cylinderNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &cylinder_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            cylinderNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &cylinder_node::solid_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
