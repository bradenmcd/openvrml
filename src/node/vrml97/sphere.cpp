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

# include "sphere.h"
# include <private.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL sphere_node :
        public openvrml::node_impl_util::abstract_node<sphere_node>,
        public openvrml::geometry_node {

        friend class openvrml_node_vrml97::sphere_metatype;

        openvrml::sffloat radius;
        openvrml::sfbool solid_;
        openvrml::bounding_sphere bsphere;

    public:
        sphere_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~sphere_node() OPENVRML_NOTHROW;

    private:
        virtual const openvrml::bounding_volume & do_bounding_volume() const;
        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        openvrml::rendering_context context);
    };

    /**
     * @class sphere_node
     *
     * @brief Sphere node instances.
     */

    /**
     * @var class sphere_node::sphere_metatype
     *
     * @brief Class object for Sphere instances.
     */

    /**
     * @var openvrml::sffloat sphere_node::radius
     *
     * @brief radius field.
     */

    /**
     * @var openvrml::bounding_sphere sphere_node::bsphere
     *
     * @brief Bounding volume.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    sphere_node::
    sphere_node(const openvrml::node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<sphere_node>(type, scope),
        geometry_node(type, scope),
        radius(1.0),
        solid_(true)
    {
        this->bounding_volume_dirty(true); // lazy calc of bvolumes
    }

    /**
     * @brief Destroy.
     */
    sphere_node::~sphere_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a @c viewer.
     * @param context   the rendering context.
     */
    void
    sphere_node::
    do_render_geometry(openvrml::viewer & viewer, openvrml::rendering_context)
    {
        viewer.insert_sphere(*this, this->radius.value());
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    sphere_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<sphere_node *>(this)
                ->bsphere.radius(this->radius.value());
            const_cast<sphere_node *>(this)
                ->bounding_volume_dirty(false); // logical const
        }
        return this->bsphere;
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::sphere_metatype::id =
    "urn:X-openvrml:node:Sphere";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::sphere_metatype::
sphere_metatype(openvrml::browser & browser):
    node_metatype(sphere_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::sphere_metatype::~sphere_metatype() OPENVRML_NOTHROW
{}

# define SPHERE_INTERFACE_SEQ                       \
    ((field,        sffloat, "radius",   radius))   \
    ((exposedfield, sfnode,  "metadata", metadata)) \
    ((field,        sfbool,  "solid",    solid_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              sphere_metatype,
                                              sphere_node,
                                              SPHERE_INTERFACE_SEQ)
