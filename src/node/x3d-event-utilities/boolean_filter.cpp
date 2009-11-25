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

# include "boolean_filter.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    /**
     * @brief Represents BooleanFilter node instances.
     */
    class OPENVRML_LOCAL boolean_filter_node :
        public openvrml::node_impl_util::abstract_node<boolean_filter_node>,
        public openvrml::child_node {

        friend class openvrml_node_x3d_event_utilities::boolean_filter_metatype;

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
        openvrml::sfbool input_false_;
        sfbool_emitter input_false_emitter_;
        openvrml::sfbool input_negate_;
        sfbool_emitter input_negate_emitter_;
        openvrml::sfbool input_true_;
        sfbool_emitter input_true_emitter_;

    public:
        boolean_filter_node(const openvrml::node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~boolean_filter_node() OPENVRML_NOTHROW;
    };


    /**
     * @var boolean_filter_node::boolean_filter_metatype
     *
     * @brief Class object for BooleanFilter nodes.
     */

    /**
     * @var boolean_filter_node::set_boolean_listener boolean_filter_node::set_boolean_listener_
     *
     * @brief set_boolean eventIn listener.
     */

    /**
     * @var openvrml::sfbool boolean_filter_node::input_false_
     *
     * @brief input_false eventOut
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<boolean_filter_node>::sfbool_emitter boolean_filter_node::input_false_emitter_
     *
     * @brief input_false eventOut emitter.
     */

    /**
     * @var openvrml::sfbool boolean_filter_node::input_negate_
     *
     * @brief input_negate eventOut
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<boolean_filter_node>::sfbool_emitter boolean_filter_node::input_negate_emitter_
     *
     * @brief input_negate eventOut emitter.
     */

    /**
     * @var openvrml::sfbool boolean_filter_node::input_true_
     *
     * @brief input_true eventOut
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<boolean_filter_node>::sfbool_emitter boolean_filter_node::input_true_emitter_
     *
     * @brief input_true eventOut emitter.
     */

    boolean_filter_node::set_boolean_listener::
    set_boolean_listener(self_t & node):
        node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    boolean_filter_node::set_boolean_listener::
    ~set_boolean_listener() OPENVRML_NOTHROW
    {}

    void boolean_filter_node::set_boolean_listener::
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
     * @param scope the scope to which the node belongs.
     */
    boolean_filter_node::
    boolean_filter_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_boolean_listener_(*this),
        input_false_emitter_(*this, this->input_false_),
        input_negate_emitter_(*this, this->input_negate_),
        input_true_emitter_(*this, this->input_true_)
    {}

    /**
     * @brief Destroy.
     */
    boolean_filter_node::~boolean_filter_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_event_utilities::boolean_filter_metatype::id =
    "urn:X-openvrml:node:BooleanFilter";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c boolean_filter_metatype.
 */
openvrml_node_x3d_event_utilities::boolean_filter_metatype::
boolean_filter_metatype(openvrml::browser & browser):
    node_metatype(boolean_filter_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_event_utilities::boolean_filter_metatype::
~boolean_filter_metatype()
    OPENVRML_NOTHROW
{}

# define BOOLEAN_FILTER_INTERFACE_SEQ                                \
    ((exposedfield, sfnode, "metadata",      metadata))              \
    ((eventin,      sfbool, "set_boolean",   set_boolean_listener_)) \
    ((eventout,     sfbool, "inputFalse",    input_false_emitter_))  \
    ((eventout,     sfbool, "inputNegate",   input_negate_emitter_)) \
    ((eventout,     sfbool, "inputTrue",     input_true_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_event_utilities,
                                              boolean_filter_metatype,
                                              boolean_filter_node,
                                              BOOLEAN_FILTER_INTERFACE_SEQ)
