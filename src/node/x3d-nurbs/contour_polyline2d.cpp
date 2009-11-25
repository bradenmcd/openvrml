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

# include "contour_polyline2d.h"
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
     * @brief Represents ContourPolyline2D node instances.
     */
    class OPENVRML_LOCAL contour_polyline2d_node :
        public abstract_node<contour_polyline2d_node> {

        friend class openvrml_node_x3d_nurbs::contour_polyline2d_metatype;

        exposedfield<mfvec2f> point_;

    public:
        contour_polyline2d_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~contour_polyline2d_node() OPENVRML_NOTHROW;
    };


    /**
     * @var contour_polyline2d_node::contour_polyline2d_metatype
     *
     * @brief Class object for ContourPolyline2D nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<contour_polyline2d_node>::exposedfield<openvrml::mfvec2f> contour_polyline2d_node::point_
     *
     * @brief point exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    contour_polyline2d_node::
    contour_polyline2d_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        point_(*this)
    {}

    /**
     * @brief Destroy.
     */
    contour_polyline2d_node::~contour_polyline2d_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_nurbs::contour_polyline2d_metatype::id =
    "urn:X-openvrml:node:ContourPolyline2D";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c contour_polyline2d_metatype.
 */
openvrml_node_x3d_nurbs::contour_polyline2d_metatype::
contour_polyline2d_metatype(openvrml::browser & browser):
    node_metatype(contour_polyline2d_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::contour_polyline2d_metatype::
~contour_polyline2d_metatype()
    OPENVRML_NOTHROW
{}

# define CONTOUR_POLYLINE2D_INTERFACE_SEQ               \
    ((exposedfield, sfnode,  "metadata",     metadata)) \
    ((exposedfield, mfvec2f, "point",        point_))   \
    ((exposedfield, mfvec2f, "controlPoint", point_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_nurbs,
                                              contour_polyline2d_metatype,
                                              contour_polyline2d_node,
                                              CONTOUR_POLYLINE2D_INTERFACE_SEQ)
