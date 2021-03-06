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

# include "boolean_sequencer.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    /**
     * @brief Represents BooleanSequencer node instances.
     */
    class OPENVRML_LOCAL boolean_sequencer_node :
        public openvrml::node_impl_util::abstract_node<boolean_sequencer_node>,
        public openvrml::child_node {

        friend
        class openvrml_node_x3d_event_utilities::boolean_sequencer_metatype;

        class next_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sfbool_listener {
        public:
            explicit next_listener(self_t & node);
            virtual ~next_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class previous_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sfbool_listener {
        public:
            explicit previous_listener(self_t & node);
            virtual ~previous_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_fraction_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_fraction_listener(self_t & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        next_listener next_listener_;
        previous_listener previous_listener_;
        set_fraction_listener set_fraction_listener_;
        exposedfield<openvrml::mffloat> key_;
        exposedfield<openvrml::mfbool> key_value_;
        openvrml::sfbool value_changed_;
        sfbool_emitter value_changed_emitter_;

    public:
        boolean_sequencer_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~boolean_sequencer_node() OPENVRML_NOTHROW;
    };


    /**
     * @var boolean_sequencer_node::boolean_sequencer_metatype
     *
     * @brief Class object for BooleanSequencer nodes.
     */

    /**
     * @var boolean_sequencer_node::next_listener boolean_sequencer_node::next_listener_
     *
     * @brief next eventIn listener.
     */

    /**
     * @var boolean_sequencer_node::previous_listener boolean_sequencer_node::previous_listener_
     *
     * @brief previous eventIn listener.
     */

    /**
     * @var boolean_sequencer_node::set_fraction_listener boolean_sequencer_node::set_fraction_listener_
     *
     * @brief set_fraction eventIn listener.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<boolean_sequencer_node>::exposedfield<openvrml::mffloat> boolean_sequencer_node::key_
     *
     * @brief key exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<boolean_sequencer_node>::exposedfield<openvrml::mfbool> boolean_sequencer_node::key_value_
     *
     * @brief key_value exposedField
     */

    /**
     * @var openvrml::sfbool boolean_sequencer_node::value_changed_
     *
     * @brief value_changed eventOut
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<boolean_sequencer_node>::sfbool_emitter boolean_sequencer_node::value_changed_emitter_
     *
     * @brief value_changed eventOut emitter.
     */

    boolean_sequencer_node::next_listener::
    next_listener(self_t & node):
        node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    boolean_sequencer_node::next_listener::
    ~next_listener() OPENVRML_NOTHROW
    {}

    void boolean_sequencer_node::next_listener::
    do_process_event(const openvrml::sfbool & /* fraction */,
                     const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    boolean_sequencer_node::previous_listener::
    previous_listener(self_t & node):
        node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    boolean_sequencer_node::previous_listener::
    ~previous_listener() OPENVRML_NOTHROW
    {}

    void boolean_sequencer_node::previous_listener::
    do_process_event(const openvrml::sfbool & /* fraction */,
                     const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    boolean_sequencer_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    boolean_sequencer_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void boolean_sequencer_node::set_fraction_listener::
    do_process_event(const openvrml::sffloat & /* fraction */, const double /* timestamp */)
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
    boolean_sequencer_node::
    boolean_sequencer_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<self_t>(type, scope),
        child_node(type, scope),
        next_listener_(*this),
        previous_listener_(*this),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    boolean_sequencer_node::~boolean_sequencer_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_event_utilities::boolean_sequencer_metatype::id =
    "urn:X-openvrml:node:BooleanSequencer";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c boolean_sequencer_metatype.
 */
openvrml_node_x3d_event_utilities::boolean_sequencer_metatype::
boolean_sequencer_metatype(openvrml::browser & browser):
    node_metatype(boolean_sequencer_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_event_utilities::boolean_sequencer_metatype::
~boolean_sequencer_metatype()
    OPENVRML_NOTHROW
{}

# define BOOLEAN_SEQUENCER_INTERFACE_SEQ                               \
    ((exposedfield, sfnode,  "metadata",      metadata))               \
    ((eventin,      sfbool,  "next",          next_listener_))         \
    ((eventin,      sfbool,  "previous",      previous_listener_))     \
    ((eventin,      sffloat, "set_fraction",  set_fraction_listener_)) \
    ((exposedfield, mffloat, "key",           key_))                   \
    ((exposedfield, mfbool,  "keyValue",      key_value_))             \
    ((eventout,     sfbool,  "value_changed", value_changed_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_event_utilities,
                                              boolean_sequencer_metatype,
                                              boolean_sequencer_node,
                                              BOOLEAN_SEQUENCER_INTERFACE_SEQ)
