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

# include "pixel_texture.h"
# include "abstract_texture.h"
# include <private.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL pixel_texture_node :
        public openvrml_node_vrml97::abstract_texture_node<pixel_texture_node> {

        friend class openvrml_node_vrml97::pixel_texture_metatype;

        exposedfield<openvrml::sfimage> image_;

    public:
        pixel_texture_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~pixel_texture_node() OPENVRML_NOTHROW;

    private:
        virtual const openvrml::image & do_image() const OPENVRML_NOTHROW;
        virtual void do_render_texture(openvrml::viewer & v);
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
    pixel_texture_node::do_image() const OPENVRML_NOTHROW
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
    void pixel_texture_node::do_render_texture(openvrml::viewer & v)
    {
        v.insert_texture(*this, true);
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

# define PIXEL_TEXTURE_INTERFACE_SEQ                 \
    ((exposedfield, sfimage, "image",    image_))    \
    ((field,        sfbool,  "repeatS",  repeat_s_)) \
    ((field,        sfbool,  "repeatT",  repeat_t_)) \
    ((exposedfield, sfnode,  "metadata", metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              pixel_texture_metatype,
                                              pixel_texture_node,
                                              PIXEL_TEXTURE_INTERFACE_SEQ)
