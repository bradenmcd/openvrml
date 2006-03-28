// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 79 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005  Braden McDaniel
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <cerrno>
# include <algorithm>
# include <functional>
# ifdef _WIN32
#   include <sys/timeb.h>
#   include <time.h>
# else
#   include <sys/time.h>
# endif
# include <boost/algorithm/string/predicate.hpp>
# include <boost/bind.hpp>
# include <boost/enable_shared_from_this.hpp>
# include <boost/functional.hpp>
# include <boost/mpl/for_each.hpp>
# include <boost/ptr_container/ptr_map.hpp>
# include <boost/spirit.hpp>
# include <boost/spirit/phoenix.hpp>
# include <boost/thread/thread.hpp>
# include <boost/utility.hpp>
# include <private.h>
# include "browser.h"
# include "vrml97node.h"
# include "x3d_core.h"
# include "x3d_networking.h"
# include "x3d_rendering.h"
# include "x3d_shape.h"
# include "x3d_geometry2d.h"
# include "x3d_texturing.h"
# include "x3d_key_device_sensor.h"
# include "x3d_event_utilities.h"

namespace openvrml {

    class OPENVRML_LOCAL null_node_class : public node_class {
    public:
        explicit null_node_class(openvrml::browser & browser) OPENVRML_NOTHROW;
        virtual ~null_node_class() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL null_node_type : public node_type {
    public:
        explicit null_node_type(null_node_class & nodeClass) OPENVRML_NOTHROW;
        virtual ~null_node_type() OPENVRML_NOTHROW;

    private:
        virtual const node_interface_set & do_interfaces() const
            OPENVRML_NOTHROW;
        virtual const boost::intrusive_ptr<node>
        do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                       const initial_value_map & initial_values) const
            OPENVRML_NOTHROW;
    };


    class proto_node;
    class proto_node_type;
    class proto_impl_cloner;

    /**
     * @internal
     *
     * @brief <code>node_class</code> for <code>PROTO</code>s.
     *
     * The <code>proto_node_class</code> is %OpenVRML's in-memory
     * representation of a <code>PROTO</code> (as opposed to a
     * <code>PROTO</code> instance).  Through the <code>proto_node_type</code>
     * intermediary, it facilitates spawning any number of
     * <code>proto_node</code> instances.
     */
    class OPENVRML_LOCAL proto_node_class : public node_class {
        friend class proto_node;
        friend class proto_node_type;
        friend class proto_impl_cloner;

    public:
        class is_target {
        public:
            node * impl_node;
            std::string impl_node_interface;

            is_target(node & impl_node,
                      const std::string & impl_node_interface);
        };

        typedef std::multimap<std::string, is_target> is_map_t;

        class route {
        public:
            node * from;
            std::string eventout;
            node * to;
            std::string eventin;

            route(node & from, const std::string & eventout,
                  node & to, const std::string & eventin);
        };

        typedef std::vector<route> routes_t;

        typedef std::map<std::string, boost::shared_ptr<field_value> >
            default_value_map_t;

    private:
        node_interface_set interfaces;
        default_value_map_t default_value_map;
        std::vector<boost::intrusive_ptr<node> > impl_nodes;
        routes_t routes;
        is_map_t is_map;

    public:
        proto_node_class(
            openvrml::browser & browser,
            const node_interface_set & interfaces,
            const default_value_map_t & default_value_map,
            const std::vector<boost::intrusive_ptr<node> > & impl_nodes,
            const is_map_t & is_map,
            const routes_t & routes);
        virtual ~proto_node_class() OPENVRML_NOTHROW;

        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @internal
     *
     * @brief <code>node_type</code> for PROTO node instances.
     */
    class OPENVRML_LOCAL proto_node_type : public node_type {
        node_interface_set interfaces_;

    public:
        proto_node_type(const proto_node_class & node_class,
                        const std::string & id,
                        const node_interface_set & interfaces)
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
        virtual ~proto_node_type() OPENVRML_NOTHROW;

    private:
        virtual const node_interface_set & do_interfaces() const
            OPENVRML_NOTHROW;
        virtual const boost::intrusive_ptr<node>
        do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                       const initial_value_map & initial_values) const
            OPENVRML_THROW1(std::bad_alloc);
    };


    class OPENVRML_LOCAL abstract_proto_node : public openvrml::node {
    public:
        virtual ~abstract_proto_node() OPENVRML_NOTHROW = 0;

    protected:
        template <typename FieldValue>
        class proto_eventin : public node_field_value_listener<FieldValue> {
            typedef std::set<field_value_listener<FieldValue> *> listeners;
            listeners listeners_;

        public:
            typedef FieldValue field_value_type;
            typedef field_value_listener<FieldValue> event_listener_type;

            explicit proto_eventin(abstract_proto_node & node);
            virtual ~proto_eventin() OPENVRML_NOTHROW;

            bool is(event_listener_type & listener)
                OPENVRML_THROW1(std::bad_alloc);

        protected:
            virtual void do_process_event(const FieldValue & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);

        private:
            virtual const std::string do_eventin_id() const OPENVRML_NOTHROW;
        };

        struct proto_eventin_creator {
            proto_eventin_creator(
                const openvrml::field_value::type_id type,
                abstract_proto_node & node,
                boost::shared_ptr<openvrml::event_listener> & listener):
                type_(type),
                node_(&node),
                listener_(&listener)
            {}

            template <typename T>
            void operator()(T) const
            {
                if (T::field_value_type_id == this->type_) {
                    this->listener_->reset(new proto_eventin<T>(*this->node_));
                }
            }

        private:
            openvrml::field_value::type_id type_;
            abstract_proto_node * node_;
            boost::shared_ptr<openvrml::event_listener> * listener_;
        };

        static boost::shared_ptr<openvrml::event_listener>
        create_eventin(field_value::type_id, abstract_proto_node & node)
            OPENVRML_THROW1(std::bad_alloc);

        struct eventin_is {
            eventin_is(const field_value::type_id field_type,
                       openvrml::event_listener & impl_eventin,
                       openvrml::event_listener & interface_eventin):
                type_(field_type),
                impl_eventin_(&impl_eventin),
                interface_eventin_(&interface_eventin)
            {}

            template <typename T>
            void operator()(T) const
            {
                if (T::field_value_type_id == this->type_) {
                    dynamic_cast<proto_eventin<T> &>(*interface_eventin_).is(
                        dynamic_cast<field_value_listener<T> &>(
                            *impl_eventin_));
                }
            }

        private:
            field_value::type_id type_;
            openvrml::event_listener * impl_eventin_;
            openvrml::event_listener * interface_eventin_;
        };

        template <typename FieldValue>
        class proto_eventout : public field_value_emitter<FieldValue> {
        protected:
            class listener_t : public node_field_value_listener<FieldValue> {
                proto_eventout & emitter;

            public:
                abstract_proto_node & node;
                FieldValue value;

                listener_t(proto_eventout & emitter,
                           abstract_proto_node & node,
                           const FieldValue & initial_value);
                virtual ~listener_t() OPENVRML_NOTHROW;

            private:
                virtual const std::string do_eventin_id() const
                    OPENVRML_NOTHROW;
                virtual void do_process_event(const FieldValue & value,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
            } listener;

        public:
            typedef FieldValue field_value_type;
            typedef field_value_emitter<FieldValue> event_emitter_type;
            typedef field_value_listener<FieldValue> event_listener_type;

            proto_eventout(
                abstract_proto_node & node,
                const FieldValue & initial_value = FieldValue());
            virtual ~proto_eventout() OPENVRML_NOTHROW;

            bool is(event_emitter_type & emitter)
                OPENVRML_THROW1(std::bad_alloc);

        private:
            const std::string do_eventout_id() const OPENVRML_NOTHROW;
        };

        struct proto_eventout_creator {
            proto_eventout_creator(
                const openvrml::field_value::type_id type,
                abstract_proto_node & node,
                boost::shared_ptr<openvrml::event_emitter> & emitter):
                type_(type),
                node_(&node),
                emitter_(&emitter)
            {}

            template <typename T>
            void operator()(T) const
            {
                if (T::field_value_type_id == this->type_) {
                    this->emitter_->reset(new proto_eventout<T>(*this->node_));
                }
            }

        private:
            openvrml::field_value::type_id type_;
            abstract_proto_node * node_;
            boost::shared_ptr<openvrml::event_emitter> * emitter_;
        };

        static boost::shared_ptr<openvrml::event_emitter>
        create_eventout(field_value::type_id, abstract_proto_node & node)
            OPENVRML_THROW1(std::bad_alloc);

        struct eventout_is {
            eventout_is(const field_value::type_id field_type,
                        openvrml::event_emitter & impl_eventout,
                        openvrml::event_emitter & interface_eventout):
                type_(field_type),
                impl_eventout_(&impl_eventout),
                interface_eventout_(&interface_eventout)
            {}

            template <typename T>
            void operator()(T) const
            {
                if (T::field_value_type_id == this->type_) {
                    dynamic_cast<proto_eventout<T> &>(*interface_eventout_).is(
                        dynamic_cast<field_value_emitter<T> &>(
                            *impl_eventout_));
                }
            }

        private:
            field_value::type_id type_;
            openvrml::event_emitter * impl_eventout_;
            openvrml::event_emitter * interface_eventout_;
        };

        typedef boost::shared_ptr<openvrml::event_listener> eventin_ptr;
        typedef std::map<std::string, eventin_ptr> eventin_map_t;

        struct proto_eventin_equal_to :
            std::unary_function<eventin_map_t::value_type, bool> {

            explicit proto_eventin_equal_to(
                const openvrml::event_listener & listener):
                listener_(&listener)
            {}

            bool operator()(const eventin_map_t::value_type & arg) const
            {
                return this->listener_ == arg.second.get();
            }

        private:
            const openvrml::event_listener * listener_;
        };

        eventin_map_t eventin_map;

        typedef boost::shared_ptr<openvrml::event_emitter> eventout_ptr;
        typedef std::map<std::string, eventout_ptr> eventout_map_t;

        struct proto_eventout_equal_to :
            std::unary_function<eventout_map_t::value_type, bool> {

            explicit proto_eventout_equal_to(
                const openvrml::event_emitter & emitter):
                emitter_(&emitter)
            {}

            bool operator()(const eventout_map_t::value_type & arg) const
            {
                return this->emitter_ == arg.second.get();
            }

        private:
            const openvrml::event_emitter * emitter_;
        };

        eventout_map_t eventout_map;

        abstract_proto_node(const openvrml::node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;
    };

    abstract_proto_node::abstract_proto_node(
        const openvrml::node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope)
        OPENVRML_NOTHROW:
        openvrml::node(type, scope)
    {}

    abstract_proto_node::~abstract_proto_node() OPENVRML_NOTHROW
    {}
} // namespace openvrml

namespace openvrml {

    class externproto_node;

    /**
     * @internal
     *
     * @brief A <code>PROTO</code> instance node.
     *
     * Like a typical node implementation, <code>proto_node</code>s have a
     * many-to-one relationship with the <code>proto_node_type</code> instance
     * that creates them. And <code>proto_node_type</code> has, in turn, a
     * many-to-one relationship with the <code>proto_node_class</code> instance
     * that creates them. Unlike a typical node implementation, there will very
     * likely be more than one <code>proto_node_class</code> instance known to
     * the <code>browser</code> instance; there will be one for each
     * <code>PROTO</code> known to the <code>browser</code>.
     *
     * As the <code>proto_node_class</code> encodes the data in a
     * <code>PROTO</code>, the <code>proto_node_type</code> can be seen as
     * modeling <code>EXTERNPROTO</code>. Each <code>EXTERNPROTO</code> will
     * spawn a new <code>proto_node_type</code> from the
     * <code>proto_node_class</code> that corresponds to the <code>PROTO</code>
     * to which the <code>EXTERNPROTO</code> refers. Recall that an
     * <code>EXTERNPROTO</code> provides a subset of the interfaces defined for
     * a <code>PROTO</code>; thus, for a <code>PROTO</code> with <var>n</var>
     * interfaces, there are <var>n</var>! possible unique
     * <code>EXTERNPROTO</code>s (and thus unique <code>proto_node_type</code>
     * instances).
     *
     * Structurally, the implementation of <code>proto_node</code> is very
     * similar to that of <code>proto_node_class</code>. The difference is that
     * event pathways for <code>ROUTE</code>s and <code>IS</code> mappings are
     * actually created in the <code>proto_node</code>. The
     * <code>proto_node_class</code>, on the other hand, includes metadata
     * about how these event pathways @e should be created.
     */
    class OPENVRML_LOCAL proto_node : public abstract_proto_node {
        friend class externproto_node;

        template <typename FieldValue>
        class proto_exposedfield : public proto_eventin<FieldValue>,
                                   public proto_eventout<FieldValue> {
        public:
            proto_exposedfield(abstract_proto_node & node,
                               const FieldValue & initial_value);
            virtual ~proto_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const FieldValue & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        struct proto_exposedfield_creator {
            proto_exposedfield_creator(
                const field_value & initial_value,
                proto_node & node,
                boost::shared_ptr<openvrml::event_listener> & exposedfield):
                initial_value_(&initial_value),
                node_(&node),
                exposedfield_(&exposedfield)
            {}

            template <typename T>
            void operator()(T) const
            {
                if (T::field_value_type_id == this->initial_value_->type()) {
                    using boost::polymorphic_downcast;
                    this->exposedfield_->reset(
                        new proto_exposedfield<T>(
                            *this->node_,
                            *polymorphic_downcast<const T *>(
                                this->initial_value_)));
                }
            }

        private:
            const openvrml::field_value * initial_value_;
            proto_node * node_;
            boost::shared_ptr<openvrml::event_listener> * exposedfield_;
        };

        static boost::shared_ptr<openvrml::event_listener>
        create_exposedfield(const field_value & initial_value,
                            proto_node & node)
            OPENVRML_THROW1(std::bad_alloc);

        boost::shared_ptr<openvrml::scope> proto_scope;
        std::vector<boost::intrusive_ptr<node> > impl_nodes;

    public:
        proto_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope,
                   const initial_value_map & initial_values)
            OPENVRML_THROW1(std::bad_alloc);
        virtual ~proto_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

    private:
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

        virtual script_node * to_script() OPENVRML_NOTHROW;
        virtual appearance_node * to_appearance() OPENVRML_NOTHROW;
        virtual bounded_volume_node * to_bounded_volume() OPENVRML_NOTHROW;
        virtual child_node * to_child() OPENVRML_NOTHROW;
        virtual color_node * to_color() OPENVRML_NOTHROW;
        virtual color_rgba_node * to_color_rgba() OPENVRML_NOTHROW;
        virtual coordinate_node * to_coordinate() OPENVRML_NOTHROW;
        virtual font_style_node * to_font_style() OPENVRML_NOTHROW ;
        virtual geometry_node * to_geometry() OPENVRML_NOTHROW;
        virtual grouping_node * to_grouping() OPENVRML_NOTHROW;
        virtual light_node * to_light() OPENVRML_NOTHROW;
        virtual material_node * to_material() OPENVRML_NOTHROW;
        virtual navigation_info_node * to_navigation_info() OPENVRML_NOTHROW;
        virtual normal_node * to_normal() OPENVRML_NOTHROW;
        virtual pointing_device_sensor_node * to_pointing_device_sensor()
            OPENVRML_NOTHROW;
        virtual scoped_light_node * to_scoped_light() OPENVRML_NOTHROW;
        virtual sound_source_node * to_sound_source() OPENVRML_NOTHROW;
        virtual texture_node * to_texture() OPENVRML_NOTHROW;
        virtual texture_coordinate_node * to_texture_coordinate()
            OPENVRML_NOTHROW;
        virtual texture_transform_node * to_texture_transform()
            OPENVRML_NOTHROW;
        virtual time_dependent_node * to_time_dependent() OPENVRML_NOTHROW;
        virtual transform_node * to_transform() OPENVRML_NOTHROW;
        virtual viewpoint_node * to_viewpoint() OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL node_path_element {
    public:
        std::vector<boost::intrusive_ptr<node> >::size_type index;
        field_value::type_id field_type;
        std::string field_id;

        node_path_element();
    };

    node_path_element::node_path_element():
        index(0),
        field_type(field_value::invalid_type_id)
    {}

    /**
     * @brief A node path is used to store the path to a node for the purpose
     *        of duplicating a route in a cloned node hierarchy.
     */
    typedef std::list<node_path_element> node_path_t;

    class OPENVRML_LOCAL path_getter : boost::noncopyable {
        const node & objective;
        node_path_t & node_path;
        bool found;

    public:
        path_getter(const node & objective, node_path_t & node_path)
            OPENVRML_NOTHROW;

        void get_path_from(const boost::intrusive_ptr<node> & node)
            OPENVRML_THROW1(std::bad_alloc);
        void get_path_from(
            const std::vector<boost::intrusive_ptr<node> > & nodes)
            OPENVRML_THROW1(std::bad_alloc);

    private:
        void traverse_children(node & n) OPENVRML_THROW1(std::bad_alloc);
    };

    path_getter::path_getter(const node & objective, node_path_t & node_path)
        OPENVRML_NOTHROW:
        objective(objective),
        node_path(node_path),
        found(false)
    {}

    void path_getter::get_path_from(const boost::intrusive_ptr<node> & node)
        OPENVRML_THROW1(std::bad_alloc)
    {
        if (node) {
            this->node_path.push_back(node_path_element());
            if (node.get() == &objective) {
                this->found = true;
                return;
            }
            this->traverse_children(*node);
            if (!this->found) { this->node_path.pop_back(); }
        }
    }

    void path_getter::get_path_from(
        const std::vector<boost::intrusive_ptr<node> > & nodes)
        OPENVRML_THROW1(std::bad_alloc)
    {
        this->node_path.push_back(node_path_element());
        node_path_element & back = this->node_path.back();
        while (back.index < nodes.size()) {
            assert(&back == &this->node_path.back());
            if (nodes[back.index].get() == &this->objective) {
                this->found = true;
                return;
            }
            if (nodes[back.index].get()) {
                this->traverse_children(*nodes[back.index]);
            }
            //
            // We need to bail early to avoid incrementing the counter.
            //
            if (this->found) { return; }
            ++back.index;
        }
        if (!this->found) { this->node_path.pop_back(); }
    }

