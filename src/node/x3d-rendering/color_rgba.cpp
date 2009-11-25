// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008, 2009  Braden McDaniel
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

# include "color_rgba.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml::node_impl_util;
using namespace std;


namespace {

    /**
     * @brief Represents ColorRGBA node instances.
     */
    class OPENVRML_LOCAL color_rgba_node :
        public abstract_node<color_rgba_node>,
        public openvrml::color_rgba_node {

        friend class openvrml_node_x3d_rendering::color_rgba_metatype;

        exposedfield<openvrml::mfcolorrgba> color_;

    public:
        color_rgba_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~color_rgba_node() OPENVRML_NOTHROW;

    private:
        //
        // color_rgba_node implementation
        //
        const std::vector<openvrml::color_rgba> & do_color_rgba() const
            OPENVRML_NOTHROW;
    };


    /**
     * @var color_rgba_node::ColorRGBA_metatype
     *
     * @brief Class object for ColorRGBA nodes.
     */

    /**
     * @var color_rgba_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    color_rgba_node::
    color_rgba_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        openvrml::color_rgba_node(type, scope),
        color_(*this)
    {}

    /**
     * @brief Destroy.
     */
    color_rgba_node::~color_rgba_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the color_rgba array.
     *
     * @return the color array associated with the node.
     */
    const std::vector<openvrml::color_rgba> &
    color_rgba_node::do_color_rgba() const OPENVRML_NOTHROW
    {
        return this->color_.openvrml::mfcolorrgba::value();
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_rendering::color_rgba_metatype::id =
    "urn:X-openvrml:node:ColorRGBA";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c color_rgba_metatype.
 */
openvrml_node_x3d_rendering::color_rgba_metatype::
color_rgba_metatype(openvrml::browser & browser):
    node_metatype(color_rgba_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_rendering::color_rgba_metatype::~color_rgba_metatype()
    OPENVRML_NOTHROW
{}

# define COLOR_RGBA_INTERFACE_SEQ \
    ((exposedfield, sfnode,      "metadata", metadata)) \
    ((exposedfield, mfcolorrgba, "color",    color_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_rendering,
                                              color_rgba_metatype,
                                              color_rgba_node,
                                              COLOR_RGBA_INTERFACE_SEQ)
