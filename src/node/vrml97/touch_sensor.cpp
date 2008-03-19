// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating TouchSensor nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by touch_sensor_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::touch_sensor_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 9> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "hitNormal_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "hitPoint_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec2f_id,
                       "hitTexCoord_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isOver"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "touchTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "description")
    };

    typedef node_impl_util::node_type_impl<touch_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & touchSensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            touchSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfbool> >(
                        &touch_sensor_node::enabled_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfbool> >(
                        &touch_sensor_node::enabled_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfbool> >(
                        &touch_sensor_node::enabled_)));
        } else if (*interface == *++supported_interface) {
            touchSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::sfvec3f_emitter>(
                        &touch_sensor_node::hit_normal_changed_emitter_)));
        } else if (*interface == *++supported_interface) {
            touchSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::sfvec3f_emitter>(
                        &touch_sensor_node::hit_point_changed_emitter_)));
        } else if (*interface == *++supported_interface) {
            touchSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::sfvec2f_emitter>(
                        &touch_sensor_node::hit_tex_coord_changed_emitter_)));
        } else if (*interface == *++supported_interface) {
            touchSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::sfbool_emitter>(
                        &touch_sensor_node::is_active_emitter_)));
        } else if (*interface == *++supported_interface) {
            touchSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::sfbool_emitter>(
                        &touch_sensor_node::is_over_emitter_)));
        } else if (*interface == *++supported_interface) {
            touchSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::sftime_emitter>(
                        &touch_sensor_node::touch_time_emitter_)));
        } else if (*interface == *++supported_interface) {
            touchSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfnode> >(
                        &touch_sensor_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfnode> >(
                        &touch_sensor_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfnode> >(
                        &touch_sensor_node::metadata)));
        } else if (*interface == *++supported_interface) {
            touchSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfstring> >(
                        &touch_sensor_node::description_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfstring> >(
                        &touch_sensor_node::description_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<touch_sensor_node>::exposedfield<sfstring> >(
                        &touch_sensor_node::description_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}
