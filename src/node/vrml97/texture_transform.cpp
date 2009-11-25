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

# include "texture_transform.h"
# include <private.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

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

# define TEXTURE_TRANSFORM_INTERFACE_SEQ                   \
    ((exposedfield, sfvec2f, "center",      center_))      \
    ((exposedfield, sffloat, "rotation",    rotation_))    \
    ((exposedfield, sfvec2f, "scale",       scale_))       \
    ((exposedfield, sfvec2f, "translation", translation_)) \
    ((exposedfield, sfnode,  "metadata",    metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              texture_transform_metatype,
                                              texture_transform_node,
                                              TEXTURE_TRANSFORM_INTERFACE_SEQ)