    void path_getter::traverse_children(node & n)
        OPENVRML_THROW1(std::bad_alloc)
    {
        const node_interface_set & interfaces = n.type().interfaces();
        node_path_element & back = this->node_path.back();
        for (node_interface_set::const_iterator interface = interfaces.begin();
             !this->found && interface != interfaces.end();
             ++interface) {
            assert(&back == &this->node_path.back());
            if (interface->type == node_interface::field_id
                || interface->type == node_interface::exposedfield_id) {
                if (interface->field_type == field_value::sfnode_id) {
                    back.field_type = field_value::sfnode_id;
                    back.field_id = interface->id;
                    try {
                        const sfnode value = n.field<sfnode>(interface->id);
                        this->get_path_from(value.value());
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                } else if (interface->field_type == field_value::mfnode_id) {
                    back.field_type = field_value::mfnode_id;
                    back.field_id = interface->id;
                    try {
                        const mfnode value = n.field<mfnode>(interface->id);
                        this->get_path_from(value.value());
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
            }
        }
    }

    OPENVRML_LOCAL const node_path_t
    get_path(const std::vector<boost::intrusive_ptr<node> > & root,
             const node & objective)
        OPENVRML_THROW1(std::bad_alloc)
    {
        node_path_t path;
        path_getter(objective, path).get_path_from(root);
        return path;
    }

    OPENVRML_LOCAL node *
    resolve_node_path(const node_path_t & path,
                      const std::vector<boost::intrusive_ptr<node> > & root)
    {
        using boost::next;
        using boost::prior;
        assert(!path.empty());
        node * result = root[path.front().index].get();
        const node_path_t::const_iterator before_end = prior(path.end());
        for (node_path_t::const_iterator path_element = path.begin();
             path_element != before_end;
             ++path_element) {
            assert(result);
            try {
                if (path_element->field_type == field_value::sfnode_id) {
                    result = result->field<sfnode>(path_element->field_id)
                        .value().get();
                } else if (path_element->field_type == field_value::mfnode_id)
                {
                    result = result->field<mfnode>(path_element->field_id)
                        .value()[next(path_element)->index].get();
                } else {
                    assert(!"invalid path_element->field_type");
                }
            } catch (unsupported_interface & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            }
        }
        return result;
    }

    class OPENVRML_LOCAL field_value_cloner {
    protected:
        const boost::shared_ptr<openvrml::scope> & target_scope;
        std::set<node *> traversed_nodes;

    public:
        explicit field_value_cloner(
            const boost::shared_ptr<openvrml::scope> & target_scope):
            target_scope(target_scope)
        {
            assert(target_scope);
        }

        virtual ~field_value_cloner()
        {}

        void clone_field_value(const boost::intrusive_ptr<node> & src_node,
                               const field_value & src,
                               field_value & dest)
            OPENVRML_THROW1(std::bad_alloc)
        {
            assert(src.type() == dest.type());
            const field_value::type_id type = src.type();
            if (type == field_value::sfnode_id) {
                this->clone_sfnode(src_node,
                                   static_cast<const sfnode &>(src),
                                   static_cast<sfnode &>(dest));
            } else if (type == field_value::mfnode_id) {
                this->clone_mfnode(src_node,
                                   static_cast<const mfnode &>(src),
                                   static_cast<mfnode &>(dest));
            } else {
                //
                // Do a shallow copy for other types.
                //
                dest.assign(src);
            }
        }

    private:
        virtual const boost::intrusive_ptr<node>
        clone_node(const boost::intrusive_ptr<node> & n)
            OPENVRML_THROW1(std::bad_alloc)
        {
            using std::set;
            using boost::intrusive_ptr;

            assert(this->target_scope);

            intrusive_ptr<node> result;

            if (!n) { return result; }

            const bool already_traversed =
                (this->traversed_nodes.find(n.get())
                 != this->traversed_nodes.end());

            if (already_traversed) {
                result = intrusive_ptr<node>(
                    this->target_scope->find_node(n->id()));
                assert(result);
            } else {
                initial_value_map initial_values;
                const node_interface_set & interfaces =
                    n->type().interfaces();
                for (node_interface_set::const_iterator interface =
                         interfaces.begin();
                     interface != interfaces.end();
                     ++interface) {
                    using std::string;
                    const node_interface::type_id type = interface->type;
                    const string & id = interface->id;
                    if (type == node_interface::exposedfield_id
                        || type == node_interface::field_id) {
                        using std::auto_ptr;
                        using boost::shared_ptr;
                        auto_ptr<field_value> src = n->field(id);
                        auto_ptr<field_value> dest =
                            field_value::create(interface->field_type);
                        assert(src->type() == dest->type());
                        this->clone_field_value(n, *src, *dest);
                        bool succeeded =
                            initial_values.insert(
                                make_pair(id, shared_ptr<field_value>(dest)))
                            .second;
                        assert(succeeded);
                    }
                }
                result = n->type().create_node(this->target_scope,
                                               initial_values);
                if (!n->id().empty()) { result->id(n->id()); }
            }
            return result;
        }

        void clone_sfnode(const boost::intrusive_ptr<node> & src_node,
                          const sfnode & src,
                          sfnode & dest)
            OPENVRML_THROW1(std::bad_alloc)
        {
            dest.value((src.value() == src_node)
                       ? node::self_tag
                       : this->clone_node(src.value()));
        }

        void clone_mfnode(const boost::intrusive_ptr<node> & src_node,
                          const mfnode & src,
                          mfnode & dest)
            OPENVRML_THROW1(std::bad_alloc)
        {
            using std::swap;
            using std::vector;
            std::vector<boost::intrusive_ptr<node> > result(
                src.value().size());
            for (vector<boost::intrusive_ptr<node> >::size_type i = 0;
                 i < src.value().size();
                 ++i) {
                result[i] = (src.value()[i] == src_node)
                          ? node::self_tag
                          : this->clone_node(src.value()[i]);
            }
            dest.value(result);
        }
    };

    //
    // Clone the implementation nodes.
    //
    class OPENVRML_LOCAL proto_impl_cloner : public field_value_cloner {
        const proto_node_class & node_class;
        const initial_value_map & initial_values_;

    public:
        proto_impl_cloner(
            const proto_node_class & node_class,
            const initial_value_map & initial_values,
            const boost::shared_ptr<openvrml::scope> & target_scope):
            field_value_cloner(target_scope),
            node_class(node_class),
            initial_values_(initial_values)
        {}

        const std::vector<boost::intrusive_ptr<node> > clone()
            OPENVRML_THROW1(std::bad_alloc)
        {
            using std::vector;

            vector<boost::intrusive_ptr<node> > result(
                this->node_class.impl_nodes.size());

            for (vector<boost::intrusive_ptr<node> >::size_type i = 0;
                 i < this->node_class.impl_nodes.size();
                 ++i) {
                result[i] = this->clone_node(this->node_class.impl_nodes[i]);
                assert(result[i]);
            }
            return result;
        }

    private:
        struct matches_is_target :
            std::unary_function<proto_node_class::is_map_t::value_type, bool> {

            explicit matches_is_target(
                const proto_node_class::is_target & is_target):
                is_target(&is_target)
            {}

            result_type operator()(const argument_type & is_map_value) const
            {
                return (is_map_value.second.impl_node
                        == this->is_target->impl_node)
                    && (is_map_value.second.impl_node_interface
                        == this->is_target->impl_node_interface);
            }

        private:
            const proto_node_class::is_target * is_target;
        };

        virtual const boost::intrusive_ptr<node>
        clone_node(const boost::intrusive_ptr<node> & n)
            OPENVRML_THROW1(std::bad_alloc)
        {
            using std::set;
            using boost::intrusive_ptr;

            assert(this->target_scope);

            intrusive_ptr<node> result;

            if (!n) { return result; }

            const bool already_traversed =
                (this->traversed_nodes.find(n.get())
                 != this->traversed_nodes.end());

            if (already_traversed) {
                result = intrusive_ptr<node>(
                    this->target_scope->find_node(n->id()));
                assert(result);
            } else {
                initial_value_map initial_values;
                const node_interface_set & interfaces =
                    n->type().interfaces();
                for (node_interface_set::const_iterator interface =
                         interfaces.begin();
                     interface != interfaces.end();
                     ++interface) {
                    using std::string;
                    const node_interface::type_id type = interface->type;
                    const string & id = interface->id;
                    if (type == node_interface::exposedfield_id
                        || type == node_interface::field_id) {
                        using std::auto_ptr;
                        using std::find_if;
                        using boost::shared_ptr;
                        auto_ptr<const field_value> src_val;
                        auto_ptr<field_value> dest_val =
                            field_value::create(interface->field_type);

                        //
                        // If the field/exposedField is IS'd, get the value
                        // from the initial_values_, or alternatively the
                        // default_values_.
                        //
                        typedef proto_node_class::is_target is_target;
                        typedef proto_node_class::is_map_t is_map;
                        typedef proto_node_class::default_value_map_t
                            default_value_map;

                        is_map::const_iterator is_mapping =
                            find_if(this->node_class.is_map.begin(),
                                    this->node_class.is_map.end(),
                                    matches_is_target(
                                        is_target(*n, interface->id)));
                        if (is_mapping != this->node_class.is_map.end()) {
                            using boost::bind;
                            using std::logical_or;
                            //
                            // If an exposedField in the implementation is IS'd
                            // to an eventIn or an eventOut in the interface,
                            // we'll still get here.  So if the implementation
                            // node interface is an exposedField, we need to
                            // check to see if the PROTO interface is an
                            // eventIn or an eventOut.
                            //
                            node_interface_set::const_iterator
                                proto_interface =
                                find_if(this->node_class.interfaces.begin(),
                                        this->node_class.interfaces.end(),
                                        bind(logical_or<bool>(),
                                             bind(node_interface_matches_exposedfield(),
                                                  _1,
                                                  is_mapping->first),
                                             bind(node_interface_matches_field(),
                                                  _1,
                                                  is_mapping->first)));

                            if (proto_interface
                                != this->node_class.interfaces.end()) {
                                initial_value_map::const_iterator
                                    initial_value =
                                    this->initial_values_.find(
                                        is_mapping->first);
                                if (initial_value
                                    != this->initial_values_.end()) {
                                    src_val = initial_value->second->clone();
                                } else {
                                    default_value_map::const_iterator
                                        default_value =
                                        this->node_class.default_value_map
                                        .find(is_mapping->first);
                                    assert(default_value
                                           != this->node_class
                                           .default_value_map.end());
                                    src_val = default_value->second->clone();
                                }
                            } else {
                                src_val = n->field(id);
                            }
                        } else {
                            src_val = n->field(id);
                        }

                        assert(src_val.get());
                        this->clone_field_value(n, *src_val, *dest_val);

                        bool succeeded =
                            initial_values.insert(
                                make_pair(id,
                                          shared_ptr<field_value>(dest_val)))
                            .second;
                        assert(succeeded);
                    }
                }
                result = n->type().create_node(this->target_scope,
                                               initial_values);
                if (!n->id().empty()) { result->id(n->id()); }
            }
            return result;
        }

        void clone_sfnode(const sfnode & src, sfnode & dest)
            OPENVRML_THROW1(std::bad_alloc)
        {
            dest.value(this->clone_node(src.value()));
        }

        void clone_mfnode(const mfnode & src, mfnode & dest)
            OPENVRML_THROW1(std::bad_alloc)
        {
            using std::swap;
            using std::vector;
            vector<boost::intrusive_ptr<node> > result(src.value().size());
            for (vector<boost::intrusive_ptr<node> >::size_type i = 0;
                 i < src.value().size();
                 ++i) {
                result[i] = this->clone_node(src.value()[i]);
            }
            dest.value(result);
        }
    };

    /**
     * @brief Construct.
     *
     * @param[in] impl_node             a node in the PROTO implementation.
     * @param[in] impl_node_interface   an interface of @p impl_node.
     */
    proto_node_class::
    is_target::is_target(node & impl_node,
                         const std::string & impl_node_interface):
        impl_node(&impl_node),
        impl_node_interface(impl_node_interface)
    {}

    /**
     * @brief Construct.
     *
     * @param[in] from      event source node.
     * @param[in] eventout  eventOut of @p from.
     * @param[in] to        event destination node.
     * @param[in] eventin   eventIn of @p to.
     */
    proto_node_class::route::route(node & from,
                                   const std::string & eventout,
                                   node & to,
                                   const std::string & eventin):
        from(&from),
        eventout(eventout),
        to(&to),
        eventin(eventin)
    {}

    /**
     * @brief Construct.
     *
     * @param[in] node_class    the proto_node_class that spawned the
     *                      proto_node_type.
     * @param[in] id            node_type identifier.
     * @param[in] interfaces    a subset of the interfaces supported by the
     *                      @p node_class.
     *
     * @exception unsupported_interface if an interface in @p interfaces is not
     *                                  supported by the @p node_class.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    proto_node_type::proto_node_type(const proto_node_class & node_class,
                                     const std::string & id,
                                     const node_interface_set & interfaces)
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc):
        node_type(node_class, id)
    {
        using std::find;
        using std::invalid_argument;
        for (node_interface_set::const_iterator interface = interfaces.begin();
             interface != interfaces.end();
             ++interface) {
            node_interface_set::const_iterator pos =
                find(node_class.interfaces.begin(),
                     node_class.interfaces.end(),
                     *interface);
            if (pos == node_class.interfaces.end()) {
                throw unsupported_interface(*interface);
            }
            const bool succeeded = this->interfaces_.insert(*interface).second;
            assert(succeeded);
        }
    }

    /**
     * @brief Destroy.
     */
    proto_node_type::~proto_node_type() OPENVRML_NOTHROW
    {}

    /**
     * @brief Interfaces.
     *
     * @return the interfaces.
     */
    const node_interface_set &
    proto_node_type::do_interfaces() const OPENVRML_NOTHROW
    {
        return this->interfaces_;
    }

    const boost::intrusive_ptr<node>
    proto_node_type::
    do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                   const initial_value_map & initial_values) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return boost::intrusive_ptr<node>(
            new proto_node(*this, scope, initial_values));
    }

    /**
     * @internal
     *
     * @class abstract_proto_node::proto_eventin
     *
     * @brief PROTO eventIn handler class template.
     */

    /**
     * @typedef abstract_proto_node::proto_eventin::listeners
     *
     * @brief Set of event listeners.
     */

    /**
     * @var abstract_proto_node::proto_eventin::listeners abstract_proto_node::proto_eventin::listeners_
     *
     * @brief Set of event listeners to which events are delegated for
     *        processing.
     */

    /**
     * @typedef abstract_proto_node::proto_eventin::field_value_type
     *
     * @brief Field value type.
     */

    /**
     * @typedef abstract_proto_node::proto_eventin::event_listener_type
     *
     * @brief Type of event listeners to which the instance delegates.
     */

    /**
     * @brief Construct.
     *
     * @param[in] node  abstract_proto_node.
     */
    template <typename FieldValue>
    abstract_proto_node::proto_eventin<FieldValue>::
    proto_eventin(abstract_proto_node & node):
        node_event_listener(node),
        node_field_value_listener<FieldValue>(node)
    {}

    /**
     * @brief Destroy.
     */
    template <typename FieldValue>
    abstract_proto_node::proto_eventin<FieldValue>::~proto_eventin()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param[in] value     field value.
     * @param[in] timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    template <typename FieldValue>
    void
    abstract_proto_node::proto_eventin<FieldValue>::
    do_process_event(const FieldValue & value, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        for (typename listeners::const_iterator listener =
                 this->listeners_.begin();
             listener != this->listeners_.end();
             ++listener) {
            (*listener)->process_event(value, timestamp);
        }
    }

    /**
     * @brief The name of the associated eventIn.
     *
     * @return the name of the associated eventIn.
     */
    template <typename FieldValue>
    const std::string
    abstract_proto_node::proto_eventin<FieldValue>::do_eventin_id() const
        OPENVRML_NOTHROW
    {
        using boost::polymorphic_downcast;
        proto_node & n = *polymorphic_downcast<proto_node *>(&this->node());
        eventin_map_t::const_iterator pos =
            std::find_if(n.eventin_map.begin(), n.eventin_map.end(),
                         proto_eventin_equal_to(*this));
        assert(pos != n.eventin_map.end());
        return pos->first;
    }

    /**
     * @brief Add a listener to delegate to. Corresponds to an IS statement.
     *
     * @param[in] listener  an event_listener to delegate to.
     *
     * @return @c true if @p listener is added successfully; @c false
     *         otherwise (if it already exists in the list of delegates).
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    template <typename FieldValue>
    bool
    abstract_proto_node::proto_eventin<FieldValue>::
    is(event_listener_type & listener)
        OPENVRML_THROW1(std::bad_alloc)
    {
        return this->listeners_.insert(&listener).second;
    }

    /**
     * @brief Factory function for proto_eventin<FieldValue> instances.
     *
     * @param[in] type  field_value::type_id.
     * @param[in] node  proto_node.
     *
     * @return a boost::shared_ptr to a proto_eventin<FieldValue> instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    boost::shared_ptr<event_listener>
    abstract_proto_node::create_eventin(const field_value::type_id type,
                                        abstract_proto_node & node)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using boost::mpl::for_each;
        using openvrml_::field_value_types;

        boost::shared_ptr<openvrml::event_listener> result;
        for_each<field_value_types>(proto_eventin_creator(type, node, result));
        assert(result);
        return result;
    }

    /**
     * @internal
     *
     * @class abstract_proto_node::proto_eventout
     *
     * @brief PROTO eventOut handler class template.
     */

    /**
     * @internal
     *
     * @class abstract_proto_node::proto_eventout::listener_t
     *
     * @brief Listens for events emitted from nodes in the PROTO implementation
     *        in order to propagate them out of the PROTO instance.
     */

    /**
     * @var abstract_proto_node::proto_eventout & abstract_proto_node::proto_eventout::listener_t::emitter
     *
     * @brief Reference to the outer proto_eventout class.
     *
     * @todo It's annoying that we need to carry this around.  Should
     *       investigate the possibility of promoting all this stuff to
     *       proto_eventout and have proto_eventout privately inherit
     *       field_value_listener<FieldValue>.
     */

    /**
     * @var abstract_proto_node & abstract_proto_node::proto_eventout::listener_t::node
     *
     * @brief Reference to the @c abstract_proto_node instance.
     */

    /**
     * @var FieldValue abstract_proto_node::proto_eventout::listener_t::value
     *
     * @brief The value of the most recently emitted event.
     */

    /**
     * @brief Construct.
     *
     * @param[in] emitter       @c proto_eventout.
     * @param[in] node          @c abstract_proto_node.
     * @param[in] initial_value initial value (used for exposedFields).
     */
    template <typename FieldValue>
    abstract_proto_node::proto_eventout<FieldValue>::listener_t::
    listener_t(proto_eventout & emitter,
               abstract_proto_node & node,
               const FieldValue & initial_value):
        node_event_listener(node),
        node_field_value_listener<FieldValue>(node),
        emitter(emitter),
        node(node),
        value(initial_value)
    {}

    /**
     * @brief Destroy.
     */
    template <typename FieldValue>
    abstract_proto_node::proto_eventout<FieldValue>::listener_t::~listener_t()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the associated eventIn identifier.
     */
    template <typename FieldValue>
    const std::string
    abstract_proto_node::proto_eventout<FieldValue>::listener_t::
    do_eventin_id() const
        OPENVRML_NOTHROW
    {
        return "<proto_node::proto_eventout::listener_t>";
    }

    /**
     * @brief Process event.
     *
     * @param[in] value     new value.
     * @param[in] timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    template<typename FieldValue>
    void
    abstract_proto_node::proto_eventout<FieldValue>::listener_t::
    do_process_event(const FieldValue & value, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        if (timestamp > this->emitter.last_time()) {
            this->value = value;
            node::emit_event(this->emitter, timestamp);
        }
    }

    /**
     * @var abstract_proto_node::proto_eventout::listener_t abstract_proto_node::proto_eventout::listener
     *
     * @brief Listens for events emitted from nodes in the @c PROTO
     *        implementation in order to propagate them out of the @c PROTO
     *        instance.
     */

    /**
     * @typedef abstract_proto_node::proto_eventout<FieldValue>::field_value_type
     *
     * @brief Field value type.
     */

    /**
     * @typedef abstract_proto_node::proto_eventout<FieldValue>::event_emitter_type
     *
     * @brief Event emitter type.
     */

    /**
     * @typedef abstract_proto_node::proto_eventout<FieldValue>::event_listener_type
     *
     * @brief Event listener type.
     */

    /**
     * @brief Construct.
     *
     * @param[in] node          @c abstract_proto_node.
     * @param[in] initial_value initial value.  This is used by
     *                      @c proto_exposedfield<FieldValue>
     */
    template <typename FieldValue>
    abstract_proto_node::proto_eventout<FieldValue>::
    proto_eventout(abstract_proto_node & node,
                   const FieldValue & initial_value):
        openvrml::event_emitter(this->listener.value),
        field_value_emitter<FieldValue>(this->listener.value),
        listener(*this, node, initial_value)
    {}

    /**
     * @brief Destroy.
     */
    template <typename FieldValue>
    abstract_proto_node::proto_eventout<FieldValue>::~proto_eventout()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Create an @c IS mapping.
     *
     * @param[in,out] emitter   the @c event_emitter from a node in the @c PROTO
     *                  implementation.
     *
     * @return @c true if the @c IS mapping is created successfully; @c false
     *         otherwise (i.e., if it already exists).
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    template <typename FieldValue>
    bool
    abstract_proto_node::proto_eventout<FieldValue>::
    is(event_emitter_type & emitter) OPENVRML_THROW1(std::bad_alloc)
    {
        return emitter.add(this->listener);
    }

    /**
     * @brief The name of the associated eventOut.
     *
     * @return the name of the associated eventOut.
     */
    template <typename FieldValue>
    const std::string
    abstract_proto_node::proto_eventout<FieldValue>::do_eventout_id() const
        OPENVRML_NOTHROW
    {
        abstract_proto_node & n = this->listener.node;
        eventout_map_t::const_iterator pos =
            std::find_if(n.eventout_map.begin(), n.eventout_map.end(),
                         proto_eventout_equal_to(*this));
        assert(pos != n.eventout_map.end());
        return pos->first;
    }

    /**
     * @brief Factory function for @c proto_eventout<FieldValue> instances.
     *
     * @param[in] type  @c field_value::type_id.
     * @param[in] node  @c abstract_proto_node.
     *
     * @return a @c boost::shared_ptr to a @c proto_eventout<FieldValue>
     *         instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    boost::shared_ptr<event_emitter>
    abstract_proto_node::create_eventout(const field_value::type_id type,
                                         abstract_proto_node & node)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using boost::mpl::for_each;
        using openvrml_::field_value_types;

        boost::shared_ptr<openvrml::event_emitter> result;
        for_each<field_value_types>(
            proto_eventout_creator(type, node, result));
        assert(result);
        return result;
    }

    /**
     * @internal
     *
     * @class proto_node::proto_exposedfield
     *
     * @brief PROTO exposedField handler class template.
     */

    /**
     * @brief Construct.
     *
     * @param[in] node          proto_node.
     * @param[in] initial_value initial value.
     */
    template <typename FieldValue>
    proto_node::proto_exposedfield<FieldValue>::
    proto_exposedfield(abstract_proto_node & node,
                       const FieldValue & initial_value):
        node_event_listener(node),
        openvrml::event_emitter(this->listener.value),
        proto_eventin<FieldValue>(node),
        proto_eventout<FieldValue>(node, initial_value)
    {}

    /**
     * @brief Destroy.
     */
    template <typename FieldValue>
    proto_node::proto_exposedfield<FieldValue>::~proto_exposedfield()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Process an event.
     *
     * @param[in] value     event value.
     * @param[in] timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    template <typename FieldValue>
    void
    proto_node::proto_exposedfield<FieldValue>::
    do_process_event(const FieldValue & value, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        this->proto_eventin<FieldValue>::do_process_event(value, timestamp);
        this->listener.value = value;
        node::emit_event(*this, timestamp);
    }

    /**
     * @brief Factory function for proto_exposedfield<FieldValue> instances.
     *
     * @param[in] type  field_value::type_id.
     * @param[in] node  proto_node.
     *
     * @return a boost::shared_ptr to a proto_exposedfield<FieldValue>
     *         instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    boost::shared_ptr<event_listener>
    proto_node::create_exposedfield(const field_value & initial_value,
                                    proto_node & node)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using boost::mpl::for_each;
        using openvrml_::field_value_types;

        boost::shared_ptr<openvrml::event_listener> result;
        for_each<field_value_types>(proto_exposedfield_creator(initial_value,
                                                               node,
                                                               result));
        assert(result.get());
        return result;
    }

    /**
     * @brief Construct.
     *
     * @param[in] type  node_type.
     * @param[in] scope scope.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    proto_node::proto_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope,
                           const initial_value_map & initial_values)
        OPENVRML_THROW1(std::bad_alloc):
        abstract_proto_node(type, scope),
        proto_scope(scope)
    {
        const proto_node_class & node_class =
            static_cast<const proto_node_class &>(type.node_class());

        this->impl_nodes = proto_impl_cloner(node_class,
                                             initial_values,
                                             this->proto_scope).clone();

        //
        // Establish routes.
        //
        typedef proto_node_class::routes_t routes_t;
        for (routes_t::const_iterator route = node_class.routes.begin();
             route != node_class.routes.end();
             ++route) {
            // XXX
            // XXX It would be better to store the node_paths along with the
            // XXX route instead of rebuilding them every time we instantiate
            // XXX the PROTO.
            // XXX
            node_path_t path_to_from;
            assert(!node_class.impl_nodes.empty());
            path_getter(*route->from, path_to_from)
                .get_path_from(node_class.impl_nodes);
            assert(!path_to_from.empty());
            node * const from_node = resolve_node_path(path_to_from,
                                                       this->impl_nodes);
            assert(from_node);

            node_path_t path_to_to;
            path_getter(*route->to, path_to_to)
                .get_path_from(node_class.impl_nodes);
            node * const to_node = resolve_node_path(path_to_to,
                                                     this->impl_nodes);
            assert(to_node);

            try {
                add_route(*from_node, route->eventout,
                          *to_node, route->eventin);
            } catch (unsupported_interface & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            } catch (field_value_type_mismatch & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            }
        }

        //
        // Add eventIns, eventOuts, exposedFields.
        //
        for (node_interface_set::const_iterator interface =
                 node_class.interfaces.begin();
             interface != node_class.interfaces.end();
             ++interface) {
            using boost::shared_ptr;
            using boost::dynamic_pointer_cast;
            using std::pair;
            bool succeeded;
            shared_ptr<openvrml::event_listener> interface_eventin;
            shared_ptr<openvrml::event_emitter> interface_eventout;
            typedef proto_node_class::is_map_t is_map_t;
            pair<is_map_t::const_iterator, is_map_t::const_iterator> is_range;
            initial_value_map::const_iterator initial_value;
            switch (interface->type) {
            case node_interface::eventin_id:
                interface_eventin = create_eventin(interface->field_type,
                                                   *this);
                is_range = node_class.is_map.equal_range(interface->id);
                for (is_map_t::const_iterator is_mapping = is_range.first;
                     is_mapping != is_range.second;
                     ++is_mapping) {
                    assert(is_mapping->second.impl_node);
                    node_path_t path_to_impl_node;
                    path_getter(*is_mapping->second.impl_node,
                                path_to_impl_node)
                        .get_path_from(node_class.impl_nodes);
                    node * impl_node = resolve_node_path(path_to_impl_node,
                                                         this->impl_nodes);
                    assert(impl_node);
                    const std::string & impl_node_interface =
                        is_mapping->second.impl_node_interface;
                    try {
                        using boost::mpl::for_each;
                        using openvrml_::field_value_types;

                        openvrml::event_listener & impl_eventin =
                            impl_node->event_listener(impl_node_interface);
                        for_each<field_value_types>(
                            eventin_is(interface->field_type,
                                       impl_eventin,
                                       *interface_eventin));
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
                succeeded = this->eventin_map
                    .insert(make_pair(interface->id, interface_eventin))
                    .second;
                assert(succeeded);
                break;
            case node_interface::eventout_id:
                interface_eventout = create_eventout(interface->field_type,
                                                     *this);
                is_range = node_class.is_map.equal_range(interface->id);
                for (is_map_t::const_iterator is_mapping = is_range.first;
                     is_mapping != is_range.second;
                     ++is_mapping) {
                    assert(is_mapping->second.impl_node);
                    node_path_t path_to_impl_node;
                    path_getter(*is_mapping->second.impl_node,
                                path_to_impl_node)
                        .get_path_from(node_class.impl_nodes);
                    node * impl_node = resolve_node_path(path_to_impl_node,
                                                         this->impl_nodes);
                    assert(impl_node);
                    const std::string & impl_node_interface =
                        is_mapping->second.impl_node_interface;
                    try {
                        using boost::mpl::for_each;
                        using openvrml_::field_value_types;

                        openvrml::event_emitter & impl_eventout =
                            impl_node->event_emitter(impl_node_interface);
                        for_each<field_value_types>(
                            eventout_is(interface->field_type,
                                        impl_eventout,
                                        *interface_eventout));
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
                succeeded = this->eventout_map
                    .insert(make_pair(interface->id, interface_eventout))
                    .second;
                assert(succeeded);
                break;
            case node_interface::exposedfield_id:
                initial_value = initial_values.find(interface->id);
                if (initial_value == initial_values.end()) {
                    initial_value =
                        node_class.default_value_map.find(interface->id);
                    assert(initial_value
                           != node_class.default_value_map.end());
                }
                interface_eventin = create_exposedfield(*initial_value->second,
                                                        *this);
                interface_eventout =
                    dynamic_pointer_cast<openvrml::event_emitter>(
                        interface_eventin);
                is_range = node_class.is_map.equal_range(interface->id);
                for (is_map_t::const_iterator is_mapping = is_range.first;
                     is_mapping != is_range.second;
                     ++is_mapping) {
                    assert(is_mapping->second.impl_node);
                    node_path_t path_to_impl_node;
                    path_getter(*is_mapping->second.impl_node,
                                path_to_impl_node)
                        .get_path_from(node_class.impl_nodes);
                    node * impl_node = resolve_node_path(path_to_impl_node,
                                                         this->impl_nodes);
                    assert(impl_node);
                    const std::string & impl_node_interface =
                        is_mapping->second.impl_node_interface;
                    try {
                        using boost::mpl::for_each;
                        using openvrml_::field_value_types;

                        openvrml::event_listener & impl_eventin =
                            impl_node->event_listener(impl_node_interface);
                        for_each<field_value_types>(
                            eventin_is(interface->field_type,
                                       impl_eventin,
                                       *interface_eventin));
                        openvrml::event_emitter & impl_eventout =
                            impl_node->event_emitter(impl_node_interface);
                        for_each<field_value_types>(
                            eventout_is(interface->field_type,
                                        impl_eventout,
                                        *interface_eventout));
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
                succeeded = this->eventin_map
                    .insert(make_pair("set_" + interface->id,
                                      interface_eventin)).second;
                assert(succeeded);
                succeeded = this->eventout_map
                    .insert(make_pair(interface->id + "_changed",
                                      interface_eventout)).second;
                assert(succeeded);
                break;
            case node_interface::field_id:
                break;
            default:
                assert(false);
            }
        }
    }

    /**
     * @brief Destroy.
     */
    proto_node::~proto_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Indicate whether the node needs to be rerendered.
     *
     * @return @c true if the node has changed and needs to be rerendered;
     *         @c false otherwise.
     */
    bool proto_node::modified() const
    {
        return !this->impl_nodes.empty()
            ? this->impl_nodes.front()->modified()
            : false;
    }

    /**
     * @brief Initialize.
     *
     * @param[in] timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void proto_node::do_initialize(const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using std::vector;
        for (vector<boost::intrusive_ptr<node> >::const_iterator node =
                 this->impl_nodes.begin();
             node != impl_nodes.end();
             ++node) {
            (*node)->initialize(*this->scene(), timestamp);
        }
        if (!this->impl_nodes.empty()) {
            this->impl_nodes.front()->modified(true);
        }
    }

    /**
     * @brief Field accessor implementation.
     *
     * @param[in] id    field identifier.
     *
     * @exception unsupported_interface if the node has no field @p id.
     *
     * @todo Make this function handle exposedFields.
     */
    const field_value & proto_node::do_field(const std::string & id) const
        OPENVRML_THROW1(unsupported_interface)
    {
        //
        // First, we need to find the implementation node that the field is
        // IS'd to.  For the accessor, we don't care if there's more than one.
        //
        const proto_node_class & node_class =
            static_cast<const proto_node_class &>(this->type().node_class());
        proto_node_class::is_map_t::const_iterator is_mapping =
            node_class.is_map.find(id);
        if (is_mapping != node_class.is_map.end()) {
            //
            // Get the path to the implementation node.
            //
            assert(is_mapping->second.impl_node);
            assert(!is_mapping->second.impl_node_interface.empty());
            node_path_t path;
            path_getter(*is_mapping->second.impl_node, path)
                .get_path_from(node_class.impl_nodes);

            //
            // Resolve the path against this instance's implementation nodes.
            //
            node * const impl_node = resolve_node_path(path, this->impl_nodes);

            //
            // Get the field value for the implementation node.
            //
            return impl_node->do_field(is_mapping->second.impl_node_interface);
        } else {
            //
            // If there are no IS mappings for the field, then return the
            // default value.
            //
            proto_node_class::default_value_map_t::const_iterator
                default_value = node_class.default_value_map.find(id);
            if (default_value == node_class.default_value_map.end()) {
                throw unsupported_interface(this->type(), id);
            }
            return *default_value->second;
        }
    }

    /**
     * @brief event_listener accessor implementation.
     *
     * @param[in] id    eventIn identifier.
     *
     * @return the event_listener for the eventIn @p id.
     *
     * @exception unsupported_interface if the node has no eventIn @p id.
     */
    event_listener &
    proto_node::do_event_listener(const std::string & id)
        OPENVRML_THROW1(unsupported_interface)
    {
        eventin_map_t::iterator pos = this->eventin_map.find(id);
        if (pos == this->eventin_map.end()) {
            throw unsupported_interface(this->type(),
                                        node_interface::eventin_id,
                                        id);
        }
        return *pos->second;
    }

    /**
     * @brief event_emitter accessor implementation.
     *
     * @param[in] id    eventOut identifier.
     *
     * @return the event_emitter for the eventOut @p id.
     *
     * @exception unsupported_interface if the node has no eventOut @p id.
     */
    event_emitter & proto_node::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(unsupported_interface)
    {
        eventout_map_t::iterator pos = this->eventout_map.find(id);
        if (pos == this->eventout_map.end()) {
            throw unsupported_interface(this->type(),
                                        node_interface::eventout_id,
                                        id);
        }
        return *pos->second;
    }

    /**
     * @brief Initialize.
     *
     * @param[in] timestamp the current time.
     */
    void proto_node::do_shutdown(const double timestamp) OPENVRML_NOTHROW
    {
        using std::vector;
        for (vector<boost::intrusive_ptr<node> >::const_iterator node =
                 this->impl_nodes.begin();
             node != impl_nodes.end();
             ++node) {
            (*node)->shutdown(timestamp);
        }
    }

    /**
     * @brief Cast to a script_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a script_node, or 0 otherwise.
     */
    script_node * proto_node::to_script() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<script_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to an appearance_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is an appearance_node, or 0 otherwise.
     */
    appearance_node * proto_node::to_appearance() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<appearance_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a <code>bounded_volume_node</code>.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a <code>bounded_volume_node</code>, or 0 otherwise.
     */
    bounded_volume_node * proto_node::to_bounded_volume() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<bounded_volume_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a child_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a child_node, or 0 otherwise.
     */
    child_node * proto_node::to_child() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<child_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a color_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a color_node, or 0 otherwise.
     */
    color_node * proto_node::to_color() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<color_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c color_rgba_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c color_rgba_node, or 0 otherwise.
     */
    color_rgba_node * proto_node::to_color_rgba() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<color_rgba_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a coordinate_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a coordinate_node, or 0 otherwise.
     */
    coordinate_node * proto_node::to_coordinate() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<coordinate_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a font_style_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a font_style_node, or 0 otherwise.
     */
    font_style_node * proto_node::to_font_style() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<font_style_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a geometry_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a geometry_node, or 0 otherwise.
     */
    geometry_node * proto_node::to_geometry() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<geometry_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a <code>grouping_node</code>.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a <code>grouping_node</code>, or 0 otherwise.
     */
    grouping_node * proto_node::to_grouping() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<grouping_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a <code>light_node</code>.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a <code>light_node</code>, or 0 otherwise.
     */
    light_node * proto_node::to_light() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<light_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a material_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a material_node, or 0 otherwise.
     */
    material_node * proto_node::to_material() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<material_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a <code>navigation_info_node</code>.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a <code>navigation_info_node</code>, or 0 otherwise.
     */
    navigation_info_node * proto_node::to_navigation_info() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<navigation_info_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a normal_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a normal_node, or 0 otherwise.
     */
    normal_node * proto_node::to_normal() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<normal_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a <code>pointing_device_sensor_node</code>.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a <code>pointing_device_sensor_node</code>, or 0 otherwise.
     */
    pointing_device_sensor_node * proto_node::to_pointing_device_sensor()
        OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<pointing_device_sensor_node *>(
            this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a <code>scoped_light_node</code>.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a <code>scoped_light_node</code>, or 0 otherwise.
     */
    scoped_light_node * proto_node::to_scoped_light() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<scoped_light_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a sound_source_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a sound_source_node, or 0 otherwise.
     */
    sound_source_node * proto_node::to_sound_source() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<sound_source_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a texture_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a texture_node, or 0 otherwise.
     */
    texture_node * proto_node::to_texture() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<texture_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a texture_coordinate_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a texture_coordinate_node, or 0 otherwise.
     */
    texture_coordinate_node * proto_node::to_texture_coordinate()
        OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<texture_coordinate_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a texture_transform_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a texture_transform_node, or 0 otherwise.
     */
    texture_transform_node * proto_node::to_texture_transform()
        OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<texture_transform_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a <code>time_dependent_node</code>.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a <code>time_dependent_node</code>, or 0 otherwise.
     */
    time_dependent_node * proto_node::to_time_dependent() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<time_dependent_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a transform_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a transform_node, or 0 otherwise.
     */
    transform_node * proto_node::to_transform() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<transform_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a viewpoint_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a viewpoint_node, or 0 otherwise.
     */
    viewpoint_node * proto_node::to_viewpoint() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<viewpoint_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Construct.
     *
     * @param[in] browser
     * @param[in] interfaces
     * @param[in] default_value_map
     * @param[in] impl_nodes
     * @param[in] is_map
     * @param[in] routes
     */
    proto_node_class::
    proto_node_class(
        openvrml::browser & browser,
        const node_interface_set & interfaces,
        const default_value_map_t & default_value_map,
        const std::vector<boost::intrusive_ptr<node> > & impl_nodes,
        const is_map_t & is_map,
        const routes_t & routes):
        node_class(browser),
        interfaces(interfaces),
        default_value_map(default_value_map),
        impl_nodes(impl_nodes),
        routes(routes),
        is_map(is_map)
    {}

    proto_node_class::~proto_node_class() OPENVRML_NOTHROW
    {}

    const boost::shared_ptr<node_type>
    proto_node_class::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        return boost::shared_ptr<node_type>(
            new proto_node_type(*this, id, interfaces));
    }
} // namespace openvrml

namespace {

    class externproto_node_type;

    /**
     * @brief <code>node_class</code> for <code>EXTERNPROTO</code>s.
     */
    class OPENVRML_LOCAL externproto_node_class :
        public boost::enable_shared_from_this<externproto_node_class>,
        public openvrml::node_class {

        friend class externproto_node_type;

        struct load_proto;

        mutable boost::mutex mutex_;
        boost::weak_ptr<openvrml::proto_node_class> proto_node_class_;

        typedef std::vector<boost::weak_ptr<externproto_node_type> >
            externproto_node_types;

        mutable externproto_node_types externproto_node_types_;
        bool externproto_node_types_cleared_;

        boost::scoped_ptr<boost::thread> load_proto_thread_;

    public:
        externproto_node_class(const openvrml::scene & scene,
                               const std::vector<std::string> & uris)
            OPENVRML_THROW1(boost::thread_resource_error);
        virtual ~externproto_node_class() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<openvrml::node_type>
        do_create_type(const std::string & id,
                       const openvrml::node_interface_set & interfaces) const
            OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);

        virtual void do_shutdown(double time) OPENVRML_NOTHROW;

        void set_proto_node_class(
            const boost::weak_ptr<openvrml::proto_node_class> & proto_node_class = boost::weak_ptr<openvrml::proto_node_class>())
            OPENVRML_THROW1(std::bad_alloc);

        void clear_externproto_node_types() OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL externproto_node_type : public openvrml::node_type {
        const boost::shared_ptr<const externproto_node_class> node_class_;

        openvrml::node_interface_set interfaces_;

        mutable boost::mutex mutex_;
        boost::shared_ptr<openvrml::proto_node_type> proto_node_type_;

        typedef std::vector<boost::intrusive_ptr<openvrml::externproto_node> >
            externproto_nodes;

        mutable externproto_nodes externproto_nodes_;

    public:
        externproto_node_type(
            const boost::shared_ptr<const externproto_node_class> & c,
            const std::string & id,
            const openvrml::node_interface_set & interfaces)
            OPENVRML_THROW1(std::bad_alloc);

        virtual ~externproto_node_type() OPENVRML_NOTHROW;

        void set_proto_node_type(openvrml::proto_node_class & proto_node_class)
            OPENVRML_THROW1(std::bad_alloc);

        void clear_externproto_nodes() OPENVRML_NOTHROW;

    private:
        virtual const openvrml::node_interface_set & do_interfaces() const
            throw();

        virtual const boost::intrusive_ptr<openvrml::node>
        do_create_node(
            const boost::shared_ptr<openvrml::scope> & scope,
            const openvrml::initial_value_map & initial_values) const
            OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                            std::bad_alloc);
    };
}

namespace openvrml {

