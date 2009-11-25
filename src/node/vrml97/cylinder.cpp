// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
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

# include "cylinder.h"
# include <private.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

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
        virtual void do_render_geometry(openvrml::viewer & viewer,
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
    void cylinder_node::do_render_geometry(openvrml::viewer & viewer,
                                           openvrml::rendering_context)
    {
        viewer.insert_cylinder(*this,
                               this->height.value(),
                               this->radius.value(),
                               this->bottom.value(),
                               this->side.value(),
                               this->top.value());
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

# define CYLINDER_INTERFACE_SEQ                     \
    ((field,        sfbool,  "bottom",   bottom))   \
    ((field,        sffloat, "height",   height))   \
    ((field,        sffloat, "radius",   radius))   \
    ((field,        sfbool,  "side",     side))     \
    ((field,        sfbool,  "top",      top))      \
    ((exposedfield, sfnode,  "metadata", metadata)) \
    ((field,        sfbool,  "solid",    solid_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              cylinder_metatype,
                                              cylinder_node,
                                              CYLINDER_INTERFACE_SEQ)
