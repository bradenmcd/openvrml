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

# include "directional_light.h"
# include "abstract_light.h"
# include <private.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL directional_light_node :
        public openvrml_node_vrml97::abstract_light_node<directional_light_node> {

        friend class openvrml_node_vrml97::directional_light_metatype;

        exposedfield<openvrml::sfvec3f> direction_;

    public:
        directional_light_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~directional_light_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
    };

    /**
     * @class directional_light_node
     *
     * @brief DirectionalLight node instances.
     */

    /**
     * @var class directional_light_node::directional_light_metatype
     *
     * @brief Class object for DirectionalLight nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<directional_light_node>::exposedfield<openvrml::sfvec3f> directional_light_node::direction_
     *
     * @brief direction exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope     the scope to which the node belongs.
     */
    directional_light_node::
    directional_light_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        light_node(type, scope),
        openvrml_node_vrml97::abstract_light_node<directional_light_node>(
            type, scope),
        direction_(*this, openvrml::make_vec3f(0.0, 0.0, -1.0))
    {}

    /**
     * @brief Destroy.
     */
    directional_light_node::~directional_light_node()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the node.
     *
     * This should be called before rendering any sibling nodes.
     *
     * @param viewer    a viewer.
     * @param context   a rendering context.
     */
    void
    directional_light_node::
    do_render_child(openvrml::viewer & viewer, openvrml::rendering_context)
    {
        if (this->on_.sfbool::value()) {
            viewer.insert_dir_light(this->ambient_intensity_.sffloat::value(),
                                    this->intensity_.sffloat::value(),
                                    this->color_.sfcolor::value(),
                                    this->direction_.sfvec3f::value());
        }
        this->node::modified(false);
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::directional_light_metatype::id =
    "urn:X-openvrml:node:DirectionalLight";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::directional_light_metatype::
directional_light_metatype(openvrml::browser & browser):
    node_metatype(directional_light_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::directional_light_metatype::~directional_light_metatype()
    OPENVRML_NOTHROW
{}

# define DIRECTIONAL_LIGHT_INTERFACE_SEQ                              \
    ((exposedfield, sffloat, "ambientIntensity", ambient_intensity_)) \
    ((exposedfield, sfcolor, "color",            color_))             \
    ((exposedfield, sfvec3f, "direction",        direction_))         \
    ((exposedfield, sffloat, "intensity",        intensity_))         \
    ((exposedfield, sfbool,  "on",               on_))                \
    ((exposedfield, sfnode,  "metadata",         metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              directional_light_metatype,
                                              directional_light_node,
                                              DIRECTIONAL_LIGHT_INTERFACE_SEQ)
