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

# include "geo_position_interpolator.h"
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
     * @brief Represents GeoPositionInterpolator node instances.
     */
    class OPENVRML_LOCAL geo_position_interpolator_node :
        public abstract_node<geo_position_interpolator_node>,
        public child_node {

        friend
        class openvrml_node_x3d_geospatial::geo_position_interpolator_metatype;

        class set_fraction_listener : public event_listener_base<self_t>,
                                      public sffloat_listener {
        public:
            explicit set_fraction_listener(self_t & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<mffloat> key_;
        exposedfield<mfvec3d> key_value_;
        sfvec3d geovalue_changed_;
        sfvec3d_emitter geovalue_changed_emitter_;
        sfvec3f value_changed_;
        sfvec3f_emitter value_changed_emitter_;
        sfnode geo_origin_;
        mfstring geo_system_;

    public:
        geo_position_interpolator_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_position_interpolator_node() OPENVRML_NOTHROW;
    };


    /**
     * @var geo_position_interpolator_node::geo_position_interpolator_metatype
     *
     * @brief Class object for GeoPositionInterpolator nodes.
     */

    /**
     * @var geo_position_interpolator_node::set_fraction_listener geo_position_interpolator_node::set_fraction_listener_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var geo_position_interpolator_node::key_
     *
     * @brief key exposedField
     */

    /**
     * @var geo_position_interpolator_node::key_value_
     *
     * @brief key_value exposedField
     */

    /**
     * @var geo_position_interpolator_node::geovalue_changed_
     *
     * @brief geovalue_changed eventOut
     */

    /**
     * @var geo_position_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut
     */

    /**
     * @var geo_position_interpolator_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_position_interpolator_node::geo_system_
     *
     * @brief geo_system field
     */

    geo_position_interpolator_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    geo_position_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void geo_position_interpolator_node::set_fraction_listener::
    do_process_event(const sffloat & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    geo_position_interpolator_node::
    geo_position_interpolator_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        geovalue_changed_emitter_(*this, this->geovalue_changed_),
        value_changed_emitter_(*this, this->value_changed_),
        geo_system_(openvrml_node_x3d_geospatial::default_geo_system)
    {}

    /**
     * @brief Destroy.
     */
    geo_position_interpolator_node::~geo_position_interpolator_node()
        OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_geospatial::geo_position_interpolator_metatype::id =
    "urn:X-openvrml:node:GeoPositionInterpolator";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c geo_position_interpolator_metatype.
 */
openvrml_node_x3d_geospatial::geo_position_interpolator_metatype::
geo_position_interpolator_metatype(openvrml::browser & browser):
    node_metatype(geo_position_interpolator_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geospatial::geo_position_interpolator_metatype::
~geo_position_interpolator_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating GeoPositionInterpolator nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c geo_position_interpolator_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_geospatial::geo_position_interpolator_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 8> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "key"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3d_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3d_id,
                       "geovalue_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "value_changed"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem")
    };
    typedef node_type_impl<geo_position_interpolator_node> node_type_t;

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
                &geo_position_interpolator_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &geo_position_interpolator_node::set_fraction_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &geo_position_interpolator_node::key_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &geo_position_interpolator_node::key_value_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_position_interpolator_node::geovalue_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_position_interpolator_node::value_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_position_interpolator_node::geo_origin_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_position_interpolator_node::geo_system_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
