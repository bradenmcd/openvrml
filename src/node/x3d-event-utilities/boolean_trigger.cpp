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

# include "boolean_trigger.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    /**
     * @brief Represents BooleanTrigger node instances.
     */
    class OPENVRML_LOCAL boolean_trigger_node :
        public openvrml::node_impl_util::abstract_node<boolean_trigger_node>,
        public openvrml::child_node {

        friend
        class openvrml_node_x3d_event_utilities::boolean_trigger_metatype;

        class set_trigger_time_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sftime_listener {
        public:
            explicit set_trigger_time_listener(self_t & node);
            virtual ~set_trigger_time_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sftime & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_trigger_time_listener set_trigger_time_listener_;
        openvrml::sfbool trigger_true_;
        sfbool_emitter trigger_true_emitter_;

    public:
        boolean_trigger_node(const openvrml::node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~boolean_trigger_node() OPENVRML_NOTHROW;
    };


    /**
     * @var boolean_trigger_node::boolean_trigger_metatype
     *
     * @brief Class object for BooleanTrigger nodes.
     */

    /**
     * @var boolean_trigger_node::set_trigger_time_listener boolean_trigger_node::set_trigger_time_listener_
     *
     * @brief set_trigger_time eventIn listener.
     */

    /**
     * @var openvrml::sfbool boolean_trigger_node::trigger_true_
     *
     * @brief trigger_true eventOut.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<boolean_trigger_node>::sfbool_emitter boolean_trigger_node::trigger_true_emitter_
     *
     * @brief trigger_true eventOut emitter.
     */

    boolean_trigger_node::set_trigger_time_listener::
    set_trigger_time_listener(self_t & node):
        node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<self_t>(node),
        sftime_listener(node)
    {}

    boolean_trigger_node::set_trigger_time_listener::
    ~set_trigger_time_listener() OPENVRML_NOTHROW
    {}

    void boolean_trigger_node::set_trigger_time_listener::
    do_process_event(const openvrml::sftime & /* fraction */,
                     const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    boolean_trigger_node::
    boolean_trigger_node(const openvrml::node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_trigger_time_listener_(*this),
        trigger_true_emitter_(*this, this->trigger_true_)
    {}

    /**
     * @brief Destroy.
     */
    boolean_trigger_node::~boolean_trigger_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_event_utilities::boolean_trigger_metatype::id =
    "urn:X-openvrml:node:BooleanTrigger";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c boolean_trigger_metatype.
 */
openvrml_node_x3d_event_utilities::boolean_trigger_metatype::
boolean_trigger_metatype(openvrml::browser & browser):
    node_metatype(boolean_trigger_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_event_utilities::boolean_trigger_metatype::
~boolean_trigger_metatype()
    OPENVRML_NOTHROW
{}

# define BOOLEAN_TRIGGER_INTERFACE_SEQ                                  \
    ((exposedfield, sfnode, "metadata",        metadata))               \
    ((eventin,      sftime, "set_triggerTime", set_trigger_time_listener_)) \
    ((eventout,     sfbool, "triggerTrue",     trigger_true_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_event_utilities,
                                              boolean_trigger_metatype,
                                              boolean_trigger_node,
                                              BOOLEAN_TRIGGER_INTERFACE_SEQ)