    class OPENVRML_LOCAL externproto_node :
        public openvrml::abstract_proto_node {

        template <typename FieldValue>
        class externproto_exposedfield :
            public FieldValue,
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

        field_map field_map_;

        boost::intrusive_ptr<openvrml::proto_node> proto_node_;

    public:
        externproto_node(const externproto_node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope,
                         const openvrml::initial_value_map & initial_values)
            OPENVRML_THROW1(std::bad_alloc);

        virtual ~externproto_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

        void set_proto_node(openvrml::proto_node_type & node_type)
            OPENVRML_THROW1(std::bad_alloc);

    private:
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
        virtual openvrml::appearance_node * to_appearance() OPENVRML_NOTHROW;
        virtual openvrml::bounded_volume_node * to_bounded_volume()
            OPENVRML_NOTHROW;
        virtual openvrml::child_node * to_child() OPENVRML_NOTHROW;
        virtual openvrml::color_node * to_color() OPENVRML_NOTHROW;
        virtual openvrml::color_rgba_node * to_color_rgba() OPENVRML_NOTHROW;
        virtual openvrml::coordinate_node * to_coordinate() OPENVRML_NOTHROW;
        virtual openvrml::font_style_node * to_font_style() OPENVRML_NOTHROW ;
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

namespace {

    class OPENVRML_LOCAL default_navigation_info :
        public openvrml::navigation_info_node {
    public:
        explicit default_navigation_info(const openvrml::null_node_type & type)
            OPENVRML_NOTHROW;
        virtual ~default_navigation_info() OPENVRML_NOTHROW;

        virtual const std::vector<float> & avatar_size() const
            OPENVRML_NOTHROW;
        virtual bool headlight() const OPENVRML_NOTHROW;
        virtual float speed() const OPENVRML_NOTHROW;
        virtual const std::vector<std::string> & type() const OPENVRML_NOTHROW;
        virtual float visibility_limit() const OPENVRML_NOTHROW;

    private:
        virtual void do_field(const std::string & id,
                              const openvrml::field_value & value)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_field(const std::string & id) const
            OPENVRML_NOTHROW;
        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_eventout(const std::string & id) const
            OPENVRML_NOTHROW;

        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
    };

    const boost::shared_ptr<openvrml::scope> null_scope_ptr;

    /**
     * @brief Construct.
     *
     * @param[in] t node type.
     */
    default_navigation_info::
    default_navigation_info(const openvrml::null_node_type & t)
        OPENVRML_NOTHROW:
        openvrml::node(t, null_scope_ptr),
        openvrml::bounded_volume_node(t, null_scope_ptr),
        openvrml::child_node(t, null_scope_ptr),
        openvrml::navigation_info_node(t, null_scope_ptr)
    {}

    /**
     * @brief Destroy
     */
    default_navigation_info::~default_navigation_info() OPENVRML_NOTHROW
    {}

    /**
     * @brief The avatar dimensions.
     *
     * @return [0.25, 1.6, 0.75]
     */
    const std::vector<float> & default_navigation_info::avatar_size() const
        OPENVRML_NOTHROW
    {
        static const float array[] = { 0.25f, 1.6f, 0.75f };
        static const std::vector<float> vec(array, array + 3);
        return vec;
    }

    /**
     * @brief The headlight state.
     *
     * @return @c true
     */
    bool default_navigation_info::headlight() const OPENVRML_NOTHROW
    {
        return true;
    }

    /**
     * @brief The speed of the user view.
     *
     * @return 1.0
     */
    float default_navigation_info::speed() const OPENVRML_NOTHROW
    {
        return 1.0;
    }

    /**
     * @brief The navigation type.
     *
     * @return ["WALK", "ANY"]
     */
    const std::vector<std::string> & default_navigation_info::type() const
        OPENVRML_NOTHROW
    {
        static const char * array[] = { "WALK", "ANY" };
        static const std::vector<std::string> vec(array, array + 2);
        return vec;
    }

    /**
     * @brief The visibility limit.
     *
     * @return 0.0
     */
    float default_navigation_info::visibility_limit() const OPENVRML_NOTHROW
    {
        return 0.0;
    }

    void default_navigation_info::do_field(const std::string &,
                                           const openvrml::field_value &)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_navigation_info::do_field(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    void
    default_navigation_info::do_process_event(const std::string &,
                                              const openvrml::field_value &,
                                              double)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_navigation_info::do_eventout(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    openvrml::event_listener &
    default_navigation_info::do_event_listener(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->node::type(), id);
        return *static_cast<openvrml::event_listener *>(0);
    }

    openvrml::event_emitter &
    default_navigation_info::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->node::type(), id);
        return *static_cast<openvrml::event_emitter *>(0);
    }


    class OPENVRML_LOCAL default_viewpoint : public openvrml::viewpoint_node {
        openvrml::mat4f userViewTransform;

    public:
        explicit default_viewpoint(const openvrml::null_node_type & type)
            OPENVRML_NOTHROW;
        virtual ~default_viewpoint() OPENVRML_NOTHROW;

        virtual const openvrml::mat4f & transformation() const
            OPENVRML_NOTHROW;
        virtual const openvrml::mat4f & user_view_transform() const
            OPENVRML_NOTHROW;
        virtual void user_view_transform(const openvrml::mat4f & transform)
            OPENVRML_NOTHROW;
        virtual const std::string & description() const OPENVRML_NOTHROW;
        virtual float field_of_view() const OPENVRML_NOTHROW;

    private:
        virtual void do_field(const std::string & id,
                              const openvrml::field_value & value)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_field(const std::string & id) const
            OPENVRML_NOTHROW;
        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_eventout(const std::string & id) const
            OPENVRML_NOTHROW;

        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
    };

    /**
     * @brief Construct.
     *
     * @param[in] type  the browser's null_node_type instance.
     */
    default_viewpoint::default_viewpoint(const openvrml::null_node_type & type)
        OPENVRML_NOTHROW:
        openvrml::node(type, null_scope_ptr),
        openvrml::bounded_volume_node(type, null_scope_ptr),
        openvrml::child_node(type, null_scope_ptr),
        openvrml::viewpoint_node(type, null_scope_ptr)
    {}

    /**
     * @brief Destroy.
     */
    default_viewpoint::~default_viewpoint() OPENVRML_NOTHROW
    {}

    const openvrml::mat4f & default_viewpoint::transformation() const
        OPENVRML_NOTHROW
    {
        using openvrml::mat4f;
        using openvrml::rotation;
        using openvrml::vec3f;
        static const vec3f position(0.0, 0.0, 10.0);
        static const rotation orientation;
        static const vec3f scale(1.0, 1.0, 1.0);
        static const rotation scaleOrientation;
        static const vec3f center;
        static const mat4f t(mat4f::transformation(position,
                                                   orientation,
                                                   scale,
                                                   scaleOrientation,
                                                   center));
        return t;
    }

    const openvrml::mat4f & default_viewpoint::user_view_transform() const
        OPENVRML_NOTHROW
    {
        return this->userViewTransform;
    }

    void
    default_viewpoint::user_view_transform(const openvrml::mat4f & transform)
        OPENVRML_NOTHROW
    {
        this->userViewTransform = transform;
    }

    const std::string & default_viewpoint::description() const OPENVRML_NOTHROW
    {
        static const std::string desc;
        return desc;
    }

    float default_viewpoint::field_of_view() const OPENVRML_NOTHROW
    {
        static const float fieldOfView = 0.785398f;
        return fieldOfView;
    }

    void default_viewpoint::do_field(const std::string &,
                                     const openvrml::field_value &)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_viewpoint::do_field(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    void default_viewpoint::do_process_event(const std::string &,
                                             const openvrml::field_value &,
                                             double)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_viewpoint::do_eventout(const std::string &) const OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    openvrml::event_listener &
    default_viewpoint::do_event_listener(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->type(), id);
        return *static_cast<openvrml::event_listener *>(0);
    }

    openvrml::event_emitter &
    default_viewpoint::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->type(), id);
        return *static_cast<openvrml::event_emitter *>(0);
    }


    template <typename SpiritActor, typename Iterator1, typename Iterator2>
    class assign_iterators_base {
        SpiritActor actor_;
        Iterator1 begin_;
        Iterator2 end_;

    public:
        typedef assign_iterators_base<SpiritActor, Iterator1, Iterator2>
            this_type;

        assign_iterators_base(const SpiritActor & actor,
                              const Iterator1 & begin,
                              const Iterator2 & end):
            actor_(actor),
            begin_(begin),
            end_(end)
        {}

        template <typename T>
        struct result {
            typedef void type;
        };

        template <typename Tuple>
        typename phoenix::actor_result<this_type, Tuple>::type
        eval(Tuple) const
        {
            this->actor_(this->begin_(), this->end_());
        }
    };

    template <typename SpiritActor, typename Iterator1, typename Iterator2>
    phoenix::actor<assign_iterators_base<SpiritActor, Iterator1, Iterator2> >
    assign_iterators(const SpiritActor & actor,
                     const Iterator1 & begin,
                     const Iterator2 & end)
    {
        return assign_iterators_base<SpiritActor, Iterator1, Iterator2>(actor,
                                                                        begin,
                                                                        end);
    }

    class OPENVRML_LOCAL null_actions {
    public:
        struct null_action {
            template <typename Iterator>
            void operator()(const Iterator &, const Iterator &) const
            {}
        };

        null_action scheme, scheme_specific_part, userinfo, host, port,
            authority, path, query, fragment;
    };


    struct OPENVRML_LOCAL uri_reserved_parser :
        public boost::spirit::char_parser<uri_reserved_parser> {

        typedef uri_reserved_parser self_t;

        template <typename CharT>
        bool test(CharT ch) const
        {
            return ch == ';'
                || ch == '/'
                || ch == '?'
                || ch == ':'
                || ch == '@'
                || ch == '&'
                || ch == '='
                || ch == '+'
                || ch == '$'
                || ch == ',';
        }
    };

    const uri_reserved_parser uri_reserved_p = uri_reserved_parser();


    struct OPENVRML_LOCAL uri_unreserved_parser :
        public boost::spirit::char_parser<uri_unreserved_parser> {

        typedef uri_unreserved_parser self_t;

        template <typename CharT>
        bool test(CharT ch) const
        {
            using namespace std;
            return isalnum(char_traits<CharT>::to_int_type(ch))
                || ch == '-'
                || ch == '_'
                || ch == '.'
                || ch == '!'
                || ch == '~'
                || ch == '*'
                || ch == '\''
                || ch == '('
                || ch == ')';
        }
    };

    const uri_unreserved_parser uri_unreserved_p = uri_unreserved_parser();


    struct OPENVRML_LOCAL uric_grammar :
        public boost::spirit::grammar<uric_grammar> {

        template <typename ScannerT>
        struct definition {
            typedef boost::spirit::rule<ScannerT> rule_type;

            rule_type uric, escaped;

            definition(const uric_grammar & self);

            const boost::spirit::rule<ScannerT> & start() const;
        };
    };

    template <typename ScannerT>
    uric_grammar::definition<ScannerT>::definition(const uric_grammar &)
    {
        using namespace boost::spirit;

        uric
            =   uri_reserved_p
            |   uri_unreserved_p
            |   escaped
            ;

        escaped
            =   '%' >> xdigit_p >> xdigit_p
            ;
    }

    template <typename ScannerT>
    const boost::spirit::rule<ScannerT> &
    uric_grammar::definition<ScannerT>::start() const
    {
        return this->uric;
    }


    template <typename Actions = null_actions>
    struct uri_authority_grammar :
        public boost::spirit::grammar<uri_authority_grammar<Actions> > {

        template <typename ScannerT>
        struct definition {
            struct server_closure :
                boost::spirit::closure<server_closure,
                                       typename ScannerT::iterator_t,
                                       typename ScannerT::iterator_t> {
                typename server_closure::member1 userinfo_begin;
                typename server_closure::member2 userinfo_end;
            };

            typedef boost::spirit::rule<ScannerT> rule_type;
            typedef boost::spirit::rule<ScannerT,
                                        typename server_closure::context_t>
                server_rule_type;

            rule_type authority;
            rule_type reg_name;
            server_rule_type server;
            rule_type userinfo;
            rule_type hostport;
            rule_type host;
            rule_type hostname;
            rule_type domainlabel;
            rule_type toplabel;
            rule_type ipv4address;
            rule_type port;
            rule_type escaped;

            explicit definition(const uri_authority_grammar & self);

            const boost::spirit::rule<ScannerT> & start() const;
        };

        const Actions & actions;

        explicit uri_authority_grammar(const Actions & actions = Actions());
    };

    template <typename Actions>
    template <typename ScannerT>
    uri_authority_grammar<Actions>::definition<ScannerT>::
    definition(const uri_authority_grammar & self)
    {
        using namespace boost::spirit;
        using namespace phoenix;

        authority
            =   (server | reg_name)[ self.actions.authority ]
            ;

        reg_name
            =  +(   uri_unreserved_p
                |   escaped
                |   '$'
                |   ','
                |   ';'
                |   ':'
                |   '@'
                |   '&'
                |   '='
                |   '+'
                )
            ;

        server
            =  !(
                   !(
                        userinfo[
                            server.userinfo_begin = arg1,
                            server.userinfo_end = arg2
                        ] >> '@'
                    )[
                        assign_iterators(self.actions.userinfo,
                                         server.userinfo_begin,
                                         server.userinfo_end)
                    ]
                    >> hostport
                )
            ;

        userinfo
            =  *(   uri_unreserved_p
                |   escaped
                |   ';'
                |   ':'
                |   '&'
                |   '='
                |   '+'
                |   '$'
                |   ','
                )
            ;

        hostport
            =   host >> !(':' >> port)
            ;

        host
            =   (hostname | ipv4address)[ self.actions.host ]
            ;

        hostname
            =   *(domainlabel >> '.') >> toplabel >> !ch_p('.')
            ;

        domainlabel
            =   alnum_p >> *(*ch_p('-') >> alnum_p)
            ;

        toplabel
            =   alpha_p >> *(*ch_p('-') >> alnum_p)
            ;

        ipv4address
            =   +digit_p >> '.' >> +digit_p >> '.' >> +digit_p >> '.'
                >> +digit_p
            ;

        port
            =   (*digit_p)[ self.actions.port ]
            ;

        escaped
            =   '%' >> xdigit_p >> xdigit_p
            ;
    }

    template <typename Actions>
    template <typename ScannerT>
    const boost::spirit::rule<ScannerT> &
    uri_authority_grammar<Actions>::definition<ScannerT>::start() const
    {
        return this->authority;
    }

    template <typename Actions>
    uri_authority_grammar<Actions>::
    uri_authority_grammar(const Actions & actions):
        actions(actions)
    {}

    template <typename Actions>
    struct uri_abs_path_grammar :
        public boost::spirit::grammar<uri_abs_path_grammar<Actions> > {

        template <typename ScannerT>
        struct definition {
            typedef boost::spirit::rule<ScannerT> rule_type;

            rule_type abs_path;
            rule_type path_segments;
            rule_type segment;
            rule_type param;
            rule_type pchar;
            rule_type query;
            rule_type escaped;

            explicit definition(const uri_abs_path_grammar & self);

            const boost::spirit::rule<ScannerT> & start() const;
        };

        const Actions & actions;

        explicit uri_abs_path_grammar(const Actions & actions = Actions());
    };

    template <typename Actions>
    template <typename ScannerT>
    uri_abs_path_grammar<Actions>::definition<ScannerT>::
    definition(const uri_abs_path_grammar & self)
    {
        using namespace boost::spirit;
        using namespace phoenix;

        abs_path
            =   ('/' >> path_segments)[ self.actions.path ]
            ;

        path_segments
            =   segment >> *('/' >> segment)
            ;

        segment
            =   *pchar >> *(';' >> param)
            ;

        param
            =   *pchar
            ;

        pchar
            =   uri_unreserved_p
            |   escaped
            |   ':'
            |   '@'
            |   '&'
            |   '='
            |   '+'
            |   '$'
            |   ','
            ;

        escaped
            =   '%' >> xdigit_p >> xdigit_p
            ;
    }

    template <typename Actions>
    template <typename ScannerT>
    const boost::spirit::rule<ScannerT> &
    uri_abs_path_grammar<Actions>::definition<ScannerT>::start() const
    {
        return this->abs_path;
    }

    template <typename Actions>
    uri_abs_path_grammar<Actions>::
    uri_abs_path_grammar(const Actions & actions):
        actions(actions)
    {}


    template <typename Actions = null_actions>
    struct absolute_uri_grammar :
        public boost::spirit::grammar<absolute_uri_grammar<Actions> > {

        template <typename ScannerT>
        struct definition {
            struct absolute_uri_closure :
                boost::spirit::closure<absolute_uri_closure,
                                       typename ScannerT::iterator_t,
                                       typename ScannerT::iterator_t> {
                typename absolute_uri_closure::member1 scheme_begin;
                typename absolute_uri_closure::member2 scheme_end;
            };

            struct server_closure :
                boost::spirit::closure<server_closure,
                                       typename ScannerT::iterator_t,
                                       typename ScannerT::iterator_t> {
                typename server_closure::member1 userinfo_begin;
                typename server_closure::member2 userinfo_end;
            };

            typedef boost::spirit::rule<ScannerT> rule_type;
            typedef boost::spirit::rule<
                ScannerT,
                typename absolute_uri_closure::context_t>
                absolute_uri_rule_type;
            typedef boost::spirit::rule<ScannerT,
                                        typename server_closure::context_t>
                server_rule_type;

            absolute_uri_rule_type absolute_uri;
            rule_type scheme;
            rule_type hier_part;
            rule_type opaque_part;
            rule_type net_path;
            uri_abs_path_grammar<Actions> abs_path;
            uri_authority_grammar<Actions> authority;
            rule_type query;
            rule_type uric_no_slash;
            uric_grammar uric;

            explicit definition(const absolute_uri_grammar & self);

            const absolute_uri_rule_type & start() const;
        };

        const Actions & actions;

        explicit absolute_uri_grammar(const Actions & actions = Actions());
    };

    template <typename Actions>
    template <typename ScannerT>
    absolute_uri_grammar<Actions>::definition<ScannerT>::
    definition(const absolute_uri_grammar & self):
        abs_path(self.actions),
        authority(self.actions)
    {
        using namespace boost::spirit;
        using namespace phoenix;

        absolute_uri
            =   (
                    scheme[
                        absolute_uri.scheme_begin = arg1,
                        absolute_uri.scheme_end = arg2
                    ] >> ':'
                )[
                    assign_iterators(self.actions.scheme,
                                     absolute_uri.scheme_begin,
                                     absolute_uri.scheme_end)
                ] >> (hier_part | opaque_part)[
                    self.actions.scheme_specific_part
                ]
            ;

        scheme
            =   (alpha_p >> *(alpha_p | digit_p | '+' | '-' | '.'))
            ;

        hier_part
            =   (net_path | abs_path) >> !('?' >> query)
            ;

        opaque_part
            =   uric_no_slash >> *uric
            ;

        uric_no_slash
            =   uric - '/'
            ;

        net_path
            =   "//" >> authority >> !abs_path
            ;

        query
            =   (*uric)[ self.actions.query ]
            ;

    }

    template <typename Actions>
    template <typename ScannerT>
    const typename absolute_uri_grammar<Actions>::
    template definition<ScannerT>::absolute_uri_rule_type &
    absolute_uri_grammar<Actions>::definition<ScannerT>::start() const
    {
        return this->absolute_uri;
    }

    template <typename Actions>
    absolute_uri_grammar<Actions>::
    absolute_uri_grammar(const Actions & actions):
        actions(actions)
    {}


    template <typename Actions = null_actions>
    struct uri_grammar : public boost::spirit::grammar<uri_grammar<Actions> > {

        template <typename ScannerT>
        struct definition {
            typedef boost::spirit::rule<ScannerT> rule_type;

            rule_type uri_reference;
            absolute_uri_grammar<Actions> absolute_uri;
            rule_type relative_uri;
            rule_type net_path;
            uri_abs_path_grammar<Actions> abs_path;
            rule_type rel_path;
            rule_type rel_segment;
            uri_authority_grammar<Actions> authority;
            rule_type query;
            rule_type fragment;
            uric_grammar uric;
            rule_type escaped;

            explicit definition(const uri_grammar & self);

            const boost::spirit::rule<ScannerT> & start() const;
        };

        const Actions & actions;

        explicit uri_grammar(const Actions & actions = Actions());
    };


    template <typename Actions>
    uri_grammar<Actions>::uri_grammar(const Actions & actions):
        actions(actions)
    {}

    template <typename Actions>
    template <typename ScannerT>
    uri_grammar<Actions>::definition<ScannerT>::
    definition(const uri_grammar & self):
        absolute_uri(self.actions),
        abs_path(self.actions),
        authority(self.actions)
    {
        using namespace boost::spirit;
        using namespace phoenix;

        BOOST_SPIRIT_DEBUG_NODE(uri_reference);
        BOOST_SPIRIT_DEBUG_NODE(absolute_uri);
        BOOST_SPIRIT_DEBUG_NODE(net_path);
        BOOST_SPIRIT_DEBUG_NODE(abs_path);
        BOOST_SPIRIT_DEBUG_NODE(rel_path);
        BOOST_SPIRIT_DEBUG_NODE(rel_segment);
        BOOST_SPIRIT_DEBUG_NODE(authority);
        BOOST_SPIRIT_DEBUG_NODE(query);
        BOOST_SPIRIT_DEBUG_NODE(fragment);
        BOOST_SPIRIT_DEBUG_NODE(uric);
        BOOST_SPIRIT_DEBUG_NODE(escaped);

        uri_reference
            =   !(absolute_uri | relative_uri) >> !('#' >> fragment)
            ;
        relative_uri
            =   (net_path | abs_path | rel_path) >> !('?' >> query)
            ;

        net_path
            =   "//" >> authority >> !abs_path
            ;

        rel_path
            =   (rel_segment >> !abs_path)[ self.actions.path ]
            ;

        rel_segment
            =  +(   uri_unreserved_p
                |   escaped
                |   ';'
                |   '@'
                |   '&'
                |   '='
                |   '+'
                |   '$'
                |   ','
                )
            ;

        query
            =   (*uric)[ self.actions.query ]
            ;

        fragment
            =   (*uric)[ self.actions.fragment ]
            ;

        escaped
            =   '%' >> xdigit_p >> xdigit_p
            ;
    }

    template <typename Actions>
    template <typename ScannerT>
    const boost::spirit::rule<ScannerT> &
    uri_grammar<Actions>::definition<ScannerT>::start() const
    {
        return this->uri_reference;
    }

    class OPENVRML_LOCAL uri {
        class actions {
            uri * uri_;

        public:
            explicit actions(uri & uri_):
                uri_(&uri_),
                scheme(*this),
                scheme_specific_part(*this),
                authority(*this),
                userinfo(*this),
                host(*this),
                port(*this),
                path(*this),
                query(*this),
                fragment(*this)
            {}

            struct scheme_action {
                explicit scheme_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->scheme_begin = first;
                    this->actions_->uri_->scheme_end = last;
                }

            private:
                actions * actions_;
            } scheme;

            struct scheme_specific_part_action {
                explicit scheme_specific_part_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->scheme_specific_part_begin = first;
                    this->actions_->uri_->scheme_specific_part_end = last;
                }

            private:
                actions * actions_;
            } scheme_specific_part;

            struct authority_action {
                explicit authority_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->authority_begin = first;
                    this->actions_->uri_->authority_end = last;
                }

            private:
                actions * actions_;
            } authority;

            struct userinfo_action {
                explicit userinfo_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->userinfo_begin = first;
                    this->actions_->uri_->userinfo_end = last;
                }

            private:
                actions * actions_;
            } userinfo;

            struct host_action {
                explicit host_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->host_begin = first;
                    this->actions_->uri_->host_end = last;
                }

            private:
                actions * actions_;
            } host;

            struct port_action {
                explicit port_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->port_begin = first;
                    this->actions_->uri_->port_end = last;
                }

            private:
                actions * actions_;
            } port;

            struct path_action {
                explicit path_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->path_begin = first;
                    this->actions_->uri_->path_end = last;
                }

            private:
                actions * actions_;
            } path;

            struct query_action {
                explicit query_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->query_begin = first;
                    this->actions_->uri_->query_end = last;
                }

            private:
                actions * actions_;
            } query;

            struct fragment_action {
                explicit fragment_action(actions & actions_):
                    actions_(&actions_)
                {}

                template <typename Iterator>
                void operator()(const Iterator & first,
                                const Iterator & last) const
                {
                    this->actions_->uri_->fragment_begin = first;
                    this->actions_->uri_->fragment_end = last;
                }

            private:
                actions * actions_;
            } fragment;
        };

        std::string str_;
        std::string::const_iterator scheme_begin, scheme_end;
        std::string::const_iterator scheme_specific_part_begin,
                                    scheme_specific_part_end;
        std::string::const_iterator authority_begin, authority_end;
        std::string::const_iterator userinfo_begin, userinfo_end;
        std::string::const_iterator host_begin, host_end;
        std::string::const_iterator port_begin, port_end;
        std::string::const_iterator path_begin, path_end;
        std::string::const_iterator query_begin, query_end;
        std::string::const_iterator fragment_begin, fragment_end;

    public:
        uri() OPENVRML_THROW1(std::bad_alloc);
        explicit uri(const std::string & str)
            OPENVRML_THROW2(openvrml::invalid_url, std::bad_alloc);

        operator std::string() const OPENVRML_THROW1(std::bad_alloc);

        const std::string scheme() const OPENVRML_THROW1(std::bad_alloc);
        const std::string scheme_specific_part() const
            OPENVRML_THROW1(std::bad_alloc);
        const std::string authority() const OPENVRML_THROW1(std::bad_alloc);
        const std::string userinfo() const OPENVRML_THROW1(std::bad_alloc);
        const std::string host() const OPENVRML_THROW1(std::bad_alloc);
        const std::string port() const OPENVRML_THROW1(std::bad_alloc);
        const std::string path() const OPENVRML_THROW1(std::bad_alloc);
        const std::string query() const OPENVRML_THROW1(std::bad_alloc);
        const std::string fragment() const OPENVRML_THROW1(std::bad_alloc);

        const uri resolve_against(const uri & absolute_uri) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    inline bool relative(const uri & id)
    {
        return id.scheme().empty();
    }

    uri::uri() OPENVRML_THROW1(std::bad_alloc)
    {}

    uri::uri(const std::string & str)
        OPENVRML_THROW2(openvrml::invalid_url, std::bad_alloc):
        str_(str)
    {
        using std::string;
        using namespace boost::spirit;

        actions a(*this);
        uri_grammar<actions> g(a);

        string::const_iterator begin = this->str_.begin();
        string::const_iterator end = this->str_.end();

        if (!parse(begin, end, g, space_p).full) {
            throw openvrml::invalid_url();
        }
    }

    uri::operator std::string() const OPENVRML_THROW1(std::bad_alloc)
    {
        return this->str_;
    }

    const std::string uri::scheme() const OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->scheme_begin, this->scheme_end);
    }

    const std::string uri::scheme_specific_part() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->scheme_specific_part_begin,
                           this->scheme_specific_part_end);
    }

    const std::string uri::authority() const OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->authority_begin, this->authority_end);
    }

    const std::string uri::userinfo() const OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->userinfo_begin, this->userinfo_end);
    }

    const std::string uri::host() const OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->host_begin, this->host_end);
    }

    const std::string uri::port() const OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->port_begin, this->port_end);
    }

    const std::string uri::path() const OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->path_begin, this->path_end);
    }

    const std::string uri::query() const OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->query_begin, this->query_end);
    }

    const std::string uri::fragment() const OPENVRML_THROW1(std::bad_alloc)
    {
        return std::string(this->fragment_begin, this->fragment_end);
    }

    const uri uri::resolve_against(const uri & absolute_uri) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using std::list;
        using std::string;
        using std::ostringstream;

        assert(relative(*this));
        assert(!relative(absolute_uri));

        ostringstream result;
        result.unsetf(ostringstream::skipws);
        result << absolute_uri.scheme() << ':';

        if (!this->authority().empty()) {
            result << this->scheme_specific_part();
            return uri(result.str());
        } else {
            result << "//" << absolute_uri.authority();
        }

        ostringstream path;
        path.unsetf(ostringstream::skipws);
        path << absolute_uri.path();
        const string::size_type last_slash_index =
            path.str().find_last_of('/');

        //
        // Chop off the leading slash and the last path segment (typically a
        // file name).
        //
        path.str(path.str().substr(1, last_slash_index));

        //
        // Append the relative path.
        //
        path << this->path();

        //
        // Put the path segments in a list to process them.
        //
        list<string> path_segments;
        string::size_type slash_index = 0;
        string::size_type segment_start_index = 0;
        do {
            slash_index = path.str().find('/', segment_start_index);
            string segment =
                path.str().substr(segment_start_index,
                                  slash_index - segment_start_index);
            if (!segment.empty()) {
                path_segments.push_back(segment);
            }
            segment_start_index = slash_index + 1;
        } while (slash_index != string::npos);

        //
        // Remove any "." segments.
        //
        path_segments.remove(".");

        //
        // Remove any ".." segments along with the segment that precedes them.
        //
        const list<string>::iterator begin(path_segments.begin());
        list<string>::iterator pos;
        for (pos = begin; pos != path_segments.end(); ++pos) {
            if (pos != begin && *pos == "..") {
                --(pos = path_segments.erase(pos));
                --(pos = path_segments.erase(pos));
            }
        }

        //
        // Reconstruct the path.
        //
        path.str(string());
        for (pos = path_segments.begin(); pos != path_segments.end(); ++pos) {
            path << '/' << *pos;
        }

        //
        // End in a slash?
        //
        if (*(this->path().end() - 1) == '/') { path << '/'; }

        result << path.str();

        const string query = this->query();
        if (!query.empty()) { result << '?' << query; }

        const string fragment = this->fragment();
        if (!fragment.empty()) { result << '#' << fragment; }

        uri result_uri;
        try {
            result_uri = uri(result.str());
        } catch (openvrml::invalid_url &) {
            assert(false); // If we constructed a bad URI, something is wrong.
        }

        return result_uri;
    }

    class OPENVRML_LOCAL profile {
        typedef std::map<std::string, int> map_t;
        map_t components_;

    protected:
        void add_component(const std::string & name, int level)
            OPENVRML_THROW1(std::bad_alloc);

    public:
        std::auto_ptr<openvrml::scope>
            create_root_scope(const openvrml::browser & browser,
                              const std::string & uri) const
            OPENVRML_THROW1(std::bad_alloc);
    };


    class OPENVRML_LOCAL vrml97_profile : public profile {
    public:
        static const char * const id;

        vrml97_profile();
    };


    class OPENVRML_LOCAL x3d_core_profile : public profile {
    public:
        static const char * const id;

        x3d_core_profile();
    };


    class OPENVRML_LOCAL x3d_interchange_profile : public profile {
    public:
        static const char * const id;

        x3d_interchange_profile();
    };


    class OPENVRML_LOCAL x3d_interactive_profile : public profile {
    public:
        static const char * const id;

        x3d_interactive_profile();
    };


    class OPENVRML_LOCAL x3d_mpeg4_profile : public profile {
    public:
        static const char * const id;

        x3d_mpeg4_profile();
    };


    class OPENVRML_LOCAL x3d_immersive_profile : public profile {
    public:
        static const char * const id;

        x3d_immersive_profile();
    };


    class OPENVRML_LOCAL component {
    public:
        virtual ~component() OPENVRML_NOTHROW = 0;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const = 0;

    protected:
        void add_scope_entry(
            const openvrml::browser & b,
            const char * node_name,
            const openvrml::node_interface_set & interface_set,
            const char * urn,
            openvrml::scope & scope) const
            OPENVRML_THROW3(openvrml::unsupported_interface,
                            std::invalid_argument, std::bad_alloc);
    };


    const class OPENVRML_LOCAL profile_registry : boost::ptr_map<std::string,
                                                                 profile> {
    public:
        profile_registry();

        using boost::ptr_map<std::string, profile>::at;
    } profile_registry_;

    profile_registry::profile_registry()
    {
        std::string key;
        bool succeeded;

        key = vrml97_profile::id;
        succeeded = this->insert(key, new vrml97_profile).second;
        assert(succeeded);

        key = x3d_core_profile::id;
        succeeded = this->insert(key, new x3d_core_profile).second;
        assert(succeeded);

        key = x3d_interchange_profile::id;
        succeeded = this->insert(key, new x3d_interchange_profile).second;
        assert(succeeded);

        key = x3d_interactive_profile::id;
        succeeded = this->insert(key, new x3d_interactive_profile).second;
        assert(succeeded);

        key = x3d_mpeg4_profile::id;
        succeeded = this->insert(key, new x3d_mpeg4_profile).second;
        assert(succeeded);

        key = x3d_immersive_profile::id;
        succeeded = this->insert(key, new x3d_immersive_profile).second;
        assert(succeeded);
    }


    const class OPENVRML_LOCAL component_registry : boost::ptr_map<std::string,
                                                                   component> {
    public:
        component_registry();

        using boost::ptr_map<std::string, component>::at;
    } component_registry_;
} // namespace

