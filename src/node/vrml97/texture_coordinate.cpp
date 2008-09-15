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

        //
        // texture_coordinate_node implementation.
        //
        virtual const std::vector<openvrml::vec2f> & point() const OPENVRML_NOTHROW;
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
    texture_coordinate_node::point() const OPENVRML_NOTHROW
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

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating
 *         TextureCoordinate nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  texture_coordinate_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::texture_coordinate_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using openvrml::field_value;
    using openvrml::sfnode;
    using openvrml::mfvec2f;
    using openvrml::node_interface;
    using openvrml::node_interface_set;
    using openvrml::node_type;
    using openvrml::unsupported_interface;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 2> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec2f_id,
                       "point"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_type_impl<texture_coordinate_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & textureCoordinateNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            textureCoordinateNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_coordinate_node::point_);
    } else if (*interface_ == *++supported_interface) {
            textureCoordinateNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_coordinate_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
}
    return type;
}
