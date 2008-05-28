// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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
# include <openvrml/browser.h>
# include <openvrml/node_impl_util.h>
# include "appearance.h"

namespace {

    class OPENVRML_LOCAL appearance_node :
        public openvrml::node_impl_util::abstract_node<appearance_node>,
        public openvrml::appearance_node {

        friend class openvrml_node_vrml97::appearance_metatype;

        exposedfield<openvrml::sfnode> material_;
        exposedfield<openvrml::sfnode> texture_;
        exposedfield<openvrml::sfnode> texture_transform_;
        exposedfield<openvrml::sfnode> fill_properties_;
        exposedfield<openvrml::sfnode> line_properties_;

    public:
        appearance_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~appearance_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

        //
        // appearance_node implementation
        //
        virtual const boost::intrusive_ptr<node> & material() const OPENVRML_NOTHROW;
        virtual const boost::intrusive_ptr<node> & texture() const OPENVRML_NOTHROW;
        virtual const boost::intrusive_ptr<node> & texture_transform() const OPENVRML_NOTHROW;

    private:
        virtual void do_render_appearance(openvrml::viewer & v,
                                          openvrml::rendering_context context);
    };

    /**
     * @class appearance_node
     *
     * @brief Appearance node instances.
     */

    /**
     * @var class appearance_node::appearance_metatype
     *
     * @brief Class object for Appearance nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<appearance_node>::exposedfield<openvrml::sfnode> appearance_node::material_
     *
     * @brief material exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<appearance_node>::exposedfield<openvrml::sfnode> appearance_node::texture_
     *
     * @brief texture exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<appearance_node>::exposedfield<openvrml::sfnode> appearance_node::texture_transform_
     *
     * @brief textureTransform exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    appearance_node::
    appearance_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml::node_impl_util::abstract_node<appearance_node>(type, scope),
        openvrml::appearance_node(type, scope),
        material_(*this),
        texture_(*this),
        texture_transform_(*this),
        fill_properties_(*this),
        line_properties_(*this)
    {}

    /**
     * @brief Destroy.
     */
    appearance_node::~appearance_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *         @c false otherwise.
     */
    bool appearance_node::modified() const
    {
        return (this->node::modified()
                || (this->material_.sfnode::value()
                    && this->material_.sfnode::value()->modified())
                || (this->texture_.sfnode::value()
                    && this->texture_.sfnode::value()->modified())
                || (this->texture_transform_.sfnode::value()
                    && this->texture_transform_.sfnode::value()->modified()));
    }

    /**
     * @brief Get the material node.
     *
     * @returns an sfnode object containing the Material node associated with
     *          this Appearance.
     */
    const boost::intrusive_ptr<openvrml::node> &
    appearance_node::material() const OPENVRML_NOTHROW
    {
        return this->material_.sfnode::value();
    }

/**
 * @brief Get the texture node.
 *
 * @return an sfnode object containing the texture node associated with
 *         this Appearance.
 */
    const boost::intrusive_ptr<openvrml::node> &
    appearance_node::texture() const OPENVRML_NOTHROW
    {
        return this->texture_.sfnode::value();
    }

    /**
     * @brief Get the texture transform node.
     *
     * @return an sfnode object containing the TextureTransform node
     *         associated with this Appearance.
     */
    const boost::intrusive_ptr<openvrml::node> &
    appearance_node::texture_transform() const OPENVRML_NOTHROW
    {
        return this->texture_transform_.sfnode::value();
    }

    OPENVRML_LOCAL void set_unlit_material(openvrml::viewer & v)
    {
        using openvrml::color;
        using openvrml::make_color;
        static const float unlit_ambient_intensity(1);
        static const color unlit_diffuse_color = make_color(1, 1, 1);
        static const color unlit_emissive_color = make_color(1, 1, 1);
        static const float unlit_shininess(0);
        static const color unlit_specular_color = make_color(1, 1, 1);
        static const float unlit_transparency(0);
        v.set_material(unlit_ambient_intensity,
                       unlit_diffuse_color,
                       unlit_emissive_color,
                       unlit_shininess,
                       unlit_specular_color,
                       unlit_transparency);
    }

    /**
     * @brief render_appearance implementation.
     *
     * @param v         viewer.
     * @param context   rendering_context.
     */
    void
    appearance_node::
    do_render_appearance(openvrml::viewer & v, openvrml::rendering_context)
    {
        using namespace openvrml;

        openvrml::material_node * const material =
            node_cast<openvrml::material_node *>(
                this->material_.sfnode::value().get());
        texture_node * const texture =
            node_cast<texture_node *>(this->texture_.sfnode::value().get());

        if (material) {
            float trans = material->transparency();
            color diffuse = material->diffuse_color();
            size_t nTexComponents = texture ? texture->image().comp() : 0;
            if (nTexComponents == 2 || nTexComponents == 4) { trans = 0.0; }
            if (nTexComponents >= 3) { diffuse = make_color(1.0, 1.0, 1.0); }

            v.enable_lighting(true);   // turn lighting on for this object
            v.set_material(material->ambient_intensity(),
                           diffuse,
                           material->emissive_color(),
                           material->shininess(),
                           material->specular_color(),
                           trans);

            material->modified(false);
        } else {
            v.enable_lighting(false);   // turn lighting off for this object
            set_unlit_material(v);
        }

        if (texture) {
            openvrml::texture_transform_node * texture_transform =
                node_cast<openvrml::texture_transform_node *>(
                    this->texture_transform_.sfnode::value().get());
            if (texture_transform) {
                texture_transform->render_texture_transform(v);
            } else {
                static const vec2f center = make_vec2f(0.0, 0.0);
                static const float rotation = 0.0;
                static const vec2f scale = make_vec2f(1.0, 1.0);
                static const vec2f translation = make_vec2f(0.0, 0.0);
                v.set_texture_transform(center, rotation, scale, translation);
            }
            texture->render_texture(v);
        }
    }
}

/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::appearance_metatype::id =
    "urn:X-openvrml:node:Appearance";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::appearance_metatype::
appearance_metatype(openvrml::browser & browser):
    node_metatype(appearance_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::appearance_metatype::~appearance_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Appearance nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by appearance_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::appearance_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using openvrml::node_interface;
    using openvrml::node_interface_set;
    using openvrml::node_type;
    using openvrml::field_value;
    using openvrml::sfnode;
    using openvrml::unsupported_interface;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 6> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "material"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "texture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "textureTransform"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "fillProperties"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "lineProperties")
    };

    typedef node_type_impl<appearance_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & appearanceNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_ = interfaces.begin();
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            appearanceNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::material_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::material_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::material_)));
        } else if (*interface_ == *++supported_interface) {
            appearanceNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::texture_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::texture_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::texture_)));
        } else if (*interface_ == *++supported_interface) {
            appearanceNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::texture_transform_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::texture_transform_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::texture_transform_)));
        } else if (*interface_ == *++supported_interface) {
            appearanceNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::metadata)));
        } else if (*interface_ == *++supported_interface) {
            appearanceNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::fill_properties_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::fill_properties_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::fill_properties_)));
        } else if (*interface_ == *++supported_interface) {
            appearanceNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::line_properties_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::line_properties_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<appearance_node>::exposedfield<sfnode> >(
                        &appearance_node::line_properties_)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