//
// Including a .cpp file is strange, but it's exactly what we want to do here.
// This usage lets us put the ANTLR parser in an unnamed namespace.
//
#   include "Vrml97Parser.cpp"
} // Close "namespace openvrml", opened in Vrml97Parser.cpp.
#   include "X3DVrmlParser.cpp"
} // Close "namespace openvrml", opened in X3DVrmlParser.cpp.

namespace {

    /**
     * @brief Parse a VRML stream.
     *
     * @param[in,out] in    input stream.
     * @param[in]     uri   URI associated with @p in.
     * @param[in]     type  MIME media type of the data to be read from @p in.
     * @param[in]     scene a scene.
     * @param[out]    nodes the root nodes.
     * @param[out]    meta  the scene metadata.
     *
     * @exception openvrml::bad_media_type
     * @exception openvrml::invalid_vrml
     */
    OPENVRML_LOCAL void
    parse_vrml(std::istream & in,
               const std::string & uri,
               const std::string & type,
               const openvrml::scene & scene,
               std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
               std::map<std::string, std::string> & meta)
    {
        try {
            using namespace openvrml;
            using boost::algorithm::iequals;

            if (iequals(type, vrml_media_type)
                || iequals(type, x_vrml_media_type)) {
                Vrml97Scanner scanner(in);
                Vrml97Parser parser(scanner, uri);
                parser.vrmlScene(scene, nodes, meta);
            } else if (iequals(type, x3d_vrml_media_type)) {
                X3DVrmlScanner scanner(in);
                X3DVrmlParser parser(scanner, uri);
                parser.vrmlScene(scene, nodes, meta);
            } else {
                throw bad_media_type(type);
            }
        } catch (antlr::RecognitionException & ex) {
            throw openvrml::invalid_vrml(ex.getFilename(),
                                         ex.getLine(),
                                         ex.getColumn(),
                                         ex.getMessage());
        } catch (antlr::ANTLRException & ex) {
            throw std::runtime_error(ex.getMessage());
        }
    }

    struct OPENVRML_LOCAL externproto_node_class::load_proto {
        load_proto(externproto_node_class & externproto_class,
                   const openvrml::scene & scene,
                   const std::vector<std::string> & alt_uris):
            externproto_node_class_(&externproto_class),
            scene_(&scene),
            alt_uris_(alt_uris)
        {}

        void operator()() const OPENVRML_NOTHROW
        try {
            using openvrml::unreachable_url;
            try {
                using namespace openvrml;
                using std::auto_ptr;
                using std::ostringstream;
                using std::string;
                using std::vector;
                using boost::dynamic_pointer_cast;
                using boost::shared_ptr;
                using openvrml_::scope_guard;
                using openvrml_::make_obj_guard;

                scope_guard guard =
                    make_obj_guard(
                        *this->externproto_node_class_,
                        &externproto_node_class::clear_externproto_node_types);
                boost::ignore_unused_variable_warning(guard);

                auto_ptr<resource_istream> in =
                    this->scene_->get_resource(this->alt_uris_);
                if (!(*in)) { throw unreachable_url(); }

                //
                // We don't actually do anything with these; but the parser
                // wants them.
                //
                vector<boost::intrusive_ptr<node> > nodes;
                std::map<string, string> meta;

                parse_vrml(*in, in->url(), in->type(),
                           *this->scene_, nodes, meta);

                shared_ptr<openvrml::proto_node_class> proto_node_class;
                for (vector<string>::const_iterator alt_uri =
                         this->alt_uris_.begin();
                     alt_uri != this->alt_uris_.end() && !proto_node_class;
                     ++alt_uri) {
                    const uri absolute_uri = !relative(uri(*alt_uri))
                        ? uri(*alt_uri)
                        : uri(*alt_uri).resolve_against(
                            uri(this->scene_->url()));

                    const shared_ptr<openvrml::node_class> node_class =
                        this->scene_->browser()
                        .node_class(node_class_id(absolute_uri));

                    proto_node_class =
                        dynamic_pointer_cast<openvrml::proto_node_class>(
                            node_class);
                }

                if (!proto_node_class) {
                    ostringstream err_msg;
                    err_msg << "no PROTO definition at <" << in->url() << ">";
                    this->scene_->browser().err(err_msg.str());
                    return;
                }

                this->externproto_node_class_
                    ->set_proto_node_class(proto_node_class);

            } catch (std::exception & ex) {
                this->scene_->browser().err(ex.what());
                throw unreachable_url();
            } catch (...) {
                //
                // The implementation of resource_istream is provided by
                // the user; and unfortunately, operations on it could
                // throw anything.
                //
                throw unreachable_url();
            }
        } catch (std::exception & ex) {
            this->scene_->browser().err(ex.what());
        }

    private:
        externproto_node_class * externproto_node_class_;
        const openvrml::scene * scene_;
        std::vector<std::string> alt_uris_;
    };

    /**
     * @brief Construct.
     *
     * @param[in] scene the <code>scene</code> in which the EXTERNPROTO occurs.
     * @param[in] uris  the list of alternative implementation identifiers.
     *
     * @exception boost::thread_resource_error  if a new thread of execution
     *                                          cannot be started.
     */
    externproto_node_class::
    externproto_node_class(const openvrml::scene & scene,
                           const std::vector<std::string> & uris)
        OPENVRML_THROW1(boost::thread_resource_error):
        node_class(scene.browser()),
        externproto_node_types_cleared_(false),
        load_proto_thread_(
            new boost::thread(
                boost::function0<void>(load_proto(*this, scene, uris))))
    {}

    /**
     * @brief Destroy.
     */
    externproto_node_class::~externproto_node_class() OPENVRML_NOTHROW
    {}

    const boost::shared_ptr<openvrml::node_type>
    externproto_node_class::
    do_create_type(const std::string & id,
                   const openvrml::node_interface_set & interfaces) const
        OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
    {
        boost::mutex::scoped_lock lock(this->mutex_);

        using boost::shared_ptr;

        shared_ptr<openvrml::proto_node_class> shared;
        if ((shared = this->proto_node_class_.lock())) {
            return shared->create_type(id, interfaces);
        }

        const shared_ptr<externproto_node_type> node_type(
            new externproto_node_type(this->shared_from_this(),
                                      id,
                                      interfaces));

        this->externproto_node_types_.push_back(node_type);

        return node_type;
    }

    void externproto_node_class::do_shutdown(double) OPENVRML_NOTHROW
    {
        this->load_proto_thread_->join();
    }

    void externproto_node_class::set_proto_node_class(
        const boost::weak_ptr<openvrml::proto_node_class> & proto_node_class)
        OPENVRML_THROW1(std::bad_alloc)
    {
        boost::mutex::scoped_lock lock(this->mutex_);

        using boost::shared_ptr;
        using boost::static_pointer_cast;

        this->proto_node_class_ = proto_node_class;

        //
        // Now that we have a proto_node_class, we need to tell all the
        // externproto_node_types we've created so that they can in turn
        // tell the externproto_nodes they've created.
        //
        const shared_ptr<openvrml::proto_node_class> shared_proto_node_class =
            proto_node_class.lock();
        assert(shared_proto_node_class);
        for (externproto_node_types::const_iterator node_type =
                 this->externproto_node_types_.begin();
             node_type != this->externproto_node_types_.end();
             ++node_type) {
            assert(!node_type->expired());
            node_type->lock()->set_proto_node_type(*shared_proto_node_class);
        }
    }

    void externproto_node_class::clear_externproto_node_types()
        OPENVRML_NOTHROW
    {
        boost::mutex::scoped_lock lock(this->mutex_);

        using boost::shared_ptr;

        for (externproto_node_types::const_iterator node_type =
                 this->externproto_node_types_.begin();
             node_type != this->externproto_node_types_.end();
             ++node_type) {
            assert(!node_type->expired());
            node_type->lock()->clear_externproto_nodes();
        }

        this->externproto_node_types_.clear();
        this->externproto_node_types_cleared_ = true;
    }


    externproto_node_type::externproto_node_type(
        const boost::shared_ptr<const externproto_node_class> & c,
        const std::string & id,
        const openvrml::node_interface_set & interfaces)
        OPENVRML_THROW1(std::bad_alloc):
        node_type(*c, id),
        node_class_(c),
        interfaces_(interfaces)
    {}

    externproto_node_type::~externproto_node_type() OPENVRML_NOTHROW
    {}

    void
    externproto_node_type::
    set_proto_node_type(openvrml::proto_node_class & proto_node_class)
        OPENVRML_THROW1(std::bad_alloc)
    {
        boost::mutex::scoped_lock lock(this->mutex_);

        assert(!this->proto_node_type_);

        using boost::static_pointer_cast;

        this->proto_node_type_ =
            static_pointer_cast<openvrml::proto_node_type>(
                proto_node_class.create_type(this->id(), this->interfaces_));

        for (externproto_nodes::const_iterator node =
                 this->externproto_nodes_.begin();
             node != this->externproto_nodes_.end();
             ++node) {
            (*node)->set_proto_node(*this->proto_node_type_);
        }
    }

    void externproto_node_type::clear_externproto_nodes() OPENVRML_NOTHROW
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        this->externproto_nodes_.clear();
    }

    const openvrml::node_interface_set &
    externproto_node_type::do_interfaces() const throw()
    {
        return this->interfaces_;
    }

    const boost::intrusive_ptr<openvrml::node>
    externproto_node_type::do_create_node(
        const boost::shared_ptr<openvrml::scope> & scope,
        const openvrml::initial_value_map & initial_values) const
        OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                        std::bad_alloc)
    {
        //
        // externproto_node_class::mutex_ must be locked first.
        // externproto_node_class::clear_externproto_node_types must not be
        // initiated while we're doing this.
        // externproto_node_class::externproto_node_types_cleared_ is checked
        // later in this function and attempting to lock
        // externproto_node_class::mutex_ at that point creates a race
        // condition.
        //
        boost::mutex::scoped_lock
            lock_externproto_node_class(
                static_cast<const externproto_node_class &>(this->node_class())
                .mutex_),
            lock(this->mutex_);

        if (this->proto_node_type_) {
            return this->proto_node_type_->create_node(scope, initial_values);
        }

        using openvrml::externproto_node;
        const boost::intrusive_ptr<externproto_node> result(
            new externproto_node(*this, scope, initial_values));

        const externproto_node_class & node_class =
            static_cast<const externproto_node_class &>(this->node_class());
        if (!node_class.externproto_node_types_cleared_) {
            this->externproto_nodes_.push_back(result);
        } else {
            assert(this->externproto_nodes_.empty());
        }

        return result;
    }
} // namespace


template <typename FieldValue>
openvrml::externproto_node::externproto_exposedfield<FieldValue>::
externproto_exposedfield(externproto_node & node,
                         const FieldValue & initial_value) OPENVRML_NOTHROW:
    node_event_listener(node),
    openvrml::event_emitter(static_cast<field_value &>(*this)),
    FieldValue(initial_value),
    proto_eventin<FieldValue>(node),
    proto_eventout<FieldValue>(node, initial_value)
{}

template <typename FieldValue>
openvrml::externproto_node::externproto_exposedfield<FieldValue>::
~externproto_exposedfield() OPENVRML_NOTHROW
{}

template <typename FieldValue>
std::auto_ptr<openvrml::field_value>
openvrml::externproto_node::externproto_exposedfield<FieldValue>::
do_clone() const
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::polymorphic_downcast;
    return std::auto_ptr<field_value>(
        new externproto_exposedfield<FieldValue>(
            *polymorphic_downcast<externproto_node *>(&this->node()),
            *this));
}


template <typename FieldValue>
void
openvrml::externproto_node::externproto_exposedfield<FieldValue>::
do_process_event(const FieldValue & value, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    static_cast<FieldValue &>(*this) = value;
    this->proto_eventin<FieldValue>::do_process_event(value, timestamp);
    this->node().modified(true);
    node::emit_event(*this, timestamp);
}

const boost::shared_ptr<openvrml::field_value>
openvrml::externproto_node::
create_exposedfield(externproto_node & node, field_value::type_id type)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::mpl::for_each;
    using openvrml_::field_value_types;

    boost::shared_ptr<field_value> result;
    for_each<field_value_types>(externproto_exposedfield_creator(node,
                                                                 type,
                                                                 result));
    assert(result.get());
    return result;
}

openvrml::externproto_node::externproto_node(
    const externproto_node_type & type,
    const boost::shared_ptr<openvrml::scope> & scope,
    const initial_value_map & initial_values)
    OPENVRML_THROW1(std::bad_alloc):
    abstract_proto_node(type, scope)
{
    for (node_interface_set::const_iterator interface =
             type.interfaces().begin();
         interface != type.interfaces().end();
         ++interface)
    {
        using boost::shared_ptr;
        using boost::dynamic_pointer_cast;

        bool succeeded = false;
        std::auto_ptr<openvrml::field_value> field_auto_ptr;
        shared_ptr<openvrml::event_listener> interface_eventin;
        shared_ptr<openvrml::event_emitter> interface_eventout;
        shared_ptr<openvrml::field_value> interface_field;
        switch (interface->type) {
        case node_interface::eventin_id:
            succeeded = this->eventin_map
                .insert(make_pair(interface->id,
                                  create_eventin(interface->field_type,
                                                 *this)))
                .second;
            break;
        case node_interface::eventout_id:
            succeeded = this->eventout_map
                .insert(make_pair(interface->id,
                                  create_eventout(interface->field_type,
                                                  *this)))
                .second;
            break;
        case node_interface::exposedfield_id:
            interface_field = create_exposedfield(*this,
                                                  interface->field_type);
            succeeded = this->field_map_
                .insert(make_pair(interface->id, interface_field)).second;
            assert(succeeded);

            interface_eventin =
                dynamic_pointer_cast<openvrml::event_listener>(
                    interface_field);
            succeeded = this->eventin_map
                .insert(make_pair("set_" + interface->id,
                                  interface_eventin))
                .second;
            assert(succeeded);

            interface_eventout =
                dynamic_pointer_cast<openvrml::event_emitter>(
                    interface_eventin);
            succeeded = this->eventout_map
                .insert(make_pair(interface->id + "_changed",
                                  interface_eventout))
                .second;
            break;
        case node_interface::field_id:
            field_auto_ptr = field_value::create(interface->field_type);
            succeeded = this->field_map_
                .insert(
                    make_pair(interface->id,
                              shared_ptr<field_value>(field_auto_ptr)))
                .second;
            break;
        case node_interface::invalid_type_id:
            assert(false
                   && "got node_interface::invalid_type_id for interface->type");
        }
        assert(succeeded);
    }

    //
    // Set the initial values.
    //
    for (initial_value_map::const_iterator map_entry =
             initial_values.begin();
         map_entry != initial_values.end();
         ++map_entry) {
        const field_map::const_iterator pos =
            this->field_map_.find(map_entry->first);
        if (pos == this->field_map_.end()) {
            throw unsupported_interface(this->type(), map_entry->first);
        }
        pos->second->assign(*map_entry->second);
    }
}

openvrml::externproto_node::~externproto_node() OPENVRML_NOTHROW
{}

bool openvrml::externproto_node::modified() const
{
    return this->proto_node_
        ? this->proto_node_->modified()
        : false;
}

void
openvrml::externproto_node::
set_proto_node(proto_node_type & node_type)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());

    using boost::static_pointer_cast;

    this->proto_node_ =
        static_pointer_cast<proto_node>(
            node_type.create_node(this->scope_, this->field_map_));

    for (eventin_map_t::const_iterator map_entry =
             this->eventin_map.begin();
         map_entry != this->eventin_map.end();
         ++map_entry) {
        using boost::mpl::for_each;
        using openvrml_::field_value_types;

        openvrml::event_listener & eventin =
            this->proto_node_->event_listener(map_entry->first);
        for_each<field_value_types>(eventin_is(map_entry->second->type(),
                                               eventin,
                                               *map_entry->second));
    }

    for (eventout_map_t::const_iterator map_entry =
             this->eventout_map.begin();
         map_entry != this->eventout_map.end();
         ++map_entry) {
        using boost::mpl::for_each;
        using openvrml_::field_value_types;

        openvrml::event_emitter & eventout =
            this->proto_node_->event_emitter(map_entry->first);
        for_each<field_value_types>(
            eventout_is(map_entry->second->value().type(),
                        eventout,
                        *map_entry->second));
    }

    if (this->scene()) {
        this->proto_node_->initialize(*this->scene(),
                                      browser::current_time());
    }
}

void openvrml::externproto_node::do_initialize(const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    if (this->proto_node_) {
        this->proto_node_->initialize(*this->scene(), timestamp);
    }
}

const openvrml::field_value &
openvrml::externproto_node::do_field(const std::string & id) const
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_field(id);
    }

    field_map::const_iterator pos = this->field_map_.find(id);
    if (pos == this->field_map_.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::field_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

openvrml::event_listener &
openvrml::externproto_node::do_event_listener(const std::string & id)
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_event_listener(id);
    }

    eventin_map_t::const_iterator pos = this->eventin_map.find(id);
    if (pos == this->eventin_map.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::eventin_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

openvrml::event_emitter &
openvrml::externproto_node::do_event_emitter(const std::string & id)
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_event_emitter(id);
    }

    eventout_map_t::const_iterator pos = this->eventout_map.find(id);
    if (pos == this->eventout_map.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::eventout_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

void openvrml::externproto_node::do_shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    if (this->proto_node_) {
        this->proto_node_->shutdown(timestamp);
    }
}

/**
 * @brief Cast to a script_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a script_node, or 0 otherwise.
 */
