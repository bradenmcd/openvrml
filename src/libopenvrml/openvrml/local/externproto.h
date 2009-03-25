// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2009  Braden McDaniel
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

# ifndef OPENVRML_LOCAL_EXTERNPROTO_H
#   define OPENVRML_LOCAL_EXTERNPROTO_H

#   include <openvrml/local/proto.h>
#   include <boost/enable_shared_from_this.hpp>
#   include <boost/thread/thread.hpp>

namespace openvrml {

    namespace local {

        class externproto_node_type;

        /**
         * @brief @c node_metatype for @c EXTERNPROTO%s.
         */
        class OPENVRML_LOCAL externproto_node_metatype :
            public boost::enable_shared_from_this<externproto_node_metatype>,
            public openvrml::node_metatype {

            friend class externproto_node_type;

            struct load_proto;

            mutable boost::mutex mutex_;
            boost::weak_ptr<proto_node_metatype> proto_node_metatype_;

            typedef std::vector<boost::weak_ptr<externproto_node_type> >
                externproto_node_types;

            mutable externproto_node_types externproto_node_types_;
            bool externproto_node_types_cleared_;

            boost::thread * const load_proto_thread_;

        public:
            externproto_node_metatype(
                const openvrml::node_metatype_id & id,
                const openvrml::scene & scene,
                const std::vector<std::string> & uris,
                boost::thread_group & load_proto_thread_group)
                OPENVRML_THROW2(boost::thread_resource_error, std::bad_alloc);
            virtual ~externproto_node_metatype() OPENVRML_NOTHROW;

        private:
            virtual const boost::shared_ptr<openvrml::node_type>
            do_create_type(const std::string & id,
                           const openvrml::node_interface_set & interfaces) const
                OPENVRML_THROW2(openvrml::unsupported_interface,
                                std::bad_alloc);

            virtual void do_shutdown(double time) OPENVRML_NOTHROW;

            void set_proto_node_metatype(
                const boost::weak_ptr<proto_node_metatype> & metatype =
                    boost::weak_ptr<proto_node_metatype>())
                OPENVRML_THROW1(std::bad_alloc);

            void clear_externproto_node_types() OPENVRML_NOTHROW;
        };


        class externproto_node;

        class OPENVRML_LOCAL externproto_node_type :
            public openvrml::node_type {

            const boost::shared_ptr<const externproto_node_metatype>
                node_metatype_;

            openvrml::node_interface_set interfaces_;

            mutable boost::mutex mutex_;
            boost::shared_ptr<openvrml::node_type> proto_node_type_;

            typedef std::vector<boost::intrusive_ptr<externproto_node> >
                externproto_nodes;

            mutable externproto_nodes externproto_nodes_;

        public:
            externproto_node_type(
                const boost::shared_ptr<const externproto_node_metatype> & c,
                const std::string & id,
                const openvrml::node_interface_set & interfaces)
                OPENVRML_THROW1(std::bad_alloc);

            virtual ~externproto_node_type() OPENVRML_NOTHROW;

            void set_proto_node_type(proto_node_metatype & proto_node_metatype)
                OPENVRML_THROW1(std::bad_alloc);

            void clear_externproto_nodes() OPENVRML_NOTHROW;

        private:
            virtual const openvrml::node_interface_set & do_interfaces() const
                OPENVRML_NOTHROW;

            virtual const boost::intrusive_ptr<openvrml::node>
            do_create_node(
                const boost::shared_ptr<openvrml::scope> & scope,
                const openvrml::initial_value_map & initial_values) const
                OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                                std::bad_alloc);
        };


