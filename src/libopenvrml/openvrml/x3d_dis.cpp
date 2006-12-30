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
# include "x3d_dis.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for EspduTransform nodes.
     */
    class OPENVRML_LOCAL espdu_transform_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit espdu_transform_metatype(openvrml::browser & browser);
        virtual ~espdu_transform_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for ReceiverPdu nodes.
     */
    class OPENVRML_LOCAL receiver_pdu_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit receiver_pdu_metatype(openvrml::browser & browser);
        virtual ~receiver_pdu_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for SignalPdu nodes.
     */
    class OPENVRML_LOCAL signal_pdu_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit signal_pdu_metatype(openvrml::browser & browser);
        virtual ~signal_pdu_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for TransmitterPdu nodes.
     */
    class OPENVRML_LOCAL transmitter_pdu_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit transmitter_pdu_metatype(openvrml::browser & browser);
        virtual ~transmitter_pdu_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_dis_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(espdu_transform_metatype::id,
                        shared_ptr<node_metatype>(
                            new espdu_transform_metatype(b)));
    b.add_node_metatype(receiver_pdu_metatype::id,
                        shared_ptr<node_metatype>(
                            new receiver_pdu_metatype(b)));
    b.add_node_metatype(signal_pdu_metatype::id,
                        shared_ptr<node_metatype>(new signal_pdu_metatype(b)));
    b.add_node_metatype(transmitter_pdu_metatype::id,
                        shared_ptr<node_metatype>(
                            new transmitter_pdu_metatype(b)));
}

namespace {
    using namespace openvrml_;