openvrml::script_node * openvrml::externproto_node::to_script()
    OPENVRML_NOTHROW
{
    return node_cast<script_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to an appearance_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is an appearance_node, or 0 otherwise.
 */
openvrml::appearance_node *
openvrml::externproto_node::to_appearance() OPENVRML_NOTHROW
{
    return node_cast<appearance_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a <code>bounded_volume_node</code>.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a <code>bounded_volume_node</code>, or 0 otherwise.
 */
openvrml::bounded_volume_node * openvrml::externproto_node::to_bounded_volume()
    OPENVRML_NOTHROW
{
    return node_cast<bounded_volume_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a child_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a child_node, or 0 otherwise.
 */
openvrml::child_node * openvrml::externproto_node::to_child() OPENVRML_NOTHROW
{
    return node_cast<child_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a color_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a color_node, or 0 otherwise.
 */
openvrml::color_node * openvrml::externproto_node::to_color() OPENVRML_NOTHROW
{
    return node_cast<color_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c color_rgba_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c color_rgba_node, or 0 otherwise.
 */
openvrml::color_rgba_node * openvrml::externproto_node::to_color_rgba()
    OPENVRML_NOTHROW
{
    return node_cast<color_rgba_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a coordinate_node, or 0 otherwise.
 */
openvrml::coordinate_node *
openvrml::externproto_node::to_coordinate() OPENVRML_NOTHROW
{
    return node_cast<coordinate_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a font_style_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a font_style_node, or 0 otherwise.
 */
openvrml::font_style_node *
openvrml::externproto_node::to_font_style() OPENVRML_NOTHROW
{
    return node_cast<font_style_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a geometry_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a geometry_node, or 0 otherwise.
 */
openvrml::geometry_node * openvrml::externproto_node::to_geometry()
    OPENVRML_NOTHROW
{
    return node_cast<geometry_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a <code>grouping_node</code>.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a <code>grouping_node</code>, or 0 otherwise.
 */
openvrml::grouping_node * openvrml::externproto_node::to_grouping()
    OPENVRML_NOTHROW
{
    return node_cast<grouping_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a <code>light_node</code>.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a <code>light_node</code>, or 0 otherwise.
 */
openvrml::light_node * openvrml::externproto_node::to_light() OPENVRML_NOTHROW
{
    return node_cast<light_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a material_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a material_node, or 0 otherwise.
 */
openvrml::material_node * openvrml::externproto_node::to_material()
    OPENVRML_NOTHROW
{
    return node_cast<material_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a <code>navigation_info_node</code>.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a <code>navigation_info_node</code>, or 0 otherwise.
 */
openvrml::navigation_info_node *
openvrml::externproto_node::to_navigation_info()
    OPENVRML_NOTHROW
{
    return node_cast<navigation_info_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a normal_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a normal_node, or 0 otherwise.
 */
openvrml::normal_node * openvrml::externproto_node::to_normal()
    OPENVRML_NOTHROW
{
    return node_cast<normal_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a <code>pointing_device_sensor_node</code>.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a <code>pointing_device_sensor_node</code>, or 0 otherwise.
 */
openvrml::pointing_device_sensor_node *
openvrml::externproto_node::to_pointing_device_sensor()
    OPENVRML_NOTHROW
{
    return node_cast<pointing_device_sensor_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a <code>scoped_light_node</code>.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a <code>scoped_light_node</code>, or 0 otherwise.
 */
openvrml::scoped_light_node *
openvrml::externproto_node::to_scoped_light() OPENVRML_NOTHROW
{
    return node_cast<scoped_light_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a sound_source_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a sound_source_node, or 0 otherwise.
 */
openvrml::sound_source_node *
openvrml::externproto_node::to_sound_source() OPENVRML_NOTHROW
{
    return node_cast<sound_source_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a texture_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a texture_node, or 0 otherwise.
 */
openvrml::texture_node * openvrml::externproto_node::to_texture()
    OPENVRML_NOTHROW
{
    return node_cast<texture_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a texture_coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a texture_coordinate_node, or 0 otherwise.
 */
openvrml::texture_coordinate_node *
openvrml::externproto_node::to_texture_coordinate()
    OPENVRML_NOTHROW
{
    return node_cast<texture_coordinate_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a texture_transform_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a texture_transform_node, or 0 otherwise.
 */
openvrml::texture_transform_node *
openvrml::externproto_node::to_texture_transform()
    OPENVRML_NOTHROW
{
    return node_cast<texture_transform_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a <code>time_dependent_node</code>.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a <code>time_dependent_node</code>, or 0 otherwise.
 */
openvrml::time_dependent_node * openvrml::externproto_node::to_time_dependent()
    OPENVRML_NOTHROW
{
    return node_cast<time_dependent_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a transform_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a transform_node, or 0 otherwise.
 */
openvrml::transform_node * openvrml::externproto_node::to_transform()
    OPENVRML_NOTHROW
{
    return node_cast<transform_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a viewpoint_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a viewpoint_node, or 0 otherwise.
 */
openvrml::viewpoint_node * openvrml::externproto_node::to_viewpoint()
    OPENVRML_NOTHROW
{
    return node_cast<viewpoint_node *>(this->proto_node_.get());
}


// Max time in seconds between updates. Make this user
// setable to balance performance with cpu usage.
#ifndef DEFAULT_DELTA
#define DEFAULT_DELTA 0.5
#endif

/**
 * @namespace openvrml
 *
 * @brief The OpenVRML Runtime Library
 */

/**
 * @brief VRML MIME media type.
 */
const char openvrml::vrml_media_type[11] = "model/vrml";

/**
 * @brief Experimental VRML MIME media type.
 */
const char openvrml::x_vrml_media_type[15] = "x-world/x-vrml";

/**
 * @brief X3D VRML MIME media type.
 */
const char openvrml::x3d_vrml_media_type[15] = "model/x3d+vrml";

/**
 * @class openvrml::resource_istream
 *
 * @brief An abstract input stream for network resources.
 *
 * <code>resource_istream</code> inherits <code>std::istream</code>, adding
 * functions to get the URI and the MIME content type associated with the
 * stream. Users of the library must provide an implementation of this class,
 * to be returned from <code>openvrml::browser::do_get_resource</code>.
 *
 * @sa openvrml::browser::do_get_resource(const std::string &)
 */

/**
 * @brief Construct.
 *
 * @param[in] streambuf a stream buffer.
 */
openvrml::resource_istream::resource_istream(std::streambuf * streambuf):
    std::istream(streambuf)
{}

/**
 * @brief Destroy.
 */
openvrml::resource_istream::~resource_istream()
{}

/**
 * @brief Get the URL associated with the stream.
 *
 * This function delegates to @c do_url.
 *
 * @return the URL associated with the stream.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string openvrml::resource_istream::url() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->do_url();
}

/**
 * @fn const std::string openvrml::resource_istream::do_url() const
 *
 * @brief Get the URL associated with the stream.
 *
 * @return the URL associated with the stream.
 */

/**
 * @brief Get the MIME content type associated with the stream.
 *
 * This function delegates to @c do_type.
 *
 * @return the MIME content type associated with the stream.
 */
const std::string openvrml::resource_istream::type() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->do_type();
}

/**
 * @fn const std::string openvrml::resource_istream::do_type() const
 *
 * @brief Get the MIME content type associated with the stream.
 *
 * @return the MIME content type associated with the stream.
 */

/**
 * @brief Indicates whether data is available to be read from the stream.
 *
 * This function delegates to @c do_data_available.
 *
 * @return @c true if there is data in the stream buffer to be read; @c false
 *         otherwise.
 */
bool openvrml::resource_istream::data_available() const OPENVRML_NOTHROW
{
    return this->do_data_available();
}

/**
 * @fn bool openvrml::resource_istream::do_data_available() const
 *
 * @brief Indicates whether data is available to be read from the stream.
 *
 * @return @c true if there is data in the stream buffer to be read; @c false
 *         otherwise.
 */


/**
 * @class openvrml::stream_listener
 *
 * @brief An interface to simplify asynchronously reading a
 *        <code>resource_istream</code>.
 */

/**
 * @brief Destroy.
 */
openvrml::stream_listener::~stream_listener() OPENVRML_NOTHROW
{}

/**
 * @brief Called once the stream is available for use.
 *
 * This function calls <code>stream_listener::do_stream_available</code>.
 *
 * @param[in] uri           the URI associated with the stream.
 * @param[in] media_type    the MIME media type for the stream.
 */
void
openvrml::stream_listener::stream_available(const std::string & uri,
                                            const std::string & media_type)
{
    this->do_stream_available(uri, media_type);
}

/**
 * @fn void openvrml::stream_listener::do_stream_available(const std::string & uri, const std::string & media_type)
 *
 * @brief Called by <code>stream_listener::stream_available</code>.
 *
 * Concrete <code>stream_listener</code>s must override this function.
 *
 * @param[in] uri           the URI associated with the stream.
 * @param[in] media_type    the MIME media type for the stream.
 */

/**
 * @brief Called when data is available.
 *
 * This function calls <code>stream_listener::do_data_available</code>.
 *
 * @param[in] data  the data.
 */
void
openvrml::stream_listener::
data_available(const std::vector<unsigned char> & data)
{
    this->do_data_available(data);
}

/**
 * @fn void openvrml::stream_listener::do_data_available(const std::vector<unsigned char> & data)
 *
 * @brief Called by <code>stream_listener::data_available</code>
 *
 * @param[in] data  the data.
 */

namespace {

    struct OPENVRML_LOCAL stream_reader {
        stream_reader(std::auto_ptr<openvrml::resource_istream> in,
                      std::auto_ptr<openvrml::stream_listener> listener):
            in_(in),
            listener_(listener)
        {}

        void operator()() const
        {
            this->listener_->stream_available(this->in_->url(),
                                              this->in_->type());
            while (*this->in_) {
                std::vector<unsigned char> data;
                while (this->in_->data_available()) {
                    using openvrml::resource_istream;
                    const resource_istream::int_type c = this->in_->get();
                    if (c != resource_istream::traits_type::eof()) {
                        data.push_back(
                            resource_istream::traits_type::to_char_type(c));
                    } else {
                        break;
                    }
                }
                this->listener_->data_available(data);
            }
        }

    private:
        boost::shared_ptr<openvrml::resource_istream> in_;
        boost::shared_ptr<openvrml::stream_listener> listener_;
    };
}

/**
 * @brief Read a stream in a new thread.
 *
 * <code>read_stream</code> takes ownership of its arguments; the resources
 * are released when reading the stream completes and the thread terminates.
 *
 * @param[in] in        an input stream.
 * @param[in] listener  a stream listener.
 */
void openvrml::read_stream(std::auto_ptr<resource_istream> in,
                           std::auto_ptr<stream_listener> listener)
{
    boost::function0<void> f = stream_reader(in, listener);
    boost::thread t(f);
}

/**
 * @class openvrml::invalid_vrml
 *
 * @brief Exception thrown when the parser fails due to errors in the VRML
 *        input.
 */

/**
 * @var const std::string openvrml::invalid_vrml::url
 *
 * @brief Resource identifier.
 */

/**
 * @var size_t openvrml::invalid_vrml::line
 *
 * @brief Line number.
 */

/**
 * @var size_t openvrml::invalid_vrml::column
 *
 * @brief Column number.
 */

/**
 * @brief Construct.
 *
 * @param[in] url       resource identifier of the stream.
 * @param[in] line      line number where the error was detected.
 * @param[in] column    column number where the error was detected.
 * @param[in] message   description of the error.
 */
openvrml::invalid_vrml::invalid_vrml(const std::string & url,
                                     const size_t line,
                                     const size_t column,
                                     const std::string & message):
    std::runtime_error(message),
    url(url),
    line(line),
    column(column)
{}

/**
 * @brief Destroy.
 */
openvrml::invalid_vrml::~invalid_vrml() throw ()
{}


/**
 * @class openvrml::viewer_in_use
 *
 * @brief Exception thrown when attempting to associate a <code>viewer</code>
 *        with a <code>browser</code> when the <code>viewer</code> is already
 *        associated with a <code>browser</code>.
 */

/**
 * @brief Construct.
 */
openvrml::viewer_in_use::viewer_in_use():
    std::invalid_argument("viewer in use")
{}

/**
 * @brief Destroy.
 */
openvrml::viewer_in_use::~viewer_in_use() throw ()
{}


/**
 * @class openvrml::browser_event
 *
 * @brief A <code>browser</code>-wide event.
 */

/**
 * @var openvrml::browser_event::browser
 *
 * @brief Only <code>browser</code>s can construct <code>browser_event</code>s.
 */

/**
 * @enum openvrml::browser_event::type_id
 *
 * @brief <code>browser_event</code> type identifier.
 */

/**
 * @var openvrml::browser_event::initialized
 *
 * @brief An <code>initialized</code> event is sent once the world has loaded
 *        and all nodes in the initial scene have been initialized.
 */

/**
 * @var openvrml::browser_event::shutdown
 *
 * @brief A <code>shutdown</code> event is sent once all of the nodes in the
 *        scene have been shut down.
 */

/**
 * @internal
 *
 * @var openvrml::browser * openvrml::browser_event::source_
 *
 * @brief The <code>browser</code> from which the event originated.
 */

/**
 * @internal
 *
 * @var openvrml::browser_event::type_id openvrml::browser_event::id_
 *
 * @brief Event type identifier.
 */

/**
 * @internal
 *
 * @brief Construct.
 *
 * @param[in] b     the <code>browser</code> from which the event originated.
 * @param[in] id    the event type.
 */
openvrml::browser_event::browser_event(browser & b, type_id id)
    OPENVRML_NOTHROW:
    source_(&b),
    id_(id)
{}

/**
 * @brief Event type identifier.
 *
 * @return the event type identifier.
 */
openvrml::browser_event::type_id openvrml::browser_event::id() const
    OPENVRML_NOTHROW
{
    return this->id_;
}

/**
 * @brief The <code>browser</code> that emitted the event.
 *
 * @return the <code>browser</code> that emitted the event.
 */
openvrml::browser & openvrml::browser_event::source() const OPENVRML_NOTHROW
{
    return *this->source_;
}


/**
 * @class openvrml::browser_listener
 *
 * @brief This class should be inherited by classes that want to listen for
 *        <code>browser_event</code>s.
 */

/**
 * @internal
 *
 * @var openvrml::browser_listener::browser
 *
 * @brief <code>browser</code> instances need to call
 *        <code>browser_listener::browser_changed</code>.
 */

/**
 * @brief Destroy.
 */
openvrml::browser_listener::~browser_listener() OPENVRML_NOTHROW
{}

/**
 * @internal
 *
 * @brief Send a <code>browser_event</code>.
 *
 * This function delegates to
 * <code>browser_listener::do_browser_changed</code>.
 *
 * @param[in] event the <code>browser_event</code> to send.
 */
void openvrml::browser_listener::browser_changed(const browser_event & event)
{
    this->do_browser_changed(event);
}

/**
 * @fn void openvrml::browser_listener::do_browser_changed(const browser_event & event)
 *
 * @param[in] event the <code>browser_event</code> to send.
 */


/**
 * @class openvrml::node_class_id
 *
 * @brief Identifier for <code>node_class</code>es.
 *
 * <code>node_class</code> identifiers take the following form:
 *
 * <pre>
 * absolute-uri ['#' proto-id ['#' proto-id [...]]]
 * </pre>
 *
 * A <code>node_class</code> identifier is basically like an absolute URI;
 * except the fragment identifier syntax has been extended to support referring
 * to nested <code>PROTO</code>s.
 *
 * For example, supposing the following %VRML world resides at
 * <code>%http://example.com/example.wrl</code>:
 *
 * <pre>
 * #VRML V2.0 utf8
 *
 * PROTO Outer [] {
 *   PROTO Inner [] { Group {} }
 *   Group {}
 * }
 * </pre>
 *
 * The <code>node_class_id</code> string for <code>Outer</code> would be
 * <code>%http://example.com/example.wrl#Outer</code>; and for
 * <code>Inner</code>,
 * <code>%http://example.com/example.wrl#Outer#Inner</code>.
 */

/**
 * @internal
 *
 * @var std::string openvrml::node_class_id::id_
 *
 * @brief The identifier string.
 */

namespace {

    struct OPENVRML_LOCAL node_class_id_grammar :
        public boost::spirit::grammar<node_class_id_grammar> {

        template <typename ScannerT>
        struct definition {
            typedef boost::spirit::rule<ScannerT> rule_type;

            rule_type node_class_id;
            absolute_uri_grammar<> absolute_uri;
            uric_grammar uric;

            definition(const node_class_id_grammar & self);

            const boost::spirit::rule<ScannerT> & start() const;
        };
    };

    template <typename ScannerT>
    node_class_id_grammar::definition<ScannerT>::
    definition(const node_class_id_grammar &)
    {
        node_class_id
            =   absolute_uri >> *('#' >> *uric)
            ;
    }

    template <typename ScannerT>
    const boost::spirit::rule<ScannerT> &
    node_class_id_grammar::definition<ScannerT>::start() const
    {
        return this->node_class_id;
    }
}

/**
 * @brief Construct from a <code>const&nbsp;char&nbsp;*</code>.
 *
 * @param[in] id    the identifier.
 *
 * @exception std::invalid_argument if @p id is not a valid
 *                                  <code>node_class</code> identifier.
 * @exception std::bad_alloc        if memory allocation fails.
 *
 * @todo Need to make sure the fragment part is valid.
 */
openvrml::node_class_id::node_class_id(const char * id)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc):
    id_(id)
{
    using namespace boost::spirit;

    node_class_id_grammar g;
    if (!parse(this->id_.begin(), this->id_.end(), g, space_p).full) {
        throw std::invalid_argument('<' + this->id_ + "> is not a valid "
                                    "node_class identifier");
    }
}

/**
 * @brief Construct from a <code>std::string</code>.
 *
 * @param[in] id    the identifier.
 *
 * @exception std::invalid_argument if @p id is not a valid
 *                                  <code>node_class</code> identifier.
 * @exception std::bad_alloc        if memory allocation fails.
 *
 * @todo Need to make sure the fragment part is valid.
 */
openvrml::node_class_id::node_class_id(const std::string & id)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc):
    id_(id)
{
    using namespace boost::spirit;

    node_class_id_grammar g;
    if (!parse(this->id_.begin(), this->id_.end(), g, space_p).full) {
        throw std::invalid_argument('<' + this->id_ + "> is not a valid "
                                    "node_class identifier");
    }
}

/**
 * @brief Convert to a <code>std::string</code>.
 *
 * @return the <code>node_class</code> identifier as a
 *         <code>std::string</code>.
 */
openvrml::node_class_id::operator std::string() const
{
    return this->id_;
}


/**
 * @class openvrml::browser
 *
 * @brief Encapsulates a VRML browser.
 *
 * <code>browser</code> is the foundation of the OpenVRML runtime. Users need
 * to inherit this class and override <code>browser::do_get_resource</code> and
 * provide an implementation of <code>resource_istream</code>.
 */

/**
 * @example sdl_viewer.cpp
 *
 * Basic example using openvrml::browser.
 */

/**
 * @var openvrml::browser::Vrml97Parser
 *
 * @brief VRML97 parser generated by ANTLR.
 */

/**
 * @var openvrml::browser::X3DVrmlParser
 *
 * @brief X3D VRML parser generated by ANTLR.
 */

/**
 * @internal
 *
 * @class openvrml::browser::node_class_map
 *
 * @brief The map of <code>node_class</code>es.
 */

/**
 * @var boost::mutex openvrml::browser::node_class_map::mutex_
 *
 * @brief Object mutex.
 */

/**
 * @typedef openvrml::browser::node_class_map::map_t
 *
 * @brief Map type.
 */

/**
 * @var openvrml::browser::node_class_map::map_t openvrml::browser::node_class_map::map_
 *
 * @brief Map.
 */

/**
 * @brief Construct.
 */
openvrml::browser::node_class_map::node_class_map()
{}

# ifndef NDEBUG
namespace {
    struct OPENVRML_LOCAL node_class_equals_ :
        public std::unary_function<std::pair<std::string,
                                             boost::shared_ptr<openvrml::node_class> >,
                                   bool> {
        explicit node_class_equals_(
            const boost::shared_ptr<openvrml::node_class> & node_class):
            node_class_(node_class)
        {}

        bool operator()(const argument_type & arg) const
        {
            return arg.second == this->node_class_;
        }

    private:
        boost::shared_ptr<openvrml::node_class> node_class_;
    };
}
# endif

/**
 * @brief Destroy.
 */
openvrml::browser::node_class_map::~node_class_map() OPENVRML_NOTHROW
{
# ifndef NDEBUG
    for (map_t::const_iterator entry = this->map_.begin();
         entry != this->map_.end();
         ++entry) {
        typedef std::iterator_traits<map_t::iterator>::difference_type
            difference_type;
        const difference_type count =
            std::count_if(this->map_.begin(),
                          this->map_.end(),
                          node_class_equals_(entry->second));
        assert(entry->second.use_count() == count
               && "shared_ptr<node_class> use_count does not match the number of entries in the browser's node_class_map");
    }
# endif
}

/**
 * @fn openvrml::browser::node_class_map::node_class_map(const node_class_map &)
 *
 * @brief Not implemented.
 */

/**
 * @brief Assign.
 *
 * @param[in] ncm   the value to assign.
 */
openvrml::browser::node_class_map &
openvrml::browser::node_class_map::operator=(const node_class_map & ncm)
{
    boost::mutex::scoped_lock my_lock(this->mutex_), map_lock(ncm.mutex_);
    map_t temp(ncm.map_);
    swap(this->map_, temp);
    return *this;
}

namespace {
    typedef std::map<std::string, boost::shared_ptr<openvrml::node_class> >
        node_class_map_t;

    struct OPENVRML_LOCAL init_node_class :
        std::unary_function<void, node_class_map_t::value_type>
    {
        init_node_class(openvrml::viewpoint_node * initial_viewpoint,
                        const double time)
            OPENVRML_NOTHROW:
            initial_viewpoint_(initial_viewpoint),
            time_(time)
        {}

        void operator()(const node_class_map_t::value_type & value) const
            OPENVRML_NOTHROW
        {
            assert(value.second);
            value.second->initialize(this->initial_viewpoint_, this->time_);
        }

    private:
        openvrml::viewpoint_node * initial_viewpoint_;
        double time_;
    };
}

/**
 * @brief Initialize the <code>node_class</code>es.
 *
 * @param[in] initial_viewpoint the viewpoint_node that should be initially active.
 * @param[in] timestamp         the current time.
 */
void
openvrml::browser::node_class_map::init(viewpoint_node * initial_viewpoint,
                                        const double timestamp)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    for_each(this->map_.begin(), this->map_.end(),
             init_node_class(initial_viewpoint, timestamp));
}

/**
 * @brief Insert a <code>node_class</code>.
 *
 * This operation will "fail" silently. That is, if a <code>node_class</code>
 * corresponding to @p id already exists in the map, the existing element will
 * simply be returned.
 *
 * @param[in] id            the implementation identifier.
 * @param[in] node_class    a <code>node_class</code>.
 *
 * @return the element in the node_class_map corresponding to @p id.
 */
const boost::shared_ptr<openvrml::node_class>
openvrml::browser::node_class_map::
insert(const std::string & id,
       const boost::shared_ptr<openvrml::node_class> & node_class)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    return this->map_.insert(make_pair(id, node_class)).first->second;
}

/**
 * @brief Remove a <code>node_class</code>.
 *
 * @param[in] id    the implementation identifier.
 *
 * @return @c true if a <code>node_class</code> is removed; @c false otherwise.
 */
bool openvrml::browser::node_class_map::remove(const std::string & id)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    return this->map_.erase(id) > 0;
}

/**
 * @brief Find a <code>node_class</code>.
 *
 * @param[in] id    an implementation id.
 *
 * @return the <code>node_class</code> corresponding to @p id, or a null
 *         pointer if no such <code>node_class</code> exists in the map.
 */
const boost::shared_ptr<openvrml::node_class>
openvrml::browser::node_class_map::find(const std::string & id) const
{
    const map_t::const_iterator pos = this->map_.find(id);
    return (pos != this->map_.end())
        ? pos->second
        : boost::shared_ptr<openvrml::node_class>();
}

namespace {

    struct OPENVRML_LOCAL render_node_class :
            std::unary_function<void, node_class_map_t::value_type> {
        explicit render_node_class(openvrml::viewer & viewer):
            viewer(&viewer)
        {}

        void operator()(const node_class_map_t::value_type & value) const
        {
            value.second->render(*this->viewer);
        }

    private:
        openvrml::viewer * viewer;
    };
}

/**
 * @brief Render the <code>node_class</code>es.
 *
 * @param[in,out] v a viewer.
 */
void openvrml::browser::node_class_map::render(openvrml::viewer & v)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    for_each(this->map_.begin(), this->map_.end(), render_node_class(v));
}

namespace {

    struct OPENVRML_LOCAL shutdown_node_class :
            std::unary_function<void, node_class_map_t::value_type> {
        explicit shutdown_node_class(const double timestamp):
            timestamp_(timestamp)
        {}

        void operator()(const node_class_map_t::value_type & value) const
        {
            value.second->shutdown(this->timestamp_);
        }

    private:
        double timestamp_;
    };
}

/**
 * @brief Shut down the <code>node_class</code>es.
 *
 * @param[in] timestamp         the current time.
 */
void
openvrml::browser::node_class_map::shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    boost::mutex::scoped_lock lock(this->mutex_);
    for_each(this->map_.begin(), this->map_.end(),
             shutdown_node_class(timestamp));
}

/**
 * @internal
 *
 * @var boost::recursive_mutex openvrml::browser::mutex_
 *
 * @brief Object mutex.
 */

/**
 * @internal
 *
 * @var std::auto_ptr<openvrml::null_node_class> openvrml::browser::null_node_class_
 *
 * @brief "Null" class object for default nodes (e.g., default_viewpoint).
 */

/**
 * @internal
 *
 * @var std::auto_ptr<openvrml::null_node_type> openvrml::browser::null_node_type_
 *
 * @brief "Null" type object for default nodes (e.g., default_viewpoint).
 */

/**
 * @internal
 *
 * @var openvrml::browser::node_class_map openvrml::browser::node_class_map_
 *
 * @brief A map of URIs to node implementations.
 */

/**
 * @internal
 *
 * @var openvrml::script_node_class openvrml::browser::script_node_class_
 *
 * @brief <code>node_class</code> for Script nodes in the browser.
 */

/**
 * @internal
 *
 * @var openvrml::scene * openvrml::browser::scene_
 *
 * @brief Pointer to the root scene.
 */

/**
 * @internal
 *
 * @var boost::intrusive_ptr<openvrml::node> openvrml::browser::default_viewpoint_
 *
 * @brief The "default" viewpoint_node used when no viewpoint_node in the scene
 *        is bound.
 */

/**
 * @internal
 *
 * @var openvrml::viewpoint_node * openvrml::browser::active_viewpoint_
 *
 * @brief The currently "active" viewpoint_node.
 */

/**
 * @internal
 *
 * @var boost::intrusive_ptr<openvrml::node> openvrml::browser::default_navigation_info_
 *
 * @brief The "default" navigation_info_node used when no navigation_info_node
 *        in the scene is bound.
 */

/**
 * @internal
 *
 * @var openvrml::navigation_info_node * openvrml::browser::active_navigation_info_
 *
 * @brief The currently "active" navigation_info_node.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::viewpoint_node *> openvrml::browser::viewpoint_list
 *
 * @brief A list of all the Viewpoint nodes in the browser.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::node *> openvrml::browser::scoped_lights
 *
 * @brief A list of all the scoped light nodes in the browser.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::script_node *> openvrml::browser::scripts
 *
 * @brief A list of all the Script nodes in the browser.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::time_dependent_node *> openvrml::browser::timers
 *
 * @brief A list of all the TimeSensor nodes in the browser.
 */

/**
 * @internal
 *
 * @var std::set<openvrml::browser_listener *> openvrml::browser::listeners_
 *
 * @brief The set of <code>browser_listener</code>s that will receive
 *        <code>browser_event</code>s.
 *
 * @sa openvrml::browser::add_listener
 * @sa openvrml::browser::remove_listener
 */

/**
 * @var bool openvrml::browser::modified_
 *
 * @brief Flag to indicate whether the browser has been modified.
 */

/**
 * @var boost::mutex openvrml::browser::modified_mutex_
 *
 * @brief Mutex protecting <code>browser::modified_</code>.
 */

/**
 * @var bool openvrml::browser::new_view
 *
 * @brief Flag to indicate if the user has changed to a new view.
 */

/**
 * @var double openvrml::browser::delta_time
 *
 * @brief Time elapsed since the last update.
 */

/**
 * @var openvrml::openvrml::viewer * openvrml::browser::viewer_
 *
 * @brief The current <code>viewer</code>.
 */

/**
 * @var double openvrml::browser::frame_rate_
 *
 * @brief Frame rate.
 */

/**
 * @brief Get the current time.
 */
double openvrml::browser::current_time() OPENVRML_NOTHROW
{
# ifdef _WIN32
    _timeb timebuffer;
#   if defined(_MSC_VER) && (_MSC_VER < 1400)
    _ftime(&timebuffer);
#   else
    const errno_t err = _ftime_s(&timebuffer);
    assert(err == 0);
#   endif
    return double(timebuffer.time) + 1.0e-3 * double(timebuffer.millitm);
# else
    timeval tv;
    const int result = gettimeofday(&tv, 0);
    assert(result == 0);

    return double(tv.tv_sec) + 1.0e-6 * double(tv.tv_usec);
# endif
}

/**
 * @var boost::mutex openvrml::browser::out_mutex_
 *
 * @brief Mutex guarding @a out_.
 */

/**
 * @var std::ostream & openvrml::browser::out_
 *
 * @brief Output stream, generally for console output.
 */

/**
 * @var boost::mutex openvrml::browser::err_mutex_
 *
 * @brief Mutex guarding @a err_.
 */

/**
 * @var std::ostream & openvrml::browser::err_
 *
 * @brief Error output stream.
 */

/**
 * @var bool openvrml::browser::flags_need_updating
 *
 * @brief Set by node::bounding_volume_dirty on any node in this browser graph,
 *      cleared by update_flags.
 *
 * @c true if the bvolume dirty flag has been set on a node in the
 * browser graph, but has not yet been propegated to that node's
 * ancestors.
 */

namespace {
    void OPENVRML_LOCAL register_node_classes(openvrml::browser & b)
    {
        register_core_node_classes(b);
        register_vrml97_node_classes(b);
        register_networking_node_classes(b);
        register_rendering_node_classes(b);
        register_shape_node_classes(b);
        register_geometry2d_node_classes(b);
        register_texturing_node_classes(b);
        register_key_device_sensor_node_classes(b);
        register_event_utilities_node_classes(b);
    }
}

/**
 * @brief Constructor.
 *
 * @param[in] out   output stream for console output.
 * @param[in] err   output stream for error console output.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::browser::browser(std::ostream & out, std::ostream & err)
    OPENVRML_THROW1(std::bad_alloc):
    null_node_class_(new null_node_class(*this)),
    null_node_type_(new null_node_type(*null_node_class_)),
    script_node_class_(*this),
    scene_(new scene(*this)),
    default_viewpoint_(new default_viewpoint(*null_node_type_)),
    active_viewpoint_(node_cast<viewpoint_node *>(default_viewpoint_.get())),
    default_navigation_info_(new default_navigation_info(*null_node_type_)),
    active_navigation_info_(
        node_cast<navigation_info_node *>(default_navigation_info_.get())),
    new_view(false),
    delta_time(DEFAULT_DELTA),
    viewer_(0),
    modified_(false),
    out_(out),
    err_(err),
    frame_rate_(0.0),
    flags_need_updating(false)
{
    assert(this->active_viewpoint_);
    assert(this->active_navigation_info_);
    register_node_classes(*this);
}

/**
 * @brief Destructor.
 */
openvrml::browser::~browser() OPENVRML_NOTHROW
{
    const double now = browser::current_time();

    if (this->scene_) { this->scene_->shutdown(now); }

    this->node_class_map_.shutdown(now);
    assert(this->viewpoint_list.empty());
    assert(this->scoped_lights.empty());
    assert(this->scripts.empty());
    assert(this->timers.empty());
}

/**
 * @brief Add a <code>node_class</code>.
 *
 * If a <code>node_class</code> identified by @p id has already been added to
 * the browser, it will be replaced.
 *
 * @warning If <code>std::bad_alloc</code> is thrown here, the
 *          <code>browser</code>'s <code>node_class</code> map is left in an
 *          unknown state. In all likelihood any preexisting entry in the map
 *          with the same implementation identifier as @p id will have been
 *          removed.
 *
 * @param[in] id    a <code>node_class</code> identifier.
 * @param[in] nc    a <code>shared_ptr</code> to a <code>node_class</code>
 *
 * @exception std::invalid_argument if @p nc is null.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void
openvrml::browser::
add_node_class(const node_class_id & id,
               const boost::shared_ptr<openvrml::node_class> & nc)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    if (!nc) {
        throw std::invalid_argument("cannot add null node_class pointer");
    }
    this->node_class_map_.remove(id); // Remove any existing entry.
    this->node_class_map_.insert(id, nc);
}

/**
 * @brief Get the <code>node_class</code> corresponding to @p id.
 *
 * @param[in] id    a <code>node_class</code> identifier.
 *
 * @return the <code>node_class</code> corresponding to @p id; or a null
 *         pointer if no such <code>node_class</code> exists.
 */
const boost::shared_ptr<openvrml::node_class>
openvrml::browser::node_class(const node_class_id & id) const OPENVRML_NOTHROW
{
    return this->node_class_map_.find(id);
}

/**
 * @brief Get the root scene.
 *
 * @return the root scene, or 0 if no scene is loaded.
 */
openvrml::scene * openvrml::browser::root_scene() const OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return this->scene_.get();
}

/**
 * @brief Get the path to a node in the scene graph.
 *
 * @param[in] n  the objective node.
 *
 * @return the path to @p node, starting with a root node, and ending with
 *         @p node. If @p node is not in the scene graph, the returned
 *         node_path is empty.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const openvrml::node_path openvrml::browser::find_node(const node & n) const
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(this->scene_);

    class FindNodeTraverser : public node_traverser {
        const node & objectiveNode;
        node_path & nodePath;

    public:
        FindNodeTraverser(const node & objectiveNode, node_path & nodePath)
            OPENVRML_NOTHROW:
            objectiveNode(objectiveNode),
            nodePath(nodePath)
        {}

        virtual ~FindNodeTraverser() throw()
        {}

    private:
        virtual void on_entering(node & n) OPENVRML_THROW1(std::bad_alloc)
        {
            this->nodePath.push_back(&n);
            if (&n == &this->objectiveNode) { this->halt_traversal(); }
        }

        virtual void on_leaving(node &) OPENVRML_NOTHROW
        {
            if (!this->halted()) { this->nodePath.pop_back(); }
        }
    };

    node_path nodePath;
    FindNodeTraverser(n, nodePath).traverse(this->scene_->nodes());
    return nodePath;
}

/**
 * @brief Get the active viewpoint_node.
 *
 * The active viewpoint_node is the one currently associated with the user
 * view.
 *
 * @return the active viewpoint_node.
 */
openvrml::viewpoint_node & openvrml::browser::active_viewpoint() const
    OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return *this->active_viewpoint_;
}

/**
 * @brief Set the active viewpoint_node.
 *
 * @param[in] viewpoint a viewpoint_node.
 */
void openvrml::browser::active_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    this->active_viewpoint_ = &viewpoint;
}

/**
 * @brief Reset the active viewpoint_node to the default.
 */
void openvrml::browser::reset_default_viewpoint() OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(this->default_viewpoint_);
    this->active_viewpoint_ =
        node_cast<viewpoint_node *>(this->default_viewpoint_.get());
    assert(this->active_viewpoint_);
}

/**
 * @brief Get the active navigation_info_node.
 *
 * The active navigation_info_node is the one currently associated with the
 * user view.
 *
 * @return the active navigation_info_node.
 */
openvrml::navigation_info_node &
openvrml::browser::active_navigation_info() const OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return *this->active_navigation_info_;
}

/**
 * @brief Set the active navigation_info_node.
 *
 * @param[in] nav_info a navigation_info_node.
 */
void openvrml::browser::active_navigation_info(navigation_info_node & nav_info)
    OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    this->active_navigation_info_ = &nav_info;
}

/**
 * @brief Reset the active navigation_info_node to the default.
 */
void openvrml::browser::reset_default_navigation_info() OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(this->default_navigation_info_);
    this->active_navigation_info_ =
        node_cast<navigation_info_node *>(
            this->default_navigation_info_.get());
    assert(this->active_navigation_info_);
}

/**
 * @brief Add a viewpoint_node to the list of viewpoint_nodes for the browser.
 *
 * @param[in] viewpoint a viewpoint_node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p viewpoint is not in the list of viewpoint_nodes for the browser.
 */
void openvrml::browser::add_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(std::find(this->viewpoint_list.begin(), this->viewpoint_list.end(),
                     &viewpoint) == this->viewpoint_list.end());
    this->viewpoint_list.push_back(&viewpoint);
}

/**
 * @brief Remove a viewpoint_node from the list of viewpoint_nodes for the
 *      browser.
 *
 * @param[in] viewpoint a viewpoint_node.
 *
 * @pre @p viewpoint is in the list of viewpoint_nodes for the browser.
 */
void openvrml::browser::remove_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(!this->viewpoint_list.empty());
    typedef std::list<viewpoint_node *> viewpoint_list_t;
    const viewpoint_list_t::iterator end = this->viewpoint_list.end();
    const viewpoint_list_t::iterator pos =
            std::find(this->viewpoint_list.begin(), end, &viewpoint);
    assert(pos != end);
    this->viewpoint_list.erase(pos);
}

/**
 * @brief Get the list of @link viewpoint_node viewpoint_nodes@endlink for the
 *      world.
 *
 * @return the list of @link viewpoint_node viewpoint_nodes@endlink for the
 *      world.
 */
const std::list<openvrml::viewpoint_node *> &
openvrml::browser::viewpoints() const OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return this->viewpoint_list;
}

/**
 * @brief Set the current <code>viewer</code>.
 *
 * @param[in] v <code>viewer</code>.
 *
 * @exception viewer_in_use if @p v is already associated with a
 *                          <code>browser</code>.
 */
void openvrml::browser::viewer(openvrml::viewer * v)
    OPENVRML_THROW1(viewer_in_use)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    if (v && v->browser_) { throw viewer_in_use(); }
    if (this->viewer_) { this->viewer_->browser_ = 0; }
    this->viewer_ = v;
    if (v) { v->browser_ = this; }
}

/**
 * @brief The current <code>viewer</code>.
 *
 * @return the current <code>viewer</code>.
 */
openvrml::viewer * openvrml::browser::viewer() const OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return this->viewer_;
}

/**
 * @brief Fetch a network resource.
 *
 * @param[in] uri   a Uniform Resource Identifier.
 *
 * @return the requested resource as a stream.
 */
std::auto_ptr<openvrml::resource_istream>
openvrml::browser::get_resource(const std::string & uri)
{
    return this->do_get_resource(uri);
}

/**
 * @fn std::auto_ptr<openvrml::resource_istream> openvrml::browser::do_get_resource(const std::string & uri)
 *
 * @brief Fetch a network resource.
 *
 * Called by <code>browser::get_resource</code>, clients of %OpenVRML are
 * required to provide an implementation for this function. %OpenVRML depends
 * on the implementation of this function for all of its input needs. As such,
 * what kind of resources %OpenVRML is capable of resolving is entirely
 * dependent on code provided by the application. A trivial implementation
 * designed to handle only <code>file</code> resources can use
 * <code>std::filebuf</code>:
 *
 * @code
 * std::auto_ptr<openvrml::resource_istream>
 * my_browser::do_get_resource(const std::string & uri)
 * {
 *     using std::auto_ptr;
 *     using std::invalid_argument;
 *     using std::string;
 *     using openvrml::resource_istream;
 *
 *     class file_resource_istream : public resource_istream {
 *         std::string url_;
 *         std::filebuf buf_;
 *
 *     public:
 *         explicit file_resource_istream(const std::string & path):
 *             resource_istream(&this->buf_)
 *         {
 *             //
 *             // Note that the failbit is set in the constructor if no data
 *             // can be read from the stream.  This is important.  If the
 *             // failbit is not set on such a stream, OpenVRML will attempt
 *             // to read data from a stream that cannot provide it.
 *             //
 *             if (!this->buf_.open(path.c_str(), ios_base::in)) {
 *                 this->setstate(ios_base::failbit);
 *             }
 *         }
 *
 *         void url(const std::string & str)
 *         {
 *             this->url_ = str;
 *         }
 *
 *     private:
 *         virtual const std::string url() const
 *         {
 *             return this->url_;
 *         }
 *
 *         virtual const std::string type() const
 *         {
 *             //
 *             // A real application should use OS facilities for this;
 *             // however, that is beyond the scope of this example (which
 *             // is intended to be portable and stupid).
 *             //
 *             using std::find;
 *             using std::string;
 *             using boost::algorithm::iequals;
 *             using boost::next;
 *             string media_type = "application/octet-stream";
 *             const string::const_reverse_iterator dot_pos =
 *                 find(this->url_.rbegin(), this->url_.rend(), '.');
 *             if (dot_pos == this->url_.rend()
 *                 || next(dot_pos.base()) == this->url_.end()) {
 *                 return media_type;
 *             }
 *             const string::const_iterator hash_pos =
 *                 find(next(dot_pos.base()), this->url_.end(), '#');
 *             const string ext(dot_pos.base(), hash_pos);
 *             if (iequals(ext, "wrl")) {
 *                 media_type = "model/vrml";
 *             } else if (iequals(ext, "png")) {
 *                 media_type = "image/png";
 *             } else if (iequals(ext, "jpg") || iequals(ext, "jpeg")) {
 *                 media_type = "image/jpeg";
 *             }
 *             return media_type;
 *         }
 *
 *         virtual bool data_available() const
 *         {
 *             return !!(*this);
 *         }
 *     };
 *
 *     const string scheme = uri.substr(0, uri.find_first_of(':'));
 *     if (scheme != "file") {
 *         throw invalid_argument('\"' + scheme + "\" URI scheme not "
 *                                "supported");
 *     }
 *     //
 *     // file://
 *     //        ^
 *     // 01234567
 *     //
 *     string path = uri.substr(uri.find_first_of('/', 7));
 *
 *     auto_ptr<resource_istream> in(new file_resource_istream(path));
 *     static_cast<file_resource_istream *>(in.get())->url(uri);
 *
 *     return in;
 * }
 * @endcode
 *
 * The @p uri parameter is provided by %OpenVRML and can be assumed to be an
 * absolute URI. As such, it will always have a scheme through which the client
 * code can choose a resolution mechanism. For more information on URI syntax,
 * see <a href="http://www.isi.edu/in-notes/rfc2396.txt">Internet
 * RFC&nbsp;2396</a>.
 *
 * @param[in] uri   an absolute Uniform Resource Identifier.
 *
 * @return the requested resource as a stream.
 *
 * @sa http://www.isi.edu/in-notes/rfc2396.txt
 */

/**
 * @brief Get the browser name.
 *
 * @return "OpenVRML"
 *
 * Specific browsers may wish to override this method.
 */
const char * openvrml::browser::name() const OPENVRML_NOTHROW
{
    return "OpenVRML";
}

/**
 * @brief Get the browser version.
 *
 * @return the version of openvrml.
 *
 * Specific browsers may wish to override this method.
 */
const char * openvrml::browser::version() const OPENVRML_NOTHROW
{
    return PACKAGE_VERSION;
}

/**
 * @brief Get the average navigation speed in meters per second.
 *
 * @return the average navigation speed.
 */
float openvrml::browser::current_speed()
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    navigation_info_node & nav_info = this->active_navigation_info();
    return nav_info.speed();
}

/**
 * @brief Get the URI for the world.
 *
 * @return the URI for the world.
 */
const std::string openvrml::browser::world_url() const
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(this->scene_);
    return this->scene_->url(); // Throws std::bad_alloc.
}

