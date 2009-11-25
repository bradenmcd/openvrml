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

# include "nurbs_curve2d.h"
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
     * @brief Represents NurbsCurve2D node instances.
     */
    class OPENVRML_LOCAL nurbs_curve2d_node :
        public abstract_node<nurbs_curve2d_node> {

        friend class openvrml_node_x3d_nurbs::nurbs_curve2d_metatype;

        exposedfield<mfvec2d> control_point_;
        exposedfield<sfint32> tessellation_;
        exposedfield<mfdouble> weight_;
        sfbool closed_;
        mfdouble knot_;
        sfint32 order_;

    public:
        nurbs_curve2d_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_curve2d_node() OPENVRML_NOTHROW;
    };


    /**
     * @var nurbs_curve2d_node::nurbs_curve2d_metatype
     *
     * @brief Class object for NurbsCurve2D nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<nurbs_curve2d_node>::exposedfield<openvrml::mfvec2d> nurbs_curve2d_node::control_point_
     *
     * @brief control_point exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<nurbs_curve2d_node>::exposedfield<openvrml::sfint32> nurbs_curve2d_node::tessellation_
     *
     * @brief tessellation exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<nurbs_curve2d_node>::exposedfield<openvrml::mfdouble> nurbs_curve2d_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var openvrml::sfbool nurbs_curve2d_node::closed_
     *
     * @brief closed field
     */

    /**
     * @var openvrml::mfdouble nurbs_curve2d_node::knot_
     *
     * @brief knot field
     */

    /**
     * @var openvrml::sfint32 nurbs_curve2d_node::order_
     *
     * @brief order field
     */


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_curve2d_node::
    nurbs_curve2d_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        control_point_(*this),
        tessellation_(*this),
        weight_(*this),
        order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_curve2d_node::~nurbs_curve2d_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_nurbs::nurbs_curve2d_metatype::id =
    "urn:X-openvrml:node:NurbsCurve2D";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c nurbs_curve2d_metatype.
 */
openvrml_node_x3d_nurbs::nurbs_curve2d_metatype::
nurbs_curve2d_metatype(openvrml::browser & browser):
    node_metatype(nurbs_curve2d_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::nurbs_curve2d_metatype::~nurbs_curve2d_metatype()
    OPENVRML_NOTHROW
{}

# define NURBS_CURVE2D_INTERFACE_SEQ                            \
    ((exposedfield, sfnode,   "metadata",     metadata))        \
    ((exposedfield, mfvec2d,  "controlPoint", control_point_))  \
    ((exposedfield, sfint32,  "tessellation", tessellation_))   \
    ((exposedfield, mfdouble, "weight",       weight_))         \
    ((field,        sfbool,   "closed",       closed_))         \
    ((field,        mfdouble, "knot",         knot_))           \
    ((field,        sfint32,  "order",        order_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_nurbs,
                                              nurbs_curve2d_metatype,
                                              nurbs_curve2d_node,
                                              NURBS_CURVE2D_INTERFACE_SEQ)
