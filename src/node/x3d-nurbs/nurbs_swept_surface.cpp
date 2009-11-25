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

# include "nurbs_swept_surface.h"
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
     * @brief Represents NurbsSweptSurface node instances.
     */
    class OPENVRML_LOCAL nurbs_swept_surface_node :
        public abstract_node<nurbs_swept_surface_node>,
        public geometry_node,
        public child_node {

        friend class openvrml_node_x3d_nurbs::nurbs_swept_surface_metatype;

        exposedfield<sfnode> cross_section_curve_;
        exposedfield<sfnode> trajectory_curve_;
        sfbool ccw_;
        sfbool solid_;

    public:
        nurbs_swept_surface_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_swept_surface_node() OPENVRML_NOTHROW;

    private:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        rendering_context context);
    };


    /**
     * @var nurbs_swept_surface_node::nurbs_swept_surface_metatype
     *
     * @brief Class object for NurbsSweptSurface nodes.
     */

    /**
     * @var nurbs_swept_surface_node::cross_section_curve_
     *
     * @brief cross_section_curve exposedField
     */

    /**
     * @var nurbs_swept_surface_node::trajectory_curve_
     *
     * @brief trajectory_curve exposedField
     */

    /**
     * @var nurbs_swept_surface_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var nurbs_swept_surface_node::solid_
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
    nurbs_swept_surface_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool nurbs_swept_surface_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return (this->cross_section_curve_.value()
                && this->cross_section_curve_.value()->modified())
            || (this->trajectory_curve_.value()
                && this->trajectory_curve_.value()->modified());
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    nurbs_swept_surface_node::
    nurbs_swept_surface_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        cross_section_curve_(*this),
        trajectory_curve_(*this),
        ccw_(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_swept_surface_node::~nurbs_swept_surface_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_nurbs::nurbs_swept_surface_metatype::id =
    "urn:X-openvrml:node:NurbsSweptSurface";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c nurbs_swept_surface_metatype.
 */
openvrml_node_x3d_nurbs::nurbs_swept_surface_metatype::
nurbs_swept_surface_metatype(openvrml::browser & browser):
    node_metatype(nurbs_swept_surface_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::nurbs_swept_surface_metatype::
~nurbs_swept_surface_metatype()
    OPENVRML_NOTHROW
{}

# define NURBS_SWEPT_SURFACE_INTERFACE_SEQ                              \
    ((exposedfield, sfnode,   "metadata",          metadata))           \
    ((exposedfield, sfnode,   "crossSectionCurve", cross_section_curve_)) \
    ((exposedfield, sfnode,   "trajectoryCurve",   trajectory_curve_))  \
    ((field,        sfbool,   "ccw",               ccw_))               \
    ((field,        sfbool,   "solid",             solid_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_nurbs,
                                              nurbs_swept_surface_metatype,
                                              nurbs_swept_surface_node,
                                              NURBS_SWEPT_SURFACE_INTERFACE_SEQ)
