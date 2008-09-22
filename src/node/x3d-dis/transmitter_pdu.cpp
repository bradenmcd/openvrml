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

# include "transmitter_pdu.h"
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
     * @brief Represents TransmitterPdu node instances.
     */
    class OPENVRML_LOCAL transmitter_pdu_node :
        public abstract_node<transmitter_pdu_node>,
        public child_node {

        friend class openvrml_node_x3d_dis::transmitter_pdu_metatype;

        exposedfield<sfstring> address_;
        exposedfield<sfvec3f> antenna_location_;
        exposedfield<mfint32> antenna_pattern_length_;
        exposedfield<mfint32> antenna_pattern_type_;
        exposedfield<sfint32> application_id_;
        exposedfield<mfint32> crypto_key_id_;
        exposedfield<mfint32> crypto_system_;
        exposedfield<sfint32> entity_id_;
        exposedfield<mfint32> frequency_;
        exposedfield<mfint32> input_source_;
        exposedfield<mfint32> length_of_modulation_parameters_;
        exposedfield<mfint32> modulation_type_detail_;
        exposedfield<mfint32> modulation_type_major_;
        exposedfield<mfint32> modulation_type_spread_spectrum_;
        exposedfield<mfint32> modulation_type_system_;
        exposedfield<sfstring> multicast_relay_host_;
        exposedfield<sfint32> multicast_relay_port_;
        exposedfield<sfstring> network_mode_;
        exposedfield<sfint32> port_;
        exposedfield<sfint32> power_;
        exposedfield<sfint32> radio_entity_type_category_;
        exposedfield<sfint32> radio_entity_type_country_;
        exposedfield<sfint32> radio_entity_type_domain_;
        exposedfield<sfint32> radio_entity_type_kind_;
        exposedfield<sfint32> radio_entity_type_nomenclature_;
        exposedfield<sfint32> radio_entity_type_nomenclature_version_;
        exposedfield<sfint32> radio_id_;
        exposedfield<sffloat> read_interval_;
        exposedfield<sfvec3f> relative_antenna_location_;
        exposedfield<sfbool> rtp_header_expected_;
        exposedfield<sfint32> site_id_;
        exposedfield<sffloat> transmit_frequency_bandwidth_;
        exposedfield<sfint32> transmit_state_;
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
        transmitter_pdu_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~transmitter_pdu_node() OPENVRML_NOTHROW;
    };


    /**
     * @var transmitter_pdu_node::transmitter_pdu_metatype
     *
     * @brief Class object for TransmitterPdu nodes.
     */

    /**
     * @var transmitter_pdu_node::address_
     *
     * @brief address exposedField
     */

    /**
     * @var transmitter_pdu_node::antenna_location_
     *
     * @brief antenna_location exposedField
     */

    /**
     * @var transmitter_pdu_node::antenna_pattern_length_
     *
     * @brief antenna_pattern_length exposedField
     */

    /**
     * @var transmitter_pdu_node::antenna_pattern_type_
     *
     * @brief antenna_pattern_type exposedField
     */

    /**
     * @var transmitter_pdu_node::application_id_
     *
     * @brief application_id exposedField
     */

    /**
     * @var transmitter_pdu_node::crypto_key_id_
     *
     * @brief crypto_key_id exposedField
     */

    /**
     * @var transmitter_pdu_node::crypto_system_
     *
     * @brief crypto_system exposedField
     */

    /**
     * @var transmitter_pdu_node::entity_id_
     *
     * @brief entity_id exposedField
     */

    /**
     * @var transmitter_pdu_node::frequency_
     *
     * @brief frequency exposedField
     */

    /**
     * @var transmitter_pdu_node::input_source_
     *
     * @brief input_source exposedField
     */

    /**
     * @var transmitter_pdu_node::length_of_modulation_parameters_
     *
     * @brief length_of_modulation_parameters exposedField
     */

    /**
     * @var transmitter_pdu_node::modulation_type_detail_
     *
     * @brief modulation_type_detail exposedField
     */

    /**
     * @var transmitter_pdu_node::modulation_type_major_
     *
     * @brief modulation_type_major exposedField
     */

    /**
     * @var transmitter_pdu_node::modulation_type_spread_spectrum_
     *
     * @brief modulation_type_spread_spectrum exposedField
     */

    /**
     * @var transmitter_pdu_node::modulation_type_system_
     *
     * @brief modulation_type_system exposedField
     */

    /**
     * @var transmitter_pdu_node::multicast_relay_host_
     *
     * @brief multicast_relay_host exposedField
     */

    /**
     * @var transmitter_pdu_node::multicast_relay_port_
     *
     * @brief multicast_relay_port exposedField
     */

    /**
     * @var transmitter_pdu_node::network_mode_
     *
     * @brief network_mode exposedField
     */

    /**
     * @var transmitter_pdu_node::port_
     *
     * @brief port exposedField
     */

    /**
     * @var transmitter_pdu_node::power_
     *
     * @brief power exposedField
     */

    /**
     * @var transmitter_pdu_node::radio_entity_type_category_
     *
     * @brief radio_entity_type_category exposedField
     */

    /**
     * @var transmitter_pdu_node::radio_entity_type_country_
     *
     * @brief radio_entity_type_country exposedField
     */

    /**
     * @var transmitter_pdu_node::radio_entity_type_domain_
     *
     * @brief radio_entity_type_domain exposedField
     */

    /**
     * @var transmitter_pdu_node::radio_entity_type_kind_
     *
     * @brief radio_entity_type_kind exposedField
     */

    /**
     * @var transmitter_pdu_node::radio_entity_type_nomenclature_
     *
     * @brief radio_entity_type_nomenclature exposedField
     */

    /**
     * @var transmitter_pdu_node::radio_entity_type_nomenclature_version_
     *
     * @brief radio_entity_type_nomenclature_version exposedField
     */

    /**
     * @var transmitter_pdu_node::radio_id_
     *
     * @brief radio_id exposedField
     */

    /**
     * @var transmitter_pdu_node::read_interval_
     *
     * @brief read_interval exposedField
     */

    /**
     * @var transmitter_pdu_node::relative_antenna_location_
     *
     * @brief relative_antenna_location exposedField
     */

    /**
     * @var transmitter_pdu_node::rtp_header_expected_
     *
     * @brief rtp_header_expected exposedField
     */

    /**
     * @var transmitter_pdu_node::site_id_
     *
     * @brief site_id exposedField
     */

    /**
     * @var transmitter_pdu_node::transmit_frequency_bandwidth_
     *
     * @brief transmit_frequency_bandwidth exposedField
     */

    /**
     * @var transmitter_pdu_node::transmit_state_
     *
     * @brief transmit_state exposedField
     */

    /**
     * @var transmitter_pdu_node::which_geometry_
     *
     * @brief which_geometry exposedField
     */

    /**
     * @var transmitter_pdu_node::write_interval_
     *
     * @brief write_interval exposedField
     */

    /**
     * @var transmitter_pdu_node::is_active_
     *
     * @brief is_active eventOut
     */

    /**
     * @var transmitter_pdu_node::is_network_reader_
     *
     * @brief is_network_reader eventOut
     */

    /**
     * @var transmitter_pdu_node::is_network_writer_
     *
     * @brief is_network_writer eventOut
     */

    /**
     * @var transmitter_pdu_node::is_rtp_header_heard_
     *
     * @brief is_rtp_header_heard eventOut
     */

    /**
     * @var transmitter_pdu_node::is_stand_alone_
     *
     * @brief is_stand_alone eventOut
     */

    /**
     * @var transmitter_pdu_node::timestamp_
     *
     * @brief timestamp eventOut
     */

    /**
     * @var transmitter_pdu_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var transmitter_pdu_node::bbox_size_
     *
     * @brief bbox_size field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    transmitter_pdu_node::
    transmitter_pdu_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        address_(*this, "localhost"),
        antenna_location_(*this),
        antenna_pattern_length_(*this),
        antenna_pattern_type_(*this),
        application_id_(*this, 1),
        crypto_key_id_(*this),
        crypto_system_(*this),
        entity_id_(*this),
        frequency_(*this),
        input_source_(*this),
        length_of_modulation_parameters_(*this),
        modulation_type_detail_(*this),
        modulation_type_major_(*this),
        modulation_type_spread_spectrum_(*this),
        modulation_type_system_(*this),
        multicast_relay_host_(*this),
        multicast_relay_port_(*this),
        network_mode_(*this, "standAlone"),
        port_(*this),
        power_(*this),
        radio_entity_type_category_(*this),
        radio_entity_type_country_(*this),
        radio_entity_type_domain_(*this),
        radio_entity_type_kind_(*this),
        radio_entity_type_nomenclature_(*this),
        radio_entity_type_nomenclature_version_(*this),
        radio_id_(*this),
        read_interval_(*this, 0.1f),
        relative_antenna_location_(*this),
        rtp_header_expected_(*this),
        site_id_(*this),
        transmit_frequency_bandwidth_(*this),
        transmit_state_(*this),
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
    transmitter_pdu_node::~transmitter_pdu_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_dis::transmitter_pdu_metatype::id =
    "urn:X-openvrml:node:TransmitterPdu";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c transmitter_pdu_metatype.
 */
