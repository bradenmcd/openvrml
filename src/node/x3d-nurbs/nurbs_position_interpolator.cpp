// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008, 2009  Braden McDaniel
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

# include "nurbs_position_interpolator.h"
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
     * @brief Represents NurbsPositionInterpolator node instances.
     */
    class OPENVRML_LOCAL nurbs_position_interpolator_node :
        public abstract_node<nurbs_position_interpolator_node>,
        public child_node {

        friend
        class openvrml_node_x3d_nurbs::nurbs_position_interpolator_metatype;

        class set_fraction_listener : public event_listener_base<self_t>,
                                      public sffloat_listener {
        public:
            explicit set_fraction_listener(self_t & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<sfnode> control_points_;
        exposedfield<mfdouble> knot_;
        exposedfield<sfint32> order_;
        exposedfield<mfdouble> weight_;
        sfvec3f value_changed_;
        sfvec3f_emitter value_changed_emitter_;

    public:
        nurbs_position_interpolator_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);

        virtual ~nurbs_position_interpolator_node() OPENVRML_NOTHROW;
    };


    /**
     * @var nurbs_position_interpolator_node::nurbs_position_interpolator_metatype
     *
     * @brief Class object for NurbsPositionInterpolator nodes.
     */

    /**
     * @var nurbs_position_interpolator_node::set_fraction_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var nurbs_position_interpolator_node::control_points_
     *
     * @brief control_points exposedField
     */

    /**
     * @var nurbs_position_interpolator_node::knot_
     *
     * @brief knot exposedField
     */

    /**
     * @var nurbs_position_interpolator_node::order_
     *
     * @brief order exposedField
     */

    /**
     * @var nurbs_position_interpolator_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_position_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut
     */

    nurbs_position_interpolator_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    nurbs_position_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void nurbs_position_interpolator_node::set_fraction_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_position_interpolator_node::
    nurbs_position_interpolator_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        control_points_(*this),
        knot_(*this),
        order_(*this, 3),
        weight_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_position_interpolator_node::~nurbs_position_interpolator_node()
        OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_nurbs::nurbs_position_interpolator_metatype::id =
    "urn:X-openvrml:node:NurbsPositionInterpolator";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c nurbs_position_interpolator_metatype.
 */
openvrml_node_x3d_nurbs::nurbs_position_interpolator_metatype::
nurbs_position_interpolator_metatype(openvrml::browser & browser):
    node_metatype(nurbs_position_interpolator_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::nurbs_position_interpolator_metatype::
~nurbs_position_interpolator_metatype()
    OPENVRML_NOTHROW
{}

# define NURBS_POSITION_INTERPOLATOR_INTERFACE_SEQ                      \
    ((exposedfield, sfnode,   "metadata",      metadata))               \
    ((eventin,      sffloat,  "set_fraction",  set_fraction_listener_)) \
    ((exposedfield, sfnode,   "controlPoints", control_points_))        \
    ((exposedfield, mfdouble, "knot",          knot_))                  \
    ((exposedfield, sfint32,  "order",         order_))                 \
    ((exposedfield, mfdouble, "weight",        weight_))                \
    ((eventout,     sfvec3f,  "value_changed", value_changed_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(
    openvrml_node_x3d_nurbs,
    nurbs_position_interpolator_metatype,
    nurbs_position_interpolator_node,
    NURBS_POSITION_INTERPOLATOR_INTERFACE_SEQ)
