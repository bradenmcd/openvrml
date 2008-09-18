// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
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

        //
        // color_rgba_node implementation
        //
        const std::vector<openvrml::color_rgba> & color_rgba() const
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
    color_rgba_node::color_rgba() const OPENVRML_NOTHROW
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

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating ColorRGBA nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by color_rgba_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_rendering::color_rgba_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using openvrml::field_value;
    using openvrml::node_interface;
    using openvrml::node_interface_set;
    using openvrml::node_type;
    using openvrml::unsupported_interface;
    typedef boost::array<node_interface, 2> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolorrgba_id,
                       "color")
    };    
    typedef node_type_impl<color_rgba_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & the_node_type = static_cast<node_type_t &>(*type);
        
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &color_rgba_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &color_rgba_node::color_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }        
    return type;
}
