// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# include "geo_touch_sensor.h"
# include "geospatial-common.h"
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
     * @brief Represents GeoTouchSensor node instances.
     */
    class OPENVRML_LOCAL geo_touch_sensor_node :
        public abstract_node<geo_touch_sensor_node>,
        public child_node {

        friend class openvrml_node_x3d_geospatial::geo_touch_sensor_metatype;

        exposedfield<sfbool> enabled_;
        sfvec3f hit_normal_changed_;
        sfvec3f_emitter hit_normal_changed_emitter_;
        sfvec3f hit_point_changed_;
        sfvec3f_emitter hit_point_changed_emitter_;
        sfvec2f hit_tex_coord_changed_;
        sfvec2f_emitter hit_tex_coord_changed_emitter_;
        sfvec3d hit_geo_coord_changed_;
        sfvec3d_emitter hit_geo_coord_changed_emitter_;
        sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        sfbool is_over_;
        sfbool_emitter is_over_emitter_;
        sftime touch_time_;
        sftime_emitter touch_time_emitter_;
        sfnode geo_origin_;
        mfstring geo_system_;

    public:
        geo_touch_sensor_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_touch_sensor_node() OPENVRML_NOTHROW;
    };


    /**
     * @var geo_touch_sensor_node::geo_touch_sensor_metatype
     *
     * @brief Class object for GeoTouchSensor nodes.
     */

    /**
     * @var geo_touch_sensor_node::enabled_
     *
     * @brief enabled exposedField
     */

    /**
     * @var geo_touch_sensor_node::hit_normal_changed_
     *
     * @brief hit_normal_changed eventOut
     */

    /**
     * @var geo_touch_sensor_node::hit_point_changed_
     *
     * @brief hit_point_changed eventOut
     */

    /**
     * @var geo_touch_sensor_node::hit_tex_coord_changed_
     *
     * @brief hit_tex_coord_changed eventOut
     */

    /**
     * @var geo_touch_sensor_node::hit_geo_coord_changed_
     *
     * @brief hit_geo_coord_changed eventOut
     */

    /**
     * @var geo_touch_sensor_node::is_active_
     *
     * @brief is_active eventOut
     */

    /**
     * @var geo_touch_sensor_node::is_over_
     *
     * @brief is_over eventOut
     */

    /**
     * @var geo_touch_sensor_node::touch_time_
     *
     * @brief touch_time eventOut
     */

    /**
     * @var geo_touch_sensor_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_touch_sensor_node::geo_system_
     *
     * @brief geo_system field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_touch_sensor_node::
    geo_touch_sensor_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        enabled_(*this, true),
        hit_normal_changed_emitter_(*this, this->hit_normal_changed_),
        hit_point_changed_emitter_(*this, this->hit_point_changed_),
        hit_tex_coord_changed_emitter_(*this, this->hit_tex_coord_changed_),
        hit_geo_coord_changed_emitter_(*this, this->hit_geo_coord_changed_),
        is_active_emitter_(*this, this->is_active_),
        is_over_emitter_(*this, this->is_over_),
        touch_time_emitter_(*this, this->touch_time_),
        geo_system_(openvrml_node_x3d_geospatial::default_geo_system)
    {}

    /**
     * @brief Destroy.
     */
    geo_touch_sensor_node::~geo_touch_sensor_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geospatial::geo_touch_sensor_metatype::id =
    "urn:X-openvrml:node:GeoTouchSensor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c geo_touch_sensor_metatype.
 */
openvrml_node_x3d_geospatial::geo_touch_sensor_metatype::
geo_touch_sensor_metatype(openvrml::browser & browser):
    node_metatype(geo_touch_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geospatial::geo_touch_sensor_metatype::
~geo_touch_sensor_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating GeoTouchSensor nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c geo_touch_sensor_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_geospatial::geo_touch_sensor_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 11> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
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
                       field_value::sfvec3d_id,
                       "hitGeoCoord_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isOver"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "touchTime"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem")
    };
    typedef node_type_impl<geo_touch_sensor_node> node_type_t;

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
                &geo_touch_sensor_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::enabled_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::hit_normal_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::hit_point_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::hit_tex_coord_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::hit_geo_coord_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::is_active_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::is_over_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::touch_time_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::geo_origin_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_touch_sensor_node::geo_system_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