openvrml_node_x3d_dis::transmitter_pdu_metatype::
transmitter_pdu_metatype(openvrml::browser & browser):
    node_metatype(transmitter_pdu_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_dis::transmitter_pdu_metatype::~transmitter_pdu_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating TransmitterPdu nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c transmitter_pdu_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_dis::transmitter_pdu_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 44> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "address"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "antennaLocation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "antennaPatternLength"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "antennaPatternType"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "applicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "cryptoKeyID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "cryptoSystem"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "frequency"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "inputSource"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "lengthOfModulationParameters"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "modulationTypeDetail"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "modulationTypeMajor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "modulationTypeSpreadSpectrum"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "modulationTypeSystem"),
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
                       "power"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeCategory"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeCountry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeDomain"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeKind"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeNomenclature"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeNomenclatureVersion"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "readInterval"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "relativeAntennaLocation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "rtpHeaderExpected"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "siteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "transmitFrequencyBandwidth"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "transmitState"),
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
    typedef node_type_impl<transmitter_pdu_node> node_type_t;

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
                &transmitter_pdu_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::address_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::antenna_location_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::antenna_pattern_length_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::antenna_pattern_type_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::application_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::crypto_key_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::crypto_system_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::entity_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::frequency_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::input_source_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::length_of_modulation_parameters_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::modulation_type_detail_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::modulation_type_major_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::modulation_type_spread_spectrum_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::modulation_type_system_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::multicast_relay_host_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::multicast_relay_port_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::network_mode_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::port_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::power_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::radio_entity_type_category_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::radio_entity_type_country_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::radio_entity_type_domain_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::radio_entity_type_kind_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::radio_entity_type_nomenclature_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::radio_entity_type_nomenclature_version_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::radio_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::read_interval_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::relative_antenna_location_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::rtp_header_expected_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::site_id_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::transmit_frequency_bandwidth_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::transmit_state_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::which_geometry_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::write_interval_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::is_active_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::is_network_reader_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::is_network_writer_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::is_rtp_header_heard_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::is_stand_alone_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::timestamp_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::bbox_center_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &transmitter_pdu_node::bbox_size_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
