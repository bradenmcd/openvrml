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

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating StringSensor nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c string_sensor_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_key_device_sensor::string_sensor_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 6> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "deletionAllowed"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::eventout_id,
                       field_value::sfstring_id,
                       "enteredText"),
        node_interface(node_interface::eventout_id,
                       field_value::sfstring_id,
                       "finalText"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive")
    };    
    typedef node_type_impl<string_sensor_node> node_type_t;

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
                &string_sensor_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &string_sensor_node::deletion_allowed_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &string_sensor_node::enabled_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &string_sensor_node::entered_text_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &string_sensor_node::final_text_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &string_sensor_node::is_active_emitter_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }        
    return type;
}
