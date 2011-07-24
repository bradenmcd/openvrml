// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2010  Braden McDaniel
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

# ifndef OPENVRML_NODE_VRML97_GROUPING_NODE_BASE_H
#   define OPENVRML_NODE_VRML97_GROUPING_NODE_BASE_H

# include <openvrml/node_impl_util.h>
# include <openvrml/viewer.h>
# include <boost/scope_exit.hpp>

namespace openvrml_node_vrml97 {

    /**
     * @brief Base class template for grouping nodes.
     */
    template <typename Derived>
    class OPENVRML_LOCAL grouping_node_base :
        public openvrml::node_impl_util::abstract_node<Derived>,
        public virtual openvrml::grouping_node {
    protected:
        typedef Derived self_t;

        class add_children_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public openvrml::node_impl_util::abstract_node<Derived>::mfnode_listener {
        public:
            explicit add_children_listener(
                grouping_node_base<Derived> & node);
            virtual ~add_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfnode & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public openvrml::node_impl_util::abstract_node<Derived>::mfnode_listener {
        public:
            explicit remove_children_listener(
                grouping_node_base<Derived> & node);
            virtual ~remove_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfnode & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class children_exposedfield :
# if defined(_MSC_VER) && _MSC_VER == 1500
            public openvrml::node_impl_util::abstract_node<Derived>::exposedfield<openvrml::mfnode> {
# else
            public openvrml::node_impl_util::abstract_node<Derived>::template exposedfield<openvrml::mfnode> {
# endif
        public:
            explicit children_exposedfield(openvrml::node & node) OPENVRML_NOTHROW;
            children_exposedfield(const children_exposedfield & obj) OPENVRML_NOTHROW;
            virtual ~children_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::mfnode & value,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        // so it can be overridden by switch node.
        virtual void do_children_event_side_effect(const openvrml::mfnode & value,
                                                   double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

        openvrml::sfvec3f bbox_center_;
        openvrml::sfvec3f bbox_size_;
        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        children_exposedfield children_;

        openvrml::bounding_sphere bsphere;

    public:
        grouping_node_base(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~grouping_node_base() OPENVRML_NOTHROW;

    protected:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
        virtual const openvrml::bounding_volume &
        do_bounding_volume() const;
        virtual const std::vector<boost::intrusive_ptr<openvrml::node> >
            do_children() const OPENVRML_THROW1(std::bad_alloc);

        virtual void recalc_bsphere();
        void render_nocull(openvrml::viewer & viewer,
                           openvrml::rendering_context context);
    };

    /**
     * @internal
     *
     * @class grouping_node_base::add_children_listener
     *
     * @brief addChildren event listener.
     */

    /**
     * @brief Construct.
     *
     * @param node  a grouping node.
     */
    template <typename Derived>
    grouping_node_base<Derived>::add_children_listener::
    add_children_listener(grouping_node_base<Derived> & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<Derived>(node),
        openvrml::node_impl_util::abstract_node<Derived>::mfnode_listener(node)
    {}

   /**
    * @brief Destroy.
    */
    template <typename Derived>
    grouping_node_base<Derived>::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process an event.
     *
     * If this function throws, it is possible (even likely) that only part of
     * the nodes in @p value have been added to the Group node's children
     * exposedField.
     *
     * @param value     @link openvrml::child_node child_nodes@endlink to add.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc        if memory allocation fails.
     */
    template <typename Derived>
    void
    grouping_node_base<Derived>::add_children_listener::
    do_process_event(const openvrml::mfnode & value, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        Derived & group = dynamic_cast<Derived &>(this->node());

        typedef std::vector<boost::intrusive_ptr<openvrml::node> > children_t;
        children_t children = group.children_.value();

        for (children_t::const_iterator n = value.value().begin();
             n != value.value().end();
             ++n) {
            //
            // Don't add NULLs.
            //
            if (*n) {
                using std::find;

                children_t::iterator pos =
                    find(children.begin(), children.end(), *n);
                if (pos == children.end()) {
                    //
                    // Throws std::bad_alloc.
                    //
                    children.push_back(*n);
                    bool succeeded = false;
                    BOOST_SCOPE_EXIT_TPL((&succeeded)(&children)) {
                        if (!succeeded) { children.pop_back(); }
                    } BOOST_SCOPE_EXIT_END
                    child_node * const child =
                        node_cast<child_node *>(n->get());
                    if (child) {
                        child->relocate(); // Throws std::bad_alloc.
                    }
                    succeeded = true;
                }
            }
        }

        group.children_.value(children);

        group.node::modified(true);
        group.bounding_volume_dirty(true);
        node::emit_event(group.children_, timestamp);
    }

    /**
     * @internal
     *
     * @class grouping_node_base::remove_children_listener
     *
     * @brief removeChildren event listener.
     */

    /**
     * @brief Construct.
     *
     * @param node  a grouping node.
     */
    template <typename Derived>
    grouping_node_base<Derived>::
    remove_children_listener::
    remove_children_listener(grouping_node_base<Derived> & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<Derived>(node),
        openvrml::node_impl_util::abstract_node<Derived>::mfnode_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    template <typename Derived>
    grouping_node_base<Derived>::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process an event.
     */
    template <typename Derived>
    void
    grouping_node_base<Derived>::remove_children_listener::
    do_process_event(const openvrml::mfnode & value, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        self_t & group = dynamic_cast<self_t &>(this->node());

        typedef std::vector<boost::intrusive_ptr<openvrml::node> > children_t;
        children_t children = group.children_.mfnode::value();

        for (children_t::const_iterator n = value.value().begin();
             n != value.value().end();
             ++n) {
            using std::remove;
            children.erase(remove(children.begin(), children.end(), *n),
                           children.end());
        }

        group.children_.mfnode::value(children);

        group.node::modified(true);
        group.bounding_volume_dirty(true);
        node::emit_event(group.children_, timestamp);
    }

    /**
     * @internal
     *
     * @class grouping_node_base::children_exposedfield
     *
     * @brief children exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  group_node.
     */
    template <typename Derived>
    grouping_node_base<Derived>::children_exposedfield::
    children_exposedfield(openvrml::node & node) OPENVRML_NOTHROW:
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        openvrml::node_impl_util::abstract_node<Derived>::mfnode_listener(node),
        openvrml::node_impl_util::abstract_node<Derived>::template exposedfield<openvrml::mfnode>(
            node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    template <typename Derived>
    grouping_node_base<Derived>::children_exposedfield::
    children_exposedfield(const children_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        openvrml::node_impl_util::abstract_node<Derived>::mfnode_listener(
            obj.openvrml::node_event_listener::node()),
        openvrml::node_impl_util::abstract_node<Derived>::template exposedfield<openvrml::mfnode>(obj)
    {}

    /**
     * @brief Destroy.
     */
    template <typename Derived>
    grouping_node_base<Derived>::children_exposedfield::
    ~children_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     */
    template <typename Derived>
    std::auto_ptr<openvrml::field_value>
    grouping_node_base<Derived>::children_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new children_exposedfield(*this));
    }

    /**
     * @brief Handle event.
     */
    template <typename Derived>
    void
    grouping_node_base<Derived>::children_exposedfield::
    event_side_effect(const openvrml::mfnode & value, double time)
        OPENVRML_THROW1(std::bad_alloc)
    {
        grouping_node_base<Derived> & group =
            dynamic_cast<grouping_node_base<Derived> &>(
                this->openvrml::node_event_listener::node());
        group.do_children_event_side_effect(value, time);
    }

    /**
     * @brief Handle event.
     */
    template <typename Derived>
    void grouping_node_base<Derived>::
    do_children_event_side_effect(const openvrml::mfnode & value, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using namespace openvrml;

        typedef std::vector<boost::intrusive_ptr<openvrml::node> > children_t;
        children_t children;

        for (children_t::const_iterator n = value.value().begin();
             n != value.value().end();
             ++n) {
            //
            // The spec is ambiguous about whether the children field of
            // grouping nodes can contain NULLs. We allow it; for now, at
            // least.
            //
            children.push_back(*n); // Throws std::bad_alloc.
            bool succeeded = false;
            BOOST_SCOPE_EXIT_TPL((&succeeded)(&children)) {
                if (!succeeded) { children.pop_back(); }
            } BOOST_SCOPE_EXIT_END
            child_node * const child =
                node_cast<child_node *>(n->get());
            if (child) { child->relocate(); } // Throws std::bad_alloc.
            succeeded = true;
        }

        this->children_.mfnode::value(children);

        this->bounding_volume_dirty(true);
    }

    /**
     * @var openvrml::sfvec3f grouping_node_base::bbox_center_
     *
     * @brief bboxCenter field.
     */

    /**
     * @var openvrml::sfvec3f grouping_node_base::bbox_size_
     *
     * @brief bboxSize field.
     */

    /**
     * @var grouping_node_base::add_children_listener grouping_node_base::add_children_listener_
     *
     * @brief addChildren eventIn handler.
     */

    /**
     * @var grouping_node_base::remove_children_listener grouping_node_base::remove_children_listener_
     *
     * @brief removeChildren eventIn handler.
     */

    /**
     * @var grouping_node_base::children_exposedfield grouping_node_base::children_
     *
     * @brief children exposedField.
     */

    /**
     * @var openvrml::bounding_sphere grouping_node_base::bsphere
     *
     * @brief Cached copy of the bounding_sphere enclosing this node's children.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope the scope to which the node belongs.
     */
    template <typename Derived>
    grouping_node_base<Derived>::
    grouping_node_base(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml::node_impl_util::abstract_node<Derived>(type, scope),
        bbox_size_(openvrml::make_vec3f(-1.0, -1.0, -1.0)),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        children_(*this)
    {
        this->bounding_volume_dirty(true);
    }

    /**
     * @brief Destroy.
     */
    template <typename Derived>
    grouping_node_base<Derived>::~grouping_node_base() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if one of the node's children has been modified,
     *         @c false otherwise.
     */
    template <typename Derived>
    bool grouping_node_base<Derived>::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        for (size_t i = 0; i < this->children_.value().size(); ++i) {
            if (this->children_.value()[i]->modified()) { return true; }
        }
        return false;
    }

    /**
     * @brief Render the node.
     *
     * Render each of the children.
     *
     * @param viewer    a Viewer.
     * @param context   a rendering context.
     */
    template <typename Derived>
    void
    grouping_node_base<Derived>::
    do_render_child(openvrml::viewer & viewer, openvrml::rendering_context context)
    {
        using namespace openvrml;
        if (context.cull_flag != bounding_volume::inside) {
            using boost::polymorphic_downcast;
            const bounding_sphere & bs =
                *polymorphic_downcast<const bounding_sphere *>(
                    &this->bounding_volume());
            bounding_sphere bv_copy(bs);
            bv_copy.transform(context.matrix());
            bounding_volume::intersection r =
                viewer.intersect_view_volume(bv_copy);
            if (context.draw_bounding_spheres) {
                viewer.draw_bounding_sphere(bs, r);
            }
            if (r == bounding_volume::outside) { return; }
            if (r == bounding_volume::inside) {
                context.cull_flag = bounding_volume::inside;
            }
        }
        this->render_nocull(viewer, context);
    }

    /**
     * because children will already have done the culling, we don't need
     * to repeat it here.
     */
    template <typename Derived>
    void
    grouping_node_base<Derived>::render_nocull(openvrml::viewer & viewer,
                                               openvrml::rendering_context context)
    {
        using std::vector;
        using namespace openvrml;

        if (this->modified()) {
            viewer.remove_object(*this);
        }

        if (!this->children_.mfnode::value().empty()) {
            vector<boost::intrusive_ptr<node> >::size_type i;
            vector<boost::intrusive_ptr<node> >::size_type n =
                this->children_.mfnode::value().size();
            size_t nSensors = 0;

            viewer.begin_object(this->id().c_str());

            // Draw nodes that impact their siblings (DirectionalLights,
            // TouchSensors, any others? ...)
            for (i = 0; i < n; ++i) {
                child_node * const child =
                    node_cast<child_node *>(
                        this->children_.mfnode::value()[i].get());
                if (child) {
                    if (node_cast<light_node *>(child)
                        && !(node_cast<scoped_light_node *>(child))) {
                        child->render_child(viewer, context);
                    } else if (
                        node_cast<pointing_device_sensor_node *>(child)) {
                        if (++nSensors == 1) {
                            viewer.set_sensitive(this);
                        }
                    }
                }
            }

            // Do the rest of the children (except the scene-level lights)
            for (i = 0; i<n; ++i) {
                child_node * const child =
                    node_cast<child_node *>(
                        this->children_.mfnode::value()[i].get());
                if (child && !(node_cast<light_node *>(child))) {
                    child->render_child(viewer, context);
                }
            }

            // Turn off sensitivity
            if (nSensors > 0) { viewer.set_sensitive(0); }

            viewer.end_object();
        }

        this->node::modified(false);
    }

    /**
     * @brief Get the children in the scene graph.
     *
     * @return the child nodes in the scene graph.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    template <typename Derived>
    const std::vector<boost::intrusive_ptr<openvrml::node> >
    grouping_node_base<Derived>::do_children() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return this->children_.openvrml::mfnode::value();
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    template <typename Derived>
    const openvrml::bounding_volume &
    grouping_node_base<Derived>::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<grouping_node_base<Derived> *>(this)
                ->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @fn void grouping_node_base<Derived>::recalc_bsphere()
     *
     * @brief Recalculate the bounding volume.
     */
    template <typename Derived>
    void grouping_node_base<Derived>::recalc_bsphere()
    {
        using namespace openvrml;
        this->bsphere = openvrml::bounding_sphere();
        for (size_t i = 0; i < this->children_.mfnode::value().size(); ++i) {
            const boost::intrusive_ptr<node> & node = this->children_.mfnode::value()[i];
            bounded_volume_node * bounded_volume =
                node_cast<bounded_volume_node *>(node.get());
            if (bounded_volume) {
                const openvrml::bounding_volume & ci_bv =
                    bounded_volume->bounding_volume();
                this->bsphere.extend(ci_bv);
            }
        }
        this->bounding_volume_dirty(false);
    }
}

# endif // ifndef OPENVRML_NODE_VRML97_GROUPING_NODE_BASE_H
