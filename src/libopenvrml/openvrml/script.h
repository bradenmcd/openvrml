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
}

# endif
