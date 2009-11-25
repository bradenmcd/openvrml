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

# define LOAD_SENSOR_INTERFACE_SEQ                             \
    ((exposedfield, sfnode,  "metadata",  metadata))           \
    ((exposedfield, sfbool,  "enabled",   enabled_))           \
    ((exposedfield, sftime,  "timeout",   timeout_))           \
    ((exposedfield, mfnode,  "watchList", watch_list_))        \
    ((eventout,     sfbool,  "isActive",  is_active_emitter_)) \
    ((eventout,     sfbool,  "isLoaded",  is_loaded_emitter_)) \
    ((eventout,     sftime,  "loadTime",  load_time_emitter_)) \
    ((eventout,     sffloat, "progress",  progress_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_networking,
                                              load_sensor_metatype,
                                              load_sensor_node,
                                              LOAD_SENSOR_INTERFACE_SEQ)
