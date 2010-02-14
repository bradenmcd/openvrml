// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007  Braden McDaniel
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

# include "multi_texture.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Represents MultiTexture node instances.
     */
    class OPENVRML_LOCAL multi_texture_node :
        public abstract_node<multi_texture_node> {

        friend class openvrml_node_x3d_texturing::multi_texture_metatype;

        exposedfield<sffloat> alpha_;
        exposedfield<sfcolor> color_;
        exposedfield<mfstring> function_;
        exposedfield<mfstring> mode_;
        exposedfield<mfstring> source_;
        exposedfield<mfnode> texture_;

    public:
        multi_texture_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~multi_texture_node() OPENVRML_NOTHROW;
    };


    /**
     * @var multi_texture_node::MultiTexture_metatype
     *
     * @brief Class object for MultiTexture nodes.
     */

    /**
     * @var multi_texture_node::alpha_
     *
     * @brief alpha exposedField
     */

    /**
     * @var multi_texture_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var multi_texture_node::function_
     *
     * @brief function exposedField
     */

    /**
     * @var multi_texture_node::mode_
     *
     * @brief mode exposedField
     */

    /**
     * @var multi_texture_node::source_
     *
     * @brief source exposedField
     */

    /**
     * @var multi_texture_node::texture_
     *
     * @brief texture exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    multi_texture_node::
    multi_texture_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        alpha_(*this, 1),
        color_(*this, make_color(1.0, 1.0, 1.0)),
        function_(*this),
        mode_(*this),
        source_(*this),
        texture_(*this)
    {}

    /**
     * @brief Destroy.
     */
    multi_texture_node::~multi_texture_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_texturing::multi_texture_metatype::id =
    "urn:X-openvrml:node:MultiTexture";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c multi_texture_metatype.
 */
openvrml_node_x3d_texturing::multi_texture_metatype::
multi_texture_metatype(openvrml::browser & browser):
    node_metatype(multi_texture_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_texturing::multi_texture_metatype::~multi_texture_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating MultiTexture nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c multi_texture_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_texturing::multi_texture_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 7> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "alpha"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "function"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "mode"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "source"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "texture")
    };
    typedef node_type_impl<multi_texture_node> node_type_t;

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
                &multi_texture_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &multi_texture_node::alpha_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &multi_texture_node::color_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &multi_texture_node::function_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &multi_texture_node::mode_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &multi_texture_node::source_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &multi_texture_node::texture_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
