// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
// Copyright 2002  S. K. Bose
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

# include <openvrml/node_impl_util.h>

namespace openvrml_node_vrml97 {

    class background_node;

    /**
     * @brief Class object for Background nodes.
     */
    class OPENVRML_LOCAL background_metatype : public openvrml::node_metatype {
        background_node * first;
        openvrml::node_impl_util::bound_node_stack<background_node> bound_nodes;
        openvrml::null_node_metatype null_texture_node_metatype_;
        openvrml::null_node_type null_texture_node_type_;

    public:
        static const char * const id;

        explicit background_metatype(openvrml::browser & browser);
        virtual ~background_metatype() OPENVRML_NOTHROW;

        void set_first(background_node & background) OPENVRML_NOTHROW;
        void reset_first() OPENVRML_NOTHROW;
        bool has_first() const OPENVRML_NOTHROW;
        bool is_first(background_node & background) OPENVRML_NOTHROW;
        void bind(background_node & background, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        void unbind(background_node & background, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

    private:
        virtual void
        do_initialize(openvrml::viewpoint_node * initial_viewpoint,
                      double timestamp) OPENVRML_NOTHROW;
        virtual void do_render(openvrml::viewer & v) const OPENVRML_NOTHROW;
        virtual const boost::shared_ptr<openvrml::node_type>
        do_create_type(const std::string & id,
                       const openvrml::node_interface_set & interfaces) const
            OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);
    };


    class OPENVRML_LOCAL background_node :
        public openvrml::node_impl_util::abstract_node<background_node>,
        public openvrml::child_node {

        friend class background_metatype;

        class set_bind_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sfbool_listener {
        public:
            explicit set_bind_listener(background_node & node);
            virtual ~set_bind_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sfbool & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_bind_listener set_bind_listener_;
        exposedfield<openvrml::mffloat> ground_angle_;
        exposedfield<openvrml::mfcolor> ground_color_;
        exposedfield<openvrml::mfstring> back_url_, bottom_url_, front_url_,
            left_url_, right_url_, top_url_;
        exposedfield<openvrml::mffloat> sky_angle_;
        exposedfield<openvrml::mfcolor> sky_color_;
        openvrml::sfbool is_bound_;
        sfbool_emitter is_bound_emitter_;
        openvrml::sftime bind_time_;
        sftime_emitter bind_time_emitter_;

        boost::intrusive_ptr<openvrml::texture_node> front;
        boost::intrusive_ptr<openvrml::texture_node> back;
        boost::intrusive_ptr<openvrml::texture_node> left;
        boost::intrusive_ptr<openvrml::texture_node> right;
        boost::intrusive_ptr<openvrml::texture_node> top;
        boost::intrusive_ptr<openvrml::texture_node> bottom;

    public:
        background_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~background_node() OPENVRML_NOTHROW;

        void bind(bool val, double timestamp) OPENVRML_THROW1(std::bad_alloc);

    private:
        virtual void do_initialize(double timestamp) OPENVRML_NOTHROW;
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);
    };
}
