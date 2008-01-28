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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <cerrno>
# include <algorithm>
# include <functional>
# ifdef _WIN32
#   include <sys/timeb.h>
#   include <direct.h>
#   include <time.h>
# else
#   include <sys/time.h>
# endif
# include <boost/algorithm/string/predicate.hpp>
# include <boost/array.hpp>
# include <boost/bind.hpp>
# include <boost/enable_shared_from_this.hpp>
# include <boost/functional.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/mpl/for_each.hpp>
# include <boost/ptr_container/ptr_map.hpp>
# include <boost/thread/thread.hpp>
# include <boost/utility.hpp>
# include <private.h>
# include "browser.h"
# include "vrml97_grammar.h"
# include "x3d_vrml_grammar.h"
# include "vrml97node.h"
# include "x3d_core.h"
# include "x3d_networking.h"
# include "x3d_grouping.h"
# include "x3d_rendering.h"
# include "x3d_shape.h"
# include "x3d_geometry2d.h"
# include "x3d_texturing.h"
# include "x3d_interpolation.h"
# include "x3d_key_device_sensor.h"
# include "x3d_event_utilities.h"
# include "x3d_dis.h"
# include "x3d_environmental_effects.h"
# include "x3d_geospatial.h"
# include "x3d_hanim.h"
# include "x3d_nurbs.h"
# include "x3d_cad_geometry.h"

/**
 * @file openvrml/browser.h
 *
 * @brief Classes that manage the scene.
 */

namespace openvrml {

    class OPENVRML_LOCAL null_node_metatype : public node_metatype {
    public:
        explicit null_node_metatype(openvrml::browser & browser)
            OPENVRML_NOTHROW;
        virtual ~null_node_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL null_node_type : public node_type {
    public:
        explicit null_node_type(null_node_metatype & nodeClass)
            OPENVRML_NOTHROW;
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
     * @brief @c node_metatype for @c PROTO%s.
     *
     * The @c proto_node_metatype is OpenVRML's in-memory representation of a
     * @c PROTO (as opposed to a @c PROTO instance).  Through the
     * @c proto_node_type intermediary, it facilitates spawning any number of
     * @c proto_node instances.
     */
    class OPENVRML_LOCAL proto_node_metatype : public node_metatype {
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
        proto_node_metatype(
            const node_metatype_id & id,
            openvrml::browser & browser,
            const node_interface_set & interfaces,
            const default_value_map_t & default_value_map,
            const std::vector<boost::intrusive_ptr<node> > & impl_nodes,
            const is_map_t & is_map,
            const routes_t & routes);
        virtual ~proto_node_metatype() OPENVRML_NOTHROW;

        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @internal
     *
     * @brief @c node_type for @c PROTO node instances.
     */
    class OPENVRML_LOCAL proto_node_type : public node_type {
        node_interface_set interfaces_;

    public:
        proto_node_type(const proto_node_metatype & node_metatype,
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


    /**
     * @internal
     *
     * @brief A @c PROTO instance node.
     *
     * Like a typical node implementation, @c proto_node%s have a many-to-one
     * relationship with the @c proto_node_type instance that creates them.
     * And @c proto_node_type has, in turn, a many-to-one relationship with
     * the @c proto_node_metatype instance that creates them.  Unlike a
     * typical node implementation, there will very likely be more than one
     * @c proto_node_metatype instance known to the @c browser instance; there
     * will be one for each @c PROTO known to the @c browser.
     *
     * As the @c proto_node_metatype encodes the data in a @c PROTO, the
     * @c proto_node_type can be seen as modeling @c EXTERNPROTO. Each
     * @c EXTERNPROTO will spawn a new @c proto_node_type from the
     * @c proto_node_metatype that corresponds to the @c PROTO to which the
     * @c EXTERNPROTO refers.  Recall that an @c EXTERNPROTO provides a subset
     * of the interfaces defined for a @c PROTO; thus, for a @c PROTO with
     * <var>n</var> interfaces, there are <var>n</var>! possible unique
     * @c EXTERNPROTO%s (and thus unique @c proto_node_type instances).
     *
     * Structurally, the implementation of @c proto_node is very similar to
     * that of @c proto_node_metatype. The difference is that event pathways
     * for @c ROUTE%s and @c IS mappings are actually created in the
     * @c proto_node.  The @c proto_node_metatype, on the other hand, includes
     * metadata about how these event pathways @e should be created.
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
     * @brief A @c node path is used to store the path to a @c node for the
     *        purpose of duplicating a route in a cloned @c node hierarchy.
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
        for (node_interface_set::const_iterator interface_ = interfaces.begin();
             !this->found && interface_ != interfaces.end();
             ++interface_) {
            assert(&back == &this->node_path.back());
            if (interface_->type == node_interface::field_id
                || interface_->type == node_interface::exposedfield_id) {
                if (interface_->field_type == field_value::sfnode_id) {
                    back.field_type = field_value::sfnode_id;
                    back.field_id = interface_->id;
                    try {
                        const sfnode value = n.field<sfnode>(interface_->id);
                        this->get_path_from(value.value());
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                } else if (interface_->field_type == field_value::mfnode_id) {
                    back.field_type = field_value::mfnode_id;
                    back.field_id = interface_->id;
                    try {
                        const mfnode value = n.field<mfnode>(interface_->id);
                        this->get_path_from(value.value());
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
            }
        }
    }

    /**
     * @internal
     *
     * @brief Get the path to a node.
     *
     * @return the path to a node.
     */
    OPENVRML_LOCAL const node_path_t
    get_path(const std::vector<boost::intrusive_ptr<node> > & root,
             const node & objective)
        OPENVRML_THROW1(std::bad_alloc)
    {
        node_path_t path;
        path_getter(objective, path).get_path_from(root);
        return path;
    }

    /**
     * @internal
     *
     * @brief Resolve a node path against a node hierarchy.
     *
     * @return a pointer to the objective node in the node hierarchy
     *         designated by @p root.
     */
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
                for (node_interface_set::const_iterator interface_ =
                         interfaces.begin();
                     interface_ != interfaces.end();
                     ++interface_) {
                    using std::string;
                    const node_interface::type_id type = interface_->type;
                    const string & id = interface_->id;
                    if (type == node_interface::exposedfield_id
                        || type == node_interface::field_id) {
                        using std::auto_ptr;
                        using boost::shared_ptr;
                        auto_ptr<field_value> src = n->field(id);
                        auto_ptr<field_value> dest =
                            field_value::create(interface_->field_type);
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
        const proto_node_metatype & node_metatype;
        const initial_value_map & initial_values_;

    public:
        proto_impl_cloner(
            const proto_node_metatype & node_metatype,
            const initial_value_map & initial_values,
            const boost::shared_ptr<openvrml::scope> & target_scope):
            field_value_cloner(target_scope),
            node_metatype(node_metatype),
            initial_values_(initial_values)
        {}

        const std::vector<boost::intrusive_ptr<node> > clone()
            OPENVRML_THROW1(std::bad_alloc)
        {
            using std::vector;

            vector<boost::intrusive_ptr<node> > result(
                this->node_metatype.impl_nodes.size());

            for (vector<boost::intrusive_ptr<node> >::size_type i = 0;
                 i < this->node_metatype.impl_nodes.size();
                 ++i) {
                result[i] = this->clone_node(this->node_metatype.impl_nodes[i]);
                assert(result[i]);
            }
            return result;
        }

    private:
        struct matches_is_target :
            std::unary_function<proto_node_metatype::is_map_t::value_type, bool> {

            explicit matches_is_target(
                const proto_node_metatype::is_target & is_target):
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
            const proto_node_metatype::is_target * is_target;
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
                for (node_interface_set::const_iterator interface_ =
                         interfaces.begin();
                     interface_ != interfaces.end();
                     ++interface_) {
                    using std::string;
                    const node_interface::type_id type = interface_->type;
                    const string & id = interface_->id;
                    if (type == node_interface::exposedfield_id
                        || type == node_interface::field_id) {
                        using std::auto_ptr;
                        using std::find_if;
                        using boost::shared_ptr;
                        auto_ptr<const field_value> src_val;
                        auto_ptr<field_value> dest_val =
                            field_value::create(interface_->field_type);

                        //
                        // If the field/exposedField is IS'd, get the value
                        // from the initial_values_, or alternatively the
                        // default_values_.
                        //
                        typedef proto_node_metatype::is_target is_target;
                        typedef proto_node_metatype::is_map_t is_map;
                        typedef proto_node_metatype::default_value_map_t
                            default_value_map;

                        is_map::const_iterator is_mapping =
                            find_if(this->node_metatype.is_map.begin(),
                                    this->node_metatype.is_map.end(),
                                    matches_is_target(
                                        is_target(*n, interface_->id)));
                        if (is_mapping != this->node_metatype.is_map.end()) {
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
                                find_if(this->node_metatype.interfaces.begin(),
                                        this->node_metatype.interfaces.end(),
                                        bind(logical_or<bool>(),
                                             bind(node_interface_matches_exposedfield(),
                                                  _1,
                                                  is_mapping->first),
                                             bind(node_interface_matches_field(),
                                                  _1,
                                                  is_mapping->first)));

                            if (proto_interface
                                != this->node_metatype.interfaces.end()) {
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
                                        this->node_metatype.default_value_map
                                        .find(is_mapping->first);
                                    assert(default_value
                                           != this->node_metatype
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
     * @param[in] impl_node             a node in the @c PROTO implementation.
     * @param[in] impl_node_interface   an interface of @p impl_node.
     */
    proto_node_metatype::
    is_target::is_target(node & impl_node,
                         const std::string & impl_node_interface):
        impl_node(&impl_node),
        impl_node_interface(impl_node_interface)
    {}

    /**
     * @brief Construct.
     *
     * @param[in] from      event source @c node.
     * @param[in] eventout  @c eventOut of @p from.
     * @param[in] to        event destination @c node.
     * @param[in] eventin   @c eventIn of @p to.
     */
    proto_node_metatype::route::route(node & from,
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
     * @param[in] node_metatype the proto_node_metatype that spawned the
     *                          @c proto_node_type.
     * @param[in] id            @c node_type identifier.
     * @param[in] interfaces    a subset of the interfaces supported by the
     *                          @p node_metatype.
     *
     * @exception unsupported_interface if an interface in @p interfaces is not
     *                                  supported by the @p node_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    proto_node_type::proto_node_type(const proto_node_metatype & node_metatype,
                                     const std::string & id,
                                     const node_interface_set & interfaces)
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc):
        node_type(node_metatype, id)
    {
        using std::find;
        using std::invalid_argument;
        for (node_interface_set::const_iterator interface_ = interfaces.begin();
             interface_ != interfaces.end();
             ++interface_) {
            node_interface_set::const_iterator pos =
                find(node_metatype.interfaces.begin(),
                     node_metatype.interfaces.end(),
                     *interface_);
            if (pos == node_metatype.interfaces.end()) {
                throw unsupported_interface(*interface_);
            }
            const bool succeeded = this->interfaces_.insert(*interface_).second;
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
     * @brief @c PROTO @c eventIn handler class template.
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
     * @brief The name of the associated @c eventIn.
     *
     * @return the name of the associated @c eventIn.
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
     * @brief Add a listener to delegate to.  Corresponds to an @c IS statement.
     *
     * @param[in] listener  an @c event_listener to delegate to.
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
     * @brief Factory function for @c proto_eventin<FieldValue> instances.
     *
     * @param[in] type  @c field_value::type_id.
     * @param[in] node  @c proto_node.
     *
     * @return a @c boost::shared_ptr to a @c proto_eventin<FieldValue>
     *         instance.
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
     * @brief @c PROTO @c eventOut handler class template.
     */

    /**
     * @internal
     *
     * @class abstract_proto_node::proto_eventout::listener_t
     *
     * @brief Listens for events emitted from @c node%s in the @c PROTO
     *        implementation in order to propagate them out of the @c PROTO
     *        instance.
     */

    /**
     * @var abstract_proto_node::proto_eventout & abstract_proto_node::proto_eventout::listener_t::emitter
     *
     * @brief Reference to the outer @c proto_eventout class.
     *
     * @todo It's annoying that we need to carry this around.  Should
     *       investigate the possibility of promoting all this stuff to
     *       @c proto_eventout and have @c proto_eventout privately inherit
     *       @c field_value_listener<FieldValue>.
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
     * @param[in] initial_value initial value (used for @c exposedFields).
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
     * @brief Get the associated @c eventIn identifier.
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
     *                          @c proto_exposedfield<FieldValue>
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
     *                          implementation.
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
     * @brief The name of the associated @c eventOut.
     *
     * @return the name of the associated @c eventOut.
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
     * @brief @c PROTO @c exposedField handler class template.
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
     * @brief Factory function for @c proto_exposedfield<FieldValue> instances.
     *
     * @param[in] type  @c field_value::type_id.
     * @param[in] node  @c proto_node.
     *
     * @return a @c boost::shared_ptr to a @c proto_exposedfield<FieldValue>
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
     * @param[in] type  @c node_type.
     * @param[in] scope @c scope.
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
        const proto_node_metatype & node_metatype =
            static_cast<const proto_node_metatype &>(type.metatype());

        this->impl_nodes = proto_impl_cloner(node_metatype,
                                             initial_values,
                                             this->proto_scope).clone();

        //
        // Establish routes.
        //
        typedef proto_node_metatype::routes_t routes_t;
        for (routes_t::const_iterator route = node_metatype.routes.begin();
             route != node_metatype.routes.end();
             ++route) {
            // XXX
            // XXX It would be better to store the node_paths along with the
            // XXX route instead of rebuilding them every time we instantiate
            // XXX the PROTO.
            // XXX
            node_path_t path_to_from;
            assert(!node_metatype.impl_nodes.empty());
            path_getter(*route->from, path_to_from)
                .get_path_from(node_metatype.impl_nodes);
            assert(!path_to_from.empty());
            node * const from_node = resolve_node_path(path_to_from,
                                                       this->impl_nodes);
            assert(from_node);

            node_path_t path_to_to;
            path_getter(*route->to, path_to_to)
                .get_path_from(node_metatype.impl_nodes);
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
        for (node_interface_set::const_iterator interface_ =
                 node_metatype.interfaces.begin();
             interface_ != node_metatype.interfaces.end();
             ++interface_) {
            using boost::shared_ptr;
            using boost::dynamic_pointer_cast;
            using std::pair;
            bool succeeded;
            shared_ptr<openvrml::event_listener> interface_eventin;
            shared_ptr<openvrml::event_emitter> interface_eventout;
            typedef proto_node_metatype::is_map_t is_map_t;
            pair<is_map_t::const_iterator, is_map_t::const_iterator> is_range;
            initial_value_map::const_iterator initial_value;
            switch (interface_->type) {
            case node_interface::eventin_id:
                interface_eventin = create_eventin(interface_->field_type,
                                                   *this);
                is_range = node_metatype.is_map.equal_range(interface_->id);
                for (is_map_t::const_iterator is_mapping = is_range.first;
                     is_mapping != is_range.second;
                     ++is_mapping) {
                    assert(is_mapping->second.impl_node);
                    node_path_t path_to_impl_node;
                    path_getter(*is_mapping->second.impl_node,
                                path_to_impl_node)
                        .get_path_from(node_metatype.impl_nodes);
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
                            eventin_is(interface_->field_type,
                                       impl_eventin,
                                       *interface_eventin));
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
                succeeded = this->eventin_map
                    .insert(make_pair(interface_->id, interface_eventin))
                    .second;
                assert(succeeded);
                break;
            case node_interface::eventout_id:
                interface_eventout = create_eventout(interface_->field_type,
                                                     *this);
                is_range = node_metatype.is_map.equal_range(interface_->id);
                for (is_map_t::const_iterator is_mapping = is_range.first;
                     is_mapping != is_range.second;
                     ++is_mapping) {
                    assert(is_mapping->second.impl_node);
                    node_path_t path_to_impl_node;
                    path_getter(*is_mapping->second.impl_node,
                                path_to_impl_node)
                        .get_path_from(node_metatype.impl_nodes);
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
                            eventout_is(interface_->field_type,
                                        impl_eventout,
                                        *interface_eventout));
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
                succeeded = this->eventout_map
                    .insert(make_pair(interface_->id, interface_eventout))
                    .second;
                assert(succeeded);
                break;
            case node_interface::exposedfield_id:
                initial_value = initial_values.find(interface_->id);
                if (initial_value == initial_values.end()) {
                    initial_value =
                        node_metatype.default_value_map.find(interface_->id);
                    assert(initial_value
                           != node_metatype.default_value_map.end());
                }
                interface_eventin = create_exposedfield(*initial_value->second,
                                                        *this);
                interface_eventout =
                    dynamic_pointer_cast<openvrml::event_emitter>(
                        interface_eventin);
                is_range = node_metatype.is_map.equal_range(interface_->id);
                for (is_map_t::const_iterator is_mapping = is_range.first;
                     is_mapping != is_range.second;
                     ++is_mapping) {
                    assert(is_mapping->second.impl_node);
                    node_path_t path_to_impl_node;
                    path_getter(*is_mapping->second.impl_node,
                                path_to_impl_node)
                        .get_path_from(node_metatype.impl_nodes);
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
                            eventin_is(interface_->field_type,
                                       impl_eventin,
                                       *interface_eventin));
                        openvrml::event_emitter & impl_eventout =
                            impl_node->event_emitter(impl_node_interface);
                        for_each<field_value_types>(
                            eventout_is(interface_->field_type,
                                        impl_eventout,
                                        *interface_eventout));
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
                succeeded = this->eventin_map
                    .insert(make_pair("set_" + interface_->id,
                                      interface_eventin)).second;
                assert(succeeded);
                succeeded = this->eventout_map
                    .insert(make_pair(interface_->id + "_changed",
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
     * @brief Indicate whether the @c node needs to be rerendered.
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
     * @exception unsupported_interface if the @c node has no field @p id.
     *
     * @todo Make this function handle @c exposedFields.
     */
    const field_value & proto_node::do_field(const std::string & id) const
        OPENVRML_THROW1(unsupported_interface)
    {
        //
        // First, we need to find the implementation node that the field is
        // IS'd to.  For the accessor, we don't care if there's more than one.
        //
        const proto_node_metatype & node_metatype =
            static_cast<const proto_node_metatype &>(this->type().metatype());
        proto_node_metatype::is_map_t::const_iterator is_mapping =
            node_metatype.is_map.find(id);
        if (is_mapping != node_metatype.is_map.end()) {
            //
            // Get the path to the implementation node.
            //
            assert(is_mapping->second.impl_node);
            assert(!is_mapping->second.impl_node_interface.empty());
            node_path_t path;
            path_getter(*is_mapping->second.impl_node, path)
                .get_path_from(node_metatype.impl_nodes);

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
            proto_node_metatype::default_value_map_t::const_iterator
                default_value = node_metatype.default_value_map.find(id);
            if (default_value == node_metatype.default_value_map.end()) {
                throw unsupported_interface(this->type(), id);
            }
            return *default_value->second;
        }
    }

    /**
     * @brief @c event_listener accessor implementation.
     *
     * @param[in] id    @c eventIn identifier.
     *
     * @return the @c event_listener for the eventIn @p id.
     *
     * @exception unsupported_interface if the node has no @c eventIn @p id.
     */
    event_listener &
    proto_node::do_event_listener(const std::string & id)
        OPENVRML_THROW1(unsupported_interface)
    {
        eventin_map_t::iterator pos = this->eventin_map.find(id);
        if (pos == this->eventin_map.end()) {
            pos = this->eventin_map.find("set_" + id);
        }
        if (pos == this->eventin_map.end()) {
            throw unsupported_interface(this->type(),
                                        node_interface::eventin_id,
                                        id);
        }
        return *pos->second;
    }

    /**
     * @brief @c event_emitter accessor implementation.
     *
     * @param[in] id    @c eventOut identifier.
     *
     * @return the @c event_emitter for the eventOut @p id.
     *
     * @exception unsupported_interface if the node has no @c eventOut @p id.
     */
    event_emitter & proto_node::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(unsupported_interface)
    {
        eventout_map_t::iterator pos = this->eventout_map.find(id);
        if (pos == this->eventout_map.end()) {
            pos = this->eventout_map.find(id + "_changed");
        }
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
     * @brief Cast to a @c script_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c script_node, or 0 otherwise.
     */
    script_node * proto_node::to_script() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<script_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to an @c appearance_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is an @c appearance_node, or 0 otherwise.
     */
    appearance_node * proto_node::to_appearance() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<appearance_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c bounded_volume_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c bounded_volume_node, or 0 otherwise.
     */
    bounded_volume_node * proto_node::to_bounded_volume() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<bounded_volume_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c child_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c child_node, or 0 otherwise.
     */
    child_node * proto_node::to_child() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<child_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c color_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c color_node, or 0 otherwise.
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
     * @brief Cast to a @c coordinate_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c coordinate_node, or 0 otherwise.
     */
    coordinate_node * proto_node::to_coordinate() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<coordinate_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c font_style_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c font_style_node, or 0 otherwise.
     */
    font_style_node * proto_node::to_font_style() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<font_style_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c geometry_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c geometry_node, or 0 otherwise.
     */
    geometry_node * proto_node::to_geometry() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<geometry_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c grouping_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c grouping_node, or 0 otherwise.
     */
    grouping_node * proto_node::to_grouping() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<grouping_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c light_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c light_node, or 0 otherwise.
     */
    light_node * proto_node::to_light() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<light_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c material_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c material_node, or 0 otherwise.
     */
    material_node * proto_node::to_material() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<material_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c navigation_info_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c navigation_info_node, or 0 otherwise.
     */
    navigation_info_node * proto_node::to_navigation_info() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<navigation_info_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c normal_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c normal_node, or 0 otherwise.
     */
    normal_node * proto_node::to_normal() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<normal_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c pointing_device_sensor_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c pointing_device_sensor_node, or 0 otherwise.
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
     * @brief Cast to a @c scoped_light_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c scoped_light_node, or 0 otherwise.
     */
    scoped_light_node * proto_node::to_scoped_light() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<scoped_light_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c sound_source_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c sound_source_node, or 0 otherwise.
     */
    sound_source_node * proto_node::to_sound_source() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<sound_source_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c texture_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c texture_node, or 0 otherwise.
     */
    texture_node * proto_node::to_texture() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<texture_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c texture_coordinate_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c texture_coordinate_node, or 0 otherwise.
     */
    texture_coordinate_node * proto_node::to_texture_coordinate()
        OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<texture_coordinate_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c texture_transform_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c texture_transform_node, or 0 otherwise.
     */
    texture_transform_node * proto_node::to_texture_transform()
        OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<texture_transform_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c time_dependent_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c time_dependent_node, or 0 otherwise.
     */
    time_dependent_node * proto_node::to_time_dependent() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<time_dependent_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c transform_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c transform_node, or 0 otherwise.
     */
    transform_node * proto_node::to_transform() OPENVRML_NOTHROW
    {
        assert(!this->impl_nodes.empty());
        assert(this->impl_nodes[0]);
        return node_cast<transform_node *>(this->impl_nodes[0].get());
    }

    /**
     * @brief Cast to a @c viewpoint_node.
     *
     * @return a pointer to the first node in the implementation if that node
     *         is a @c viewpoint_node, or 0 otherwise.
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
    proto_node_metatype::
    proto_node_metatype(
        const node_metatype_id & id,
        openvrml::browser & browser,
        const node_interface_set & interfaces,
        const default_value_map_t & default_value_map,
        const std::vector<boost::intrusive_ptr<node> > & impl_nodes,
        const is_map_t & is_map,
        const routes_t & routes):
        node_metatype(id, browser),
        interfaces(interfaces),
        default_value_map(default_value_map),
        impl_nodes(impl_nodes),
        routes(routes),
        is_map(is_map)
    {}

    proto_node_metatype::~proto_node_metatype() OPENVRML_NOTHROW
    {}

    const boost::shared_ptr<node_type>
    proto_node_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        return boost::shared_ptr<node_type>(
            new proto_node_type(*this, id, interfaces));
    }
} // namespace openvrml


/**
 * @brief @c node_metatype for @c EXTERNPROTO%s.
 */
class OPENVRML_LOCAL openvrml::browser::externproto_node_metatype :
    public boost::enable_shared_from_this<externproto_node_metatype>,
    public openvrml::node_metatype {

    friend class externproto_node_type;

    struct load_proto;

    mutable boost::mutex mutex_;
    boost::weak_ptr<openvrml::proto_node_metatype> proto_node_metatype_;

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
        OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);

    virtual void do_shutdown(double time) OPENVRML_NOTHROW;

    void set_proto_node_metatype(
        const boost::weak_ptr<openvrml::proto_node_metatype> & proto_node_metatype = boost::weak_ptr<openvrml::proto_node_metatype>())
        OPENVRML_THROW1(std::bad_alloc);

    void clear_externproto_node_types() OPENVRML_NOTHROW;
};


class OPENVRML_LOCAL openvrml::browser::externproto_node_type :
    public openvrml::node_type {

    const boost::shared_ptr<const externproto_node_metatype> node_metatype_;

    openvrml::node_interface_set interfaces_;

    mutable boost::mutex mutex_;
    boost::shared_ptr<openvrml::proto_node_type> proto_node_type_;

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

    void set_proto_node_type(openvrml::proto_node_metatype & proto_node_metatype)
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


class OPENVRML_LOCAL openvrml::externproto_node :
    public openvrml::abstract_proto_node {

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

    typedef std::map<std::string, boost::shared_ptr<openvrml::field_value> >
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

    boost::intrusive_ptr<openvrml::proto_node> proto_node_;

public:
    externproto_node(const browser::externproto_node_type & type,
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
     * @param[in] t @c node_type.
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
     * @param[in] type  the <code>browser</code>'s @c null_node_type instance.
     */
    default_viewpoint::default_viewpoint(const openvrml::null_node_type & type)
        OPENVRML_NOTHROW:
        openvrml::node(type, null_scope_ptr),
        openvrml::bounded_volume_node(type, null_scope_ptr),
        openvrml::child_node(type, null_scope_ptr),
        openvrml::viewpoint_node(type, null_scope_ptr),
        userViewTransform(openvrml::make_mat4f())
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
        using openvrml::make_rotation;
        using openvrml::vec3f;
        using openvrml::make_vec3f;
        static const vec3f position = make_vec3f(0.0, 0.0, 10.0);
        static const rotation orientation = make_rotation();
        static const vec3f scale = make_vec3f(1.0, 1.0, 1.0);
        static const rotation scaleOrientation = make_rotation();
        static const vec3f center = make_vec3f();
        static const mat4f t(make_transformation_mat4f(position,
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
        uri(const uri & id) OPENVRML_THROW1(std::bad_alloc);

        uri & operator=(const uri & id) OPENVRML_THROW1(std::bad_alloc);

        operator std::string() const OPENVRML_THROW1(std::bad_alloc);

        void swap(uri & id) OPENVRML_NOTHROW;

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

    OPENVRML_LOCAL inline bool relative(const uri & id)
    {
        return id.scheme().empty();
    }

    const std::string anonymous_stream_id_prefix_ =
        "urn:X-openvrml:stream:";

    OPENVRML_LOCAL bool anonymous_stream_id(const uri & id)
    {
        const std::string str(id);
        return str.length() > anonymous_stream_id_prefix_.length()
            && std::equal(anonymous_stream_id_prefix_.begin(),
                          anonymous_stream_id_prefix_.end(),
                          str.begin());
    }

    uri::uri() OPENVRML_THROW1(std::bad_alloc):
        scheme_begin(this->str_.begin()),
        scheme_end(this->str_.begin()),
        scheme_specific_part_begin(this->str_.begin()),
        scheme_specific_part_end(this->str_.begin()),
        authority_begin(this->str_.begin()),
        authority_end(this->str_.begin()),
        userinfo_begin(this->str_.begin()),
        userinfo_end(this->str_.begin()),
        host_begin(this->str_.begin()),
        host_end(this->str_.begin()),
        port_begin(this->str_.begin()),
        port_end(this->str_.begin()),
        path_begin(this->str_.begin()),
        path_end(this->str_.begin()),
        query_begin(this->str_.begin()),
        query_end(this->str_.begin()),
        fragment_begin(this->str_.begin()),
        fragment_end(this->str_.begin())
    {}

    uri::uri(const std::string & str)
        OPENVRML_THROW2(openvrml::invalid_url, std::bad_alloc):
        str_(str),
        scheme_begin(this->str_.begin()),
        scheme_end(this->str_.begin()),
        scheme_specific_part_begin(this->str_.begin()),
        scheme_specific_part_end(this->str_.begin()),
        authority_begin(this->str_.begin()),
        authority_end(this->str_.begin()),
        userinfo_begin(this->str_.begin()),
        userinfo_end(this->str_.begin()),
        host_begin(this->str_.begin()),
        host_end(this->str_.begin()),
        port_begin(this->str_.begin()),
        port_end(this->str_.begin()),
        path_begin(this->str_.begin()),
        path_end(this->str_.begin()),
        query_begin(this->str_.begin()),
        query_end(this->str_.begin()),
        fragment_begin(this->str_.begin()),
        fragment_end(this->str_.begin())
    {
        using std::string;
        using namespace boost::spirit;

        actions a(*this);
        openvrml_::uri_grammar<actions> g(a);

        string::const_iterator begin = this->str_.begin();
        string::const_iterator end = this->str_.end();

        if (!parse(begin, end, g, space_p).full) {
            throw openvrml::invalid_url();
        }
    }

    uri::uri(const uri & id) OPENVRML_THROW1(std::bad_alloc):
        str_(id.str_),
        scheme_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                        id.scheme_begin)),
        scheme_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                      id.scheme_end)),
        scheme_specific_part_begin(
            this->str_.begin() + std::distance(id.str_.begin(),
                                               id.scheme_specific_part_begin)),
        scheme_specific_part_end(
            this->str_.begin() + std::distance(id.str_.begin(),
                                               id.scheme_specific_part_end)),
        authority_begin(this->str_.begin()
                        + std::distance(id.str_.begin(), id.authority_begin)),
        authority_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                         id.authority_end)),
        userinfo_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                          id.userinfo_begin)),
        userinfo_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                        id.userinfo_end)),
        host_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                      id.host_begin)),
        host_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                    id.host_end)),
        port_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                      id.port_begin)),
        port_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                    id.port_end)),
        path_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                      id.path_begin)),
        path_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                    id.path_end)),
        query_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                       id.query_begin)),
        query_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                     id.query_end)),
        fragment_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                          id.fragment_begin)),
        fragment_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                        id.fragment_end))
    {}

    uri & uri::operator=(const uri & id) OPENVRML_THROW1(std::bad_alloc)
    {
        uri temp(id);
        this->swap(temp);
        return *this;
    }

    uri::operator std::string() const OPENVRML_THROW1(std::bad_alloc)
    {
        return this->str_;
    }

    void uri::swap(uri & id) OPENVRML_NOTHROW
    {
        using std::distance;
        using std::string;
        using std::swap;

        const string::size_type id_scheme_begin_distance =
            distance<string::const_iterator>(id.str_.begin(), id.scheme_begin);
        const string::size_type id_scheme_end_distance =
            distance<string::const_iterator>(id.str_.begin(), id.scheme_end);
        const string::size_type id_scheme_specific_part_begin_distance =
            distance<string::const_iterator>(id.str_.begin(),
                                             id.scheme_specific_part_begin);
        const string::size_type id_scheme_specific_part_end_distance =
            distance<string::const_iterator>(id.str_.begin(),
                                             id.scheme_specific_part_end);
        const string::size_type id_authority_begin_distance =
            distance<string::const_iterator>(id.str_.begin(), id.authority_begin);
        const string::size_type id_authority_end_distance =
            distance<string::const_iterator>(id.str_.begin(), id.authority_end);
        const string::size_type id_userinfo_begin_distance =
            distance<string::const_iterator>(id.str_.begin(), id.userinfo_begin);
        const string::size_type id_userinfo_end_distance =
            distance<string::const_iterator>(id.str_.begin(), id.userinfo_end);
        const string::size_type id_host_begin_distance =
            distance<string::const_iterator>(id.str_.begin(), id.host_begin);
        const string::size_type id_host_end_distance =
            distance<string::const_iterator>(id.str_.begin(), id.host_end);
        const string::size_type id_port_begin_distance =
            distance<string::const_iterator>(id.str_.begin(), id.port_begin);
        const string::size_type id_port_end_distance =
            distance<string::const_iterator>(id.str_.begin(), id.port_end);
        const string::size_type id_path_begin_distance =
            distance<string::const_iterator>(id.str_.begin(), id.path_begin);
        const string::size_type id_path_end_distance =
            distance<string::const_iterator>(id.str_.begin(), id.path_end);
        const string::size_type id_query_begin_distance =
            distance<string::const_iterator>(id.str_.begin(), id.query_begin);
        const string::size_type id_query_end_distance =
            distance<string::const_iterator>(id.str_.begin(), id.query_end);
        const string::size_type id_fragment_begin_distance =
            distance<string::const_iterator>(id.str_.begin(), id.fragment_begin);
        const string::size_type id_fragment_end_distance =
            distance<string::const_iterator>(id.str_.begin(), id.fragment_end);

        const string::size_type this_scheme_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->scheme_begin);
        const string::size_type this_scheme_end_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->scheme_end);
        const string::size_type this_scheme_specific_part_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->scheme_specific_part_begin);
        const string::size_type this_scheme_specific_part_end_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->scheme_specific_part_end);
        const string::size_type this_authority_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->authority_begin);
        const string::size_type this_authority_end_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->authority_end);
        const string::size_type this_userinfo_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->userinfo_begin);
        const string::size_type this_userinfo_end_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->userinfo_end);
        const string::size_type this_host_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->host_begin);
        const string::size_type this_host_end_distance =
            distance<string::const_iterator>(this->str_.begin(), this->host_end);
        const string::size_type this_port_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->port_begin);
        const string::size_type this_port_end_distance =
            distance<string::const_iterator>(this->str_.begin(), this->port_end);
        const string::size_type this_path_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->path_begin);
        const string::size_type this_path_end_distance =
            distance<string::const_iterator>(this->str_.begin(), this->path_end);
        const string::size_type this_query_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->query_begin);
        const string::size_type this_query_end_distance =
            distance<string::const_iterator>(this->str_.begin(), this->query_end);
        const string::size_type this_fragment_begin_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->fragment_begin);
        const string::size_type this_fragment_end_distance =
            distance<string::const_iterator>(this->str_.begin(),
                                             this->fragment_end);

        swap(this->str_, id.str_);

        id.scheme_begin = id.str_.begin() + this_scheme_begin_distance;
        id.scheme_end = id.str_.begin() + this_scheme_end_distance;
        id.scheme_specific_part_begin =
            id.str_.begin() + this_scheme_specific_part_begin_distance;
        id.scheme_specific_part_end =
            id.str_.begin() + this_scheme_specific_part_end_distance;
        id.authority_begin = id.str_.begin() + this_authority_begin_distance;
        id.authority_end = id.str_.begin() + this_authority_end_distance;
        id.userinfo_begin = id.str_.begin() + this_userinfo_begin_distance;
        id.userinfo_end = id.str_.begin() + this_userinfo_end_distance;
        id.host_begin = id.str_.begin() + this_host_begin_distance;
        id.host_end = id.str_.begin() + this_host_end_distance;
        id.port_begin = id.str_.begin() + this_port_begin_distance;
        id.port_end = id.str_.begin() + this_port_end_distance;
        id.path_begin = id.str_.begin() + this_path_begin_distance;
        id.path_end = id.str_.begin() + this_path_end_distance;
        id.query_begin = id.str_.begin() + this_query_begin_distance;
        id.query_end = id.str_.begin() + this_query_end_distance;
        id.fragment_begin = id.str_.begin() + this_fragment_begin_distance;
        id.fragment_end = id.str_.begin() + this_fragment_end_distance;

        this->scheme_begin = this->str_.begin() + id_scheme_begin_distance;
        this->scheme_end = this->str_.begin() + id_scheme_end_distance;
        this->scheme_specific_part_begin =
            this->str_.begin() + id_scheme_specific_part_begin_distance;
        this->scheme_specific_part_end =
            this->str_.begin() + id_scheme_specific_part_end_distance;
        this->authority_begin =
            this->str_.begin() + id_authority_begin_distance;
        this->authority_end = this->str_.begin() + id_authority_end_distance;
        this->userinfo_begin =
            this->str_.begin() + id_userinfo_begin_distance;
        this->userinfo_end = this->str_.begin() + id_userinfo_end_distance;
        this->host_begin = this->str_.begin() + id_host_begin_distance;
        this->host_end = this->str_.begin() + id_host_end_distance;
        this->port_begin = this->str_.begin() + id_port_begin_distance;
        this->port_end = this->str_.begin() + id_port_end_distance;
        this->path_begin = this->str_.begin() + id_path_begin_distance;
        this->path_end = this->str_.begin() + id_path_end_distance;
        this->query_begin = this->str_.begin() + id_query_begin_distance;
        this->query_end = this->str_.begin() + id_query_end_distance;
        this->fragment_begin =
            this->str_.begin() + id_fragment_begin_distance;
        this->fragment_end = this->str_.begin() + id_fragment_end_distance;
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

        string abs_uri_path = absolute_uri.path();
        if (abs_uri_path.empty()) { abs_uri_path = "/"; }
        const string::size_type last_slash_index =
            abs_uri_path.find_last_of('/');

        //
        // Chop off the leading slash and the last path segment (typically a
        // file name).
        //
        ostringstream path;
        path.unsetf(ostringstream::skipws);
        path << abs_uri_path.substr(1, last_slash_index);

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
        list<string>::iterator pos = path_segments.begin();
        while (pos != path_segments.end()) {
            if (*pos != "..") { ++pos; continue; }
            //
            // If pos == begin after the first erase operation, we're
            // dealing with a bogus path.  The best we can do is Not
            // Crash.  That means we'll emit *some* absolute URI from this
            // function; but it may not be very useful.
            //
            pos = path_segments.erase(pos);
            if (pos == path_segments.begin()) { continue; }
            pos = path_segments.erase(--pos);
        }

        //
        // Reconstruct the path.
        //
        path.clear();
        path.str(string());
        for (list<string>::const_iterator pos = path_segments.begin();
             pos != path_segments.end();
             ++pos) {
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
        std::auto_ptr<openvrml::node_type_decls>
            create_node_type_desc_map() const
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


    class OPENVRML_LOCAL x3d_full_profile : public profile {
    public:
        static const char * const id;

        x3d_full_profile();
    };


    class OPENVRML_LOCAL component {
    public:
        virtual ~component() OPENVRML_NOTHROW = 0;

        virtual size_t support_level() const OPENVRML_NOTHROW = 0;
        void add_to_scope(const openvrml::browser & b,
                          openvrml::scope & scope,
                          size_t level) const
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        void add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                       size_t level) const
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);

    protected:
        static bool
            add_type_desc(openvrml::node_type_decls & type_descs,
                          const std::string & node_type_id,
                          const openvrml::node_interface_set & interfaces)
            OPENVRML_THROW1(std::bad_alloc);

        void add_scope_entry(
            const openvrml::browser & b,
            const char * node_name,
            const openvrml::node_interface_set & interface_set,
            const char * urn,
            openvrml::scope & scope) const
            OPENVRML_THROW3(openvrml::unsupported_interface,
                            std::invalid_argument, std::bad_alloc);

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc) = 0;

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc) = 0;
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

        key = x3d_full_profile::id;
        succeeded = this->insert(key, new x3d_full_profile).second;
        assert(succeeded);
    }
}

