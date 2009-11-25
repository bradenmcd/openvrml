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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "texture_coordinate.h"

namespace {

    class OPENVRML_LOCAL texture_coordinate_node :
        public openvrml::node_impl_util::abstract_node<texture_coordinate_node>,
        public openvrml::texture_coordinate_node {

        friend class openvrml_node_vrml97::texture_coordinate_metatype;

        exposedfield<openvrml::mfvec2f> point_;

    public:
        texture_coordinate_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~texture_coordinate_node() OPENVRML_NOTHROW;

    private:
        //
        // texture_coordinate_node implementation.
        //
        virtual const std::vector<openvrml::vec2f> & do_point() const
            OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL texture_transform_node :
        public openvrml::node_impl_util::abstract_node<texture_transform_node>,
        public openvrml::texture_transform_node {

        friend class texture_transform_metatype;

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
     * @class texture_coordinate_node
     *
     * @brief TextureCoordinate node instances.
     */

    /**
     * @var class texture_coordinate_node::texture_coordinate_metatype
     *
     * @brief Class object for TextureCoordinate instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<texture_coordinate_node>::exposedfield<openvrml::mfvec2f> texture_coordinate_node::point_
     *
     * @brief point exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope that the new node will belong to.
     */
    texture_coordinate_node::
    texture_coordinate_node(const openvrml::node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml::node_impl_util::abstract_node<texture_coordinate_node>(type, scope),
        openvrml::texture_coordinate_node(type, scope),
        point_(*this)
    {}

    /**
     * @brief Destroy.
     */
    texture_coordinate_node::~texture_coordinate_node()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the points encapsulated by this node.
     *
     * @return the mfvec2f array of points for this node.
     */
    const std::vector<openvrml::vec2f> &
    texture_coordinate_node::do_point() const OPENVRML_NOTHROW
    {
        return this->point_.mfvec2f::value();
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::texture_coordinate_metatype::id =
    "urn:X-openvrml:node:TextureCoordinate";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::texture_coordinate_metatype::
texture_coordinate_metatype(openvrml::browser & browser):
    node_metatype(texture_coordinate_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::texture_coordinate_metatype::
~texture_coordinate_metatype() OPENVRML_NOTHROW
{}

# define TEXTURE_COORDINATE_INTERFACE_SEQ           \
    ((exposedfield, mfvec2f, "point",    point_))   \
    ((exposedfield, sfnode,  "metadata", metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              texture_coordinate_metatype,
                                              texture_coordinate_node,
                                              TEXTURE_COORDINATE_INTERFACE_SEQ)
