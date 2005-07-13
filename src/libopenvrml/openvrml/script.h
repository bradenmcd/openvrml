// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004  Braden McDaniel
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

# ifndef OPENVRML_SCRIPT_H
#   define OPENVRML_SCRIPT_H

#   include <boost/scoped_ptr.hpp>
#   include <openvrml/node.h>
#   include <openvrml/event.h>

namespace openvrml {

    class script_node;

    class script : boost::noncopyable {
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

        bool direct_output() const throw ();
        bool must_evaluate() const throw ();
        void field(const std::string & id, const field_value & value)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        void direct_output(event_listener & listener,
                           const boost::shared_ptr<field_value> & value)
            throw (field_value_type_mismatch, std::bad_alloc);

    private:
        virtual void do_initialize(double timestamp) = 0;
        virtual void do_process_event(const std::string & id,
                                      const field_value & value,
                                      double timestamp) = 0;
        virtual void do_events_processed(double timestamp) = 0;
        virtual void do_shutdown(double timestamp) = 0;

        void process_direct_output(double timestamp);
    };


    class script_node_class : public node_class {
    public:
        script_node_class(openvrml::browser & browser);
        virtual ~script_node_class() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            throw ();
    };


    class script_node : public child_node {
        friend class script;

    public:
        typedef std::map<std::string, boost::shared_ptr<field_value> >
            field_value_map_t;

        class eventout : boost::noncopyable {
            script_node & node_;
            boost::scoped_ptr<field_value> value_;
            bool modified_;
            boost::scoped_ptr<openvrml::event_emitter> emitter_;

        public:
            eventout(field_value::type_id type, script_node & node)
                throw (std::bad_alloc);

            const field_value & value() const throw ();
            void value(const field_value & val)
                throw (std::bad_alloc, std::bad_cast);

            bool modified() const throw ();

            openvrml::event_emitter & emitter() throw ();

            void emit_event(double timestamp) throw (std::bad_alloc);
        };

        typedef boost::shared_ptr<eventout> eventout_ptr;
        typedef std::map<std::string, eventout_ptr> eventout_map_t;

    private:
        class script_node_type : public node_type {
            node_interface_set interfaces_;

        public:
            explicit script_node_type(script_node_class & class_);
            virtual ~script_node_type() throw ();

            void add_interface(const node_interface & interface)
                throw (std::bad_alloc, std::invalid_argument);

        private:
            virtual const node_interface_set & do_interfaces() const throw ();
            virtual const node_ptr
            do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                           const initial_value_map & initial_values) const
                throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        };

        template <typename FieldValue>
        class script_event_listener : public field_value_listener<FieldValue> {
            const std::string id;

        public:
            script_event_listener(const std::string & id, script_node & node);
            virtual ~script_event_listener() throw ();

        private:
            virtual const std::string do_eventin_id() const throw ();
            virtual void do_process_event(const FieldValue & value,
                                          double timestamp)
                throw (std::bad_alloc);
        };

        typedef script_event_listener<sfbool> sfbool_listener;
        typedef script_event_listener<sfcolor> sfcolor_listener;
        typedef script_event_listener<sffloat> sffloat_listener;
        typedef script_event_listener<sfimage> sfimage_listener;
        typedef script_event_listener<sfint32> sfint32_listener;
        typedef script_event_listener<sfnode> sfnode_listener;
        typedef script_event_listener<sfrotation> sfrotation_listener;
        typedef script_event_listener<sfstring> sfstring_listener;
        typedef script_event_listener<sftime> sftime_listener;
        typedef script_event_listener<sfvec2f> sfvec2f_listener;
        typedef script_event_listener<sfvec3f> sfvec3f_listener;
        typedef script_event_listener<mfcolor> mfcolor_listener;
        typedef script_event_listener<mffloat> mffloat_listener;
        typedef script_event_listener<mfint32> mfint32_listener;
        typedef script_event_listener<mfnode> mfnode_listener;
        typedef script_event_listener<mfrotation> mfrotation_listener;
        typedef script_event_listener<mfstring> mfstring_listener;
        typedef script_event_listener<mftime> mftime_listener;
        typedef script_event_listener<mfvec2f> mfvec2f_listener;
        typedef script_event_listener<mfvec3f> mfvec3f_listener;

        static const boost::shared_ptr<openvrml::event_listener>
        create_listener(field_value::type_id type, const std::string & id,
                        script_node & node)
            throw (std::bad_alloc);

        template <typename FieldValue>
        class script_event_emitter :
            public openvrml::field_value_emitter<FieldValue> {
            BOOST_CLASS_REQUIRE(FieldValue, openvrml, FieldValueConcept);

            script_node * node_;

            struct event_emitter_equal_to :
                std::unary_function<typename eventout_map_t::value_type, bool>
            {
                explicit event_emitter_equal_to(
                    const script_event_emitter<FieldValue> & emitter)
                    throw ():
                    emitter_(&emitter)
                {}

                bool operator()(
                    const typename eventout_map_t::value_type & arg) const
                {
                    return this->emitter_ == &arg.second->emitter();
                }

            private:
                const script_event_emitter * emitter_;
            };

        public:
            script_event_emitter(script_node & node, const FieldValue & value)
                throw ();
            virtual ~script_event_emitter() throw ();

        private:
            virtual const std::string do_eventout_id() const throw ();
        };