/**
 * @brief Get a @c node_type_decls map corresponding to a particular profile.
 *
 * @param[in] profile_id    a profile identifier.
 *
 * @return a @c node_type_decls map corresponding to @p profile_id.
 *
 * @exception std::invalid_argument if @p profile_id does not correspond to a
 *                                  known profile.
 * @exception std::bad_alloc        if memory allocation fails.
 */
std::auto_ptr<openvrml::node_type_decls>
openvrml::profile(const std::string & profile_id)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    try {
        return ::profile_registry_.at(profile_id).create_node_type_desc_map();
    } catch (boost::bad_ptr_container_operation &) {
        throw std::invalid_argument("unknown profile identifier \""
                                    + profile_id + '"');
    }
}

namespace {

    const class OPENVRML_LOCAL component_registry : boost::ptr_map<std::string,
                                                                   component> {
    public:
        component_registry();

        using boost::ptr_map<std::string, component>::at;
    } component_registry_;
}

/**
 * @brief Add the nodes corresponding to a component level to a
 *        @c node_type_decls map.
 *
 * @param[in,out] node_types    a @c node_type_decls map.
 * @param[in] component_id      a component identifier.
 * @param[in] level             a component level.
 *
 * @exception std::invalid_argument if @p component_id is not a recognized
 *                                  component identifier; or if @p level does
 *                                  not correspond to a supported level of
 *                                  @p component_id.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void openvrml::add_component(node_type_decls & node_types,
                             const std::string & component_id,
                             const size_t level)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    try {
        const component & comp = ::component_registry_.at(component_id);
        comp.add_to_node_type_desc_map(node_types, level);
    } catch (boost::bad_ptr_container_operation &) {
        throw std::invalid_argument("unknown component identifier \""
                                    + component_id + '"');
    }
}

namespace {
    /**
     * @internal
     *
     * @brief Create an absolute &ldquo;file&rdquo; URL from a relative path.
     *
     * This function constructs a syntactically valid &ldquo;file&rdquo; URL;
     * it does not check to see whether the file actually exists.
     *
     * @param[in] relative_uri  a relative URI.
     *
     * @return an absolute &ldquo;file&rdquo; URL corresponding to
     *         @p relative_uri.
     *
     * @exception std::runtime_error    if the current working directory
     *                                  cannot be determined.
     * @exception std::bad_alloc        if memory allocation fails.
     *
     * @sa ftp://ftp.rfc-editor.org/in-notes/rfc1738.txt
     */
    OPENVRML_LOCAL const uri create_file_url(const uri & relative_uri)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        assert(relative(relative_uri));

        using std::string;
        using std::ostringstream;

        ostringstream base_uri;
        base_uri.unsetf(ostringstream::skipws);
        base_uri << "file://";

        const string relative_path = relative_uri.path();

        if (!relative_path.empty() && relative_path[0] == '/') {
            base_uri << relative_path;
            return uri(base_uri.str());
        }

        char * getcwd_result = 0;
# ifdef _WIN32
        std::vector<char> cwd_buf(_MAX_PATH);
        //
        // Path must begin with a slash.  _getcwd returns something that
        // begins with a drive letter.
        //
        cwd_buf[0] = '/';
        while (!(getcwd_result = _getcwd(&cwd_buf[1], int(cwd_buf.size())))
               && (errno == ERANGE)) {
            cwd_buf.resize(cwd_buf.size() * 2);
        }
        std::replace_if(cwd_buf.begin() + 1,
                        cwd_buf.begin() + strlen(&cwd_buf.front()) + 1,
                        std::bind2nd(std::equal_to<char>(), '\\'), '/');
# else
        std::vector<char> cwd_buf(PATH_MAX);
        while (!(getcwd_result = getcwd(&cwd_buf.front(), cwd_buf.size()))
               && (errno == ERANGE)) {
            cwd_buf.resize(cwd_buf.size() * 2);
        }
# endif
        if (getcwd_result == 0) {
            assert(errno != 0);
            assert(errno != EFAULT);
            assert(errno != EINVAL);
            throw std::runtime_error(strerror(errno));
        }

        base_uri << &cwd_buf.front();

        //
        // Don't trust getcwd implementations not to vary on this--add a
        // trailing slash if there isn't one.
        //
        if (base_uri.str()[base_uri.str().length() - 1] != '/') {
            base_uri << '/';
        }

        const uri result = relative_uri.resolve_against(uri(base_uri.str()));
        return result;
    }
} // namespace


struct OPENVRML_LOCAL openvrml::browser::vrml97_parse_actions {
    vrml97_parse_actions(
        const std::string & uri,
        const openvrml::scene & scene,
        std::vector<boost::intrusive_ptr<openvrml::node> > & nodes):
        on_scene_start(*this),
        on_scene_finish(*this),
        on_externproto(*this),
        on_proto_start(*this),
        on_proto_interface(*this),
        on_proto_default_value_start(*this),
        on_proto_default_value_finish(*this),
        on_proto_body_start(*this),
        on_proto_finish(*this),
        on_node_start(*this),
        on_node_finish(*this),
        on_script_interface_decl(*this),
        on_route(*this),
        on_use(*this),
        on_is_mapping(*this),
        on_field_start(*this),
        on_sfnode(*this),
        on_mfnode(*this),
        on_sfbool(*this),
        on_sfcolor(*this),
        on_mfcolor(*this),
        on_sffloat(*this),
        on_mffloat(*this),
        on_sfimage(*this),
        on_sfint32(*this),
        on_mfint32(*this),
        on_sfrotation(*this),
        on_mfrotation(*this),
        on_sfstring(*this),
        on_mfstring(*this),
        on_sftime(*this),
        on_mftime(*this),
        on_sfvec2f(*this),
        on_mfvec2f(*this),
        on_sfvec3f(*this),
        on_mfvec3f(*this),
        uri_(uri),
        scene_(scene),
        nodes_(nodes)
    {}

    struct on_scene_start_t {
        explicit on_scene_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            using std::vector;
            using boost::intrusive_ptr;

            this->actions_.ps.push(parse_scope());

            const ::profile & p = ::profile_registry_.at(vrml97_profile::id);
            std::auto_ptr<scope>
                root_scope(p.create_root_scope(this->actions_.scene_.browser(),
                                               this->actions_.uri_));
            this->actions_.ps.top().scope = root_scope;
            this->actions_.ps.top().children.push(
                parse_scope::children_t());
        }

