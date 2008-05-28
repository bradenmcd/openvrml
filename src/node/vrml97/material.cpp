// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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

        //
        // material_node implementation
        //
        virtual float ambient_intensity() const OPENVRML_NOTHROW;
        virtual const openvrml::color & diffuse_color() const OPENVRML_NOTHROW;
        virtual const openvrml::color & emissive_color() const
            OPENVRML_NOTHROW;
        virtual float shininess() const OPENVRML_NOTHROW;
        virtual const openvrml::color & specular_color() const
            OPENVRML_NOTHROW;
        virtual float transparency() const OPENVRML_NOTHROW;
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
    float material_node::ambient_intensity() const OPENVRML_NOTHROW
    {
        return this->ambient_intensity_.sffloat::value();
    }

    /**
     * @brief Get the diffuse color.
     *
     * @return the diffuse color.
     */
    const openvrml::color &
    material_node::diffuse_color() const OPENVRML_NOTHROW
    {
        return this->diffuse_color_.sfcolor::value();
    }

    /**
     * @brief Get the emissive color.
     *
     * @return the emissive color.
     */
    const openvrml::color &
    material_node::emissive_color() const OPENVRML_NOTHROW
    {
        return this->emissive_color_.sfcolor::value();
    }

    /**
     * @brief Get the shininess.
     *
     * @return the shininess.
     */
    float material_node::shininess() const OPENVRML_NOTHROW
    {
        return this->shininess_.sffloat::value();
    }

    /**
     * @brief Get the specular color.
     *
     * @return the specular color.
     */
    const openvrml::color &
    material_node::specular_color() const OPENVRML_NOTHROW
    {
        return this->specular_color_.sfcolor::value();
    }

    /**
     * @brief Get the transparency.
     *
     * @return the transparency.
     */
    float material_node::transparency() const OPENVRML_NOTHROW
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

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Material nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by material_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::material_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using openvrml::field_value;
    using openvrml::sfcolor;
    using openvrml::sffloat;
    using openvrml::sfnode;
    using openvrml::node_interface;
    using openvrml::node_interface_set;
    using openvrml::node_type;
    using openvrml::unsupported_interface;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 7> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "ambientIntensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "diffuseColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "emissiveColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "shininess"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "specularColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "transparency"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_type_impl<material_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & materialNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            materialNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::ambient_intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::ambient_intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::ambient_intensity_)));
        } else if (*interface_ == *++supported_interface) {
            materialNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::diffuse_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::diffuse_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::diffuse_color_)));
        } else if (*interface_ == *++supported_interface) {
            materialNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::emissive_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::emissive_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::emissive_color_)));
        } else if (*interface_ == *++supported_interface) {
            materialNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::shininess_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::shininess_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::shininess_)));
        } else if (*interface_ == *++supported_interface) {
            materialNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::specular_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::specular_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<material_node>::exposedfield<sfcolor> >(
                        &material_node::specular_color_)));
        } else if (*interface_ == *++supported_interface) {
            materialNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::transparency_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::transparency_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<material_node>::exposedfield<sffloat> >(
                        &material_node::transparency_)));
        } else if (*interface_ == *++supported_interface) {
            materialNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<material_node>::exposedfield<sfnode> >(
                        &material_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<material_node>::exposedfield<sfnode> >(
                        &material_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<material_node>::exposedfield<sfnode> >(
                        &material_node::metadata)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
