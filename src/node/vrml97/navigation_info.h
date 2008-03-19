// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# ifndef OPENVRML_NODE_VRML97_NAVIGATION_INFO_H
#   define OPENVRML_NODE_VRML97_NAVIGATION_INFO_H

#   include <openvrml/node_impl_util.h>

namespace openvrml_node_vrml97 {

    class navigation_info_node;

    /**
     * @brief Class object for NavigationInfo nodes.
     */
    class OPENVRML_LOCAL navigation_info_metatype : public openvrml::node_metatype {
        navigation_info_node * first;
        openvrml::node_impl_util::bound_node_stack<navigation_info_node>
            bound_nodes;

    public:
        static const char * const id;

        explicit navigation_info_metatype(openvrml::browser & browser);
        virtual ~navigation_info_metatype() OPENVRML_NOTHROW;

        void set_first(navigation_info_node & nav_info) OPENVRML_NOTHROW;
        void reset_first() OPENVRML_NOTHROW;
        bool has_first() const OPENVRML_NOTHROW;
        bool is_first(navigation_info_node & nav_info) OPENVRML_NOTHROW;
        void bind(navigation_info_node & nav_info, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        void unbind(navigation_info_node & nav_info, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

    private:
        virtual void
        do_initialize(openvrml::viewpoint_node * initial_viewpoint,
                      double timestamp) OPENVRML_NOTHROW;
        virtual const boost::shared_ptr<openvrml::node_type>
        do_create_type(const std::string & id,
                       const openvrml::node_interface_set & interfaces) const
            OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);
    };


    class OPENVRML_LOCAL navigation_info_node :
        public openvrml::node_impl_util::abstract_node<navigation_info_node>,
        public openvrml::navigation_info_node {

        friend class navigation_info_metatype;

        class set_bind_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                  public sfbool_listener {
        public:
            explicit set_bind_listener(navigation_info_node & node);
            virtual ~set_bind_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sfbool & bind,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_bind_listener set_bind_listener_;
        exposedfield<openvrml::mffloat> avatar_size_;
        exposedfield<openvrml::sfbool> headlight_;
        exposedfield<openvrml::sffloat> speed_;
        exposedfield<openvrml::mfstring> type_;
        exposedfield<openvrml::sffloat> visibility_limit_;
        exposedfield<openvrml::mfstring> transition_type_;
        openvrml::sfbool is_bound_;
        sfbool_emitter is_bound_emitter_;
        openvrml::sftime bind_time_;
        sftime_emitter bind_time_emitter_;

    public:
        navigation_info_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~navigation_info_node() OPENVRML_NOTHROW;

        void bind(bool val, double timestamp) OPENVRML_THROW1(std::bad_alloc);

        virtual const std::vector<float> & avatar_size() const
            OPENVRML_NOTHROW;
        virtual bool headlight() const OPENVRML_NOTHROW;
        virtual float speed() const OPENVRML_NOTHROW;
        virtual const std::vector<std::string> & type() const OPENVRML_NOTHROW;
        virtual float visibility_limit() const OPENVRML_NOTHROW;

    private:
        virtual void do_initialize(double timestamp) OPENVRML_NOTHROW;
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;
    };
}

# endif // ifndef OPENVRML_NODE_VRML97_NAVIGATION_INFO_H
