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

# include "key_sensor.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    class OPENVRML_LOCAL key_sensor_node :
        public abstract_node<key_sensor_node>,
        public child_node {

        friend class openvrml_node_x3d_key_device_sensor::key_sensor_metatype;

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
        virtual ~key_sensor_node() OPENVRML_NOTHROW;
    };

    /**
     * @class key_sensor_node
     *
     * @brief Represents KeySensor node instances.
     */

    /**
     * @var key_sensor_node::key_sensor_metatype
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
     * @param type  the @c node_type associated with this @c node.
     * @param scope the @c scope to which the @c node belongs.
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
    key_sensor_node::~key_sensor_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_key_device_sensor::key_sensor_metatype::id =
    "urn:X-openvrml:node:KeySensor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c key_sensor_metatype.
 */
openvrml_node_x3d_key_device_sensor::key_sensor_metatype::
key_sensor_metatype(openvrml::browser & browser):
    node_metatype(key_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_key_device_sensor::key_sensor_metatype::
~key_sensor_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating KeySensor nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c key_sensor_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_key_device_sensor::key_sensor_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
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

    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::enabled_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::action_key_press_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::action_key_release_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::alt_key_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::control_key_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::is_active_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::key_press_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::key_release_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &key_sensor_node::shift_key_emitter_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
