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

# include "string_sensor.h"
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
     * @brief Represents StringSensor node instances.
     */
    class OPENVRML_LOCAL string_sensor_node :
        public abstract_node<string_sensor_node>,
        public child_node {

        friend
        class openvrml_node_x3d_key_device_sensor::string_sensor_metatype;

        exposedfield<sfbool> deletion_allowed_;
        exposedfield<sfbool> enabled_;
        sfstring entered_text_;
        sfstring_emitter entered_text_emitter_;
        sfstring final_text_;
        sfstring_emitter final_text_emitter_;
        sfbool is_active_;
        sfbool_emitter is_active_emitter_;

    public:
        string_sensor_node(const node_type & type, 
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~string_sensor_node() OPENVRML_NOTHROW;
    };


    /**
     * @var string_sensor_node::string_sensor_metatype
     *
     * @brief Class object for StringSensor nodes.
     */

    /**
     * @var string_sensor_node::deletion_allowed_
     *
     * @brief deletion_allowed exposedField
     */

    /**
     * @var string_sensor_node::enabled_
     *
     * @brief enabled exposedField
     */

    /**
     * @var string_sensor_node::entered_text_
     *
     * @brief entered_text eventOut
     */

    /**
     * @var string_sensor_node::final_text_
     *
     * @brief final_text eventOut
     */

    /**
     * @var string_sensor_node::is_active_
     *
     * @brief is_active eventOut
     */

    
    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this @c node.
     * @param scope the @c scope to which the @c node belongs.
     */
    string_sensor_node::
    string_sensor_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        deletion_allowed_(*this, true),
        enabled_(*this, true),
        entered_text_emitter_(*this, this->entered_text_),
        final_text_emitter_(*this, this->final_text_),
        is_active_emitter_(*this, this->is_active_)
    {}

    /**
     * @brief Destroy.
     */
    string_sensor_node::~string_sensor_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_key_device_sensor::string_sensor_metatype::id =
    "urn:X-openvrml:node:StringSensor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c string_sensor_metatype.
 */
openvrml_node_x3d_key_device_sensor::string_sensor_metatype::
string_sensor_metatype(openvrml::browser & browser):
    node_metatype(string_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_key_device_sensor::string_sensor_metatype::
~string_sensor_metatype()
    OPENVRML_NOTHROW
{}

# define STRING_SENSOR_INTERFACE_SEQ                                    \
    ((exposedfield, sfnode,   "metadata",        metadata))             \
    ((exposedfield, sfbool,   "deletionAllowed", deletion_allowed_))    \
    ((exposedfield, sfbool,   "enabled",         enabled_))             \
    ((eventout,     sfstring, "enteredText",     entered_text_emitter_)) \
    ((eventout,     sfstring, "finalText",       final_text_emitter_))  \
    ((eventout,     sfbool,   "isActive",        is_active_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(
    openvrml_node_x3d_key_device_sensor,
    string_sensor_metatype,
    string_sensor_node,
    STRING_SENSOR_INTERFACE_SEQ)
