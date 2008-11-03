// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  Braden McDaniel
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

# include "proto.h"
# include "field_value_types.h"
# include <private.h>
# include <openvrml/scope.h>
# include <boost/mpl/for_each.hpp>
# include <list>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

/**
 * @internal
 *
 * @brief @c node_type for @c PROTO node instances.
 */
class OPENVRML_LOCAL openvrml::local::proto_node_metatype::proto_node_type :
    public openvrml::node_type {

    openvrml::node_interface_set interfaces_;

public:
    proto_node_type(const openvrml::local::proto_node_metatype & node_metatype,
                    const std::string & id,
                    const openvrml::node_interface_set & interfaces)
        OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);
    virtual ~proto_node_type() OPENVRML_NOTHROW;

private:
    virtual const openvrml::node_interface_set & do_interfaces() const
        OPENVRML_NOTHROW;
    virtual const boost::intrusive_ptr<openvrml::node>
    do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                   const openvrml::initial_value_map & initial_values) const
        OPENVRML_THROW1(std::bad_alloc);
};

namespace openvrml {

    namespace local {

        /**
         * @internal
         *
         * @brief A @c PROTO instance node.
         *
         * Like a typical node implementation, @c proto_node%s have a
         * many-to-one relationship with the @c proto_node_type instance that
         * creates them.  And @c proto_node_type has, in turn, a many-to-one
         * relationship with the @c proto_node_metatype instance that creates
         * them.  Unlike a typical node implementation, there will very likely
         * be more than one @c proto_node_metatype instance known to the
         * @c browser instance; there will be one for each @c PROTO known to
         * the @c browser.
         *
         * As the @c proto_node_metatype encodes the data in a @c PROTO, the
         * @c proto_node_type can be seen as modeling @c EXTERNPROTO. Each
         * @c EXTERNPROTO will spawn a new @c proto_node_type from the
         * @c proto_node_metatype that corresponds to the @c PROTO to which the
         * @c EXTERNPROTO refers.  Recall that an @c EXTERNPROTO provides a
         * subset of the interfaces defined for a @c PROTO; thus, for a
         * @c PROTO with <var>n</var> interfaces, there are <var>n</var>!
         * possible unique @c EXTERNPROTO%s (and thus unique
         * @c proto_node_type instances).
         *
         * Structurally, the implementation of @c proto_node is very similar
         * to that of @c proto_node_metatype. The difference is that event
         * pathways for @c ROUTE%s and @c IS mappings are actually created in
         * the @c proto_node.  The @c proto_node_metatype, on the other hand,
         * includes metadata about how these event pathways @e should be
         * created.
         */
        class OPENVRML_LOCAL proto_node : public abstract_proto_node {

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
                    const openvrml::field_value & initial_value,
                    proto_node & node,
                    boost::shared_ptr<openvrml::event_listener> & exposedfield):
                    initial_value_(&initial_value),
                    node_(&node),
                    exposedfield_(&exposedfield)
                {}

                template <typename T>
                void operator()(T) const
                {
                    if (T::field_value_type_id == this->initial_value_->type())
                        {
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
                create_exposedfield(const openvrml::field_value & initial_value,
                                    proto_node & node)
                OPENVRML_THROW1(std::bad_alloc);

            boost::shared_ptr<openvrml::scope> proto_scope;
            std::vector<boost::intrusive_ptr<node> > impl_nodes;

        public:
            proto_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope,
                       const openvrml::initial_value_map & initial_values)
                OPENVRML_THROW1(std::bad_alloc);
            virtual ~proto_node() OPENVRML_NOTHROW;

            virtual bool modified() const;

        private:
            virtual void do_initialize(double timestamp)
                OPENVRML_THROW1(std::bad_alloc);

            virtual
            const openvrml::field_value & do_field(const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface);
            virtual openvrml::event_listener &
                do_event_listener(const std::string & id)
                OPENVRML_THROW1(openvrml::unsupported_interface);
            virtual openvrml::event_emitter &
                do_event_emitter(const std::string & id)
                OPENVRML_THROW1(openvrml::unsupported_interface);

            virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;

            virtual openvrml::script_node * to_script() OPENVRML_NOTHROW;
            virtual openvrml::appearance_node * to_appearance()
                OPENVRML_NOTHROW;
            virtual openvrml::bounded_volume_node * to_bounded_volume()
                OPENVRML_NOTHROW;
            virtual openvrml::child_node * to_child() OPENVRML_NOTHROW;
            virtual openvrml::color_node * to_color() OPENVRML_NOTHROW;
            virtual openvrml::color_rgba_node * to_color_rgba()
                OPENVRML_NOTHROW;
            virtual openvrml::coordinate_node * to_coordinate()
                OPENVRML_NOTHROW;
            virtual openvrml::font_style_node * to_font_style()
                OPENVRML_NOTHROW ;
            virtual openvrml::geometry_node * to_geometry() OPENVRML_NOTHROW;
            virtual openvrml::grouping_node * to_grouping() OPENVRML_NOTHROW;
            virtual openvrml::light_node * to_light() OPENVRML_NOTHROW;
            virtual openvrml::material_node * to_material() OPENVRML_NOTHROW;
            virtual openvrml::navigation_info_node * to_navigation_info()
                OPENVRML_NOTHROW;
            virtual openvrml::normal_node * to_normal() OPENVRML_NOTHROW;
            virtual
            openvrml::pointing_device_sensor_node * to_pointing_device_sensor()
                OPENVRML_NOTHROW;
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
}

namespace {

