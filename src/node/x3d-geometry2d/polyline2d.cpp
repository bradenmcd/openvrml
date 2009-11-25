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

# include "polyline2d.h"
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
     * @brief Represents Polyline2D node instances.
     */
    class OPENVRML_LOCAL polyline2d_node :
        public abstract_node<polyline2d_node>,
        public geometry_node,
        public child_node {

        friend class openvrml_node_x3d_geometry2d::polyline2d_metatype;

        mfvec2f line_segments_;

    public:
        polyline2d_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~polyline2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        rendering_context context);
    };


    /**
     * @var polyline2d_node::polyline2d_metatype
     *
     * @brief Class object for Polyline2D nodes.
     */

    /**
     * @var polyline2d_node::line_segments_
     *
     * @brief line_segments field
     */


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    void
    polyline2d_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {}


    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool polyline2d_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    polyline2d_node::
    polyline2d_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope)
    {}

    /**
     * @brief Destroy.
     */
    polyline2d_node::~polyline2d_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geometry2d::polyline2d_metatype::id =
    "urn:X-openvrml:node:Polyline2D";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c polyline2d_metatype.
 */
openvrml_node_x3d_geometry2d::polyline2d_metatype::
polyline2d_metatype(openvrml::browser & browser):
    node_metatype(polyline2d_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geometry2d::polyline2d_metatype::~polyline2d_metatype()
    OPENVRML_NOTHROW
{}

# define POLYLINE2D_INTERFACE_SEQ                               \
    ((exposedfield, sfnode,  "metadata",     metadata))         \
    ((field,        mfvec2f, "lineSegments", line_segments_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_geometry2d,
                                              polyline2d_metatype,
                                              polyline2d_node,
                                              POLYLINE2D_INTERFACE_SEQ)
