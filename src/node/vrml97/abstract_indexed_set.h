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

# ifndef OPENVRML_NODE_VRML97_ABSTRACT_INDEXED_SET_H
#   define OPENVRML_NODE_VRML97_ABSTRACT_INDEXED_SET_H

#   include <openvrml/node_impl_util.h>

namespace openvrml_node_vrml97 {

    /**
     * @brief Abstract base class for IndexedFaceSet and IndexedLineSet.
     */
    template <typename Derived>
    class OPENVRML_LOCAL abstract_indexed_set_node :
        public openvrml::node_impl_util::abstract_node<Derived>,
        public openvrml::geometry_node {
    protected:
        typedef typename openvrml::node_impl_util::abstract_node<Derived>::self_t
            self_t;

        class set_color_index_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public openvrml::node_impl_util::abstract_node<Derived>::mfint32_listener {
        public:
            explicit set_color_index_listener(
                abstract_indexed_set_node & node);
            virtual ~set_color_index_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfint32 & color_index,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_coord_index_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public openvrml::node_impl_util::abstract_node<Derived>::mfint32_listener {
        public:
            explicit set_coord_index_listener(
                abstract_indexed_set_node & node);
            virtual ~set_coord_index_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfint32 & coord_index,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_color_index_listener set_color_index_;
        set_coord_index_listener set_coord_index_;
        typename openvrml::node_impl_util::abstract_node<Derived>::
            template exposedfield<openvrml::sfnode> color_;
        typename openvrml::node_impl_util::abstract_node<Derived>::
            template exposedfield<openvrml::sfnode> coord_;
        openvrml::mfint32 color_index_;
        openvrml::sfbool color_per_vertex_;
        openvrml::mfint32 coord_index_;

    public:
        virtual ~abstract_indexed_set_node() OPENVRML_NOTHROW = 0;

        virtual const openvrml::color_node * do_color() const OPENVRML_NOTHROW;

    protected:
        abstract_indexed_set_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);

        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);
    };

    /**
     * @internal
     *
     * @class abstract_indexed_set_node::set_color_index_listener
     *
     * @brief set_colorIndex event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  abstract_indexed_set_node.
     */
    template <typename Derived>
    abstract_indexed_set_node<Derived>::set_color_index_listener::
    set_color_index_listener(abstract_indexed_set_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<Derived>(node),
        openvrml::node_impl_util::abstract_node<Derived>::mfint32_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    template <typename Derived>
    abstract_indexed_set_node<Derived>::set_color_index_listener::
    ~set_color_index_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param color_index colorIndex.
     * @param timestamp   the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    template <typename Derived>
    void
    abstract_indexed_set_node<Derived>::set_color_index_listener::
    do_process_event(const openvrml::mfint32 & color_index, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        abstract_indexed_set_node * abstract_indexed_set =
            dynamic_cast<abstract_indexed_set_node *>(&this->node());
        assert(abstract_indexed_set);
        abstract_indexed_set->color_index_ = color_index;
        abstract_indexed_set->node::modified(true);
    }

    /**
     * @internal
     *
     * @class abstract_indexed_set_node::set_coord_index_listener
     *
     * @brief set_coordIndex event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  abstract_indexed_set_node.
     */
    template <typename Derived>
    abstract_indexed_set_node<Derived>::set_coord_index_listener::
    set_coord_index_listener(abstract_indexed_set_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<Derived>(node),
        openvrml::node_impl_util::abstract_node<Derived>::mfint32_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    template <typename Derived>
    abstract_indexed_set_node<Derived>::set_coord_index_listener::
    ~set_coord_index_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param coord_index coordIndex.
     * @param timestamp   the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    template <typename Derived>
    void
    abstract_indexed_set_node<Derived>::set_coord_index_listener::
    do_process_event(const openvrml::mfint32 & coord_index, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        abstract_indexed_set_node * abstract_indexed_set =
            dynamic_cast<abstract_indexed_set_node *>(&this->node());
        assert(abstract_indexed_set);
        abstract_indexed_set->coord_index_ = coord_index;
        abstract_indexed_set->node::modified(true);
    }

    /**
     * @var abstract_indexed_set_node<Derived>::set_color_index_listener abstract_indexed_set_node<Derived>::set_color_index_
     *
     * @brief set_colorIndex event handler.
     */

    /**
     * @var abstract_indexed_set_node<Derived>::set_coord_index_listener template abstract_indexed_set_node<Derived>::set_coord_index_
     *
     * @brief set_coordIndex event handler.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<Derived>::exposedfield<openvrml::sfnode> abstract_indexed_set_node<Derived>::color_
     *
     * @brief color exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<Derived>::exposedfield<openvrml::sfnode> abstract_indexed_set_node<Derived>::coord_
     *
     * @brief coord exposedField.
     */

    /**
     * @var openvrml::mfint32 abstract_indexed_set_node<Derived>::color_index_
     *
     * @brief colorIndex field.
     */

    /**
     * @var openvrml::sfbool abstract_indexed_set_node<Derived>::color_per_vertex_
     *
     * @brief colorPerVertex field.
     */

    /**
     * @var openvrml::mfint32 abstract_indexed_set_node<Derived>::coord_index_
     *
     * @brief coordIndex field.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope that the new node will belong to.
     */
    template <typename Derived>
    abstract_indexed_set_node<Derived>::abstract_indexed_set_node(
        const openvrml::node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<Derived>(type, scope),
        geometry_node(type, scope),
        set_color_index_(*this),
        set_coord_index_(*this),
        color_(*this),
        coord_(*this),
        color_per_vertex_(true)
    {}

    /**
     * @brief Destroy.
     */
    template <typename Derived>
    abstract_indexed_set_node<Derived>::~abstract_indexed_set_node()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    template <typename Derived>
    bool abstract_indexed_set_node<Derived>::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return (this->color_.value() && this->color_.value()->modified())
            || (this->coord_.value() && this->coord_.value()->modified());
    }

    /**
     * @brief color_node.
     *
     * @return the color_node, or 0 if none is set.
     */
    template <typename Derived>
    const openvrml::color_node *
    abstract_indexed_set_node<Derived>::do_color() const OPENVRML_NOTHROW
    {
        using openvrml::sfnode;
        return openvrml::node_cast<openvrml::color_node *>(
            this->color_.sfnode::value().get());
    }
}

# endif // OPENVRML_NODE_VRML97_ABSTRACT_INDEXED_SET_H