/**
 * @brief Set the world from a stream.
 *
 * @param[in,out] in    an input stream.
 *
 * @exception bad_media_type    if @p in.type() is not "model/vrml",
 *                              "x-world/x-vrml", or "model/x3d+vrml".
 * @excpetion invalid_vrml      if @p in has invalid syntax.
 */
void openvrml::browser::set_world(resource_istream & in)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);

    using std::for_each;
    using std::string;

    //
    // Clear out the current scene.
    //
    double now = browser::current_time();
    if (this->scene_) { this->scene_->shutdown(now); }
    this->node_class_map_.shutdown(now);
    for_each(this->listeners_.begin(), this->listeners_.end(),
             boost::bind2nd(boost::mem_fun(&browser_listener::browser_changed),
                            browser_event(*this, browser_event::shutdown)));
    this->scene_.reset();
    this->active_viewpoint_ =
        node_cast<viewpoint_node *>(this->default_viewpoint_.get());
    assert(this->viewpoint_list.empty());
    assert(this->scoped_lights.empty());
    assert(this->scripts.empty());
    assert(this->timers.empty());

    //
    // Create the new scene.
    //
    node_class_map new_map;
    this->node_class_map_ = new_map;
    register_node_classes(*this);
    this->scene_.reset(new scene(*this));
    this->scene_->load(in);

    //
    // Initialize.
    //
    now = browser::current_time();
    this->scene_->initialize(now);

    //
    // Get the initial viewpoint_node, if any was specified.
    //
    viewpoint_node * initial_viewpoint = 0;
    const string viewpoint_node_id = uri(this->scene_->url()).fragment();
    if (!viewpoint_node_id.empty()) {
        if (!this->scene_->nodes().empty()) {
            using boost::intrusive_ptr;
            const intrusive_ptr<node> & n = this->scene_->nodes().front();
            if (n) {
                node * const vp = n->scope().find_node(viewpoint_node_id);
                initial_viewpoint = dynamic_cast<viewpoint_node *>(vp);
            }
        }
    }

    //
    // Initialize the node_classes.
    //
    this->node_class_map_.init(initial_viewpoint, now);

    if (this->active_viewpoint_
        != node_cast<viewpoint_node *>(this->default_viewpoint_.get())) {
        // XXX
        // XXX Fix openvrml::viewpoint_node so that we don't have to get an
        // XXX event_listener here.
        // XXX
        event_listener & listener =
            this->active_viewpoint_->event_listener("set_bind");
        dynamic_cast<sfbool_listener &>(listener).process_event(sfbool(true),
                                                                now);
    }

    this->modified(true);
    this->new_view = true; // Force resetUserNav

    for_each(this->listeners_.begin(), this->listeners_.end(),
             boost::bind2nd(boost::mem_fun(&browser_listener::browser_changed),
                            browser_event(*this, browser_event::initialized)));
}

/**
 * @brief Replace the root nodes of the world.
 *
 * @param[in] nodes new root nodes for the world.
 */
void
openvrml::browser::replace_world(
    const std::vector<boost::intrusive_ptr<node> > & nodes)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    const double now = browser::current_time();
    this->scene_->nodes(nodes);
    this->scene_->initialize(now);
    //
    // Initialize the node_classes.
    //
    static viewpoint_node * const initial_viewpoint = 0;
    this->node_class_map_.init(initial_viewpoint, now);
    this->modified(true);
    this->new_view = true; // Force resetUserNav
}

struct OPENVRML_LOCAL openvrml::browser::root_scene_loader {
    root_scene_loader(browser & b, const std::vector<std::string> & url):
        browser_(&b),
        url_(&url)
    {}

    void operator()() const OPENVRML_NOTHROW
    try {
        boost::recursive_mutex::scoped_lock lock(this->browser_->mutex_);

        using std::endl;

        openvrml::browser & browser = *this->browser_;

        try {
            std::auto_ptr<resource_istream> in =
                browser.scene_->get_resource(*this->url_);
            if (!(*in)) { throw unreachable_url(); }
            browser.set_world(*in);
        } catch (antlr::ANTLRException & ex) {
            browser.err(ex.getMessage());
        } catch (unreachable_url &) {
            throw;
        } catch (std::exception & ex) {
            browser.err(ex.what());
        } catch (...) {
            throw unreachable_url();
        }
    } catch (unreachable_url & ex) {
        this->browser_->err(ex.what());
    }

private:
    browser * browser_;
    const std::vector<std::string> * url_;
};

/**
 * @brief Load a VRML world into the browser.
 *
 * @param[in] url       a URI.
 * @param[in] parameter parameters for @p url.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::browser::load_url(const std::vector<std::string> & url,
                                 const std::vector<std::string> &)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::function0<void> f = root_scene_loader(*this, url);
    boost::thread t(f);
}

/**
 * @brief Send a string to the user interface.
 *
 * The default implementation of this method simply prints @p description to
 * @a out.  Subclasses can override this method to direct messages to an
 * application's UI; for instance, a status bar.
 *
 * @param[in] description   a string.
 */
void openvrml::browser::description(const std::string & description)
{
    this->out(description);
}


//
// stream_id_index_ is used to construct the URI for the stream; this is used
// to identify any PROTOs in the stream in the browser's node_class map.  A
// side-effect of this approach is that the node_class map will keep growing,
// even if identical streams are repeatedly loaded.  For this reason it is
// preferable to use an EXTERNPROTO in the stream.
//
namespace {
    OPENVRML_LOCAL size_t stream_id_index_ = 0;
    OPENVRML_LOCAL boost::mutex stream_id_index_mutex_;
}

/**
 * @brief Generate nodes from a stream of VRML syntax.
 *
 * In addition to the exceptions listed, this method may throw any
 * exception that may result from reading the input stream.
 *
 * @param[in,out] in    an input stream.
 * @param[in]     type  MIME content type of @p in.
 *
 * @return the root nodes generated from @p in.
 *
 * @exception invalid_vrml          if @p in has invalid VRML syntax.
 * @exception std::invalid_argument if @p type refers to an unsupported content
 *                                  type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const std::vector<boost::intrusive_ptr<openvrml::node> >
openvrml::browser::create_vrml_from_stream(std::istream & in,
                                           const std::string & type)
{
    using std::string;
    using std::vector;
    using boost::lexical_cast;

    {
        boost::mutex::scoped_lock lock(stream_id_index_mutex_);
        ++stream_id_index_;
    }

    const string stream_id =
        "urn:X-openvrml:stream:" + lexical_cast<string>(stream_id_index_);

    std::vector<boost::intrusive_ptr<node> > nodes;
    try {
        assert(this->scene_);
        std::map<string, string> meta;
        parse_vrml(in, stream_id, type, *this->scene_, nodes, meta);
    } catch (openvrml::bad_media_type & ex) {
        throw std::invalid_argument(ex.what());
    }
    return nodes;
}

struct OPENVRML_LOCAL openvrml::browser::vrml_from_url_creator {
    vrml_from_url_creator(openvrml::browser & browser,
                          const std::vector<std::string> & url,
                          const boost::intrusive_ptr<node> & node,
                          const std::string & event)
        OPENVRML_THROW2(unsupported_interface, std::bad_cast):
        browser_(&browser),
        url_(&url),
        node_(node),
        listener_(&dynamic_cast<mfnode_listener &>(
                      node->event_listener(event)))
    {}

    void operator()() const OPENVRML_NOTHROW
    try {
        try {
            std::auto_ptr<resource_istream> in =
                this->browser_->scene_->get_resource(*this->url_);
            if (!(*in)) { throw unreachable_url(); }
            mfnode nodes;
            nodes.value(this->browser_->create_vrml_from_stream(*in, in->type()));
            this->listener_->process_event(nodes, browser::current_time());
        } catch (std::exception & ex) {
            this->browser_->err(ex.what());
            throw unreachable_url();
        } catch (...) {
            //
            // The implementation of resource_istream is provided by the user;
            // and unfortunately, operations on it could throw anything.
            //
            throw unreachable_url();
        }
    } catch (std::exception & ex) {
        this->browser_->err(ex.what());
    }

private:
    openvrml::browser * const browser_;
    const std::vector<std::string> * const url_;
    const boost::intrusive_ptr<node> node_;
    mfnode_listener * const listener_;
};

/**
 * @brief Create nodes from a URI.
 *
 * This function executes asynchronously. When the nodes have been completely
 * loaded, they are sent to the @p event MFNode eventIn of @p node.
 *
 * @param[in] url       an alternative URI list.
 * @param[in] node      the node to which the nodes loaded from @p url should be
 *                  sent as an event.
 * @param[in] event     the event of @p node to which the new nodes will be sent.
 *
 * @exception unsupported_interface         if @p node has no eventIn @p event.
 * @exception std::bad_cast                 if the @p event eventIn of @p node
 *                                          is not an MFNode.
 * @exception boost::thread_resource_error  if thread creation fails.
 */
void
openvrml::browser::
create_vrml_from_url(const std::vector<std::string> & url,
                     const boost::intrusive_ptr<node> & node,
                     const std::string & event)
    OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                    boost::thread_resource_error)
{
    boost::function0<void> f = vrml_from_url_creator(*this, url, node, event);
    boost::thread t(f);
}

/**
 * @brief Add a listener for <code>browser_event</code>s.
 *
 * @param[in] listener  a <code>browser_listener</code>.
 *
 * @return @c true if @p listener is added successfully; @c false otherwise (if
 *         @p listener is already listening for events from the
 *         <code>browser</code>).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
bool openvrml::browser::add_listener(browser_listener & listener)
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->listeners_.insert(&listener).second;
}

/**
 * @brief Remove a listener for <code>browser_event</code>s.
 *
 * @param[in] listener  a <code>browser_listener</code>.
 *
 * @return @c true if @p listener is removed successfully; @c false otherwise
 *         (if @p listener is not listening for events from the
 *         <code>browser</code>).
 */
bool openvrml::browser::remove_listener(browser_listener & listener)
    OPENVRML_NOTHROW
{
    return this->listeners_.erase(&listener) > 0;
}

/**
 * @brief Get the current frame rate.
 *
 * @return the current frame rate.
 */
double openvrml::browser::frame_rate() const
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return this->frame_rate_;
}

/**
 * Called by the viewer when the cursor passes over, clicks, drags, or
 * releases a sensitive object (an Anchor or another grouping node with
 * an enabled TouchSensor child).
 */
void openvrml::browser::sensitive_event(node * const n,
                                        const double timestamp,
                                        const bool is_over,
                                        const bool is_active,
                                        const double (&point)[3])
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    if (n) {
        if (pointing_device_sensor_node * pointing_device_sensor =
            node_cast<pointing_device_sensor_node *>(n)) {
            pointing_device_sensor->activate(timestamp,
                                             is_over,
                                             is_active,
                                             point);
        } else if (grouping_node * g = node_cast<grouping_node *>(n)) {
            //
            // The parent grouping node is registered for Touch/Drag Sensors.
            //
            g->activate_pointing_device_sensors(timestamp,
                                                is_over,
                                                is_active,
                                                point);
            this->modified(true);
        }
    }
}

/**
 * @brief Process events (update the browser).
 *
 * This method should be called after each frame is rendered.
 *
 * @return @c true if the browser needs to be rerendered, @c false otherwise.
 */
bool openvrml::browser::update(double current_time)
{
    using std::for_each;

    boost::recursive_mutex::scoped_lock lock(this->mutex_);

    if (current_time <= 0.0) { current_time = browser::current_time(); }

    this->delta_time = DEFAULT_DELTA;

    //
    // Update each of the timers.
    //
    for_each(this->timers.begin(), this->timers.end(),
             boost::bind2nd(boost::mem_fun(&time_dependent_node::update),
                            current_time));

    //
    // Update each of the scripts.
    //
    for_each(this->scripts.begin(), this->scripts.end(),
             boost::bind2nd(boost::mem_fun(&script_node::update),
                            current_time));

    // Signal a redisplay if necessary
    return this->modified();
}

/**
 * @brief Indicate whether the headlight is on.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */
bool openvrml::browser::headlight_on()
{
    navigation_info_node & nav_info = this->active_navigation_info();
    return nav_info.headlight();
}

/**
 * @brief Draw this browser into the specified viewer
 */
void openvrml::browser::render()
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    if (!this->viewer_) { return; }

    //
    // Per-node_class rendering happens before viewer::set_viewpoint is called
    // This is important for things like background rendering, since
    // viewer::insert_background must be called before viewer::set_viewpoint.
    //
    this->node_class_map_.render(*this->viewer_);

    if (this->new_view) {
        this->viewer_->reset_user_navigation();
        this->new_view = false;
    }
    navigation_info_node & nav_info = this->active_navigation_info();
    const float avatarSize = nav_info.avatar_size().empty()
        ? 0.25f
        : nav_info.avatar_size()[0];
    const float visibilityLimit = nav_info.visibility_limit();

    // Activate the headlight.
    // ambient is supposed to be 0 according to the spec...
    if (this->headlight_on()) {
        static const color color(1.0, 1.0, 1.0);
        static const vec3f direction(0.0, 0.0, -1.0);
        static const float ambientIntensity = 0.3f;
        static const float intensity = 1.0;

        this->viewer_->insert_dir_light(ambientIntensity,
                                        intensity,
                                        color,
                                        direction);
    }

    // sets the viewpoint transformation
    //
    const mat4f t = this->active_viewpoint_->user_view_transform()
                    * this->active_viewpoint_->transformation();
    vec3f position, scale;
    rotation orientation;
    t.transformation(position, orientation, scale);
    this->viewer_->set_viewpoint(position,
                                 orientation,
                                 this->active_viewpoint_->field_of_view(),
                                 avatarSize,
                                 visibilityLimit);

    // Top level object

    this->viewer_->begin_object(0);
    mat4f modelview = t.inverse();
    rendering_context rc(bounding_volume::partial, modelview);
    rc.draw_bounding_spheres = true;

    // Do the browser-level lights (Points and Spots)
    std::for_each(this->scoped_lights.begin(), this->scoped_lights.end(),
                  boost::bind2nd(
                      boost::mem_fun(&scoped_light_node::render_scoped_light),
                      *this->viewer_));

    //
    // Render the nodes.  scene_ may be 0 if the world failed to load.
    //
    if (this->scene_) {
        this->scene_->render(*this->viewer_, rc);
    }

    this->viewer_->end_object();

    // This is actually one frame late...
    this->frame_rate_ = this->viewer_->frame_rate();

    this->modified(false);
}

/**
 * @brief Indicate whether rendering is necessary.
 *
 * @param[in] value @c true to indicate that the browser state has changed and
 *              rerendering is necessary; @c false once rendering has occurred.
 */
void openvrml::browser::modified(const bool value)
{
    boost::mutex::scoped_lock lock(this->modified_mutex_);
    this->modified_ = value;
}

/**
 * @brief Check if the browser has been modified.
 *
 * @return @c true if the browser has been modified, @c false otherwise.
 */
bool openvrml::browser::modified() const
{
    boost::mutex::scoped_lock lock(this->modified_mutex_);
    return this->modified_;
}

/**
 * @brief Set the time until the next update is needed.
 *
 * @param[in] d a time interval.
 */
void openvrml::browser::delta(const double d)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    if (d < this->delta_time) { this->delta_time = d; }
}

/**
 * @brief Get the time interval between browser updates.
 *
 * @return the time interval between browser updates.
 */
double openvrml::browser::delta() const
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return this->delta_time;
}

/**
 * @brief Add a scoped light node to the browser.
 *
 * @param[in] light a light node.
 *
 * @pre @p light is not in the list of light nodes for the browser.
 */
void
openvrml::browser::add_scoped_light(scoped_light_node & light)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(std::find(this->scoped_lights.begin(), this->scoped_lights.end(),
                     &light) == this->scoped_lights.end());
    this->scoped_lights.push_back(&light);
}

/**
 * @brief Remove a scoped light node from the browser.
 *
 * @param[in] light the light node to remove.
 *
 * @pre @p light is in the list of light nodes for the browser.
 */
void
openvrml::browser::
remove_scoped_light(scoped_light_node & light)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(!this->scoped_lights.empty());
    const std::list<scoped_light_node *>::iterator end =
        this->scoped_lights.end();
    const std::list<scoped_light_node *>::iterator pos =
            std::find(this->scoped_lights.begin(), end, &light);
    assert(pos != end);
    this->scoped_lights.erase(pos);
}

/**
 * @brief Add a Script node to the browser.
 *
 * @param[in] script    a Script node.
 *
 * @pre @p script is not in the list of Script nodes for the browser.
 */
void openvrml::browser::add_script(script_node & script)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(std::find(this->scripts.begin(), this->scripts.end(), &script)
            == this->scripts.end());
    this->scripts.push_back(&script);
}

/**
 * @brief Remove a Script node from the browser.
 *
 * @param[in] script    the Script node to remove.
 *
 * @pre @p script is in the list of Script nodes for the browser.
 */
void openvrml::browser::remove_script(script_node & script)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(!this->scripts.empty());
    typedef std::list<script_node *> script_node_list_t;
    const script_node_list_t::iterator end = this->scripts.end();
    const script_node_list_t::iterator pos =
            std::find(this->scripts.begin(), end, &script);
    assert(pos != end);
    this->scripts.erase(pos);
}

/**
 * @brief Add a time-dependent node to the browser.
 *
 * @param[in] n a <code>time_dependent_node</code>.
 *
 * @pre @p n is not in the list of <code>time_dependent_node</code>s for the
 *      browser.
 */
void openvrml::browser::add_time_dependent(time_dependent_node & n)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(std::find(this->timers.begin(), this->timers.end(), &n)
           == this->timers.end());
    this->timers.push_back(&n);
}

/**
 * @brief Remove a time-dependent node from the browser.
 *
 * @param[in] n the <code>time_dependent_node</code> to remove.
 *
 * @pre @p n is in the list of <code>time_dependent_node</code>s for the
 *      browser.
 */
void
openvrml::browser::remove_time_dependent(time_dependent_node & n)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    assert(!this->timers.empty());
    const std::list<time_dependent_node *>::iterator end = this->timers.end();
    const std::list<time_dependent_node *>::iterator pos =
            std::find(this->timers.begin(), end, &n);
    assert(pos != end);
    this->timers.erase(pos);
}

/**
 * @brief Propagate the bvolume dirty flag from children to ancestors.
 *
 * The invariant is that if a node's bounding volume is out of date,
 * then the bounding volumes of all that nodes's ancestors must be
 * out of date. However, Node does not maintain a parent pointer. So
 * we must do a traversal of the entire browser graph to do the propagation.
 *
 * @see node::setBVolumeDirty
 * @see node::isBVolumeDirty
 */
void openvrml::browser::update_flags()
{
//  Node* root = this->getRoot();
//  root->updateModified(0x002);
}

/**
 * @brief Print a message to the output stream.
 *
 * @param[in] str   a string.
 */
void openvrml::browser::out(const std::string & str)
{
    boost::mutex::scoped_lock(this->out_mutex_);
    this->out_ << str << std::endl;
}

/**
 * @brief Print a message to the error stream.
 *
 * @param[in] str   a string.
 */
void openvrml::browser::err(const std::string & str)
{
    boost::mutex::scoped_lock(this->err_mutex_);
    this->err_ << str << std::endl;
}


/**
 * @class openvrml::bad_url
 *
 * @brief Thrown when there is a problem resolving a URI.
 */

/**
 * @brief Construct.
 *
 * @param[in] message   Informative text.
 */
openvrml::bad_url::bad_url(const std::string & message):
    std::runtime_error(message)
{}

/**
 * @brief Destroy.
 */
openvrml::bad_url::~bad_url() throw ()
{}


/**
 * @class openvrml::invalid_url
 *
 * @brief Thrown when parsing a URI fails.
 */

/**
 * @brief Construct.
 */
openvrml::invalid_url::invalid_url():
    bad_url("invalid URI")
{}

/**
 * @brief Destroy.
 */
openvrml::invalid_url::~invalid_url() throw ()
{}


/**
 * @class openvrml::bad_media_type
 *
 * @brief Thrown when an unexpected media type is received.
 */

/**
 * @brief Construct.
 *
 * @param[in] received_type the media type that was unexpectedly received.
 */
openvrml::bad_media_type::bad_media_type(const std::string & received_type)
    OPENVRML_NOTHROW:
    bad_url("unexpected media type \"" + received_type + "\"")
{}

/**
 * @brief Destroy.
 */
openvrml::bad_media_type::~bad_media_type() throw ()
{}


/**
 * @class unreachable_url
 *
 * @brief Thrown when a URI cannot be reached.
 */

/**
 * @brief Construct.
 */
openvrml::unreachable_url::unreachable_url():
    bad_url("unreachable URI")
{}

/**
 * @brief Destroy.
 */
openvrml::unreachable_url::~unreachable_url() throw ()
{}


/**
 * @class openvrml::no_alternative_url
 *
 * @brief Exception thrown when no URI in an alternative URI list can be
 *        resolved.
 */

/**
 * @brief Construct.
 */
openvrml::no_alternative_url::no_alternative_url():
    bad_url("no alternative URI could be resolved")
{}

/**
 * @brief Destroy.
 */
openvrml::no_alternative_url::~no_alternative_url() throw ()
{}


/**
 * @class openvrml::scene
 *
 * @brief A scene in the VRML world.
 */

/**
 * @internal
 *
 * @var openvrml::browser * const openvrml::scene::browser_
 *
 * @brief A reference to the browser associated with the scene.
 */

/**
 * @internal
 *
 * @var openvrml::scene * const openvrml::scene::parent_
 *
 * @brief A pointer to the parent scene.
 *
 * If the scene is the root scene, @a parent will be 0.
 */

/**
 * @internal
 *
 * @var boost::recursive_mutex openvrml::scene::nodes_mutex_
 *
 * @brief Mutex protecting @a nodes_.
 */

/**
 * @internal
 *
 * @var openvrml::mfnode openvrml::scene::nodes_
 *
 * @brief The nodes for the scene.
 */

/**
 * @internal
 *
 * @var boost::read_write_mutex openvrml::scene::url_mutex_
 *
 * @brief Mutex protecting @a url_.
 */

/**
 * @internal
 *
 * @var const std::string openvrml::scene::url_
 *
 * @brief The URI for the scene.
 *
 * @a uri may be a relative or an absolute reference.
 */

/**
 * @internal
 *
 * @var boost::mutex openvrml::scene::meta_mutex_
 *
 * @brief Mutex protecting @a meta_.
 */

/**
 * @internal
 *
 * @var std::map<std::string, std::string> openvrml::scene::meta_
 *
 * @brief Scene metadata map.
 */

namespace {

    class OPENVRML_LOCAL bad_path : public openvrml::bad_url {
    public:
        explicit bad_path(const std::string & message);
        virtual ~bad_path() throw ();
    };

    bad_path::bad_path(const std::string & message):
        openvrml::bad_url(message)
    {}

    bad_path::~bad_path() throw ()
    {}

    /**
     * @brief Create an absolute "file" URL from a relative path.
     *
     * @param[in] relative_uri  a relative URI.
     *
     * @exception bad_path          if @p relative_uri cannot be resolved.
     * @exception std::bad_alloc    if memory allocation fails.
     */
    OPENVRML_LOCAL const uri create_file_url(const uri & relative_uri)
        OPENVRML_THROW2(bad_path, std::bad_alloc)
    {
        assert(relative(relative_uri));

        using std::string;
        using std::ostringstream;

        ostringstream result;
        result.unsetf(ostringstream::skipws);
        result << "file://";
        uri result_uri;

        try {
# ifdef _WIN32
            //
            // _fullpath returns a string starting with the drive letter; for
            // the URL, the path must begin with a '/'. So we simply put one at
            // the beginning of the buffer.
            //
            char buffer[_MAX_PATH] = { '/' };
            char * resolved_path =
                _fullpath(buffer + 1, relative_uri.path().c_str(), _MAX_PATH);
            if (!resolved_path) {
                throw bad_path("cannot resolve \"" + relative_uri.path()
                               + "\"");
            }
            std::replace_if(resolved_path,
                            resolved_path + strlen(resolved_path) + 1,
                            std::bind2nd(std::equal_to<char>(), '\\'), '/');
            assert(resolved_path == buffer);
# else
            char buffer[PATH_MAX];
            const char * resolved_path = realpath(relative_uri.path().c_str(),
                                                  buffer);
            if (!resolved_path) {
                static const size_t buf_size = 256;
                char buf[buf_size];
                strerror_r(errno, buf, buf_size);
                throw bad_path(buf);
            }
# endif

            result << resolved_path;

            const string query = relative_uri.query();
            if (!query.empty()) { result << '?' << query; }

            const string fragment = relative_uri.fragment();
            if (!fragment.empty()) { result << '#' << fragment; }

            result_uri = uri(result.str());

        } catch (openvrml::invalid_url &) {
            assert(false); // If we constructed a bad URI, something is wrong.
        }

        return result_uri;
    }
} // namespace

/**
 * @brief Construct.
 *
 * @param[in] browser   the browser associated with the scene.
 * @param[in] parent    the parent scene.
 */
openvrml::scene::scene(openvrml::browser & browser, scene * parent)
    OPENVRML_NOTHROW:
    browser_(&browser),
    parent_(parent)
{}

/**
 * @brief Destroy.
 */
openvrml::scene::~scene() OPENVRML_NOTHROW
{}

/**
 * @brief Get the associated <code>browser</code>.
 *
 * @return the associated <code>browser</code>.
 */
openvrml::browser & openvrml::scene::browser() const OPENVRML_NOTHROW
{
    return *this->browser_;
}

/**
 * @brief Get the parent <code>scene</code>.
 *
 * @return the parent <code>scene</code>, or 0 if this is the root
 *         <code>scene</code>.
 */
openvrml::scene * openvrml::scene::parent() const OPENVRML_NOTHROW
{
    return this->parent_;
}

struct OPENVRML_LOCAL openvrml::scene::load_scene {

    load_scene(openvrml::scene & scene,
               const std::vector<std::string> & url):
        scene_(&scene),
        url_(&url)
    {}

    void operator()() const OPENVRML_NOTHROW
    try {
        using std::endl;
        using std::string;
        using std::vector;

        openvrml::scene & scene = *this->scene_;
        const vector<string> & url = *this->url_;

        assert(scene.url_.empty());

        vector<boost::intrusive_ptr<node> > nodes;
        try {
            std::auto_ptr<resource_istream> in = scene.get_resource(url);
            if (!(*in)) { throw unreachable_url(); }
            scene.load(*in);
        } catch (antlr::ANTLRException & ex) {
            scene.browser().err(ex.getMessage());
        } catch (std::exception & ex) {
            scene.browser().err(ex.what());
            throw unreachable_url();
        } catch (...) {
            throw unreachable_url();
        }
        scene.nodes(nodes);
        scene.scene_loaded();
    } catch (std::exception & ex) {
        this->scene_->browser().err(ex.what());
    }

private:
    openvrml::scene * const scene_;
    const std::vector<std::string> * const url_;
};

/**
 * @brief Load a world.
 *
 * The world specified by @p url is loaded asynchronously.
 * <code>scene::scene_loaded</code> is called when loading the world has
 * completed (i.e., when <code>scene::nodes</code> will return the world's
 * root <code>node</code>s).
 *
 * As this function executes asynchronously, note that it will not throw upon
 * encountering a malformed or unreachable URI, or syntactically incorrect
 * VRML.
 *
 * @param[in] url   the URI for the world.  Per VRML97 convention, this is a list
 *              of alternative URIs.  The first one in the list to load
 *              successfully is used.
 *
 * @exception boost::thread_resource_error  if a new thread of execution cannot
 *                                          be started.
 * @exception std::bad_alloc                if memory allocation fails.
 */
void openvrml::scene::load(const std::vector<std::string> & url)
    OPENVRML_THROW2(boost::thread_resource_error, std::bad_alloc)
{
    boost::function0<void> f = load_scene(*this, url);
    boost::thread t(f);
}

/**
 * @brief Load the scene from a stream.
 *
 * @param[in,out] in    an input stream.
 *
 * @exception bad_media_type    if @p in.type() is not "model/vrml",
 *                              "x-world/x-vrml", or "model/x3d+vrml".
 * @excpetion invalid_vrml      if @p in has invalid syntax.
 */
void openvrml::scene::load(resource_istream & in)
{
    boost::recursive_mutex::scoped_lock nodes_lock(this->nodes_mutex_);
    boost::mutex::scoped_lock
        url_lock(this->url_mutex_),
        meta_lock(this->meta_mutex_);

    this->url_ = in.url();
    parse_vrml(in, in.url(), in.type(), *this, this->nodes_, this->meta_);
}

