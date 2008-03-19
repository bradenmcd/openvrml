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
# include "texture_transform.h"

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL texture_transform_node :
        public openvrml::node_impl_util::abstract_node<texture_transform_node>,
        public openvrml::texture_transform_node {

        friend class openvrml_node_vrml97::texture_transform_metatype;

        exposedfield<openvrml::sfvec2f> center_;
        exposedfield<openvrml::sffloat> rotation_;
        exposedfield<openvrml::sfvec2f> scale_;
        exposedfield<openvrml::sfvec2f> translation_;

    public:
        texture_transform_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~texture_transform_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_texture_transform(openvrml::viewer & v);
    };

    /**
     * @class texture_transform_node
     *
     * @brief TextureTransform node instances.
     */

    /**
     * @var class texture_transform_node::texture_transform_metatype
     *
     * @brief Class object for TextureTransform instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<texture_transform_node>::exposedfield<openvrml::sfvec2f> texture_transform_node::center_
     *
     * @brief center exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<texture_transform_node>::exposedfield<openvrml::sffloat> texture_transform_node::rotation_
     *
     * @brief rotation exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<texture_transform_node>::exposedfield<openvrml::sfvec2f> texture_transform_node::scale_
     *
     * @brief scale exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<texture_transform_node>::exposedfield<openvrml::sfvec2f> texture_transform_node::translation_
     *
     * @brief translation exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type      the node_type associated with the instance.
     * @param scope         the scope that the new node will belong to.
     */
    texture_transform_node::
    texture_transform_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml::node_impl_util::abstract_node<texture_transform_node>(type, scope),
        openvrml::texture_transform_node(type, scope),
        center_(*this, openvrml::make_vec2f(0.0, 0.0)),
        rotation_(*this, 0.0),
        scale_(*this, openvrml::make_vec2f(1.0, 1.0)),
        translation_(*this, openvrml::make_vec2f(0.0, 0.0))
    {}

    /**
     * @brief Destroy.
     */
    texture_transform_node::~texture_transform_node()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief render_texture_transform implementation.
     *
     * @param v viewer.
     */
    void
    texture_transform_node::
    do_render_texture_transform(openvrml::viewer & v)
    {
        v.set_texture_transform(this->center_.sfvec2f::value(),
                                this->rotation_.sffloat::value(),
                                this->scale_.sfvec2f::value(),
                                this->translation_.sfvec2f::value());
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::texture_transform_metatype::id =
    "urn:X-openvrml:node:TextureTransform";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::texture_transform_metatype::
texture_transform_metatype(openvrml::browser & browser):
    node_metatype(texture_transform_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::texture_transform_metatype::~texture_transform_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating TextureTransform nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  texture_transform_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::texture_transform_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using openvrml::field_value;
    using openvrml::sfvec2f;
    using openvrml::sffloat;
    using openvrml::sfnode;
    using openvrml::node_interface;
    using openvrml::node_interface_set;
    using openvrml::node_type;
    using openvrml::unsupported_interface;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 5> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec2f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "rotation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec2f_id,
                       "scale"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec2f_id,
                       "translation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_type_impl<texture_transform_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & textureTransformNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            textureTransformNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<texture_transform_node>::
                    exposedfield<sfvec2f> >(
                        &texture_transform_node::center_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<texture_transform_node>::
                        exposedfield<sfvec2f> >(
                            &texture_transform_node::center_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<texture_transform_node>::
                        exposedfield<sfvec2f> >(
                            &texture_transform_node::center_)));
    } else if (*interface == *++supported_interface) {
            textureTransformNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<texture_transform_node>::
                    exposedfield<sffloat> >(
                        &texture_transform_node::rotation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<texture_transform_node>::
                        exposedfield<sffloat> >(
                            &texture_transform_node::rotation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<texture_transform_node>::
                        exposedfield<sffloat> >(
                            &texture_transform_node::rotation_)));
} else if (*interface == *++supported_interface) {
            textureTransformNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<texture_transform_node>::
                    exposedfield<sfvec2f> >(
                        &texture_transform_node::scale_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<texture_transform_node>::
                        exposedfield<sfvec2f> >(
                            &texture_transform_node::scale_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<texture_transform_node>::
                        exposedfield<sfvec2f> >(
                            &texture_transform_node::scale_)));
} else if (*interface == *++supported_interface) {
            textureTransformNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<texture_transform_node>::
                    exposedfield<sfvec2f> >(
                        &texture_transform_node::translation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<texture_transform_node>::
                        exposedfield<sfvec2f> >(
                            &texture_transform_node::translation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<texture_transform_node>::
                        exposedfield<sfvec2f> >(
                            &texture_transform_node::translation_)));
    } else if (*interface == *++supported_interface) {
            textureTransformNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<texture_transform_node>::exposedfield<sfnode> >(
                        &texture_transform_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<texture_transform_node>::exposedfield<sfnode> >(
                        &texture_transform_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<texture_transform_node>::exposedfield<sfnode> >(
                        &texture_transform_node::metadata)));
        } else {
            throw unsupported_interface(*interface);
        }
}
    return type;
}
