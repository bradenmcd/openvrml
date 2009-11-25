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

# include "triangle_set2d.h"
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
     * @brief Represents TriangleSet2D node instances.
     */
    class OPENVRML_LOCAL triangle_set2d_node :
        public abstract_node<triangle_set2d_node>,
        public geometry_node,
        public child_node {

        friend class openvrml_node_x3d_geometry2d::triangle_set2d_metatype;

        exposedfield<mfvec2f> vertices_;
        sfbool solid_;

    public:
        triangle_set2d_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~triangle_set2d_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        rendering_context context);
    };


    /**
     * @var triangle_set2d_node::triangle_set2d_metatype
     *
     * @brief Class object for TriangleSet2D nodes.
     */

    /**
     * @var triangle_set2d_node::vertices_
     *
     * @brief vertices exposedField
     */

    /**
     * @var triangle_set2d_node::solid_
     *
     * @brief solid field
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
    triangle_set2d_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {}

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    triangle_set2d_node::
    triangle_set2d_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        vertices_(*this)
    {}

    /**
     * @brief Destroy.
     */
    triangle_set2d_node::~triangle_set2d_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geometry2d::triangle_set2d_metatype::id =
    "urn:X-openvrml:node:TriangleSet2D";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c triangle_set2d_metatype.
 */
openvrml_node_x3d_geometry2d::triangle_set2d_metatype::
triangle_set2d_metatype(openvrml::browser & browser):
    node_metatype(triangle_set2d_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geometry2d::triangle_set2d_metatype::
~triangle_set2d_metatype()
    OPENVRML_NOTHROW
{}

# define TRIANGLE_SET2D_INTERFACE_SEQ                   \
    ((exposedfield, sfnode,  "metadata", metadata))     \
    ((field,        mfvec2f, "vertices", vertices_))    \
    ((field,        sfbool,  "solid",    solid_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_geometry2d,
                                              triangle_set2d_metatype,
                                              triangle_set2d_node,
                                              TRIANGLE_SET2D_INTERFACE_SEQ)