    private:
        vrml97_parse_actions & actions_;
    } on_scene_start;

    struct on_scene_finish_t {
        explicit on_scene_finish_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            using std::vector;

            assert(this->actions_.ps.top().children.size() == 1);
            this->actions_.nodes_ =
                this->actions_.ps.top().children.top();
            this->actions_.ps.top().children.pop();

            assert(this->actions_.ps.size() == 1);

            parse_scope & ps = this->actions_.ps.top();

            for (vector<route>::const_iterator r = ps.routes.begin();
                 r != ps.routes.end();
                 ++r) {
                node * const from = ps.scope->find_node(r->from);
                assert(from);
                node * const to = ps.scope->find_node(r->to);
                assert(to);
                //
                // Redundant routing is ignored (4.10.2), so we don't care if
                // add_route returns false.
                //
                add_route(*from, r->eventout, *to, r->eventin);
            }
            this->actions_.ps.pop();
        }

    private:
        vrml97_parse_actions & actions_;
    } on_scene_finish;

    struct on_externproto_t {
        explicit on_externproto_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & node_type_id,
                        const node_interface_set & interfaces,
                        const std::vector<std::string> & uri_list) const
        {
            using std::string;
            using std::vector;
            using boost::shared_ptr;

            std::cout << node_type_id << std::endl;

            //
            // If we are parsing an "anonymous" stream--that is, one passed to
            // browser::create_vrml_from_stream--we cannot use the URI
            // associated with the stream to resolve relative URIs.  So in
            // this instance, we get the base URI from browser::world_url.  If
            // browser::world_url is an empty string, we call create_file_url
            // with an empty (relative) uri.
            //
            const ::uri base_uri =
                anonymous_stream_id(::uri(this->actions_.uri_))
                ? this->actions_.scene_.browser().world_url().empty()
                    ? create_file_url(::uri())
                    : ::uri(this->actions_.scene_.browser().world_url())
                : ::uri(this->actions_.uri_);

            shared_ptr<node_type> node_type;
            for (vector<string>::const_iterator resource_id = uri_list.begin();
                 resource_id != uri_list.end();
                 ++resource_id) {
                const ::uri absolute_uri = relative(::uri(*resource_id))
                    ? ::uri(*resource_id).resolve_against(base_uri)
                    : ::uri(*resource_id);
                const shared_ptr<openvrml::node_metatype> node_metatype =
                    this->actions_.scene_.browser().node_metatype(
                        node_metatype_id(absolute_uri));
                if (node_metatype) {
                    node_type = node_metatype->create_type(node_type_id,
                                                           interfaces);
                    break;
                }
            }

            if (!node_type) {
                const std::string metatype_id =
                    path(*this->actions_.ps.top().scope) + '#'
                    + node_type_id;
                const shared_ptr<openvrml::node_metatype>
                    externproto_class(
                        new externproto_node_metatype(
                            metatype_id,
                            this->actions_.scene_,
                            uri_list,
                            this->actions_.scene_.browser()
                                .load_proto_thread_group_));

                this->actions_.scene_.browser().add_node_metatype(
                    externproto_class->id(),
                    externproto_class);
                for (vector<string>::const_iterator resource_id = uri_list.begin();
                     resource_id != uri_list.end();
                     ++resource_id) {
                    const ::uri absolute_uri = relative(::uri(*resource_id))
                        ? ::uri(*resource_id).resolve_against(base_uri)
                        : ::uri(*resource_id);
                    this->actions_.scene_.browser()
                        .add_node_metatype(node_metatype_id(absolute_uri),
                                           externproto_class);
                }

                node_type = externproto_class->create_type(node_type_id,
                                                           interfaces);
            }

            assert(node_type);

            const bool succeeded =
                this->actions_.ps.top().scope->add_type(node_type)
                .second;
            assert(succeeded);
        }

    private:
        vrml97_parse_actions & actions_;
    } on_externproto;

    struct on_proto_start_t {
        explicit on_proto_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & node_type_id) const
        {
            const boost::shared_ptr<scope> outer_scope =
                this->actions_.ps.top().scope;
            assert(outer_scope);

            this->actions_.ps.push(parse_scope());

            parse_scope & ps = this->actions_.ps.top();

            ps.proto_node_type_id = node_type_id;

            //
            // We don't want to create the new scope until we start the PROTO
            // body.  So, for now we set the parse_scope::scope to the outer
            // scope.
            //
            ps.scope = outer_scope;

            //
            // We push a node_data onto the stack to hold any field default
            // values.
            //
            ps.node_data_.push(node_data());
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_start;

    struct on_proto_interface_t {
        explicit on_proto_interface_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const node_interface & interface_) const
        {
            parse_scope & ps = this->actions_.ps.top();

            const bool succeeded =
                ps.proto_interfaces.insert(interface_).second;
            assert(succeeded);

            assert(!ps.node_data_.empty());
            node_data & nd = ps.node_data_.top();

            if (interface_.type == node_interface::field_id
                || interface_.type == node_interface::exposedfield_id) {
                using std::auto_ptr;
                using std::pair;
                auto_ptr<field_value> value =
                    field_value::create(interface_.field_type);
                pair<initial_value_map::iterator, bool> insert_result =
                    nd.initial_values.insert(
                        make_pair(interface_.id,
                                  boost::shared_ptr<field_value>(value)));
                assert(insert_result.second);
                nd.current_field_value = &(*insert_result.first);
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_interface;

    struct on_proto_default_value_start_t {
        explicit on_proto_default_value_start_t(
            vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            parse_scope & ps = this->actions_.ps.top();

            boost::shared_ptr<scope> outer_scope = ps.scope;
            const std::string & proto_node_type_id = ps.proto_node_type_id;
            assert(!proto_node_type_id.empty());
            const std::string & field_id =
                ps.node_data_.top().current_field_value->first;
            const field_value::type_id field_type =
                ps.node_data_.top().current_field_value->second->type();

            if (field_type == field_value::sfnode_id
                || field_type == field_value::mfnode_id) {
                using std::vector;
                using boost::intrusive_ptr;

                const node_data old_nd = ps.node_data_.top();

                const std::string scope_id =
                    proto_node_type_id + '.' + field_id;
                actions_.ps.push(parse_scope());

                parse_scope & new_ps = this->actions_.ps.top();

                new_ps.scope.reset(new scope(scope_id, outer_scope));
                new_ps.node_data_.push(old_nd);
                new_ps.children.push(parse_scope::children_t());
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_default_value_start;

    struct on_proto_default_value_finish_t {
        explicit on_proto_default_value_finish_t(
            vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            assert(!actions_.ps.empty());

            parse_scope & ps = this->actions_.ps.top();

            const field_value::type_id field_type =
                ps.node_data_.top().current_field_value->second->type();

            if (field_type == field_value::sfnode_id
                || field_type == field_value::mfnode_id) {
                using std::vector;

                assert(actions_.ps.size() > 1);

                for (vector<route>::const_iterator r = ps.routes.begin();
                     r != ps.routes.end();
                     ++r) {
                    node * const from = ps.scope->find_node(r->from);
                    assert(from);
                    node * const to = ps.scope->find_node(r->to);
                    assert(to);
                    //
                    // Redundant routing is ignored (4.10.2), so we don't care
                    // if add_route returns false.
                    //
                    add_route(*from, r->eventout, *to, r->eventin);
                }

                parse_scope old_ps = ps;
                this->actions_.ps.pop();
                parse_scope & new_ps = this->actions_.ps.top();
                new_ps.node_data_.top().current_field_value->second->assign(
                    *old_ps.node_data_.top().current_field_value->second);
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_default_value_finish;

    struct on_proto_body_start_t {
        explicit on_proto_body_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            assert(!actions_.ps.empty());
            const boost::shared_ptr<scope> parent =
                actions_.ps.top().scope;
            actions_.ps.top().scope
                .reset(new scope(actions_.ps.top().proto_node_type_id,
                                 parent));
            actions_.ps.top().proto_default_values =
                actions_.ps.top().node_data_.top().initial_values;
            actions_.ps.top().node_data_.pop();
            assert(actions_.ps.top().children.empty());
            actions_.ps.top().children.push(
                parse_scope::children_t());
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_body_start;

    struct on_proto_finish_t {
        explicit on_proto_finish_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            using std::vector;
            using boost::shared_ptr;
            using boost::dynamic_pointer_cast;
            parse_scope & ps = this->actions_.ps.top();
            assert(ps.children.size() == 1);
            assert(ps.node_data_.empty());
            proto_node_metatype::routes_t proto_routes;
            for (vector<route>::const_iterator r = ps.routes.begin();
                 r != ps.routes.end();
                 ++r) {
                node * const from = ps.scope->find_node(r->from);
                assert(from);
                node * const to = ps.scope->find_node(r->to);
                assert(to);
                proto_routes.push_back(
                    proto_node_metatype::route(*from, r->eventout,
                                               *to, r->eventin));
            }
            const shared_ptr<openvrml::node_metatype> node_metatype(
                new proto_node_metatype(
                    path(*ps.scope),
                    this->actions_.scene_.browser(),
                    ps.proto_interfaces,
                    ps.proto_default_values,
                    ps.children.top(),
                    ps.proto_is_map,
                    proto_routes));

            //
            // Add the new node_metatype (prototype definition) to the
            // browser's node_metatype_map.
            //
            this->actions_.scene_.browser()
                .add_node_metatype(node_metatype->id(), node_metatype);

            if (!dynamic_pointer_cast<proto_node_metatype>(
                    this->actions_.scene_.browser().node_metatype(
                        node_metatype_id(this->actions_.uri_)))) {
                this->actions_.scene_.browser()
                    .add_node_metatype(node_metatype_id(this->actions_.uri_),
                                       node_metatype);
            }

            //
            // PROTOs implicitly introduce a new node type as well.
            //
            const shared_ptr<node_type> node_type =
                node_metatype->create_type(
                    ps.proto_node_type_id,
                    this->actions_.ps.top().proto_interfaces);
            assert(node_type);

            //
            // We want to add the type to the PROTO's enclosing scope, so pop
            // off the current scope.
            //
            assert(this->actions_.ps.size() > 1);
            this->actions_.ps.pop();

            const bool succeeded =
                this->actions_.ps.top().scope->add_type(node_type)
                .second;
            assert(succeeded);
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_finish;

    struct on_node_start_t {
        explicit on_node_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & node_name_id,
                        const std::string & node_type_id) const
        {
            parse_scope & ps = this->actions_.ps.top();

            ps.node_data_.push(node_data());
            ps.node_data_.top().node_name_id = node_name_id;

            assert(ps.scope);
            if (node_type_id != "Script") {
                ps.node_data_.top().type = ps.scope->find_type(node_type_id);
                assert(ps.node_data_.top().type);
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_node_start;

    struct on_node_finish_t {
        explicit on_node_finish_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            using boost::intrusive_ptr;

            parse_scope & ps = this->actions_.ps.top();

            assert(!ps.children.empty());
            assert(!ps.node_data_.empty());

            const node_data & nd = ps.node_data_.top();

            const intrusive_ptr<node> n = nd.type
                ? nd.type->create_node(ps.scope, nd.initial_values)
                : intrusive_ptr<node>(
                    new script_node(
                        this->actions_.scene_.browser().script_node_metatype_,
                        ps.scope,
                        nd.script_interfaces,
                        nd.initial_values));

            if (!nd.node_name_id.empty()) { n->id(nd.node_name_id); }

            for (node_data::is_map_t::const_iterator entry = nd.is_map.begin();
                 entry != nd.is_map.end();
                 ++entry) {
                ps.proto_is_map.insert(
                    make_pair(entry->second,
                              proto_node_metatype::is_target(
                                  *n, entry->first)));
            }

            ps.children.top().push_back(n);
            ps.node_data_.pop();
        }

    private:
        vrml97_parse_actions & actions_;
    } on_node_finish;

    struct on_script_interface_decl_t {
        explicit on_script_interface_decl_t(
            vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const node_interface & interface_) const
        {
            parse_scope & ps = this->actions_.ps.top();

            assert(!ps.node_data_.empty());

            node_data & nd = ps.node_data_.top();

            const bool succeeded =
                nd.script_interfaces.insert(interface_).second;
            assert(succeeded);

            this->actions_.on_field_start(interface_.id, interface_.field_type);
            if ((interface_.type == node_interface::eventin_id
                 || interface_.type == node_interface::eventout_id)
                && (interface_.field_type == field_value::sfnode_id
                    || interface_.field_type == field_value::mfnode_id)) {
                ps.children.pop();
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_script_interface_decl;

    struct on_route_t {
        explicit on_route_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & from_node_name_id,
                        const node_interface & from_node_interface,
                        const std::string & to_node_name_id,
                        const node_interface & to_node_interface) const
        {
            parse_scope & ps = this->actions_.ps.top();
            const route r(from_node_name_id, from_node_interface.id,
                          to_node_name_id, to_node_interface.id);
            ps.routes.push_back(r);
        }

    private:
        vrml97_parse_actions & actions_;
    } on_route;

    struct on_use_t {
        explicit on_use_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & node_name_id) const
        {
            using boost::intrusive_ptr;
            parse_scope & ps = this->actions_.ps.top();
            node_data & nd = ps.node_data_.top();

            //
            // Check for self-reference.  This is legitimate in Script nodes.
            //
            node * const n = (node_name_id == nd.node_name_id)
                           ? node::self_tag.get()
                           : ps.scope->find_node(node_name_id);
            assert(n);
            assert(!ps.children.empty());
            ps.children.top().push_back(intrusive_ptr<node>(n));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_use;

    struct on_is_mapping_t {
        explicit on_is_mapping_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & proto_interface_id) const
        {
            node_data & nd =
                this->actions_.ps.top().node_data_.top();
            assert(nd.current_field_value);
            nd.is_map.insert(make_pair(nd.current_field_value->first,
                                       proto_interface_id));

            const field_value::type_id field_type =
                nd.current_field_value->second->type();
            if (field_type == field_value::sfnode_id
                || field_type == field_value::mfnode_id) {
                this->actions_.ps.top().children.pop();
            }

            //
            // We need to leave this around for Script nodes.
            //
            if (nd.type) {
                const initial_value_map::size_type erased =
                    nd.initial_values.erase(nd.current_field_value->first);
                assert(erased != 0);
            }
            nd.current_field_value = 0;
        }

    private:
        vrml97_parse_actions & actions_;
    } on_is_mapping;

    struct on_field_start_t {
        explicit on_field_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & field_name_id,
                        const field_value::type_id field_type) const
        {
            parse_scope & ps = this->actions_.ps.top();
            node_data & nd = ps.node_data_.top();

            using boost::shared_ptr;
            using std::pair;
            std::auto_ptr<field_value> value = field_value::create(field_type);
            pair<initial_value_map::iterator, bool> insert_result =
                nd.initial_values.insert(
                    make_pair(field_name_id, shared_ptr<field_value>(value)));
            assert(insert_result.second);
            nd.current_field_value = &(*insert_result.first);

            if (field_type == field_value::sfnode_id
                || field_type == field_value::mfnode_id) {
                ps.children.push(parse_scope::children_t());
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_field_start;

    struct on_sfnode_t {
        explicit on_sfnode_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(bool null) const
        {
            assert(!actions_.ps.empty());
            parse_scope & ps = this->actions_.ps.top();

            if (!null) {
                assert(!ps.node_data_.empty());
                assert(!ps.children.top().empty());
                ps.node_data_.top()
                    .current_field_value->second->assign(
                        sfnode(ps.children.top().front()));
            }
            actions_.ps.top().children.pop();
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfnode;

    struct on_mfnode_t {
        explicit on_mfnode_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            assert(!this->actions_.ps.empty());
            assert(!this->actions_.ps.top().node_data_.empty());
            assert(!this->actions_.ps.top().children.empty());
            this->actions_.ps.top().node_data_.top()
                .current_field_value->second
                ->assign(mfnode(this->actions_.ps.top().children.top()));
            this->actions_.ps.top().children.pop();
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfnode;

    struct on_sfbool_t {
        explicit on_sfbool_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const bool val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfbool(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfbool;

    struct on_sfcolor_t {
        explicit on_sfcolor_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const color & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfcolor(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfcolor;

    struct on_mfcolor_t {
        explicit on_mfcolor_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<color> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfcolor(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfcolor;

    struct on_sffloat_t {
        explicit on_sffloat_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const float val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sffloat(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sffloat;

    struct on_mffloat_t {
        explicit on_mffloat_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<float> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mffloat(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mffloat;

    struct on_sfimage_t {
        explicit on_sfimage_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const image & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfimage(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfimage;

    struct on_sfint32_t {
        explicit on_sfint32_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const int32 val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfint32(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfint32;

    struct on_mfint32_t {
        explicit on_mfint32_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<int32> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfint32(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfint32;

    struct on_sfrotation_t {
        explicit on_sfrotation_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const rotation & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfrotation(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfrotation;

    struct on_mfrotation_t {
        explicit on_mfrotation_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<rotation> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfrotation(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfrotation;

    struct on_sfstring_t {
        explicit on_sfstring_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfstring(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfstring;

    struct on_mfstring_t {
        explicit on_mfstring_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<std::string> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfstring(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfstring;

    struct on_sftime_t {
        explicit on_sftime_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const double val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sftime(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sftime;

    struct on_mftime_t {
        explicit on_mftime_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<double> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mftime(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mftime;

    struct on_sfvec2f_t {
        explicit on_sfvec2f_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const vec2f & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfvec2f(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfvec2f;

    struct on_mfvec2f_t {
        explicit on_mfvec2f_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<vec2f> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfvec2f(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfvec2f;

    struct on_sfvec3f_t {
        explicit on_sfvec3f_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const vec3f & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfvec3f(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfvec3f;

    struct on_mfvec3f_t {
        explicit on_mfvec3f_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<vec3f> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfvec3f(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfvec3f;

    struct route {
        std::string from;
        std::string eventout;
        std::string to;
        std::string eventin;

        route(const std::string & from, const std::string & eventout,
              const std::string & to, const std::string & eventin):
            from(from),
            eventout(eventout),
            to(to),
            eventin(eventin)
        {}
    };

    struct node_data {
        //
        // This is a multimap because an exposedField might have multiple
        // entries (i.e., for the corresponding eventIn, field, and eventOut).
        //
        typedef std::multimap<std::string, std::string> is_map_t;

        boost::shared_ptr<node_type> type;
        std::string node_name_id;
        initial_value_map initial_values;
        initial_value_map::value_type * current_field_value;
        node_interface_set script_interfaces;
        is_map_t is_map;

        node_data():
            current_field_value(0)
        {}

        node_data(const node_data & nd):
            type(nd.type),
            node_name_id(nd.node_name_id),
            initial_values(nd.initial_values),
            current_field_value(0),
            script_interfaces(nd.script_interfaces),
            is_map(nd.is_map)
        {
            if (nd.current_field_value) {
                initial_value_map::iterator pos =
                    this->initial_values.find(nd.current_field_value->first);
                assert(pos != this->initial_values.end());
                this->current_field_value = &(*pos);
            }
        }

        node_data & operator=(const node_data & nd)
        {
            node_data temp(nd);
            this->swap(temp);
            return *this;
        }

        void swap(node_data & nd) OPENVRML_NOTHROW
        {
            this->type.swap(nd.type);
            this->node_name_id.swap(nd.node_name_id);
            this->initial_values.swap(nd.initial_values);
            std::swap(this->current_field_value, nd.current_field_value);
            this->script_interfaces.swap(nd.script_interfaces);
            this->is_map.swap(nd.is_map);
        }
    };

    struct parse_scope {
        typedef std::vector<boost::intrusive_ptr<node> > children_t;

        std::string proto_node_type_id;
        boost::shared_ptr<openvrml::scope> scope;
        node_interface_set proto_interfaces;
        proto_node_metatype::default_value_map_t proto_default_values;
        proto_node_metatype::is_map_t proto_is_map;
        std::vector<route> routes;
        std::stack<children_t> children;
        std::stack<node_data> node_data_;
    };

    //
    // We push a parse_scope onto the stack
    // * at the scene root
    std::stack<parse_scope> ps;

private:
    const std::string uri_;
    const openvrml::scene & scene_;
    std::vector<boost::intrusive_ptr<openvrml::node> > & nodes_;
};

struct OPENVRML_LOCAL openvrml::browser::x3d_vrml_parse_actions :
    openvrml::browser::vrml97_parse_actions {

    x3d_vrml_parse_actions(
        const std::string & uri,
        const openvrml::scene & scene,
        std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
        std::map<std::string, std::string> & meta):
        openvrml::browser::vrml97_parse_actions(uri, scene, nodes),
        on_profile_statement(*this),
        on_component_statement(*this),
        on_meta_statement(*this),
        on_sfcolorrgba(*this),
        on_sfdouble(*this),
        on_sfvec2d(*this),
        on_sfvec3d(*this),
        on_mfbool(*this),
        on_mfcolorrgba(*this),
        on_mfimage(*this),
        on_mfvec2d(*this),
        on_mfvec3d(*this),
        meta_(meta)
    {}

    struct on_profile_statement_t {
        explicit on_profile_statement_t(x3d_vrml_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & /* profile_id */) const
        {}

    private:
        x3d_vrml_parse_actions & actions_;
    } on_profile_statement;

    struct on_component_statement_t {
        explicit on_component_statement_t(x3d_vrml_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & /* component_id */,
                        int32 /* level */) const
        {}

    private:
        x3d_vrml_parse_actions & actions_;
    } on_component_statement;

    struct on_meta_statement_t {
        explicit on_meta_statement_t(x3d_vrml_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & name,
                        const std::string & value) const
        {
            this->actions_.meta_[name] = value;
        }

    private:
        x3d_vrml_parse_actions & actions_;
    } on_meta_statement;

    struct on_import_statement_t {
        void operator()(const std::string & /* inline_node_name_id */,
                        const std::string & /* exported_node_name_id */,
                        const std::string & /* imported_node_name_id */) const
        {}
    } on_import_statement;

    struct on_export_statement_t {
        void operator()(const std::string & /* node_name_id */,
                        const std::string & /* exported_node_name_id */) const
        {}
    } on_export_statement;

    struct on_sfcolorrgba_t {
        explicit on_sfcolorrgba_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const color_rgba & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfcolorrgba(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfcolorrgba;

    struct on_sfdouble_t {
        explicit on_sfdouble_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const double val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfdouble(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfdouble;

    struct on_sfvec2d_t {
        explicit on_sfvec2d_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const vec2d & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfvec2d(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfvec2d;

    struct on_sfvec3d_t {
        explicit on_sfvec3d_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const vec3d & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfvec3d(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfvec3d;

    struct on_mfbool_t {
        explicit on_mfbool_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<bool> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfbool(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfbool;

    struct on_mfcolorrgba_t {
        explicit on_mfcolorrgba_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<color_rgba> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfcolorrgba(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfcolorrgba;

    struct on_mfimage_t {
        explicit on_mfimage_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<image> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfimage(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfimage;

    struct on_mfvec2d_t {
        explicit on_mfvec2d_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<vec2d> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfvec2d(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfvec2d;

    struct on_mfvec3d_t {
        explicit on_mfvec3d_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<vec3d> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfvec3d(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfvec3d;

private:
    std::map<std::string, std::string> & meta_;
};

namespace {

    struct OPENVRML_LOCAL parse_error {
        size_t line, column;
        std::string message;

        parse_error(): line(0), column(0) {}
    };

    struct OPENVRML_LOCAL error_handler {
        error_handler(openvrml::browser & b, parse_error & error):
            browser_(b),
            error_(error)
        {}

        template <typename ScannerT, typename ErrorT>
        boost::spirit::error_status<> operator()(const ScannerT & scan,
                                                 const ErrorT & err) const
        {
            using std::endl;
            using std::string;
            using boost::lexical_cast;
            using boost::spirit::error_status;
            using boost::spirit::file_position;

            const file_position fpos = err.where.get_position();

            //
            // Warnings we want to spew directly to the browser.
            //
            if (   err.descriptor == openvrml::rotation_axis_not_normalized
                || err.descriptor == openvrml::eventin_deprecated
                || err.descriptor == openvrml::eventout_deprecated
                || err.descriptor == openvrml::exposedfield_deprecated
                || err.descriptor == openvrml::field_deprecated) {
                std::ostringstream warn;
                warn << fpos.file << ':' << fpos.line << ':' << fpos.column
                     << ": warning: "
                     << x3d_vrml_parse_error_msg(err.descriptor);
                this->browser_.err(warn.str());
                scan.first = err.where;
                return error_status<>(error_status<>::accept, 0);
            }

            //
            // Errors presumably halt parsing and result in a failure.  We
            // need enough information to create an invalid_vrml exception in
            // parse_vrml.
            //
            this->error_.line = fpos.line;
            this->error_.column = fpos.column;
            this->error_.message = x3d_vrml_parse_error_msg(err.descriptor);

            return error_status<>(error_status<>::fail);
        }

    private:
        openvrml::browser & browser_;
        parse_error & error_;
    };
}

/**
 * @internal
 *
 * @brief Parse a VRML stream.
 *
 * @param[in,out] in    input stream.
 * @param[in]     uri   URI associated with @p in.
 * @param[in]     type  MIME media type of the data to be read from @p in.
 * @param[in]     scene a @c scene.
 * @param[out]    nodes the root @c node%s.
 * @param[out]    meta  the @c scene metadata.
 *
 * @exception openvrml::bad_media_type
 * @exception openvrml::invalid_vrml
 */
OPENVRML_LOCAL void
openvrml::browser::
parse_vrml(std::istream & in,
           const std::string & uri,
           const std::string & type,
           const openvrml::scene & scene,
           std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
           std::map<std::string, std::string> & meta)
{
    using std::istream;
    using std::istreambuf_iterator;
    using boost::algorithm::iequals;
    using boost::spirit::multi_pass;
    using boost::spirit::make_multi_pass;
    using boost::spirit::position_iterator;

    typedef multi_pass<istreambuf_iterator<char> > multi_pass_iterator_t;
    typedef istream::char_type char_t;

    vrml97_skip_grammar skip_g;

    if (iequals(type, vrml_media_type) || iequals(type, x_vrml_media_type)) {
        multi_pass_iterator_t
            in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
            in_end(make_multi_pass(istreambuf_iterator<char_t>()));

        typedef position_iterator<multi_pass_iterator_t> iterator_t;

        iterator_t first(in_begin, in_end, uri), last;

        parse_error error;
        error_handler handler(scene.browser(), error);
        vrml97_parse_actions actions(uri, scene, nodes);
        vrml97_grammar<vrml97_parse_actions, error_handler>
            g(actions, handler);

        BOOST_SPIRIT_DEBUG_NODE(skip_g);
        BOOST_SPIRIT_DEBUG_NODE(g);

        if (!parse(first, last, g, skip_g).full) {
            throw openvrml::invalid_vrml(uri,
                                         error.line,
                                         error.column,
                                         error.message);
        }
    } else if (iequals(type, x3d_vrml_media_type)) {
        multi_pass_iterator_t
            in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
            in_end(make_multi_pass(istreambuf_iterator<char_t>()));

        typedef position_iterator<multi_pass_iterator_t> iterator_t;

        iterator_t first(in_begin, in_end, uri), last;

        parse_error error;
        error_handler handler(scene.browser(), error);
        x3d_vrml_parse_actions actions(uri, scene, nodes, meta);
        x3d_vrml_grammar<x3d_vrml_parse_actions, error_handler>
            g(actions, handler);

        BOOST_SPIRIT_DEBUG_NODE(skip_g);
        BOOST_SPIRIT_DEBUG_NODE(g);

        if (!parse(first, last, g, skip_g).full) {
            throw openvrml::invalid_vrml(uri,
                                         error.line,
                                         error.column,
                                         error.message);
        }
    } else {
        throw bad_media_type(type);
    }
}

struct OPENVRML_LOCAL openvrml::browser::externproto_node_metatype::load_proto {
    load_proto(externproto_node_metatype & externproto_class,
               const openvrml::scene & scene,
               const std::vector<std::string> & alt_uris):
        externproto_node_metatype_(&externproto_class),
        scene_(&scene),
        alt_uris_(alt_uris)
    {}

    void operator()() const OPENVRML_NOTHROW
    {
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
                        *this->externproto_node_metatype_,
                        &externproto_node_metatype::clear_externproto_node_types);
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

                shared_ptr<openvrml::proto_node_metatype>
                    proto_node_metatype;
                for (vector<string>::const_iterator alt_uri =
                         this->alt_uris_.begin();
                     (alt_uri != this->alt_uris_.end())
                         && !proto_node_metatype;
                     ++alt_uri) {
                    const uri absolute_uri = !relative(uri(*alt_uri))
                        ? uri(*alt_uri)
                        : this->scene_->url().empty()
                            ? create_file_url(uri(*alt_uri))
                            : uri(*alt_uri).resolve_against(
                                uri(this->scene_->url()));

                    const shared_ptr<openvrml::node_metatype> node_metatype =
                        this->scene_->browser().node_metatype(
                            node_metatype_id(absolute_uri));

                    proto_node_metatype =
                        dynamic_pointer_cast<openvrml::proto_node_metatype>(
                            node_metatype);
                }

                if (!proto_node_metatype) {
                    ostringstream err_msg;
                    err_msg << "no PROTO definition at <" << in->url()
                            << ">";
                    this->scene_->browser().err(err_msg.str());
                    return;
                }

                this->externproto_node_metatype_
                    ->set_proto_node_metatype(proto_node_metatype);

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
    }

private:
    externproto_node_metatype * externproto_node_metatype_;
    const openvrml::scene * scene_;
    std::vector<std::string> alt_uris_;
};

/**
 * @brief Construct.
 *
 * @param[in] id                            the @c node_metatype_id for the
 *                                          @c EXTERNPROTO.
 * @param[in] scene                         the @c scene in which the
 *                                          @c EXTERNPROTO occurs.
 * @param[in] uris                          the list of alternative
 *                                          implementation identifiers.
 * @param[in,out] load_proto_thread_group   the thread group used for loading
 *                                          @c EXTERNPROTO implementations.
 *
 * @exception boost::thread_resource_error  if a new thread of execution
 *                                          cannot be started.
 * @exception std::bad_alloc                if memory allocation fails.
 */
openvrml::browser::externproto_node_metatype::
externproto_node_metatype(const openvrml::node_metatype_id & id,
                          const openvrml::scene & scene,
                          const std::vector<std::string> & uris,
                          boost::thread_group & load_proto_thread_group)
    OPENVRML_THROW2(boost::thread_resource_error, std::bad_alloc):
    node_metatype(id, scene.browser()),
                  externproto_node_types_cleared_(false),
                  load_proto_thread_(
                      load_proto_thread_group.create_thread(
                          boost::function0<void>(
                              load_proto(*this, scene, uris))))
{}

/**
 * @brief Destroy.
 */
openvrml::browser::externproto_node_metatype::~externproto_node_metatype()
    OPENVRML_NOTHROW
{}

const boost::shared_ptr<openvrml::node_type>
openvrml::browser::externproto_node_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    using boost::shared_ptr;

    shared_ptr<openvrml::proto_node_metatype> shared;
    if ((shared = this->proto_node_metatype_.lock())) {
        return shared->create_type(id, interfaces);
    }

    const shared_ptr<externproto_node_type> node_type(
        new externproto_node_type(this->shared_from_this(),
                                  id,
                                  interfaces));

    this->externproto_node_types_.push_back(node_type);

    return node_type;
}

void openvrml::browser::externproto_node_metatype::do_shutdown(double)
    OPENVRML_NOTHROW
{
    this->load_proto_thread_->join();
}

void
openvrml::browser::externproto_node_metatype::
set_proto_node_metatype(
    const boost::weak_ptr<openvrml::proto_node_metatype> & proto_node_metatype)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    using boost::shared_ptr;
    using boost::static_pointer_cast;

    this->proto_node_metatype_ = proto_node_metatype;

    //
    // Now that we have a proto_node_metatype, we need to tell all the
    // externproto_node_types we've created so that they can in turn
    // tell the externproto_nodes they've created.
    //
    const shared_ptr<openvrml::proto_node_metatype> shared_proto_node_metatype =
        proto_node_metatype.lock();
    assert(shared_proto_node_metatype);
    for (externproto_node_types::const_iterator node_type =
             this->externproto_node_types_.begin();
         node_type != this->externproto_node_types_.end();
         ++node_type) {
        assert(!node_type->expired());
        node_type->lock()->set_proto_node_type(*shared_proto_node_metatype);
    }
}

void openvrml::browser::externproto_node_metatype::clear_externproto_node_types()
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


openvrml::browser::externproto_node_type::externproto_node_type(
    const boost::shared_ptr<const externproto_node_metatype> & c,
    const std::string & id,
    const openvrml::node_interface_set & interfaces)
    OPENVRML_THROW1(std::bad_alloc):
    node_type(*c, id),
    node_metatype_(c),
    interfaces_(interfaces)
{}

openvrml::browser::externproto_node_type::~externproto_node_type()
    OPENVRML_NOTHROW
{}

void
openvrml::browser::externproto_node_type::
set_proto_node_type(openvrml::proto_node_metatype & proto_node_metatype)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    assert(!this->proto_node_type_);

    using boost::static_pointer_cast;

    this->proto_node_type_ =
        static_pointer_cast<openvrml::proto_node_type>(
            proto_node_metatype.create_type(this->id(), this->interfaces_));

    for (externproto_nodes::const_iterator node =
             this->externproto_nodes_.begin();
         node != this->externproto_nodes_.end();
         ++node) {
        (*node)->set_proto_node(*this->proto_node_type_);
    }
}

void openvrml::browser::externproto_node_type::clear_externproto_nodes()
    OPENVRML_NOTHROW
{
    boost::mutex::scoped_lock lock(this->mutex_);
    this->externproto_nodes_.clear();
}

const openvrml::node_interface_set &
openvrml::browser::externproto_node_type::do_interfaces() const throw()
{
    return this->interfaces_;
}

const boost::intrusive_ptr<openvrml::node>
openvrml::browser::externproto_node_type::do_create_node(
    const boost::shared_ptr<openvrml::scope> & scope,
    const openvrml::initial_value_map & initial_values) const
    OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                    std::bad_alloc)
{
    //
    // externproto_node_metatype::mutex_ must be locked first.
    // externproto_node_metatype::clear_externproto_node_types must not be
    // initiated while we're doing this.
    // externproto_node_metatype::externproto_node_types_cleared_ is checked
    // later in this function and attempting to lock
    // externproto_node_metatype::mutex_ at that point creates a race
    // condition.
    //
    boost::mutex::scoped_lock
        lock_externproto_node_metatype(
            static_cast<const externproto_node_metatype &>(this->metatype())
            .mutex_),
        lock(this->mutex_);

    if (this->proto_node_type_) {
        return this->proto_node_type_->create_node(scope, initial_values);
    }

    const boost::intrusive_ptr<externproto_node> result(
        new externproto_node(*this, scope, initial_values));

    const externproto_node_metatype & node_metatype =
        static_cast<const externproto_node_metatype &>(this->metatype());
    if (!node_metatype.externproto_node_types_cleared_) {
        this->externproto_nodes_.push_back(result);
    } else {
        assert(this->externproto_nodes_.empty());
    }

    return result;
}


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
    static_cast<externproto_node &>(this->node()).received_event_.insert(this);
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

openvrml::externproto_node::
externproto_node(const browser::externproto_node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const initial_value_map & initial_values)
    OPENVRML_THROW1(std::bad_alloc):
    abstract_proto_node(type, scope),
    initial_values_(initial_values)
{
    for (node_interface_set::const_iterator interface_ =
             type.interfaces().begin();
         interface_ != type.interfaces().end();
         ++interface_)
    {
        using boost::shared_ptr;
        using boost::dynamic_pointer_cast;

        bool succeeded = false;
        std::auto_ptr<openvrml::field_value> field_auto_ptr;
        shared_ptr<openvrml::event_listener> interface_eventin;
        shared_ptr<openvrml::event_emitter> interface_eventout;
        shared_ptr<openvrml::field_value> interface_field;
        switch (interface_->type) {
        case node_interface::eventin_id:
            succeeded = this->eventin_map
                .insert(make_pair(interface_->id,
                                  create_eventin(interface_->field_type,
                                                 *this)))
                .second;
            break;
        case node_interface::eventout_id:
            succeeded = this->eventout_map
                .insert(make_pair(interface_->id,
                                  create_eventout(interface_->field_type,
                                                  *this)))
                .second;
            break;
        case node_interface::exposedfield_id:
            interface_field = create_exposedfield(*this,
                                                  interface_->field_type);
            succeeded = this->field_map_
                .insert(make_pair(interface_->id, interface_field)).second;
            assert(succeeded);

            interface_eventin =
                dynamic_pointer_cast<openvrml::event_listener>(
                    interface_field);
            succeeded = this->eventin_map
                .insert(make_pair("set_" + interface_->id,
                                  interface_eventin))
                .second;
            assert(succeeded);

            interface_eventout =
                dynamic_pointer_cast<openvrml::event_emitter>(
                    interface_eventin);
            succeeded = this->eventout_map
                .insert(make_pair(interface_->id + "_changed",
                                  interface_eventout))
                .second;
            break;
        case node_interface::field_id:
            field_auto_ptr = field_value::create(interface_->field_type);
            succeeded = this->field_map_
                .insert(
                    make_pair(interface_->id,
                              shared_ptr<field_value>(field_auto_ptr)))
                .second;
            break;
        case node_interface::invalid_type_id:
            assert(false
                   && "got node_interface::invalid_type_id for "
                   "interface_->type");
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
    using boost::static_pointer_cast;

    //
    // Any exposedFields that received events add to/override what's in the
    // initial_values_ map.
    //
    for (field_map::const_iterator field = this->field_map_.begin();
         field != this->field_map_.end();
         ++field) {
        std::set<openvrml::field_value *>::const_iterator pos =
            this->received_event_.find(field->second.get());
        if (pos != this->received_event_.end()) {
            this->initial_values_[field->first] = field->second;
        }
    }
    this->received_event_.clear();

    this->proto_node_ =
        static_pointer_cast<proto_node>(
            node_type.create_node(this->scope_, this->initial_values_));
    this->initial_values_.clear(); // No longer need these.

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
 * @brief Cast to a @c script_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c script_node, or 0 otherwise.
 */
openvrml::script_node * openvrml::externproto_node::to_script()
    OPENVRML_NOTHROW
{
    return node_cast<script_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to an @c appearance_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is an @c appearance_node, or 0 otherwise.
 */
openvrml::appearance_node *
openvrml::externproto_node::to_appearance() OPENVRML_NOTHROW
{
    return node_cast<appearance_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c bounded_volume_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c bounded_volume_node, or 0 otherwise.
 */
openvrml::bounded_volume_node *
openvrml::externproto_node::to_bounded_volume()
    OPENVRML_NOTHROW
{
    return node_cast<bounded_volume_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c child_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c child_node, or 0 otherwise.
 */
openvrml::child_node *
openvrml::externproto_node::to_child() OPENVRML_NOTHROW
{
    return node_cast<child_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c color_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c color_node, or 0 otherwise.
 */
openvrml::color_node *
openvrml::externproto_node::to_color() OPENVRML_NOTHROW
{
    return node_cast<color_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c color_rgba_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c color_rgba_node, or 0 otherwise.
 */
openvrml::color_rgba_node *
openvrml::externproto_node::to_color_rgba()
    OPENVRML_NOTHROW
{
    return node_cast<color_rgba_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c coordinate_node, or 0 otherwise.
 */
openvrml::coordinate_node *
openvrml::externproto_node::to_coordinate() OPENVRML_NOTHROW
{
    return node_cast<coordinate_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c font_style_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c font_style_node, or 0 otherwise.
 */
openvrml::font_style_node *
openvrml::externproto_node::to_font_style() OPENVRML_NOTHROW
{
    return node_cast<font_style_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c geometry_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c geometry_node, or 0 otherwise.
 */
openvrml::geometry_node * openvrml::externproto_node::to_geometry()
    OPENVRML_NOTHROW
{
    return node_cast<geometry_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c grouping_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c grouping_node, or 0 otherwise.
 */
openvrml::grouping_node * openvrml::externproto_node::to_grouping()
    OPENVRML_NOTHROW
{
    return node_cast<grouping_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c light_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c light_node, or 0 otherwise.
 */
openvrml::light_node * openvrml::externproto_node::to_light()
    OPENVRML_NOTHROW
{
    return node_cast<light_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c material_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c material_node, or 0 otherwise.
 */
openvrml::material_node * openvrml::externproto_node::to_material()
    OPENVRML_NOTHROW
{
    return node_cast<material_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c navigation_info_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c navigation_info_node, or 0 otherwise.
 */
openvrml::navigation_info_node *
openvrml::externproto_node::to_navigation_info()
    OPENVRML_NOTHROW
{
    return node_cast<navigation_info_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c normal_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c normal_node, or 0 otherwise.
 */
openvrml::normal_node * openvrml::externproto_node::to_normal()
    OPENVRML_NOTHROW
{
    return node_cast<normal_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c pointing_device_sensor_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c pointing_device_sensor_node, or 0 otherwise.
 */
openvrml::pointing_device_sensor_node *
openvrml::externproto_node::to_pointing_device_sensor()
    OPENVRML_NOTHROW
{
    return node_cast<pointing_device_sensor_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c scoped_light_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c scoped_light_node, or 0 otherwise.
 */
openvrml::scoped_light_node *
openvrml::externproto_node::to_scoped_light() OPENVRML_NOTHROW
{
    return node_cast<scoped_light_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c sound_source_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c sound_source_node, or 0 otherwise.
 */
openvrml::sound_source_node *
openvrml::externproto_node::to_sound_source() OPENVRML_NOTHROW
{
    return node_cast<sound_source_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_node, or 0 otherwise.
 */
openvrml::texture_node * openvrml::externproto_node::to_texture()
    OPENVRML_NOTHROW
{
    return node_cast<texture_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_coordinate_node, or 0 otherwise.
 */
openvrml::texture_coordinate_node *
openvrml::externproto_node::to_texture_coordinate()
    OPENVRML_NOTHROW
{
    return node_cast<texture_coordinate_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_transform_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_transform_node, or 0 otherwise.
 */
openvrml::texture_transform_node *
openvrml::externproto_node::to_texture_transform()
    OPENVRML_NOTHROW
{
    return node_cast<texture_transform_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c time_dependent_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c time_dependent_node, or 0 otherwise.
 */
openvrml::time_dependent_node *
openvrml::externproto_node::to_time_dependent()
    OPENVRML_NOTHROW
{
    return node_cast<time_dependent_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c transform_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c transform_node, or 0 otherwise.
 */
openvrml::transform_node * openvrml::externproto_node::to_transform()
    OPENVRML_NOTHROW
{
    return node_cast<transform_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c viewpoint_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c viewpoint_node, or 0 otherwise.
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
 * @class openvrml::resource_istream openvrml/browser.h
 *
 * @brief An abstract input stream for network resources.
 *
 * @c resource_istream inherits @c std::istream, adding functions to get the
 * URI and the MIME content type associated with the stream.  Users of the
 * library must provide an implementation of this class, to be returned from
 * @c openvrml::resource_fetcher::do_get_resource.
 *
 * @sa openvrml::resource_fetcher::do_get_resource(const std::string &)
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
 * This function delegates to @c #do_url.
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
 * This function delegates to @c #do_type.
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
 * This function delegates to @c #do_data_available.
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
 * @class openvrml::resource_fetcher
 *
 * @brief An abstract factory for @c resource_istream%s.
 *
 * A concrete implementation of this interface must be passed to
 * <code>browser</code>'s constructor.
 *
 * @sa openvrml::browser
 */

/**
 * @brief Destroy.
 */
openvrml::resource_fetcher::~resource_fetcher() OPENVRML_NOTHROW
{}

/**
 * @brief Fetch a network resource.
 *
 * @param[in] uri   a Uniform Resource Identifier.
 *
 * This function delegates to @c resource_fetcher::do_get_resource.
 *
 * @return the requested resource as a stream.
 *
 * @exception std::invalid_argument if @p uri is malformed or in an
 *                                  unsupported format.
 */
std::auto_ptr<openvrml::resource_istream>
openvrml::resource_fetcher::get_resource(const std::string & uri)
{
    return this->do_get_resource(uri);
}

/**
 * @fn std::auto_ptr<openvrml::resource_istream> openvrml::resource_fetcher::do_get_resource(const std::string & uri)
 *
 * @brief Fetch a network resource.
 *
 * Called by @c #get_resource, clients of OpenVRML are required to provide an
 * implementation for this function.  OpenVRML depends on the implementation
 * of this function for all of its input needs.  As such, what kinds of
 * resources OpenVRML is capable of resolving are entirely dependent on code
 * provided by the application.
 *
 * Implementations should throw @c std::invalid_argument if @p uri is
 * malformed or in an format that is not supported by the implementation.
 *
 * A trivial implementation designed to handle only @c file resources can use
 * @c std::filebuf:
 *
 * @code
 * std::auto_ptr<openvrml::resource_istream>
 * my_resource_fetcher::do_get_resource(const std::string & uri)
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
 *             if (!this->buf_.open(path.c_str(),
 *                                  ios_base::in | ios_base::binary)) {
 *                 this->setstate(ios_base::badbit);
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
 * The @p uri parameter is provided by OpenVRML and can be assumed to be an
 * absolute URI.  As such, it will always have a scheme through which the
 * client code can choose a resolution mechanism.  For more information on URI
 * syntax, see <a
 * href="ftp://ftp.rfc-editor.org/in-notes/std/std66.txt">Internet
 * STD&nbsp;66</a>.
 *
 * @param[in] uri   an absolute Uniform Resource Identifier.
 *
 * @return the requested resource as a stream.
 *
 * @exception std::invalid_argument if @p uri is malformed or in an
 *                                  unsupported format.
 *
 * @sa ftp://ftp.rfc-editor.org/in-notes/std/std66.txt
 */


/**
 * @class openvrml::stream_listener openvrml/browser.h
 *
 * @brief An interface to simplify asynchronously reading a
 *        @c resource_istream.
 */

/**
 * @brief Destroy.
 */
openvrml::stream_listener::~stream_listener() OPENVRML_NOTHROW
{}

/**
 * @brief Called once the stream is available for use.
 *
 * This function calls @c #do_stream_available.
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
 * @brief Called by @c #stream_available.
 *
 * Concrete @c stream_listener%s must override this function.
 *
 * @param[in] uri           the URI associated with the stream.
 * @param[in] media_type    the MIME media type for the stream.
 */

/**
 * @brief Called when data is available.
 *
 * This function calls @c #do_data_available.
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
 * @brief Called by @c #data_available.
 *
 * @param[in] data  the data.
 */

/**
 * @class openvrml::invalid_vrml openvrml/browser.h
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
 * @class openvrml::viewer_in_use openvrml/browser.h
 *
 * @brief Exception thrown when attempting to associate a @c viewer
 *        with a @c browser when the @c viewer is already
 *        associated with a @c browser.
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
 * @class openvrml::browser_event openvrml/browser.h
 *
 * @brief A <code>browser</code>-wide event.
 */

/**
 * @var openvrml::browser_event::browser
 *
 * @brief Only @c browser%s can construct @c browser_event%s.
 */

/**
 * @enum openvrml::browser_event::type_id
 *
 * @brief @c browser_event type identifier.
 */

/**
 * @var openvrml::browser_event::initialized
 *
 * @brief An @c initialized event is sent once the world has loaded and all
 *        nodes in the initial scene have been initialized.
 */

/**
 * @var openvrml::browser_event::shutdown
 *
 * @brief A @c shutdown event is sent once all of the nodes in the scene have
 *        been shut down.
 */

/**
 * @internal
 *
 * @var openvrml::browser * openvrml::browser_event::source_
 *
 * @brief The @c browser from which the event originated.
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
 * @param[in] b     the @c browser from which the event originated.
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
 * @brief The @c browser that emitted the event.
 *
 * @return the @c browser that emitted the event.
 */
openvrml::browser & openvrml::browser_event::source() const OPENVRML_NOTHROW
{
    return *this->source_;
}


/**
 * @class openvrml::browser_listener openvrml/browser.h
 *
 * @brief This class should be inherited by classes that want to listen for
 *        @c browser_event%s.
 */

/**
 * @internal
 *
 * @var openvrml::browser_listener::browser
 *
 * @brief @c browser instances need to call @c #browser_changed.
 */

/**
 * @brief Destroy.
 */
openvrml::browser_listener::~browser_listener() OPENVRML_NOTHROW
{}

/**
 * @internal
 *
 * @brief Send a @c browser_event.
 *
 * This function delegates to @c #do_browser_changed.
 *
 * @param[in] event the @c browser_event to send.
 */
void openvrml::browser_listener::browser_changed(const browser_event & event)
{
    this->do_browser_changed(event);
}

/**
 * @fn void openvrml::browser_listener::do_browser_changed(const browser_event & event)
 *
 * @param[in] event the @c browser_event to send.
 */


/**
 * @class openvrml::browser openvrml/browser.h
 *
 * @brief Encapsulates a VRML browser.
 *
 * @c browser is the foundation of the OpenVRML runtime.  @c browser is
 * instantiated with an implementation of @c resource_fetcher, which is
 * provided by application code.  The @c resource_fetcher instance must have a
 * longer lifetime than the @c browser instance, since the @c resource_fetcher
 * instance could be used during destruction of the @c browser.  Note,
 * however, that <code>browser</code>'s destructor will block until all
 * threads that may use the @c resource_fetcher have completed.  So it is
 * sufficient to have the @c browser and the @c resource_fetcher destroyed
 * sequentially in the same thread.
 *
 * @sa openvrml::resource_fetcher
 */

/**
 * @example sdl_viewer.cpp
 *
 * Basic example using @c openvrml::browser.
 */

/**
 * @var class openvrml::browser::scene
 *
 * @brief The scene.
 */

/**
 * @internal
 *
 * @class openvrml::browser::node_metatype_map openvrml/browser.h
 *
 * @brief The map of @c node_metatype%s.
 */

/**
 * @var openvrml::read_write_mutex openvrml::browser::node_metatype_map::mutex_
 *
 * @brief Object mutex.
 */

/**
 * @typedef openvrml::browser::node_metatype_map::map_t
 *
 * @brief Map type.
 */

/**
 * @var openvrml::browser::node_metatype_map::map_t openvrml::browser::node_metatype_map::map_
 *
 * @brief Map.
 */

/**
 * @brief Construct.
 */
openvrml::browser::node_metatype_map::node_metatype_map()
{}

# ifndef NDEBUG
namespace {
    struct OPENVRML_LOCAL node_metatype_equals_ :
        public std::unary_function<std::pair<std::string,
                                             boost::shared_ptr<openvrml::node_metatype> >,
                                   bool> {
        explicit node_metatype_equals_(
            const boost::shared_ptr<openvrml::node_metatype> & node_metatype):
            node_metatype_(node_metatype)
        {}

        bool operator()(const argument_type & arg) const
        {
            return arg.second == this->node_metatype_;
        }

    private:
        boost::shared_ptr<openvrml::node_metatype> node_metatype_;
    };
}
# endif

/**
 * @brief Destroy.
 */
openvrml::browser::node_metatype_map::~node_metatype_map() OPENVRML_NOTHROW
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
                          node_metatype_equals_(entry->second));
        assert(entry->second.use_count() == count
               && "shared_ptr<node_metatype> use_count does not match the "
               "number of entries in the browser's node_metatype_map");
    }
# endif
}

/**
 * @fn openvrml::browser::node_metatype_map::node_metatype_map(const node_metatype_map &)
 *
 * @brief Not implemented.
 */

/**
 * @brief Assign.
 *
 * @param[in] ncm   the value to assign.
 */
openvrml::browser::node_metatype_map &
openvrml::browser::node_metatype_map::operator=(const node_metatype_map & ncm)
{
    read_write_mutex::scoped_write_lock my_lock(this->mutex_);
    read_write_mutex::scoped_read_lock map_lock(ncm.mutex_);
    map_t temp(ncm.map_);
    swap(this->map_, temp);
    return *this;
}

namespace {
    typedef std::map<std::string, boost::shared_ptr<openvrml::node_metatype> >
        node_metatype_map_t;

    struct OPENVRML_LOCAL init_node_metatype :
        std::unary_function<void, node_metatype_map_t::value_type>
    {
        init_node_metatype(openvrml::viewpoint_node * initial_viewpoint,
                        const double time)
            OPENVRML_NOTHROW:
            initial_viewpoint_(initial_viewpoint),
            time_(time)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
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
 * @brief Initialize the @c node_metatype%s.
 *
 * @param[in] initial_viewpoint the @c viewpoint_node that should be initially
 *                              active.
 * @param[in] timestamp         the current time.
 */
void
openvrml::browser::node_metatype_map::init(viewpoint_node * initial_viewpoint,
                                           const double timestamp)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    for_each(this->map_.begin(), this->map_.end(),
             init_node_metatype(initial_viewpoint, timestamp));
}

/**
 * @brief Insert a @c node_metatype.
 *
 * This operation will &ldquo;fail&rdquo; silently.  That is, if a
 * @c node_metatype corresponding to @p id already exists in the map, the
 * existing element will simply be returned.
 *
 * @param[in] id            the implementation identifier.
 * @param[in] node_metatype a @c node_metatype.
 *
 * @return the element in the node_metatype_map corresponding to @p id.
 */
const boost::shared_ptr<openvrml::node_metatype>
openvrml::browser::node_metatype_map::
insert(const std::string & id,
       const boost::shared_ptr<openvrml::node_metatype> & node_metatype)
{
    read_write_mutex::scoped_write_lock lock(this->mutex_);
    return this->map_.insert(make_pair(id, node_metatype)).first->second;
}

/**
 * @brief Remove a @c node_metatype.
 *
 * @param[in] id    the implementation identifier.
 *
 * @return @c true if a @c node_metatype is removed; @c false otherwise.
 */
bool openvrml::browser::node_metatype_map::remove(const std::string & id)
{
    read_write_mutex::scoped_write_lock lock(this->mutex_);
    return this->map_.erase(id) > 0;
}

/**
 * @brief Find a @c node_metatype.
 *
 * @param[in] id    an implementation identifier.
 *
 * @return the @c node_metatype corresponding to @p id, or a null
 *         pointer if no such @c node_metatype exists in the map.
 */
const boost::shared_ptr<openvrml::node_metatype>
openvrml::browser::node_metatype_map::find(const std::string & id) const
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    const map_t::const_iterator pos = this->map_.find(id);
    return (pos != this->map_.end())
        ? pos->second
        : boost::shared_ptr<openvrml::node_metatype>();
}

/**
 * @brief The @c node_metatype identifiers associated with @p node_metatype.
 *
 * @param[in] node_metatype    a @c node_metatype.
 *
 * @return the @c node_metatype identifiers associated with @p node_metatype.
 */
const std::vector<openvrml::node_metatype_id>
openvrml::browser::node_metatype_map::
node_metatype_ids(const openvrml::node_metatype & node_metatype) const
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::vector<node_metatype_id> ids;
    for (map_t::const_iterator entry = this->map_.begin();
         entry != this->map_.end();
         ++entry) {
        if (entry->second.get() == &node_metatype) {
            ids.push_back(entry->first);
        }
    }
    return ids;
}

namespace {

    struct OPENVRML_LOCAL render_node_metatype :
            std::unary_function<void, node_metatype_map_t::value_type> {
        explicit render_node_metatype(openvrml::viewer & viewer):
            viewer(&viewer)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
        {
            value.second->render(*this->viewer);
        }

    private:
        openvrml::viewer * viewer;
    };
}

/**
 * @brief Render the @c node_metatype%s.
 *
 * @param[in,out] v a @c viewer.
 */
void openvrml::browser::node_metatype_map::render(openvrml::viewer & v)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    for_each(this->map_.begin(), this->map_.end(), render_node_metatype(v));
}

namespace {

    struct OPENVRML_LOCAL shutdown_node_metatype :
            std::unary_function<void, node_metatype_map_t::value_type> {
        explicit shutdown_node_metatype(const double timestamp):
            timestamp_(timestamp)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
        {
            value.second->shutdown(this->timestamp_);
        }

    private:
        double timestamp_;
    };
}

/**
 * @brief Shut down the @c node_metatype%s.
 *
 * @param[in] timestamp the current time.
 */
void
openvrml::browser::node_metatype_map::shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    for_each(this->map_.begin(), this->map_.end(),
             shutdown_node_metatype(timestamp));
}

/**
 * @internal
 *
 * @var std::auto_ptr<openvrml::null_node_metatype> openvrml::browser::null_node_metatype_
 *
 * @brief &ldquo;Null&rdquo; class object for default nodes (e.g.,
 *        @c default_viewpoint).
 */

/**
 * @internal
 *
 * @var std::auto_ptr<openvrml::null_node_type> openvrml::browser::null_node_type_
 *
 * @brief &ldquo;Null&rdquo; type object for default nodes (e.g., @c
 *        default_viewpoint).
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::load_root_scene_thread_mutex_
 *
 * @brief Mutex protecting @c #load_root_scene_thread_.
 */

/**
 * @internal
 *
 * @var boost::scoped_ptr<boost::thread> openvrml::browser::load_root_scene_thread_
 *
 * @brief The thread that loads the root scene.
 *
 * This thread is spawned by @c #load_url. It is joined there (in a subsequent
 * call to @c #load_url) or when the @c browser is destroyed.
 */

/**
 * @internal
 *
 * @var boost::thread_group openvrml::browser::load_proto_thread_group_
 *
 * @brief The threads that load @c EXTERNPROTO implementations.
 *
 * These threads @b must be joined by the @c browser before it is destroyed.
 */

/**
 * @internal
 *
 * @var openvrml::browser::node_metatype_map openvrml::browser::node_metatype_map_
 *
 * @brief A map of URIs to node implementations.
 */

/**
 * @internal
 *
 * @var openvrml::script_node_metatype openvrml::browser::script_node_metatype_
 *
 * @brief @c node_metatype for Script @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::resource_fetcher & openvrml::browser::fetcher_
 *
 * @brief A reference to the @c resource_fetcher associated with the
 *        @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scene_mutex_
 *
 * @brief Mutex protecting @c #scene_.
 */

/**
 * @internal
 *
 * @var openvrml::scene * openvrml::browser::scene_
 *
 * @brief Pointer to the root @c scene.
 */

/**
 * @internal
 *
 * @var boost::intrusive_ptr<openvrml::node> openvrml::browser::default_viewpoint_
 *
 * @brief The &ldquo;default&rdquo; @c viewpoint_node used when no
 *        @c viewpoint_node in the scene is bound.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::active_viewpoint_mutex_
 *
 * @brief Mutex protecting @c #active_viewpoint_.
 */

/**
 * @internal
 *
 * @var openvrml::viewpoint_node * openvrml::browser::active_viewpoint_
 *
 * @brief The currently &ldquo;active&rdquo; @c viewpoint_node.
 */

/**
 * @internal
 *
 * @var boost::intrusive_ptr<openvrml::node> openvrml::browser::default_navigation_info_
 *
 * @brief The &ldquo;default&rdquo; @c navigation_info_node used when no @c
 *        navigation_info_node in the scene is bound.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::active_navigation_info_mutex_
 *
 * @brief Mutex protecting @c #active_navigation_info_.
 */

/**
 * @internal
 *
 * @var openvrml::navigation_info_node * openvrml::browser::active_navigation_info_
 *
 * @brief The currently &ldquo;active&rdquo; @c navigation_info_node.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::viewpoint_list_mutex_
 *
 * @brief Mutex protecting @c #viewpoint_list_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::viewpoint_node *> openvrml::browser::viewpoint_list_
 *
 * @brief A list of all the Viewpoint @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scoped_lights_mutex_
 *
 * @brief Mutex protecting @c #scoped_lights_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::node *> openvrml::browser::scoped_lights_
 *
 * @brief A list of all the scoped light @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scripts_mutex_
 *
 * @brief Mutex protecting @c #scripts_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::script_node *> openvrml::browser::scripts_
 *
 * @brief A list of all the Script @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::timers_mutex_
 *
 * @brief Mutex protecting @c #timers_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::time_dependent_node *> openvrml::browser::timers_
 *
 * @brief A list of all the TimeSensor @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var boost::mutex openvrml::browser::listeners_mutex_
 *
 * @brief Mutex to guard @c #listeners_.
 */

/**
 * @internal
 *
 * @var std::set<openvrml::browser_listener *> openvrml::browser::listeners_
 *
 * @brief The set of @c browser_listener%s that will receive
 *        @c browser_event%s.
 *
 * @sa #add_listener
 * @sa #remove_listener
 */

/**
 * @var bool openvrml::browser::modified_
 *
 * @brief Flag to indicate whether the @c browser has been modified.
 */

/**
 * @var openvrml::read_write_mutex openvrml::browser::modified_mutex_
 *
 * @brief Mutex protecting @c #modified_.
 */

/**
 * @internal
 *
 * @var bool openvrml::browser::new_view
 *
 * @brief Flag to indicate if the user has changed to a new view.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::delta_time_mutex_
 *
 * @brief Mutex protecting @c #delta_time.
 */

/**
 * @internal
 *
 * @var double openvrml::browser::delta_time
 *
 * @brief Time elapsed since the last update.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::viewer_mutex_
 *
 * @brief Mutex protecting @c #viewer_.
 */

/**
 * @internal
 *
 * @var openvrml::openvrml::viewer * openvrml::browser::viewer_
 *
 * @brief The current @c viewer.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::frame_rate_mutex_
 *
 * @brief Mutex protecting @c #frame_rate_.
 */

/**
 * @internal
 *
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
 * @brief Mutex guarding @c #out_.
 */

/**
 * @var std::ostream & openvrml::browser::out_
 *
 * @brief Output stream, generally for console output.
 */

/**
 * @var boost::mutex openvrml::browser::err_mutex_
 *
 * @brief Mutex guarding @c #err_.
 */

/**
 * @var std::ostream & openvrml::browser::err_
 *
 * @brief Error output stream.
 */

/**
 * @var bool openvrml::browser::flags_need_updating
 *
 * @brief Set by @c node::bounding_volume_dirty on any node in this browser graph,
 *        cleared by @c #update_flags.
 *
 * @c true if the bvolume dirty flag has been set on a @c node in the
 * @c browser graph, but has not yet been propagated to that
 * <code>node</code>'s ancestors.
 */

namespace {
    void OPENVRML_LOCAL register_node_metatypes(openvrml::browser & b)
    {
        register_core_node_metatypes(b);
        register_vrml97_node_metatypes(b);
        register_networking_node_metatypes(b);
        register_grouping_node_metatypes(b);
        register_rendering_node_metatypes(b);
        register_shape_node_metatypes(b);
        register_geometry2d_node_metatypes(b);
        register_texturing_node_metatypes(b);
        register_interpolation_node_metatypes(b);
        register_key_device_sensor_node_metatypes(b);
        register_event_utilities_node_metatypes(b);
        register_dis_node_metatypes(b);
        register_environmental_effects_node_metatypes(b);
        register_geospatial_node_metatypes(b);
        register_hanim_node_metatypes(b);
        register_nurbs_node_metatypes(b);
        register_cad_geometry_node_metatypes(b);
    }
}

/**
 * @brief Constructor.
 *
 * @param[in] fetcher   a @c resource_fetcher implementation.
 * @param[in] out       output stream for console output.
 * @param[in] err       output stream for error console output.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::browser::browser(resource_fetcher & fetcher,
                           std::ostream & out,
                           std::ostream & err)
    OPENVRML_THROW1(std::bad_alloc):
    null_node_metatype_(new null_node_metatype(*this)),
    null_node_type_(new null_node_type(*null_node_metatype_)),
    script_node_metatype_(*this),
    fetcher_(fetcher),
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
    frame_rate_(0.0),
    out_(&out),
    err_(&err),
    flags_need_updating(false)
{
    assert(this->active_viewpoint_);
    assert(this->active_navigation_info_);
    register_node_metatypes(*this);
}

/**
 * @brief Destructor.
 */
openvrml::browser::~browser() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock
        lock(this->load_root_scene_thread_mutex_);
    if (this->load_root_scene_thread_) {
        this->load_root_scene_thread_->join();
    }

    this->load_proto_thread_group_.join_all();

    const double now = browser::current_time();

    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    if (this->scene_) { this->scene_->shutdown(now); }

    this->node_metatype_map_.shutdown(now);
    assert(this->viewpoint_list_.empty());
    assert(this->scoped_lights_.empty());
    assert(this->scripts_.empty());
    assert(this->timers_.empty());
}

/**
 * @brief Add a @c node_metatype.
 *
 * If a @c node_metatype identified by @p id has already been added to
 * the browser, it will be replaced.
 *
 * @warning If @c std::bad_alloc is thrown here, the <code>browser</code>'s
 *          @c node_metatype map is left in an unknown state.  In all
 *          likelihood any preexisting entry in the map with the same
 *          implementation identifier as @p id will have been removed.
 *
 * @param[in] id    a @c node_metatype identifier.
 * @param[in] nc    a @c boost::shared_ptr to a @c node_metatype
 *
 * @exception std::invalid_argument if @p nc is null.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void
openvrml::browser::
add_node_metatype(const node_metatype_id & id,
               const boost::shared_ptr<openvrml::node_metatype> & nc)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    if (!nc) {
        throw std::invalid_argument("cannot add null node_metatype pointer");
    }
    this->node_metatype_map_.remove(id); // Remove any existing entry.
    this->node_metatype_map_.insert(id, nc);
}

/**
 * @brief Get the @c node_metatype corresponding to @p id.
 *
 * @param[in] id    a @c node_metatype identifier.
 *
 * @return the @c node_metatype corresponding to @p id; or a null pointer if
 *         no such @c node_metatype exists.
 */
const boost::shared_ptr<openvrml::node_metatype>
openvrml::browser::node_metatype(const node_metatype_id & id) const OPENVRML_NOTHROW
{
    return this->node_metatype_map_.find(id);
}

/**
 * @brief Get the root @c scene.
 *
 * @return the root @c scene, or 0 if no @c scene is loaded.
 */
openvrml::scene * openvrml::browser::root_scene() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    return this->scene_.get();
}

/**
 * @brief Get the path to a @c node in the scene graph.
 *
 * @param[in] n  the objective @c node.
 *
 * @return the path to @p node, starting with a root @c node, and ending with
 *         @p node. If @p node is not in the scene graph, the returned
 *         @c node_path is empty.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const openvrml::node_path openvrml::browser::find_node(const node & n) const
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
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
 * @brief Get the active @c viewpoint_node.
 *
 * The active @c viewpoint_node is the one currently associated with the user
 * view.
 *
 * @return the active @c viewpoint_node.
 */
openvrml::viewpoint_node & openvrml::browser::active_viewpoint() const
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->active_viewpoint_mutex_);
    return *this->active_viewpoint_;
}

/**
 * @brief Set the active @c viewpoint_node.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @pre @p viewpoint.scene() == @c this->root_scene()
 */
void openvrml::browser::active_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    read_write_mutex::scoped_write_lock
        active_viewpoint_lock(this->active_viewpoint_mutex_);
# ifndef NDEBUG
    scene * root_scene = 0;
    for (root_scene = viewpoint.scene();
         root_scene->parent();
         root_scene = root_scene->parent())
    {}
# endif
    assert(root_scene == this->scene_.get());
    this->active_viewpoint_ = &viewpoint;
}

/**
 * @brief Reset the active @c viewpoint_node to the default.
 */
void openvrml::browser::reset_default_viewpoint() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->active_viewpoint_mutex_);
    assert(this->default_viewpoint_);
    this->active_viewpoint_ =
        node_cast<viewpoint_node *>(this->default_viewpoint_.get());
    assert(this->active_viewpoint_);
}

/**
 * @brief Get the active @c navigation_info_node.
 *
 * The active @c navigation_info_node is the one currently associated with the
 * user view.
 *
 * @return the active @c navigation_info_node.
 */
openvrml::navigation_info_node &
openvrml::browser::active_navigation_info() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock
        lock(this->active_navigation_info_mutex_);
    return *this->active_navigation_info_;
}

/**
 * @brief Set the active @c navigation_info_node.
 *
 * @param[in] nav_info a @c navigation_info_node.
 *
 * @pre @p viewpoint.scene() == @c this->root_scene()
 */
void openvrml::browser::active_navigation_info(navigation_info_node & nav_info)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    read_write_mutex::scoped_write_lock
        active_navigation_info_lock(this->active_navigation_info_mutex_);
# ifndef NDEBUG
    scene * root_scene = 0;
    for (root_scene = nav_info.scene();
         root_scene->parent();
         root_scene = root_scene->parent())
    {}
# endif
    assert(root_scene == this->scene_.get());
    this->active_navigation_info_ = &nav_info;
}

/**
 * @brief Reset the active @c navigation_info_node to the default.
 */
void openvrml::browser::reset_default_navigation_info() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock
        lock(this->active_navigation_info_mutex_);
    assert(this->default_navigation_info_);
    this->active_navigation_info_ =
        node_cast<navigation_info_node *>(
            this->default_navigation_info_.get());
    assert(this->active_navigation_info_);
}

/**
 * @brief Add a @c viewpoint_node to the list of @c viewpoint_node%s for the
 *        @c browser.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p viewpoint is not in the list of @c viewpoint_node%s for the @c
 *      browser.
 */
void openvrml::browser::add_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_write_lock lock(this->viewpoint_list_mutex_);
    assert(std::find(this->viewpoint_list_.begin(), this->viewpoint_list_.end(),
                     &viewpoint) == this->viewpoint_list_.end());
    this->viewpoint_list_.push_back(&viewpoint);
}

/**
 * @brief Remove a @c viewpoint_node from the list of @c viewpoint_node%s for
 *        the @c browser.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @pre @p viewpoint is in the list of @c viewpoint_node%s for the @c browser.
 */
void openvrml::browser::remove_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->viewpoint_list_mutex_);
    assert(!this->viewpoint_list_.empty());
    typedef std::list<viewpoint_node *> viewpoint_list_t;
    const viewpoint_list_t::iterator end = this->viewpoint_list_.end();
    const viewpoint_list_t::iterator pos =
            std::find(this->viewpoint_list_.begin(), end, &viewpoint);
    assert(pos != end);
    this->viewpoint_list_.erase(pos);
}

/**
 * @brief Get the list of @c viewpoint_node%s for the world.
 *
 * @return the list of @c viewpoint_node%s for the world.
 */
const std::list<openvrml::viewpoint_node *>
openvrml::browser::viewpoints() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->viewpoint_list_mutex_);
    return this->viewpoint_list_;
}

/**
 * @brief Set the current @c viewer.
 *
 * @param[in] v @c viewer.
 *
 * @exception viewer_in_use if @p v is already associated with a
 *                          @c browser.
 */
void openvrml::browser::viewer(openvrml::viewer * v)
    OPENVRML_THROW1(viewer_in_use)
{
    read_write_mutex::scoped_write_lock lock(this->viewer_mutex_);
    if (v && v->browser_) { throw viewer_in_use(); }
    if (this->viewer_) { this->viewer_->browser_ = 0; }
    this->viewer_ = v;
    if (v) { v->browser_ = this; }
}

/**
 * @brief The current @c viewer.
 *
 * @return the current @c viewer.
 */
openvrml::viewer * openvrml::browser::viewer() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->viewer_mutex_);
    return this->viewer_;
}

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
 * @brief Get the @c browser version.
 *
 * @return the version of OpenVRML.
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
    read_write_mutex::scoped_read_lock
        lock(this->active_navigation_info_mutex_);
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
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    assert(this->scene_);
    return this->scene_->url(); // Throws std::bad_alloc.
}

/**
 * @brief Set the world from a stream.
 *
 * @param[in,out] in    an input stream.
 *
 * @exception bad_media_type    if @p in.type() is not @c model/vrml,
 *                              @c x-world/x-vrml, or @c model/x3d+vrml.
 * @exception invalid_vrml      if @p in has invalid syntax.
 */
void openvrml::browser::set_world(resource_istream & in)
{
    {
        read_write_mutex::scoped_read_write_lock scene_lock(this->scene_mutex_);

        using std::for_each;
        using std::string;

        //
        // Clear out the current scene.
        //
        this->load_proto_thread_group_.join_all();
        double now = browser::current_time();
        if (this->scene_) { this->scene_->shutdown(now); }
        this->node_metatype_map_.shutdown(now);
        read_write_mutex::scoped_read_lock
            listeners_lock(this->listeners_mutex_);
        for_each(this->listeners_.begin(), this->listeners_.end(),
                 boost::bind2nd(
                     boost::mem_fun(&browser_listener::browser_changed),
                     browser_event(*this, browser_event::shutdown)));

        scene_lock.promote();

        this->scene_.reset();
        assert(this->viewpoint_list_.empty());
        assert(this->scoped_lights_.empty());
        assert(this->scripts_.empty());
        assert(this->timers_.empty());

        //
        // Create the new scene.
        //
        node_metatype_map new_map;
        this->node_metatype_map_ = new_map;
        register_node_metatypes(*this);
        this->scene_.reset(new scene(*this));

        scene_lock.demote();

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
        // Initialize the node_metatypes.
        //
        this->node_metatype_map_.init(initial_viewpoint, now);

        if (!this->active_viewpoint_) {
            read_write_mutex::scoped_write_lock
                lock(this->active_viewpoint_mutex_);
            this->active_viewpoint_ = this->default_viewpoint_.get();
        }

        this->modified(true);
        this->new_view = true; // Force resetUserNav
    } // unlock this->scene_mutex_, this->active_viewpoint_mutex_

    read_write_mutex::scoped_read_lock
        listeners_lock(this->listeners_mutex_);
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
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    const double now = browser::current_time();
    this->scene_->nodes(nodes);
    this->scene_->initialize(now);
    //
    // Initialize the node_metatypes.
    //
    static viewpoint_node * const initial_viewpoint = 0;
    this->node_metatype_map_.init(initial_viewpoint, now);
    this->modified(true);
    this->new_view = true; // Force resetUserNav
}

struct OPENVRML_LOCAL openvrml::browser::root_scene_loader {
    root_scene_loader(browser & b, const std::vector<std::string> & url)
        OPENVRML_THROW1(std::bad_alloc):
        browser_(&b),
        url_(url)
    {}

    void operator()() const OPENVRML_NOTHROW
    {
        try {
            using std::endl;

            openvrml::browser & browser = *this->browser_;

            try {
                std::auto_ptr<resource_istream> in;
                {
                    read_write_mutex::scoped_read_lock
                        lock(browser.scene_mutex_);
                    in = browser.scene_->get_resource(this->url_);
                }
                if (!(*in)) { throw unreachable_url(); }
                browser.set_world(*in);
            } catch (invalid_vrml & ex) {
                std::ostringstream out;
                out << ex.url << ':' << ex.line << ':' << ex.column
                    << ": error: " << ex.what();
                browser.err(out.str());
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
    }

private:
    browser * browser_;
    const std::vector<std::string> url_;
};

/**
 * @brief Load a VRML world into the browser.
 *
 * @param[in] url       a URI.
 * @param[in] parameter parameters for @p url.
 *
 * @exception std::bad_alloc                if memory allocation fails.
 * @exception boost::thread_resource_error  if thread creation fails.
 */
void openvrml::browser::load_url(const std::vector<std::string> & url,
                                 const std::vector<std::string> &)
    OPENVRML_THROW2(std::bad_alloc, boost::thread_resource_error)
{
    {
        read_write_mutex::scoped_read_lock
            lock(this->load_root_scene_thread_mutex_);
        if (this->load_root_scene_thread_) {
            this->load_root_scene_thread_->join();
        }
    }

    boost::function0<void> f = root_scene_loader(*this, url);

    read_write_mutex::scoped_write_lock
        lock(this->load_root_scene_thread_mutex_);
    this->load_root_scene_thread_.reset(new boost::thread(f));
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
// to identify any PROTOs in the stream in the browser's node_metatype map.  A
// side-effect of this approach is that the node_metatype map will keep growing,
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
    using std::ostringstream;
    using std::string;
    using std::vector;
    using boost::lexical_cast;

    {
        boost::mutex::scoped_lock lock(stream_id_index_mutex_);
        ++stream_id_index_;
    }

    ostringstream stream_id;
    stream_id << anonymous_stream_id_prefix_
              << lexical_cast<string>(stream_id_index_);

    std::vector<boost::intrusive_ptr<node> > nodes;
    try {
        read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
        assert(this->scene_);
        std::map<string, string> meta;
        parse_vrml(in, stream_id.str(), type, *this->scene_, nodes, meta);
    } catch (openvrml::bad_media_type & ex) {
        //
        // bad_media_type is a std::runtime_error.  However, here we're using
        // the media type as an argument.
        //
        throw std::invalid_argument(ex.what());
    }
    return nodes;
}

/**
 * @brief Create nodes from a URI.
 *
 * This function executes asynchronously. When the nodes have been completely
 * loaded, they are sent to the @p event MFNode eventIn of @p node.
 *
 * @param[in] url       an alternative URI list.
 * @param[in] node      the node to which the nodes loaded from @p url should be
 *                      sent as an event.
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
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    assert(this->scene_);
    this->scene_->create_vrml_from_url(url, node, event);
}

/**
 * @brief Add a listener for @c browser_event%s.
 *
 * @param[in] listener  a @c browser_listener.
 *
 * @return @c true if @p listener is added successfully; @c false otherwise (if
 *         @p listener is already listening for events from the
 *         @c browser).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
bool openvrml::browser::add_listener(browser_listener & listener)
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_write_lock lock(this->listeners_mutex_);
    return this->listeners_.insert(&listener).second;
}

/**
 * @brief Remove a listener for @c browser_event%s.
 *
 * @param[in] listener  a @c browser_listener.
 *
 * @return @c true if @p listener is removed successfully; @c false otherwise
 *         (if @p listener is not listening for events from the
 *         @c browser).
 */
bool openvrml::browser::remove_listener(browser_listener & listener)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->listeners_mutex_);
    return this->listeners_.erase(&listener) > 0;
}

/**
 * @brief Get the current frame rate.
 *
 * @return the current frame rate.
 */
double openvrml::browser::frame_rate() const
{
    read_write_mutex::scoped_read_lock lock(this->frame_rate_mutex_);
    return this->frame_rate_;
}

/**
 * Called by the @c viewer when the cursor passes over, clicks, drags, or
 * releases a sensitive object (an Anchor or another grouping node with
 * an enabled TouchSensor child).
 */
void openvrml::browser::sensitive_event(node * const n,
                                        const double timestamp,
                                        const bool is_over,
                                        const bool is_active,
                                        const double (&point)[3])
{
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
 * @brief Process events (update the @c browser).
 *
 * This method should be called after each frame is rendered.
 *
 * @return @c true if the @c browser needs to be rerendered, @c false otherwise.
 */
bool openvrml::browser::update(double current_time)
{
    using std::for_each;

    read_write_mutex::scoped_read_lock
        timers_lock(this->timers_mutex_),
        scripts_lock(this->scripts_mutex_);

    if (current_time <= 0.0) { current_time = browser::current_time(); }

    this->delta_time = DEFAULT_DELTA;

    //
    // Update each of the timers.
    //
    for_each(this->timers_.begin(), this->timers_.end(),
             boost::bind2nd(boost::mem_fun(&time_dependent_node::update),
                            current_time));

    //
    // Update each of the scripts.
    //
    for_each(this->scripts_.begin(), this->scripts_.end(),
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
 * @brief Draw this @c browser into the specified @c viewer
 */
void openvrml::browser::render()
{
    read_write_mutex::scoped_read_lock
        scene_lock(this->scene_mutex_),
        active_viewpoint_lock_(this->active_viewpoint_mutex_);

    if (!this->viewer_) { return; }

    if (this->new_view) {
        this->viewer_->reset_user_navigation();
        this->new_view = false;
    }
    navigation_info_node & nav_info = this->active_navigation_info();
    const float avatarSize = nav_info.avatar_size().empty()
        ? 0.25f
        : nav_info.avatar_size()[0];
    const float visibilityLimit = nav_info.visibility_limit();

    this->viewer_->set_frustum(this->active_viewpoint_->field_of_view(),
                               avatarSize,
                               visibilityLimit);

    //
    // Per-node_metatype rendering happens before viewer::set_viewpoint is called
    // This is important for things like background rendering, since
    // viewer::insert_background must be called before viewer::set_viewpoint.
    //
    this->node_metatype_map_.render(*this->viewer_);

    // Activate the headlight.
    // ambient is supposed to be 0 according to the spec...
    if (this->headlight_on()) {
        static const color color = make_color(1.0, 1.0, 1.0);
        static const vec3f direction = make_vec3f(0.0, 0.0, -1.0);
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
                                 avatarSize,
                                 visibilityLimit);

    // Top level object

    this->viewer_->begin_object(0);
    mat4f modelview = t.inverse();
    rendering_context rc(bounding_volume::partial, modelview);
    rc.draw_bounding_spheres = true;

    // Do the browser-level lights (Points and Spots)
    {
        using std::for_each;
        read_write_mutex::scoped_read_lock
            scoped_lights_lock(this->scoped_lights_mutex_);
        for_each(this->scoped_lights_.begin(), this->scoped_lights_.end(),
                 boost::bind2nd(
                     boost::mem_fun(&scoped_light_node::render_scoped_light),
                     *this->viewer_));
    }

    //
    // Render the nodes.  scene_ may be 0 if the world failed to load.
    //
    if (this->scene_) {
        this->scene_->render(*this->viewer_, rc);
    }

    this->viewer_->end_object();

    // This is actually one frame late...
    read_write_mutex::scoped_write_lock
        frame_rate_lock(this->frame_rate_mutex_);
    this->frame_rate_ = this->viewer_->frame_rate();

    this->modified(false);
}

/**
 * @brief Indicate whether rendering is necessary.
 *
 * @param[in] value @c true to indicate that the browser state has changed and
 *                  rerendering is necessary; @c false once rendering has
 *                  occurred.
 */
void openvrml::browser::modified(const bool value)
{
    read_write_mutex::scoped_write_lock lock(this->modified_mutex_);
    this->modified_ = value;
}

/**
 * @brief Check if the browser has been modified.
 *
 * @return @c true if the browser has been modified, @c false otherwise.
 */
bool openvrml::browser::modified() const
{
    read_write_mutex::scoped_read_lock lock(this->modified_mutex_);
    return this->modified_;
}

/**
 * @brief Set the time until the next update is needed.
 *
 * @param[in] d a time interval.
 */
void openvrml::browser::delta(const double d)
{
    read_write_mutex::scoped_write_lock lock(this->delta_time_mutex_);
    if (d < this->delta_time) { this->delta_time = d; }
}

/**
 * @brief Get the time interval between @c browser updates.
 *
 * @return the time interval between @c browser updates.
 */
double openvrml::browser::delta() const
{
    read_write_mutex::scoped_read_lock lock(this->delta_time_mutex_);
    return this->delta_time;
}

/**
 * @brief Add a scoped light node to the @c browser.
 *
 * @param[in] light a light node.
 *
 * @pre @p light is not in the list of light nodes for the @c browser.
 */
void
openvrml::browser::add_scoped_light(scoped_light_node & light)
{
    read_write_mutex::scoped_write_lock lock(this->scoped_lights_mutex_);
    assert(std::find(this->scoped_lights_.begin(), this->scoped_lights_.end(),
                     &light) == this->scoped_lights_.end());
    this->scoped_lights_.push_back(&light);
}

/**
 * @brief Remove a scoped light node from the @c browser.
 *
 * @param[in] light the light node to remove.
 *
 * @pre @p light is in the list of light nodes for the @c browser.
 */
void
openvrml::browser::remove_scoped_light(scoped_light_node & light)
{
    read_write_mutex::scoped_write_lock lock(this->scoped_lights_mutex_);
    assert(!this->scoped_lights_.empty());
    const std::list<scoped_light_node *>::iterator end =
        this->scoped_lights_.end();
    const std::list<scoped_light_node *>::iterator pos =
            std::find(this->scoped_lights_.begin(), end, &light);
    assert(pos != end);
    this->scoped_lights_.erase(pos);
}

/**
 * @brief Add a Script node to the browser.
 *
 * @param[in] script    a Script node.
 *
 * @pre @p script is not in the list of Script nodes for the @c browser.
 */
void openvrml::browser::add_script(script_node & script)
{
    read_write_mutex::scoped_write_lock lock(this->scripts_mutex_);
    assert(std::find(this->scripts_.begin(), this->scripts_.end(), &script)
            == this->scripts_.end());
    this->scripts_.push_back(&script);
}

/**
 * @brief Remove a Script node from the browser.
 *
 * @param[in] script    the Script node to remove.
 *
 * @pre @p script is in the list of Script nodes for the @c browser.
 */
void openvrml::browser::remove_script(script_node & script)
{
    read_write_mutex::scoped_write_lock lock(this->scripts_mutex_);
    assert(!this->scripts_.empty());
    typedef std::list<script_node *> script_node_list_t;
    const script_node_list_t::iterator end = this->scripts_.end();
    const script_node_list_t::iterator pos =
            std::find(this->scripts_.begin(), end, &script);
    assert(pos != end);
    this->scripts_.erase(pos);
}

/**
 * @brief Add a time-dependent node to the browser.
 *
 * @param[in] n a @c time_dependent_node.
 *
 * @pre @p n is not in the list of @c time_dependent_node%s for the @c browser.
 */
void openvrml::browser::add_time_dependent(time_dependent_node & n)
{
    read_write_mutex::scoped_write_lock lock(this->timers_mutex_);
    assert(std::find(this->timers_.begin(), this->timers_.end(), &n)
           == this->timers_.end());
    this->timers_.push_back(&n);
}

/**
 * @brief Remove a time-dependent node from the browser.
 *
 * @param[in] n the @c time_dependent_node to remove.
 *
 * @pre @p n is in the list of @c time_dependent_node%s for the @c browser.
 */
void
openvrml::browser::remove_time_dependent(time_dependent_node & n)
{
    read_write_mutex::scoped_write_lock lock(this->timers_mutex_);
    assert(!this->timers_.empty());
    const std::list<time_dependent_node *>::iterator end = this->timers_.end();
    const std::list<time_dependent_node *>::iterator pos =
            std::find(this->timers_.begin(), end, &n);
    assert(pos != end);
    this->timers_.erase(pos);
}

/**
 * @brief Propagate the bvolume dirty flag from children to ancestors.
 *
 * The invariant is that if a <code>node</code>'s bounding volume is out of
 * date, then the bounding volumes of all that <code>node</code>'s ancestors
 * must be out of date.  However, @c node does not maintain a parent pointer.
 * So we must do a traversal of the entire browser graph to do the
 * propagation.
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
void openvrml::browser::out(const std::string & str) const
{
    boost::mutex::scoped_lock(this->out_mutex_);
    *this->out_ << str << std::endl;
}

/**
 * @brief Print a message to the error stream.
 *
 * @param[in] str   a string.
 */
void openvrml::browser::err(const std::string & str) const
{
    boost::mutex::scoped_lock(this->err_mutex_);
    *this->err_ << str << std::endl;
}


/**
 * @class openvrml::bad_url openvrml/browser.h
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
 * @class openvrml::invalid_url openvrml/browser.h
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
 * @class openvrml::bad_media_type openvrml/browser.h
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
 * @class openvrml::unreachable_url openvrml/browser.h
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
 * @class openvrml::no_alternative_url openvrml/browser.h
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
 * @class openvrml::scene openvrml/browser.h
 *
 * @brief A scene in the VRML world.
 */

/**
 * @internal
 *
 * @var openvrml::browser * const openvrml::scene::browser_
 *
 * @brief A reference to the @c browser associated with the @c scene.
 */

/**
 * @internal
 *
 * @var openvrml::scene * const openvrml::scene::parent_
 *
 * @brief A pointer to the parent @c scene.
 *
 * If the @c scene is the root @c scene, @a parent will be 0.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::scene::nodes_mutex_
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
 * @var openvrml::read_write_mutex openvrml::scene::url_mutex_
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
 * This may be a relative or an absolute reference.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::scene::meta_mutex_
 *
 * @brief Mutex protecting @c #meta_.
 */

/**
 * @internal
 *
 * @var std::map<std::string, std::string> openvrml::scene::meta_
 *
 * @brief Scene metadata map.
 */

/**
 * @internal
 *
 * @var boost::thread_group openvrml::scene::stream_reader_threads_
 *
 * @brief Stream reader thread group.
 */

/**
 * @brief Construct.
 *
 * @param[in] browser   the @c browser associated with the @c scene.
 * @param[in] parent    the parent @c scene.
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
{
    this->stream_reader_threads_.join_all();
}

/**
 * @brief Get the associated @c browser.
 *
 * @return the associated @c browser.
 */
openvrml::browser & openvrml::scene::browser() const OPENVRML_NOTHROW
{
    return *this->browser_;
}

/**
 * @brief Get the parent @c scene.
 *
 * @return the parent @c scene, or 0 if this is the root @c scene.
 */
openvrml::scene * openvrml::scene::parent() const OPENVRML_NOTHROW
{
    return this->parent_;
}

/**
 * @brief Load the @c scene from a stream.
 *
 * @param[in,out] in    an input stream.
 *
 * @exception bad_media_type    if @p in.type() is not
 *                              &ldquo;model/vrml&rdquo;,
 *                              &ldquo;x-world/x-vrml&rdquo;, or
 *                              &ldquo;model/x3d+vrml&rdquo;.
 * @exception invalid_vrml      if @p in has invalid syntax.
 */
void openvrml::scene::load(resource_istream & in)
{
    {
        read_write_mutex::scoped_write_lock
            nodes_lock(this->nodes_mutex_),
            url_lock(this->url_mutex_),
            meta_lock(this->meta_mutex_);

        this->nodes_.clear();
        this->meta_.clear();
        this->url_ = in.url();
        browser::parse_vrml(in,
                            in.url(),
                            in.type(),
                            *this,
                            this->nodes_,
                            this->meta_);
    }
    this->scene_loaded();
}

/**
 * @brief Initialize the @c scene.
 *
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::scene::initialize(const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->nodes_mutex_);
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
    read_write_mutex::scoped_read_lock lock(this->meta_mutex_);

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
    read_write_mutex::scoped_write_lock lock(this->meta_mutex_);
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
    read_write_mutex::scoped_read_lock lock(this->meta_mutex_);

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
 * @brief Root @c node%s for the @c scene.
 *
 * @return the root @c node%s for the @c scene.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::vector<boost::intrusive_ptr<openvrml::node> >
openvrml::scene::nodes() const OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->nodes_mutex_);
    return this->nodes_;
}

/**
 * @brief Set the root @c node%s for the @c scene.
 *
 * This function calls @c #shutdown to shut down the
 * <code>scene</code>'s existing @c node%s.
 *
 * @param[in] n the new root @c node%s for the @c scene.
 *
 * @exception std::invalid_argument if any of the @c node%s in @p n has
 *                                  already been initialized.
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

    read_write_mutex::scoped_write_lock lock(this->nodes_mutex_);
    const double now = browser::current_time();
    this->shutdown(now);
    this->nodes_ = n;
}

/**
 * @brief Get the root @c scope.
 *
 * @return the root @c scope.
 */
const openvrml::scope * openvrml::scene::root_scope() const
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->nodes_mutex_);
    return this->nodes_.empty()
        ? 0
        : &this->nodes_.front()->scope();
}

/**
 * @brief Get the absolute URI for the @c scene.
 *
 * @return the absolute URI for the @c scene.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string openvrml::scene::url() const OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->url_mutex_);
    using std::string;
    return (this->parent_ && !this->url_.empty() && relative(uri(this->url_)))
        ? string(uri(this->url_).resolve_against(uri(this->parent_->url())))
        : this->url_;
}

/**
 * @brief Render the scene.
 *
 * @param[in,out] viewer    a @c viewer to render to.
 * @param[in]     context   a @c rendering_context.
 */
void openvrml::scene::render(openvrml::viewer & viewer,
                             rendering_context context)
{
    read_write_mutex::scoped_read_lock lock(this->nodes_mutex_);
    for (std::vector<boost::intrusive_ptr<node> >::iterator n(
             this->nodes_.begin());
         n != this->nodes_.end();
         ++n) {
        assert(*n);
        child_node * child = node_cast<child_node *>(n->get());
        if (child) { child->render_child(viewer, context); }
    }
}

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
                if (!data.empty()) {
                    this->listener_->data_available(data);
                }
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
 * @c #read_stream takes ownership of its arguments; the resources are released
 * when reading the stream completes and the thread terminates.
 *
 * @param[in] in        an input stream.
 * @param[in] listener  a stream listener.
 */
void openvrml::scene::read_stream(std::auto_ptr<resource_istream> in,
                                  std::auto_ptr<stream_listener> listener)
{
    boost::function0<void> f = stream_reader(in, listener);
    this->stream_reader_threads_.create_thread(f);
}

struct OPENVRML_LOCAL openvrml::scene::vrml_from_url_creator {
    vrml_from_url_creator(openvrml::scene & scene,
                          const std::vector<std::string> & url,
                          const boost::intrusive_ptr<node> & node,
                          const std::string & event)
        OPENVRML_THROW2(unsupported_interface, std::bad_cast):
        scene_(&scene),
        url_(&url),
        node_(node),
        listener_(&dynamic_cast<mfnode_listener &>(
                      node->event_listener(event)))
    {}

    void operator()() const OPENVRML_NOTHROW
    {
        try {
            try {
                std::auto_ptr<resource_istream> in =
                    this->scene_->get_resource(*this->url_);
                if (!(*in)) { throw unreachable_url(); }
                mfnode nodes;
                nodes.value(
                    this->scene_->browser()
                    .create_vrml_from_stream(*in, in->type()));
                this->listener_->process_event(nodes, browser::current_time());
            } catch (std::exception & ex) {
                this->scene_->browser().err(ex.what());
                throw unreachable_url();
            } catch (...) {
                //
                // The implementation of resource_istream is provided by the
                // user; and unfortunately, operations on it could throw
                // anything.
                //
                throw unreachable_url();
            }
        } catch (std::exception & ex) {
            this->scene_->browser().err(ex.what());
        }
    }

private:
    openvrml::scene * const scene_;
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
 *                      sent as an event.
 * @param[in] event     the event of @p node to which the new nodes will be sent.
 *
 * @exception unsupported_interface         if @p node has no eventIn @p event.
 * @exception std::bad_cast                 if the @p event eventIn of @p node
 *                                          is not an MFNode.
 * @exception boost::thread_resource_error  if thread creation fails.
 */
void
openvrml::scene::
create_vrml_from_url(const std::vector<std::string> & url,
                     const boost::intrusive_ptr<node> & node,
                     const std::string & event)
    OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                    boost::thread_resource_error)
{
    boost::function0<void> f = vrml_from_url_creator(*this, url, node, event);
    this->stream_reader_threads_.create_thread(f);
}

/**
 * @brief Load a resource into @a browser.
 *
 * This method simply resolves any relative references in @p uri and calls
 * @c browser::load_url.
 *
 * @note There are a couple of edge cases here where we are probably doing the
 *      wrong thing:
 *       - If there is a URI in the list of the form
 *         &ldquo;<code>\#NodeId</code>&rdquo; and it is not the first URI in
 *         the list, this URI will be loaded as if it were a new world rather
 *         than as a Viewpoint that should simply be bound.
 *       - If the first URI in the list is of the form
 *         &ldquo;<code>\#NodeId</code>&rdquo; and no Viewpoint named
 *         &ldquo;<code>NodeId</code>&rdquo; exists in the scene, this method
 *         will not try any subsequent URIs in the list.
 *
 * @param[in] url       an array of URIs.  Per VRML97 convention, the first
 *                      resource in the sequence that can be reached will be
 *                      loaded into the browser.
 * @param[in] parameter an array of parameters to be associated with the URIs in
 *                      @p uri.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @todo This method currently fails silently if any of the URIs in @p url is
 *       invalid.  Should this throw invalid_url?
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
 * @c scene.
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
        //
        // If we have a relative reference, resolve it against this->url();
        // unless the parent is null and this->url() is empty, in which case
        // we are loading the root scene.  In that case, construct an absolute
        // file URL.
        //
        uri test_uri, absolute_uri;
        try {
            //
            // Throw invalid_url if it isn't a valid URI.
            //
            test_uri = uri(url[i]);

            absolute_uri = !relative(test_uri)
                         ? test_uri
                         : (!this->parent() && this->url().empty())
                             ? create_file_url(test_uri)
                             : test_uri.resolve_against(uri(this->url()));
            in = this->browser().fetcher_.get_resource(absolute_uri);
        } catch (invalid_url &) {
            std::ostringstream msg;
            msg << url[i] << ": invalid URI";
            this->browser().err(msg.str());
            continue;
        } catch (std::exception & ex) {
            std::ostringstream msg;
            msg << string(!absolute_uri.scheme().empty() ? absolute_uri
                                                         : test_uri)
                << ": " << ex.what();
            this->browser().err(msg.str());
            continue;
        } catch (...) {
            //
            // Swallow unrecognized exceptions.  Output to browser::err
            // happens below when "in" is found to be unusable.
            //
        }
        if (!in.get() || !(*in)) {
            std::ostringstream msg;
            msg << string(absolute_uri)
                << ": unrecognized error during resolution";
            this->browser().err(msg.str());
            continue;
        }
        break; // Success.
    }
    if (!in.get() || !(*in)) { throw no_alternative_url(); }
    return in;
}

/**
 * @brief Shut down the nodes in the scene.
 *
 * This function @b must be called before the @c scene is destroyed.
 *
 * @param[in] timestamp the current time.
 */
void openvrml::scene::shutdown(const double timestamp) OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->nodes_mutex_);
    for (std::vector<boost::intrusive_ptr<node> >::iterator n(
             this->nodes_.begin());
         n != this->nodes_.end();
         ++n) {
        if (*n) { (*n)->shutdown(timestamp); }
    }
}

/**
 * @brief Function called once the @c scene has been loaded.
 *
 * @c #load calls this function once the @c scene has finished loading.  The
 * default implementation does nothing.
 */
void openvrml::scene::scene_loaded()
{}


openvrml::null_node_metatype::null_node_metatype(openvrml::browser & browser)
    OPENVRML_NOTHROW:
    node_metatype("urn:X-openvrml:node:null", browser)
{}

openvrml::null_node_metatype::~null_node_metatype() OPENVRML_NOTHROW
{}

const boost::shared_ptr<openvrml::node_type>
openvrml::null_node_metatype::
do_create_type(const std::string &, const node_interface_set &) const
    OPENVRML_NOTHROW
{
    assert(false);
    static const boost::shared_ptr<node_type> nodeType;
    return nodeType;
}


openvrml::null_node_type::null_node_type(null_node_metatype & nodeClass)
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

    bool
    component::add_type_desc(openvrml::node_type_decls & type_descs,
                             const std::string & node_type_id,
                             const openvrml::node_interface_set & interfaces)
        OPENVRML_THROW1(std::bad_alloc)
    {
        return type_descs.insert(make_pair(node_type_id, interfaces)).second;
    }

    void component::add_scope_entry(
        const openvrml::browser & b,
        const char * node_name,
        const openvrml::node_interface_set & interface_set,
        const char * urn,
        openvrml::scope & scope) const
        OPENVRML_THROW3(openvrml::unsupported_interface, std::invalid_argument,
                        std::bad_alloc)
    {
        using boost::shared_ptr;
        using openvrml::node_metatype;
        using openvrml::node_type;

        const shared_ptr<node_metatype> class_ = b.node_metatype(urn);
        assert(class_);
        const shared_ptr<node_type> type = class_->create_type(node_name,
                                                               interface_set);
        const std::pair<shared_ptr<node_type>, bool> add_type_result =
            scope.add_type(type);

        //
        // If scope::add_type failed, there are two possible reasons:
        //  1. We're trying to add exactly the same node_type as already exists
        //     in the scope.  The mostly likely way this would happen is if a
        //     component is added redundantly.  There are entirely reasonable
        //     ways that could happen; e.g., if the profile incorporates
        //     component A level 1, and the X3D file also has a component
        //     statement to import component A level 2.
        //  2. node_types in two different components have the same
        //     node_type::id.
        //
        if (!add_type_result.second && *add_type_result.first != *type) {
            //
            // The way things are currently implemented, we don't want to throw
            // here.  Even if a component has a problematic node, we might as
            // well add the rest of the nodes in it.  After all, we don't know
            // how many nodes in the component we've already added.
            //
            // At some point it may be desirable to make component import
            // transactional--i.e., all nodes are added successfully, or
            // component import fails totally.  But that doesn't seem necessary
            // at this point.
            //
            b.err(std::string("interfaces for \"") + node_name + "\" are not "
                  "consistent with the existing node type by that name");
        }
    }

    void component::add_to_scope(const openvrml::browser & b,
                                 openvrml::scope & scope,
                                 const size_t level) const
        OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
    {
        if (level > this->support_level()) {
            throw std::invalid_argument("unsupported component level");
        }
        this->do_add_to_scope(b, scope, level);
    }

    void
    component::
    add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                              const size_t level) const
        OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
    {
        if (level == 0 || level > this->support_level()) {
            throw std::invalid_argument("unsupported component level");
        }
        this->do_add_to_node_type_desc_map(type_descs, level);
    }


    class OPENVRML_LOCAL vrml97_component : public component {
        static const boost::array<openvrml::node_interface, 8>
            anchor_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            appearance_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            audio_clip_interfaces_;
        static const boost::array<openvrml::node_interface, 12>
            background_interfaces_;
        static const boost::array<openvrml::node_interface, 6>
            billboard_interfaces_;
        static const boost::array<openvrml::node_interface, 1>
            box_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            collision_interfaces_;
        static const boost::array<openvrml::node_interface, 1>
            color_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            color_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            cone_interfaces_;
        static const boost::array<openvrml::node_interface, 1>
            coordinate_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            coordinate_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            cylinder_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            cylinder_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            directional_light_interfaces_;
        static const boost::array<openvrml::node_interface, 14>
            elevation_grid_interfaces_;
        static const boost::array<openvrml::node_interface, 14>
            extrusion_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            fog_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            font_style_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            group_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            image_texture_interfaces_;
        static const boost::array<openvrml::node_interface, 18>
            indexed_face_set_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            indexed_line_set_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            inline_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            lod_interfaces_;
        static const boost::array<openvrml::node_interface, 6>
            material_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            movie_texture_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            navigation_info_interfaces_;
        static const boost::array<openvrml::node_interface, 1>
            normal_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            normal_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            orientation_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            pixel_texture_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            plane_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            point_light_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            point_set_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            position_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            proximity_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            scalar_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            shape_interfaces_;
        static const boost::array<openvrml::node_interface, 10>
            sound_interfaces_;
        static const boost::array<openvrml::node_interface, 1>
            sphere_interfaces_;
        static const boost::array<openvrml::node_interface, 6>
            sphere_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 10>
            spot_light_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            switch_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            text_interfaces_;
        static const boost::array<openvrml::node_interface, 1>
            texture_coordinate_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            texture_transform_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            time_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            touch_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 10>
            transform_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            viewpoint_interfaces_;
        static const boost::array<openvrml::node_interface, 6>
            visibility_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            world_info_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & root_scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    using openvrml::node_interface;
    using openvrml::field_value;

    const boost::array<node_interface, 8>
    vrml97_component::anchor_interfaces_ = {
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

    const boost::array<node_interface, 3>
    vrml97_component::appearance_interfaces_ = {
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

    const boost::array<node_interface, 8>
    vrml97_component::audio_clip_interfaces_ = {
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

    const boost::array<node_interface, 12>
    vrml97_component::background_interfaces_ = {
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

    const boost::array<node_interface, 6>
    vrml97_component::billboard_interfaces_ = {
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

    const boost::array<node_interface, 1>
    vrml97_component::box_interfaces_ = {
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "size")
    };

    const boost::array<node_interface, 8>
    vrml97_component::collision_interfaces_ = {
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

    const boost::array<node_interface, 1>
    vrml97_component::color_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "color")
    };

    const boost::array<node_interface, 4>
    vrml97_component::color_interpolator_interfaces_ = {
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

    const boost::array<node_interface, 4>
    vrml97_component::cone_interfaces_ = {
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

    const boost::array<node_interface, 1>
    vrml97_component::coordinate_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "point")
    };

    const boost::array<node_interface, 4>
    vrml97_component::coordinate_interpolator_interfaces_ = {
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

    const boost::array<node_interface, 5>
    vrml97_component::cylinder_interfaces_ = {
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

    const boost::array<node_interface, 9>
    vrml97_component::cylinder_sensor_interfaces_ = {
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

    const boost::array<node_interface, 5>
    vrml97_component::directional_light_interfaces_ = {
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

    const boost::array<node_interface, 14>
    vrml97_component::elevation_grid_interfaces_ = {
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

    const boost::array<node_interface, 14>
    vrml97_component::extrusion_interfaces_ = {
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

    const boost::array<node_interface, 5>
    vrml97_component::fog_interfaces_ = {
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

    const boost::array<node_interface, 9>
    vrml97_component::font_style_interfaces_ = {
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

    const boost::array<node_interface, 5>
    vrml97_component::group_interfaces_ = {
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

    const boost::array<node_interface, 3>
    vrml97_component::image_texture_interfaces_ = {
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

    const boost::array<node_interface, 18>
    vrml97_component::indexed_face_set_interfaces_ = {
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

    const boost::array<node_interface, 7>
    vrml97_component::indexed_line_set_interfaces_ = {
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

    const boost::array<node_interface, 3>
    vrml97_component::inline_interfaces_ = {
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

    const boost::array<node_interface, 3>
    vrml97_component::lod_interfaces_ = {
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

    const boost::array<node_interface, 6>
    vrml97_component::material_interfaces_ = {
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

    const boost::array<node_interface, 9>
    vrml97_component::movie_texture_interfaces_ = {
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

    const boost::array<node_interface, 7>
    vrml97_component::navigation_info_interfaces_ = {
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

    const boost::array<node_interface, 1>
    vrml97_component::normal_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "vector")
    };

    const boost::array<node_interface, 4>
    vrml97_component::normal_interpolator_interfaces_ = {
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

    const boost::array<node_interface, 4>
    vrml97_component::orientation_interpolator_interfaces_ = {
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

    const boost::array<node_interface, 3>
    vrml97_component::pixel_texture_interfaces_ = {
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

    const boost::array<node_interface, 8>
    vrml97_component::plane_sensor_interfaces_ = {
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

    const boost::array<node_interface, 7>
    vrml97_component::point_light_interfaces_ = {
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

    const boost::array<node_interface, 2>
    vrml97_component::point_set_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord")
    };

    const boost::array<node_interface, 4>
    vrml97_component::position_interpolator_interfaces_ = {
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

    const boost::array<node_interface, 8>
    vrml97_component::proximity_sensor_interfaces_ = {
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

    const boost::array<node_interface, 4>
    vrml97_component::scalar_interpolator_interfaces_ = {
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

    const boost::array<node_interface, 2>
    vrml97_component::shape_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "appearance"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "geometry")
    };

    const boost::array<node_interface, 10>
    vrml97_component::sound_interfaces_ = {
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

    const boost::array<node_interface, 1>
    vrml97_component::sphere_interfaces_ = {
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "radius")
    };

    const boost::array<node_interface, 6>
    vrml97_component::sphere_sensor_interfaces_ = {
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

    const boost::array<node_interface, 10>
    vrml97_component::spot_light_interfaces_ = {
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

    const boost::array<node_interface, 2>
    vrml97_component::switch_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "choice"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "whichChoice")
    };

    const boost::array<node_interface, 4>
    vrml97_component::text_interfaces_ = {
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

    const boost::array<node_interface, 1>
    vrml97_component::texture_coordinate_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec2f_id,
                       "point")
    };

    const boost::array<node_interface, 4>
    vrml97_component::texture_transform_interfaces_ = {
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

    const boost::array<node_interface, 9>
    vrml97_component::time_sensor_interfaces_ = {
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

    const boost::array<node_interface, 7>
    vrml97_component::touch_sensor_interfaces_ = {
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

    const boost::array<node_interface, 10>
    vrml97_component::transform_interfaces_ = {
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

    const boost::array<node_interface, 8>
    vrml97_component::viewpoint_interfaces_ = {
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

    const boost::array<node_interface, 6>
    vrml97_component::visibility_sensor_interfaces_ = {
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

    const boost::array<node_interface, 2>
    vrml97_component::world_info_interfaces_ = {
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "info"),
        node_interface(node_interface::field_id,
                       field_value::sfstring_id,
                       "title")
    };
    
    const char * const vrml97_component::id = "VRML97";

    size_t vrml97_component::support_level() const OPENVRML_NOTHROW
    {
        return 1;
    }

    void vrml97_component::do_add_to_scope(const openvrml::browser & b,
                                           openvrml::scope & scope,
                                           size_t) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        //
        // Anchor node
        //
        {
            static const node_interface_set
                interface_set(vrml97_component::anchor_interfaces_.begin(),
                              vrml97_component::anchor_interfaces_.end());

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
            static const node_interface_set
                interface_set(vrml97_component::appearance_interfaces_.begin(),
                              vrml97_component::appearance_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::audio_clip_interfaces_.begin(),
                              vrml97_component::audio_clip_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::background_interfaces_.begin(),
                              vrml97_component::background_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::billboard_interfaces_.begin(),
                              vrml97_component::billboard_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::box_interfaces_.begin(),
                              vrml97_component::box_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::collision_interfaces_.begin(),
                              vrml97_component::collision_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::color_interfaces_.begin(),
                              vrml97_component::color_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::color_interpolator_interfaces_.begin(),
                              vrml97_component::color_interpolator_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::cone_interfaces_.begin(),
                              vrml97_component::cone_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::coordinate_interfaces_.begin(),
                              vrml97_component::coordinate_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::coordinate_interpolator_interfaces_.begin(),
                              vrml97_component::coordinate_interpolator_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::cylinder_interfaces_.begin(),
                              vrml97_component::cylinder_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::cylinder_sensor_interfaces_.begin(),
                              vrml97_component::cylinder_sensor_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::directional_light_interfaces_.begin(),
                              vrml97_component::directional_light_interfaces_.end());
            add_scope_entry(b,
                            "DirectionalLight",
                            interface_set,
                            "urn:X-openvrml:node:DirectionalLight",
                            scope);
        }

        //
        // ElevationGrid node
        //
        {
            static const node_interface_set
                interface_set(vrml97_component::elevation_grid_interfaces_.begin(),
                              vrml97_component::elevation_grid_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::extrusion_interfaces_.begin(),
                              vrml97_component::extrusion_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::fog_interfaces_.begin(),
                              vrml97_component::fog_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::font_style_interfaces_.begin(),
                              vrml97_component::font_style_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::group_interfaces_.begin(),
                              vrml97_component::group_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::image_texture_interfaces_.begin(),
                              vrml97_component::image_texture_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::indexed_face_set_interfaces_.begin(),
                              vrml97_component::indexed_face_set_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::indexed_line_set_interfaces_.begin(),
                              vrml97_component::indexed_line_set_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::inline_interfaces_.begin(),
                              vrml97_component::inline_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::lod_interfaces_.begin(),
                              vrml97_component::lod_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::material_interfaces_.begin(),
                              vrml97_component::material_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::movie_texture_interfaces_.begin(),
                    vrml97_component::movie_texture_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::navigation_info_interfaces_.begin(),
                    vrml97_component::navigation_info_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::normal_interfaces_.begin(),
                              vrml97_component::normal_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::normal_interpolator_interfaces_.begin(),
                    vrml97_component::normal_interpolator_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::orientation_interpolator_interfaces_.begin(),
                              vrml97_component::orientation_interpolator_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::pixel_texture_interfaces_.begin(),
                              vrml97_component::pixel_texture_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::plane_sensor_interfaces_.begin(),
                              vrml97_component::plane_sensor_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::point_light_interfaces_.begin(),
                              vrml97_component::point_light_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::point_set_interfaces_.begin(),
                              vrml97_component::point_set_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::position_interpolator_interfaces_.begin(),
                    vrml97_component::position_interpolator_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::proximity_sensor_interfaces_.begin(),
                    vrml97_component::proximity_sensor_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::scalar_interpolator_interfaces_.begin(),
                    vrml97_component::scalar_interpolator_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::shape_interfaces_.begin(),
                              vrml97_component::shape_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::sound_interfaces_.begin(),
                              vrml97_component::sound_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::sphere_interfaces_.begin(),
                              vrml97_component::sphere_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::sphere_sensor_interfaces_.begin(),
                    vrml97_component::sphere_sensor_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::spot_light_interfaces_.begin(),
                              vrml97_component::spot_light_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::switch_interfaces_.begin(),
                              vrml97_component::switch_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::text_interfaces_.begin(),
                              vrml97_component::text_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::texture_coordinate_interfaces_.begin(),
                    vrml97_component::texture_coordinate_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::texture_transform_interfaces_.begin(),
                    vrml97_component::texture_transform_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::time_sensor_interfaces_.begin(),
                    vrml97_component::time_sensor_interfaces_.end());
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
            static const node_interface_set
                interface_set(
                    vrml97_component::touch_sensor_interfaces_.begin(),
                    vrml97_component::touch_sensor_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::transform_interfaces_.begin(),
                              vrml97_component::transform_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::viewpoint_interfaces_.begin(),
                              vrml97_component::viewpoint_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::visibility_sensor_interfaces_.begin(),
                              vrml97_component::visibility_sensor_interfaces_.end());
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
            static const node_interface_set
                interface_set(vrml97_component::world_info_interfaces_.begin(),
                              vrml97_component::world_info_interfaces_.end());
            add_scope_entry(b,
                            "WorldInfo",
                            interface_set,
                            "urn:X-openvrml:node:WorldInfo",
                            scope);
        }
    }

    void
    vrml97_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "Anchor",
            node_interface_set(vrml97_component::anchor_interfaces_.begin(),
                               vrml97_component::anchor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Appearance",
            node_interface_set(
                vrml97_component::appearance_interfaces_.begin(),
                vrml97_component::appearance_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "AudioClip",
            node_interface_set(
                vrml97_component::audio_clip_interfaces_.begin(),
                vrml97_component::audio_clip_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Background",
            node_interface_set(
                vrml97_component::background_interfaces_.begin(),
                vrml97_component::background_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Billboard",
            node_interface_set(vrml97_component::billboard_interfaces_.begin(),
                               vrml97_component::billboard_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Box",
            node_interface_set(vrml97_component::box_interfaces_.begin(),
                               vrml97_component::box_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Collision",
            node_interface_set(vrml97_component::collision_interfaces_.begin(),
                               vrml97_component::collision_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Color",
            node_interface_set(vrml97_component::color_interfaces_.begin(),
                               vrml97_component::color_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "ColorInterpolator",
            node_interface_set(
                vrml97_component::color_interpolator_interfaces_.begin(),
                vrml97_component::color_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Cone",
            node_interface_set(vrml97_component::cone_interfaces_.begin(),
                               vrml97_component::cone_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Coordinate",
            node_interface_set(
                vrml97_component::coordinate_interfaces_.begin(),
                vrml97_component::coordinate_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "CoordinateInterpolator",
            node_interface_set(
                vrml97_component::coordinate_interpolator_interfaces_.begin(),
                vrml97_component::coordinate_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Cylinder",
            node_interface_set(
                vrml97_component::cylinder_interfaces_.begin(),
                vrml97_component::cylinder_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "CylinderSensor",
            node_interface_set(
                vrml97_component::cylinder_sensor_interfaces_.begin(),
                vrml97_component::cylinder_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "DirectionalLight",
            node_interface_set(
                vrml97_component::directional_light_interfaces_.begin(),
                vrml97_component::directional_light_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "ElevationGrid",
            node_interface_set(
                vrml97_component::elevation_grid_interfaces_.begin(),
                vrml97_component::elevation_grid_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Extrusion",
            node_interface_set(
                vrml97_component::extrusion_interfaces_.begin(),
                vrml97_component::extrusion_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Fog",
            node_interface_set(
                vrml97_component::fog_interfaces_.begin(),
                vrml97_component::fog_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "FontStyle",
            node_interface_set(
                vrml97_component::font_style_interfaces_.begin(),
                vrml97_component::font_style_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Group",
            node_interface_set(vrml97_component::group_interfaces_.begin(),
                               vrml97_component::group_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "ImageTexture",
            node_interface_set(
                vrml97_component::image_texture_interfaces_.begin(),
                vrml97_component::image_texture_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "IndexedFaceSet",
            node_interface_set(
                vrml97_component::indexed_face_set_interfaces_.begin(),
                vrml97_component::indexed_face_set_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "IndexedLineSet",
            node_interface_set(
                vrml97_component::indexed_line_set_interfaces_.begin(),
                vrml97_component::indexed_line_set_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Inline",
            node_interface_set(
                vrml97_component::inline_interfaces_.begin(),
                vrml97_component::inline_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "LOD",
            node_interface_set(
                vrml97_component::lod_interfaces_.begin(),
                vrml97_component::lod_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Material",
            node_interface_set(
                vrml97_component::material_interfaces_.begin(),
                vrml97_component::material_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "MovieTexture",
            node_interface_set(
                vrml97_component::movie_texture_interfaces_.begin(),
                vrml97_component::movie_texture_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "NavigationInfo",
            node_interface_set(
                vrml97_component::navigation_info_interfaces_.begin(),
                vrml97_component::navigation_info_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Normal",
            node_interface_set(vrml97_component::normal_interfaces_.begin(),
                               vrml97_component::normal_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "NormalInterpolator",
            node_interface_set(
                vrml97_component::normal_interpolator_interfaces_.begin(),
                vrml97_component::normal_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "OrientationInterpolator",
            node_interface_set(
                vrml97_component::orientation_interpolator_interfaces_.begin(),
                vrml97_component::orientation_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PixelTexture",
            node_interface_set(
                vrml97_component::pixel_texture_interfaces_.begin(),
                vrml97_component::pixel_texture_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PlaneSensor",
            node_interface_set(
                vrml97_component::plane_sensor_interfaces_.begin(),
                vrml97_component::plane_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PointLight",
            node_interface_set(
                vrml97_component::point_light_interfaces_.begin(),
                vrml97_component::point_light_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PointSet",
            node_interface_set(
                vrml97_component::point_set_interfaces_.begin(),
                vrml97_component::point_set_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PositionInterpolator",
            node_interface_set(
                vrml97_component::position_interpolator_interfaces_.begin(),
                vrml97_component::position_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "ProximitySensor",
            node_interface_set(
                vrml97_component::proximity_sensor_interfaces_.begin(),
                vrml97_component::proximity_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "ScalarInterpolator",
            node_interface_set(
                vrml97_component::scalar_interpolator_interfaces_.begin(),
                vrml97_component::scalar_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Shape",
            node_interface_set(vrml97_component::shape_interfaces_.begin(),
                               vrml97_component::shape_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Sound",
            node_interface_set(vrml97_component::sound_interfaces_.begin(),
                               vrml97_component::sound_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Sphere",
            node_interface_set(vrml97_component::sphere_interfaces_.begin(),
                               vrml97_component::sphere_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "SphereSensor",
            node_interface_set(vrml97_component::sphere_sensor_interfaces_.begin(),
                               vrml97_component::sphere_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "SpotLight",
            node_interface_set(vrml97_component::spot_light_interfaces_.begin(),
                               vrml97_component::spot_light_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Switch",
            node_interface_set(vrml97_component::switch_interfaces_.begin(),
                               vrml97_component::switch_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Text",
            node_interface_set(vrml97_component::text_interfaces_.begin(),
                               vrml97_component::text_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TextureCoordinate",
            node_interface_set(vrml97_component::texture_coordinate_interfaces_.begin(),
                               vrml97_component::texture_coordinate_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TextureTransform",
            node_interface_set(vrml97_component::texture_transform_interfaces_.begin(),
                               vrml97_component::texture_transform_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TimeSensor",
            node_interface_set(vrml97_component::time_sensor_interfaces_.begin(),
                               vrml97_component::time_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TouchSensor",
            node_interface_set(vrml97_component::touch_sensor_interfaces_.begin(),
                               vrml97_component::touch_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Transform",
            node_interface_set(vrml97_component::transform_interfaces_.begin(),
                               vrml97_component::transform_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Viewpoint",
            node_interface_set(vrml97_component::viewpoint_interfaces_.begin(),
                               vrml97_component::viewpoint_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "VisibilitySensor",
            node_interface_set(vrml97_component::visibility_sensor_interfaces_.begin(),
                               vrml97_component::visibility_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "WorldInfo",
            node_interface_set(vrml97_component::world_info_interfaces_.begin(),
                               vrml97_component::world_info_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_core_component : public component {
        static const boost::array<openvrml::node_interface, 4>
            metadata_double_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            metadata_float_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            metadata_integer_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            metadata_set_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            metadata_string_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs,
                size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<node_interface, 4>
    x3d_core_component::metadata_double_interfaces_ = {
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

    const boost::array<node_interface, 4>
    x3d_core_component::metadata_float_interfaces_ = {
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

    const boost::array<node_interface, 4>
    x3d_core_component::metadata_integer_interfaces_ = {
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

    const boost::array<node_interface, 4>
    x3d_core_component::metadata_set_interfaces_ = {
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

    const boost::array<node_interface, 4>
    x3d_core_component::metadata_string_interfaces_ = {
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

    const char * const x3d_core_component::id = "Core";

    size_t x3d_core_component::support_level() const OPENVRML_NOTHROW
    {
        return 2;
    }

    void x3d_core_component::do_add_to_scope(const openvrml::browser & b,
                                             openvrml::scope & scope,
                                             const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            {
                static const node_interface_set
                    interface_set(
                        x3d_core_component::metadata_double_interfaces_.begin(),
                        x3d_core_component::metadata_double_interfaces_.end());
                add_scope_entry(b,
                                "MetadataDouble",
                                interface_set,
                                "urn:X-openvrml:node:MetadataDouble",
                                scope);
            }

            {
                static const node_interface_set
                    interface_set(
                        x3d_core_component::metadata_float_interfaces_.begin(),
                        x3d_core_component::metadata_float_interfaces_.end());
                add_scope_entry(b,
                                "MetadataFloat",
                                interface_set,
                                "urn:X-openvrml:node:MetadataFloat",
                                scope);
            }

            {
                static const node_interface_set
                    interface_set(
                        x3d_core_component::metadata_integer_interfaces_.begin(),
                        x3d_core_component::metadata_integer_interfaces_.end());
                add_scope_entry(b,
                                "MetadataInteger",
                                interface_set,
                                "urn:X-openvrml:node:MetadataInteger",
                                scope);
            }


            {
                static const node_interface_set
                    interface_set(
                        x3d_core_component::metadata_set_interfaces_.begin(),
                        x3d_core_component::metadata_set_interfaces_.end());
                add_scope_entry(b,
                                "MetadataSet",
                                interface_set,
                                "urn:X-openvrml:node:MetadataSet",
                                scope);
            }

            {
                static const node_interface_set
                    interface_set(
                        x3d_core_component::metadata_string_interfaces_.begin(),
                        x3d_core_component::metadata_string_interfaces_.end());
                add_scope_entry(b,
                                "MetadataString",
                                interface_set,
                                "urn:X-openvrml:node:MetadataString",
                                scope);
            }
        }
    }

    void
    x3d_core_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "MetadataDouble",
            node_interface_set(
                x3d_core_component::metadata_double_interfaces_.begin(),
                x3d_core_component::metadata_double_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "MetadataFloat",
            node_interface_set(
                x3d_core_component::metadata_float_interfaces_.begin(),
                x3d_core_component::metadata_float_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "MetadataInteger",
            node_interface_set(
                x3d_core_component::metadata_integer_interfaces_.begin(),
                x3d_core_component::metadata_integer_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "MetadataSet",
            node_interface_set(
                x3d_core_component::metadata_set_interfaces_.begin(),
                x3d_core_component::metadata_set_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "MetadataString",
            node_interface_set(
                x3d_core_component::metadata_string_interfaces_.begin(),
                x3d_core_component::metadata_string_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_time_component : public component {
        static const boost::array<openvrml::node_interface, 14>
            time_sensor_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<node_interface, 14>
    x3d_time_component::time_sensor_interfaces_ = {
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

    const char * const x3d_time_component::id = "Time";

    size_t x3d_time_component::support_level() const OPENVRML_NOTHROW
    {
        return 2;
    }

    void x3d_time_component::do_add_to_scope(const openvrml::browser & b,
                                             openvrml::scope & scope,
                                             const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        //
        // TimeSensor node
        //
        if (level >= 1) {
            static const node_interface_set
                interface_set(
                    x3d_time_component::time_sensor_interfaces_.begin(),
                    x3d_time_component::time_sensor_interfaces_.end());
            add_scope_entry(b,
                            "TimeSensor",
                            interface_set,
                            "urn:X-openvrml:node:TimeSensor",
                            scope);
        }
    }

    void
    x3d_time_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "TimeSensor",
            node_interface_set(
                x3d_time_component::time_sensor_interfaces_.begin(),
                x3d_time_component::time_sensor_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_networking_component : public component {
        static const boost::array<openvrml::node_interface, 9>
            anchor_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            inline_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            load_sensor_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 9>
    x3d_networking_component::anchor_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_networking_component::inline_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 8>
    x3d_networking_component::load_sensor_interfaces_ = {
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

    const char * const x3d_networking_component::id = "Networking";

    size_t x3d_networking_component::support_level() const OPENVRML_NOTHROW
    {
        return 3;
    }

    void
    x3d_networking_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 2) {
            //
            // Anchor node
            //
            {
                static const node_interface_set
                    interface_set(x3d_networking_component::anchor_interfaces_.begin(),
                                  x3d_networking_component::anchor_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_networking_component::anchor_interfaces_.begin(),
                                  x3d_networking_component::anchor_interfaces_.end());
                add_scope_entry(b,
                                "Inline",
                                interface_set,
                                "urn:X-openvrml:node:Inline",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // LoadSensor node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_networking_component::load_sensor_interfaces_.begin(),
                        x3d_networking_component::load_sensor_interfaces_.end());
                add_scope_entry(b,
                                "LoadSensor",
                                interface_set,
                                "urn:X-openvrml:node:LoadSensor",
                                scope);
            }
        }
    }

    void
    x3d_networking_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "Anchor",
                node_interface_set(
                    x3d_networking_component::anchor_interfaces_.begin(),
                    x3d_networking_component::anchor_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "Inline",
                node_interface_set(
                    x3d_networking_component::inline_interfaces_.begin(),
                    x3d_networking_component::inline_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "LoadSensor",
                node_interface_set(
                    x3d_networking_component::load_sensor_interfaces_.begin(),
                    x3d_networking_component::load_sensor_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_grouping_component : public component {
        static const boost::array<openvrml::node_interface, 6>
            group_interfaces_;
        static const boost::array<openvrml::node_interface, 11>
            transform_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            world_info_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            switch_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            static_group_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 6>
    x3d_grouping_component::group_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 11>
    x3d_grouping_component::transform_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 3>
    x3d_grouping_component::world_info_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_grouping_component::switch_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 4>
    x3d_grouping_component::static_group_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const char * const x3d_grouping_component::id = "Grouping";

    size_t x3d_grouping_component::support_level() const OPENVRML_NOTHROW
    {
        return 3;
    }

    void
    x3d_grouping_component::do_add_to_scope(const openvrml::browser & b,
                                            openvrml::scope & scope,
                                            const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // Group node
            //
            {
                static const node_interface_set
                    interface_set(x3d_grouping_component::group_interfaces_.begin(),
                                  x3d_grouping_component::group_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_grouping_component::transform_interfaces_.begin(),
                                  x3d_grouping_component::transform_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_grouping_component::world_info_interfaces_.begin(),
                                  x3d_grouping_component::world_info_interfaces_.end());
                add_scope_entry(b,
                                "WorldInfo",
                                interface_set,
                                "urn:X-openvrml:node:WorldInfo",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // Switch node
            //
            {
                static const node_interface_set
                    interface_set(x3d_grouping_component::switch_interfaces_.begin(),
                                  x3d_grouping_component::switch_interfaces_.end());
                add_scope_entry(b,
                                "Switch",
                                interface_set,
                                "urn:X-openvrml:node:Switch",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // StaticGroup node
            //
            {
                static const node_interface_set
                    interface_set(x3d_grouping_component::static_group_interfaces_.begin(),
                                  x3d_grouping_component::static_group_interfaces_.end());
                add_scope_entry(b,
                                "StaticGroup",
                                interface_set,
                                "urn:X-openvrml:node:StaticGroup",
                                scope);
            }
        }
    }

    void
    x3d_grouping_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "Group",
            node_interface_set(
                x3d_grouping_component::group_interfaces_.begin(),
                x3d_grouping_component::group_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Transform",
            node_interface_set(
                x3d_grouping_component::transform_interfaces_.begin(),
                x3d_grouping_component::transform_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "WorldInfo",
            node_interface_set(
                x3d_grouping_component::world_info_interfaces_.begin(),
                x3d_grouping_component::world_info_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "Switch",
                node_interface_set(
                    x3d_grouping_component::switch_interfaces_.begin(),
                    x3d_grouping_component::switch_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "StaticGroup",
                node_interface_set(
                    x3d_grouping_component::static_group_interfaces_.begin(),
                    x3d_grouping_component::static_group_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_rendering_component : public component {
        static const boost::array<node_interface, 2>
            color_interfaces_;
        static const boost::array<node_interface, 2>
            color_rgba_interfaces_;
        static const boost::array<node_interface, 2>
            coordinate_interfaces_;
        static const boost::array<node_interface, 8>
            indexed_line_set_interfaces_;
        static const boost::array<node_interface, 4>
            line_set_interfaces_;
        static const boost::array<node_interface, 3>
            point_set_interfaces_;
        static const boost::array<node_interface, 2>
            normal_interfaces_;
        static const boost::array<node_interface, 11>
            indexed_triangle_fan_set_interfaces_;
        static const boost::array<node_interface, 11>
            indexed_triangle_set_interfaces_;
        static const boost::array<node_interface, 12>
            indexed_triangle_strip_set_interfaces_;
        static const boost::array<node_interface, 10>
            triangle_fan_set_interfaces_;
        static const boost::array<node_interface, 9>
            triangle_set_interfaces_;
        static const boost::array<node_interface, 10>
            triangle_strip_set_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<node_interface, 2>
    x3d_rendering_component::color_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "color")
    };

    const boost::array<node_interface, 2>
    x3d_rendering_component::color_rgba_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolorrgba_id,
                       "color")
    };

    const boost::array<node_interface, 2>
    x3d_rendering_component::coordinate_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "point")
    };

    const boost::array<node_interface, 8>
    x3d_rendering_component::indexed_line_set_interfaces_ = {
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

    const boost::array<node_interface, 4>
    x3d_rendering_component::line_set_interfaces_ = {
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

    const boost::array<node_interface, 3>
    x3d_rendering_component::point_set_interfaces_ = {
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

    const boost::array<node_interface, 2>
    x3d_rendering_component::normal_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "vector")
    };

    const boost::array<node_interface, 11>
    x3d_rendering_component::indexed_triangle_fan_set_interfaces_ = {
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

    const boost::array<node_interface, 11>
    x3d_rendering_component::indexed_triangle_set_interfaces_ = {
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

    const boost::array<node_interface, 12>
    x3d_rendering_component::indexed_triangle_strip_set_interfaces_ = {
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

    const boost::array<node_interface, 10>
    x3d_rendering_component::triangle_fan_set_interfaces_ = {
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

    const boost::array<node_interface, 9>
    x3d_rendering_component::triangle_set_interfaces_ = {
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

    const boost::array<node_interface, 10>
    x3d_rendering_component::triangle_strip_set_interfaces_ = {
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

    const char * const x3d_rendering_component::id = "Rendering";

    size_t x3d_rendering_component::support_level() const OPENVRML_NOTHROW
    {
        return 4;
    }

    void
    x3d_rendering_component::do_add_to_scope(const openvrml::browser & b,
                                             openvrml::scope & scope,
                                             const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // Color node
            //
            {
                static const node_interface_set
                    interface_set(x3d_rendering_component::color_interfaces_.begin(),
                                  x3d_rendering_component::color_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::color_rgba_interfaces_.begin(),
                                  x3d_rendering_component::color_rgba_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::coordinate_interfaces_.begin(),
                                  x3d_rendering_component::coordinate_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::indexed_line_set_interfaces_.begin(),
                                  x3d_rendering_component::indexed_line_set_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::line_set_interfaces_.begin(),
                                  x3d_rendering_component::line_set_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::point_set_interfaces_.begin(),
                                  x3d_rendering_component::point_set_interfaces_.end());
                add_scope_entry(b,
                                "PointSet",
                                interface_set,
                                "urn:X-openvrml:node:PointSet",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // Normal node
            //
            {
                static const node_interface_set
                    interface_set(x3d_rendering_component::normal_interfaces_.begin(),
                                  x3d_rendering_component::normal_interfaces_.end());
                add_scope_entry(b,
                                "Normal",
                                interface_set,
                                "urn:X-openvrml:node:Normal",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // IndexedTriangleFanSet node
            //
            {
                static const node_interface_set
                    interface_set(x3d_rendering_component::indexed_triangle_fan_set_interfaces_.begin(),
                                  x3d_rendering_component::indexed_triangle_fan_set_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::indexed_triangle_set_interfaces_.begin(),
                                  x3d_rendering_component::indexed_triangle_set_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::indexed_triangle_strip_set_interfaces_.begin(),
                                  x3d_rendering_component::indexed_triangle_strip_set_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::triangle_fan_set_interfaces_.begin(),
                                  x3d_rendering_component::triangle_fan_set_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::triangle_set_interfaces_.begin(),
                                  x3d_rendering_component::triangle_set_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_rendering_component::triangle_strip_set_interfaces_.begin(),
                                  x3d_rendering_component::triangle_strip_set_interfaces_.end());
                add_scope_entry(b,
                                "TriangleStripSet",
                                interface_set,
                                "urn:X-openvrml:node:TriangleStripSet",
                                scope);
            }
        }
    }

    void
    x3d_rendering_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "Color",
            node_interface_set(
                x3d_rendering_component::color_interfaces_.begin(),
                x3d_rendering_component::color_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "ColorRGBA",
            node_interface_set(
                x3d_rendering_component::color_interfaces_.begin(),
                x3d_rendering_component::color_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Coordinate",
            node_interface_set(
                x3d_rendering_component::coordinate_interfaces_.begin(),
                x3d_rendering_component::coordinate_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "IndexedLineSet",
            node_interface_set(
                x3d_rendering_component::indexed_line_set_interfaces_.begin(),
                x3d_rendering_component::indexed_line_set_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "LineSet",
            node_interface_set(
                x3d_rendering_component::line_set_interfaces_.begin(),
                x3d_rendering_component::line_set_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PointSet",
            node_interface_set(
                x3d_rendering_component::point_set_interfaces_.begin(),
                x3d_rendering_component::point_set_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "Normal",
                node_interface_set(
                    x3d_rendering_component::normal_interfaces_.begin(),
                    x3d_rendering_component::normal_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "IndexedTriangleFanSet",
                node_interface_set(
                    x3d_rendering_component::indexed_triangle_fan_set_interfaces_.begin(),
                    x3d_rendering_component::indexed_triangle_fan_set_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "IndexedTriangleSet",
                node_interface_set(
                    x3d_rendering_component::indexed_triangle_set_interfaces_.begin(),
                    x3d_rendering_component::indexed_triangle_set_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "IndexedTriangleStripSet",
                node_interface_set(
                    x3d_rendering_component::indexed_triangle_strip_set_interfaces_.begin(),
                    x3d_rendering_component::indexed_triangle_strip_set_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "TriangleFanSet",
                node_interface_set(
                    x3d_rendering_component::triangle_fan_set_interfaces_.begin(),
                    x3d_rendering_component::triangle_fan_set_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "TriangleSet",
                node_interface_set(
                    x3d_rendering_component::triangle_set_interfaces_.begin(),
                    x3d_rendering_component::triangle_set_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "TriangleStripSet",
                node_interface_set(
                    x3d_rendering_component::triangle_strip_set_interfaces_.begin(),
                    x3d_rendering_component::triangle_strip_set_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_shape_component : public component {
        static const boost::array<openvrml::node_interface, 6>
            appearance_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            material_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            shape_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            line_properties_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            fill_properties_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 6>
    x3d_shape_component::appearance_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_shape_component::material_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_shape_component::shape_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 4>
    x3d_shape_component::line_properties_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_shape_component::fill_properties_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "filled"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "hatchColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "hatched"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "hatchStyle")
    };

    const char * const x3d_shape_component::id = "Shape";

    size_t x3d_shape_component::support_level() const OPENVRML_NOTHROW
    {
        return 3;
    }

    void x3d_shape_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // Appearance node
            //
            {
                static const node_interface_set
                    interface_set(x3d_shape_component::appearance_interfaces_.begin(),
                                  x3d_shape_component::appearance_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_shape_component::material_interfaces_.begin(),
                                  x3d_shape_component::material_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_shape_component::shape_interfaces_.begin(),
                                  x3d_shape_component::shape_interfaces_.end());
                add_scope_entry(b,
                                "Shape",
                                interface_set,
                                "urn:X-openvrml:node:Shape",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // LineProperties node
            //
            {
                static const node_interface_set
                    interface_set(x3d_shape_component::line_properties_interfaces_.begin(),
                                  x3d_shape_component::line_properties_interfaces_.end());
                add_scope_entry(b,
                                "LineProperties",
                                interface_set,
                                "urn:X-openvrml:node:LineProperties",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // FillProperties node
            //
            {
                static const node_interface_set
                    interface_set(x3d_shape_component::fill_properties_interfaces_.begin(),
                                  x3d_shape_component::fill_properties_interfaces_.end());
                add_scope_entry(b,
                                "FillProperties",
                                interface_set,
                                "urn:X-openvrml:node:FillProperties",
                                scope);
            }
        }
    }

    void
    x3d_shape_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "Appearance",
            node_interface_set(
                x3d_shape_component::appearance_interfaces_.begin(),
                x3d_shape_component::appearance_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Material",
            node_interface_set(
                x3d_shape_component::material_interfaces_.begin(),
                x3d_shape_component::material_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Shape",
            node_interface_set(
                x3d_shape_component::shape_interfaces_.begin(),
                x3d_shape_component::shape_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "LineProperties",
                node_interface_set(
                    x3d_shape_component::line_properties_interfaces_.begin(),
                    x3d_shape_component::line_properties_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "FillProperties",
                node_interface_set(
                    x3d_shape_component::fill_properties_interfaces_.begin(),
                    x3d_shape_component::fill_properties_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_geometry3d_component : public component {
        static const boost::array<openvrml::node_interface, 3> box_interfaces_;
        static const boost::array<openvrml::node_interface, 6>
            cone_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            cylinder_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            sphere_interfaces_;
        static const boost::array<openvrml::node_interface, 19>
            indexed_face_set_interfaces_;
        static const boost::array<openvrml::node_interface, 15>
            elevation_grid_interfaces_;
        static const boost::array<openvrml::node_interface, 15>
            extrusion_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 3>
    x3d_geometry3d_component::box_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 6>
    x3d_geometry3d_component::cone_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_geometry3d_component::cylinder_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 3>
    x3d_geometry3d_component::sphere_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 19>
    x3d_geometry3d_component::indexed_face_set_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 15>
    x3d_geometry3d_component::elevation_grid_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 15>
    x3d_geometry3d_component::extrusion_interfaces_ = {
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

    const char * const x3d_geometry3d_component::id = "Geometry3D";

    size_t x3d_geometry3d_component::support_level() const OPENVRML_NOTHROW
    {
        return 4;
    }

    void
    x3d_geometry3d_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // Box node
            //
            {
                static const node_interface_set
                    interface_set(x3d_geometry3d_component::box_interfaces_.begin(),
                                  x3d_geometry3d_component::box_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_geometry3d_component::cone_interfaces_.begin(),
                                  x3d_geometry3d_component::cone_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_geometry3d_component::cylinder_interfaces_.begin(),
                                  x3d_geometry3d_component::cylinder_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_geometry3d_component::sphere_interfaces_.begin(),
                                  x3d_geometry3d_component::sphere_interfaces_.end());
                add_scope_entry(b,
                                "Sphere",
                                interface_set,
                                "urn:X-openvrml:node:Sphere",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // IndexedFaceSet node
            //
            {
                static const node_interface_set
                    interface_set(x3d_geometry3d_component::indexed_face_set_interfaces_.begin(),
                                  x3d_geometry3d_component::indexed_face_set_interfaces_.end());
                add_scope_entry(b,
                                "IndexedFaceSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedFaceSet",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // ElevationGrid node
            //
            {
                static const node_interface_set
                    interface_set(x3d_geometry3d_component::elevation_grid_interfaces_.begin(),
                                  x3d_geometry3d_component::elevation_grid_interfaces_.end());
                add_scope_entry(b,
                                "ElevationGrid",
                                interface_set,
                                "urn:X-openvrml:node:ElevationGrid",
                                scope);
            }
        }

        if (level >= 4) {
            //
            // Extrusion node
            //
            {
                static const node_interface_set
                    interface_set(x3d_geometry3d_component::extrusion_interfaces_.begin(),
                                  x3d_geometry3d_component::extrusion_interfaces_.end());
                add_scope_entry(b,
                                "Extrusion",
                                interface_set,
                                "urn:X-openvrml:node:Extrusion",
                                scope);
            }
        }
    }

    void
    x3d_geometry3d_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "Box",
            node_interface_set(
                x3d_geometry3d_component::box_interfaces_.begin(),
                x3d_geometry3d_component::box_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Cone",
            node_interface_set(
                x3d_geometry3d_component::cone_interfaces_.begin(),
                x3d_geometry3d_component::cone_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Cylinder",
            node_interface_set(
                x3d_geometry3d_component::cylinder_interfaces_.begin(),
                x3d_geometry3d_component::cylinder_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Sphere",
            node_interface_set(
                x3d_geometry3d_component::sphere_interfaces_.begin(),
                x3d_geometry3d_component::sphere_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "IndexedFaceSet",
                node_interface_set(
                    x3d_geometry3d_component::indexed_face_set_interfaces_.begin(),
                    x3d_geometry3d_component::indexed_face_set_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "ElevationGrid",
                node_interface_set(
                    x3d_geometry3d_component::elevation_grid_interfaces_.begin(),
                    x3d_geometry3d_component::elevation_grid_interfaces_.end()));
        }

        if (level >= 4) {
            component::add_type_desc(
                type_descs,
                "Extrusion",
                node_interface_set(
                    x3d_geometry3d_component::extrusion_interfaces_.begin(),
                    x3d_geometry3d_component::extrusion_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_geometry2d_component : public component {
        static const boost::array<openvrml::node_interface, 2>
            polyline2d_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            polypoint2d_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            rectangle2d_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            triangle_set2d_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            arc2d_interfaces_;
        static const boost::array<openvrml::node_interface, 6>
            arc_close2d_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            circle2d_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            disk2d_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 2>
    x3d_geometry2d_component::polyline2d_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::mfvec2f_id,
                       "lineSegments")
    };

    const boost::array<openvrml::node_interface, 2>
    x3d_geometry2d_component::polypoint2d_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::mfvec2f_id,
                       "point")
    };

    const boost::array<openvrml::node_interface, 3>
    x3d_geometry2d_component::rectangle2d_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 3>
    x3d_geometry2d_component::triangle_set2d_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 4>
    x3d_geometry2d_component::arc2d_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "endAngle"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "radius"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "startAngle")
    };

    const boost::array<openvrml::node_interface, 6>
    x3d_geometry2d_component::arc_close2d_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sfstring_id,
                       "closureType"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "endAngle"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "radius"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "startAngle")
    };

    const boost::array<openvrml::node_interface, 2>
    x3d_geometry2d_component::circle2d_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "radius")
    };

    const boost::array<openvrml::node_interface, 4>
    x3d_geometry2d_component::disk2d_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "innerRadius"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "outerRadius"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid")
    };

    const char * const x3d_geometry2d_component::id = "Geometry2D";

    size_t x3d_geometry2d_component::support_level() const OPENVRML_NOTHROW
    {
        return 2;
    }

    void
    x3d_geometry2d_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // Polyline2D node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geometry2d_component::polyline2d_interfaces_.begin(),
                        x3d_geometry2d_component::polyline2d_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_geometry2d_component::polypoint2d_interfaces_.begin(),
                        x3d_geometry2d_component::polypoint2d_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_geometry2d_component::rectangle2d_interfaces_.begin(),
                        x3d_geometry2d_component::rectangle2d_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_geometry2d_component::triangle_set2d_interfaces_.begin(),
                        x3d_geometry2d_component::triangle_set2d_interfaces_.end());
                add_scope_entry(b,
                                "TriangleSet2D",
                                interface_set,
                                "urn:X-openvrml:node:TriangleSet2D",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // Arc2D node
            //
            {
                static const node_interface_set
                    interface_set(x3d_geometry2d_component::arc2d_interfaces_.begin(),
                                  x3d_geometry2d_component::arc2d_interfaces_.end());
                add_scope_entry(b,
                                "Arc2D",
                                interface_set,
                                "urn:X-openvrml:node:Arc2D",
                                scope);
            }

            //
            // ArcClose2D node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geometry2d_component::arc_close2d_interfaces_.begin(),
                        x3d_geometry2d_component::arc_close2d_interfaces_.end());
                add_scope_entry(b,
                                "ArcClose2D",
                                interface_set,
                                "urn:X-openvrml:node:ArcClose2D",
                                scope);
            }

            //
            // Circle2D node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geometry2d_component::circle2d_interfaces_.begin(),
                        x3d_geometry2d_component::circle2d_interfaces_.end());
                add_scope_entry(b,
                                "Circle2D",
                                interface_set,
                                "urn:X-openvrml:node:Circle2D",
                                scope);
            }

            //
            // Disk2D node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geometry2d_component::disk2d_interfaces_.begin(),
                        x3d_geometry2d_component::disk2d_interfaces_.end());
                add_scope_entry(b,
                                "Disk2D",
                                interface_set,
                                "urn:X-openvrml:node:Disk2D",
                                scope);
            }
        }
    }

    void
    x3d_geometry2d_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "Polyline2D",
            node_interface_set(
                x3d_geometry2d_component::polyline2d_interfaces_.begin(),
                x3d_geometry2d_component::polyline2d_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Polypoint2D",
            node_interface_set(
                x3d_geometry2d_component::polypoint2d_interfaces_.begin(),
                x3d_geometry2d_component::polypoint2d_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Rectangle2D",
            node_interface_set(
                x3d_geometry2d_component::rectangle2d_interfaces_.begin(),
                x3d_geometry2d_component::rectangle2d_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TriangleSet2D",
            node_interface_set(
                x3d_geometry2d_component::triangle_set2d_interfaces_.begin(),
                x3d_geometry2d_component::triangle_set2d_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "Arc2D",
                node_interface_set(
                    x3d_geometry2d_component::arc2d_interfaces_.begin(),
                    x3d_geometry2d_component::arc2d_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "ArcClose2D",
                node_interface_set(
                    x3d_geometry2d_component::arc_close2d_interfaces_.begin(),
                    x3d_geometry2d_component::arc_close2d_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "Circle2D",
                node_interface_set(
                    x3d_geometry2d_component::circle2d_interfaces_.begin(),
                    x3d_geometry2d_component::circle2d_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "Disk2D",
                node_interface_set(
                    x3d_geometry2d_component::disk2d_interfaces_.begin(),
                    x3d_geometry2d_component::disk2d_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_text_component : public component {
        static const boost::array<openvrml::node_interface, 10>
            font_style_interfaces_;
        static const boost::array<openvrml::node_interface, 6>
            text_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 10>
    x3d_text_component::font_style_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 6>
    x3d_text_component::text_interfaces_ = {
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

    const char * const x3d_text_component::id = "Text";

    size_t x3d_text_component::support_level() const OPENVRML_NOTHROW
    {
        return 1;
    }

    void x3d_text_component::do_add_to_scope(const openvrml::browser & b,
                                             openvrml::scope & scope,
                                             const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // FontStyle node
            //
            {
                static const node_interface_set
                    interface_set(x3d_text_component::font_style_interfaces_.begin(),
                                  x3d_text_component::font_style_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_text_component::text_interfaces_.begin(),
                                  x3d_text_component::text_interfaces_.end());
                add_scope_entry(b,
                                "Text",
                                interface_set,
                                "urn:X-openvrml:node:Text",
                                scope);
            }
        }
    }

    void
    x3d_text_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "FontStyle",
            node_interface_set(
                x3d_text_component::font_style_interfaces_.begin(),
                x3d_text_component::font_style_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Text",
            node_interface_set(
                x3d_text_component::text_interfaces_.begin(),
                x3d_text_component::text_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_sound_component : public component {
        static const boost::array<openvrml::node_interface, 13>
            audio_clip_interfaces_;
        static const boost::array<openvrml::node_interface, 11>
            sound_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 13>
    x3d_sound_component::audio_clip_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 11>
    x3d_sound_component::sound_interfaces_ = {
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

    const char * const x3d_sound_component::id = "Sound";

    size_t x3d_sound_component::support_level() const OPENVRML_NOTHROW
    {
        return 1;
    }

    void x3d_sound_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // AudioClip node
            //
            {
                static const node_interface_set
                    interface_set(x3d_sound_component::audio_clip_interfaces_.begin(),
                                  x3d_sound_component::audio_clip_interfaces_.end());
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
                static const node_interface_set
                    interface_set(x3d_sound_component::sound_interfaces_.begin(),
                                  x3d_sound_component::sound_interfaces_.end());
                add_scope_entry(b,
                                "Sound",
                                interface_set,
                                "urn:X-openvrml:node:Sound",
                                scope);
            }
        }
    }

    void
    x3d_sound_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "AudioClip",
            node_interface_set(
                x3d_sound_component::audio_clip_interfaces_.begin(),
                x3d_sound_component::audio_clip_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Sound",
            node_interface_set(
                x3d_sound_component::sound_interfaces_.begin(),
                x3d_sound_component::sound_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_lighting_component : public component {
        static const boost::array<openvrml::node_interface, 6>
            directional_light_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            point_light_interfaces_;
        static const boost::array<openvrml::node_interface, 11>
            spot_light_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 6>
    x3d_lighting_component::directional_light_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 8>
    x3d_lighting_component::point_light_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 11>
    x3d_lighting_component::spot_light_interfaces_ = {
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

    const char * const x3d_lighting_component::id = "Lighting";

    size_t x3d_lighting_component::support_level() const OPENVRML_NOTHROW
    {
        return 3;
    }

    void
    x3d_lighting_component::do_add_to_scope(const openvrml::browser & b,
                                            openvrml::scope & scope,
                                            const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // DirectionalLight node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_lighting_component::directional_light_interfaces_.begin(),
                        x3d_lighting_component::directional_light_interfaces_.end());
                add_scope_entry(b,
                                "DirectionalLight",
                                interface_set,
                                "urn:X-openvrml:node:DirectionalLight",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // PointLight node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_lighting_component::point_light_interfaces_.begin(),
                        x3d_lighting_component::point_light_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_lighting_component::spot_light_interfaces_.begin(),
                        x3d_lighting_component::spot_light_interfaces_.end());
                add_scope_entry(b,
                                "SpotLight",
                                interface_set,
                                "urn:X-openvrml:node:SpotLight",
                                scope);
            }
        }
    }

    void
    x3d_lighting_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "DirectionalLight",
            node_interface_set(
                x3d_lighting_component::directional_light_interfaces_.begin(),
                x3d_lighting_component::directional_light_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "PointLight",
                node_interface_set(
                    x3d_lighting_component::point_light_interfaces_.begin(),
                    x3d_lighting_component::point_light_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "SpotLight",
                node_interface_set(
                    x3d_lighting_component::spot_light_interfaces_.begin(),
                    x3d_lighting_component::spot_light_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_texturing_component : public component {
        static const boost::array<openvrml::node_interface, 4>
            image_texture_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            pixel_texture_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            texture_coordinate_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            texture_transform_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            multi_texture_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            multi_texture_coordinate_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            multi_texture_transform_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            texture_coordinate_generator_interfaces_;
        static const boost::array<openvrml::node_interface, 14>
            movie_texture_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 4>
    x3d_texturing_component::image_texture_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 4>
    x3d_texturing_component::pixel_texture_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 2>
    x3d_texturing_component::texture_coordinate_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec2f_id,
                       "point")
    };

    const boost::array<openvrml::node_interface, 5>
    x3d_texturing_component::texture_transform_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_texturing_component::multi_texture_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 2>
    x3d_texturing_component::multi_texture_coordinate_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "texCoord")
    };

    const boost::array<openvrml::node_interface, 2>
    x3d_texturing_component::multi_texture_transform_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "textureTransform")
    };

    const boost::array<openvrml::node_interface, 3>
    x3d_texturing_component::texture_coordinate_generator_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 14>
    x3d_texturing_component::movie_texture_interfaces_ = {
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

    const char * const x3d_texturing_component::id = "Texturing";

    size_t x3d_texturing_component::support_level() const OPENVRML_NOTHROW
    {
        return 3;
    }

    void
    x3d_texturing_component::do_add_to_scope(const openvrml::browser & b,
                                             openvrml::scope & scope,
                                             const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >=  1) {
            //
            // ImageTexture node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::image_texture_interfaces_.begin(),
                        x3d_texturing_component::image_texture_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::pixel_texture_interfaces_.begin(),
                        x3d_texturing_component::pixel_texture_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::texture_coordinate_interfaces_.begin(),
                        x3d_texturing_component::texture_coordinate_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::texture_transform_interfaces_.begin(),
                        x3d_texturing_component::texture_transform_interfaces_.end());
                add_scope_entry(b,
                                "TextureTransform",
                                interface_set,
                                "urn:X-openvrml:node:TextureTransform",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // MultiTexture node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::multi_texture_interfaces_.begin(),
                        x3d_texturing_component::multi_texture_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::multi_texture_coordinate_interfaces_.begin(),
                        x3d_texturing_component::multi_texture_coordinate_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::multi_texture_transform_interfaces_.begin(),
                        x3d_texturing_component::multi_texture_transform_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::texture_coordinate_generator_interfaces_.begin(),
                        x3d_texturing_component::texture_coordinate_generator_interfaces_.end());
                add_scope_entry(b,
                                "TextureCoordinateGenerator",
                                interface_set,
                                "urn:X-openvrml:node:TextureCoordinateGenerator",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // MovieTexture node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_texturing_component::movie_texture_interfaces_.begin(),
                        x3d_texturing_component::movie_texture_interfaces_.end());
                add_scope_entry(b,
                                "MovieTexture",
                                interface_set,
                                "urn:X-openvrml:node:MovieTexture",
                                scope);
            }
        }
    }

    void
    x3d_texturing_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "ImageTexture",
            node_interface_set(
                x3d_texturing_component::image_texture_interfaces_.begin(),
                x3d_texturing_component::image_texture_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PixelTexture",
            node_interface_set(
                x3d_texturing_component::pixel_texture_interfaces_.begin(),
                x3d_texturing_component::pixel_texture_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TextureCoordinate",
            node_interface_set(
                x3d_texturing_component::texture_coordinate_interfaces_.begin(),
                x3d_texturing_component::texture_coordinate_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TextureTransform",
            node_interface_set(
                x3d_texturing_component::texture_transform_interfaces_.begin(),
                x3d_texturing_component::texture_transform_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "MultiTexture",
                node_interface_set(
                    x3d_texturing_component::multi_texture_interfaces_.begin(),
                    x3d_texturing_component::multi_texture_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "MultiTextureCoordinate",
                node_interface_set(
                    x3d_texturing_component::multi_texture_coordinate_interfaces_.begin(),
                    x3d_texturing_component::multi_texture_coordinate_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "MultiTextureTransform",
                node_interface_set(
                    x3d_texturing_component::multi_texture_transform_interfaces_.begin(),
                    x3d_texturing_component::multi_texture_transform_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "TextureCoordinateGenerator",
                node_interface_set(
                    x3d_texturing_component::texture_coordinate_generator_interfaces_.begin(),
                    x3d_texturing_component::texture_coordinate_generator_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "MovieTexture",
                node_interface_set(
                    x3d_texturing_component::movie_texture_interfaces_.begin(),
                    x3d_texturing_component::movie_texture_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_interpolation_component : public component {
        static const boost::array<openvrml::node_interface, 5>
            coordinate_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            orientation_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            position_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            scalar_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            color_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            normal_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            coordinate_interpolator2d_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            position_interpolator2d_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 5>
    x3d_interpolation_component::coordinate_interpolator_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_interpolation_component::orientation_interpolator_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_interpolation_component::position_interpolator_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_interpolation_component::scalar_interpolator_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_interpolation_component::color_interpolator_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_interpolation_component::normal_interpolator_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 5>
    x3d_interpolation_component::coordinate_interpolator2d_interfaces_ = {
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
                       field_value::mfvec2f_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::mfvec2f_id,
                       "value_changed")
    };

    const boost::array<openvrml::node_interface, 5>
    x3d_interpolation_component::position_interpolator2d_interfaces_ = {
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
                       field_value::mfvec2f_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec2f_id,
                       "value_changed")
    };

    const char * const x3d_interpolation_component::id = "Interpolation";

    size_t x3d_interpolation_component::support_level() const OPENVRML_NOTHROW
    {
        return 3;
    }

    void
    x3d_interpolation_component::
    do_add_to_scope(const openvrml::browser & b,
                    openvrml::scope & scope,
                    const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // CoordinateInterpolator node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_interpolation_component::coordinate_interpolator_interfaces_.begin(),
                        x3d_interpolation_component::coordinate_interpolator_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_interpolation_component::orientation_interpolator_interfaces_.begin(),
                        x3d_interpolation_component::orientation_interpolator_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_interpolation_component::position_interpolator_interfaces_.begin(),
                        x3d_interpolation_component::position_interpolator_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_interpolation_component::scalar_interpolator_interfaces_.begin(),
                        x3d_interpolation_component::scalar_interpolator_interfaces_.end());
                add_scope_entry(b,
                                "ScalarInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:ScalarInterpolator",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // ColorInterpolator node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_interpolation_component::color_interpolator_interfaces_.begin(),
                        x3d_interpolation_component::color_interpolator_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_interpolation_component::normal_interpolator_interfaces_.begin(),
                        x3d_interpolation_component::normal_interpolator_interfaces_.end());
                add_scope_entry(b,
                                "NormalInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:NormalInterpolator",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // CoordinateInterpolator2D
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_interpolation_component::coordinate_interpolator2d_interfaces_.begin(),
                        x3d_interpolation_component::coordinate_interpolator2d_interfaces_.end());
                add_scope_entry(b,
                                "CoordinateInterpolator2D",
                                interface_set,
                                "urn:X-openvrml:node:CoordinateInterpolator2D",
                                scope);
            }

            //
            // PositionInterpolator2D node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_interpolation_component::position_interpolator2d_interfaces_.begin(),
                        x3d_interpolation_component::position_interpolator2d_interfaces_.end());
                add_scope_entry(b,
                                "PositionInterpolator2D",
                                interface_set,
                                "urn:X-openvrml:node:PositionInterpolator2D",
                                scope);
            }
        }
    }

    void
    x3d_interpolation_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "CoordinateInterpolator",
            node_interface_set(
                x3d_interpolation_component::coordinate_interpolator_interfaces_.begin(),
                x3d_interpolation_component::coordinate_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "OrientationInterpolator",
            node_interface_set(
                x3d_interpolation_component::orientation_interpolator_interfaces_.begin(),
                x3d_interpolation_component::orientation_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PositionInterpolator",
            node_interface_set(
                x3d_interpolation_component::position_interpolator_interfaces_.begin(),
                x3d_interpolation_component::position_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "ScalarInterpolator",
            node_interface_set(
                x3d_interpolation_component::scalar_interpolator_interfaces_.begin(),
                x3d_interpolation_component::scalar_interpolator_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "ColorInterpolator",
                node_interface_set(
                    x3d_interpolation_component::color_interpolator_interfaces_.begin(),
                    x3d_interpolation_component::color_interpolator_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "NormalInterpolator",
                node_interface_set(
                    x3d_interpolation_component::normal_interpolator_interfaces_.begin(),
                    x3d_interpolation_component::normal_interpolator_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "CoordinateInterpolator2D",
                node_interface_set(
                    x3d_interpolation_component::coordinate_interpolator2d_interfaces_.begin(),
                    x3d_interpolation_component::coordinate_interpolator2d_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "PositionInterpolator2D",
                node_interface_set(
                    x3d_interpolation_component::position_interpolator2d_interfaces_.begin(),
                    x3d_interpolation_component::position_interpolator2d_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_pointing_device_sensor_component :
        public component {
        static const boost::array<openvrml::node_interface, 12>
            cylinder_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 11>
            plane_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            sphere_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            touch_sensor_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 12>
    x3d_pointing_device_sensor_component::cylinder_sensor_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 11>
    x3d_pointing_device_sensor_component::plane_sensor_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 9>
    x3d_pointing_device_sensor_component::sphere_sensor_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 9>
    x3d_pointing_device_sensor_component::touch_sensor_interfaces_ = {
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

    const char * const x3d_pointing_device_sensor_component::id =
        "PointingDeviceSensor";

    size_t x3d_pointing_device_sensor_component::support_level() const
        OPENVRML_NOTHROW
    {
        return 1;
    }

    void
    x3d_pointing_device_sensor_component::
    do_add_to_scope(const openvrml::browser & b,
                    openvrml::scope & scope,
                    const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // CylinderSensor node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_pointing_device_sensor_component::cylinder_sensor_interfaces_.begin(),
                        x3d_pointing_device_sensor_component::cylinder_sensor_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_pointing_device_sensor_component::plane_sensor_interfaces_.begin(),
                        x3d_pointing_device_sensor_component::plane_sensor_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_pointing_device_sensor_component::sphere_sensor_interfaces_.begin(),
                        x3d_pointing_device_sensor_component::sphere_sensor_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_pointing_device_sensor_component::touch_sensor_interfaces_.begin(),
                        x3d_pointing_device_sensor_component::touch_sensor_interfaces_.end());
                add_scope_entry(b,
                                "TouchSensor",
                                interface_set,
                                "urn:X-openvrml:node:TouchSensor",
                                scope);
            }
        }
    }

    void
    x3d_pointing_device_sensor_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "CylinderSensor",
            node_interface_set(
                x3d_pointing_device_sensor_component::cylinder_sensor_interfaces_.begin(),
                x3d_pointing_device_sensor_component::cylinder_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "PlaneSensor",
            node_interface_set(
                x3d_pointing_device_sensor_component::plane_sensor_interfaces_.begin(),
                x3d_pointing_device_sensor_component::plane_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "SphereSensor",
            node_interface_set(
                x3d_pointing_device_sensor_component::sphere_sensor_interfaces_.begin(),
                x3d_pointing_device_sensor_component::sphere_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TouchSensor",
            node_interface_set(
                x3d_pointing_device_sensor_component::touch_sensor_interfaces_.begin(),
                x3d_pointing_device_sensor_component::touch_sensor_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_key_device_sensor_component : public component {
        static const boost::array<openvrml::node_interface, 10>
            key_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 6>
            string_sensor_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 10>
    x3d_key_device_sensor_component::key_sensor_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 6>
    x3d_key_device_sensor_component::string_sensor_interfaces_ = {
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

    const char * const x3d_key_device_sensor_component::id = "KeyDeviceSensor";

    size_t x3d_key_device_sensor_component::support_level() const
        OPENVRML_NOTHROW
    {
        return 2;
    }

    void
    x3d_key_device_sensor_component::
    do_add_to_scope(const openvrml::browser & b,
                    openvrml::scope & scope,
                    const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // KeySensor node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_key_device_sensor_component::key_sensor_interfaces_.begin(),
                        x3d_key_device_sensor_component::key_sensor_interfaces_.end());
                add_scope_entry(b,
                                "KeySensor",
                                interface_set,
                                "urn:X-openvrml:node:KeySensor",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // StringSensor node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_key_device_sensor_component::string_sensor_interfaces_.begin(),
                        x3d_key_device_sensor_component::string_sensor_interfaces_.end());
                add_scope_entry(b,
                                "StringSensor",
                                interface_set,
                                "urn:X-openvrml:node:StringSensor",
                                scope);
            }
        }
    }

    void
    x3d_key_device_sensor_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "KeySensor",
            node_interface_set(
                x3d_key_device_sensor_component::key_sensor_interfaces_.begin(),
                x3d_key_device_sensor_component::key_sensor_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "StringSensor",
                node_interface_set(
                    x3d_key_device_sensor_component::string_sensor_interfaces_.begin(),
                    x3d_key_device_sensor_component::string_sensor_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_environmental_sensor_component :
        public component {
        static const boost::array<openvrml::node_interface, 10>
            proximity_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            visibility_sensor_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 10>
    x3d_environmental_sensor_component::proximity_sensor_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_environmental_sensor_component::visibility_sensor_interfaces_ = {
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

    const char * const x3d_environmental_sensor_component::id =
        "EnvironmentalSensor";

    size_t x3d_environmental_sensor_component::support_level() const
        OPENVRML_NOTHROW
    {
        return 2;
    }

    void
    x3d_environmental_sensor_component::
    do_add_to_scope(const openvrml::browser & b,
                    openvrml::scope & scope,
                    const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // ProximitySensor node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_environmental_sensor_component::proximity_sensor_interfaces_.begin(),
                        x3d_environmental_sensor_component::proximity_sensor_interfaces_.end());
                add_scope_entry(b,
                                "ProximitySensor",
                                interface_set,
                                "urn:X-openvrml:node:ProximitySensor",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // VisibilitySensor node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_environmental_sensor_component::visibility_sensor_interfaces_.begin(),
                        x3d_environmental_sensor_component::visibility_sensor_interfaces_.end());
                add_scope_entry(b,
                                "VisibilitySensor",
                                interface_set,
                                "urn:X-openvrml:node:VisibilitySensor",
                                scope);
            }
        }
    }

    void
    x3d_environmental_sensor_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "ProximitySensor",
            node_interface_set(
                x3d_environmental_sensor_component::proximity_sensor_interfaces_.begin(),
                x3d_environmental_sensor_component::proximity_sensor_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "VisibilitySensor",
                node_interface_set(
                    x3d_environmental_sensor_component::visibility_sensor_interfaces_.begin(),
                    x3d_environmental_sensor_component::visibility_sensor_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_navigation_component : public component {
        static const boost::array<openvrml::node_interface, 10>
            navigation_info_interfaces_;
        static const boost::array<openvrml::node_interface, 10>
            viewpoint_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            billboard_interfaces_;
        static const boost::array<openvrml::node_interface, 10>
            collision_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            lod_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 10>
    x3d_navigation_component::navigation_info_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 10>
    x3d_navigation_component::viewpoint_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_navigation_component::billboard_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 10>
    x3d_navigation_component::collision_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 8>
    x3d_navigation_component::lod_interfaces_ = {
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

    const char * const x3d_navigation_component::id = "Navigation";

    size_t x3d_navigation_component::support_level() const OPENVRML_NOTHROW
    {
        return 2;
    }

    void
    x3d_navigation_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // NavigationInfo node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_navigation_component::navigation_info_interfaces_.begin(),
                        x3d_navigation_component::navigation_info_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_navigation_component::viewpoint_interfaces_.begin(),
                        x3d_navigation_component::viewpoint_interfaces_.end());
                add_scope_entry(b,
                                "Viewpoint",
                                interface_set,
                                "urn:X-openvrml:node:Viewpoint",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // Billboard node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_navigation_component::billboard_interfaces_.begin(),
                        x3d_navigation_component::billboard_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_navigation_component::collision_interfaces_.begin(),
                        x3d_navigation_component::collision_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_navigation_component::lod_interfaces_.begin(),
                        x3d_navigation_component::lod_interfaces_.end());
                add_scope_entry(b,
                                "LOD",
                                interface_set,
                                "urn:X-openvrml:node:LOD",
                                scope);
            }
        }
    }

    void
    x3d_navigation_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "NavigationInfo",
            node_interface_set(
                x3d_navigation_component::navigation_info_interfaces_.begin(),
                x3d_navigation_component::navigation_info_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "Viewpoint",
            node_interface_set(
                x3d_navigation_component::viewpoint_interfaces_.begin(),
                x3d_navigation_component::viewpoint_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "Billboard",
                node_interface_set(
                    x3d_navigation_component::billboard_interfaces_.begin(),
                    x3d_navigation_component::billboard_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "Collision",
                node_interface_set(
                    x3d_navigation_component::collision_interfaces_.begin(),
                    x3d_navigation_component::collision_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "LOD",
                node_interface_set(
                    x3d_navigation_component::lod_interfaces_.begin(),
                    x3d_navigation_component::lod_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_environmental_effects_component :
        public component {
        static const boost::array<openvrml::node_interface, 14>
            background_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            fog_interfaces_;
        static const boost::array<openvrml::node_interface, 15>
            texture_background_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 14>
    x3d_environmental_effects_component::background_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_environmental_effects_component::fog_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 15>
    x3d_environmental_effects_component::texture_background_interfaces_ = {
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
                       field_value::sfnode_id,
                       "backTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "bottomTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "frontTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "leftTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "rightTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "topTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "skyAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "skyColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "transparency"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "bindTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isBound")
    };

    const char * const x3d_environmental_effects_component::id =
        "EnvironmentalEffects";

    size_t x3d_environmental_effects_component::support_level() const
        OPENVRML_NOTHROW
    {
        return 3;
    }

    void
    x3d_environmental_effects_component::
    do_add_to_scope(const openvrml::browser & b,
                    openvrml::scope & scope,
                    const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // Background node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_environmental_effects_component::background_interfaces_.begin(),
                        x3d_environmental_effects_component::background_interfaces_.end());
                add_scope_entry(b,
                                "Background",
                                interface_set,
                                "urn:X-openvrml:node:Background",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // Fog node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_environmental_effects_component::fog_interfaces_.begin(),
                        x3d_environmental_effects_component::fog_interfaces_.end());
                add_scope_entry(b,
                                "Fog",
                                interface_set,
                                "urn:X-openvrml:node:Fog",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // TextureBackground node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_environmental_effects_component::texture_background_interfaces_.begin(),
                        x3d_environmental_effects_component::texture_background_interfaces_.end());
                add_scope_entry(b,
                                "TextureBackground",
                                interface_set,
                                "urn:X-openvrml:node:TextureBackground",
                                scope);
            }
        }
    }

    void
    x3d_environmental_effects_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "Background",
            node_interface_set(
                x3d_environmental_effects_component::background_interfaces_.begin(),
                x3d_environmental_effects_component::background_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "Fog",
                node_interface_set(
                    x3d_environmental_effects_component::fog_interfaces_.begin(),
                    x3d_environmental_effects_component::fog_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "TextureBackground",
                node_interface_set(
                    x3d_environmental_effects_component::texture_background_interfaces_.begin(),
                    x3d_environmental_effects_component::texture_background_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_cad_geometry_component : public component {
        static const boost::array<openvrml::node_interface, 11>
            indexed_quad_set_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            quad_set_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            cad_assembly_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            cad_face_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            cad_layer_interfaces_;
        static const boost::array<openvrml::node_interface, 12>
            cad_part_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 11>
    x3d_cad_geometry_component::indexed_quad_set_interfaces_ = {
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
                       "metadata"),
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

    const boost::array<openvrml::node_interface, 9>
    x3d_cad_geometry_component::quad_set_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
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

    const boost::array<openvrml::node_interface, 7>
    x3d_cad_geometry_component::cad_assembly_interfaces_ = {
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
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 3>
    x3d_cad_geometry_component::cad_face_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "shape")
    };

    const boost::array<openvrml::node_interface, 8>
    x3d_cad_geometry_component::cad_layer_interfaces_ = {
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
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfbool_id,
                       "visible"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 12>
    x3d_cad_geometry_component::cad_part_interfaces_ = {
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
                       field_value::sfvec3f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
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

    const char * const x3d_cad_geometry_component::id = "CADGeometry";

    size_t x3d_cad_geometry_component::support_level() const OPENVRML_NOTHROW
    {
        return 2;
    }

    void x3d_cad_geometry_component::do_add_to_scope(const openvrml::browser & b,
                                                     openvrml::scope & scope,
                                                     const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            // IndexedQuadSet node
            {
                static const node_interface_set
                    interface_set(
                        x3d_cad_geometry_component::indexed_quad_set_interfaces_.begin(),
                        x3d_cad_geometry_component::indexed_quad_set_interfaces_.end());
                add_scope_entry(b,
                                "IndexedQuadSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedQuadSet",
                                scope);
            }

            // QuadSet node
            {
                static const node_interface_set
                    interface_set(
                        x3d_cad_geometry_component::quad_set_interfaces_.begin(),
                        x3d_cad_geometry_component::quad_set_interfaces_.end());
                add_scope_entry(b,
                                "QuadSet",
                                interface_set,
                                "urn:X-openvrml:node:IndexedQuadSet",
                                scope);
            }
        }

        if (level >= 2) {
            // CADAssembly node
            {
                static const node_interface_set
                    interface_set(
                        x3d_cad_geometry_component::cad_assembly_interfaces_.begin(),
                        x3d_cad_geometry_component::cad_assembly_interfaces_.end());
                add_scope_entry(b,
                                "CADAssembly",
                                interface_set,
                                "urn:X-openvrml:node:CADAssembly",
                                scope);
            }
            // CADFace node
            {
                static const node_interface_set
                    interface_set(
                        x3d_cad_geometry_component::cad_face_interfaces_.begin(),
                        x3d_cad_geometry_component::cad_face_interfaces_.end());
                add_scope_entry(b,
                                "CADFace",
                                interface_set,
                                "urn:X-openvrml:node:CADFace",
                                scope);
            }
            // CADLayer node
            {
                static const node_interface_set
                    interface_set(
                        x3d_cad_geometry_component::cad_layer_interfaces_.begin(),
                        x3d_cad_geometry_component::cad_layer_interfaces_.end());
                add_scope_entry(b,
                                "CADLayer",
                                interface_set,
                                "urn:X-openvrml:node:CADLayer",
                                scope);
            }
            // CADPart node
            {
                static const node_interface_set
                    interface_set(
                        x3d_cad_geometry_component::cad_part_interfaces_.begin(),
                        x3d_cad_geometry_component::cad_part_interfaces_.end());
                add_scope_entry(b, "CADPart", interface_set,
                                "urn:X-openvrml:node:CADPart", scope);
            }
        }
    }

    void
    x3d_cad_geometry_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "IndexedQuadSet",
            node_interface_set(
                x3d_cad_geometry_component::indexed_quad_set_interfaces_.begin(),
                x3d_cad_geometry_component::indexed_quad_set_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "QuadSet",
            node_interface_set(
                x3d_cad_geometry_component::quad_set_interfaces_.begin(),
                x3d_cad_geometry_component::quad_set_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "CADAssembly",
                node_interface_set(
                    x3d_cad_geometry_component::cad_assembly_interfaces_.begin(),
                    x3d_cad_geometry_component::cad_assembly_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "CADFace",
                node_interface_set(
                    x3d_cad_geometry_component::cad_face_interfaces_.begin(),
                    x3d_cad_geometry_component::cad_face_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "CADLayer",
                node_interface_set(
                    x3d_cad_geometry_component::cad_layer_interfaces_.begin(),
                    x3d_cad_geometry_component::cad_layer_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "CADPart",
                node_interface_set(
                    x3d_cad_geometry_component::cad_part_interfaces_.begin(),
                    x3d_cad_geometry_component::cad_part_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_geospatial_component : public component {
        static const boost::array<openvrml::node_interface, 4>
            geo_coordinate_interfaces_;
        static const boost::array<openvrml::node_interface, 19>
            geo_elevation_grid_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            geo_location_interfaces_;
        static const boost::array<openvrml::node_interface, 16>
            geo_lod_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            geo_metadata_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            geo_origin_interfaces_;
        static const boost::array<openvrml::node_interface, 8>
            geo_position_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 11>
            geo_touch_sensor_interfaces_;
        static const boost::array<openvrml::node_interface, 16>
            geo_viewpoint_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 4>
    x3d_geospatial_component::geo_coordinate_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3d_id,
                       "point"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem")
    };

    const boost::array<openvrml::node_interface, 19>
    x3d_geospatial_component::geo_elevation_grid_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::mfdouble_id,
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
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "yScale"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "ccw"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "colorPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sfdouble_id,
                       "creaseAngle"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3d_id,
                       "geoGridOrigin"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
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
                       field_value::sfdouble_id,
                       "xSpacing"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "zDimension"),
        node_interface(node_interface::field_id,
                       field_value::sfdouble_id,
                       "zSpacing")
    };

    const boost::array<openvrml::node_interface, 9>
    x3d_geospatial_component::geo_location_interfaces_ = {
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
                       field_value::sfvec3d_id,
                       "geoCoords"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 16>
    x3d_geospatial_component::geo_lod_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::eventout_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3d_id,
                       "center"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "child1Url"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "child2Url"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "child3Url"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "child4Url"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "range"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "rootUrl"),
        node_interface(node_interface::field_id,
                       field_value::mfnode_id,
                       "rootNode"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 4>
    x3d_geospatial_component::geo_metadata_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "data"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "summary"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url")
    };

    const boost::array<openvrml::node_interface, 4>
    x3d_geospatial_component::geo_origin_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3d_id,
                       "geoCoords"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "geoSystem"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "rotateYUp")
    };

    const boost::array<openvrml::node_interface, 8>
    x3d_geospatial_component::geo_position_interpolator_interfaces_ = {
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
                       field_value::mfvec3d_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3d_id,
                       "geovalue_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "value_changed"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem")
    };

    const boost::array<openvrml::node_interface, 11>
    x3d_geospatial_component::geo_touch_sensor_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
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
                       field_value::sfvec3d_id,
                       "hitGeoCoord_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isOver"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "touchTime"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem")
    };

    const boost::array<openvrml::node_interface, 16>
    x3d_geospatial_component::geo_viewpoint_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sfbool_id,
                       "set_bind"),
        node_interface(node_interface::eventin_id,
                       field_value::sfrotation_id,
                       "set_orientation"),
        node_interface(node_interface::eventin_id,
                       field_value::sfvec3d_id,
                       "set_position"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "description"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "fieldOfView"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "headlight"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "jump"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "navType"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "bindTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isBound"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "geoOrigin"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "geoSystem"),
        node_interface(node_interface::field_id,
                       field_value::sfrotation_id,
                       "orientation"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3d_id,
                       "position"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "speedFactor")
    };

    const char * const x3d_geospatial_component::id = "Geospatial";

    size_t x3d_geospatial_component::support_level() const OPENVRML_NOTHROW
    {
        return 1;
    }

    void
    x3d_geospatial_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // GeoCoordinate node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_coordinate_interfaces_.begin(),
                        x3d_geospatial_component::geo_coordinate_interfaces_.end());
                add_scope_entry(b,
                                "GeoCoordinate",
                                interface_set,
                                "urn:X-openvrml:node:GeoCoordinate",
                                scope);
            }

            //
            // GeoElevationGrid node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_elevation_grid_interfaces_.begin(),
                        x3d_geospatial_component::geo_elevation_grid_interfaces_.end());
                add_scope_entry(b,
                                "GeoElevationGrid",
                                interface_set,
                                "urn:X-openvrml:node:GeoElevationGrid",
                                scope);
            }

            //
            // GeoLocation node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_location_interfaces_.begin(),
                        x3d_geospatial_component::geo_location_interfaces_.end());
                add_scope_entry(b,
                                "GeoLocation",
                                interface_set,
                                "urn:X-openvrml:node:GeoLocation",
                                scope);
            }

            //
            // GeoLOD node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_lod_interfaces_.begin(),
                        x3d_geospatial_component::geo_lod_interfaces_.end());
                add_scope_entry(b,
                                "GeoLOD",
                                interface_set,
                                "urn:X-openvrml:node:GeoLOD",
                                scope);
            }

            //
            // GeoMetadata node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_metadata_interfaces_.begin(),
                        x3d_geospatial_component::geo_metadata_interfaces_.end());
                add_scope_entry(b,
                                "GeoMetadata",
                                interface_set,
                                "urn:X-openvrml:node:GeoMetadata",
                                scope);
            }

            //
            // GeoOrigin node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_origin_interfaces_.begin(),
                        x3d_geospatial_component::geo_origin_interfaces_.end());
                add_scope_entry(b,
                                "GeoOrigin",
                                interface_set,
                                "urn:X-openvrml:node:GeoOrigin",
                                scope);
            }

            //
            // GeoPositionInterpolator node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_position_interpolator_interfaces_.begin(),
                        x3d_geospatial_component::geo_position_interpolator_interfaces_.end());
                add_scope_entry(b,
                                "GeoPositionInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:GeoPositionInterpolator",
                                scope);
            }

            //
            // GeoTouchSensor node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_touch_sensor_interfaces_.begin(),
                        x3d_geospatial_component::geo_touch_sensor_interfaces_.end());
                add_scope_entry(b,
                                "GeoTouchSensor",
                                interface_set,
                                "urn:X-openvrml:node:GeoTouchSensor",
                                scope);
            }

            //
            // GeoViewpoint node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_geospatial_component::geo_viewpoint_interfaces_.begin(),
                        x3d_geospatial_component::geo_viewpoint_interfaces_.end());
                add_scope_entry(b,
                                "GeoViewpoint",
                                interface_set,
                                "urn:X-openvrml:node:GeoViewpoint",
                                scope);
            }
        }
    }

    void
    x3d_geospatial_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "GeoCoordinate",
            node_interface_set(
                x3d_geospatial_component::geo_coordinate_interfaces_.begin(),
                x3d_geospatial_component::geo_coordinate_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "GeoElevationGrid",
            node_interface_set(
                x3d_geospatial_component::geo_elevation_grid_interfaces_.begin(),
                x3d_geospatial_component::geo_elevation_grid_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "GeoLocation",
            node_interface_set(
                x3d_geospatial_component::geo_location_interfaces_.begin(),
                x3d_geospatial_component::geo_location_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "GeoLOD",
            node_interface_set(
                x3d_geospatial_component::geo_lod_interfaces_.begin(),
                x3d_geospatial_component::geo_lod_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "GeoMetadata",
            node_interface_set(
                x3d_geospatial_component::geo_metadata_interfaces_.begin(),
                x3d_geospatial_component::geo_metadata_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "GeoOrigin",
            node_interface_set(
                x3d_geospatial_component::geo_origin_interfaces_.begin(),
                x3d_geospatial_component::geo_origin_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "GeoPositionInterpolator",
            node_interface_set(
                x3d_geospatial_component::geo_position_interpolator_interfaces_.begin(),
                x3d_geospatial_component::geo_position_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "GeoTouchSensor",
            node_interface_set(
                x3d_geospatial_component::geo_touch_sensor_interfaces_.begin(),
                x3d_geospatial_component::geo_touch_sensor_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "GeoViewpoint",
            node_interface_set(
                x3d_geospatial_component::geo_viewpoint_interfaces_.begin(),
                x3d_geospatial_component::geo_viewpoint_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_hanim_component : public component {
        static const boost::array<openvrml::node_interface, 5>
            hanim_displacer_interfaces_;
        static const boost::array<openvrml::node_interface, 19>
            hanim_humanoid_interfaces_;
        static const boost::array<openvrml::node_interface, 19>
            hanim_joint_interfaces_;
        static const boost::array<openvrml::node_interface, 12>
            hanim_segment_interfaces_;
        static const boost::array<openvrml::node_interface, 12>
            hanim_site_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 5>
    x3d_hanim_component::hanim_displacer_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "coordIndex"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "displacements"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "weight")
    };

    const boost::array<openvrml::node_interface, 19>
    x3d_hanim_component::hanim_humanoid_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "info"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "joints"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
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
                       field_value::mfnode_id,
                       "segments"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "sites"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "skeleton"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "skin"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "skinCoord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "skinNormal"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "translation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "version"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "viewpoints"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 19>
    x3d_hanim_component::hanim_joint_interfaces_ = {
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
                       field_value::mfnode_id,
                       "displacers"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfrotation_id,
                       "limitOrientation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "llimit"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
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
                       field_value::mfint32_id,
                       "skinCoordIndex"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "skinCoordWeight"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "stiffness"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "translation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "ulimit"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 12>
    x3d_hanim_component::hanim_segment_interfaces_ = {
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
                       "centerOfMass"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "displacers"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "mass"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "momentsOfInertia"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 12>
    x3d_hanim_component::hanim_site_interfaces_ = {
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
                       field_value::sfstring_id,
                       "name"),
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

    const char * const x3d_hanim_component::id = "H-Anim";

    size_t x3d_hanim_component::support_level() const OPENVRML_NOTHROW
    {
        return 1;
    }

    void x3d_hanim_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // HAnimDisplacer node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_hanim_component::hanim_displacer_interfaces_.begin(),
                        x3d_hanim_component::hanim_displacer_interfaces_.end());
                add_scope_entry(b,
                                "HAnimDisplacer",
                                interface_set,
                                "urn:X-openvrml:node:HAnimDisplacer",
                                scope);
            }

            //
            // HAnimHumanoid node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_hanim_component::hanim_humanoid_interfaces_.begin(),
                        x3d_hanim_component::hanim_humanoid_interfaces_.end());
                add_scope_entry(b,
                                "HAnimHumanoid",
                                interface_set,
                                "urn:X-openvrml:node:HAnimHumanoid",
                                scope);
            }

            //
            // HAnimJoint node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_hanim_component::hanim_joint_interfaces_.begin(),
                        x3d_hanim_component::hanim_joint_interfaces_.end());
                add_scope_entry(b,
                                "HAnimJoint",
                                interface_set,
                                "urn:X-openvrml:node:HAnimJoint",
                                scope);
            }

            //
            // HAnimSegment node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_hanim_component::hanim_segment_interfaces_.begin(),
                        x3d_hanim_component::hanim_segment_interfaces_.end());
                add_scope_entry(b,
                                "HAnimSegment",
                                interface_set,
                                "urn:X-openvrml:node:HAnimSegment",
                                scope);
            }

            //
            // HAnimSite node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_hanim_component::hanim_site_interfaces_.begin(),
                        x3d_hanim_component::hanim_site_interfaces_.end());
                add_scope_entry(b,
                                "HAnimSite",
                                interface_set,
                                "urn:X-openvrml:node:HAnimSite",
                                scope);
            }
        }
    }

    void
    x3d_hanim_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "HAnimDisplacer",
            node_interface_set(
                x3d_hanim_component::hanim_displacer_interfaces_.begin(),
                x3d_hanim_component::hanim_displacer_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "HAnimHumanoid",
            node_interface_set(
                x3d_hanim_component::hanim_humanoid_interfaces_.begin(),
                x3d_hanim_component::hanim_humanoid_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "HAnimJoint",
            node_interface_set(
                x3d_hanim_component::hanim_joint_interfaces_.begin(),
                x3d_hanim_component::hanim_joint_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "HAnimSegment",
            node_interface_set(
                x3d_hanim_component::hanim_segment_interfaces_.begin(),
                x3d_hanim_component::hanim_segment_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "HAnimSite",
            node_interface_set(
                x3d_hanim_component::hanim_site_interfaces_.begin(),
                x3d_hanim_component::hanim_site_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_nurbs_component : public component {
        static const boost::array<openvrml::node_interface, 2>
            coordinate_double_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            nurbs_curve_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            nurbs_orientation_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 15>
            nurbs_patch_surface_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            nurbs_position_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 12>
            nurbs_surface_interpolator_interfaces_;
        static const boost::array<openvrml::node_interface, 9>
            nurbs_texture_coordinate_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            nurbs_set_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            nurbs_curve2d_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            contour_polyline2d_3_0_interfaces_;
        static const boost::array<openvrml::node_interface, 2>
            contour_polyline2d_3_1_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            nurbs_swept_surface_interfaces_;
        static const boost::array<openvrml::node_interface, 5>
            nurbs_swung_surface_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            contour2d_interfaces_;
        static const boost::array<openvrml::node_interface, 18>
            nurbs_trimmed_surface_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 2>
    x3d_nurbs_component::coordinate_double_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3d_id,
                       "point")
    };

    const boost::array<openvrml::node_interface, 7>
    x3d_nurbs_component::nurbs_curve_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "controlPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "tessellation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "closed"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "knot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "order")
    };

    const boost::array<openvrml::node_interface, 7>
    x3d_nurbs_component::nurbs_orientation_interpolator_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "controlPoints"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "knot"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "order"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::eventout_id,
                       field_value::sfrotation_id,
                       "value_changed")
    };

    const boost::array<openvrml::node_interface, 15>
    x3d_nurbs_component::nurbs_patch_surface_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "controlPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "texCoord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "uTessellation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "vTessellation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "uClosed"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "uKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uOrder"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "vClosed"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "vKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vOrder")
    };

    const boost::array<openvrml::node_interface, 7>
    x3d_nurbs_component::nurbs_position_interpolator_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "controlPoints"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "knot"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "order"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "value_changed")
    };

    const boost::array<openvrml::node_interface, 12>
    x3d_nurbs_component::nurbs_surface_interpolator_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sfvec2f_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "controlPoints"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "position_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "normal_changed"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "uKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uOrder"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "vKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vOrder")
    };

    const boost::array<openvrml::node_interface, 9>
    x3d_nurbs_component::nurbs_texture_coordinate_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec2f_id,
                       "controlPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "weight"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "uKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uOrder"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "vKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vOrder")
    };

    const boost::array<openvrml::node_interface, 7>
    x3d_nurbs_component::nurbs_set_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addGeometry"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeGeometry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "geometry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "tessellationScale"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 7>
    x3d_nurbs_component::nurbs_curve2d_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec2d_id,
                       "controlPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "tessellation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "closed"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "knot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "order")
    };

    const boost::array<openvrml::node_interface, 2>
    x3d_nurbs_component::contour_polyline2d_3_0_interfaces_ = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec2f_id,
                                   "point"),
    };

    const boost::array<openvrml::node_interface, 2>
    x3d_nurbs_component::contour_polyline2d_3_1_interfaces_ = {
                    node_interface(node_interface::exposedfield_id,
                                   field_value::sfnode_id,
                                   "metadata"),
                    node_interface(node_interface::exposedfield_id,
                                   field_value::mfvec2f_id,
                                   "controlPoint")
    };

    const boost::array<openvrml::node_interface, 5>
    x3d_nurbs_component::nurbs_swept_surface_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "crossSectionCurve"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "trajectoryCurve"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "ccw"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid")
    };

    const boost::array<openvrml::node_interface, 5>
    x3d_nurbs_component::nurbs_swung_surface_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "profileCurve"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "trajectoryCurve"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "ccw"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid")
    };

    const boost::array<openvrml::node_interface, 4>
    x3d_nurbs_component::contour2d_interfaces_ = {
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
                       "children")
    };

    const boost::array<openvrml::node_interface, 18>
    x3d_nurbs_component::nurbs_trimmed_surface_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addTrimmingContour"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeTrimmingContour"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "controlPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "texCoord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "trimmingContour"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "uTessellation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "vTessellation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfdouble_id,
                       "weight"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "uClosed"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "uKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "uOrder"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "vClosed"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vDimension"),
        node_interface(node_interface::field_id,
                       field_value::mfdouble_id,
                       "vKnot"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "vOrder")
    };

    const char * const x3d_nurbs_component::id = "NURBS";

    size_t x3d_nurbs_component::support_level() const OPENVRML_NOTHROW
    {
        return 4;
    }

    void x3d_nurbs_component::do_add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // CoordinateDouble node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::coordinate_double_interfaces_.begin(),
                        x3d_nurbs_component::coordinate_double_interfaces_.end());
                add_scope_entry(b,
                                "CoordinateDouble",
                                interface_set,
                                "urn:X-openvrml:node:CoordinateDouble",
                                scope);
            }

            //
            // NurbsCurve node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_curve_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_curve_interfaces_.end());
                add_scope_entry(b,
                                "NurbsCurve",
                                interface_set,
                                "urn:X-openvrml:node:NurbsCurve",
                                scope);
            }

            //
            // NurbsOrientationInterpolator node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_orientation_interpolator_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_orientation_interpolator_interfaces_.end());
                add_scope_entry(b,
                                "NurbsOrientationInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:NurbsOrientationInterpolator",
                                scope);
            }

            //
            // NurbsPatchSurface node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_patch_surface_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_patch_surface_interfaces_.end());
                add_scope_entry(b,
                                "NurbsPatchSurface",
                                interface_set,
                                "urn:X-openvrml:node:NurbsPatchSurface",
                                scope);
            }

            //
            // NurbsPositionInterpolator node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_position_interpolator_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_position_interpolator_interfaces_.end());
                add_scope_entry(b,
                                "NurbsPositionInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:NurbsPositionInterpolator",
                                scope);
            }

            //
            // NurbsSurfaceInterpolator node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_surface_interpolator_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_surface_interpolator_interfaces_.end());
                add_scope_entry(b,
                                "NurbsSurfaceInterpolator",
                                interface_set,
                                "urn:X-openvrml:node:NurbsSurfaceInterpolator",
                                scope);
            }

            //
            // NurbsTextureCoordinate node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_texture_coordinate_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_texture_coordinate_interfaces_.end());
                add_scope_entry(b,
                                "NurbsTextureCoordinate",
                                interface_set,
                                "urn:X-openvrml:node:NurbsTextureCoordinate",
                                scope);
            }
        }

        if (level >= 2) {
            //
            // NurbsSet node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_set_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_set_interfaces_.end());
                add_scope_entry(b,
                                "NurbsSet",
                                interface_set,
                                "urn:X-openvrml:node:NurbsSet",
                                scope);
            }
        }

        if (level >= 3) {
            //
            // NurbsCurve2D node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_curve2d_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_curve2d_interfaces_.end());
                add_scope_entry(b,
                                "NurbsCurve2D",
                                interface_set,
                                "urn:X-openvrml:node:NurbsCurve2D",
                                scope);
            }

            //
            // ContourPolyline2D node
            //
            {
                // XXX
                // XXX For now, just support the X3D 3.1 version.  When all
                // XXX this gets replaced with something sane, it should take
                // XXX the X3D version into account when determining the
                // XXX component makeup.
                // XXX
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::contour_polyline2d_3_1_interfaces_.begin(),
                        x3d_nurbs_component::contour_polyline2d_3_1_interfaces_.end());
                add_scope_entry(b,
                                "ContourPolyline2D",
                                interface_set,
                                "urn:X-openvrml:node:ContourPolyline2D",
                                scope);
            }

            //
            // NurbsSweptSurface node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_swept_surface_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_swept_surface_interfaces_.end());
                add_scope_entry(b,
                                "NurbsSweptSurface",
                                interface_set,
                                "urn:X-openvrml:node:NurbsSweptSurface",
                                scope);
            }

            //
            // NurbsSwungSurface node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_swung_surface_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_swung_surface_interfaces_.end());
                add_scope_entry(b,
                                "NurbsSwungSurface",
                                interface_set,
                                "urn:X-openvrml:node:NurbsSwungSurface",
                                scope);
            }
        }

        if (level >= 4) {
            //
            // Contour2D node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::contour2d_interfaces_.begin(),
                        x3d_nurbs_component::contour2d_interfaces_.end());
                add_scope_entry(b,
                                "Contour2D",
                                interface_set,
                                "urn:X-openvrml:node:Contour2D",
                                scope);
            }

            //
            // NurbsTrimmedSurface node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_nurbs_component::nurbs_trimmed_surface_interfaces_.begin(),
                        x3d_nurbs_component::nurbs_trimmed_surface_interfaces_.end());
                add_scope_entry(b,
                                "NurbsTrimmedSurface",
                                interface_set,
                                "urn:X-openvrml:node:NurbsTrimmedSurface",
                                scope);
            }
        }
    }

    void
    x3d_nurbs_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "CoordinateDouble",
            node_interface_set(
                x3d_nurbs_component::coordinate_double_interfaces_.begin(),
                x3d_nurbs_component::coordinate_double_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "NurbsCurve",
            node_interface_set(
                x3d_nurbs_component::nurbs_curve_interfaces_.begin(),
                x3d_nurbs_component::nurbs_curve_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "NurbsOrientationInterpolator",
            node_interface_set(
                x3d_nurbs_component::nurbs_orientation_interpolator_interfaces_.begin(),
                x3d_nurbs_component::nurbs_orientation_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "NurbsPatchSurface",
            node_interface_set(
                x3d_nurbs_component::nurbs_patch_surface_interfaces_.begin(),
                x3d_nurbs_component::nurbs_patch_surface_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "NurbsPositionInterpolator",
            node_interface_set(
                x3d_nurbs_component::nurbs_position_interpolator_interfaces_.begin(),
                x3d_nurbs_component::nurbs_position_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "NurbsSurfaceInterpolator",
            node_interface_set(
                x3d_nurbs_component::nurbs_surface_interpolator_interfaces_.begin(),
                x3d_nurbs_component::nurbs_surface_interpolator_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "NurbsTextureCoordinate",
            node_interface_set(
                x3d_nurbs_component::nurbs_texture_coordinate_interfaces_.begin(),
                x3d_nurbs_component::nurbs_texture_coordinate_interfaces_.end()));

        if (level >= 2) {
            component::add_type_desc(
                type_descs,
                "NurbsSet",
                node_interface_set(
                    x3d_nurbs_component::nurbs_set_interfaces_.begin(),
                    x3d_nurbs_component::nurbs_set_interfaces_.end()));
        }

        if (level >= 3) {
            component::add_type_desc(
                type_descs,
                "NurbsCurve2D",
                node_interface_set(
                    x3d_nurbs_component::nurbs_curve2d_interfaces_.begin(),
                    x3d_nurbs_component::nurbs_curve2d_interfaces_.end()));
            // XXX
            // XXX For now, just support the X3D 3.1 version.  When all
            // XXX this gets replaced with something sane, it should take
            // XXX the X3D version into account when determining the
            // XXX component makeup.
            // XXX
            component::add_type_desc(
                type_descs,
                "ContourPolyline2D",
                node_interface_set(
                    x3d_nurbs_component::contour_polyline2d_3_1_interfaces_.begin(),
                    x3d_nurbs_component::contour_polyline2d_3_1_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "NurbsSweptSurface",
                node_interface_set(
                    x3d_nurbs_component::nurbs_swept_surface_interfaces_.begin(),
                    x3d_nurbs_component::nurbs_swept_surface_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "NurbsSwungSurface",
                node_interface_set(
                    x3d_nurbs_component::nurbs_swung_surface_interfaces_.begin(),
                    x3d_nurbs_component::nurbs_swung_surface_interfaces_.end()));
        }

        if (level >= 4) {
            component::add_type_desc(
                type_descs,
                "Contour2D",
                node_interface_set(
                    x3d_nurbs_component::contour2d_interfaces_.begin(),
                    x3d_nurbs_component::contour2d_interfaces_.end()));
            component::add_type_desc(
                type_descs,
                "NurbsTrimmedSurface",
                node_interface_set(
                    x3d_nurbs_component::nurbs_trimmed_surface_interfaces_.begin(),
                    x3d_nurbs_component::nurbs_trimmed_surface_interfaces_.end()));
        }
    }


    class OPENVRML_LOCAL x3d_dis_component : public component {
        static const boost::array<openvrml::node_interface, 88>
            espdu_transform_interfaces_;
        static const boost::array<openvrml::node_interface, 28>
            receiver_pdu_interfaces_;
        static const boost::array<openvrml::node_interface, 28>
            signal_pdu_interfaces_;
        static const boost::array<openvrml::node_interface, 44>
            transmitter_pdu_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 88>
    x3d_dis_component::espdu_transform_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_articulationParameterValue0"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_articulationParameterValue1"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_articulationParameterValue2"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_articulationParameterValue3"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_articulationParameterValue4"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_articulationParameterValue5"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_articulationParameterValue6"),
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_articulationParameterValue7"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "address"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "applicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "articulationParameterCount"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "articulationParameterDesignatorArray"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "articulationParameterChangeIndicatorArray"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "articulationParameterIdPartAttachedToArray"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "articulationParameterTypeArray"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "articulationParameterArray"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "collisionType"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "deadReckoning"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "detonationLocation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "detonationRelativeLocation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "detonationResult"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityCategory"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityCountry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityDomain"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityExtra"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityKind"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entitySpecific"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entitySubCategory"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "eventApplicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "eventEntityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "eventNumber"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "eventSiteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "fired1"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "fired2"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "fireMissionIndex"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "firingRange"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "firingRate"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "forceID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "fuse"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "linearVelocity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "linearAcceleration"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "marking"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "multicastRelayHost"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "multicastRelayPort"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "munitionApplicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "munitionEndPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "munitionEntityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "munitionQuantity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "munitionSiteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "munitionStartPoint"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "networkMode"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "port"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "readInterval"),
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
                       field_value::sfint32_id,
                       "siteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "translation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "warhead"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "writeInterval"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "articulationParameterValue0_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "articulationParameterValue1_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "articulationParameterValue2_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "articulationParameterValue3_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "articulationParameterValue4_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "articulationParameterValue5_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "articulationParameterValue6_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "articulationParameterValue7_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "collideTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "detonateTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "firedTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isCollided"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isDetonated"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkReader"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkWriter"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isRtpHeaderHeard"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isStandAlone"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "timestamp"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "rtpHeaderExpected")
    };

    const boost::array<openvrml::node_interface, 28>
    x3d_dis_component::receiver_pdu_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "address"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "applicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "multicastRelayHost"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "multicastRelayPort"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "networkMode"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "port"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "readInterval"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "receivedPower"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "receiverState"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "rtpHeaderExpected"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "siteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "transmitterApplicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "transmitterEntityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "transmitterRadioID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "transmitterSiteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "whichGeometry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "writeInterval"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkReader"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkWriter"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isRtpHeaderHeard"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isStandAlone"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "timestamp"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 28>
    x3d_dis_component::signal_pdu_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "address"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "applicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "data"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "dataLength"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "encodingScheme"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "multicastRelayHost"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "multicastRelayPort"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "networkMode"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "port"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "readInterval"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "rtpHeaderExpected"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "sampleRate"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "samples"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "siteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "tdlType"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "whichGeometry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "writeInterval"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkReader"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkWriter"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isRtpHeaderHeard"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isStandAlone"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "timestamp"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const boost::array<openvrml::node_interface, 44>
    x3d_dis_component::transmitter_pdu_interfaces_ = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "address"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "antennaLocation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "antennaPatternLength"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "antennaPatternType"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "applicationID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "cryptoKeyID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "cryptoSystem"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "entityID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "frequency"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "inputSource"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "lengthOfModulationParameters"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "modulationTypeDetail"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "modulationTypeMajor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "modulationTypeSpreadSpectrum"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "modulationTypeSystem"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "multicastRelayHost"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "multicastRelayPort"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "networkMode"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "port"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "power"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeCategory"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeCountry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeDomain"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeKind"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeNomenclature"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioEntityTypeNomenclatureVersion"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "radioID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "readInterval"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "relativeAntennaLocation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "rtpHeaderExpected"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "siteID"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "transmitFrequencyBandwidth"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "transmitState"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "whichGeometry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "writeInterval"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkReader"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isNetworkWriter"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isRtpHeaderHeard"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isStandAlone"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "timestamp"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    const char * const x3d_dis_component::id = "DIS";

    size_t x3d_dis_component::support_level() const OPENVRML_NOTHROW
    {
        return 1;
    }

    void x3d_dis_component::do_add_to_scope(const openvrml::browser & b,
                                            openvrml::scope & scope,
                                            const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // EspduTransform node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_dis_component::espdu_transform_interfaces_.begin(),
                        x3d_dis_component::espdu_transform_interfaces_.end());
                add_scope_entry(b,
                                "EspduTransform",
                                interface_set,
                                "urn:X-openvrml:node:EspduTransform",
                                scope);
            }

            //
            // ReceiverPdu node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_dis_component::receiver_pdu_interfaces_.begin(),
                        x3d_dis_component::receiver_pdu_interfaces_.end());
                add_scope_entry(b,
                                "ReceiverPdu",
                                interface_set,
                                "urn:X-openvrml:node:ReceiverPdu",
                                scope);
            }

            //
            // SignalPdu node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_dis_component::signal_pdu_interfaces_.begin(),
                        x3d_dis_component::signal_pdu_interfaces_.end());
                add_scope_entry(b,
                                "SignalPdu",
                                interface_set,
                                "urn:X-openvrml:node:SignalPdu",
                                scope);
            }

            //
            // TransmitterPdu node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_dis_component::transmitter_pdu_interfaces_.begin(),
                        x3d_dis_component::transmitter_pdu_interfaces_.end());
                add_scope_entry(b,
                                "TransmitterPdu",
                                interface_set,
                                "urn:X-openvrml:node:TransmitterPdu",
                                scope);
            }
        }
    }

    void
    x3d_dis_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "EspduTransform",
            node_interface_set(
                x3d_dis_component::espdu_transform_interfaces_.begin(),
                x3d_dis_component::espdu_transform_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "ReceiverPdu",
            node_interface_set(
                x3d_dis_component::receiver_pdu_interfaces_.begin(),
                x3d_dis_component::receiver_pdu_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "SignalPdu",
            node_interface_set(
                x3d_dis_component::signal_pdu_interfaces_.begin(),
                x3d_dis_component::signal_pdu_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TransmitterPdu",
            node_interface_set(
                x3d_dis_component::transmitter_pdu_interfaces_.begin(),
                x3d_dis_component::transmitter_pdu_interfaces_.end()));
    }


    class OPENVRML_LOCAL x3d_event_utilities_component : public component {
        static const boost::array<openvrml::node_interface, 5>
            boolean_filter_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            boolean_sequencer_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            boolean_toggle_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            boolean_trigger_interfaces_;
        static const boost::array<openvrml::node_interface, 7>
            integer_sequencer_interfaces_;
        static const boost::array<openvrml::node_interface, 4>
            integer_trigger_interfaces_;
        static const boost::array<openvrml::node_interface, 3>
            time_trigger_interfaces_;

    public:
        static const char * const id;

        virtual size_t support_level() const OPENVRML_NOTHROW;

    private:
        virtual void do_add_to_scope(const openvrml::browser & b,
                                     openvrml::scope & scope,
                                     size_t level) const
            OPENVRML_THROW1(std::bad_alloc);

        virtual void
            do_add_to_node_type_desc_map(
                openvrml::node_type_decls & type_descs, size_t level) const
            OPENVRML_THROW1(std::bad_alloc);
    };

    const boost::array<openvrml::node_interface, 5>
    x3d_event_utilities_component::boolean_filter_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_event_utilities_component::boolean_sequencer_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 3>
    x3d_event_utilities_component::boolean_toggle_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 3>
    x3d_event_utilities_component::boolean_trigger_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 7>
    x3d_event_utilities_component::integer_sequencer_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 4>
    x3d_event_utilities_component::integer_trigger_interfaces_ = {
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

    const boost::array<openvrml::node_interface, 3>
    x3d_event_utilities_component::time_trigger_interfaces_ = {
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

    const char * const x3d_event_utilities_component::id = "EventUtilities";

    size_t x3d_event_utilities_component::support_level() const
        OPENVRML_NOTHROW
    {
        return 1;
    }

    void
    x3d_event_utilities_component::
    do_add_to_scope(const openvrml::browser & b,
                    openvrml::scope & scope,
                    const size_t level) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        if (level >= 1) {
            //
            // BooleanFilter node
            //
            {
                static const node_interface_set
                    interface_set(
                        x3d_event_utilities_component::boolean_filter_interfaces_.begin(),
                        x3d_event_utilities_component::boolean_filter_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_event_utilities_component::boolean_sequencer_interfaces_.begin(),
                        x3d_event_utilities_component::boolean_sequencer_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_event_utilities_component::boolean_toggle_interfaces_.begin(),
                        x3d_event_utilities_component::boolean_toggle_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_event_utilities_component::boolean_trigger_interfaces_.begin(),
                        x3d_event_utilities_component::boolean_trigger_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_event_utilities_component::integer_sequencer_interfaces_.begin(),
                        x3d_event_utilities_component::integer_sequencer_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_event_utilities_component::integer_trigger_interfaces_.begin(),
                        x3d_event_utilities_component::integer_trigger_interfaces_.end());
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
                static const node_interface_set
                    interface_set(
                        x3d_event_utilities_component::time_trigger_interfaces_.begin(),
                        x3d_event_utilities_component::time_trigger_interfaces_.end());
                add_scope_entry(b,
                                "TimeTrigger",
                                interface_set,
                                "urn:X-openvrml:node:TimeTrigger",
                                scope);
            }
        }
    }

    void
    x3d_event_utilities_component::
    do_add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                 size_t /* level */) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_interface_set;
        component::add_type_desc(
            type_descs,
            "BooleanFilter",
            node_interface_set(
                x3d_event_utilities_component::boolean_filter_interfaces_.begin(),
                x3d_event_utilities_component::boolean_filter_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "BooleanSequencer",
            node_interface_set(
                x3d_event_utilities_component::boolean_sequencer_interfaces_.begin(),
                x3d_event_utilities_component::boolean_sequencer_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "BooleanToggle",
            node_interface_set(
                x3d_event_utilities_component::boolean_toggle_interfaces_.begin(),
                x3d_event_utilities_component::boolean_toggle_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "BooleanTrigger",
            node_interface_set(
                x3d_event_utilities_component::boolean_trigger_interfaces_.begin(),
                x3d_event_utilities_component::boolean_trigger_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "IntegerSequencer",
            node_interface_set(
                x3d_event_utilities_component::integer_sequencer_interfaces_.begin(),
                x3d_event_utilities_component::integer_sequencer_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "IntegerTrigger",
            node_interface_set(
                x3d_event_utilities_component::integer_trigger_interfaces_.begin(),
                x3d_event_utilities_component::integer_trigger_interfaces_.end()));
        component::add_type_desc(
            type_descs,
            "TimeTrigger",
            node_interface_set(
                x3d_event_utilities_component::time_trigger_interfaces_.begin(),
                x3d_event_utilities_component::time_trigger_interfaces_.end()));
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

        key = x3d_geospatial_component::id;
        succeeded = this->insert(key, new x3d_geospatial_component).second;
        assert(succeeded);

        key = x3d_hanim_component::id;
        succeeded = this->insert(key, new x3d_hanim_component).second;
        assert(succeeded);

        key = x3d_nurbs_component::id;
        succeeded = this->insert(key, new x3d_nurbs_component).second;
        assert(succeeded);

        key = x3d_dis_component::id;
        succeeded = this->insert(key, new x3d_dis_component).second;
        assert(succeeded);

        key = x3d_event_utilities_component::id;
        succeeded =
            this->insert(key, new x3d_event_utilities_component).second;
        assert(succeeded);

        key = x3d_cad_geometry_component::id;
        succeeded = this->insert(key, new x3d_cad_geometry_component).second;
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

    std::auto_ptr<openvrml::node_type_decls>
    profile::create_node_type_desc_map() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_type_decls;

        std::auto_ptr<node_type_decls> node_type_descs(new node_type_decls);
        for (map_t::const_iterator entry = this->components_.begin();
             entry != this->components_.end();
             ++entry) {
            try {
                const component & c = ::component_registry_.at(entry->first);
                c.add_to_node_type_desc_map(*node_type_descs, entry->second);
            } catch (boost::bad_ptr_container_operation & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            } catch (std::invalid_argument & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            }
        }
        return node_type_descs;
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


    const char * const x3d_full_profile::id = "Full";

    x3d_full_profile::x3d_full_profile()
    {
        this->add_component(x3d_core_component::id, 2);
        this->add_component(x3d_time_component::id, 2);
        this->add_component(x3d_networking_component::id, 3);
        this->add_component(x3d_grouping_component::id, 3);
        this->add_component(x3d_rendering_component::id, 4);
        this->add_component(x3d_shape_component::id, 3);
        this->add_component(x3d_geometry3d_component::id, 4);
        this->add_component(x3d_geometry2d_component::id, 2);
        this->add_component(x3d_text_component::id, 1);
        this->add_component(x3d_sound_component::id, 1);
        this->add_component(x3d_lighting_component::id, 3);
        this->add_component(x3d_texturing_component::id, 3);
        this->add_component(x3d_interpolation_component::id, 3);
        this->add_component(x3d_pointing_device_sensor_component::id, 1);
        this->add_component(x3d_key_device_sensor_component::id, 2);
        this->add_component(x3d_environmental_sensor_component::id, 2);
        this->add_component(x3d_navigation_component::id, 2);
        this->add_component(x3d_environmental_effects_component::id, 3);
        this->add_component(x3d_geospatial_component::id, 1);
        this->add_component(x3d_hanim_component::id, 1);
        this->add_component(x3d_nurbs_component::id, 4);
        this->add_component(x3d_dis_component::id, 1);
        this->add_component(x3d_event_utilities_component::id, 1);
    }
} // namespace
