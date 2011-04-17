// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007  Braden McDaniel
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

# include "load_sensor.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;

namespace {

    /**
     * @brief Represents LoadSensor node instances.
     */
    class OPENVRML_LOCAL load_sensor_node :
        public abstract_node<load_sensor_node>,
        public child_node {

        friend class openvrml_node_x3d_networking::load_sensor_metatype;

        exposedfield<sfbool> enabled_;
        exposedfield<sftime> timeout_;
        exposedfield<mfnode> watch_list_;
        sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        sfbool is_loaded_;
        sfbool_emitter is_loaded_emitter_;
        sftime load_time_;
        sftime_emitter load_time_emitter_;
        sffloat progress_;
        sffloat_emitter progress_emitter_;

    public:
        load_sensor_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~load_sensor_node() OPENVRML_NOTHROW;
    };


    /**
     * @var load_sensor_node::load_sensor_metatype
     *
     * @brief Class object for LoadSensor nodes.
     */

    /**
     * @var load_sensor_node::enabled_
     *
     * @brief enabled exposedField
     */

    /**
     * @var load_sensor_node::timeout_
     *
     * @brief timeout exposedField
     */

    /**
     * @var load_sensor_node::watch_list_
     *
     * @brief watch_list exposedField
     */

    /**
     * @var load_sensor_node::is_active_
     *
     * @brief is_active eventOut
     */

    /**
     * @var load_sensor_node::is_loaded_
     *
     * @brief is_loaded eventOut
     */

    /**
     * @var load_sensor_node::load_time_
     *
     * @brief load_time eventOut
     */

    /**
     * @var load_sensor_node::progress_
     *
     * @brief progress eventOut
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    load_sensor_node::
    load_sensor_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        enabled_(*this, true),
        timeout_(*this),
        watch_list_(*this),
        is_active_emitter_(*this, this->is_active_),
        is_loaded_emitter_(*this, this->is_loaded_),
        load_time_emitter_(*this, this->load_time_),
        progress_emitter_(*this, this->progress_)
    {}

    /**
     * @brief Destroy.
     */
    load_sensor_node::~load_sensor_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_networking::load_sensor_metatype::id =
    "urn:X-openvrml:node:LoadSensor";

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this load_sensor_metatype.
 */
openvrml_node_x3d_networking::load_sensor_metatype::
load_sensor_metatype(openvrml::browser & browser):
    node_metatype(load_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_networking::load_sensor_metatype::~load_sensor_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c boost::shared_ptr to a @c node_type capable of creating
 *         LoadSensor nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c load_sensor_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_networking::load_sensor_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 8> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "timeout"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "watchList"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isLoaded"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "loadTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "progress")
    };
    typedef node_type_impl<load_sensor_node> node_type_t;

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
                &load_sensor_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &load_sensor_node::enabled_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &load_sensor_node::timeout_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &load_sensor_node::watch_list_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &load_sensor_node::is_active_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &load_sensor_node::is_loaded_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &load_sensor_node::load_time_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &load_sensor_node::progress_emitter_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
