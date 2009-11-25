// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
// Copyright 2002  S. K. Bose
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "material.h"

namespace {

    class OPENVRML_LOCAL material_node :
        public openvrml::node_impl_util::abstract_node<material_node>,
        public openvrml::material_node {

        friend class openvrml_node_vrml97::material_metatype;

        exposedfield<openvrml::sffloat> ambient_intensity_;
        exposedfield<openvrml::sfcolor> diffuse_color_;
        exposedfield<openvrml::sfcolor> emissive_color_;
        exposedfield<openvrml::sffloat> shininess_;
        exposedfield<openvrml::sfcolor> specular_color_;
        exposedfield<openvrml::sffloat> transparency_;

    public:
        material_node(const openvrml::node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~material_node() OPENVRML_NOTHROW;

    private:
        //
        // material_node implementation
        //
        virtual float do_ambient_intensity() const OPENVRML_NOTHROW;
        virtual const openvrml::color & do_diffuse_color() const
            OPENVRML_NOTHROW;
        virtual const openvrml::color & do_emissive_color() const
            OPENVRML_NOTHROW;
        virtual float do_shininess() const OPENVRML_NOTHROW;
        virtual const openvrml::color & do_specular_color() const
            OPENVRML_NOTHROW;
        virtual float do_transparency() const OPENVRML_NOTHROW;
    };

    /**
     * @class material_node
     *
     * @brief Material node instances.
     */

    /**
     * @var class material_node::material_metatype
     *
     * @brief Class object for Material nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<material_node>::exposedfield<openvrml::sffloat> material_node::ambient_intensity_
     *
     * @brief ambientIntensity exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<material_node>::exposedfield<openvrml::sfcolor> material_node::diffuse_color_
     *
     * @brief diffuseColor exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<material_node>::exposedfield<openvrml::sfcolor> material_node::emissive_color_
     *
     * @brief emissiveColor exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<material_node>::exposedfield<openvrml::sffloat> material_node::shininess_
     *
     * @brief shininess exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<material_node>::exposedfield<openvrml::sfcolor> material_node::specular_color_
     *
     * @brief specularColor exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<material_node>::exposedfield<openvrml::sffloat> material_node::transparency_
     *
     * @brief transparency exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    material_node::
    material_node(const openvrml::node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml::node_impl_util::abstract_node<material_node>(type, scope),
        openvrml::material_node(type, scope),
        ambient_intensity_(*this, 0.2f),
        diffuse_color_(*this, openvrml::make_color(0.8f, 0.8f, 0.8f)),
        emissive_color_(*this, openvrml::make_color(0.0, 0.0, 0.0)),
        shininess_(*this, 0.2f),
        specular_color_(*this, openvrml::make_color(0.0, 0.0, 0.0)),
        transparency_(*this, 0.0)
    {}

    /**
     * @brief Destroy.
     */
    material_node::~material_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the ambient intensity.
     *
     * @return the ambient intensity.
     */
    float material_node::do_ambient_intensity() const OPENVRML_NOTHROW
    {
        return this->ambient_intensity_.sffloat::value();
    }

    /**
     * @brief Get the diffuse color.
     *
     * @return the diffuse color.
     */
    const openvrml::color &
    material_node::do_diffuse_color() const OPENVRML_NOTHROW
    {
        return this->diffuse_color_.sfcolor::value();
    }

    /**
     * @brief Get the emissive color.
     *
     * @return the emissive color.
     */
    const openvrml::color &
    material_node::do_emissive_color() const OPENVRML_NOTHROW
    {
        return this->emissive_color_.sfcolor::value();
    }

    /**
     * @brief Get the shininess.
     *
     * @return the shininess.
     */
    float material_node::do_shininess() const OPENVRML_NOTHROW
    {
        return this->shininess_.sffloat::value();
    }

    /**
     * @brief Get the specular color.
     *
     * @return the specular color.
     */
    const openvrml::color &
    material_node::do_specular_color() const OPENVRML_NOTHROW
    {
        return this->specular_color_.sfcolor::value();
    }

    /**
     * @brief Get the transparency.
     *
     * @return the transparency.
     */
    float material_node::do_transparency() const OPENVRML_NOTHROW
    {
        return this->transparency_.sffloat::value();
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::material_metatype::id =
    "urn:X-openvrml:node:Material";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::material_metatype::
material_metatype(openvrml::browser & browser):
    node_metatype(material_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::material_metatype::~material_metatype() OPENVRML_NOTHROW
{}

# define MATERIAL_INTERFACE_SEQ                                       \
    ((exposedfield, sffloat, "ambientIntensity", ambient_intensity_)) \
    ((exposedfield, sfcolor, "diffuseColor",     diffuse_color_))     \
    ((exposedfield, sfcolor, "emissiveColor",    emissive_color_))    \
    ((exposedfield, sffloat, "shininess",        shininess_))         \
    ((exposedfield, sfcolor, "specularColor",    specular_color_))    \
    ((exposedfield, sffloat, "transparency",     transparency_))      \
    ((exposedfield, sfnode,  "metadata",         metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              material_metatype,
                                              material_node,
                                              MATERIAL_INTERFACE_SEQ)
