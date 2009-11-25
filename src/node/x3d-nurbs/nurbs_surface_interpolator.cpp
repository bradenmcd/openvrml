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

# define NURBS_SURFACE_INTERPOLATOR_INTERFACE_SEQ                       \
    ((exposedfield, sfnode,   "metadata",         metadata))            \
    ((eventin,      sfvec2f,  "set_fraction",     set_fraction_listener_)) \
    ((exposedfield, sfnode,   "controlPoints",    control_points_))     \
    ((exposedfield, mfdouble, "weight",           weight_))             \
    ((eventout,     sfvec3f,  "position_changed", position_changed_emitter_)) \
    ((eventout,     sfvec3f,  "normal_changed",   normal_changed_emitter_)) \
    ((field,        sfint32,  "uDimension",       u_dimension_))        \
    ((field,        mfdouble, "uKnot",            u_knot_))             \
    ((field,        sfint32,  "uOrder",           u_order_))            \
    ((field,        sfint32,  "vDimension",       v_dimension_))        \
    ((field,        mfdouble, "vKnot",            v_knot_))             \
    ((field,        sfint32,  "vOrder",           v_order_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(
    openvrml_node_x3d_nurbs,
    nurbs_surface_interpolator_metatype,
    nurbs_surface_interpolator_node,
    NURBS_SURFACE_INTERPOLATOR_INTERFACE_SEQ)
