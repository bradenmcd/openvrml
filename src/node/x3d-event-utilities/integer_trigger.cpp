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

# include "integer_trigger.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL integer_trigger_node :
        public openvrml::node_impl_util::abstract_node<integer_trigger_node>,
        public openvrml::child_node {

        friend
        class openvrml_node_x3d_event_utilities::integer_trigger_metatype;

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
        exposedfield<openvrml::mfint32> integer_key_;
        openvrml::sfint32 trigger_value_;
        sfint32_emitter trigger_value_emitter_;

    public:
        integer_trigger_node(const openvrml::node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~integer_trigger_node() OPENVRML_NOTHROW;
    };


    /**
     * @class integer_trigger_node
     *
     * @brief Represents IntegerTrigger node instances.
     */

    /**
     * @var integer_trigger_node::integer_trigger_metatype
     *
     * @brief Class object for IntegerTrigger nodes.
     */

    /**
     * @var integer_trigger_node::set_boolean_listener integer_trigger_node::set_boolean_listener_
     *
     * @brief set_boolean eventIn listener.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<integer_trigger_node>::exposedfield<openvrml::mfint32> integer_trigger_node::integer_key_
     *
     * @brief integer_key exposedField
     */

    /**
     * @var openvrml::sfint32 integer_trigger_node::trigger_value_
     *
     * @brief trigger_value eventOut
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<integer_trigger_node>::sfint32_emitter integer_trigger_node::trigger_value_emitter_
     *
     * @brief trigger_value eventOut emitter.
     */

    integer_trigger_node::set_boolean_listener::
    set_boolean_listener(self_t & node):
        node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    integer_trigger_node::set_boolean_listener::
    ~set_boolean_listener() OPENVRML_NOTHROW
    {}

    void integer_trigger_node::set_boolean_listener::
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
    integer_trigger_node::
    integer_trigger_node(const openvrml::node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_boolean_listener_(*this),
        integer_key_(*this),
        trigger_value_emitter_(*this, this->trigger_value_)
    {}

    /**
     * @brief Destroy.
     */
    integer_trigger_node::~integer_trigger_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_event_utilities::integer_trigger_metatype::id =
    "urn:X-openvrml:node:IntegerTrigger";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c integer_trigger_metatype.
 */
openvrml_node_x3d_event_utilities::integer_trigger_metatype::
integer_trigger_metatype(openvrml::browser & browser):
    node_metatype(integer_trigger_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_event_utilities::integer_trigger_metatype::
~integer_trigger_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating IntegerTrigger nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c integer_trigger_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_event_utilities::integer_trigger_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 4> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sfbool_id,
                       "set_boolean"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "integerKey"),
        node_interface(node_interface::eventout_id,
                       field_value::sfint32_id,
                       "triggerValue")
    };
    typedef node_type_impl<integer_trigger_node> node_type_t;

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
                &integer_trigger_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &integer_trigger_node::set_boolean_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &integer_trigger_node::integer_key_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &integer_trigger_node::trigger_value_emitter_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