        static std::auto_ptr<openvrml::event_emitter>
        create_emitter(script_node & node, const field_value & value)
            throw (std::bad_alloc);

        class set_url_listener_t : public openvrml::mfstring_listener {
        public:
            explicit set_url_listener_t(script_node & node);
            virtual ~set_url_listener_t() throw ();

        private:
            virtual const std::string do_eventin_id() const throw ();
            virtual void do_process_event(const mfstring & value,
                                          double timestamp)
                throw (std::bad_alloc);
        };

        class url_changed_emitter : public openvrml::mfstring_emitter {
        public:
            explicit url_changed_emitter(const mfstring & value) throw ();
            virtual ~url_changed_emitter() throw ();

        private:
            virtual const std::string do_eventout_id() const throw ();
        };

        script_node_type type;
        sfbool direct_output;
        sfbool must_evaluate;
        set_url_listener_t set_url_listener;
        mfstring url_;
        url_changed_emitter url_changed_emitter_;
        field_value_map_t field_value_map_;
        typedef boost::shared_ptr<openvrml::event_listener> event_listener_ptr;
        typedef std::map<std::string, event_listener_ptr> event_listener_map_t;
        event_listener_map_t event_listener_map;
        eventout_map_t eventout_map_;
        script * script_;
        int events_received;

    public:
        script_node(script_node_class & class_,
                    const boost::shared_ptr<openvrml::scope> & scope,
                    const node_interface_set & interfaces,
                    const initial_value_map & initial_values)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc,
                   std::invalid_argument);
        virtual ~script_node() throw ();

        void update(double current_time);

        const field_value_map_t & field_value_map() const throw ();
        const eventout_map_t & eventout_map() const throw ();

    private:
        script * create_script();

        void assign_with_self_ref_check(const sfnode &, sfnode &) const
            throw ();
        void assign_with_self_ref_check(const mfnode &, mfnode &) const
            throw ();

        virtual script_node * to_script() throw ();

        virtual void do_initialize(double timestamp) throw (std::bad_alloc);
        virtual const field_value & do_field(const std::string & id) const
            throw (unsupported_interface);
        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            throw (unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            throw (unsupported_interface);
        virtual void do_shutdown(double timestamp) throw ();
        virtual void do_render_child(viewer & v, rendering_context context);
    };

    template <typename FieldValue>
    script_node::script_event_listener<FieldValue>::script_event_listener(
        const std::string & id,
        script_node & node):
        openvrml::event_listener(node),
        field_value_listener<FieldValue>(node),
        id(id)
    {}

    template <typename FieldValue>
    script_node::script_event_listener<FieldValue>::~script_event_listener()
        throw ()
    {}

    template <typename FieldValue>
    const std::string
    script_node::script_event_listener<FieldValue>::do_eventin_id() const
        throw ()
    {
        return this->id;
    }

    template <typename FieldValue>
    void script_node::script_event_listener<FieldValue>::do_process_event(
        const FieldValue & value,
        const double timestamp)
        throw (std::bad_alloc)
    {
        assert(dynamic_cast<openvrml::script_node *>(&this->node()));
        openvrml::script_node & script_node =
            dynamic_cast<openvrml::script_node &>(this->node());
        if (script_node.script_) {
            script_node.script_->process_event(this->id, value, timestamp);
        }
        ++script_node.events_received;
    }

    template <typename FieldValue>
    script_node::script_event_emitter<FieldValue>::
    script_event_emitter(script_node & node,
                         const FieldValue & value)
        throw ():
        openvrml::event_emitter(value),
        openvrml::field_value_emitter<FieldValue>(value),
        node_(&node)
    {}

    template <typename FieldValue>
    script_node::script_event_emitter<FieldValue>::~script_event_emitter()
        throw ()
    {}

    template <typename FieldValue>
    const std::string
    script_node::script_event_emitter<FieldValue>::do_eventout_id() const
        throw ()
    {
        const eventout_map_t::const_iterator pos =
            std::find_if(this->node_->eventout_map_.begin(),
                         this->node_->eventout_map_.end(),
                         event_emitter_equal_to(*this));
        assert(pos != this->node_->eventout_map_.end());
        return pos->first;
    }
    

    inline const script_node::field_value_map_t &
    script_node::field_value_map() const throw ()
    {
        return this->field_value_map_;
    }

    inline const script_node::eventout_map_t &
    script_node::eventout_map() const throw ()
    {
        return this->eventout_map_;
    }
}

# endif
