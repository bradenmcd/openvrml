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

# include "geo_viewpoint.h"
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
     * @brief Represents GeoViewpoint node instances.
     */
    class OPENVRML_LOCAL geo_viewpoint_node :
        public abstract_node<geo_viewpoint_node>,
        public child_node {

        friend class openvrml_node_x3d_geospatial::geo_viewpoint_metatype;

        class set_bind_listener : public event_listener_base<self_t>,
                                  public sfbool_listener {
        public:
            explicit set_bind_listener(self_t & node);
            virtual ~set_bind_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_orientation_listener : public event_listener_base<self_t>,
                                         public sfrotation_listener {
        public:
            explicit set_orientation_listener(self_t & node);
            virtual ~set_orientation_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfrotation & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_position_listener : public event_listener_base<self_t>,
                                      public sfvec3d_listener {
        public:
            explicit set_position_listener(self_t & node);
            virtual ~set_position_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfvec3d & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_bind_listener set_bind_listener_;
        set_orientation_listener set_orientation_listener_;
        set_position_listener set_position_listener_;
        exposedfield<sfstring> description_;
        exposedfield<sffloat> field_of_view_;
        exposedfield<sfbool> headlight_;
        exposedfield<sfbool> jump_;
        exposedfield<mfstring> nav_type_;
        sftime bind_time_;
        sftime_emitter bind_time_emitter_;
        sfbool is_bound_;
        sfbool_emitter is_bound_emitter_;
        sfnode geo_origin_;
        mfstring geo_system_;
        sfrotation orientation_;
        sfvec3d position_;
        sffloat speed_factor_;

    public:
        geo_viewpoint_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_viewpoint_node() OPENVRML_NOTHROW;
    };


    /**
     * @var geo_viewpoint_node::geo_viewpoint_metatype
     *
     * @brief Class object for GeoViewpoint nodes.
     */

    /**
     * @var geo_viewpoint_node::set_bind_listener geo_viewpoint_node::set_bind_listener_
     *
     * @brief set_bind eventIn
     */

    /**
     * @var geo_viewpoint_node::set_orientation_listener geo_viewpoint_node::set_orientation_listener_
     *
     * @brief set_orientation eventIn
     */

    /**
     * @var geo_viewpoint_node::set_position_listener geo_viewpoint_node::set_position_listener_
     *
     * @brief set_position eventIn
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<geo_viewpoint_node>::exposedfield<openvrml::sfstring> geo_viewpoint_node::description_
     *
     * @brief description exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<geo_viewpoint_node>::exposedfield<openvrml::sffloat> geo_viewpoint_node::field_of_view_
     *
     * @brief field_of_view exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<geo_viewpoint_node>::exposedfield<openvrml::sfbool> geo_viewpoint_node::headlight_
     *
     * @brief headlight exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<geo_viewpoint_node>::exposedfield<openvrml::sfbool> geo_viewpoint_node::jump_
     *
     * @brief jump exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<geo_viewpoint_node>::exposedfield<openvrml::mfstring> geo_viewpoint_node::nav_type_
     *
     * @brief navType exposedField
     */

    /**
     * @var openvrml::sftime geo_viewpoint_node::bind_time_
     *
     * @brief bind_time eventOut
     */

    /**
     * @var openvrml::sfbool geo_viewpoint_node::is_bound_
     *
     * @brief is_bound eventOut
     */

    /**
     * @var openvrml::sfnode geo_viewpoint_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var openvrml::mfstring geo_viewpoint_node::geo_system_
     *
     * @brief geo_system field
     */

    /**
     * @var openvrml::sfrotation geo_viewpoint_node::orientation_
     *
     * @brief orientation field
     */

    /**
     * @var openvrml::sfvec3d geo_viewpoint_node::position_
     *
     * @brief position field
     */

    /**
     * @var openvrml::sffloat geo_viewpoint_node::speed_factor_
     *
     * @brief speed_factor field
     */

    geo_viewpoint_node::set_bind_listener::
    set_bind_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    geo_viewpoint_node::set_bind_listener::
    ~set_bind_listener() OPENVRML_NOTHROW
    {}

    void geo_viewpoint_node::set_bind_listener::
    do_process_event(const sfbool & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    geo_viewpoint_node::set_orientation_listener::
    set_orientation_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfrotation_listener(node)
    {}

    geo_viewpoint_node::set_orientation_listener::
    ~set_orientation_listener() OPENVRML_NOTHROW
    {}

    void geo_viewpoint_node::set_orientation_listener::
    do_process_event(const sfrotation & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    geo_viewpoint_node::set_position_listener::
    set_position_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfvec3d_listener(node)
    {}

    geo_viewpoint_node::set_position_listener::
    ~set_position_listener() OPENVRML_NOTHROW
    {}

    void geo_viewpoint_node::set_position_listener::
    do_process_event(const sfvec3d & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    const string default_nav_type_[] = { "EXAMINE", "ANY" };
    const vector<string> default_nav_type(default_nav_type_,
                                          default_nav_type_ + 1);

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_viewpoint_node::
    geo_viewpoint_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_bind_listener_(*this),
        set_orientation_listener_(*this),
        set_position_listener_(*this),
        description_(*this),
        field_of_view_(*this, 0.785398163f),
        headlight_(*this, true),
        jump_(*this, true),
        nav_type_(*this, default_nav_type),
        bind_time_emitter_(*this, this->bind_time_),
        is_bound_emitter_(*this, this->is_bound_),
        geo_system_(openvrml_node_x3d_geospatial::default_geo_system),
        position_(make_vec3d(0.0, 0.0, 100000.0)),
        speed_factor_(1)
    {}

    /**
     * @brief Destroy.
     */
    geo_viewpoint_node::~geo_viewpoint_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geospatial::geo_viewpoint_metatype::id =
    "urn:X-openvrml:node:GeoViewpoint";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c geo_viewpoint_metatype.
 */
openvrml_node_x3d_geospatial::geo_viewpoint_metatype::
geo_viewpoint_metatype(openvrml::browser & browser):
    node_metatype(geo_viewpoint_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geospatial::geo_viewpoint_metatype::~geo_viewpoint_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating GeoViewpoint nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c geo_viewpoint_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_geospatial::geo_viewpoint_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 16> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sfbool_id,
                       "set_bind"),
        node_interface(node_interface::eventin_id,
                       field_value::sfrotation_id,
                       "set_orientation"),
        node_interface(node_interface::eventin_id,
                       field_value::sfvec3d_id,
                       "set_position"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "description"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "fieldOfView"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "headlight"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "jump"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "navType"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "bindTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isBound"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem"),
        node_interface(node_interface::field_id,
                       field_value::sfrotation_id,
                       "orientation"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3d_id,
                       "position"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "speedFactor")
    };
    typedef node_type_impl<geo_viewpoint_node> node_type_t;

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
                &geo_viewpoint_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::set_bind_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::set_orientation_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::set_position_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::description_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::field_of_view_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::headlight_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::jump_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::nav_type_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::bind_time_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::is_bound_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::geo_origin_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::geo_system_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::orientation_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::position_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &geo_viewpoint_node::speed_factor_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
