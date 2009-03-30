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

# include "image_texture.h"
# include "abstract_texture.h"
# include "image_stream_listener.h"
# include <openvrml/scene.h>
# include <private.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL image_texture_node :
        public openvrml_node_vrml97::abstract_texture_node<image_texture_node> {

        friend class openvrml_node_vrml97::image_texture_metatype;

        class url_exposedfield : public exposedfield<openvrml::mfstring> {
        public:
            explicit url_exposedfield(image_texture_node & node);
            url_exposedfield(const url_exposedfield & obj) OPENVRML_NOTHROW;
            virtual ~url_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::mfstring & url,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        url_exposedfield url_;

        openvrml::read_write_mutex image_mutex_;
        openvrml::image image_;
        bool texture_needs_update;

    public:
        image_texture_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~image_texture_node() OPENVRML_NOTHROW;

    private:
        virtual const openvrml::image & do_image() const OPENVRML_NOTHROW;
        virtual openvrml::viewer::texture_object_t
            do_render_texture(openvrml::viewer & v);

        void update_texture();
    };

    /**
     * @class image_texture_node
     *
     * @brief Represents ImageTexture node instances.
     */

    /**
     * @var class image_texture_node::image_texture_metatype
     *
     * @brief Class object for ImageTexture nodes.
     */

    /**
     * @internal
     *
     * @class image_texture_node::url_exposedfield
     *
     * @brief url exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param node  image_texture_node.
     */
    image_texture_node::url_exposedfield::
    url_exposedfield(image_texture_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        mfstring_listener(node),
        exposedfield<mfstring>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    image_texture_node::url_exposedfield::
    url_exposedfield(const url_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        mfstring_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::mfstring>(obj)
    {}

    /**
     * @brief Destroy.
     */
    image_texture_node::url_exposedfield::
    ~url_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    image_texture_node::url_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new url_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param url       url.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    image_texture_node::url_exposedfield::
    event_side_effect(const openvrml::mfstring & url, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            image_texture_node & image_texture =
                dynamic_cast<image_texture_node &>(
                    this->node_event_listener::node());

            image_texture.url_.mfstring::value(url.value());
            image_texture.texture_needs_update = true;
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var image_texture_node::url_exposedfield image_texture_node::url_
     *
     * @brief url exposedField.
     */

    /**
     * @var openvrml::image image_texture_node::image_
     *
     * @brief Image data.
     */

    /**
     * @var bool image_texture_node::texture_needs_update
     *
     * @brief Flag to indicate that image_texture_node::update_texture should
     *        be called.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope the scope to which the node belongs.
     */
    image_texture_node::
    image_texture_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml_node_vrml97::abstract_texture_node<image_texture_node>(
            type, scope),
        url_(*this),
        texture_needs_update(true)
    {}

    /**
     * @brief Destroy.
     */
    image_texture_node::~image_texture_node() OPENVRML_NOTHROW
    {
        // delete texObject...
    }

    /**
     * @brief The image.
     *
     * @return the image.
     */
    const openvrml::image &
    image_texture_node::do_image() const OPENVRML_NOTHROW
    {
        return this->image_;
    }

    /**
     * @brief render_texture implementation.
     *
     * @param v viewer.
     *
     * @return object identifier for the inserted texture.
     */
    openvrml::viewer::texture_object_t
    image_texture_node::do_render_texture(openvrml::viewer & v)
    {
        this->update_texture();
        return v.insert_texture(this->image_,
                                this->repeat_s_.value(),
                                this->repeat_t_.value(),
                                true);
    }

    /**
     * @brief Called to update texture data lazily.
     */
    void image_texture_node::update_texture()
    {
        assert(this->scene());

        if (this->texture_needs_update) {
            using openvrml_node_vrml97::image_stream_listener;
            try {
                if (!this->url_.mfstring::value().empty()) {
                    using std::auto_ptr;
                    auto_ptr<openvrml::resource_istream> in(
                        this->scene()->get_resource(
                            this->url_.mfstring::value()));
                    auto_ptr<openvrml::stream_listener> listener(
                        new image_stream_listener(in->url(),
                                                  this->image_,
                                                  *this,
                                                  this->image_mutex_));
                    this->scene()->read_stream(in, listener);
                }
            } catch (std::exception & ex) {
                this->scene()->browser().err(ex.what());
            }
            this->texture_needs_update = false;
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::image_texture_metatype::id =
    "urn:X-openvrml:node:ImageTexture";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::image_texture_metatype::
image_texture_metatype(openvrml::browser & browser):
    node_metatype(image_texture_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::image_texture_metatype::~image_texture_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating ImageTexture nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by image_texture_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::image_texture_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 4> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
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

    typedef node_impl_util::node_type_impl<image_texture_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & imageTextureNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            imageTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &image_texture_node::url_);
        } else if (*interface_ == *++supported_interface) {
            imageTextureNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &image_texture_node::repeat_s_);
        } else if (*interface_ == *++supported_interface) {
            imageTextureNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &image_texture_node::repeat_t_);
        } else if (*interface_ == *++supported_interface) {
            imageTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &image_texture_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
