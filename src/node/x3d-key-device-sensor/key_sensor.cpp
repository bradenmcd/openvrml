// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2009  Braden McDaniel
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

# define KEY_SENSOR_INTERFACE_SEQ                                       \
    ((exposedfield, sfnode,   "metadata",         metadata))            \
    ((exposedfield, sfbool,   "enabled",          enabled_))            \
    ((eventout,     sfint32,  "actionKeyPress",   action_key_press_emitter_)) \
    ((eventout,     sfint32,  "actionKeyRelease", action_key_release_emitter_)) \
    ((eventout,     sfbool,   "altKey",           alt_key_emitter_))    \
    ((eventout,     sfbool,   "controlKey",       control_key_emitter_)) \
    ((eventout,     sfbool,   "isActive",         is_active_emitter_))  \
    ((eventout,     sfstring, "keyPress",         key_press_emitter_))  \
    ((eventout,     sfstring, "keyRelease",       key_release_emitter_)) \
    ((eventout,     sfbool,   "shiftKey",         shift_key_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(
    openvrml_node_x3d_key_device_sensor,
    key_sensor_metatype,
    key_sensor_node,
    KEY_SENSOR_INTERFACE_SEQ)