    class OPENVRML_LOCAL node_path_element {
    public:
        std::vector<boost::intrusive_ptr<openvrml::node> >::size_type index;
        openvrml::field_value::type_id field_type;
        std::string field_id;

        node_path_element();
    };

    node_path_element::node_path_element():
        index(0),
        field_type(openvrml::field_value::invalid_type_id)
    {}

    /**
     * @brief A @c node path is used to store the path to a @c node for the
     *        purpose of duplicating a route in a cloned @c node hierarchy.
     */
    typedef std::list<node_path_element> node_path_t;

    class OPENVRML_LOCAL path_getter : boost::noncopyable {
        const openvrml::node & objective;
        node_path_t & node_path;
        bool found;

    public:
        path_getter(const openvrml::node & objective, node_path_t & node_path)
            OPENVRML_NOTHROW;

        void get_path_from(const boost::intrusive_ptr<openvrml::node> & node)
            OPENVRML_THROW1(std::bad_alloc);
        void get_path_from(
            const std::vector<boost::intrusive_ptr<openvrml::node> > & nodes)
            OPENVRML_THROW1(std::bad_alloc);

    private:
        void traverse_children(openvrml::node & n)
            OPENVRML_THROW1(std::bad_alloc);
    };

    path_getter::path_getter(const openvrml::node & objective,
                             node_path_t & node_path)
        OPENVRML_NOTHROW:
        objective(objective),
        node_path(node_path),
        found(false)
    {}

    void
    path_getter::
    get_path_from(const boost::intrusive_ptr<openvrml::node> & node)
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
        const std::vector<boost::intrusive_ptr<openvrml::node> > & nodes)
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

