// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008, 2009  Braden McDaniel
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

# include "espdu_transform.h"
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
     * @brief Represents EspduTransform node instances.
     */
    class OPENVRML_LOCAL espdu_transform_node :
        public abstract_node<espdu_transform_node>,
        public child_node {

        friend class openvrml_node_x3d_dis::espdu_transform_metatype;

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


    /**
     * @var class espdu_transform_node::espdu_transform_metatype
     *
     * @brief Class object for EspduTransform nodes.
     */

    /**
     * @var espdu_transform_node::add_children_listener espdu_transform_node::add_children_listener_
     *
     * @brief add_children eventIn
     */

    /**
     * @var espdu_transform_node::remove_children_listener espdu_transform_node::remove_children_listener_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value0_listener espdu_transform_node::set_articulation_parameter_value0_listener_
     *
     * @brief set_articulation_parameter_value0 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value1_listener espdu_transform_node::set_articulation_parameter_value1_listener_
     *
     * @brief set_articulation_parameter_value1 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value2_listener espdu_transform_node::set_articulation_parameter_value2_listener_
     *
     * @brief set_articulation_parameter_value2 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value3_listener espdu_transform_node::set_articulation_parameter_value3_listener_
     *
     * @brief set_articulation_parameter_value3 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value4_listener espdu_transform_node::set_articulation_parameter_value4_listener_
     *
     * @brief set_articulation_parameter_value4 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value5_listener espdu_transform_node::set_articulation_parameter_value5_listener_
     *
     * @brief set_articulation_parameter_value5 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value6_listener espdu_transform_node::set_articulation_parameter_value6_listener_
     *
     * @brief set_articulation_parameter_value6 eventIn
     */

    /**
     * @var espdu_transform_node::set_articulation_parameter_value7_listener espdu_transform_node::set_articulation_parameter_value7_listener_
     *
     * @brief set_articulation_parameter_value7 eventIn
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfstring> espdu_transform_node::address_
     *
     * @brief address exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::application_id_
     *
     * @brief application_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::articulation_parameter_count_
     *
     * @brief articulation_parameter_count exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::mfint32> espdu_transform_node::articulation_parameter_designator_array_
     *
     * @brief articulation_parameter_designator_array exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::mfint32> espdu_transform_node::articulation_parameter_change_indicator_array_
     *
     * @brief articulation_parameter_change_indicator_array exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::mfint32> espdu_transform_node::articulation_parameter_id_part_attached_to_array_
     *
     * @brief articulation_parameter_id_part_attached_to_array exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::mfint32> espdu_transform_node::articulation_parameter_type_array_
     *
     * @brief articulation_parameter_type_array exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::mffloat> espdu_transform_node::articulation_parameter_array_
     *
     * @brief articulation_parameter_array exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::center_
     *
     * @brief center exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::mfnode> espdu_transform_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::collision_type_
     *
     * @brief collision_type exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::dead_reckoning_
     *
     * @brief dead_reckoning exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::detonation_location_
     *
     * @brief detonation_location exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::detonation_relative_location_
     *
     * @brief detonation_relative_location exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::detonation_result_
     *
     * @brief detonation_result exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::entity_category_
     *
     * @brief entity_category exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::entity_country_
     *
     * @brief entity_country exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::entity_domain_
     *
     * @brief entity_domain exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::entity_extra_
     *
     * @brief entity_extra exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::entity_id_
     *
     * @brief entity_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::entity_kind_
     *
     * @brief entity_kind exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::entity_specific_
     *
     * @brief entity_specific exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::entity_sub_category_
     *
     * @brief entity_sub_category exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::event_application_id_
     *
     * @brief event_application_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::event_entity_id_
     *
     * @brief event_entity_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::event_number_
     *
     * @brief event_number exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::event_site_id_
     *
     * @brief event_site_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfbool> espdu_transform_node::fired1_
     *
     * @brief fired1 exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfbool> espdu_transform_node::fired2_
     *
     * @brief fired2 exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::fire_mission_index_
     *
     * @brief fire_mission_index exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sffloat> espdu_transform_node::firing_range_
     *
     * @brief firing_range exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::firing_rate_
     *
     * @brief firing_rate exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::force_id_
     *
     * @brief force_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::fuse_
     *
     * @brief fuse exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::linear_velocity_
     *
     * @brief linear_velocity exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::linear_acceleration_
     *
     * @brief linear_acceleration exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfstring> espdu_transform_node::marking_
     *
     * @brief marking exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfstring> espdu_transform_node::multicast_relay_host_
     *
     * @brief multicast_relay_host exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::multicast_relay_port_
     *
     * @brief multicast_relay_port exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::munition_application_id_
     *
     * @brief munition_application_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::munition_end_point_
     *
     * @brief munition_end_point exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::munition_entity_id_
     *
     * @brief munition_entity_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::munition_quantity_
     *
     * @brief munition_quantity exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::munition_site_id_
     *
     * @brief munition_site_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::munition_start_point_
     *
     * @brief munition_start_point exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfstring> espdu_transform_node::network_mode_
     *
     * @brief network_mode exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::port_
     *
     * @brief port exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sftime> espdu_transform_node::read_interval_
     *
     * @brief read_interval exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfrotation> espdu_transform_node::rotation_
     *
     * @brief rotation exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::scale_
     *
     * @brief scale exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfrotation> espdu_transform_node::scale_orientation_
     *
     * @brief scale_orientation exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::site_id_
     *
     * @brief site_id exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfvec3f> espdu_transform_node::translation_
     *
     * @brief translation exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sfint32> espdu_transform_node::warhead_
     *
     * @brief warhead exposedField
     */

    /**
     * @var abstract_node<espdu_transform_node>::exposedfield<openvrml::sftime> espdu_transform_node::write_interval_
     *
     * @brief write_interval exposedField
     */

    /**
     * @var openvrml::sffloat espdu_transform_node::articulation_parameter_value0_changed_
     *
     * @brief articulation_parameter_value0_changed eventOut
     */

    /**
     * @var abstract_node::sffloat_emitter espdu_transform_node::articulation_parameter_value0_changed_emitter_
     *
     * @brief articulation_parameter_value0_changed eventOut
     */

    /**
     * @var openvrml::sffloat espdu_transform_node::articulation_parameter_value1_changed_
     *
     * @brief articulation_parameter_value1_changed eventOut
     */

    /**
     * @var abstract_node::sffloat_emitter espdu_transform_node::articulation_parameter_value1_changed_emitter_
     *
     * @brief articulation_parameter_value1_changed eventOut
     */

    /**
     * @var openvrml::sffloat espdu_transform_node::articulation_parameter_value2_changed_
     *
     * @brief articulation_parameter_value2_changed eventOut
     */

    /**
     * @var abstract_node::sffloat_emitter espdu_transform_node::articulation_parameter_value2_changed_emitter_
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
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
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
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
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
    do_process_event(const sffloat & /* fraction */, double  /* timestamp */)
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
    do_process_event(const sffloat & /* fraction */, double /* timestamp */)
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
    do_process_event(const sffloat & /* fraction */, double /* timestamp */)
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
    do_process_event(const sffloat & /* fraction */, double /* timestamp */)
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
    do_process_event(const sffloat & /* fraction */, double /* timestamp */)
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
    do_process_event(const sffloat & /* fraction */, double /* timestamp */)
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
    do_process_event(const sffloat & /* fraction */, double /* timestamp */)
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
    do_process_event(const sffloat & /* fraction */, double /* timestamp */)
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
        scale_(*this, make_vec3f(1.0, 1.0, 1.0)),
        scale_orientation_(*this),
        site_id_(*this),
        translation_(*this),
        warhead_(*this),
        write_interval_(*this, 1),
        articulation_parameter_value0_changed_emitter_(
            *this, this->articulation_parameter_value0_changed_),
        articulation_parameter_value1_changed_emitter_(
            *this, this->articulation_parameter_value1_changed_),
        articulation_parameter_value2_changed_emitter_(
            *this, this->articulation_parameter_value2_changed_),
        articulation_parameter_value3_changed_emitter_(
            *this, this->articulation_parameter_value3_changed_),
        articulation_parameter_value4_changed_emitter_(
            *this, this->articulation_parameter_value4_changed_),
        articulation_parameter_value5_changed_emitter_(
            *this, this->articulation_parameter_value5_changed_),
        articulation_parameter_value6_changed_emitter_(
            *this, this->articulation_parameter_value6_changed_),
        articulation_parameter_value7_changed_emitter_(
            *this, this->articulation_parameter_value7_changed_),
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
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    espdu_transform_node::~espdu_transform_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_dis::espdu_transform_metatype::id =
    "urn:X-openvrml:node:EspduTransform";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c espdu_transform_metatype.
 */
