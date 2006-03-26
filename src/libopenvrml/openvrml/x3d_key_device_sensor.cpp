// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2006  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <private.h>
# include "browser.h"
# include "node_impl_util.h"
# include "x3d_key_device_sensor.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for KeySensor nodes.
     */
    class OPENVRML_LOCAL key_sensor_class : public node_class {
    public:
        explicit key_sensor_class(openvrml::browser & browser);
        virtual ~key_sensor_class() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };
}

void register_key_device_sensor_node_classes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_class;
    b.add_node_class("urn:X-openvrml:node:KeySensor",
                     shared_ptr<node_class>(new key_sensor_class(b)));
}

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL key_sensor_node : public abstract_node<key_sensor_node>,
                                           public child_node {
        friend class key_sensor_class;

        exposedfield<sfbool> enabled_;
        sfint32 action_key_press_;
        sfint32_emitter action_key_press_emitter_;
        sfint32 action_key_release_;
        sfint32_emitter action_key_release_emitter_;
        sfbool alt_key_;
        sfbool_emitter alt_key_emitter_;
        sfbool control_key_;
        sfbool_emitter control_key_emitter_;
        sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        sfstring key_press_;
        sfstring_emitter key_press_emitter_;
        sfstring key_release_;
        sfstring_emitter key_release_emitter_;
        sfbool shift_key_;
        sfbool_emitter shift_key_emitter_;

    public:
        key_sensor_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~key_sensor_node() throw ();
    };

    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this key_sensor_class.
     */
    key_sensor_class::key_sensor_class(openvrml::browser & browser):
        node_class(browser)
    {}

    /**
     * @brief Destroy.
     */
    key_sensor_class::~key_sensor_class() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating KeySensor nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by key_sensor_class.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    key_sensor_class::do_create_type(const std::string & id,
                                     const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 10> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::eventout_id,
                           field_value::sfint32_id,
                           "actionKeyPress"),
            node_interface(node_interface::eventout_id,
                           field_value::sfint32_id,
                           "actionKeyRelease"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "altKey"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "controlKey"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfstring_id,
                           "keyPress"),
            node_interface(node_interface::eventout_id,
                           field_value::sfstring_id,
                           "keyRelease"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "shiftKey")
        };
        typedef node_type_impl<key_sensor_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<key_sensor_node>::exposedfield<sfnode> >(
                            &key_sensor_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<key_sensor_node>::exposedfield<sfnode> >(
                            &key_sensor_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<key_sensor_node>::exposedfield<sfnode> >(
                            &key_sensor_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<key_sensor_node>::exposedfield<sfbool> >(
                            &key_sensor_node::enabled_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<key_sensor_node>::exposedfield<sfbool> >(
                            &key_sensor_node::enabled_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<key_sensor_node>::exposedfield<sfbool> >(
                            &key_sensor_node::enabled_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        key_sensor_node::sfint32_emitter>(
                            &key_sensor_node::action_key_press_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        key_sensor_node::sfint32_emitter>(
                            &key_sensor_node::action_key_release_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        key_sensor_node::sfbool_emitter>(
                            &key_sensor_node::alt_key_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        key_sensor_node::sfbool_emitter>(
                            &key_sensor_node::control_key_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        key_sensor_node::sfbool_emitter>(
                            &key_sensor_node::is_active_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        key_sensor_node::sfstring_emitter>(
                            &key_sensor_node::key_press_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        key_sensor_node::sfstring_emitter>(
                            &key_sensor_node::key_release_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        key_sensor_node::sfbool_emitter>(
                            &key_sensor_node::shift_key_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @class key_sensor_node
     *
     * @brief Represents KeySensor node instances.
     */

    /**
     * @var key_sensor_node::key_sensor_class
     *
     * @brief Class object for KeySensor nodes.
     */

    /**
     * @var key_sensor_node::enabled_
     *
     * @brief enabled exposedField
     */

    /**
     * @var key_sensor_node::action_key_press_
     *
     * @brief action_key_press eventOut
     */

    /**
     * @var key_sensor_node::action_key_release_
     *
     * @brief action_key_release eventOut
     */

    /**
     * @var key_sensor_node::alt_key_
     *
     * @brief alt_key eventOut
     */

    /**
     * @var key_sensor_node::control_key_
     *
     * @brief control_key eventOut
     */

    /**
     * @var key_sensor_node::is_active_
     *
     * @brief is_active eventOut
     */

    /**
     * @var key_sensor_node::key_press_
     *
     * @brief key_press eventOut
     */

    /**
     * @var key_sensor_node::key_release_
     *
     * @brief key_release eventOut
     */

    /**
     * @var key_sensor_node::shift_key_
     *
     * @brief shift_key eventOut
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    key_sensor_node::
    key_sensor_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        enabled_(*this, true),
        action_key_press_emitter_(*this, this->action_key_press_),
        action_key_release_emitter_(*this, this->action_key_release_),
        alt_key_emitter_(*this, this->alt_key_),
        control_key_emitter_(*this, this->control_key_),
        is_active_emitter_(*this, this->is_active_),
        key_press_emitter_(*this, this->key_press_),
        key_release_emitter_(*this, this->key_release_),
        shift_key_emitter_(*this, this->shift_key_)
    {}

    /**
     * @brief Destroy.
     */
    key_sensor_node::~key_sensor_node() throw ()
    {}
}
