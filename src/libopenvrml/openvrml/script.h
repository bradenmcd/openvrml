// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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

# ifndef OPENVRML_SCRIPT_H
#   define OPENVRML_SCRIPT_H

#   include <openvrml/bad_url.h>
#   include <openvrml/node.h>
#   include <openvrml/event.h>

namespace openvrml {

    class script_node;

    class OPENVRML_API script : boost::noncopyable {
        typedef std::map<openvrml::event_listener *,
                         boost::shared_ptr<field_value> >
            direct_output_map_t;
        direct_output_map_t direct_output_map_;

    public:
        virtual ~script() = 0;
        void initialize(double timestamp);
        void process_event(const std::string & id,
                           const field_value & value,
                           double timestamp);
        void events_processed(double timestamp);
        void shutdown(double timestamp);

    protected:
        script_node & node;

        explicit script(script_node & node);

        bool direct_output() const OPENVRML_NOTHROW;
        bool must_evaluate() const OPENVRML_NOTHROW;
        void field(const std::string & id, const field_value & value)
            OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                            std::bad_alloc);
        void direct_output(event_listener & listener,
                           const boost::shared_ptr<field_value> & value)
            OPENVRML_THROW2(field_value_type_mismatch, std::bad_alloc);

    private:
        virtual void do_initialize(double timestamp) = 0;
        virtual void do_process_event(const std::string & id,
                                      const field_value & value,
                                      double timestamp) = 0;
        virtual void do_events_processed(double timestamp) = 0;
        virtual void do_shutdown(double timestamp) = 0;

