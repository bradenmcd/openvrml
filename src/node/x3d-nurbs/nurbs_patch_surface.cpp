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

# include "nurbs_patch_surface.h"
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
     * @brief Represents NurbsPatchSurface node instances.
     */
    class OPENVRML_LOCAL nurbs_patch_surface_node :
        public abstract_node<nurbs_patch_surface_node>,
        public geometry_node,
        public child_node {

        friend class openvrml_node_x3d_nurbs::nurbs_patch_surface_metatype;

        exposedfield<sfnode> control_point_;
        exposedfield<sfnode> tex_coord_;
        exposedfield<sfint32> u_tessellation_;
        exposedfield<sfint32> v_tessellation_;
        exposedfield<mfdouble> weight_;
        sfbool solid_;
        sfbool u_closed_;
        sfint32 u_dimension_;
        mfdouble u_knot_;
        sfint32 u_order_;
        sfbool v_closed_;
        sfint32 v_dimension_;
        mfdouble v_knot_;
        sfint32 v_order_;

    public:
        nurbs_patch_surface_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_patch_surface_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };


    /**
     * @var nurbs_patch_surface_node::nurbs_patch_surface_metatype
     *
     * @brief Class object for NurbsPatchSurface nodes.
     */

    /**
     * @var nurbs_patch_surface_node::control_point_
     *
     * @brief control_point exposedField
     */

    /**
     * @var nurbs_patch_surface_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var nurbs_patch_surface_node::u_tessellation_
     *
     * @brief u_tessellation exposedField
     */

    /**
     * @var nurbs_patch_surface_node::v_tessellation_
     *
     * @brief v_tessellation exposedField
     */

    /**
     * @var nurbs_patch_surface_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_patch_surface_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var nurbs_patch_surface_node::u_closed_
     *
     * @brief u_closed field
     */

    /**
     * @var nurbs_patch_surface_node::u_dimension_
     *
     * @brief u_dimension field
     */

    /**
     * @var nurbs_patch_surface_node::u_knot_
     *
     * @brief u_knot field
     */

    /**
     * @var nurbs_patch_surface_node::u_order_
     *
     * @brief u_order field
     */

    /**
     * @var nurbs_patch_surface_node::v_closed_
     *
     * @brief v_closed field
     */

    /**
     * @var nurbs_patch_surface_node::v_dimension_
     *
     * @brief v_dimension field
     */

    /**
     * @var nurbs_patch_surface_node::v_knot_
     *
     * @brief v_knot field
     */

    /**
     * @var nurbs_patch_surface_node::v_order_
     *
     * @brief v_order field
     */


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    openvrml::viewer::object_t
    nurbs_patch_surface_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {
        //TODO: Implement this!
        return 0;
    }


    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool nurbs_patch_surface_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_patch_surface_node::
    nurbs_patch_surface_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        control_point_(*this),
        tex_coord_(*this),
        u_tessellation_(*this),
        v_tessellation_(*this),
        weight_(*this),
        solid_(true),
        u_order_(3),
        v_order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_patch_surface_node::~nurbs_patch_surface_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_nurbs::nurbs_patch_surface_metatype::id =
    "urn:X-openvrml:node:NurbsPatchSurface";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c nurbs_patch_surface_metatype.
 */
openvrml_node_x3d_nurbs::nurbs_patch_surface_metatype::
nurbs_patch_surface_metatype(openvrml::browser & browser):
    node_metatype(nurbs_patch_surface_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::nurbs_patch_surface_metatype::
~nurbs_patch_surface_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating NurbsPatchSurface nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c nurbs_patch_surface_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_nurbs::nurbs_patch_surface_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 15> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "controlPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "texCoord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "uTessellation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "vTessellation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "uClosed"),
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
                       field_value::sfbool_id,
                       "vClosed"),
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
    typedef node_type_impl<nurbs_patch_surface_node> node_type_t;

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
                &nurbs_patch_surface_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::control_point_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::tex_coord_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::u_tessellation_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::v_tessellation_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::weight_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::solid_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::u_closed_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::u_dimension_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::u_knot_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::u_order_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::v_closed_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::v_dimension_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::v_knot_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_patch_surface_node::v_order_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
