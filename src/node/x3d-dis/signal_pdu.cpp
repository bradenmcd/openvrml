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

# include "signal_pdu.h"
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
     * @brief Represents SignalPdu node instances.
     */
    class OPENVRML_LOCAL signal_pdu_node :
        public abstract_node<signal_pdu_node>,
        public child_node {

        friend class openvrml_node_x3d_dis::signal_pdu_metatype;

        exposedfield<sfstring> address_;
        exposedfield<sfint32> application_id_;
        exposedfield<mfint32> data_;
        exposedfield<sfint32> data_length_;
        exposedfield<sfint32> encoding_scheme_;
        exposedfield<sfint32> entity_id_;
        exposedfield<sfstring> multicast_relay_host_;
        exposedfield<sfint32> multicast_relay_port_;
        exposedfield<sfstring> network_mode_;
        exposedfield<sfint32> port_;
        exposedfield<sfint32> radio_id_;
        exposedfield<sffloat> read_interval_;
        exposedfield<sfbool> rtp_header_expected_;
        exposedfield<sfint32> sample_rate_;
        exposedfield<sfint32> samples_;
        exposedfield<sfint32> site_id_;
        exposedfield<sfint32> tdl_type_;
        exposedfield<sfint32> which_geometry_;
        exposedfield<sffloat> write_interval_;
        sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        sfbool is_network_reader_;
        sfbool_emitter is_network_reader_emitter_;
        sfbool is_network_writer_;
        sfbool_emitter is_network_writer_emitter_;
        sfbool is_rtp_header_heard_;
        sfbool_emitter is_rtp_header_heard_emitter_;
        sfbool is_stand_alone_;
        sfbool_emitter is_stand_alone_emitter_;
        sftime timestamp_;
        sftime_emitter timestamp_emitter_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        signal_pdu_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~signal_pdu_node() OPENVRML_NOTHROW;
    };


    /**
     * @var signal_pdu_node::signal_pdu_metatype
     *
     * @brief Class object for SignalPdu nodes.
     */

    /**
     * @var signal_pdu_node::address_
     *
     * @brief address exposedField
     */

    /**
     * @var signal_pdu_node::application_id_
     *
     * @brief application_id exposedField
     */

    /**
     * @var signal_pdu_node::data_
     *
     * @brief data exposedField
     */

    /**
     * @var signal_pdu_node::data_length_
     *
     * @brief data_length exposedField
     */

    /**
     * @var signal_pdu_node::encoding_scheme_
     *
     * @brief encoding_scheme exposedField
     */

    /**
     * @var signal_pdu_node::entity_id_
     *
     * @brief entity_id exposedField
     */

    /**
     * @var signal_pdu_node::multicast_relay_host_
     *
     * @brief multicast_relay_host exposedField
     */

    /**
     * @var signal_pdu_node::multicast_relay_port_
     *
     * @brief multicast_relay_port exposedField
     */

    /**
     * @var signal_pdu_node::network_mode_
     *
     * @brief network_mode exposedField
     */

    /**
     * @var signal_pdu_node::port_
     *
     * @brief port exposedField
     */

    /**
     * @var signal_pdu_node::radio_id_
     *
     * @brief radio_id exposedField
     */

    /**
     * @var signal_pdu_node::read_interval_
     *
     * @brief read_interval exposedField
     */

    /**
     * @var signal_pdu_node::rtp_header_expected_
     *
     * @brief rtp_header_expected exposedField
     */

    /**
     * @var signal_pdu_node::sample_rate_
     *
     * @brief sample_rate exposedField
     */

    /**
     * @var signal_pdu_node::samples_
     *
     * @brief samples exposedField
     */

    /**
     * @var signal_pdu_node::site_id_
     *
     * @brief site_id exposedField
     */

    /**
     * @var signal_pdu_node::tdl_type_
     *
     * @brief tdl_type exposedField
     */

    /**
     * @var signal_pdu_node::which_geometry_
     *
     * @brief which_geometry exposedField
     */

    /**
     * @var signal_pdu_node::write_interval_
     *
     * @brief write_interval exposedField
     */

    /**
     * @var signal_pdu_node::is_active_
     *
     * @brief is_active eventOut
     */

    /**
     * @var signal_pdu_node::is_network_reader_
     *
     * @brief is_network_reader eventOut
     */

    /**
     * @var signal_pdu_node::is_network_writer_
     *
     * @brief is_network_writer eventOut
     */

    /**
     * @var signal_pdu_node::is_rtp_header_heard_
     *
     * @brief is_rtp_header_heard eventOut
     */

    /**
     * @var signal_pdu_node::is_stand_alone_
     *
     * @brief is_stand_alone eventOut
     */

    /**
     * @var signal_pdu_node::timestamp_
     *
     * @brief timestamp eventOut
     */

    /**
     * @var signal_pdu_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var signal_pdu_node::bbox_size_
     *
     * @brief bbox_size field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    signal_pdu_node::
    signal_pdu_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        address_(*this, "localhost"),
        application_id_(*this, 1),
        data_(*this),
        data_length_(*this),
        encoding_scheme_(*this),
        entity_id_(*this),
        multicast_relay_host_(*this),
        multicast_relay_port_(*this),
        network_mode_(*this, "standAlone"),
        port_(*this),
        radio_id_(*this),
        read_interval_(*this, 0.1f),
        rtp_header_expected_(*this),
        sample_rate_(*this),
        samples_(*this),
        site_id_(*this),
        tdl_type_(*this),
        which_geometry_(*this, 1),
        write_interval_(*this, 1),
        is_active_emitter_(*this, this->is_active_),
        is_network_reader_emitter_(*this, this->is_network_reader_),
        is_network_writer_emitter_(*this, this->is_network_writer_),
        is_rtp_header_heard_emitter_(*this, this->is_rtp_header_heard_),
        is_stand_alone_emitter_(*this, this->is_stand_alone_),
        timestamp_emitter_(*this, this->timestamp_),
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    signal_pdu_node::~signal_pdu_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_dis::signal_pdu_metatype::id =
    "urn:X-openvrml:node:SignalPdu";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c signal_pdu_metatype.
 */
openvrml_node_x3d_dis::signal_pdu_metatype::
signal_pdu_metatype(openvrml::browser & browser):
    node_metatype(signal_pdu_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_dis::signal_pdu_metatype::~signal_pdu_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating SignalPdu nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c signal_pdu_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_dis::signal_pdu_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 28> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "address"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "applicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "data"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "dataLength"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "encodingScheme"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "multicastRelayHost"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "multicastRelayPort"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "networkMode"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "port"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "readInterval"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "rtpHeaderExpected"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "sampleRate"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "samples"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "siteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "tdlType"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "whichGeometry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "writeInterval"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkReader"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkWriter"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isRtpHeaderHeard"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isStandAlone"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "timestamp"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };
    typedef node_type_impl<signal_pdu_node> node_type_t;

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
                &signal_pdu_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::address_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::application_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::data_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::data_length_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::encoding_scheme_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::entity_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::multicast_relay_host_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::multicast_relay_port_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::network_mode_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::port_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::radio_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::read_interval_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::rtp_header_expected_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::sample_rate_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::samples_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::site_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::tdl_type_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::which_geometry_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::write_interval_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::is_active_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::is_network_reader_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::is_network_writer_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::is_rtp_header_heard_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::is_stand_alone_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::timestamp_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::bbox_center_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &signal_pdu_node::bbox_size_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