        OPENVRML_LOCAL void process_direct_output(double timestamp);
    };


    class resource_istream;

    class OPENVRML_API script_factory : boost::noncopyable {
    public:
        virtual ~script_factory() OPENVRML_NOTHROW = 0;

        virtual std::auto_ptr<script>
        create_script(script_node & node,
                      const boost::shared_ptr<resource_istream> & source) = 0;

    protected:
        script_factory() OPENVRML_NOTHROW;
    };


    class OPENVRML_API script_factory_registry : boost::noncopyable {
        friend class script_node;

    public:
        class impl;

    private:
        boost::scoped_ptr<impl> impl_;

        script_factory_registry();
        ~script_factory_registry();

    public:
        bool register_factory(
            const std::set<std::string> & media_types,
            const std::set<std::string> & uri_schemes,
            const boost::shared_ptr<script_factory> & factory)
            OPENVRML_THROW2(std::bad_alloc, std::invalid_argument);
    };


    class OPENVRML_API script_node_metatype : public node_metatype {
    public:
        script_node_metatype(openvrml::browser & browser);
        virtual ~script_node_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_NOTHROW;
    };


    class OPENVRML_API script_node : public child_node {
        friend class script;

        static script_factory_registry script_factory_registry_;

    public:
        typedef std::map<std::string, boost::shared_ptr<field_value> >
            field_value_map_t;

        class OPENVRML_API eventout : boost::noncopyable {
            script_node & node_;
            boost::scoped_ptr<field_value> value_;
            bool modified_;
            boost::scoped_ptr<openvrml::event_emitter> emitter_;

        public:
            eventout(field_value::type_id type, script_node & node)
                OPENVRML_THROW1(std::bad_alloc);

            const field_value & value() const OPENVRML_NOTHROW;
            void value(const field_value & val)
                OPENVRML_THROW2(std::bad_alloc, std::bad_cast);

            bool modified() const OPENVRML_NOTHROW;

            openvrml::event_emitter & emitter() OPENVRML_NOTHROW;

            void emit_event(double timestamp) OPENVRML_THROW1(std::bad_alloc);
        };

        typedef boost::shared_ptr<openvrml::event_listener> event_listener_ptr;
        typedef std::map<std::string, event_listener_ptr> event_listener_map_t;
        typedef boost::shared_ptr<eventout> eventout_ptr;
        typedef std::map<std::string, eventout_ptr> eventout_map_t;

    private:
        class script_node_type : public node_type {
            node_interface_set interfaces_;

        public:
            explicit script_node_type(script_node_metatype & class_);
            virtual ~script_node_type() OPENVRML_NOTHROW;

            void add_interface(const node_interface & interface_)
                OPENVRML_THROW2(std::bad_alloc, std::invalid_argument);

        private:
            virtual const node_interface_set & do_interfaces() const
                OPENVRML_NOTHROW;
            virtual const boost::intrusive_ptr<node>
            do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                           const initial_value_map & initial_values) const
                OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                                std::bad_alloc);
        };

        template <typename FieldValue> class script_event_listener;

        typedef script_event_listener<sfbool> sfbool_listener;
        typedef script_event_listener<sfcolor> sfcolor_listener;
        typedef script_event_listener<sfcolorrgba> sfcolorrgba_listener;
        typedef script_event_listener<sffloat> sffloat_listener;
        typedef script_event_listener<sfdouble> sfdouble_listener;
        typedef script_event_listener<sfimage> sfimage_listener;
        typedef script_event_listener<sfint32> sfint32_listener;
        typedef script_event_listener<sfnode> sfnode_listener;
        typedef script_event_listener<sfrotation> sfrotation_listener;
        typedef script_event_listener<sfstring> sfstring_listener;
        typedef script_event_listener<sftime> sftime_listener;
        typedef script_event_listener<sfvec2f> sfvec2f_listener;
        typedef script_event_listener<sfvec2d> sfvec2d_listener;
        typedef script_event_listener<sfvec3f> sfvec3f_listener;
        typedef script_event_listener<sfvec3d> sfvec3d_listener;
        typedef script_event_listener<mfbool> mfbool_listener;
        typedef script_event_listener<mfcolor> mfcolor_listener;
        typedef script_event_listener<mfcolorrgba> mfcolorrgba_listener;
        typedef script_event_listener<mffloat> mffloat_listener;
        typedef script_event_listener<mfdouble> mfdouble_listener;
        typedef script_event_listener<mfimage> mfimage_listener;
        typedef script_event_listener<mfint32> mfint32_listener;
        typedef script_event_listener<mfnode> mfnode_listener;
        typedef script_event_listener<mfrotation> mfrotation_listener;
        typedef script_event_listener<mfstring> mfstring_listener;
        typedef script_event_listener<mftime> mftime_listener;
        typedef script_event_listener<mfvec2f> mfvec2f_listener;
        typedef script_event_listener<mfvec2d> mfvec2d_listener;
        typedef script_event_listener<mfvec3f> mfvec3f_listener;
        typedef script_event_listener<mfvec3d> mfvec3d_listener;

        struct script_event_listener_creator;

        static const boost::shared_ptr<openvrml::event_listener>
        create_listener(field_value::type_id type, const std::string & id,
                        script_node & node)
            OPENVRML_THROW1(std::bad_alloc);

        template <typename FieldValue> class script_event_emitter;

        struct script_event_emitter_creator;

        static std::auto_ptr<openvrml::event_emitter>
        create_emitter(script_node & node, const field_value & value)
            OPENVRML_THROW1(std::bad_alloc);

        class set_url_listener_t : public node_field_value_listener<mfstring> {
        public:
            explicit set_url_listener_t(script_node & node);
            virtual ~set_url_listener_t() OPENVRML_NOTHROW;

        private:
            virtual const std::string do_eventin_id() const OPENVRML_NOTHROW;
            virtual void do_process_event(const mfstring & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class url_changed_emitter : public openvrml::mfstring_emitter {
        public:
            explicit url_changed_emitter(const mfstring & value)
                OPENVRML_NOTHROW;
            virtual ~url_changed_emitter() OPENVRML_NOTHROW;

        private:
            virtual const std::string do_eventout_id() const OPENVRML_NOTHROW;
        };

        class set_metadata_listener :
            public node_field_value_listener<sfnode> {
        public:
            explicit set_metadata_listener(script_node & node);
            virtual ~set_metadata_listener() OPENVRML_NOTHROW;

        private:
            virtual const std::string do_eventin_id() const OPENVRML_NOTHROW;
            virtual void do_process_event(const sfnode & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class metadata_changed_emitter : public openvrml::sfnode_emitter {
        public:
            explicit metadata_changed_emitter(const sfnode & value)
                OPENVRML_NOTHROW;
            virtual ~metadata_changed_emitter() OPENVRML_NOTHROW;

        private:
            virtual const std::string do_eventout_id() const OPENVRML_NOTHROW;
        };

        script_node_type type_;
        set_metadata_listener set_metadata_listener_;
        sfnode metadata_;
        metadata_changed_emitter metadata_changed_emitter_;
        sfbool direct_output;
        sfbool must_evaluate;
        set_url_listener_t set_url_listener;
        mfstring url_;
        url_changed_emitter url_changed_emitter_;
        field_value_map_t field_value_map_;
        event_listener_map_t event_listener_map_;
        eventout_map_t eventout_map_;
        boost::scoped_ptr<script> script_;
        int events_received;

    public:
        script_node(script_node_metatype & class_,
                    const boost::shared_ptr<openvrml::scope> & scope,
                    const node_interface_set & interfaces,
                    const initial_value_map & initial_values)
            OPENVRML_THROW4(unsupported_interface, std::bad_cast,
                            std::bad_alloc, std::invalid_argument);
        virtual ~script_node() OPENVRML_NOTHROW;

        void update(double current_time);

        const event_listener_map_t & event_listener_map() const
            OPENVRML_NOTHROW;
        const field_value_map_t & field_value_map() const OPENVRML_NOTHROW;
        const eventout_map_t & eventout_map() const OPENVRML_NOTHROW;

    private:
        OPENVRML_LOCAL std::auto_ptr<script> create_script()
            OPENVRML_THROW2(no_alternative_url, std::bad_alloc);

        OPENVRML_LOCAL void assign_with_self_ref_check(const sfnode &,
                                                       sfnode &) const
            OPENVRML_NOTHROW;
        OPENVRML_LOCAL void assign_with_self_ref_check(const mfnode &,
                                                       mfnode &) const
            OPENVRML_NOTHROW;

        virtual script_node * to_script() OPENVRML_NOTHROW;

        virtual void do_initialize(double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        virtual const field_value & do_field(const std::string & id) const
            OPENVRML_THROW1(unsupported_interface);
        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(unsupported_interface);
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;
        virtual void do_render_child(viewer & v, rendering_context context);
    };
}

# endif