/**
 * @brief Initialize the scene.
 *
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::scene::initialize(const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::recursive_mutex::scoped_lock lock(this->nodes_mutex_);
    for (std::vector<boost::intrusive_ptr<node> >::iterator node(
             this->nodes_.begin());
         node != this->nodes_.end();
         ++node) {
        assert(*node);
        (*node)->initialize(*this, timestamp);
        if (child_node * const child = node_cast<child_node *>(node->get())) {
            child->relocate();
        }
    }
}

/**
 * @brief Get metadata.
 *
 * @param[in] key   metadata key.
 *
 * @return the metadata value associated with @p key.
 *
 * @exception std::invalid_argument if there is no value associated with
 *                                  @p key.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const std::string openvrml::scene::meta(const std::string & key) const
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->meta_mutex_);

    using std::map;
    using std::string;

    const map<string, string>::const_iterator pos = this->meta_.find(key);
    if (pos == this->meta_.end()) {
        throw std::invalid_argument("no metadata value associated with \""
                                    + key + "\"");
    }
    return pos->second;
}

/**
 * @brief Set metadata.
 *
 * @param[in] key   metadata key.
 * @param[in] value metadata value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::scene::meta(const std::string & key, const std::string & value)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->meta_mutex_);
    this->meta_[key] = value;
}

/**
 * @brief Get the metadata keys.
 *
 * @return the metadata keys.
 */
const std::vector<std::string> openvrml::scene::meta_keys() const
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->meta_mutex_);

    using std::map;
    using std::string;
    using std::vector;

    vector<string> result;
    for (map<string, string>::const_iterator entry = this->meta_.begin();
         entry != this->meta_.end();
         ++entry) {
        result.push_back(entry->first);
    }
    return result;
}

/**
 * @brief Root nodes for the scene.
 *
 * @return the root nodes for the scene.
 */
const std::vector<boost::intrusive_ptr<openvrml::node> > &
openvrml::scene::nodes() const throw()
{
    boost::recursive_mutex::scoped_lock lock(this->nodes_mutex_);
    return this->nodes_;
}

/**
 * @brief Set the root nodes for the scene.
 *
 * This function calls @c scene::shutdown to shut down the scene's existing nodes.
 *
 * @param[in] n the new root nodes for the scene.
 *
 * @exception std::invalid_argument if any of the nodes in @p n has already been
 *                                  initialized.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void openvrml::scene::nodes(const std::vector<boost::intrusive_ptr<node> > & n)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    class check_uninitialized_traverser : public node_traverser {
    private:
        virtual void on_entering(node & n)
        {
            if (n.scene()) {
                throw std::invalid_argument("node already initialized");
            }
        }
    } check_uninitialized;
    check_uninitialized.traverse(n);

    boost::recursive_mutex::scoped_lock lock(this->nodes_mutex_);
    const double now = browser::current_time();
    this->shutdown(now);
    this->nodes_ = n;
}

/**
 * @brief Get the root scope.
 *
 * @return the root scope.
 */
const openvrml::scope * openvrml::scene::root_scope() const
    OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->nodes_mutex_);
    return this->nodes_.empty()
        ? 0
        : &this->nodes_.front()->scope();
}

/**
 * @brief Get the absolute URI for the scene.
 *
 * @return the absolute URI for the scene.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string openvrml::scene::url() const OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->url_mutex_);
    using std::string;
    const string result = this->parent_
                        ? string(uri(this->url_)
                                 .resolve_against(uri(this->parent_->url())))
                        : this->url_;
    return result;
}

/**
 * @brief Render the scene.
 *
 * @param[in,out] viewer    a viewer to render to.
 * @param[in]     context   a rendering_context.
 */
void openvrml::scene::render(openvrml::viewer & viewer,
                             rendering_context context)
{
    boost::recursive_mutex::scoped_lock lock(this->nodes_mutex_);
    for (std::vector<boost::intrusive_ptr<node> >::iterator n(
             this->nodes_.begin());
         n != this->nodes_.end();
         ++n) {
        assert(*n);
        child_node * child = node_cast<child_node *>(n->get());
        if (child) { child->render_child(viewer, context); }
    }
}

/**
 * @brief Load a resource into @a browser.
 *
 * This method simply resolves any relative references in @p uri and calls
 * browser::load_url.
 *
 * @note There are a couple of edge cases here where we are probably doing the
 *      wrong thing:
 *       - If there is a URI in the list of the form "#NodeId" and it is not
 *         the first URI in the list, this URI will be loaded as if it were a
 *         new world rather than as a Viewpoint that should simply be bound.
 *       - If the first URI in the list is of the form "#NodeId" and no
 *         Viewpoint named "NodeId" exists in the scene, this method will not
 *         try any subsequent URIs in the list.
 *
 * @param[in] url       an array of URIs. Per VRML97 convention, the first resource
 *                  in the sequence that can be reached will be loaded into the
 *                  browser.
 * @param[in] parameter an array of parameters to be associated with the URIs in
 *                  @p uri.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @todo This method currently fails silently if any of the URIs in @p url is
 *      invalid. Should this throw invalid_url?
 */
void openvrml::scene::load_url(const std::vector<std::string> & url,
                               const std::vector<std::string> & parameter)
    OPENVRML_THROW1(std::bad_alloc)
{
    using std::string;

    if (!url.empty()) {
        if (url[0][0] == '#') {
# if 0
            //
            // If the first element in uri is a Viewpoint name, bind the
            // Viewpoint.
            //
            this->browser_->setViewpoint(uri[0].substr(1));
# endif
        } else {
            std::vector<std::string> absoluteURIs(url.size());
            for (size_t i = 0; i < absoluteURIs.size(); ++i) {
                try {
                    const uri urlElement(url[i]);
                    const string value =
                        relative(urlElement)
                            ? urlElement.resolve_against(uri(this->url()))
                            : urlElement;
                    absoluteURIs[i] = value;
                } catch (invalid_url & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            this->browser_->load_url(absoluteURIs, parameter);
        }
    }
}

/**
 * @brief Get a resource using a list of alternative URIs.
 *
 * Relative URIs in @p url are resolved against the absolute URI of the
 * <code>scene</code>.
 *
 * @param[in] url   a list of alternative URIs.
 *
 * @return the resource.
 *
 * @exception no_alternative_url    if none of the elements of @p url can be
 *                                  resolved.
 * @exception std::bad_alloc        if memory allocation fails.
 */
std::auto_ptr<openvrml::resource_istream>
openvrml::scene::get_resource(const std::vector<std::string> & url) const
    OPENVRML_THROW2(no_alternative_url, std::bad_alloc)
{
    using std::string;
    using std::vector;

    std::auto_ptr<resource_istream> in;

    for (vector<string>::size_type i = 0; i < url.size(); ++i) {
        try {
            //
            // Throw invalid_url if it isn't a valid URI.
            //
            uri test_uri(url[i]);

            //
            // If we have a relative reference, resolve it against this->url();
            // unless the parent is null and this->url() is empty, in which
            // case we are loading the root scene.  In that case, construct an
            // absolute file URL.
            //
            const uri absolute_uri = !relative(test_uri)
                                   ? test_uri
                                   : (!this->parent() && this->url().empty())
                                        ? create_file_url(test_uri)
                                        : test_uri
                                            .resolve_against(uri(this->url()));

            try {
                in = this->browser().get_resource(absolute_uri);
            } catch (...) {
                throw unreachable_url();
            }
            if (!in.get() || !(*in)) { throw unreachable_url(); }
            break;
        } catch (bad_url & ex) {
            this->browser().err(ex.what());
            continue;
        }
    }
    if (!in.get()) { throw no_alternative_url(); }
    return in;
}

/**
 * @brief Shut down the nodes in the scene.
 *
 * This function @b must be called before the scene is destroyed.
 *
 * @param[in] timestamp the current time.
 */
void openvrml::scene::shutdown(const double timestamp) OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->nodes_mutex_);
    for (std::vector<boost::intrusive_ptr<node> >::iterator n(
             this->nodes_.begin());
         n != this->nodes_.end();
         ++n) {
        if (*n) { (*n)->shutdown(timestamp); }
    }
}

/**
 * @brief Function called once the scene has been loaded.
 *
 * <code>scene::load</code> calls this function once the scene has finished
 * loading. The default implementation does nothing.
 */
void openvrml::scene::scene_loaded()
{}


openvrml::null_node_class::null_node_class(openvrml::browser & browser)
    OPENVRML_NOTHROW:
    node_class(browser)
{}

openvrml::null_node_class::~null_node_class() OPENVRML_NOTHROW
{}

const boost::shared_ptr<openvrml::node_type>
openvrml::null_node_class::
do_create_type(const std::string &, const node_interface_set &) const
    OPENVRML_NOTHROW
{
    assert(false);
    static const boost::shared_ptr<node_type> nodeType;
    return nodeType;
}


openvrml::null_node_type::null_node_type(null_node_class & nodeClass)
    OPENVRML_NOTHROW:
    node_type(nodeClass, std::string())
{}

openvrml::null_node_type::~null_node_type() OPENVRML_NOTHROW
{}

const openvrml::node_interface_set &
openvrml::null_node_type::do_interfaces() const OPENVRML_NOTHROW
{
    assert(false);
    static const node_interface_set interfaces;
    return interfaces;
}

const boost::intrusive_ptr<openvrml::node>
openvrml::null_node_type::
do_create_node(const boost::shared_ptr<openvrml::scope> &,
               const initial_value_map &) const
    OPENVRML_NOTHROW
{
    assert(false);
    static const boost::intrusive_ptr<node> node;
    return node;
}

namespace {

    component::~component() OPENVRML_NOTHROW
    {}

    void component::add_scope_entry(
        const openvrml::browser & b,
        const char * node_name,
        const openvrml::node_interface_set & interface_set,
        const char * urn,
        openvrml::scope & scope) const
        OPENVRML_THROW3(openvrml::unsupported_interface, std::invalid_argument,
                        std::bad_alloc)
    {
        boost::shared_ptr<openvrml::node_class> node_class = b.node_class(urn);
        if (!node_class) { std::cout << node_name << std::endl; }
        assert(node_class);
        const bool succeeded =
            scope.add_type(node_class->create_type(node_name, interface_set));
        assert(succeeded);
    }


