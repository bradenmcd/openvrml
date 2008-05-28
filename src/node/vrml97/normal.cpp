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
# include "normal.h"

namespace {

    class OPENVRML_LOCAL normal_node :
        public openvrml::node_impl_util::abstract_node<normal_node>,
        public openvrml::normal_node {

        friend class openvrml_node_vrml97::normal_metatype;

        exposedfield<openvrml::mfvec3f> vector_;

    public:
        normal_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~normal_node() OPENVRML_NOTHROW;

        //
        // normal_node implementation.
        //
        virtual const std::vector<openvrml::vec3f> & vector() const
            OPENVRML_NOTHROW;
    };

    /**
     * @class normal_node
     *
     * @brief Normal node instances.
     */

    /**
     * @var class normal_node::normal_metatype
     *
     * @brief Class object for Normal nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<normal_node>::exposedfield<openvrml::mfvec3f> normal_node::vector_
     *
     * @brief vector exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope     the scope to which the node belongs.
     */
    normal_node::
    normal_node(const openvrml::node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml::node_impl_util::abstract_node<normal_node>(type, scope),
        openvrml::normal_node(type, scope),
        vector_(*this)
    {}

    /**
     * @brief Destroy.
     */
    normal_node::~normal_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the array of normal vectors.
     *
     * @return the array of normal vectors.
     */
    const std::vector<openvrml::vec3f> &
    normal_node::vector() const OPENVRML_NOTHROW
    {
        return this->vector_.mfvec3f::value();
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::normal_metatype::id =
    "urn:X-openvrml:node:Normal";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::normal_metatype::
normal_metatype(openvrml::browser & browser):
    node_metatype(normal_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::normal_metatype::~normal_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Normal nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by normal_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::normal_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using openvrml::field_value;
    using openvrml::sfnode;
    using openvrml::mfvec3f;
    using openvrml::node_interface;
    using openvrml::node_interface_set;
    using openvrml::node_type;
    using openvrml::unsupported_interface;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 2> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "vector"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_type_impl<normal_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & normalNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            normalNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<normal_node>::exposedfield<mfvec3f> >(
                        &normal_node::vector_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<normal_node>::exposedfield<mfvec3f> >(
                        &normal_node::vector_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<normal_node>::exposedfield<mfvec3f> >(
                        &normal_node::vector_)));
        } else if (*interface_ == *++supported_interface) {
            normalNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<normal_node>::exposedfield<sfnode> >(
                        &normal_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<normal_node>::exposedfield<sfnode> >(
                        &normal_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<normal_node>::exposedfield<sfnode> >(
                        &normal_node::metadata)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
