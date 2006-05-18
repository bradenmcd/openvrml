// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2006  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <private.h>
# include "browser.h"
# include "node_impl_util.h"
# include "x3d_networking.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for LoadSensor nodes.
     */
    class OPENVRML_LOCAL load_sensor_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit load_sensor_metatype(openvrml::browser & browser);
        virtual ~load_sensor_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_networking_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(load_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new load_sensor_metatype(b)));
}

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL load_sensor_node :
        public abstract_node<load_sensor_node>,
        public child_node {

        friend class load_sensor_metatype;

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
     * @brief @c node_metatype identifier.
     */
    const char * const load_sensor_metatype::id =
        "urn:X-openvrml:node:LoadSensor";

    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this load_sensor_metatype.
     */
    load_sensor_metatype::load_sensor_metatype(openvrml::browser & browser):
        node_metatype(load_sensor_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    load_sensor_metatype::~load_sensor_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating LoadSensor nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by load_sensor_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    load_sensor_metatype::do_create_type(const std::string & id,
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

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sfnode> >(
                            &load_sensor_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sfnode> >(
                            &load_sensor_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sfnode> >(
                            &load_sensor_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sfbool> >(
                            &load_sensor_node::enabled_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sfbool> >(
                            &load_sensor_node::enabled_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sfbool> >(
                            &load_sensor_node::enabled_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sftime> >(
                            &load_sensor_node::timeout_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sftime> >(
                            &load_sensor_node::timeout_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<load_sensor_node>::exposedfield<sftime> >(
                            &load_sensor_node::timeout_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<load_sensor_node>::exposedfield<mfnode> >(
                            &load_sensor_node::watch_list_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<load_sensor_node>::exposedfield<mfnode> >(
                            &load_sensor_node::watch_list_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<load_sensor_node>::exposedfield<mfnode> >(
                            &load_sensor_node::watch_list_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        load_sensor_node::sfbool_emitter>(
                            &load_sensor_node::is_active_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        load_sensor_node::sfbool_emitter>(
                            &load_sensor_node::is_loaded_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        load_sensor_node::sftime_emitter>(
                            &load_sensor_node::load_time_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        load_sensor_node::sffloat_emitter>(
                            &load_sensor_node::progress_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @class load_sensor_node
     *
     * @brief Represents LoadSensor node instances.
     */

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
