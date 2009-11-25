// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
// Copyright 2002  S. K. Bose
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "touch_sensor.h"

namespace {

    class OPENVRML_LOCAL touch_sensor_node :
        public openvrml::node_impl_util::abstract_node<touch_sensor_node>,
        public openvrml::pointing_device_sensor_node {

        friend class openvrml_node_vrml97::touch_sensor_metatype;

        exposedfield<openvrml::sfbool> enabled_;
        exposedfield<openvrml::sfstring> description_;
        openvrml::sfvec3f hit_normal_changed_;
        sfvec3f_emitter hit_normal_changed_emitter_;
        openvrml::sfvec3f hit_point_changed_;
        sfvec3f_emitter hit_point_changed_emitter_;
        openvrml::sfvec2f hit_tex_coord_changed_;
        sfvec2f_emitter hit_tex_coord_changed_emitter_;
        openvrml::sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        openvrml::sfbool is_over_;
        sfbool_emitter is_over_emitter_;
        openvrml::sftime touch_time_;
        sftime_emitter touch_time_emitter_;

    public:
        touch_sensor_node(const openvrml::node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~touch_sensor_node() OPENVRML_NOTHROW;

    private:
        virtual void do_activate(double timestamp, bool over, bool active,
                                 const double (&point)[3]);
    };

    /**
     * @class touch_sensor_node
     *
     * @brief TouchSensor node instances.
     */

    /**
     * @var class touch_sensor_node::touch_sensor_metatype
     *
     * @brief Class object for TouchSensor instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<touch_sensor_node>::exposedfield<openvrml::sfbool> touch_sensor_node::enabled_
     *
     * @brief enabled exposedField.
     */

    /**
     * @var openvrml::sfvec3f touch_sensor_node::hit_normal_changed_
     *
     * @brief hitNormal_changed eventOut value.
     */

    /**
     * @var openvrml::sfvec3f_emitter touch_sensor_node::hit_normal_changed_emitter_
     *
     * @brief hitNormal_changed eventOut emitter.
     */

    /**
     * @var openvrml::sfvec3f touch_sensor_node::hit_point_changed_
     *
     * @brief hitPoint_changed eventOut value.
     */

    /**
     * @var openvrml::sfvec3f_emitter touch_sensor_node::hit_point_changed_emitter_
     *
     * @brief hitPoint_changed eventOut emitter.
     */

    /**
     * @var openvrml::sfvec2f touch_sensor_node::hit_tex_coord_changed_
     *
     * @brief hitTexCoord_changed eventOut value.
     */

    /**
     * @var openvrml::sfvec2f_emitter touch_sensor_node::hit_tex_coord_changed_emitter_
     *
     * @brief hitTexCoord_changed eventOut emitter.
     */

    /**
     * @var openvrml::sfbool touch_sensor_node::is_active_
     *
     * @brief isActive eventOut value.
     */

    /**
     * @var openvrml::sfbool_emitter touch_sensor_node::is_active_emitter_
     *
     * @brief isActive eventOut emitter.
     */

    /**
     * @var openvrml::sfbool touch_sensor_node::is_over_
     *
     * @brief isOver eventOut value.
     */

    /**
     * @var openvrml::sfbool_emitter touch_sensor_node::is_over_emitter_
     *
     * @brief isOver eventOut emitter.
     */

    /**
     * @var openvrml::sftime touch_sensor_node::touch_time_
     *
     * @brief touchTime eventOut value.
     */

    /**
     * @var openvrml::sftime_emitter touch_sensor_node::touch_time_emitter_
     *
     * @brief touchTime eventOut emitter.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope that the new node will belong to.
     */
    touch_sensor_node::
    touch_sensor_node(const openvrml::node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        openvrml::node_impl_util::abstract_node<touch_sensor_node>(type, scope),
        pointing_device_sensor_node(type, scope),
        enabled_(*this, true),
        description_(*this),
        hit_normal_changed_emitter_(*this, this->hit_normal_changed_),
        hit_point_changed_emitter_(*this, this->hit_point_changed_),
        hit_tex_coord_changed_emitter_(*this, this->hit_tex_coord_changed_),
        is_active_(false),
        is_active_emitter_(*this, this->is_active_),
        is_over_(false),
        is_over_emitter_(*this, this->is_over_),
        touch_time_(0.0),
        touch_time_emitter_(*this, this->touch_time_)
    {
        this->node::modified(true);
    }

    /**
     * @brief Destroy.
     */
    touch_sensor_node::~touch_sensor_node() OPENVRML_NOTHROW
    {}

    /**
     * @todo Doesn't compute the xxx_changed eventOuts yet...
     */
    void touch_sensor_node::do_activate(double timestamp,
                                        const bool over,
                                        const bool active,
                                        const double (&)[3])
    {
        if (this->enabled_.sfbool::value()) {
            if (over && !active && this->is_active_.value()) {
                this->touch_time_.value(timestamp);
                node::emit_event(this->touch_time_emitter_, timestamp);
            }

            if (over != this->is_over_.value()) {
                this->is_over_.value(over);
                node::emit_event(this->is_over_emitter_, timestamp);
            }

            if (active != this->is_active_.value()) {
                this->is_active_.value(active);
                node::emit_event(this->is_active_emitter_, timestamp);
            }
            // if (over && any routes from eventOuts)
            //   generate xxx_changed eventOuts...
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::touch_sensor_metatype::id =
    "urn:X-openvrml:node:TouchSensor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::touch_sensor_metatype::
touch_sensor_metatype(openvrml::browser & browser):
    node_metatype(touch_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::touch_sensor_metatype::~touch_sensor_metatype()
    OPENVRML_NOTHROW
{}

# define TOUCH_SENSOR_INTERFACE_SEQ                                     \
    ((exposedfield, sfbool,   "enabled",             enabled_))         \
    ((eventout,     sfvec3f,  "hitNormal_changed",   hit_normal_changed_emitter_)) \
    ((eventout,     sfvec3f,  "hitPoint_changed",    hit_point_changed_emitter_)) \
    ((eventout,     sfvec2f,  "hitTexCoord_changed", hit_tex_coord_changed_emitter_)) \
    ((eventout,     sfbool,   "isActive",            is_active_emitter_)) \
    ((eventout,     sfbool,   "isOver",              is_over_emitter_)) \
    ((eventout,     sftime,   "touchTime",           touch_time_emitter_)) \
    ((exposedfield, sfnode,   "metadata",            metadata))         \
    ((exposedfield, sfstring, "description",         description_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              touch_sensor_metatype,
                                              touch_sensor_node,
                                              TOUCH_SENSOR_INTERFACE_SEQ)