    class OPENVRML_LOCAL vrml97_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & root_scope,
                                  int support_level) const;
    };

    const char * const vrml97_component::id = "VRML97";

    void vrml97_component::add_to_scope(const openvrml::browser & b,
                                        openvrml::scope & scope,
                                        int) const
    {
        using namespace openvrml;

        //
        // Anchor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfstring_id,
                               "description"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "parameter"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 8);

            add_scope_entry(b,
                            "Anchor",
                            interface_set,
                            "urn:X-openvrml:node:Anchor",
                            scope);
        }

        //
        // Appearance node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "material"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "texture"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "textureTransform")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 3);
            add_scope_entry(b,
                            "Appearance",
                            interface_set,
                            "urn:X-openvrml:node:Appearance",
                            scope);
        }

        //
        // AudioClip node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfstring_id,
                               "description"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "loop"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "pitch"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "startTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "stopTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "duration_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 8);
            add_scope_entry(b,
                            "AudioClip",
                            interface_set,
                            "urn:X-openvrml:node:AudioClip",
                            scope);
        }

        //
        // Background node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sfbool_id,
                               "set_bind"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "groundAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfcolor_id,
                               "groundColor"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "backUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "bottomUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "frontUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "leftUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "rightUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "topUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "skyAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfcolor_id,
                               "skyColor"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isBound")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 12);
            add_scope_entry(b,
                            "Background",
                            interface_set,
                            "urn:X-openvrml:node:Background",
                            scope);
        }

        //
        // Billboard node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "axisOfRotation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 6);
            add_scope_entry(b,
                            "Billboard",
                            interface_set,
                            "urn:X-openvrml:node:Billboard",
                            scope);
        }

        //
        // Box node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "size");
            static const node_interface_set interface_set(&interface,
                                                          &interface + 1);
            add_scope_entry(b,
                            "Box",
                            interface_set,
                            "urn:X-openvrml:node:Box",
                            scope);
        }

        //
        // Collision node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "collide"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize"),
                node_interface(node_interface::field_id,
                               field_value::sfnode_id,
                               "proxy"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "collideTime")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 8);
            add_scope_entry(b,
                            "Collision",
                            interface_set,
                            "urn:X-openvrml:node:Collision",
                            scope);
        }

        //
        // Color node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::exposedfield_id,
                               field_value::mfcolor_id,
                               "color");
            static const node_interface_set interface_set(&interface,
                                                          &interface + 1);
            add_scope_entry(b,
                            "Color",
                            interface_set,
                            "urn:X-openvrml:node:Color",
                            scope);
        }

        //
        // ColorInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfcolor_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::sfcolor_id,
                               "value_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "ColorInterpolator",
                            interface_set,
                            "urn:X-openvrml:node:ColorInterpolator",
                            scope);
        }

        //
        // Cone node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "bottomRadius"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "height"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "side"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "bottom")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "Cone",
                            interface_set,
                            "urn:X-openvrml:node:Cone",
                            scope);
        }

        //
        // Coordinate node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "point");
            static const node_interface_set interface_set(&interface,
                                                          &interface + 1);
            add_scope_entry(b,
                            "Coordinate",
                            interface_set,
                            "urn:X-openvrml:node:Coordinate",
                            scope);
        }

        //
        // CoordinateInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::mfvec3f_id,
                               "value_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "CoordinateInterpolator",
                            interface_set,
                            "urn:X-openvrml:node:CoordinateInterpolator",
                            scope);
        }

        //
        // Cylinder node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "bottom"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "height"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "radius"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "side"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "top")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 5);
            add_scope_entry(b,
                            "Cylinder",
                            interface_set,
                            "urn:X-openvrml:node:Cylinder",
                            scope);
        }

        //
        // CylinderSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "autoOffset"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "diskAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "maxAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "minAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "offset"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfrotation_id,
                               "rotation_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "trackPoint_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 9);
            add_scope_entry(b,
                            "CylinderSensor",
                            interface_set,
                            "urn:X-openvrml:node:CylinderSensor",
                            scope);
        }

        //
        // DirectionalLight node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                                field_value::sffloat_id,
                                "ambientIntensity"),
                node_interface(node_interface::exposedfield_id,
                                field_value::sfcolor_id,
                                "color"),
                node_interface(node_interface::exposedfield_id,
                                field_value::sfvec3f_id,
                                "direction"),
                node_interface(node_interface::exposedfield_id,
                                field_value::sffloat_id,
                                "intensity"),
                node_interface(node_interface::exposedfield_id,
                                field_value::sfbool_id,
                                "on")
            };
            static const node_interface_set
                interface_set(interfaces, interfaces + 5);
            add_scope_entry(b, "DirectionalLight", interface_set, "urn:X-openvrml:node:DirectionalLight", scope);
        }

        //
        // ElevationGrid node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mffloat_id,
                               "set_height"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "normal"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "texCoord"),
                node_interface(node_interface::field_id,
                               field_value::mffloat_id,
                               "height"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "ccw"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "colorPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "creaseAngle"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "normalPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "solid"),
                node_interface(node_interface::field_id,
                               field_value::sfint32_id,
                               "xDimension"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "xSpacing"),
                node_interface(node_interface::field_id,
                               field_value::sfint32_id,
                               "zDimension"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "zSpacing")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 14);
            add_scope_entry(b,
                            "ElevationGrid",
                            interface_set,
                            "urn:X-openvrml:node:ElevationGrid",
                            scope);
        }

        //
        // Extrusion node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfvec2f_id,
                               "set_crossSection"),
                node_interface(node_interface::eventin_id,
                               field_value::mfrotation_id,
                               "set_orientation"),
                node_interface(node_interface::eventin_id,
                               field_value::mfvec2f_id,
                               "set_scale"),
                node_interface(node_interface::eventin_id,
                               field_value::mfvec3f_id,
                               "set_spine"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "beginCap"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "ccw"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "convex"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "creaseAngle"),
                node_interface(node_interface::field_id,
                               field_value::mfvec2f_id,
                               "crossSection"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "endCap"),
                node_interface(node_interface::field_id,
                               field_value::mfrotation_id,
                               "orientation"),
                node_interface(node_interface::field_id,
                               field_value::mfvec2f_id,
                               "scale"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "solid"),
                node_interface(node_interface::field_id,
                               field_value::mfvec3f_id,
                               "spine")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 14);
            add_scope_entry(b,
                            "Extrusion",
                            interface_set,
                            "urn:X-openvrml:node:Extrusion",
                            scope);
        }

        //
        // Fog node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sfbool_id,
                               "set_bind"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfstring_id,
                               "fogType"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "visibilityRange"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isBound")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 5);
            add_scope_entry(b,
                            "Fog",
                            interface_set,
                            "urn:X-openvrml:node:Fog",
                            scope);
        }

        //
        // FontStyle node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::field_id,
                               field_value::mfstring_id,
                               "family"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "horizontal"),
                node_interface(node_interface::field_id,
                               field_value::mfstring_id,
                               "justify"),
                node_interface(node_interface::field_id,
                               field_value::sfstring_id,
                               "language"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "leftToRight"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "size"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "spacing"),
                node_interface(node_interface::field_id,
                               field_value::sfstring_id,
                               "style"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "topToBottom")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 9);
            add_scope_entry(b,
                            "FontStyle",
                            interface_set,
                            "urn:X-openvrml:node:FontStyle",
                            scope);
        }

        //
        // Group node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 5);
            add_scope_entry(b,
                            "Group",
                            interface_set,
                            "urn:X-openvrml:node:Group",
                            scope);
        }

        //
        // ImageTexture node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatS"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatT")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 3);
            add_scope_entry(b,
                            "ImageTexture",
                            interface_set,
                            "urn:X-openvrml:node:ImageTexture",
                            scope);
        }

        //
        // IndexedFaceSet node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_colorIndex"),
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_coordIndex"),
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_normalIndex"),
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_texCoordIndex"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "coord"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "normal"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "texCoord"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "ccw"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "colorIndex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "colorPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "convex"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "coordIndex"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "creaseAngle"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "normalIndex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "normalPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "solid"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "texCoordIndex")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 18);
            add_scope_entry(b,
                            "IndexedFaceSet",
                            interface_set,
                            "urn:X-openvrml:node:IndexedFaceSet",
                            scope);
        }

        //
        // IndexedLineSet node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_colorIndex"),
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_coordIndex"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "coord"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "colorIndex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "colorPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "coordIndex")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 7);
            add_scope_entry(b,
                            "IndexedLineSet",
                            interface_set,
                            "urn:X-openvrml:node:IndexedLineSet",
                            scope);
        }

        //
        // Inline node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 3);
            add_scope_entry(b,
                            "Inline",
                            interface_set,
                            "urn:X-openvrml:node:Inline",
                            scope);
        }

        //
        // LOD node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "level"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "center"),
                node_interface(node_interface::field_id,
                               field_value::mffloat_id,
                               "range")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 3);
            add_scope_entry(b,
                            "LOD",
                            interface_set,
                            "urn:X-openvrml:node:LOD",
                            scope);
        }

        //
        // Material node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "ambientIntensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "diffuseColor"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "emissiveColor"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "shininess"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "specularColor"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "transparency")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 6);
            add_scope_entry(b,
                            "Material",
                            interface_set,
                            "urn:X-openvrml:node:Material",
                            scope);
        }

        //
        // MovieTexture node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "loop"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "speed"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "startTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "stopTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatS"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatT"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "duration_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 9);
            add_scope_entry(b,
                            "MovieTexture",
                            interface_set,
                            "urn:X-openvrml:node:MovieTexture",
                            scope);
        }

        //
        // NavigationInfo node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sfbool_id,
                               "set_bind"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "avatarSize"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "headlight"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "speed"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "type"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "visibilityLimit"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isBound")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 7);
            add_scope_entry(b,
                            "NavigationInfo",
                            interface_set,
                            "urn:X-openvrml:node:NavigationInfo",
                            scope);
        }

        //
        // Normal node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "vector");
            static const node_interface_set interface_set(&interface,
                                                          &interface + 1);
            add_scope_entry(b,
                            "Normal",
                            interface_set,
                            "urn:X-openvrml:node:Normal",
                            scope);
        }

        //
        // NormalInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::mfvec3f_id,
                               "value_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "NormalInterpolator",
                            interface_set,
                            "urn:X-openvrml:node:NormalInterpolator",
                            scope);
        }

        //
        // OrientationInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfrotation_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::sfrotation_id,
                               "value_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "OrientationInterpolator",
                            interface_set,
                            "urn:X-openvrml:node:OrientationInterpolator",
                            scope);
        }

        //
        // PixelTexture node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfimage_id,
                               "image"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatS"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatT")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 3);
            add_scope_entry(b,
                            "PixelTexture",
                            interface_set,
                            "urn:X-openvrml:node:PixelTexture",
                            scope);
        }

        //
        // PlaneSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "autoOffset"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "maxPosition"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "minPosition"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "offset"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "trackPoint_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "translation_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 8);
            add_scope_entry(b,
                            "PlaneSensor",
                            interface_set,
                            "urn:X-openvrml:node:PlaneSensor",
                            scope);
        }

        //
        // PointLight node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "ambientIntensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "attenuation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "intensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "location"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "on"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "radius")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 7);
            add_scope_entry(b,
                            "PointLight",
                            interface_set,
                            "urn:X-openvrml:node:PointLight",
                            scope);
        }

        //
        // PointSet node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "coord")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 2);
            add_scope_entry(b,
                            "PointSet",
                            interface_set,
                            "urn:X-openvrml:node:PointSet",
                            scope);
        }

        //
        // PositionInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "value_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "PositionInterpolator",
                            interface_set,
                            "urn:X-openvrml:node:PositionInterpolator",
                            scope);
        }

        //
        // ProximitySensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "center"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "size"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "position_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfrotation_id,
                               "orientation_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "enterTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "exitTime")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 8);
            add_scope_entry(b,
                            "ProximitySensor",
                            interface_set,
                            "urn:X-openvrml:node:ProximitySensor",
                            scope);
        }

        //
        // ScalarInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::sffloat_id,
                               "value_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "ScalarInterpolator",
                            interface_set,
                            "urn:X-openvrml:node:ScalarInterpolator",
                            scope);
        }

        //
        // Shape node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "appearance"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "geometry")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 2);
            add_scope_entry(b,
                            "Shape",
                            interface_set,
                            "urn:X-openvrml:node:Shape",
                            scope);
        }

        //
        // Sound node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "direction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "intensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "location"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "maxBack"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "maxFront"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "minBack"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "minFront"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "priority"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "source"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "spatialize")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 10);
            add_scope_entry(b,
                            "Sound",
                            interface_set,
                            "urn:X-openvrml:node:Sound",
                            scope);
        }

        //
        // Sphere node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "radius");
            static const node_interface_set interface_set(&interface,
                                                          &interface + 1);
            add_scope_entry(b,
                            "Sphere",
                            interface_set,
                            "urn:X-openvrml:node:Sphere",
                            scope);
        }

        //
        // SphereSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "autoOffset"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfrotation_id,
                               "offset"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfrotation_id,
                               "rotation_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "trackPoint_changed")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 6);
            add_scope_entry(b,
                            "SphereSensor",
                            interface_set,
                            "urn:X-openvrml:node:SphereSensor",
                            scope);
        }

        //
        // SpotLight node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "ambientIntensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "attenuation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "beamWidth"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "cutOffAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "direction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "intensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "location"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "on"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "radius")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 10);
            add_scope_entry(b,
                            "SpotLight",
                            interface_set,
                            "urn:X-openvrml:node:SpotLight",
                            scope);
        }

        //
        // Switch node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "choice"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfint32_id,
                               "whichChoice")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 2);
            add_scope_entry(b,
                            "Switch",
                            interface_set,
                            "urn:X-openvrml:node:Switch",
                            scope);
        }

        //
        // Text node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "string"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "fontStyle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "length"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "maxExtent")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "Text",
                            interface_set,
                            "urn:X-openvrml:node:Text",
                            scope);
        }

        //
        // TextureCoordinate node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec2f_id,
                               "point");
            static const node_interface_set interface_set(&interface,
                                                          &interface + 1);
            add_scope_entry(b,
                            "TextureCoordinate",
                            interface_set,
                            "urn:X-openvrml:node:TextureCoordinate",
                            scope);
        }

        //
        // TextureTransform node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "center"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "rotation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "scale"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "translation")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
            add_scope_entry(b,
                            "TextureTransform",
                            interface_set,
                            "urn:X-openvrml:node:TextureTransform",
                            scope);
        }

        //
        // TimeSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "cycleInterval"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "loop"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "startTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "stopTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "cycleTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sffloat_id,
                               "fraction_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "time")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 9);
            add_scope_entry(b,
                            "TimeSensor",
                            interface_set,
                            "urn:X-openvrml:node:TimeSensor",
                            scope);
        }

        //
        // TouchSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "hitNormal_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "hitPoint_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec2f_id,
                               "hitTexCoord_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isOver"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "touchTime")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 7);
            add_scope_entry(b,
                            "TouchSensor",
                            interface_set,
                            "urn:X-openvrml:node:TouchSensor",
                            scope);
        }

        //
        // Transform node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "center"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfrotation_id,
                               "rotation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "scale"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfrotation_id,
                               "scaleOrientation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "translation"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 10);
            add_scope_entry(b,
                            "Transform",
                            interface_set,
                            "urn:X-openvrml:node:Transform",
                            scope);
        }

        //
        // Viewpoint node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sfbool_id,
                               "set_bind"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "fieldOfView"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "jump"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfrotation_id,
                               "orientation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "position"),
                node_interface(node_interface::field_id,
                               field_value::sfstring_id,
                               "description"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "bindTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isBound")
            };
            static const node_interface_set
                interface_set(interfaces, interfaces + 8);
            add_scope_entry(b,
                            "Viewpoint",
                            interface_set,
                            "urn:X-openvrml:node:Viewpoint",
                            scope);
        }

        //
        // VisibilitySensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "center"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "size"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "enterTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "exitTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 6);
            add_scope_entry(b,
                            "VisibilitySensor",
                            interface_set,
                            "urn:X-openvrml:node:VisibilitySensor",
                            scope);
        }

        //
        // WorldInfo node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::field_id,
                               field_value::mfstring_id,
                               "info"),
                node_interface(node_interface::field_id,
                               field_value::sfstring_id,
                               "title")
            };
            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 2);
            add_scope_entry(b,
                            "WorldInfo",
                            interface_set,
                            "urn:X-openvrml:node:WorldInfo",
                            scope);
        }
    }


    class OPENVRML_LOCAL x3d_core_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_core_component::id = "Core";

    void x3d_core_component::add_to_scope(const openvrml::browser & b,
                                          openvrml::scope & scope,
                                          const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "name"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "reference"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfdouble_id,
                                   "value")
                };
                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "MetadataDouble",
                                interface_set,
                                "urn:X-openvrml:node:MetadataDouble",
                                scope);
            }

            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "name"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "reference"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "value")
                };
                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "MetadataFloat",
                                interface_set,
                                "urn:X-openvrml:node:MetadataFloat",
                                scope);
            }

            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "name"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "reference"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfint32_id,
                                   "value")
                };
                static const node_interface_set interface_set(interfaces,
                                                          interfaces + 4);
                add_scope_entry(b,
                                "MetadataInteger",
                                interface_set,
                                "urn:X-openvrml:node:MetadataInteger",
                                scope);
            }


            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "name"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "reference"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "value")
                };
                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "MetadataSet",
                                interface_set,
                                "urn:X-openvrml:node:MetadataSet",
                                scope);
            }

            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "name"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "reference"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "value")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "MetadataString",
                                interface_set,
                                "urn:X-openvrml:node:MetadataString",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_time_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_time_component::id = "Time";

    void x3d_time_component::add_to_scope(const openvrml::browser & b,
                                          openvrml::scope & scope,
                                          const int support_level) const
    {
        using namespace openvrml;

        //
        // TimeSensor node
        //
        if (support_level >= 1) {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "metadata"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "cycleInterval"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "loop"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "pauseTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "resumeTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "startTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "stopTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "cycleTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "elapsedTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sffloat_id,
                               "fraction_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isPaused"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "time")
            };

            static const node_interface_set interface_set(interfaces,
                                                          interfaces + 14);
            add_scope_entry(b,
                            "TimeSensor",
                            interface_set,
                            "urn:X-openvrml:node:TimeSensor",
                            scope);
        }
    }


    class OPENVRML_LOCAL x3d_networking_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_networking_component::id = "Networking";

    void x3d_networking_component::add_to_scope(const openvrml::browser & b,
                                                openvrml::scope & scope,
                                                const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 2) {
            //
            // Anchor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "addChildren"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "removeChildren"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "children"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "description"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "parameter"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "url"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 9);
                add_scope_entry(b,
                                "Anchor",
                                interface_set,
                                "urn:X-openvrml:node:Anchor",
                                scope);
            }

            //
            // Inline node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "load"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "url"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "Inline",
                                interface_set,
                                "urn:X-openvrml:node:Inline",
                                scope);
            }
        }

        if (support_level >= 3) {
            //
            // LoadSensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "timeout"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "watchList"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isLoaded"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "loadTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sffloat_id,
                                   "progress")
                };
                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 8);
                add_scope_entry(b,
                                "LoadSensor",
                                interface_set,
                                "urn:X-openvrml:node:LoadSensor",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_grouping_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_grouping_component::id = "Grouping";

    void x3d_grouping_component::add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // Group node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "addChildren"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "removeChildren"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "children"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 6);
                add_scope_entry(b,
                                "Group",
                                interface_set,
                                "urn:X-openvrml:node:Group",
                                scope);
            }

            //
            // Transform node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "addChildren"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "removeChildren"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "center"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "children"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfrotation_id,
                                   "rotation"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "scale"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfrotation_id,
                                   "scaleOrientation"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "translation"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 11);
                add_scope_entry(b,
                                "Transform",
                                interface_set,
                                "urn:X-openvrml:node:Transform",
                                scope);
            }

            //
            // WorldInfo node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::mfstring_id,
                                   "info"),
                    node_interface(node_interface::field_id,
                                   field_value::sfstring_id,
                                   "title")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "WorldInfo",
                                interface_set,
                                "urn:X-openvrml:node:WorldInfo",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // Switch node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "addChildren"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "removeChildren"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "children"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfint32_id,
                                   "whichChoice"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "Switch",
                                interface_set,
                                "urn:X-openvrml:node:Switch",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_rendering_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_rendering_component::id = "Rendering";

    void x3d_rendering_component::add_to_scope(const openvrml::browser & b,
                                               openvrml::scope & scope,
                                               const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // Color node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfcolor_id,
                                   "color")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "Color",
                                interface_set,
                                "urn:X-openvrml:node:Color",
                                scope);
            }

            //
            // ColorRGBA node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfcolorrgba_id,
                                   "color")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "ColorRGBA",
                                interface_set,
                                "urn:X-openvrml:node:ColorRGBA",
                                scope);
            }

            //
            // Coordinate node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec3f_id,
                                   "point")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "Coordinate",
                                interface_set,
                                "urn:X-openvrml:node:Coordinate",
                                scope);
            }

            //
            // IndexedLineSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_colorIndex"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_coordIndex"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "colorIndex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "coordIndex")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 8);
                add_scope_entry(b,
                                "IndexedLineSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedLineSet",
                                scope);
            }

            //
            // LineSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfint32_id,
                                   "vertexCount")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "LineSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedLineSet",
                                scope);
            }

            //
            // PointSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "PointSet",
                                interface_set,
                                "urn:X-openvrml:node:PointSet",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // Normal node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec3f_id,
                                   "vector")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "Normal",
                                interface_set,
                                "urn:X-openvrml:node:Normal",
                                scope);
            }
        }

        if (support_level >= 3) {
            //
            // IndexedTriangleFanSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_index"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "normal"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texCoord"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "normalPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "index")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 11);
                add_scope_entry(b,
                                "IndexedTriangleFanSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedTriangleFanSet",
                                scope);
            }

            //
            // IndexedTriangleSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_index"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "normal"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texCoord"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "normalPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "index")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 11);
                add_scope_entry(b,
                                "IndexedTriangleSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedTriangleSet",
                                scope);
            }

            //
            // IndexedTriangleStripSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_index"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "creaseAngle"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "normal"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texCoord"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "normalPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "index")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 12);
                add_scope_entry(b,
                                "IndexedTriangleStripSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedTriangleStripSet",
                                scope);
            }

            //
            // TriangleFanSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfint32_id,
                                   "fanCount"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "normal"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texCoord"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "normalPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 10);
                add_scope_entry(b,
                                "TriangleFanSet",
                                interface_set,
                                "urn:X-openvrml:node:TriangleFanSet",
                                scope);
            }

            //
            // TriangleSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "normal"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texCoord"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "normalPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 9);
                add_scope_entry(b,
                                "TriangleSet",
                                interface_set,
                                "urn:X-openvrml:node:TriangleSet",
                                scope);
            }

            //
            // TriangleStripSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "normal"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfint32_id,
                                   "stripCount"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texCoord"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "normalPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 10);
                add_scope_entry(b,
                                "TriangleStripSet",
                                interface_set,
                                "urn:X-openvrml:node:TriangleStripSet",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_shape_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_shape_component::id = "Shape";

    void x3d_shape_component::add_to_scope(const openvrml::browser & b,
                                           openvrml::scope & scope,
                                           const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // Appearance node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "fillProperties"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "lineProperties"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "material"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texture"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "textureTransform")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 6);
                add_scope_entry(b,
                                "Appearance",
                                interface_set,
                                "urn:X-openvrml:node:Appearance",
                                scope);
            }

            //
            // Material node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "ambientIntensity"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfcolor_id,
                                   "diffuseColor"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfcolor_id,
                                   "emissiveColor"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "shininess"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfcolor_id,
                                   "specularColor"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "transparency")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "Material",
                                interface_set,
                                "urn:X-openvrml:node:Material",
                                scope);
            }

            //
            // Shape node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "appearance"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "geometry"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "Shape",
                                interface_set,
                                "urn:X-openvrml:node:Shape",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // LineProperties node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "applied"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfint32_id,
                                   "lineType"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "linewidthScaleFactor")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "LineProperties",
                                interface_set,
                                "urn:X-openvrml:node:LineProperties",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_geometry3d_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_geometry3d_component::id = "Geometry3D";

    void x3d_geometry3d_component::add_to_scope(const openvrml::browser & b,
                                                openvrml::scope & scope,
                                                const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // Box node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "size"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "Box",
                                interface_set,
                                "urn:X-openvrml:node:Box",
                                scope);
            }

            //
            // Cone node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "bottom"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "bottomRadius"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "height"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "side"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 6);
                add_scope_entry(b,
                                "Cone",
                                interface_set,
                                "urn:X-openvrml:node:Cone",
                                scope);
            }

            //
            // Cylinder node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "bottom"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "height"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "radius"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "side"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "top")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "Cylinder",
                                interface_set,
                                "urn:X-openvrml:node:Cylinder",
                                scope);
            }

            //
            // Sphere node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "radius"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "Sphere",
                                interface_set,
                                "urn:X-openvrml:node:Sphere",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // IndexedFaceSet node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_colorIndex"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_coordIndex"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_normalIndex"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfint32_id,
                                   "set_texCoordIndex"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "coord"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "normal"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texCoord"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "colorIndex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "convex"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "coordIndex"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "creaseAngle"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "normalIndex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "normalPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid"),
                    node_interface(node_interface::field_id,
                                   field_value::mfint32_id,
                                   "texCoordIndex")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 19);
                add_scope_entry(b,
                                "IndexedFaceSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedFaceSet",
                                scope);
            }
        }

        if (support_level >= 3) {
            //
            // ElevationGrid node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mffloat_id,
                                   "set_height"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "normal"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "texCoord"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "colorPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "creaseAngle"),
                    node_interface(node_interface::field_id,
                                   field_value::mffloat_id,
                                   "height"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "normalPerVertex"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid"),
                    node_interface(node_interface::field_id,
                                   field_value::sfint32_id,
                                   "xDimension"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "xSpacing"),
                    node_interface(node_interface::field_id,
                                   field_value::sfint32_id,
                                   "zDimension"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "zSpacing")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 15);
                add_scope_entry(b,
                                "ElevationGrid",
                                interface_set,
                                "urn:X-openvrml:node:ElevationGrid",
                                scope);
            }
        }

        if (support_level >= 4) {
            //
            // Extrusion node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfvec2f_id,
                                   "set_crossSection"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfrotation_id,
                                   "set_orientation"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfvec2f_id,
                                   "set_scale"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfvec3f_id,
                                   "set_spine"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "beginCap"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "ccw"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "convex"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "creaseAngle"),
                    node_interface(node_interface::field_id,
                                   field_value::mfvec2f_id,
                                   "crossSection"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "endCap"),
                    node_interface(node_interface::field_id,
                                   field_value::mfrotation_id,
                                   "orientation"),
                    node_interface(node_interface::field_id,
                                   field_value::mfvec2f_id,
                                   "scale"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid"),
                    node_interface(node_interface::field_id,
                                   field_value::mfvec3f_id,
                                   "spine")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 15);
                add_scope_entry(b,
                                "Extrusion",
                                interface_set,
                                "urn:X-openvrml:node:Extrusion",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_geometry2d_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_geometry2d_component::id = "Geometry2D";

    void x3d_geometry2d_component::add_to_scope(const openvrml::browser & b,
                                                openvrml::scope & scope,
                                                const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // Polyline2D node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::mfvec2f_id,
                                   "lineSegments")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "Polyline2D",
                                interface_set,
                                "urn:X-openvrml:node:Polyline2D",
                                scope);
            }

            //
            // Polypoint2D node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::mfvec2f_id,
                                   "point")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "Polypoint2D",
                                interface_set,
                                "urn:X-openvrml:node:Polypoint2D",
                                scope);
            }

            //
            // Rectangle2D node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec2f_id,
                                   "size"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "Rectangle2D",
                                interface_set,
                                "urn:X-openvrml:node:Rectangle2D",
                                scope);
            }

            //
            // TriangleSet2D node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec2f_id,
                                   "vertices"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "TriangleSet2D",
                                interface_set,
                                "urn:X-openvrml:node:TriangleSet2D",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_text_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_text_component::id = "Text";

    void x3d_text_component::add_to_scope(const openvrml::browser & b,
                                          openvrml::scope & scope,
                                          const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // FontStyle node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::field_id,
                                   field_value::mfstring_id,
                                   "family"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "horizontal"),
                    node_interface(node_interface::field_id,
                                   field_value::mfstring_id,
                                   "justify"),
                    node_interface(node_interface::field_id,
                                   field_value::sfstring_id,
                                   "language"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "leftToRight"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "size"),
                    node_interface(node_interface::field_id,
                                   field_value::sffloat_id,
                                   "spacing"),
                    node_interface(node_interface::field_id,
                                   field_value::sfstring_id,
                                   "style"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "topToBottom")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 10);
                add_scope_entry(b,
                                "FontStyle",
                                interface_set,
                                "urn:X-openvrml:node:FontStyle",
                                scope);
            }

            //
            // Text node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "fontStyle"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "length"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "maxExtent"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "string"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "solid")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 6);
                add_scope_entry(b,
                                "Text",
                                interface_set,
                                "urn:X-openvrml:node:Text",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_sound_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_sound_component::id = "Sound";

    void x3d_sound_component::add_to_scope(const openvrml::browser & b,
                                           openvrml::scope & scope,
                                           const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // AudioClip node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "description"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "loop"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "pauseTime"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "pitch"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "resumeTime"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "startTime"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "stopTime"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "url"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "duration_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "elapsedTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isPaused")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 13);
                add_scope_entry(b,
                                "AudioClip",
                                interface_set,
                                "urn:X-openvrml:node:AudioClip",
                                scope);
            }

            //
            // Sound node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "direction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "intensity"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "location"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "maxBack"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "maxFront"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "minBack"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "minFront"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "priority"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "source"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "spatialize")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 11);
                add_scope_entry(b,
                                "Sound",
                                interface_set,
                                "urn:X-openvrml:node:Sound",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_lighting_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_lighting_component::id = "Lighting";

    void x3d_lighting_component::add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // DirectionalLight node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "ambientIntensity"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfcolor_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "direction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "intensity"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "on")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 6);
                add_scope_entry(b,
                                "DirectionalLight",
                                interface_set,
                                "urn:X-openvrml:node:DirectionalLight",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // PointLight node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "ambientIntensity"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "attenuation"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfcolor_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "intensity"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "location"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "on"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "radius")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 8);
                add_scope_entry(b,
                                "PointLight",
                                interface_set,
                                "urn:X-openvrml:node:PointLight",
                                scope);
            }

            //
            // SpotLight node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "ambientIntensity"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "attenuation"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "beamWidth"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfcolor_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "cutOffAngle"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "direction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "intensity"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "location"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "on"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "radius")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 11);
                add_scope_entry(b,
                                "SpotLight",
                                interface_set,
                                "urn:X-openvrml:node:SpotLight",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_texturing_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_texturing_component::id = "Texturing";

    void x3d_texturing_component::add_to_scope(const openvrml::browser & b,
                                               openvrml::scope & scope,
                                               const int support_level) const
    {
        using namespace openvrml;

        if (support_level >=  1) {
            //
            // ImageTexture node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "url"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "repeatS"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "repeatT")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "ImageTexture",
                                interface_set,
                                "urn:X-openvrml:node:ImageTexture",
                                scope);
            }

            //
            // PixelTexture node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfimage_id,
                                   "image"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "repeatS"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "repeatT")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "PixelTexture",
                                interface_set,
                                "urn:X-openvrml:node:PixelTexture",
                                scope);
            }

            //
            // TextureCoordinate node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec2f_id,
                                   "point")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "TextureCoordinate",
                                interface_set,
                                "urn:X-openvrml:node:TextureCoordinate",
                                scope);
            }

            //
            // TextureTransform node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec2f_id,
                                   "center"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "rotation"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec2f_id,
                                   "scale"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec2f_id,
                                   "translation")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "TextureTransform",
                                interface_set,
                                "urn:X-openvrml:node:TextureTransform",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // MultiTexture node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "alpha"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfcolor_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "function"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "mode"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "source"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "texture")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "MultiTexture",
                                interface_set,
                                "urn:X-openvrml:node:MultiTexture",
                                scope);
            }

            //
            // MultiTextureCoordinate node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "texCoord")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "MultiTextureCoordinate",
                                interface_set,
                                "urn:X-openvrml:node:MultiTextureCoordinate",
                                scope);
            }

            //
            // MultiTextureTransform node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "textureTransform")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 2);
                add_scope_entry(b,
                                "MultiTextureTransform",
                                interface_set,
                                "urn:X-openvrml:node:MultiTextureTransform",
                                scope);
            }

            //
            // TextureCoordinateGenerator node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "mode"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "parameter")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "TextureCoordinateGenerator",
                                interface_set,
                                "urn:X-openvrml:node:TextureCoordinateGenerator",
                                scope);
            }
        }

        if (support_level >= 3) {
            //
            // MovieTexture node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "loop"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "resumeTime"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "pauseTime"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "speed"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "startTime"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sftime_id,
                                   "stopTime"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "url"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "duration_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "elapsedTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isPaused"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "repeatS"),
                    node_interface(node_interface::field_id,
                                   field_value::sfbool_id,
                                   "repeatT")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 14);
                add_scope_entry(b,
                                "MovieTexture",
                                interface_set,
                                "urn:X-openvrml:node:MovieTexture",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_interpolation_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_interpolation_component::id = "Interpolation";

    void
    x3d_interpolation_component::add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // CoordinateInterpolator node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sffloat_id,
                                   "set_fraction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "key"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec3f_id,
                                   "keyValue"),
                    node_interface(node_interface::eventout_id,
                                   field_value::mfvec3f_id,
                                   "value_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "CoordinateInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:CoordinateInterpolator",
                                scope);
            }

            //
            // OrientationInterpolator node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sffloat_id,
                                   "set_fraction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "key"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfrotation_id,
                                   "keyValue"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfrotation_id,
                                   "value_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "OrientationInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:OrientationInterpolator",
                                scope);
            }

            //
            // PositionInterpolator node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sffloat_id,
                                   "set_fraction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "key"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec3f_id,
                                   "keyValue"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "value_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "PositionInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:PositionInterpolator",
                                scope);
            }

            //
            // ScalarInterpolator node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sffloat_id,
                                   "set_fraction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "key"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "keyValue"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sffloat_id,
                                   "value_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "ScalarInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:ScalarInterpolator",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // ColorInterpolator node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sffloat_id,
                                   "set_fraction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "key"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfcolor_id,
                                   "keyValue"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfcolor_id,
                                   "value_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "ColorInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:ColorInterpolator",
                                scope);
            }

            //
            // NormalInterpolator node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sffloat_id,
                                   "set_fraction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "key"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec3f_id,
                                   "keyValue"),
                    node_interface(node_interface::eventout_id,
                                   field_value::mfvec3f_id,
                                   "value_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "NormalInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:NormalInterpolator",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_pointing_device_sensor_component :
        public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_pointing_device_sensor_component::id =
        "PointingDeviceSensor";

    void
    x3d_pointing_device_sensor_component::
    add_to_scope(const openvrml::browser & b,
                 openvrml::scope & scope,
                 const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // CylinderSensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "autoOffset"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "description"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "diskAngle"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "maxAngle"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "minAngle"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "offset"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isOver"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfrotation_id,
                                   "rotation_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "trackPoint_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 12);
                add_scope_entry(b,
                                "CylinderSensor",
                                interface_set,
                                "urn:X-openvrml:node:CylinderSensor",
                                scope);
            }

            //
            // PlaneSensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "autoOffset"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "description"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec2f_id,
                                   "maxPosition"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec2f_id,
                                   "minPosition"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "offset"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isOver"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "trackPoint_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "translation_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 11);
                add_scope_entry(b,
                                "PlaneSensor",
                                interface_set,
                                "urn:X-openvrml:node:PlaneSensor",
                                scope);
            }

            //
            // SphereSensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "autoOffset"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "description"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfrotation_id,
                                   "offset"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isOver"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfrotation_id,
                                   "rotation_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "trackPoint_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 9);
                add_scope_entry(b,
                                "SphereSensor",
                                interface_set,
                                "urn:X-openvrml:node:SphereSensor",
                                scope);
            }

            //
            // TouchSensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "description"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "hitNormal_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "hitPoint_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec2f_id,
                                   "hitTexCoord_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isOver"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "touchTime")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 9);
                add_scope_entry(b,
                                "TouchSensor",
                                interface_set,
                                "urn:X-openvrml:node:TouchSensor",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_key_device_sensor_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_key_device_sensor_component::id = "KeyDeviceSensor";

    void
    x3d_key_device_sensor_component::
    add_to_scope(const openvrml::browser & b,
                 openvrml::scope & scope,
                 const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // KeySensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfint32_id,
                                   "actionKeyPress"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfint32_id,
                                   "actionKeyRelease"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "altKey"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "controlKey"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfstring_id,
                                   "keyPress"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfstring_id,
                                   "keyRelease"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "shiftKey")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 10);
                add_scope_entry(b,
                                "KeySensor",
                                interface_set,
                                "urn:X-openvrml:node:KeySensor",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // StringSensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "deletionAllowed"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfstring_id,
                                   "enteredText"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfstring_id,
                                   "finalText"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 6);
                add_scope_entry(b,
                                "StringSensor",
                                interface_set,
                                "urn:X-openvrml:node:StringSensor",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_environmental_sensor_component :
        public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_environmental_sensor_component::id =
        "EnvironmentalSensor";

    void
    x3d_environmental_sensor_component::
    add_to_scope(const openvrml::browser & b,
                 openvrml::scope & scope,
                 const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // ProximitySensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "center"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "size"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "enterTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "exitTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "centerOfRotation_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfrotation_id,
                                   "orientation_changed"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfvec3f_id,
                                   "position_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 10);
                add_scope_entry(b,
                                "ProximitySensor",
                                interface_set,
                                "urn:X-openvrml:node:ProximitySensor",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // VisibilitySensor node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "center"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "size"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "enterTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "exitTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "VisibilitySensor",
                                interface_set,
                                "urn:X-openvrml:node:VisibilitySensor",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_navigation_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_navigation_component::id = "Navigation";

    void x3d_navigation_component::add_to_scope(const openvrml::browser & b,
                                                openvrml::scope & scope,
                                                const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // NavigationInfo node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "set_bind"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "avatarSize"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "headlight"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "speed"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "transitionType"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "type"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "visibilityLimit"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "bindTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isBound")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 10);
                add_scope_entry(b,
                                "NavigationInfo",
                                interface_set,
                                "urn:X-openvrml:node:NavigationInfo",
                                scope);
            }

            //
            // Viewpoint node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "set_bind"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "description"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "centerOfRotation"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "fieldOfView"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "jump"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfrotation_id,
                                   "orientation"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "position"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "bindTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isBound")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 10);
                add_scope_entry(b,
                                "Viewpoint",
                                interface_set,
                                "urn:X-openvrml:node:Viewpoint",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // Billboard node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "addChildren"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "removeChildren"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfvec3f_id,
                                   "axisOfRotation"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "children"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "Billboard",
                                interface_set,
                                "urn:X-openvrml:node:Billboard",
                                scope);
            }

            //
            // Collision node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "addChildren"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "removeChildren"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "enabled"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "children"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "collideTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isActive"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize"),
                    node_interface(node_interface::field_id,
                                   field_value::sfnode_id,
                                   "proxy")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 10);
                add_scope_entry(b,
                                "Collision",
                                interface_set,
                                "urn:X-openvrml:node:Collision",
                                scope);
            }

            //
            // LOD node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "addChildren"),
                    node_interface(node_interface::eventin_id,
                                   field_value::mfnode_id,
                                   "removeChildren"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfnode_id,
                                   "children"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxCenter"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "bboxSize"),
                    node_interface(node_interface::field_id,
                                   field_value::sfvec3f_id,
                                   "center"),
                    node_interface(node_interface::field_id,
                                   field_value::mffloat_id,
                                   "range")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 8);
                add_scope_entry(b,
                                "LOD",
                                interface_set,
                                "urn:X-openvrml:node:LOD",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_environmental_effects_component :
        public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_environmental_effects_component::id =
        "EnvironmentalEffects";

    void
    x3d_environmental_effects_component::
    add_to_scope(const openvrml::browser & b,
                 openvrml::scope & scope,
                 const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // Background node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "set_bind"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "groundAngle"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfcolor_id,
                                   "groundColor"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "backUrl"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "bottomUrl"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "frontUrl"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "leftUrl"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "rightUrl"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfstring_id,
                                   "topUrl"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "skyAngle"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfcolor_id,
                                   "skyColor"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "bindTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isBound")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 14);
                add_scope_entry(b,
                                "Background",
                                interface_set,
                                "urn:X-openvrml:node:Background",
                                scope);
            }
        }

        if (support_level >= 2) {
            //
            // Fog node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "set_bind"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfcolor_id,
                                   "color"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfstring_id,
                                   "fogType"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sffloat_id,
                                   "visibilityRange"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "bindTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "isBound")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "Fog",
                                interface_set,
                                "urn:X-openvrml:node:Fog",
                                scope);
            }
        }
    }


    class OPENVRML_LOCAL x3d_event_utilities_component : public component {
    public:
        static const char * const id;

        virtual void add_to_scope(const openvrml::browser & b,
                                  openvrml::scope & scope,
                                  int support_level) const;
    };

    const char * const x3d_event_utilities_component::id = "EventUtilities";

    void
    x3d_event_utilities_component::add_to_scope(const openvrml::browser & b,
                                                openvrml::scope & scope,
                                                const int support_level) const
    {
        using namespace openvrml;

        if (support_level >= 1) {
            //
            // BooleanFilter node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "set_boolean"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "inputFalse"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "inputNegate"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "inputTrue")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 5);
                add_scope_entry(b,
                                "BooleanFilter",
                                interface_set,
                                "urn:X-openvrml:node:BooleanFilter",
                                scope);
            }

            //
            // BooleanSequencer node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "next"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "previous"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sffloat_id,
                                   "set_fraction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "key"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfbool_id,
                                   "keyValue"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "value_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "BooleanSequencer",
                                interface_set,
                                "urn:X-openvrml:node:BooleanSequencer",
                                scope);
            }

            //
            // BooleanToggle node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "set_boolean"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfbool_id,
                                   "toggle")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "BooleanToggle",
                                interface_set,
                                "urn:X-openvrml:node:BooleanToggle",
                                scope);
            }

            //
            // BooleanTrigger node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sftime_id,
                                   "set_triggerTime"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfbool_id,
                                   "triggerTrue")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "BooleanTrigger",
                                interface_set,
                                "urn:X-openvrml:node:BooleanTrigger",
                                scope);
            }

            //
            // IntegerSequencer node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "next"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "previous"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sffloat_id,
                                   "set_fraction"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mffloat_id,
                                   "key"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfint32_id,
                                   "keyValue"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfint32_id,
                                   "value_changed")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 7);
                add_scope_entry(b,
                                "IntegerSequencer",
                                interface_set,
                                "urn:X-openvrml:node:IntegerSequencer",
                                scope);
            }

            //
            // IntegerTrigger node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "set_boolean"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfint32_id,
                                   "integerKey"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sfint32_id,
                                   "triggerValue")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 4);
                add_scope_entry(b,
                                "IntegerTrigger",
                                interface_set,
                                "urn:X-openvrml:node:IntegerTrigger",
                                scope);
            }

            //
            // TimeTrigger node
            //
            {
                static const node_interface interfaces[] = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::eventin_id,
                                   field_value::sfbool_id,
                                   "set_boolean"),
                    node_interface(node_interface::eventout_id,
                                   field_value::sftime_id,
                                   "triggerTime")
                };

                static const node_interface_set interface_set(interfaces,
                                                              interfaces + 3);
                add_scope_entry(b,
                                "TimeTrigger",
                                interface_set,
                                "urn:X-openvrml:node:TimeTrigger",
                                scope);
            }
        }
    }


    component_registry::component_registry()
    {
        std::string key;
        bool succeeded;

        key = vrml97_component::id;
        succeeded = this->insert(key, new vrml97_component).second;
        assert(succeeded);

        key = x3d_core_component::id;
        succeeded = this->insert(key, new x3d_core_component).second;
        assert(succeeded);

        key = x3d_time_component::id;
        succeeded = this->insert(key, new x3d_time_component).second;
        assert(succeeded);

        key = x3d_networking_component::id;
        succeeded = this->insert(key, new x3d_networking_component).second;
        assert(succeeded);

        key = x3d_grouping_component::id;
        succeeded = this->insert(key, new x3d_grouping_component).second;
        assert(succeeded);

        key = x3d_rendering_component::id;
        succeeded = this->insert(key, new x3d_rendering_component).second;
        assert(succeeded);

        key = x3d_shape_component::id;
        succeeded = this->insert(key, new x3d_shape_component).second;
        assert(succeeded);

        key = x3d_geometry3d_component::id;
        succeeded = this->insert(key, new x3d_geometry3d_component).second;
        assert(succeeded);

        key = x3d_geometry2d_component::id;
        succeeded = this->insert(key, new x3d_geometry2d_component).second;
        assert(succeeded);

        key = x3d_text_component::id;
        succeeded = this->insert(key, new x3d_text_component).second;
        assert(succeeded);

        key = x3d_sound_component::id;
        succeeded = this->insert(key, new x3d_sound_component).second;
        assert(succeeded);

        key = x3d_lighting_component::id;
        succeeded = this->insert(key, new x3d_lighting_component).second;
        assert(succeeded);

        key = x3d_texturing_component::id;
        succeeded = this->insert(key, new x3d_texturing_component).second;
        assert(succeeded);

        key = x3d_interpolation_component::id;
        succeeded = this->insert(key, new x3d_interpolation_component).second;
        assert(succeeded);

        key = x3d_pointing_device_sensor_component::id;
        succeeded =
            this->insert(key, new x3d_pointing_device_sensor_component).second;
        assert(succeeded);

        key = x3d_key_device_sensor_component::id;
        succeeded =
            this->insert(key, new x3d_key_device_sensor_component).second;
        assert(succeeded);

        key = x3d_environmental_sensor_component::id;
        succeeded =
            this->insert(key, new x3d_environmental_sensor_component).second;
        assert(succeeded);

        key = x3d_navigation_component::id;
        succeeded = this->insert(key, new x3d_navigation_component).second;
        assert(succeeded);

        key = x3d_environmental_effects_component::id;
        succeeded =
            this->insert(key, new x3d_environmental_effects_component).second;
        assert(succeeded);

        key = x3d_event_utilities_component::id;
        succeeded =
            this->insert(key, new x3d_event_utilities_component).second;
        assert(succeeded);
    }


    void profile::add_component(const std::string & name, const int level)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //get the existing/new support level
        int & comp_level = components_[name];

        //update the support level.
        comp_level = std::max(level, comp_level);
    }

    std::auto_ptr<openvrml::scope>
    profile::create_root_scope(const openvrml::browser & browser,
                               const std::string & uri) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::scope;

        std::auto_ptr<scope> root_scope(new scope(uri));
        for (map_t::const_iterator entry = this->components_.begin();
             entry != this->components_.end();
             ++entry) {
            try {
                const component & c = ::component_registry_.at(entry->first);
                c.add_to_scope(browser, *root_scope, entry->second);
            } catch (boost::bad_ptr_container_operation & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            } catch (std::invalid_argument & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            }
        }
        return root_scope;
    }


    const char * const vrml97_profile::id = "VRML97";

    vrml97_profile::vrml97_profile()
    {
        this->add_component(vrml97_component::id, 1);
    }


    const char * const x3d_core_profile::id = "Core";

    x3d_core_profile::x3d_core_profile()
    {
        this->add_component(x3d_core_component::id, 1);
    }


    const char * const x3d_interchange_profile::id = "Interchange";

    x3d_interchange_profile::x3d_interchange_profile()
    {
        this->add_component(x3d_core_component::id, 1);
        this->add_component(x3d_time_component::id, 1);
        this->add_component(x3d_grouping_component::id, 1);
        this->add_component(x3d_rendering_component::id, 3);
        this->add_component(x3d_shape_component::id, 1);
        this->add_component(x3d_geometry3d_component::id, 2);
        this->add_component(x3d_lighting_component::id, 1);
        this->add_component(x3d_texturing_component::id, 2);
        this->add_component(x3d_interpolation_component::id, 2);
        this->add_component(x3d_navigation_component::id, 1);
        this->add_component(x3d_environmental_effects_component::id, 1);
    }


    const char * const x3d_interactive_profile::id = "Interactive";

    x3d_interactive_profile::x3d_interactive_profile()
    {
        this->add_component(x3d_core_component::id, 1);
        this->add_component(x3d_time_component::id, 1);
        this->add_component(x3d_grouping_component::id, 2);
        this->add_component(x3d_rendering_component::id, 2);
        this->add_component(x3d_shape_component::id, 1);
        this->add_component(x3d_geometry3d_component::id, 3);
        this->add_component(x3d_lighting_component::id, 2);
        this->add_component(x3d_texturing_component::id, 2);
        this->add_component(x3d_interpolation_component::id, 2);
        this->add_component(x3d_pointing_device_sensor_component::id, 1);
        this->add_component(x3d_key_device_sensor_component::id, 1);
        this->add_component(x3d_environmental_sensor_component::id, 1);
        this->add_component(x3d_navigation_component::id, 1);
        this->add_component(x3d_environmental_effects_component::id, 1);
        this->add_component(x3d_event_utilities_component::id, 1);
    }


    const char * const x3d_mpeg4_profile::id = "MPEG-4";

    x3d_mpeg4_profile::x3d_mpeg4_profile()
    {
        this->add_component(x3d_core_component::id, 1);
        this->add_component(x3d_time_component::id, 1);
        this->add_component(x3d_networking_component::id, 2);
        this->add_component(x3d_grouping_component::id, 2);
        this->add_component(x3d_rendering_component::id, 1);
        this->add_component(x3d_shape_component::id, 1);
        this->add_component(x3d_geometry3d_component::id, 2);
        this->add_component(x3d_lighting_component::id, 2);
        this->add_component(x3d_texturing_component::id, 2);
        this->add_component(x3d_interpolation_component::id, 2);
        this->add_component(x3d_pointing_device_sensor_component::id, 1);
        this->add_component(x3d_environmental_sensor_component::id, 1);
        this->add_component(x3d_navigation_component::id, 1);
        this->add_component(x3d_environmental_effects_component::id, 1);
    }


    const char * const x3d_immersive_profile::id = "Immersive";

    x3d_immersive_profile::x3d_immersive_profile()
    {
        this->add_component(x3d_core_component::id, 2);
        this->add_component(x3d_time_component::id, 1);
        this->add_component(x3d_networking_component::id, 3);
        this->add_component(x3d_grouping_component::id, 2);
        this->add_component(x3d_rendering_component::id, 3);
        this->add_component(x3d_shape_component::id, 2);
        this->add_component(x3d_geometry3d_component::id, 4);
        this->add_component(x3d_geometry2d_component::id, 1);
        this->add_component(x3d_text_component::id, 1);
        this->add_component(x3d_sound_component::id, 1);
        this->add_component(x3d_lighting_component::id, 2);
        this->add_component(x3d_texturing_component::id, 3);
        this->add_component(x3d_interpolation_component::id, 2);
        this->add_component(x3d_pointing_device_sensor_component::id, 1);
        this->add_component(x3d_key_device_sensor_component::id, 2);
        this->add_component(x3d_environmental_sensor_component::id, 2);
        this->add_component(x3d_navigation_component::id, 2);
        this->add_component(x3d_environmental_effects_component::id, 2);
        this->add_component(x3d_event_utilities_component::id, 1);
    }
} // namespace