openvrml_node_x3d_dis::espdu_transform_metatype::
espdu_transform_metatype(openvrml::browser & browser):
    node_metatype(espdu_transform_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_dis::espdu_transform_metatype::~espdu_transform_metatype()
    OPENVRML_NOTHROW
{}

# define ESPDU_TRANSFORM_INTERFACE_SEQ                                  \
    ((exposedfield, sfnode,     "metadata",                                   metadata)) \
    ((eventin,      mfnode,     "addChildren",                                add_children_listener_)) \
    ((eventin,      mfnode,     "removeChildren",                             remove_children_listener_)) \
    ((eventin,      sffloat,    "set_articulationParameterValue0",            set_articulation_parameter_value0_listener_)) \
    ((eventin,      sffloat,    "set_articulationParameterValue1",            set_articulation_parameter_value1_listener_)) \
    ((eventin,      sffloat,    "set_articulationParameterValue2",            set_articulation_parameter_value2_listener_)) \
    ((eventin,      sffloat,    "set_articulationParameterValue3",            set_articulation_parameter_value3_listener_)) \
    ((eventin,      sffloat,    "set_articulationParameterValue4",            set_articulation_parameter_value4_listener_)) \
    ((eventin,      sffloat,    "set_articulationParameterValue5",            set_articulation_parameter_value5_listener_)) \
    ((eventin,      sffloat,    "set_articulationParameterValue6",            set_articulation_parameter_value6_listener_)) \
    ((eventin,      sffloat,    "set_articulationParameterValue7",            set_articulation_parameter_value7_listener_)) \
    ((exposedfield, sfstring,   "address",                                    address_)) \
    ((exposedfield, sfint32,    "applicationID",                              application_id_)) \
    ((exposedfield, sfint32,    "articulationParameterCount",                 articulation_parameter_count_)) \
    ((exposedfield, mfint32,    "articulationParameterDesignatorArray",       articulation_parameter_designator_array_)) \
    ((exposedfield, mfint32,    "articulationParameterChangeIndicatorArray",  articulation_parameter_change_indicator_array_)) \
    ((exposedfield, mfint32,    "articulationParameterIdPartAttachedToArray", articulation_parameter_id_part_attached_to_array_)) \
    ((exposedfield, mfint32,    "articulationParameterTypeArray",             articulation_parameter_type_array_)) \
    ((exposedfield, mffloat,    "articulationParameterArray",                 articulation_parameter_array_)) \
    ((exposedfield, sfvec3f,    "center",                                     center_)) \
    ((exposedfield, mfnode,     "children",                                   children_)) \
    ((exposedfield, sfint32,    "collisionType",                              collision_type_)) \
    ((exposedfield, sfint32,    "deadReckoning",                              dead_reckoning_)) \
    ((exposedfield, sfvec3f,    "detonationLocation",                         detonation_location_)) \
    ((exposedfield, sfvec3f,    "detonationRelativeLocation",                 detonation_relative_location_)) \
    ((exposedfield, sfint32,    "detonationResult",                           detonation_result_)) \
    ((exposedfield, sfint32,    "entityCategory",                             entity_category_)) \
    ((exposedfield, sfint32,    "entityCountry",                              entity_country_)) \
    ((exposedfield, sfint32,    "entityDomain",                               entity_domain_)) \
    ((exposedfield, sfint32,    "entityExtra",                                entity_extra_)) \
    ((exposedfield, sfint32,    "entityID",                                   entity_id_)) \
    ((exposedfield, sfint32,    "entityKind",                                 entity_kind_)) \
    ((exposedfield, sfint32,    "entitySpecific",                             entity_specific_)) \
    ((exposedfield, sfint32,    "entitySubCategory",                          entity_sub_category_)) \
    ((exposedfield, sfint32,    "eventApplicationID",                         event_application_id_)) \
    ((exposedfield, sfint32,    "eventEntityID",                              event_entity_id_)) \
    ((exposedfield, sfint32,    "eventNumber",                                event_number_)) \
    ((exposedfield, sfint32,    "eventSiteID",                                event_site_id_)) \
    ((exposedfield, sfbool,     "fired1",                                     fired1_)) \
    ((exposedfield, sfbool,     "fired2",                                     fired2_)) \
    ((exposedfield, sfint32,    "fireMissionIndex",                           fire_mission_index_)) \
    ((exposedfield, sffloat,    "firingRange",                                firing_range_)) \
    ((exposedfield, sfint32,    "firingRate",                                 firing_rate_)) \
    ((exposedfield, sfint32,    "forceID",                                    force_id_)) \
    ((exposedfield, sfint32,    "fuse",                                       fuse_)) \
    ((exposedfield, sfvec3f,    "linearVelocity",                             linear_velocity_)) \
    ((exposedfield, sfvec3f,    "linearAcceleration",                         linear_acceleration_)) \
    ((exposedfield, sfstring,   "marking",                                    marking_)) \
    ((exposedfield, sfstring,   "multicastRelayHost",                         multicast_relay_host_)) \
    ((exposedfield, sfint32,    "multicastRelayPort",                         multicast_relay_port_)) \
    ((exposedfield, sfint32,    "munitionApplicationID",                      munition_application_id_)) \
    ((exposedfield, sfvec3f,    "munitionEndPoint",                           munition_end_point_)) \
    ((exposedfield, sfint32,    "munitionEntityID",                           munition_entity_id_)) \
    ((exposedfield, sfint32,    "munitionQuantity",                           munition_quantity_)) \
    ((exposedfield, sfint32,    "munitionSiteID",                             munition_site_id_)) \
    ((exposedfield, sfvec3f,    "munitionStartPoint",                         munition_start_point_)) \
    ((exposedfield, sfstring,   "networkMode",                                network_mode_)) \
    ((exposedfield, sfint32,    "port",                                       port_)) \
    ((exposedfield, sftime,     "readInterval",                               read_interval_)) \
    ((exposedfield, sfrotation, "rotation",                                   rotation_)) \
    ((exposedfield, sfvec3f,    "scale",                                      scale_)) \
    ((exposedfield, sfrotation, "scaleOrientation",                           scale_orientation_)) \
    ((exposedfield, sfint32,    "siteID",                                     site_id_)) \
    ((exposedfield, sfvec3f,    "translation",                                translation_)) \
    ((exposedfield, sfint32,    "warhead",                                    warhead_)) \
    ((exposedfield, sftime,     "writeInterval",                              write_interval_)) \
    ((eventout,     sffloat,    "articulationParameterValue0_changed",        articulation_parameter_value0_changed_emitter_)) \
    ((eventout,     sffloat,    "articulationParameterValue1_changed",        articulation_parameter_value1_changed_emitter_)) \
    ((eventout,     sffloat,    "articulationParameterValue2_changed",        articulation_parameter_value2_changed_emitter_)) \
    ((eventout,     sffloat,    "articulationParameterValue3_changed",        articulation_parameter_value3_changed_emitter_)) \
    ((eventout,     sffloat,    "articulationParameterValue4_changed",        articulation_parameter_value4_changed_emitter_)) \
    ((eventout,     sffloat,    "articulationParameterValue5_changed",        articulation_parameter_value5_changed_emitter_)) \
    ((eventout,     sffloat,    "articulationParameterValue6_changed",        articulation_parameter_value6_changed_emitter_)) \
    ((eventout,     sffloat,    "articulationParameterValue7_changed",        articulation_parameter_value7_changed_emitter_)) \
    ((eventout,     sftime,     "collideTime",                                collide_time_emitter_)) \
    ((eventout,     sftime,     "detonateTime",                               detonate_time_emitter_)) \
    ((eventout,     sftime,     "firedTime",                                  fired_time_emitter_)) \
    ((eventout,     sfbool,     "isActive",                                   is_active_emitter_)) \
    ((eventout,     sfbool,     "isCollided",                                 is_collided_emitter_)) \
    ((eventout,     sfbool,     "isDetonated",                                is_detonated_emitter_)) \
    ((eventout,     sfbool,     "isNetworkReader",                            is_network_reader_emitter_)) \
    ((eventout,     sfbool,     "isNetworkWriter",                            is_network_writer_emitter_)) \
    ((eventout,     sfbool,     "isRtpHeaderHeard",                           is_rtp_header_heard_emitter_)) \
    ((eventout,     sfbool,     "isStandAlone",                               is_stand_alone_emitter_)) \
    ((eventout,     sftime,     "timestamp",                                  timestamp_emitter_)) \
    ((field,        sfvec3f,    "bboxCenter",                                 bbox_center_)) \
    ((field,        sfvec3f,    "bboxSize",                                   bbox_size_)) \
    ((field,        sfbool,     "rtpHeaderExpected",                          rtp_header_expected_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_dis,
                                              espdu_transform_metatype,
                                              espdu_transform_node,
                                              ESPDU_TRANSFORM_INTERFACE_SEQ)