    class OPENVRML_LOCAL espdu_transform_node :
        public abstract_node<espdu_transform_node>,
        public child_node {

        friend class espdu_transform_metatype;

        class add_children_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
        public:
            explicit add_children_listener(self_t & node);
            virtual ~add_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
        public:
            explicit remove_children_listener(self_t & node);
            virtual ~remove_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_articulation_parameter_value0_listener :
            public event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_articulation_parameter_value0_listener(self_t & node);
            virtual ~set_articulation_parameter_value0_listener()
                OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_articulation_parameter_value1_listener :
            public event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_articulation_parameter_value1_listener(self_t & node);
            virtual ~set_articulation_parameter_value1_listener()
                OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_articulation_parameter_value2_listener :
            public event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_articulation_parameter_value2_listener(self_t & node);
            virtual ~set_articulation_parameter_value2_listener()
                OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_articulation_parameter_value3_listener :
            public event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_articulation_parameter_value3_listener(self_t & node);
            virtual ~set_articulation_parameter_value3_listener()
                OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_articulation_parameter_value4_listener :
            public event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_articulation_parameter_value4_listener(self_t & node);
            virtual ~set_articulation_parameter_value4_listener()
                OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_articulation_parameter_value5_listener :
            public event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_articulation_parameter_value5_listener(self_t & node);
            virtual ~set_articulation_parameter_value5_listener()
                OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_articulation_parameter_value6_listener :
            public event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_articulation_parameter_value6_listener(self_t & node);
            virtual ~set_articulation_parameter_value6_listener()
                OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_articulation_parameter_value7_listener :
            public event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_articulation_parameter_value7_listener(self_t & node);
            virtual ~set_articulation_parameter_value7_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        set_articulation_parameter_value0_listener set_articulation_parameter_value0_listener_;
        set_articulation_parameter_value1_listener set_articulation_parameter_value1_listener_;
        set_articulation_parameter_value2_listener set_articulation_parameter_value2_listener_;
        set_articulation_parameter_value3_listener set_articulation_parameter_value3_listener_;
        set_articulation_parameter_value4_listener set_articulation_parameter_value4_listener_;
        set_articulation_parameter_value5_listener set_articulation_parameter_value5_listener_;
        set_articulation_parameter_value6_listener set_articulation_parameter_value6_listener_;
        set_articulation_parameter_value7_listener set_articulation_parameter_value7_listener_;
        exposedfield<sfstring> address_;
        exposedfield<sfint32> application_id_;
        exposedfield<sfint32> articulation_parameter_count_;
        exposedfield<mfint32> articulation_parameter_designator_array_;
        exposedfield<mfint32> articulation_parameter_change_indicator_array_;
        exposedfield<mfint32> articulation_parameter_id_part_attached_to_array_;
        exposedfield<mfint32> articulation_parameter_type_array_;
        exposedfield<mffloat> articulation_parameter_array_;
        exposedfield<sfvec3f> center_;
        exposedfield<mfnode> children_;
        exposedfield<sfint32> collision_type_;
        exposedfield<sfint32> dead_reckoning_;
        exposedfield<sfvec3f> detonation_location_;
        exposedfield<sfvec3f> detonation_relative_location_;
        exposedfield<sfint32> detonation_result_;
        exposedfield<sfint32> entity_category_;
        exposedfield<sfint32> entity_country_;
        exposedfield<sfint32> entity_domain_;
        exposedfield<sfint32> entity_extra_;
        exposedfield<sfint32> entity_id_;
        exposedfield<sfint32> entity_kind_;
        exposedfield<sfint32> entity_specific_;
        exposedfield<sfint32> entity_sub_category_;
        exposedfield<sfint32> event_application_id_;
        exposedfield<sfint32> event_entity_id_;
        exposedfield<sfint32> event_number_;
        exposedfield<sfint32> event_site_id_;
        exposedfield<sfbool> fired1_;
        exposedfield<sfbool> fired2_;
        exposedfield<sfint32> fire_mission_index_;
        exposedfield<sffloat> firing_range_;
        exposedfield<sfint32> firing_rate_;
        exposedfield<sfint32> force_id_;
        exposedfield<sfint32> fuse_;
        exposedfield<sfvec3f> linear_velocity_;
        exposedfield<sfvec3f> linear_acceleration_;
        exposedfield<sfstring> marking_;
        exposedfield<sfstring> multicast_relay_host_;
        exposedfield<sfint32> multicast_relay_port_;
        exposedfield<sfint32> munition_application_id_;
        exposedfield<sfvec3f> munition_end_point_;
        exposedfield<sfint32> munition_entity_id_;
        exposedfield<sfint32> munition_quantity_;
        exposedfield<sfint32> munition_site_id_;
        exposedfield<sfvec3f> munition_start_point_;
        exposedfield<sfstring> network_mode_;
        exposedfield<sfint32> port_;
        exposedfield<sftime> read_interval_;
        exposedfield<sfrotation> rotation_;
        exposedfield<sfvec3f> scale_;
        exposedfield<sfrotation> scale_orientation_;
        exposedfield<sfint32> site_id_;
        exposedfield<sfvec3f> translation_;
        exposedfield<sfint32> warhead_;
        exposedfield<sftime> write_interval_;
        sffloat articulation_parameter_value0_changed_;
        sffloat_emitter articulation_parameter_value0_changed_emitter_;
        sffloat articulation_parameter_value1_changed_;
        sffloat_emitter articulation_parameter_value1_changed_emitter_;
        sffloat articulation_parameter_value2_changed_;
        sffloat_emitter articulation_parameter_value2_changed_emitter_;
        sffloat articulation_parameter_value3_changed_;
        sffloat_emitter articulation_parameter_value3_changed_emitter_;
        sffloat articulation_parameter_value4_changed_;
        sffloat_emitter articulation_parameter_value4_changed_emitter_;
        sffloat articulation_parameter_value5_changed_;
        sffloat_emitter articulation_parameter_value5_changed_emitter_;
        sffloat articulation_parameter_value6_changed_;
        sffloat_emitter articulation_parameter_value6_changed_emitter_;
        sffloat articulation_parameter_value7_changed_;
        sffloat_emitter articulation_parameter_value7_changed_emitter_;
        sftime collide_time_;
        sftime_emitter collide_time_emitter_;
        sftime detonate_time_;
        sftime_emitter detonate_time_emitter_;
        sftime fired_time_;
        sftime_emitter fired_time_emitter_;
        sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        sfbool is_collided_;
        sfbool_emitter is_collided_emitter_;
        sfbool is_detonated_;
        sfbool_emitter is_detonated_emitter_;
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
        sfbool rtp_header_expected_;

    public:
        espdu_transform_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~espdu_transform_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL receiver_pdu_node :
        public abstract_node<receiver_pdu_node>,
        public child_node {

        friend class receiver_pdu_metatype;

        exposedfield<sfstring> address_;
        exposedfield<sfint32> application_id_;
        exposedfield<sfint32> entity_id_;
        exposedfield<sfstring> multicast_relay_host_;
        exposedfield<sfint32> multicast_relay_port_;
        exposedfield<sfstring> network_mode_;
        exposedfield<sfint32> port_;
        exposedfield<sfint32> radio_id_;
        exposedfield<sffloat> read_interval_;
        exposedfield<sffloat> received_power_;
        exposedfield<sfint32> receiver_state_;
        exposedfield<sfbool> rtp_header_expected_;
        exposedfield<sfint32> site_id_;
        exposedfield<sfint32> transmitter_application_id_;
        exposedfield<sfint32> transmitter_entity_id_;
        exposedfield<sfint32> transmitter_radio_id_;
        exposedfield<sfint32> transmitter_site_id_;
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
        receiver_pdu_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~receiver_pdu_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL signal_pdu_node :
        public abstract_node<signal_pdu_node>,
        public child_node {

        friend class signal_pdu_metatype;

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

    class OPENVRML_LOCAL transmitter_pdu_node :
        public abstract_node<transmitter_pdu_node>,
        public child_node {

        friend class transmitter_pdu_metatype;

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
     * @brief @c node_metatype identifier.
     */
    const char * const espdu_transform_metatype::id =
        "urn:X-openvrml:node:EspduTransform";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c espdu_transform_metatype.
     */
    espdu_transform_metatype::espdu_transform_metatype(openvrml::browser & browser):
        node_metatype(espdu_transform_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    espdu_transform_metatype::~espdu_transform_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating EspduTransform nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by espdu_transform_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    espdu_transform_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 88> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_articulationParameterValue0"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_articulationParameterValue1"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_articulationParameterValue2"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_articulationParameterValue3"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_articulationParameterValue4"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_articulationParameterValue5"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_articulationParameterValue6"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_articulationParameterValue7"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "address"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "applicationID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "articulationParameterCount"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "articulationParameterDesignatorArray"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "articulationParameterChangeIndicatorArray"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "articulationParameterIdPartAttachedToArray"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "articulationParameterTypeArray"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "articulationParameterArray"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "collisionType"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "deadReckoning"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "detonationLocation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "detonationRelativeLocation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "detonationResult"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "entityCategory"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "entityCountry"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "entityDomain"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "entityExtra"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "entityID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "entityKind"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "entitySpecific"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "entitySubCategory"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "eventApplicationID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "eventEntityID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "eventNumber"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "eventSiteID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "fired1"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "fired2"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "fireMissionIndex"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "firingRange"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "firingRate"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "forceID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "fuse"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "linearVelocity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "linearAcceleration"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "marking"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "multicastRelayHost"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "multicastRelayPort"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "munitionApplicationID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "munitionEndPoint"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "munitionEntityID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "munitionQuantity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "munitionSiteID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "munitionStartPoint"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "networkMode"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "port"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "readInterval"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "rotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "scale"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "scaleOrientation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "siteID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "translation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "warhead"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "writeInterval"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "articulationParameterValue0_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "articulationParameterValue1_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "articulationParameterValue2_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "articulationParameterValue3_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "articulationParameterValue4_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "articulationParameterValue5_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "articulationParameterValue6_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "articulationParameterValue7_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "collideTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "detonateTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "firedTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isCollided"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isDetonated"),
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
                           "bboxSize"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "rtpHeaderExpected")
        };
        typedef node_type_impl<espdu_transform_node> node_type_t;

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
                        abstract_node<espdu_transform_node>::exposedfield<sfnode> >(
                            &espdu_transform_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfnode> >(
                            &espdu_transform_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfnode> >(
                            &espdu_transform_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::add_children_listener>(
                            &espdu_transform_node::add_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::remove_children_listener>(
                            &espdu_transform_node::remove_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::set_articulation_parameter_value0_listener>(
                            &espdu_transform_node::set_articulation_parameter_value0_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::set_articulation_parameter_value1_listener>(
                            &espdu_transform_node::set_articulation_parameter_value1_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::set_articulation_parameter_value2_listener>(
                            &espdu_transform_node::set_articulation_parameter_value2_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::set_articulation_parameter_value3_listener>(
                            &espdu_transform_node::set_articulation_parameter_value3_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::set_articulation_parameter_value4_listener>(
                            &espdu_transform_node::set_articulation_parameter_value4_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::set_articulation_parameter_value5_listener>(
                            &espdu_transform_node::set_articulation_parameter_value5_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::set_articulation_parameter_value6_listener>(
                            &espdu_transform_node::set_articulation_parameter_value6_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        espdu_transform_node::set_articulation_parameter_value7_listener>(
                            &espdu_transform_node::set_articulation_parameter_value7_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::address_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::address_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::address_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::application_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::application_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::application_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::articulation_parameter_count_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::articulation_parameter_count_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::articulation_parameter_count_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_designator_array_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_designator_array_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_designator_array_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_change_indicator_array_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_change_indicator_array_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_change_indicator_array_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_id_part_attached_to_array_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_id_part_attached_to_array_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_id_part_attached_to_array_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_type_array_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_type_array_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfint32> >(
                            &espdu_transform_node::articulation_parameter_type_array_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mffloat> >(
                            &espdu_transform_node::articulation_parameter_array_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mffloat> >(
                            &espdu_transform_node::articulation_parameter_array_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mffloat> >(
                            &espdu_transform_node::articulation_parameter_array_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::center_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::center_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfnode> >(
                            &espdu_transform_node::children_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfnode> >(
                            &espdu_transform_node::children_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<mfnode> >(
                            &espdu_transform_node::children_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::collision_type_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::collision_type_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::collision_type_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::dead_reckoning_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::dead_reckoning_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::dead_reckoning_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::detonation_location_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::detonation_location_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::detonation_location_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::detonation_relative_location_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::detonation_relative_location_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::detonation_relative_location_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::detonation_result_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::detonation_result_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::detonation_result_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_category_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_category_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_category_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_country_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_country_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_country_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_domain_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_domain_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_domain_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_extra_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_extra_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_extra_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_kind_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_kind_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_kind_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_specific_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_specific_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_specific_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_sub_category_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_sub_category_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::entity_sub_category_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_application_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_application_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_application_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_entity_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_entity_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_entity_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_number_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_number_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_number_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_site_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_site_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::event_site_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfbool> >(
                            &espdu_transform_node::fired1_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfbool> >(
                            &espdu_transform_node::fired1_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfbool> >(
                            &espdu_transform_node::fired1_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfbool> >(
                            &espdu_transform_node::fired2_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfbool> >(
                            &espdu_transform_node::fired2_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfbool> >(
                            &espdu_transform_node::fired2_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::fire_mission_index_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::fire_mission_index_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::fire_mission_index_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sffloat> >(
                            &espdu_transform_node::firing_range_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sffloat> >(
                            &espdu_transform_node::firing_range_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sffloat> >(
                            &espdu_transform_node::firing_range_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::firing_rate_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::firing_rate_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::firing_rate_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::force_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::force_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::force_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::fuse_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::fuse_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::fuse_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::linear_velocity_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::linear_velocity_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::linear_velocity_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::linear_acceleration_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::linear_acceleration_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::linear_acceleration_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::marking_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::marking_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::marking_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::multicast_relay_host_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::multicast_relay_host_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::multicast_relay_host_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::multicast_relay_port_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::multicast_relay_port_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::multicast_relay_port_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_application_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_application_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_application_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::munition_end_point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::munition_end_point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::munition_end_point_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_entity_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_entity_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_entity_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_quantity_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_quantity_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_quantity_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_site_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_site_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::munition_site_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::munition_start_point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::munition_start_point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::munition_start_point_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::network_mode_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::network_mode_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfstring> >(
                            &espdu_transform_node::network_mode_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::port_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::port_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::port_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sftime> >(
                            &espdu_transform_node::read_interval_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sftime> >(
                            &espdu_transform_node::read_interval_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sftime> >(
                            &espdu_transform_node::read_interval_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfrotation> >(
                            &espdu_transform_node::rotation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfrotation> >(
                            &espdu_transform_node::rotation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfrotation> >(
                            &espdu_transform_node::rotation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::scale_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::scale_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::scale_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfrotation> >(
                            &espdu_transform_node::scale_orientation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfrotation> >(
                            &espdu_transform_node::scale_orientation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfrotation> >(
                            &espdu_transform_node::scale_orientation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::site_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::site_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::site_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::translation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::translation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfvec3f> >(
                            &espdu_transform_node::translation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::warhead_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::warhead_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sfint32> >(
                            &espdu_transform_node::warhead_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sftime> >(
                            &espdu_transform_node::write_interval_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sftime> >(
                            &espdu_transform_node::write_interval_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<espdu_transform_node>::exposedfield<sftime> >(
                            &espdu_transform_node::write_interval_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sffloat_emitter>(
                            &espdu_transform_node::articulation_parameter_value0_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sffloat_emitter>(
                            &espdu_transform_node::articulation_parameter_value1_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sffloat_emitter>(
                            &espdu_transform_node::articulation_parameter_value2_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sffloat_emitter>(
                            &espdu_transform_node::articulation_parameter_value3_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sffloat_emitter>(
                            &espdu_transform_node::articulation_parameter_value4_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sffloat_emitter>(
                            &espdu_transform_node::articulation_parameter_value5_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sffloat_emitter>(
                            &espdu_transform_node::articulation_parameter_value6_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sffloat_emitter>(
                            &espdu_transform_node::articulation_parameter_value7_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sftime_emitter>(
                            &espdu_transform_node::collide_time_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sftime_emitter>(
                            &espdu_transform_node::detonate_time_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sftime_emitter>(
                            &espdu_transform_node::fired_time_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sfbool_emitter>(
                            &espdu_transform_node::is_active_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sfbool_emitter>(
                            &espdu_transform_node::is_collided_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sfbool_emitter>(
                            &espdu_transform_node::is_detonated_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sfbool_emitter>(
                            &espdu_transform_node::is_network_reader_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sfbool_emitter>(
                            &espdu_transform_node::is_network_writer_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sfbool_emitter>(
                            &espdu_transform_node::is_rtp_header_heard_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sfbool_emitter>(
                            &espdu_transform_node::is_stand_alone_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        espdu_transform_node::sftime_emitter>(
                            &espdu_transform_node::timestamp_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &espdu_transform_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &espdu_transform_node::bbox_size_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &espdu_transform_node::rtp_header_expected_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const receiver_pdu_metatype::id =
        "urn:X-openvrml:node:ReceiverPdu";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c receiver_pdu_metatype.
     */
    receiver_pdu_metatype::receiver_pdu_metatype(openvrml::browser & browser):
        node_metatype(receiver_pdu_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    receiver_pdu_metatype::~receiver_pdu_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating ReceiverPdu nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by receiver_pdu_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    receiver_pdu_metatype::
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
                           field_value::sffloat_id,
                           "receivedPower"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "receiverState"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "rtpHeaderExpected"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "siteID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "transmitterApplicationID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "transmitterEntityID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "transmitterRadioID"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "transmitterSiteID"),
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
        typedef node_type_impl<receiver_pdu_node> node_type_t;

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
                        abstract_node<receiver_pdu_node>::exposedfield<sfnode> >(
                            &receiver_pdu_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfnode> >(
                            &receiver_pdu_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfnode> >(
                            &receiver_pdu_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::address_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::address_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::address_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::application_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::application_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::application_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::entity_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::entity_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::entity_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::multicast_relay_host_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::multicast_relay_host_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::multicast_relay_host_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::multicast_relay_port_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::multicast_relay_port_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::multicast_relay_port_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::network_mode_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::network_mode_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfstring> >(
                            &receiver_pdu_node::network_mode_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::port_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::port_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::port_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::radio_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::radio_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::radio_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::read_interval_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::read_interval_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::read_interval_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::received_power_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::received_power_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::received_power_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::receiver_state_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::receiver_state_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::receiver_state_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfbool> >(
                            &receiver_pdu_node::rtp_header_expected_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfbool> >(
                            &receiver_pdu_node::rtp_header_expected_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfbool> >(
                            &receiver_pdu_node::rtp_header_expected_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::site_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::site_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::site_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_application_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_application_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_application_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_entity_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_entity_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_entity_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_radio_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_radio_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_radio_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_site_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_site_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::transmitter_site_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::which_geometry_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::which_geometry_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sfint32> >(
                            &receiver_pdu_node::which_geometry_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::write_interval_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::write_interval_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<receiver_pdu_node>::exposedfield<sffloat> >(
                            &receiver_pdu_node::write_interval_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        receiver_pdu_node::sfbool_emitter>(
                            &receiver_pdu_node::is_active_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        receiver_pdu_node::sfbool_emitter>(
                            &receiver_pdu_node::is_network_reader_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        receiver_pdu_node::sfbool_emitter>(
                            &receiver_pdu_node::is_network_writer_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        receiver_pdu_node::sfbool_emitter>(
                            &receiver_pdu_node::is_rtp_header_heard_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        receiver_pdu_node::sfbool_emitter>(
                            &receiver_pdu_node::is_stand_alone_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        receiver_pdu_node::sftime_emitter>(
                            &receiver_pdu_node::timestamp_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &receiver_pdu_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &receiver_pdu_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const signal_pdu_metatype::id =
        "urn:X-openvrml:node:SignalPdu";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c signal_pdu_metatype.
     */
    signal_pdu_metatype::signal_pdu_metatype(openvrml::browser & browser):
        node_metatype(signal_pdu_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    signal_pdu_metatype::~signal_pdu_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating SignalPdu nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by signal_pdu_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    signal_pdu_metatype::
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
                        abstract_node<signal_pdu_node>::exposedfield<sfnode> >(
                            &signal_pdu_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfnode> >(
                            &signal_pdu_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfnode> >(
                            &signal_pdu_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::address_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::address_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::address_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::application_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::application_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::application_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<mfint32> >(
                            &signal_pdu_node::data_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<mfint32> >(
                            &signal_pdu_node::data_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<mfint32> >(
                            &signal_pdu_node::data_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::data_length_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::data_length_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::data_length_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::encoding_scheme_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::encoding_scheme_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::encoding_scheme_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::entity_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::entity_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::entity_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::multicast_relay_host_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::multicast_relay_host_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::multicast_relay_host_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::multicast_relay_port_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::multicast_relay_port_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::multicast_relay_port_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::network_mode_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::network_mode_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfstring> >(
                            &signal_pdu_node::network_mode_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::port_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::port_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::port_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::radio_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::radio_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::radio_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sffloat> >(
                            &signal_pdu_node::read_interval_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sffloat> >(
                            &signal_pdu_node::read_interval_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sffloat> >(
                            &signal_pdu_node::read_interval_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfbool> >(
                            &signal_pdu_node::rtp_header_expected_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfbool> >(
                            &signal_pdu_node::rtp_header_expected_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfbool> >(
                            &signal_pdu_node::rtp_header_expected_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::sample_rate_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::sample_rate_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::sample_rate_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::samples_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::samples_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::samples_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::site_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::site_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::site_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::tdl_type_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::tdl_type_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::tdl_type_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::which_geometry_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::which_geometry_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sfint32> >(
                            &signal_pdu_node::which_geometry_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sffloat> >(
                            &signal_pdu_node::write_interval_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sffloat> >(
                            &signal_pdu_node::write_interval_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<signal_pdu_node>::exposedfield<sffloat> >(
                            &signal_pdu_node::write_interval_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        signal_pdu_node::sfbool_emitter>(
                            &signal_pdu_node::is_active_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        signal_pdu_node::sfbool_emitter>(
                            &signal_pdu_node::is_network_reader_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        signal_pdu_node::sfbool_emitter>(
                            &signal_pdu_node::is_network_writer_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        signal_pdu_node::sfbool_emitter>(
                            &signal_pdu_node::is_rtp_header_heard_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        signal_pdu_node::sfbool_emitter>(
                            &signal_pdu_node::is_stand_alone_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        signal_pdu_node::sftime_emitter>(
                            &signal_pdu_node::timestamp_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &signal_pdu_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &signal_pdu_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const transmitter_pdu_metatype::id =
        "urn:X-openvrml:node:TransmitterPdu";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c transmitter_pdu_metatype.
     */
    transmitter_pdu_metatype::transmitter_pdu_metatype(openvrml::browser & browser):
        node_metatype(transmitter_pdu_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    transmitter_pdu_metatype::~transmitter_pdu_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating TransmitterPdu nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by transmitter_pdu_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    transmitter_pdu_metatype::
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
                        abstract_node<transmitter_pdu_node>::exposedfield<sfnode> >(
                            &transmitter_pdu_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfnode> >(
                            &transmitter_pdu_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfnode> >(
                            &transmitter_pdu_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::address_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::address_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::address_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfvec3f> >(
                            &transmitter_pdu_node::antenna_location_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfvec3f> >(
                            &transmitter_pdu_node::antenna_location_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfvec3f> >(
                            &transmitter_pdu_node::antenna_location_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::antenna_pattern_length_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::antenna_pattern_length_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::antenna_pattern_length_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::antenna_pattern_type_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::antenna_pattern_type_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::antenna_pattern_type_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::application_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::application_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::application_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::crypto_key_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::crypto_key_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::crypto_key_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::crypto_system_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::crypto_system_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::crypto_system_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::entity_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::entity_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::entity_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::frequency_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::frequency_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::frequency_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::input_source_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::input_source_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::input_source_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::length_of_modulation_parameters_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::length_of_modulation_parameters_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::length_of_modulation_parameters_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_detail_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_detail_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_detail_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_major_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_major_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_major_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_spread_spectrum_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_spread_spectrum_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_spread_spectrum_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_system_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_system_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<mfint32> >(
                            &transmitter_pdu_node::modulation_type_system_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::multicast_relay_host_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::multicast_relay_host_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::multicast_relay_host_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::multicast_relay_port_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::multicast_relay_port_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::multicast_relay_port_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::network_mode_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::network_mode_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfstring> >(
                            &transmitter_pdu_node::network_mode_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::port_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::port_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::port_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::power_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::power_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::power_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_category_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_category_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_category_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_country_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_country_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_country_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_domain_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_domain_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_domain_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_kind_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_kind_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_kind_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_nomenclature_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_nomenclature_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_nomenclature_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_nomenclature_version_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_nomenclature_version_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_entity_type_nomenclature_version_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::radio_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::read_interval_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::read_interval_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::read_interval_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfvec3f> >(
                            &transmitter_pdu_node::relative_antenna_location_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfvec3f> >(
                            &transmitter_pdu_node::relative_antenna_location_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfvec3f> >(
                            &transmitter_pdu_node::relative_antenna_location_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfbool> >(
                            &transmitter_pdu_node::rtp_header_expected_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfbool> >(
                            &transmitter_pdu_node::rtp_header_expected_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfbool> >(
                            &transmitter_pdu_node::rtp_header_expected_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::site_id_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::site_id_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::site_id_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::transmit_frequency_bandwidth_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::transmit_frequency_bandwidth_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::transmit_frequency_bandwidth_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::transmit_state_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::transmit_state_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::transmit_state_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::which_geometry_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::which_geometry_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sfint32> >(
                            &transmitter_pdu_node::which_geometry_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::write_interval_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::write_interval_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<transmitter_pdu_node>::exposedfield<sffloat> >(
                            &transmitter_pdu_node::write_interval_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        transmitter_pdu_node::sfbool_emitter>(
                            &transmitter_pdu_node::is_active_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        transmitter_pdu_node::sfbool_emitter>(
                            &transmitter_pdu_node::is_network_reader_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        transmitter_pdu_node::sfbool_emitter>(
                            &transmitter_pdu_node::is_network_writer_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        transmitter_pdu_node::sfbool_emitter>(
                            &transmitter_pdu_node::is_rtp_header_heard_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        transmitter_pdu_node::sfbool_emitter>(
                            &transmitter_pdu_node::is_stand_alone_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        transmitter_pdu_node::sftime_emitter>(
                            &transmitter_pdu_node::timestamp_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &transmitter_pdu_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &transmitter_pdu_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @class espdu_transform_node
     *
     * @brief Represents EspduTransform node instances.
     */

    /**
     * @var espdu_transform_node::espdu_transform_metatype
     *
     * @brief Class object for EspduTransform nodes.
     */

    /**
     * @var espdu_transform_node::add_children_
     *
     * @brief add_children eventIn
     */

    /**
     * @var espdu_transform_node::remove_children_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value0_
     *
     * @brief set_articulation_parameter_value0 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value1_
     *
     * @brief set_articulation_parameter_value1 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value2_
     *
     * @brief set_articulation_parameter_value2 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value3_
     *
     * @brief set_articulation_parameter_value3 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value4_
     *
     * @brief set_articulation_parameter_value4 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value5_
     *
     * @brief set_articulation_parameter_value5 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value6_
     *
     * @brief set_articulation_parameter_value6 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value7_
     *
     * @brief set_articulation_parameter_value7 eventIn
     */

    /**
     * @var espdu_transform_node::address_
     *
     * @brief address exposedField
     */

    /**
     * @var espdu_transform_node::application_id_
     *
     * @brief application_id exposedField
     */

    /**
     * @var espdu_transform_node::articulation_parameter_count_
     *
     * @brief articulation_parameter_count exposedField
     */

    /**
     * @var espdu_transform_node::articulation_parameter_designator_array_
     *
     * @brief articulation_parameter_designator_array exposedField
     */

    /**
     * @var espdu_transform_node::articulation_parameter_change_indicator_array_
     *
     * @brief articulation_parameter_change_indicator_array exposedField
     */

    /**
     * @var espdu_transform_node::articulation_parameter_id_part_attached_to_array_
     *
     * @brief articulation_parameter_id_part_attached_to_array exposedField
     */

    /**
     * @var espdu_transform_node::articulation_parameter_type_array_
     *
     * @brief articulation_parameter_type_array exposedField
     */

    /**
     * @var espdu_transform_node::articulation_parameter_array_
     *
     * @brief articulation_parameter_array exposedField
     */

    /**
     * @var espdu_transform_node::center_
     *
     * @brief center exposedField
     */

    /**
     * @var espdu_transform_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var espdu_transform_node::collision_type_
     *
     * @brief collision_type exposedField
     */

    /**
     * @var espdu_transform_node::dead_reckoning_
     *
     * @brief dead_reckoning exposedField
     */

    /**
     * @var espdu_transform_node::detonation_location_
     *
     * @brief detonation_location exposedField
     */

    /**
     * @var espdu_transform_node::detonation_relative_location_
     *
     * @brief detonation_relative_location exposedField
     */

    /**
     * @var espdu_transform_node::detonation_result_
     *
     * @brief detonation_result exposedField
     */

    /**
     * @var espdu_transform_node::entity_category_
     *
     * @brief entity_category exposedField
     */

    /**
     * @var espdu_transform_node::entity_country_
     *
     * @brief entity_country exposedField
     */

    /**
     * @var espdu_transform_node::entity_domain_
     *
     * @brief entity_domain exposedField
     */

    /**
     * @var espdu_transform_node::entity_extra_
     *
     * @brief entity_extra exposedField
     */

    /**
     * @var espdu_transform_node::entity_id_
     *
     * @brief entity_id exposedField
     */

    /**
     * @var espdu_transform_node::entity_kind_
     *
     * @brief entity_kind exposedField
     */

    /**
     * @var espdu_transform_node::entity_specific_
     *
     * @brief entity_specific exposedField
     */

    /**
     * @var espdu_transform_node::entity_sub_category_
     *
     * @brief entity_sub_category exposedField
     */

    /**
     * @var espdu_transform_node::event_application_id_
     *
     * @brief event_application_id exposedField
     */

    /**
     * @var espdu_transform_node::event_entity_id_
     *
     * @brief event_entity_id exposedField
     */

    /**
     * @var espdu_transform_node::event_number_
     *
     * @brief event_number exposedField
     */

    /**
     * @var espdu_transform_node::event_site_id_
     *
     * @brief event_site_id exposedField
     */

    /**
     * @var espdu_transform_node::fired1_
     *
     * @brief fired1 exposedField
     */

    /**
     * @var espdu_transform_node::fired2_
     *
     * @brief fired2 exposedField
     */

    /**
     * @var espdu_transform_node::fire_mission_index_
     *
     * @brief fire_mission_index exposedField
     */

    /**
     * @var espdu_transform_node::firing_range_
     *
     * @brief firing_range exposedField
     */

    /**
     * @var espdu_transform_node::firing_rate_
     *
     * @brief firing_rate exposedField
     */

    /**
     * @var espdu_transform_node::force_id_
     *
     * @brief force_id exposedField
     */

    /**
     * @var espdu_transform_node::fuse_
     *
     * @brief fuse exposedField
     */

    /**
     * @var espdu_transform_node::linear_velocity_
     *
     * @brief linear_velocity exposedField
     */

    /**
     * @var espdu_transform_node::linear_acceleration_
     *
     * @brief linear_acceleration exposedField
     */

    /**
     * @var espdu_transform_node::marking_
     *
     * @brief marking exposedField
     */

    /**
     * @var espdu_transform_node::multicast_relay_host_
     *
     * @brief multicast_relay_host exposedField
     */

    /**
     * @var espdu_transform_node::multicast_relay_port_
     *
     * @brief multicast_relay_port exposedField
     */

    /**
     * @var espdu_transform_node::munition_application_id_
     *
     * @brief munition_application_id exposedField
     */

    /**
     * @var espdu_transform_node::munition_end_point_
     *
     * @brief munition_end_point exposedField
     */

    /**
     * @var espdu_transform_node::munition_entity_id_
     *
     * @brief munition_entity_id exposedField
     */

    /**
     * @var espdu_transform_node::munition_quantity_
     *
     * @brief munition_quantity exposedField
     */

    /**
     * @var espdu_transform_node::munition_site_id_
     *
     * @brief munition_site_id exposedField
     */

    /**
     * @var espdu_transform_node::munition_start_point_
     *
     * @brief munition_start_point exposedField
     */

    /**
     * @var espdu_transform_node::network_mode_
     *
     * @brief network_mode exposedField
     */

    /**
     * @var espdu_transform_node::port_
     *
     * @brief port exposedField
     */

    /**
     * @var espdu_transform_node::read_interval_
     *
     * @brief read_interval exposedField
     */

    /**
     * @var espdu_transform_node::rotation_
     *
     * @brief rotation exposedField
     */

    /**
     * @var espdu_transform_node::scale_
     *
     * @brief scale exposedField
     */

    /**
     * @var espdu_transform_node::scale_orientation_
     *
     * @brief scale_orientation exposedField
     */

    /**
     * @var espdu_transform_node::site_id_
     *
     * @brief site_id exposedField
     */

    /**
     * @var espdu_transform_node::translation_
     *
     * @brief translation exposedField
     */

    /**
     * @var espdu_transform_node::warhead_
     *
     * @brief warhead exposedField
     */

    /**
     * @var espdu_transform_node::write_interval_
     *
     * @brief write_interval exposedField
     */

    /**
     * @var espdu_transform_node::articulation_parameter_value0_changed_
     *
     * @brief articulation_parameter_value0_changed eventOut
     */

    /**
     * @var espdu_transform_node::articulation_parameter_value1_changed_
     *
     * @brief articulation_parameter_value1_changed eventOut
     */

    /**
     * @var espdu_transform_node::articulation_parameter_value2_changed_
     *
     * @brief articulation_parameter_value2_changed eventOut
     */

    /**
     * @var espdu_transform_node::articulation_parameter_value3_changed_
     *
     * @brief articulation_parameter_value3_changed eventOut
     */

    /**
     * @var espdu_transform_node::articulation_parameter_value4_changed_
     *
     * @brief articulation_parameter_value4_changed eventOut
     */

    /**
     * @var espdu_transform_node::articulation_parameter_value5_changed_
     *
     * @brief articulation_parameter_value5_changed eventOut
     */

    /**
     * @var espdu_transform_node::articulation_parameter_value6_changed_
     *
     * @brief articulation_parameter_value6_changed eventOut
     */

    /**
     * @var espdu_transform_node::articulation_parameter_value7_changed_
     *
     * @brief articulation_parameter_value7_changed eventOut
     */

    /**
     * @var espdu_transform_node::collide_time_
     *
     * @brief collide_time eventOut
     */

    /**
     * @var espdu_transform_node::detonate_time_
     *
     * @brief detonate_time eventOut
     */

    /**
     * @var espdu_transform_node::fired_time_
     *
     * @brief fired_time eventOut
     */

    /**
     * @var espdu_transform_node::is_active_
     *
     * @brief is_active eventOut
     */

    /**
     * @var espdu_transform_node::is_collided_
     *
     * @brief is_collided eventOut
     */

    /**
     * @var espdu_transform_node::is_detonated_
     *
     * @brief is_detonated eventOut
     */

    /**
     * @var espdu_transform_node::is_network_reader_
     *
     * @brief is_network_reader eventOut
     */

    /**
     * @var espdu_transform_node::is_network_writer_
     *
     * @brief is_network_writer eventOut
     */

    /**
     * @var espdu_transform_node::is_rtp_header_heard_
     *
     * @brief is_rtp_header_heard eventOut
     */

    /**
     * @var espdu_transform_node::is_stand_alone_
     *
     * @brief is_stand_alone eventOut
     */

    /**
     * @var espdu_transform_node::timestamp_
     *
     * @brief timestamp eventOut
     */

    /**
     * @var espdu_transform_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var espdu_transform_node::bbox_size_
     *
     * @brief bbox_size field
     */

    /**
     * @var espdu_transform_node::rtp_header_expected_
     *
     * @brief rtp_header_expected field
     */

    espdu_transform_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    espdu_transform_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    espdu_transform_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::set_articulation_parameter_value0_listener::
    set_articulation_parameter_value0_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    espdu_transform_node::set_articulation_parameter_value0_listener::
    ~set_articulation_parameter_value0_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::set_articulation_parameter_value0_listener::
    do_process_event(const sffloat & /* fraction */, const double  /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::set_articulation_parameter_value1_listener::
    set_articulation_parameter_value1_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    espdu_transform_node::set_articulation_parameter_value1_listener::
    ~set_articulation_parameter_value1_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::set_articulation_parameter_value1_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::set_articulation_parameter_value2_listener::
    set_articulation_parameter_value2_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    espdu_transform_node::set_articulation_parameter_value2_listener::
    ~set_articulation_parameter_value2_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::set_articulation_parameter_value2_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::set_articulation_parameter_value3_listener::
    set_articulation_parameter_value3_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    espdu_transform_node::set_articulation_parameter_value3_listener::
    ~set_articulation_parameter_value3_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::set_articulation_parameter_value3_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::set_articulation_parameter_value4_listener::
    set_articulation_parameter_value4_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    espdu_transform_node::set_articulation_parameter_value4_listener::
    ~set_articulation_parameter_value4_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::set_articulation_parameter_value4_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::set_articulation_parameter_value5_listener::
    set_articulation_parameter_value5_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    espdu_transform_node::set_articulation_parameter_value5_listener::
    ~set_articulation_parameter_value5_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::set_articulation_parameter_value5_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::set_articulation_parameter_value6_listener::
    set_articulation_parameter_value6_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    espdu_transform_node::set_articulation_parameter_value6_listener::
    ~set_articulation_parameter_value6_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::set_articulation_parameter_value6_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    espdu_transform_node::set_articulation_parameter_value7_listener::
    set_articulation_parameter_value7_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    espdu_transform_node::set_articulation_parameter_value7_listener::
    ~set_articulation_parameter_value7_listener() OPENVRML_NOTHROW
    {}

    void espdu_transform_node::set_articulation_parameter_value7_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    espdu_transform_node::
    espdu_transform_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        set_articulation_parameter_value0_listener_(*this),
        set_articulation_parameter_value1_listener_(*this),
        set_articulation_parameter_value2_listener_(*this),
        set_articulation_parameter_value3_listener_(*this),
        set_articulation_parameter_value4_listener_(*this),
        set_articulation_parameter_value5_listener_(*this),
        set_articulation_parameter_value6_listener_(*this),
        set_articulation_parameter_value7_listener_(*this),
        address_(*this, "localhost"),
        application_id_(*this, 1),
        articulation_parameter_count_(*this),
        articulation_parameter_designator_array_(*this),
        articulation_parameter_change_indicator_array_(*this),
        articulation_parameter_id_part_attached_to_array_(*this),
        articulation_parameter_type_array_(*this),
        articulation_parameter_array_(*this),
        center_(*this),
        children_(*this),
        collision_type_(*this),
        dead_reckoning_(*this),
        detonation_location_(*this),
        detonation_relative_location_(*this),
        detonation_result_(*this),
        entity_category_(*this),
        entity_country_(*this),
        entity_domain_(*this),
        entity_extra_(*this),
        entity_id_(*this),
        entity_kind_(*this),
        entity_specific_(*this),
        entity_sub_category_(*this),
        event_application_id_(*this, 1),
        event_entity_id_(*this),
        event_number_(*this),
        event_site_id_(*this),
        fired1_(*this),
        fired2_(*this),
        fire_mission_index_(*this),
        firing_range_(*this),
        firing_rate_(*this),
        force_id_(*this),
        fuse_(*this),
        linear_velocity_(*this),
        linear_acceleration_(*this),
        marking_(*this),
        multicast_relay_host_(*this),
        multicast_relay_port_(*this),
        munition_application_id_(*this, 1),
        munition_end_point_(*this),
        munition_entity_id_(*this),
        munition_quantity_(*this),
        munition_site_id_(*this),
        munition_start_point_(*this),
        network_mode_(*this, "standAlone"),
        port_(*this),
        read_interval_(*this, 0.1f),
        rotation_(*this),
        scale_(*this, vec3f(1,1,1)),
        scale_orientation_(*this),
        site_id_(*this),
        translation_(*this),
        warhead_(*this),
        write_interval_(*this, 1),
        articulation_parameter_value0_changed_emitter_(*this, this->articulation_parameter_value0_changed_),
        articulation_parameter_value1_changed_emitter_(*this, this->articulation_parameter_value1_changed_),
        articulation_parameter_value2_changed_emitter_(*this, this->articulation_parameter_value2_changed_),
        articulation_parameter_value3_changed_emitter_(*this, this->articulation_parameter_value3_changed_),
        articulation_parameter_value4_changed_emitter_(*this, this->articulation_parameter_value4_changed_),
        articulation_parameter_value5_changed_emitter_(*this, this->articulation_parameter_value5_changed_),
        articulation_parameter_value6_changed_emitter_(*this, this->articulation_parameter_value6_changed_),
        articulation_parameter_value7_changed_emitter_(*this, this->articulation_parameter_value7_changed_),
        collide_time_emitter_(*this, this->collide_time_),
        detonate_time_emitter_(*this, this->detonate_time_),
        fired_time_emitter_(*this, this->fired_time_),
        is_active_emitter_(*this, this->is_active_),
        is_collided_emitter_(*this, this->is_collided_),
        is_detonated_emitter_(*this, this->is_detonated_),
        is_network_reader_emitter_(*this, this->is_network_reader_),
        is_network_writer_emitter_(*this, this->is_network_writer_),
        is_rtp_header_heard_emitter_(*this, this->is_rtp_header_heard_),
        is_stand_alone_emitter_(*this, this->is_stand_alone_),
        timestamp_emitter_(*this, this->timestamp_),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    espdu_transform_node::~espdu_transform_node() OPENVRML_NOTHROW
    {}


    /**
     * @class receiver_pdu_node
     *
     * @brief Represents ReceiverPdu node instances.
     */

    /**
     * @var receiver_pdu_node::receiver_pdu_metatype
     *
     * @brief Class object for ReceiverPdu nodes.
     */

    /**
     * @var receiver_pdu_node::address_
     *
     * @brief address exposedField
     */

    /**
     * @var receiver_pdu_node::application_id_
     *
     * @brief application_id exposedField
     */

    /**
     * @var receiver_pdu_node::entity_id_
     *
     * @brief entity_id exposedField
     */

    /**
     * @var receiver_pdu_node::multicast_relay_host_
     *
     * @brief multicast_relay_host exposedField
     */

    /**
     * @var receiver_pdu_node::multicast_relay_port_
     *
     * @brief multicast_relay_port exposedField
     */

    /**
     * @var receiver_pdu_node::network_mode_
     *
     * @brief network_mode exposedField
     */

    /**
     * @var receiver_pdu_node::port_
     *
     * @brief port exposedField
     */

    /**
     * @var receiver_pdu_node::radio_id_
     *
     * @brief radio_id exposedField
     */

    /**
     * @var receiver_pdu_node::read_interval_
     *
     * @brief read_interval exposedField
     */

    /**
     * @var receiver_pdu_node::received_power_
     *
     * @brief received_power exposedField
     */

    /**
     * @var receiver_pdu_node::receiver_state_
     *
     * @brief receiver_state exposedField
     */

    /**
     * @var receiver_pdu_node::rtp_header_expected_
     *
     * @brief rtp_header_expected exposedField
     */

    /**
     * @var receiver_pdu_node::site_id_
     *
     * @brief site_id exposedField
     */

    /**
     * @var receiver_pdu_node::transmitter_application_id_
     *
     * @brief transmitter_application_id exposedField
     */

    /**
     * @var receiver_pdu_node::transmitter_entity_id_
     *
     * @brief transmitter_entity_id exposedField
     */

    /**
     * @var receiver_pdu_node::transmitter_radio_id_
     *
     * @brief transmitter_radio_id exposedField
     */

    /**
     * @var receiver_pdu_node::transmitter_site_id_
     *
     * @brief transmitter_site_id exposedField
     */

    /**
     * @var receiver_pdu_node::which_geometry_
     *
     * @brief which_geometry exposedField
     */

    /**
     * @var receiver_pdu_node::write_interval_
     *
     * @brief write_interval exposedField
     */

    /**
     * @var receiver_pdu_node::is_active_
     *
     * @brief is_active eventOut
     */

    /**
     * @var receiver_pdu_node::is_network_reader_
     *
     * @brief is_network_reader eventOut
     */

    /**
     * @var receiver_pdu_node::is_network_writer_
     *
     * @brief is_network_writer eventOut
     */

    /**
     * @var receiver_pdu_node::is_rtp_header_heard_
     *
     * @brief is_rtp_header_heard eventOut
     */

    /**
     * @var receiver_pdu_node::is_stand_alone_
     *
     * @brief is_stand_alone eventOut
     */

    /**
     * @var receiver_pdu_node::timestamp_
     *
     * @brief timestamp eventOut
     */

    /**
     * @var receiver_pdu_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var receiver_pdu_node::bbox_size_
     *
     * @brief bbox_size field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    receiver_pdu_node::
    receiver_pdu_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        address_(*this, "localhost"),
        application_id_(*this, 1),
        entity_id_(*this),
        multicast_relay_host_(*this),
        multicast_relay_port_(*this),
        network_mode_(*this, "standAlone"),
        port_(*this),
        radio_id_(*this),
        read_interval_(*this, 0.1f),
        received_power_(*this),
        receiver_state_(*this),
        rtp_header_expected_(*this),
        site_id_(*this),
        transmitter_application_id_(*this, 1),
        transmitter_entity_id_(*this),
        transmitter_radio_id_(*this),
        transmitter_site_id_(*this),
        which_geometry_(*this, 1),
        write_interval_(*this, 1),
        is_active_emitter_(*this, this->is_active_),
        is_network_reader_emitter_(*this, this->is_network_reader_),
        is_network_writer_emitter_(*this, this->is_network_writer_),
        is_rtp_header_heard_emitter_(*this, this->is_rtp_header_heard_),
        is_stand_alone_emitter_(*this, this->is_stand_alone_),
        timestamp_emitter_(*this, this->timestamp_),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    receiver_pdu_node::~receiver_pdu_node() OPENVRML_NOTHROW
    {}


    /**
     * @class signal_pdu_node
     *
     * @brief Represents SignalPdu node instances.
     */

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
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    signal_pdu_node::~signal_pdu_node() OPENVRML_NOTHROW
    {}


    /**
     * @class transmitter_pdu_node
     *
     * @brief Represents TransmitterPdu node instances.
     */

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
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    transmitter_pdu_node::~transmitter_pdu_node() OPENVRML_NOTHROW
    {}
}
