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

# define NURBS_TEXTURE_COORDINATE_INTERFACE_SEQ                \
    ((exposedfield, sfnode,   "metadata",     metadata))       \
    ((exposedfield, sfnode,   "controlPoint", control_point_)) \
    ((exposedfield, mfdouble, "weight",       weight_))        \
    ((field,        sfint32,  "uDimension",   u_dimension_))   \
    ((field,        mfdouble, "uKnot",        u_knot_))        \
    ((field,        sfint32,  "uOrder",       u_order_))       \
    ((field,        sfint32,  "vDimension",   v_dimension_))   \
    ((field,        mfdouble, "vKnot",        v_knot_))        \
    ((field,        sfint32,  "vOrder",       v_order_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(
    openvrml_node_x3d_nurbs,
    nurbs_texture_coordinate_metatype,
    nurbs_texture_coordinate_node,
    NURBS_TEXTURE_COORDINATE_INTERFACE_SEQ)
