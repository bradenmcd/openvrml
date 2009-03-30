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

# ifndef OPENVRML_NODE_VRML97_VIEWPOINT_H
#   define OPENVRML_NODE_VRML97_VIEWPOINT_H

#   include <openvrml/node_impl_util.h>

namespace openvrml_node_vrml97 {

    class viewpoint_node;

    /**
     * @brief Class object for Viewpoint nodes.
     */
    class OPENVRML_LOCAL viewpoint_metatype : public openvrml::node_metatype {
        viewpoint_node * first;
        openvrml::node_impl_util::bound_node_stack<viewpoint_node> bound_nodes;

    public:
        static const char * const id;

        explicit viewpoint_metatype(openvrml::browser & browser);
        virtual ~viewpoint_metatype() OPENVRML_NOTHROW;

        void set_first(viewpoint_node & viewpoint) OPENVRML_NOTHROW;
        void reset_first() OPENVRML_NOTHROW;
        bool has_first() const OPENVRML_NOTHROW;
        bool is_first(viewpoint_node & viewpoint) OPENVRML_NOTHROW;
        void bind(viewpoint_node & viewpoint, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        void unbind(viewpoint_node & viewpoint, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

    private:
        virtual void
        do_initialize(openvrml::viewpoint_node * initial_viewpoint,
                      double timestamp)
            OPENVRML_NOTHROW;
        virtual const boost::shared_ptr<openvrml::node_type>
        do_create_type(const std::string & id,
                       const openvrml::node_interface_set & interfaces) const
            OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);
    };


    class OPENVRML_LOCAL viewpoint_node :
        public openvrml::node_impl_util::abstract_node<viewpoint_node>,
        public openvrml::viewpoint_node {

        friend class viewpoint_metatype;

        class set_bind_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                  public sfbool_listener {
        public:
            explicit set_bind_listener(viewpoint_node & node);
            virtual ~set_bind_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sfbool & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class orientation_exposedfield : public exposedfield<openvrml::sfrotation> {
        public:
            explicit orientation_exposedfield(viewpoint_node & node);
            orientation_exposedfield(const orientation_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~orientation_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::sfrotation & orientation,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class position_exposedfield : public exposedfield<openvrml::sfvec3f> {
        public:
            explicit position_exposedfield(viewpoint_node & node);
            position_exposedfield(const position_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~position_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::sfvec3f & position,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_bind_listener set_bind_listener_;
        exposedfield<openvrml::sffloat> field_of_view_;
        exposedfield<openvrml::sfbool> jump_;
        orientation_exposedfield orientation_;
        position_exposedfield position_;
        // description is a field in VRML97 and an exposedField in X3D.
        exposedfield<openvrml::sfstring> description_;
        exposedfield<openvrml::sfvec3f> center_of_rotation_;
        openvrml::sfbool is_bound_;
        sfbool_emitter is_bound_emitter_;
        openvrml::sftime bind_time_;
        sftime_emitter bind_time_emitter_;

        openvrml::mat4f parent_transform;
        mutable openvrml::mat4f final_transformation;
        mutable bool final_transformation_dirty;
        openvrml::mat4f user_view_transform_;

    public:
        viewpoint_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~viewpoint_node() OPENVRML_NOTHROW;

        void bind(bool val, double timestamp) OPENVRML_THROW1(std::bad_alloc);

        const openvrml::sfrotation & orientation() const;
        const openvrml::sfvec3f & position() const;

    private:
        virtual void do_initialize(double timestamp) OPENVRML_NOTHROW;
        virtual void do_relocate() OPENVRML_THROW1(std::bad_alloc);
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;

        virtual const openvrml::mat4f & do_transformation() const
            OPENVRML_NOTHROW;
        virtual const openvrml::mat4f & do_user_view_transform() const
            OPENVRML_NOTHROW;
        virtual void do_user_view_transform(const openvrml::mat4f & transform)
            OPENVRML_NOTHROW;
        virtual const std::string & do_description() const OPENVRML_NOTHROW;
        virtual float do_field_of_view() const OPENVRML_NOTHROW;

        void update_final_transformation() const OPENVRML_NOTHROW;
    };
}

# endif // ifndef OPENVRML_NODE_VRML97_VIEWPOINT_H