        class OPENVRML_LOCAL externproto_node :
            public openvrml::local::abstract_proto_node {

            template <typename FieldValue>
            class externproto_exposedfield : public FieldValue,
                                             public proto_eventin<FieldValue>,
                                             public proto_eventout<FieldValue> {
            public:
                explicit externproto_exposedfield(
                    externproto_node & node,
                    const FieldValue & initial_value = FieldValue())
                    OPENVRML_NOTHROW;
                virtual ~externproto_exposedfield() OPENVRML_NOTHROW;

            private:
                virtual std::auto_ptr<openvrml::field_value> do_clone() const
                    OPENVRML_THROW1(std::bad_alloc);
                virtual void do_process_event(const FieldValue & value,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
            };

            struct externproto_exposedfield_creator {
                externproto_exposedfield_creator(
                    externproto_node & node,
                    const openvrml::field_value::type_id type,
                    boost::shared_ptr<openvrml::field_value> & exposedfield):
                    node_(&node),
                    type_(type),
                    exposedfield_(&exposedfield)
                {}

                template <typename T>
                void operator()(T) const
                {
                    if (T::field_value_type_id == this->type_) {
                        this->exposedfield_->reset(
                            new externproto_exposedfield<T>(*this->node_));
                    }
                }

            private:
                externproto_node * node_;
                openvrml::field_value::type_id type_;
                boost::shared_ptr<openvrml::field_value> * exposedfield_;
            };

            static const boost::shared_ptr<openvrml::field_value>
            create_exposedfield(externproto_node & node,
                                openvrml::field_value::type_id type)
                OPENVRML_THROW1(std::bad_alloc);

            typedef std::map<std::string,
                             boost::shared_ptr<openvrml::field_value> >
                field_map;

            struct externproto_field_equal_to :
                std::unary_function<field_map::value_type, bool> {

                explicit externproto_field_equal_to(
                    const openvrml::field_value & field_value):
                    field_value_(&field_value)
                {}

                bool operator()(const field_map::value_type & arg) const
                {
                    return this->field_value_ == arg.second.get();
                }

            private:
                const openvrml::field_value * field_value_;
            };

            initial_value_map initial_values_;
            std::set<openvrml::field_value *> received_event_;
            field_map field_map_;

            boost::intrusive_ptr<openvrml::node> proto_node_;

        public:
            externproto_node(const externproto_node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope,
                             const openvrml::initial_value_map & initial_values)
                OPENVRML_THROW1(std::bad_alloc);

            virtual ~externproto_node() OPENVRML_NOTHROW;

            virtual bool modified() const;

            void set_proto_node(node_type & type)
                OPENVRML_THROW1(std::bad_alloc);

        private:
            virtual
            const std::vector<boost::intrusive_ptr<node> > &
            do_impl_nodes() const
                OPENVRML_NOTHROW;

            virtual void do_initialize(double timestamp)
                OPENVRML_THROW1(std::bad_alloc);

            virtual const openvrml::field_value &
            do_field(const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface);

            virtual openvrml::event_listener &
            do_event_listener(const std::string & id)
                OPENVRML_THROW1(openvrml::unsupported_interface);

            virtual openvrml::event_emitter &
            do_event_emitter(const std::string & id)
                OPENVRML_THROW1(openvrml::unsupported_interface);

            virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;

            virtual openvrml::script_node * to_script() OPENVRML_NOTHROW;
            virtual openvrml::appearance_node * to_appearance()
                OPENVRML_NOTHROW;
            virtual openvrml::bounded_volume_node * to_bounded_volume()
                OPENVRML_NOTHROW;
            virtual openvrml::child_node * to_child() OPENVRML_NOTHROW;
            virtual openvrml::color_node * to_color() OPENVRML_NOTHROW;
            virtual openvrml::color_rgba_node * to_color_rgba()
                OPENVRML_NOTHROW;
            virtual openvrml::coordinate_node * to_coordinate()
                OPENVRML_NOTHROW;
            virtual openvrml::font_style_node * to_font_style()
                OPENVRML_NOTHROW ;
            virtual openvrml::geometry_node * to_geometry() OPENVRML_NOTHROW;
            virtual openvrml::grouping_node * to_grouping() OPENVRML_NOTHROW;
            virtual openvrml::light_node * to_light() OPENVRML_NOTHROW;
            virtual openvrml::material_node * to_material() OPENVRML_NOTHROW;
            virtual openvrml::navigation_info_node * to_navigation_info()
                OPENVRML_NOTHROW;
            virtual openvrml::normal_node * to_normal() OPENVRML_NOTHROW;
            virtual openvrml::pointing_device_sensor_node *
            to_pointing_device_sensor() OPENVRML_NOTHROW;
            virtual openvrml::scoped_light_node * to_scoped_light()
                OPENVRML_NOTHROW;
            virtual openvrml::sound_source_node * to_sound_source()
                OPENVRML_NOTHROW;
            virtual openvrml::texture_node * to_texture() OPENVRML_NOTHROW;
            virtual openvrml::texture_coordinate_node * to_texture_coordinate()
                OPENVRML_NOTHROW;
            virtual openvrml::texture_transform_node * to_texture_transform()
                OPENVRML_NOTHROW;
            virtual openvrml::time_dependent_node * to_time_dependent()
                OPENVRML_NOTHROW;
            virtual openvrml::transform_node * to_transform() OPENVRML_NOTHROW;
            virtual openvrml::viewpoint_node * to_viewpoint() OPENVRML_NOTHROW;
        };
    }
}

# endif // ifndef OPENVRML_LOCAL_EXTERNPROTO_H
