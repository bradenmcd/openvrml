// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2001  Braden McDaniel
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

#   include <openvrml/field_value_ptr.h>
#   include <openvrml/node.h>

namespace openvrml {

    class script_node;

    class script {
    public:
        virtual ~script() = 0;
        virtual void initialize(double timestamp) = 0;
        virtual void process_event(const std::string & id,
                                   const field_value & value,
                                   double timestamp) = 0;
        virtual void events_processed(double timestamp) = 0;
        virtual void shutdown(double timestamp) = 0;

    protected:
        script_node & node;

        script(script_node & node);

    private:
        // non-copyable
        script(const script &);
        script & operator=(const script &);
    };


    class script_node_class : public node_class {
    public:
        script_node_class(openvrml::browser & browser);
        virtual ~script_node_class() throw ();

        virtual const node_type_ptr
        create_type(const std::string & id,
                    const node_interface_set & interfaces)
            throw ();
    };


    class script_node : public child_node {
    public:
        typedef std::map<std::string, field_value_ptr> field_value_map_t;
        typedef std::map<std::string, polled_eventout_value>
            eventout_value_map_t;

    private:
        class script_node_type : public node_type {
            node_interface_set interfaces_;

        public:
            script_node_type(script_node_class & class_);
            virtual ~script_node_type() throw ();

            void add_interface(const node_interface & interface)
                throw (std::invalid_argument);

            virtual const node_interface_set & interfaces() const throw ();
            virtual const node_ptr create_node(const scope_ptr & scope) const
                throw (std::bad_alloc);
        };

        friend class script_node_type;

        script_node_type type;
        sfbool direct_output;
        sfbool must_evaluate;
        mfstring url_;
        field_value_map_t field_value_map_;
        eventout_value_map_t eventout_value_map_;
        script * script_;
        int events_received;

    public:
        script_node(script_node_class & class_, const scope_ptr & scope)
            throw ();
        virtual ~script_node() throw ();

        void url(const mfstring & value, double timestamp);
        const mfstring & url() const;

        void add_eventin(field_value::type_id type, const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        void add_eventout(field_value::type_id type, const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        void add_field(const std::string & id,
                       const field_value_ptr & default_val)
            throw (std::invalid_argument, std::bad_alloc);

        void update(double current_time);

        void eventout(const std::string & id, const field_value & value)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);

        const field_value_map_t & field_value_map() const throw ();
        const eventout_value_map_t & eventout_value_map() const throw ();

        virtual const script_node * to_script() const throw ();
        virtual script_node * to_script() throw ();

    private:
        script * create_script();

        void assign_with_self_ref_check(const sfnode &, sfnode &) const
            throw ();
        void assign_with_self_ref_check(const mfnode &, mfnode &) const
            throw ();

        virtual void do_initialize(double timestamp) throw (std::bad_alloc);
        virtual void do_field(const std::string & id,
                              const field_value & value)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const field_value & do_field(const std::string & id) const
            throw (unsupported_interface);
        virtual void do_process_event(const std::string & id,
                                      const field_value & value,
                                      double timestamp)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const field_value &
        do_eventout(const std::string & id) const
            throw (unsupported_interface);
        virtual void do_shutdown(double timestamp) throw ();
    };

    inline const script_node::field_value_map_t &
    script_node::field_value_map() const throw ()
    {
        return this->field_value_map_;
    }

    inline const script_node::eventout_value_map_t &
    script_node::eventout_value_map() const throw ()
    {
        return this->eventout_value_map_;
    }
}

# endif
