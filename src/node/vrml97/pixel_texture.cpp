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
# include <private.h>
# include "pixel_texture.h"
# include "abstract_texture.h"

namespace {

    class OPENVRML_LOCAL pixel_texture_node :
        public openvrml_node_vrml97::abstract_texture_node<pixel_texture_node> {

        friend class openvrml_node_vrml97::pixel_texture_metatype;

        exposedfield<openvrml::sfimage> image_;

    public:
        pixel_texture_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~pixel_texture_node() OPENVRML_NOTHROW;

        virtual const openvrml::image & image() const OPENVRML_NOTHROW;

    private:
        virtual openvrml::viewer::texture_object_t
            do_render_texture(openvrml::viewer & v);
    };

    /**
     * @class pixel_texture_node
     *
     * @brief Represents PixelTexture node instances.
     */

    /**
     * @var class pixel_texture_node::pixel_texture_metatype
     *
     * @brief Class object for PixelTexture nodes.
     */

    /**
     * @var openvrml::sfimage pixel_texture_node::image_
     *
     * @brief image exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope the scope to which the node belongs.
     */
    pixel_texture_node::
    pixel_texture_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml_node_vrml97::abstract_texture_node<pixel_texture_node>(
            type, scope),
        image_(*this)
    {}

    /**
     * @brief Destroy.
     */
    pixel_texture_node::~pixel_texture_node() OPENVRML_NOTHROW
    {
        // viewer.remove_texture_object(this->texObject); ...
    }

    /**
     * @brief The image.
     *
     * @return the image.
     */
    const openvrml::image &
    pixel_texture_node::image() const OPENVRML_NOTHROW
    {
        return this->image_.sfimage::value();
    }

    /**
     * @brief render_texture implementation.
     *
     * @param v viewer.
     *
     * @return object identifier for the inserted texture.
     */
    openvrml::viewer::texture_object_t
    pixel_texture_node::do_render_texture(openvrml::viewer & v)
    {
        return v.insert_texture(this->image_.sfimage::value(),
                                this->repeat_s_.value(),
                                this->repeat_t_.value(),
                                true);
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::pixel_texture_metatype::id =
    "urn:X-openvrml:node:PixelTexture";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::pixel_texture_metatype::
pixel_texture_metatype(openvrml::browser & browser):
    node_metatype(pixel_texture_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::pixel_texture_metatype::~pixel_texture_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating PixelTexture nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by pixel_texture_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::pixel_texture_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 4> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfimage_id,
                       "image"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "repeatS"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "repeatT"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<pixel_texture_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & pixelTextureNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end(); ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            pixelTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<pixel_texture_node>::exposedfield<sfimage> >(
                        &pixel_texture_node::image_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<pixel_texture_node>::exposedfield<sfimage> >(
                        &pixel_texture_node::image_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<pixel_texture_node>::exposedfield<sfimage> >(
                        &pixel_texture_node::image_)));
        } else if (*interface_ == *++supported_interface) {
            pixelTextureNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &pixel_texture_node::repeat_s_)));
        } else if (*interface_ == *++supported_interface) {
            pixelTextureNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &pixel_texture_node::repeat_t_)));
        } else if (*interface_ == *++supported_interface) {
            pixelTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<pixel_texture_node>::exposedfield<sfnode> >(
                        &pixel_texture_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<pixel_texture_node>::exposedfield<sfnode> >(
                        &pixel_texture_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<pixel_texture_node>::exposedfield<sfnode> >(
                        &pixel_texture_node::metadata)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