    void path_getter::traverse_children(openvrml::node & n)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::field_value;
        using openvrml::node_interface;
        using openvrml::node_interface_set;
        using openvrml::unsupported_interface;
        using openvrml::sfnode;
        using openvrml::mfnode;
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
    get_path(const std::vector<boost::intrusive_ptr<openvrml::node> > & root,
             const openvrml::node & objective)
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
    OPENVRML_LOCAL openvrml::node *
    resolve_node_path(
        const node_path_t & path,
        const std::vector<boost::intrusive_ptr<openvrml::node> > & root)
    {
        using boost::next;
        using boost::prior;
        using openvrml::node;
        using openvrml::field_value;
        using openvrml::sfnode;
        using openvrml::mfnode;
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
            } catch (openvrml::unsupported_interface & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            }
        }
        return result;
    }

    class OPENVRML_LOCAL field_value_cloner {
    protected:
        const boost::shared_ptr<openvrml::scope> & target_scope;
        std::set<openvrml::node *> traversed_nodes;

    public:
        explicit field_value_cloner(
            const boost::shared_ptr<openvrml::scope> & target_scope):
            target_scope(target_scope)
        {
            assert(target_scope);
        }

        virtual ~field_value_cloner()
        {}

        void
        clone_field_value(const boost::intrusive_ptr<openvrml::node> & src_node,
                          const openvrml::field_value & src,
                          openvrml::field_value & dest)
            OPENVRML_THROW1(std::bad_alloc)
        {
            using openvrml::field_value;
            using openvrml::sfnode;
            using openvrml::mfnode;
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
        virtual const boost::intrusive_ptr<openvrml::node>
        clone_node(const boost::intrusive_ptr<openvrml::node> & n)
            OPENVRML_THROW1(std::bad_alloc)
        {
            using std::set;
            using boost::intrusive_ptr;
            using openvrml::node;

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
                using openvrml::node_interface_set;
                openvrml::initial_value_map initial_values;
                const node_interface_set & interfaces =
                    n->type().interfaces();
                for (node_interface_set::const_iterator interface_ =
                         interfaces.begin();
                     interface_ != interfaces.end();
                     ++interface_) {
                    using std::string;
                    using openvrml::node_interface;
                    const node_interface::type_id type = interface_->type;
                    const string & id = interface_->id;
                    if (type == node_interface::exposedfield_id
                        || type == node_interface::field_id) {
                        using std::auto_ptr;
                        using boost::shared_ptr;
                        using openvrml::field_value;
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

        void clone_sfnode(const boost::intrusive_ptr<openvrml::node> & src_node,
                          const openvrml::sfnode & src,
                          openvrml::sfnode & dest)
            OPENVRML_THROW1(std::bad_alloc)
        {
            dest.value((src.value() == src_node)
                       ? openvrml::node::self_tag
                       : this->clone_node(src.value()));
        }

        void clone_mfnode(const boost::intrusive_ptr<openvrml::node> & src_node,
                          const openvrml::mfnode & src,
                          openvrml::mfnode & dest)
            OPENVRML_THROW1(std::bad_alloc)
        {
            using std::swap;
            using std::vector;
            using openvrml::node;
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
}


//
// Clone the implementation nodes.
//
class OPENVRML_LOCAL openvrml::local::proto_node_metatype::proto_impl_cloner :
    public field_value_cloner {

    const openvrml::local::proto_node_metatype & node_metatype;
    const openvrml::initial_value_map & initial_values_;

public:
    proto_impl_cloner(
        const openvrml::local::proto_node_metatype & node_metatype,
        const openvrml::initial_value_map & initial_values,
        const boost::shared_ptr<openvrml::scope> & target_scope):
        field_value_cloner(target_scope),
        node_metatype(node_metatype),
        initial_values_(initial_values)
        {}

    const std::vector<boost::intrusive_ptr<openvrml::node> > clone()
        OPENVRML_THROW1(std::bad_alloc)
    {
        using std::vector;
        using openvrml::node;

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
    std::unary_function<
    openvrml::local::proto_node_metatype::is_map_t::value_type,
        bool> {

        explicit matches_is_target(
            const openvrml::local::proto_node_metatype::is_target &
            is_target):
            is_target(is_target)
            {}

        result_type operator()(const argument_type & is_map_value) const
            {
                return (is_map_value.second.impl_node
                        == this->is_target.impl_node)
                    && (is_map_value.second.impl_node_interface
                        == this->is_target.impl_node_interface);
            }

    private:
        const openvrml::local::proto_node_metatype::is_target & is_target;
    };

    virtual const boost::intrusive_ptr<openvrml::node>
        clone_node(const boost::intrusive_ptr<openvrml::node> & n)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using std::set;
        using boost::intrusive_ptr;
        using openvrml::node;

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
            using openvrml::node_interface_set;
            openvrml::initial_value_map initial_values;
            const node_interface_set & interfaces =
                n->type().interfaces();
            for (node_interface_set::const_iterator interface_ =
                     interfaces.begin();
                 interface_ != interfaces.end();
                 ++interface_) {
                using std::string;
                using openvrml::node_interface;
                const node_interface::type_id type = interface_->type;
                const string & id = interface_->id;
                if (type == node_interface::exposedfield_id
                    || type == node_interface::field_id) {
                    using std::auto_ptr;
                    using std::find_if;
                    using boost::shared_ptr;
                    using openvrml::field_value;
                    auto_ptr<const field_value> src_val;
                    auto_ptr<field_value> dest_val;

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
                                dest_val = initial_value->second->clone();
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

                    //
                    // See above logic; we don't clone subtrees from the
                    // initial_values; just ones from the default values
                    // and the PROTO definition body.
                    //
                    if (src_val.get()) {
                        assert(!dest_val.get());
                        dest_val =
                            field_value::create(interface_->field_type);
                        this->clone_field_value(n, *src_val, *dest_val);
                    }

                    assert(dest_val.get());

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

    void clone_sfnode(const openvrml::sfnode & src, openvrml::sfnode & dest)
        OPENVRML_THROW1(std::bad_alloc)
    {
        dest.value(this->clone_node(src.value()));
    }

    void clone_mfnode(const openvrml::mfnode & src, openvrml::mfnode & dest)
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
openvrml::local::proto_node_metatype::proto_node_type::proto_node_type(
    const openvrml::local::proto_node_metatype & node_metatype,
    const std::string & id,
    const openvrml::node_interface_set & interfaces)
OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc):
node_type(node_metatype, id)
{
    using std::find;
    using std::invalid_argument;
    using openvrml::node_interface_set;
    for (node_interface_set::const_iterator interface_ = interfaces.begin();
         interface_ != interfaces.end();
         ++interface_) {
        node_interface_set::const_iterator pos =
            find(node_metatype.interfaces.begin(),
                 node_metatype.interfaces.end(),
                 *interface_);
        if (pos == node_metatype.interfaces.end()) {
            throw openvrml::unsupported_interface(*interface_);
        }
        const bool succeeded = this->interfaces_.insert(*interface_).second;
        assert(succeeded);
    }
}

/**
 * @brief Destroy.
 */
openvrml::local::proto_node_metatype::proto_node_type::~proto_node_type()
    OPENVRML_NOTHROW
{}

/**
 * @brief Interfaces.
 *
 * @return the interfaces.
 */
const openvrml::node_interface_set &
openvrml::local::proto_node_metatype::proto_node_type::do_interfaces() const
    OPENVRML_NOTHROW
{
    return this->interfaces_;
}

const boost::intrusive_ptr<openvrml::node>
openvrml::local::proto_node_metatype::proto_node_type::
do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
               const openvrml::initial_value_map & initial_values) const
    OPENVRML_THROW1(std::bad_alloc)
{
    return boost::intrusive_ptr<node>(
        new proto_node(*this, scope, initial_values));
}


/**
 * @brief Construct.
 *
 * @param[in] impl_node             a node in the @c PROTO implementation.
 * @param[in] impl_node_interface   an interface of @p impl_node.
 */
openvrml::local::proto_node_metatype::
is_target::is_target(node & impl_node, const std::string & impl_node_interface):
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
openvrml::local::proto_node_metatype::route::route(node & from,
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
 * @param[in] browser
 * @param[in] interfaces
 * @param[in] default_value_map
 * @param[in] impl_nodes
 * @param[in] is_map
 * @param[in] routes
 */
openvrml::local::proto_node_metatype::
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

openvrml::local::proto_node_metatype::~proto_node_metatype() OPENVRML_NOTHROW
{}

const boost::shared_ptr<openvrml::node_type>
openvrml::local::proto_node_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    return boost::shared_ptr<node_type>(
        new proto_node_type(*this, id, interfaces));
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
boost::shared_ptr<openvrml::event_listener>
openvrml::local::abstract_proto_node::
create_eventin(const field_value::type_id type, abstract_proto_node & node)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::mpl::for_each;
    using openvrml::local::field_value_types;

    boost::shared_ptr<openvrml::event_listener> result;
    for_each<field_value_types>(proto_eventin_creator(type, node, result));
    assert(result);
    return result;
}

/**
 * @brief Factory function for @c proto_eventout<FieldValue> instances.
 *
 * @param[in] type  @c field_value::type_id.
 * @param[in] node  @c abstract_proto_node.
 *
 * @return a @c boost::shared_ptr to a @c proto_eventout<FieldValue> instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
boost::shared_ptr<openvrml::event_emitter>
openvrml::local::abstract_proto_node::
create_eventout(const field_value::type_id type, abstract_proto_node & node)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::mpl::for_each;
    using openvrml::local::field_value_types;

    boost::shared_ptr<openvrml::event_emitter> result;
    for_each<field_value_types>(proto_eventout_creator(type, node, result));
    assert(result);
    return result;
}

openvrml::local::abstract_proto_node::abstract_proto_node(
    const openvrml::node_type & type,
    const boost::shared_ptr<openvrml::scope> & scope)
    OPENVRML_NOTHROW:
    openvrml::node(type, scope)
{}

openvrml::local::abstract_proto_node::~abstract_proto_node() OPENVRML_NOTHROW
{}


/**
 * @internal
 *
 * @class openvrml::local::proto_node::proto_exposedfield
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
openvrml::local::proto_node::
proto_exposedfield<FieldValue>::
proto_exposedfield(abstract_proto_node & node,
                   const FieldValue & initial_value):
    openvrml::node_event_listener(node),
    openvrml::event_emitter(this->listener.value),
    proto_eventin<FieldValue>(node),
    proto_eventout<FieldValue>(node, initial_value)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
openvrml::local::proto_node::
proto_exposedfield<FieldValue>::~proto_exposedfield()
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
openvrml::local::proto_node::
proto_exposedfield<FieldValue>::
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
boost::shared_ptr<openvrml::event_listener>
openvrml::local::proto_node::
create_exposedfield(const openvrml::field_value & initial_value,
                    proto_node & node)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::mpl::for_each;
    using openvrml::local::field_value_types;

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
openvrml::local::proto_node::
proto_node(const openvrml::node_type & type,
           const boost::shared_ptr<openvrml::scope> & scope,
           const openvrml::initial_value_map & initial_values)
    OPENVRML_THROW1(std::bad_alloc):
    abstract_proto_node(type, scope),
    proto_scope(scope)
{
    const proto_node_metatype & node_metatype =
        static_cast<const proto_node_metatype &>(type.metatype());

    this->impl_nodes =
        proto_node_metatype::proto_impl_cloner(node_metatype,
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
                    using openvrml::local::field_value_types;

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
                    using openvrml::local::field_value_types;

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
                    using openvrml::local::field_value_types;

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
openvrml::local::proto_node::~proto_node() OPENVRML_NOTHROW
{}

/**
 * @brief Indicate whether the @c node needs to be rerendered.
 *
 * @return @c true if the node has changed and needs to be rerendered;
 *         @c false otherwise.
 */
bool openvrml::local::proto_node::modified() const
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
void openvrml::local::proto_node::
do_initialize(const double timestamp)
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
const openvrml::field_value &
openvrml::local::proto_node::
do_field(const std::string & id) const
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
openvrml::event_listener &
openvrml::local::proto_node::
do_event_listener(const std::string & id)
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
openvrml::event_emitter &
openvrml::local::proto_node::
do_event_emitter(const std::string & id)
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
 * @brief Shutdown.
 *
 * @param[in] timestamp the current time.
 */
void
openvrml::local::proto_node::
do_shutdown(const double timestamp)
    OPENVRML_NOTHROW
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
openvrml::script_node *
openvrml::local::proto_node::to_script() OPENVRML_NOTHROW
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
openvrml::appearance_node *
openvrml::local::proto_node::to_appearance()
    OPENVRML_NOTHROW
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
openvrml::bounded_volume_node *
openvrml::local::proto_node::to_bounded_volume()
    OPENVRML_NOTHROW
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
openvrml::child_node *
openvrml::local::proto_node::to_child()
    OPENVRML_NOTHROW
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
openvrml::color_node *
openvrml::local::proto_node::to_color()
    OPENVRML_NOTHROW
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
openvrml::color_rgba_node *
openvrml::local::proto_node::to_color_rgba()
    OPENVRML_NOTHROW
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
openvrml::coordinate_node *
openvrml::local::proto_node::to_coordinate()
    OPENVRML_NOTHROW
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
openvrml::font_style_node *
openvrml::local::proto_node::to_font_style()
    OPENVRML_NOTHROW
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
openvrml::geometry_node *
openvrml::local::proto_node::to_geometry()
    OPENVRML_NOTHROW
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
openvrml::grouping_node *
openvrml::local::proto_node::to_grouping()
    OPENVRML_NOTHROW
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
openvrml::light_node *
openvrml::local::proto_node::to_light()
    OPENVRML_NOTHROW
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
openvrml::material_node *
openvrml::local::proto_node::to_material()
    OPENVRML_NOTHROW
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
openvrml::navigation_info_node *
openvrml::local::proto_node::to_navigation_info()
    OPENVRML_NOTHROW
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
openvrml::normal_node *
openvrml::local::proto_node::to_normal()
    OPENVRML_NOTHROW
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
openvrml::pointing_device_sensor_node *
openvrml::local::proto_node::to_pointing_device_sensor()
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
openvrml::scoped_light_node *
openvrml::local::proto_node::to_scoped_light()
    OPENVRML_NOTHROW
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
openvrml::sound_source_node *
openvrml::local::proto_node::to_sound_source()
    OPENVRML_NOTHROW
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
openvrml::texture_node *
openvrml::local::proto_node::to_texture()
    OPENVRML_NOTHROW
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
openvrml::texture_coordinate_node *
openvrml::local::proto_node::to_texture_coordinate()
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
openvrml::texture_transform_node *
openvrml::local::proto_node::to_texture_transform()
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
openvrml::time_dependent_node *
openvrml::local::proto_node::to_time_dependent()
    OPENVRML_NOTHROW
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
openvrml::transform_node *
openvrml::local::proto_node::to_transform()
    OPENVRML_NOTHROW
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
openvrml::viewpoint_node *
openvrml::local::proto_node::to_viewpoint()
    OPENVRML_NOTHROW
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<viewpoint_node *>(this->impl_nodes[0].get());
}
