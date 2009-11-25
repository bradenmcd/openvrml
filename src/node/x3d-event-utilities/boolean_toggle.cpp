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

# include "boolean_toggle.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    /**
     * @brief Represents BooleanToggle node instances.
     */
    class OPENVRML_LOCAL boolean_toggle_node :
        public openvrml::node_impl_util::abstract_node<boolean_toggle_node>,
        public openvrml::child_node {

        friend class openvrml_node_x3d_event_utilities::boolean_toggle_metatype;

        class set_boolean_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sfbool_listener {
        public:
            explicit set_boolean_listener(self_t & node);
            virtual ~set_boolean_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_boolean_listener set_boolean_listener_;
        exposedfield<openvrml::sfbool> toggle_;

    public:
        boolean_toggle_node(const openvrml::node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~boolean_toggle_node() OPENVRML_NOTHROW;
    };


    /**
     * @var boolean_toggle_node::boolean_toggle_metatype
     *
     * @brief Class object for BooleanToggle nodes.
     */

    /**
     * @var boolean_toggle_node::set_boolean_listener boolean_toggle_node::set_boolean_listener_
     *
     * @brief set_boolean eventIn listener.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<boolean_sequencer_node>::exposedfield<openvrml::mfbool> boolean_toggle_node::toggle_
     *
     * @brief toggle exposedField.
     */

    boolean_toggle_node::set_boolean_listener::
    set_boolean_listener(self_t & node):
        node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    boolean_toggle_node::set_boolean_listener::
    ~set_boolean_listener() OPENVRML_NOTHROW
    {}

    void boolean_toggle_node::set_boolean_listener::
    do_process_event(const openvrml::sfbool & /* fraction */,
                     const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    boolean_toggle_node::
    boolean_toggle_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_boolean_listener_(*this),
        toggle_(*this)
    {}

    /**
     * @brief Destroy.
     */
    boolean_toggle_node::~boolean_toggle_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_event_utilities::boolean_toggle_metatype::id =
    "urn:X-openvrml:node:BooleanToggle";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with
 *                @c this boolean_toggle_metatype.
 */
openvrml_node_x3d_event_utilities::boolean_toggle_metatype::
boolean_toggle_metatype(openvrml::browser & browser):
    node_metatype(boolean_toggle_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_event_utilities::boolean_toggle_metatype::
~boolean_toggle_metatype()
     OPENVRML_NOTHROW
{}

# define BOOLEAN_TOGGLE_INTERFACE_SEQ                              \
    ((exposedfield, sfnode, "metadata",    metadata))              \
    ((eventin,      sfbool, "set_boolean", set_boolean_listener_)) \
    ((exposedfield, sfbool, "toggle",      toggle_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_event_utilities,
                                              boolean_toggle_metatype,
                                              boolean_toggle_node,
                                              BOOLEAN_TOGGLE_INTERFACE_SEQ)
