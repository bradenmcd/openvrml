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

# include "nurbs_surface_interpolator.h"
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
     * @brief Represents NurbsSurfaceInterpolator node instances.
     */
    class OPENVRML_LOCAL nurbs_surface_interpolator_node :
        public abstract_node<nurbs_surface_interpolator_node>,
        public child_node {

        friend
        class openvrml_node_x3d_nurbs::nurbs_surface_interpolator_metatype;

        class set_fraction_listener : public event_listener_base<self_t>,
                                      public sfvec2f_listener {
        public:
            explicit set_fraction_listener(self_t & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfvec2f & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<sfnode> control_points_;
        exposedfield<mfdouble> weight_;
        sfvec3f position_changed_;
        sfvec3f_emitter position_changed_emitter_;
        sfvec3f normal_changed_;
        sfvec3f_emitter normal_changed_emitter_;
        sfint32 u_dimension_;
        mfdouble u_knot_;
        sfint32 u_order_;
        sfint32 v_dimension_;
        mfdouble v_knot_;
        sfint32 v_order_;

    public:
        nurbs_surface_interpolator_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_surface_interpolator_node() OPENVRML_NOTHROW;
    };


    /**
     * @var nurbs_surface_interpolator_node::nurbs_surface_interpolator_metatype
     *
     * @brief Class object for NurbsSurfaceInterpolator nodes.
     */

    /**
     * @var nurbs_surface_interpolator_node::set_fraction_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var nurbs_surface_interpolator_node::control_points_
     *
     * @brief control_points exposedField
     */

    /**
     * @var nurbs_surface_interpolator_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_surface_interpolator_node::position_changed_
     *
     * @brief position_changed eventOut
     */

    /**
     * @var nurbs_surface_interpolator_node::normal_changed_
     *
     * @brief normal_changed eventOut
     */

    /**
     * @var nurbs_surface_interpolator_node::u_dimension_
     *
     * @brief u_dimension field
     */

    /**
     * @var nurbs_surface_interpolator_node::u_knot_
     *
     * @brief u_knot field
     */

    /**
     * @var nurbs_surface_interpolator_node::u_order_
     *
     * @brief u_order field
     */

    /**
     * @var nurbs_surface_interpolator_node::v_dimension_
     *
     * @brief v_dimension field
     */

    /**
     * @var nurbs_surface_interpolator_node::v_knot_
     *
     * @brief v_knot field
     */

    /**
     * @var nurbs_surface_interpolator_node::v_order_
     *
     * @brief v_order field
     */

    nurbs_surface_interpolator_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfvec2f_listener(node)
    {}

    nurbs_surface_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void nurbs_surface_interpolator_node::set_fraction_listener::
    do_process_event(const sfvec2f & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    nurbs_surface_interpolator_node::
    nurbs_surface_interpolator_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        control_points_(*this),
        weight_(*this),
        position_changed_emitter_(*this, this->position_changed_),
        normal_changed_emitter_(*this, this->normal_changed_),
        u_order_(3),
        v_order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_surface_interpolator_node::~nurbs_surface_interpolator_node()
        OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_nurbs::nurbs_surface_interpolator_metatype::id =
    "urn:X-openvrml:node:NurbsSurfaceInterpolator";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c nurbs_surface_interpolator_metatype.
 */
openvrml_node_x3d_nurbs::nurbs_surface_interpolator_metatype::
nurbs_surface_interpolator_metatype(openvrml::browser & browser):
    node_metatype(nurbs_surface_interpolator_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::nurbs_surface_interpolator_metatype::
~nurbs_surface_interpolator_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating NurbsSurfaceInterpolator nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c nurbs_surface_interpolator_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_nurbs::nurbs_surface_interpolator_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 12> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sfvec2f_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "controlPoints"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "position_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "normal_changed"),
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
    typedef node_type_impl<nurbs_surface_interpolator_node> node_type_t;

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
                &nurbs_surface_interpolator_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::set_fraction_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::control_points_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::weight_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::position_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::normal_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::u_dimension_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::u_knot_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::u_order_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::v_dimension_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::v_knot_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &nurbs_surface_interpolator_node::v_order_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
