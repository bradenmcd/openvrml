// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
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

# ifndef OPENVRML_NODE_VRML97_FOG_H
#   define OPENVRML_NODE_VRML97_FOG_H

#   include <openvrml/node_impl_util.h>

namespace openvrml_node_vrml97 {

    class fog_node;

    /**
     * @brief Class object for Fog nodes.
     */
    class OPENVRML_LOCAL fog_metatype : public openvrml::node_metatype {
        fog_node * first;
        openvrml::node_impl_util::bound_node_stack<fog_node> bound_nodes;

    public:
        static const char * const id;

        explicit fog_metatype(openvrml::browser & browser);
        virtual ~fog_metatype() OPENVRML_NOTHROW;

        void set_first(fog_node & fog) OPENVRML_NOTHROW;
        void reset_first() OPENVRML_NOTHROW;
        bool has_first() const OPENVRML_NOTHROW;
        bool is_first(fog_node & fog) OPENVRML_NOTHROW;
        void bind(fog_node & fog, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        void unbind(fog_node & fog, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

    private:
        virtual void
        do_initialize(openvrml::viewpoint_node * initialViewpoint,
                      double timestamp) OPENVRML_NOTHROW;
        virtual void do_render(openvrml::viewer & v) const OPENVRML_NOTHROW;
        virtual const boost::shared_ptr<openvrml::node_type>
        do_create_type(const std::string & id,
                       const openvrml::node_interface_set & interfaces) const
            OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);
    };


    class OPENVRML_LOCAL fog_node :
        public openvrml::node_impl_util::abstract_node<fog_node>,
        public openvrml::child_node {

        friend class fog_metatype;

        class set_bind_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                  public sfbool_listener {
        public:
            explicit set_bind_listener(fog_node & node);
            virtual ~set_bind_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sfbool & bind,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_bind_listener set_bind_listener_;
        exposedfield<openvrml::sfcolor> color_;
        exposedfield<openvrml::sfstring> fog_type_;
        exposedfield<openvrml::sffloat> visibility_range_;
        openvrml::sfbool is_bound_;
        sfbool_emitter is_bound_emitter_;
        openvrml::sftime bind_time_;
        sftime_emitter bind_time_emitter_;

    public:
        fog_node(const openvrml::node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~fog_node() OPENVRML_NOTHROW;

        void bind(bool val, double timestamp) OPENVRML_THROW1(std::bad_alloc);

    private:
        virtual void do_initialize(double timestamp) OPENVRML_NOTHROW;
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;
    };
}

# endif // ifndef OPENVRML_NODE_VRML97_FOG_H
