// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 1999  Kumaran Santhanam
// Copyright (C) 2001, 2002  Braden McDaniel
// Copyright (C) 2002  S. K. Bose
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

# include <algorithm>
# include <iostream>
# include <iterator>
# include <limits>
# ifdef OPENVRML_ENABLE_TEXT_NODE
#   include <ft2build.h>
#   include FT_FREETYPE_H
#   include FT_GLYPH_H
#   include FT_OUTLINE_H
#   include <fontconfig/fontconfig.h>
extern "C" {
#   include <fontconfig/fcfreetype.h>
}
# endif
# include "vrml97node.h"
# include "doc.h"
# include "browser.h"
# include "private.h"

namespace openvrml {

/**
 * @brief Implementations of the VRML97 nodes.
 *
 * It is a legacy of OpenVRML's initial architecture that these classes are
 * exposed as part of the public API. Once all the dependencies on members of
 * this namespace have been factored out of other parts of OpenVRML, all of
 * these classes will be moving into the library's implementation.
 */
namespace vrml97_node {

namespace {

    /**
     * @internal
     */
    class vrml97_node_type : public node_type {
    public:
        virtual ~vrml97_node_type() throw () = 0;
        virtual void field_value(openvrml::node & node, const std::string & id,
                                 const openvrml::field_value &) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc) = 0;
        virtual const openvrml::field_value &
        field_value(const openvrml::node & node,
                    const std::string & id) const
            throw (unsupported_interface) = 0;
        virtual void
        dispatch_eventin(openvrml::node & node, const std::string & id,
                         const openvrml::field_value &, double timestamp) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc) = 0;
        virtual const openvrml::field_value &
        eventout_value(const openvrml::node & node,
                       const std::string & id) const
            throw (unsupported_interface) = 0;

    protected:
        vrml97_node_type(openvrml::node_class & node_class,
                         const std::string & id);
    };

    vrml97_node_type::vrml97_node_type(openvrml::node_class & node_class,
                                       const std::string & id):
        node_type(node_class, id)
    {}

    vrml97_node_type::~vrml97_node_type() throw ()
    {}


    template <typename NodeT>
    class node_field_ptr {
    public:
        virtual ~node_field_ptr() = 0;
        virtual field_value & dereference(NodeT & obj) = 0;
        virtual const field_value & dereference(const NodeT & obj) = 0;
    };

    template <typename NodeT>
    node_field_ptr<NodeT>::~node_field_ptr()
    {}


    template <typename NodeT, typename ConcreteFieldValue>
    class node_field_ptr_impl : public node_field_ptr<NodeT> {
        ConcreteFieldValue NodeT::* itsPtr;

    public:
        node_field_ptr_impl(ConcreteFieldValue NodeT::* ptr):
            itsPtr(ptr)
        {}

        virtual ~node_field_ptr_impl();

        virtual field_value & dereference(NodeT &);
        virtual const field_value & dereference(const NodeT &);
    };

    template <typename NodeT, typename ConcreteFieldValue>
    node_field_ptr_impl<NodeT, ConcreteFieldValue>::~node_field_ptr_impl()
    {}

    template <typename NodeT, typename ConcreteFieldValue>
    field_value &
    node_field_ptr_impl<NodeT, ConcreteFieldValue>::dereference(NodeT & obj)
    {
        return obj.*itsPtr;
    }

    template <typename NodeT, typename ConcreteFieldValue>
    const field_value &
    node_field_ptr_impl<NodeT, ConcreteFieldValue>::dereference(
        const NodeT & obj)
    {
        return obj.*itsPtr;
    }


    template <typename NodeT>
    class vrml97_node_type_impl : public vrml97_node_type {
    public:
        typedef node_field_ptr_impl<NodeT, sfbool> sfbool_ptr;
        typedef node_field_ptr_impl<NodeT, sfcolor> sfcolor_ptr;
        typedef node_field_ptr_impl<NodeT, sffloat> sffloat_ptr;
        typedef node_field_ptr_impl<NodeT, sfint32> sfint32_ptr;
        typedef node_field_ptr_impl<NodeT, sfimage> sfimage_ptr;
        typedef node_field_ptr_impl<NodeT, sfnode> sfnode_ptr;
        typedef node_field_ptr_impl<NodeT, sfrotation> sfrotation_ptr;
        typedef node_field_ptr_impl<NodeT, sfstring> sfstring_ptr;
        typedef node_field_ptr_impl<NodeT, sftime> sftime_ptr;
        typedef node_field_ptr_impl<NodeT, sfvec2f> sfvec2f_ptr;
        typedef node_field_ptr_impl<NodeT, sfvec3f> sfvec3f_ptr;
        typedef node_field_ptr_impl<NodeT, mfcolor> mfcolor_ptr;
        typedef node_field_ptr_impl<NodeT, mffloat> mffloat_ptr;
        typedef node_field_ptr_impl<NodeT, mfint32> mfint32_ptr;
        typedef node_field_ptr_impl<NodeT, mfnode> mfnode_ptr;
        typedef node_field_ptr_impl<NodeT, mfrotation> mfrotation_ptr;
        typedef node_field_ptr_impl<NodeT, mfstring> mfstring_ptr;
        typedef node_field_ptr_impl<NodeT, mftime> mftime_ptr;
        typedef node_field_ptr_impl<NodeT, mfvec2f> mfvec2f_ptr;
        typedef node_field_ptr_impl<NodeT, mfvec3f> mfvec3f_ptr;

        typedef boost::shared_ptr<node_field_ptr<NodeT> >
            node_field_ptr_ptr;
        typedef void (NodeT::* eventin_handler_ptr)
            (const openvrml::field_value &, double);

    private:
        node_interface_set interfaces_;
        typedef std::map<std::string, eventin_handler_ptr>
            eventin_handler_map_t;
        typedef std::map<std::string, node_field_ptr_ptr> field_value_map_t;
        mutable field_value_map_t field_value_map;
        eventin_handler_map_t eventin_handler_map;
        field_value_map_t eventout_value_map;

    public:
        vrml97_node_type_impl(openvrml::node_class & node_class,
                              const std::string & id);
        virtual ~vrml97_node_type_impl() throw ();

        void add_eventin(field_value::type_id, const std::string & id,
                         eventin_handler_ptr eventInHandlerPtr)
            throw (unsupported_interface, std::bad_alloc);
        void add_eventout(field_value::type_id, const std::string & id,
                          const node_field_ptr_ptr & eventOutPtrPtr)
            throw (unsupported_interface, std::bad_alloc);
        void add_exposedfield(field_value::type_id, const std::string & id,
                              eventin_handler_ptr eventInHandlerPtr,
                              const node_field_ptr_ptr & fieldPtrPtr)
            throw (unsupported_interface, std::bad_alloc);
        void add_field(field_value::type_id, const std::string & id,
                       const node_field_ptr_ptr & fieldPtrPtr)
            throw (unsupported_interface, std::bad_alloc);

        virtual void field_value(openvrml::node & node, const std::string & id,
                                 const openvrml::field_value &) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const openvrml::field_value &
        field_value(const openvrml::node & node, const std::string & id) const
            throw (unsupported_interface);
        virtual void dispatch_eventin(openvrml::node & node,
                                      const std::string & id,
                                      const openvrml::field_value &,
                                      double timestamp) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const openvrml::field_value &
        eventout_value(const openvrml::node & node,
                       const std::string & id) const
            throw (unsupported_interface);

        virtual const node_interface_set & interfaces() const throw ();
        virtual const node_ptr create_node(const scope_ptr & scope) const
            throw (std::bad_alloc);

    private:
        void do_field_value(NodeT & node, const std::string & id,
                            const openvrml::field_value &) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        const openvrml::field_value &
        do_field_value(const NodeT & node,
                       const std::string & id) const
            throw (unsupported_interface);
        void do_dispatch_eventin(NodeT & node,
                                 const std::string & id,
                                 const openvrml::field_value &,
                                 double timestamp) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        const openvrml::field_value &
        do_eventout_value(const NodeT & node, const std::string & id) const
            throw (unsupported_interface);
    };

    template <typename NodeT>
    vrml97_node_type_impl<NodeT>::vrml97_node_type_impl(
        openvrml::node_class & node_class,
        const std::string & id):
        vrml97_node_type(node_class, id)
    {}

    template <typename NodeT>
    vrml97_node_type_impl<NodeT>::~vrml97_node_type_impl() throw ()
    {}

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::add_eventin(
            const field_value::type_id type,
            const std::string & id,
            const eventin_handler_ptr eventInHandlerPtr)
        throw (unsupported_interface, std::bad_alloc)
    {
        const node_interface interface(node_interface::eventin_id, type, id);
        this->interfaces_.add(interface);
        const typename eventin_handler_map_t::value_type
                value(id, eventInHandlerPtr);
        const bool succeeded = this->eventin_handler_map.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::add_eventout(
            const field_value::type_id type,
            const std::string & id,
            const node_field_ptr_ptr & eventOutPtrPtr)
        throw (unsupported_interface, std::bad_alloc)
    {
        const node_interface interface(node_interface::eventout_id, type, id);
        this->interfaces_.add(interface);
        const typename field_value_map_t::value_type value(id, eventOutPtrPtr);
        const bool succeeded = this->eventout_value_map.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::add_exposedfield(
            const field_value::type_id type,
            const std::string & id,
            const eventin_handler_ptr eventInHandlerPtr,
            const node_field_ptr_ptr & fieldPtrPtr)
        throw (unsupported_interface, std::bad_alloc)
    {
        const node_interface interface(node_interface::exposedfield_id,
                                       type,
                                       id);
        this->interfaces_.add(interface);

        bool succeeded;
        {
            const typename eventin_handler_map_t::value_type
                    value("set_" + id, eventInHandlerPtr);
            succeeded = this->eventin_handler_map.insert(value).second;
            assert(succeeded);
        }
        {
            const typename field_value_map_t::value_type
                value(id, fieldPtrPtr);
            succeeded = this->field_value_map.insert(value).second;
            assert(succeeded);
        }
        {
            const typename field_value_map_t::value_type
                    value(id + "_changed", fieldPtrPtr);
            succeeded = this->eventout_value_map.insert(value).second;
            assert(succeeded);
        }
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::add_field(
            const field_value::type_id type,
            const std::string & id,
            const node_field_ptr_ptr & nodeFieldPtrPtr)
        throw (unsupported_interface, std::bad_alloc)
    {
        const node_interface interface(node_interface::field_id, type, id);
        this->interfaces_.add(interface);
        const typename field_value_map_t::value_type
            value(id, nodeFieldPtrPtr);
        const bool succeeded = this->field_value_map.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::field_value(
            openvrml::node & node,
            const std::string & id,
            const openvrml::field_value & newVal) const
        throw (unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        assert(dynamic_cast<NodeT *>(&node));
        this->do_field_value(dynamic_cast<NodeT &>(node), id, newVal);
    }

    template <typename NodeT>
    const field_value &
    vrml97_node_type_impl<NodeT>::field_value(const openvrml::node & node,
                                              const std::string & id) const
        throw (unsupported_interface)
    {
        assert(dynamic_cast<const NodeT *>(&node));
        return this->do_field_value(dynamic_cast<const NodeT &>(node), id);
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::dispatch_eventin(
        openvrml::node & node,
        const std::string & id,
        const openvrml::field_value & value,
        const double timestamp) const
        throw (unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        assert(dynamic_cast<NodeT *>(&node));
        this->do_dispatch_eventin(dynamic_cast<NodeT &>(node), id, value,
                                  timestamp);
    }

    template <typename NodeT>
    const field_value &
    vrml97_node_type_impl<NodeT>::eventout_value(const openvrml::node & node,
                                                 const std::string & id) const
        throw (unsupported_interface)
    {
        assert(dynamic_cast<const NodeT *>(&node));
        return this->do_eventout_value(dynamic_cast<const NodeT &>(node), id);
    }

    template <typename NodeT>
    const node_interface_set & vrml97_node_type_impl<NodeT>::interfaces() const
        throw ()
    {
        return this->interfaces_;
    }

    template <typename NodeT>
    const node_ptr
    vrml97_node_type_impl<NodeT>::create_node(const scope_ptr & scope) const
        throw (std::bad_alloc)
    {
        return node_ptr(new NodeT(*this, scope));
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::do_field_value(
            NodeT & node,
            const std::string & id,
            const openvrml::field_value & newVal) const
        throw (unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        typename field_value_map_t::iterator itr =
            this->field_value_map.find(id);
        if (itr == this->field_value_map.end()) {
            throw unsupported_interface(node.openvrml::node::type,
                                        node_interface::field_id,
                                        id);
        }
        itr->second->dereference(node).assign(newVal);
    }

    template <typename NodeT>
    const field_value &
    vrml97_node_type_impl<NodeT>::do_field_value(const NodeT & node,
                                                 const std::string & id) const
        throw (unsupported_interface)
    {
        const typename field_value_map_t::const_iterator itr =
                this->field_value_map.find(id);
        if (itr == this->field_value_map.end()) {
            throw unsupported_interface(node.openvrml::node::type,
                                        node_interface::field_id,
                                        id);
        }
        return itr->second->dereference(node);
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::do_dispatch_eventin(
            NodeT & node,
            const std::string & id,
            const openvrml::field_value & value,
            const double timestamp) const
        throw (unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        typename eventin_handler_map_t::const_iterator
                itr(this->eventin_handler_map.find(id));
        if (itr == this->eventin_handler_map.end()) {
            itr = this->eventin_handler_map.find("set_" + id);
        }
        if (itr == this->eventin_handler_map.end()) {
            throw unsupported_interface(node.openvrml::node::type,
                                        node_interface::eventin_id,
                                        id);
        }
        (node.*(itr->second))(value, timestamp);
    }

    template <typename NodeT>
    const field_value &
    vrml97_node_type_impl<NodeT>::do_eventout_value(
        const NodeT & node,
        const std::string & id) const
        throw (unsupported_interface)
    {
        typename field_value_map_t::const_iterator
                itr(this->eventout_value_map.find(id));
        if (itr == this->eventout_value_map.end()) {
            itr = this->eventout_value_map.find(id + "_changed");
        }
        if (itr == this->eventout_value_map.end()) {
            throw unsupported_interface(node.openvrml::node::type,
                                        node_interface::eventout_id,
                                        id);
        }
        return itr->second->dereference(node);
    }
}

/**
 * @class abstract_base
 *
 * @brief Abstract base class for VRML97 node implementations.
 *
 * abstract_base encapsulates the mechanisms for field access and mutation,
 * event dispatch, and eventOut access.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
abstract_base::abstract_base(const node_type & type, const scope_ptr & scope):
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
abstract_base::~abstract_base() throw ()
{}

/**
 * @brief Set a field value for a node.
 *
 * @param id    a field name.
 * @param value a field_value.
 *
 * @exception unsupported_interface  if the node has no field @p id.
 * @exception std::bad_cast         if @p value is not the correct type.
 * @exception std::bad_alloc        if memory allocation fails.
 *
 * @pre @p value must be of the correct type.
 */
void abstract_base::do_field(const std::string & id, const field_value & value)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    assert(dynamic_cast<const vrml97_node_type *>(&this->type));
    static_cast<const vrml97_node_type &>(this->type)
        .field_value(*this, id, value);
}

/**
 * @brief Get a field value for a node.
 *
 * @param id    a field name.
 *
 * @exception unsupported_interface  if the node has no field @p id.
 */
const field_value & abstract_base::do_field(const std::string & id) const
    throw (unsupported_interface)
{
    assert(dynamic_cast<const vrml97_node_type *>(&this->type));
    return static_cast<const vrml97_node_type &>(this->type)
        .field_value(*this, id);
}

/**
 * @brief Process an event.
 *
 * @param id        an eventIn name.
 * @param value     a field_value.
 * @param timestamp the current time.
 *
 * @exception unsupported_interface  if the node has no eventIn @p id.
 * @exception std::bad_cast         if @p value is not the correct type.
 * @exception std::bad_alloc        if memory allocation fails.
 *
 * @pre @p value must be of the correct type.
 */
void abstract_base::do_process_event(const std::string & id,
                                     const field_value & value,
                                     const double timestamp)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    assert(dynamic_cast<const vrml97_node_type *>(&this->type));
    static_cast<const vrml97_node_type &>(this->type)
        .dispatch_eventin(*this, id, value, timestamp);
}

/**
 * @brief Get an eventOut value for a node.
 *
 * @param id    an eventOut name.
 *
 * @exception unsupported_interface  if the node has no eventOut @p id.
 */
const field_value & abstract_base::do_eventout(const std::string & id) const
    throw (unsupported_interface)
{
    assert(dynamic_cast<const vrml97_node_type *>(&this->type));
    return static_cast<const vrml97_node_type &>(this->type)
        .eventout_value(*this, id);
}


/**
 * @class abstract_child_node
 *
 * @brief Base class for all child nodes.
 *
 * VRML97 defines "child" nodes as those that may occur at the root of the
 * scene, in the "children" field of a Group, Transform, Anchor, or Billboard
 * node, in the "level" field of a LOD node, or in the "choice" field of a
 * Switch node.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type for the node.
 * @param scope     the scope the new node should belong to.
 */
abstract_child_node::abstract_child_node(const node_type & type,
                                         const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
abstract_child_node::~abstract_child_node() throw ()
{}

/**
 * @class abstract_geometry_node
 *
 * @brief Base class for all geometry nodes.
 */

/**
 * @var viewer::object_t abstract_geometry_node::viewerObject
 *
 * @brief Handle for the renderer.
 *
 * @todo Move this to node?
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type for the node.
 * @param scope     the scope the new node should belong to.
 */
abstract_geometry_node::abstract_geometry_node(const node_type & type,
                                               const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    geometry_node(type, scope),
    viewerObject(0)
{}

/**
 * @brief Destroy.
 */
abstract_geometry_node::~abstract_geometry_node() throw ()
{
    /* Need access to viewer to delete viewerObject...*/
}

/**
 * @brief Render this node.
 *
 * Subclasses need only define insert_geometry(), not render().
 *
 * @param viewer a renderer
 * @param context the renderer context
 */
void abstract_geometry_node::render(openvrml::viewer & viewer,
                                    rendering_context context)
{
    if (this->viewerObject && this->modified()) {
        viewer.remove_object(this->viewerObject);
        this->viewerObject = 0;
    }

    if (this->viewerObject) {
        viewer.insert_reference(this->viewerObject);
    } else {
        this->viewerObject = this->insert_geometry(viewer, context);
        this->modified(false);
    }
}


/**
 * @class abstract_indexed_set_node
 *
 * @brief Abstract base class for IndexedFaceSet and IndexedLineSet.
 */

/**
 * @var sfnode abstract_indexed_set_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @var mfint32 abstract_indexed_set_node::colorIndex
 *
 * @brief colorIndex field.
 */

/**
 * @var sfbool abstract_indexed_set_node::colorPerVertex
 *
 * @brief colorPerVertex field.
 */

/**
 * @var sfnode abstract_indexed_set_node::coord
 *
 * @brief coord exposedField.
 */

/**
 * @var mfint32 abstract_indexed_set_node::coordIndex
 *
 * @brief coordIndex field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
abstract_indexed_set_node::abstract_indexed_set_node(const node_type & type,
                                                     const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope),
    colorPerVertex(true)
{}

/**
 * @brief Destroy.
 */
abstract_indexed_set_node::~abstract_indexed_set_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool abstract_indexed_set_node::modified() const
{
    return (this->node::modified()
            || (this->color_.value && this->color_.value->modified())
            || (this->coord.value && this->coord.value->modified()));
}

/**
 * @brief color_node.
 *
 * @return the color_node, or 0 if none is set.
 */
const openvrml::color_node * abstract_indexed_set_node::color() const throw ()
{
    return this->color_.value
            ? this->color_.value->to_color()
            : 0;
}

/**
 * @brief set_color eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void abstract_indexed_set_node::process_set_color(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->color_ = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("color_changed", this->color_, timestamp);
}

/**
 * @brief set_colorIndex eventIn handler.
 *
 * @param value     an mfint32 value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfint32.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
abstract_indexed_set_node::process_set_colorIndex(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->colorIndex = dynamic_cast<const mfint32 &>(value);
    this->node::modified(true);
}

/**
 * @brief set_coord eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void abstract_indexed_set_node::process_set_coord(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->coord = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("coord_changed", this->coord, timestamp);
}

/**
 * @brief set_coordIndex eventIn handler.
 *
 * @param value     an mfint32 value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfint32.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
abstract_indexed_set_node::process_set_coordIndex(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->coordIndex = dynamic_cast<const mfint32 &>(value);
    this->node::modified(true);
}


/**
 * @class abstract_light_node
 *
 * @brief Base class for all light nodes.
 */

/**
 * @var sffloat abstract_light_node::ambientIntensity
 *
 * @brief ambientIntensity exposedField.
 */

/**
 * @var sfcolor abstract_light_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @var sffloat abstract_light_node::intensity_
 *
 * @brief intensity exposedField.
 */

/**
 * @var sfbool abstract_light_node::on_
 *
 * @brief on exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type for the node.
 * @param scope     the scope to which the node belongs.
 */
abstract_light_node::abstract_light_node(const node_type & type,
                                         const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    ambientIntensity(0.0),
    color_(openvrml::color(1.0, 1.0, 1.0)),
    intensity_(1.0),
    on_(true)
{}

/**
 * @brief Destroy.
 */
abstract_light_node::~abstract_light_node() throw ()
{}

/**
 * @brief Downcast to a light node.
 *
 * @return a pointer to this object.
 */
abstract_light_node * abstract_light_node::to_light() const
{
    return const_cast<abstract_light_node *>(this);
}

/**
 * @brief Ambient intensity.
 *
 * @return the ambient intensity.
 */
float abstract_light_node::ambient_intensity() const throw ()
{
    return this->ambientIntensity.value;
}

/**
 * @brief Intensity.
 *
 * @return the intensity.
 */
float abstract_light_node::intensity() const throw ()
{
    return this->intensity_.value;
}

/**
 * @brief Whether the light is on.
 *
 * @return @c true if the light is on; @c false otherwise.
 */
bool abstract_light_node::on() const throw ()
{
    return this->on_.value;
}

/**
 * @brief Light color.
 *
 * @return the light color.
 */
const openvrml::color & abstract_light_node::color() const throw ()
{
    return this->color_.value;
}

/**
 * @brief Render this node as scoped.
 *
 * The default version of this method does nothing.
 *
 * @param viewer a renderer.
 */
void abstract_light_node::renderScoped(openvrml::viewer & viewer)
{}

/**
 * @brief set_ambientIntensity eventIn handler.
 *
 * @param value     an SFFloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void
abstract_light_node::process_set_ambientIntensity(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast)
{
    this->ambientIntensity = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("ambientIntensity_changed", this->ambientIntensity,
                    timestamp);
}

/**
 * @brief set_color eventIn handler.
 *
 * @param value     an sfcolor value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfcolor.
 */
void abstract_light_node::process_set_color(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast)
{
    this->color_ = dynamic_cast<const sfcolor &>(value);
    this->node::modified(true);
    this->emit_event("color_changed", this->color_, timestamp);
}

/**
 * @brief set_intensity eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void abstract_light_node::process_set_intensity(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->intensity_ = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("intensity_changed", this->intensity_, timestamp);
}

/**
 * @brief set_on eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void abstract_light_node::process_set_on(const field_value & value,
                                         const double timestamp)
    throw (std::bad_cast)
{
    this->on_ = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("on_changed", this->on_, timestamp);
}


/**
 * @class abstract_texture_node
 *
 * @brief Abstract base class for VRML97 texture nodes.
 */

/**
 * @var sfbool abstract_texture_node::repeatS
 *
 * @brief repeatS field.
 */

/**
 * @var sfbool abstract_texture_node::repeatT
 *
 * @brief repeatT field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type for the node instance.
 * @param scope the scope to which the node belongs.
 */
abstract_texture_node::abstract_texture_node(const node_type & type,
                                             const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    texture_node(type, scope),
    repeatS(true),
    repeatT(true)
{}

/**
 * @brief Destroy.
 */
abstract_texture_node::~abstract_texture_node() throw ()
{}

/**
 * @brief Get the repeatS flag.
 *
 * @return @c true if the texture should repeat in the <var>S</var> direction,
 *      @c false otherwise.
 */
bool abstract_texture_node::repeat_s() const throw ()
{
    return this->repeatS.value;
}

/**
 * @brief Get the repeatT flag.
 *
 * @return @c true if the texture should repeat in the <var>T</var> direction,
 *      @c false otherwise.
 */
bool abstract_texture_node::repeat_t() const throw ()
{
    return this->repeatT.value;
}


/**
 * @class anchor_class.
 *
 * @brief Class object for Anchor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this anchor_class.
 */
anchor_class::anchor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
anchor_class::~anchor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Anchor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by anchor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
anchor_class::create_type(const std::string & id,
                          const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef anchor_node node_t;
    typedef vrml97_node_type_impl<anchor_node> node_type_t;

    const node_type_ptr type(new node_type_t(*this, id));
    node_type_t & anchorNodeType = static_cast<node_type_t &>(*type);
    typedef node_type_t::node_field_ptr_ptr field_ptr_ptr;
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            anchorNodeType.add_eventin(supportedInterfaces[0].field_type,
                                       supportedInterfaces[0].id,
                                       &node_t::process_addChildren);
        } else if (*interface == supportedInterfaces[1]) {
            anchorNodeType.add_eventin(supportedInterfaces[1].field_type,
                                       supportedInterfaces[1].id,
                                       &node_t::process_removeChildren);
        } else if (*interface == supportedInterfaces[2]) {
            anchorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &anchor_node::process_set_children,
                field_ptr_ptr(
                    new node_type_t::mfnode_ptr(&node_t::children_)));
        } else if (*interface == supportedInterfaces[3]) {
            anchorNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &anchor_node::process_set_description,
                field_ptr_ptr(
                    new node_type_t::sfstring_ptr(&node_t::description)));
        } else if (*interface == supportedInterfaces[4]) {
            anchorNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &anchor_node::process_set_parameter,
                field_ptr_ptr(
                    new node_type_t::mfstring_ptr(&node_t::parameter)));
        } else if (*interface == supportedInterfaces[5]) {
            anchorNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &anchor_node::process_set_url,
                field_ptr_ptr(new node_type_t::mfstring_ptr(&node_t::url)));
        } else if (*interface == supportedInterfaces[6]) {
            anchorNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                field_ptr_ptr(
                    new node_type_t::sfvec3f_ptr(&node_t::bboxCenter)));
        } else if (*interface == supportedInterfaces[7]) {
            anchorNodeType.add_field(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                field_ptr_ptr(
                    new node_type_t::sfvec3f_ptr(&node_t::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class anchor_node
 *
 * @brief Represents Anchor node instances.
 */

/**
 * @var anchor_node::anchor_class
 *
 * @brief Class object for Anchor nodes.
 */

/**
 * @var sfstring anchor_node::description
 *
 * @brief description exposedField
 */

/**
 * @var mfstring anchor_node::parameter
 *
 * @brief parameter exposedField
 */

/**
 * @var mfstring anchor_node::url
 *
 * @brief url exposedField
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
anchor_node::anchor_node(const node_type & type,
                         const scope_ptr & scope):
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    group_node(type, scope)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
anchor_node::~anchor_node() throw ()
{}

/**
 * @brief set_description eventIn handler.
 *
 * @param value     an sfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void anchor_node::process_set_description(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->description = dynamic_cast<const sfstring &>(value);
    this->emit_event("description_changed", this->description, timestamp);
}

/**
 * @brief set_parameter eventIn handler.
 *
 * @param value     an mfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void anchor_node::process_set_parameter(const field_value & value,
                                        const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->parameter = dynamic_cast<const mfstring &>(value);
    this->emit_event("parameter_changed", this->parameter, timestamp);
}

/**
 * @brief set_url eventIn handler.
 *
 * @param value     an mfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void anchor_node::process_set_url(const field_value & value,
                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->url = dynamic_cast<const mfstring &>(value);
    this->emit_event("url_changed", this->url, timestamp);
}

/**
 * @brief Cast to an anchor.
 *
 * @return a pointer to this node.
 */
anchor_node * anchor_node::to_anchor() const
{
    return const_cast<anchor_node *>(this);
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void anchor_node::render(openvrml::viewer & viewer,
                         const rendering_context context)
{
    viewer.set_sensitive(this);

    // Render children
    this->group_node::render(viewer, context);

    viewer.set_sensitive(0);
}

/**
 * @brief Handle a click by loading the url.
 */
void anchor_node::activate()
{
    assert(this->scene());
    this->scene()->load_url(this->url.value, this->parameter.value);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & anchor_node::bounding_volume() const
{
    return this->group_node::bounding_volume();
}

/**
 * @class appearance_class
 *
 * @brief Class object for Appearance nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
appearance_class::appearance_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
appearance_class::~appearance_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Appearance nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by appearance_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
appearance_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef appearance_node node_t;
    typedef vrml97_node_type_impl<appearance_node> node_type_t;

    const node_type_ptr type(new node_type_t(*this, id));
    node_type_t & appearanceNodeType = static_cast<node_type_t &>(*type);
    typedef node_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            appearanceNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &node_t::process_set_material,
                node_field_ptr_ptr(
                    new node_type_t::sfnode_ptr(&node_t::material_)));
        } else if (*interface == supportedInterfaces[1]) {
            appearanceNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &node_t::process_set_texture,
                node_field_ptr_ptr(
                    new node_type_t::sfnode_ptr(&node_t::texture_)));
        } else if (*interface == supportedInterfaces[2]) {
            appearanceNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &node_t::process_set_textureTransform,
                node_field_ptr_ptr(
                    new node_type_t::sfnode_ptr(&node_t::textureTransform)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class appearance_node
 *
 * @brief Appearance node instances.
 */

/**
 * @var appearance_node::appearance_class
 *
 * @brief Class object for Appearance nodes.
 */

/**
 * @var sfnode appearance_node::material_
 *
 * @brief material exposedField.
 */

/**
 * @var sfnode appearance_node::texture_
 *
 * @brief texture exposedField.
 */

/**
 * @var sfnode appearance_node::textureTransform
 *
 * @brief textureTransform exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
appearance_node::appearance_node(const node_type & type,
                                 const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::appearance_node(type, scope)
{}

/**
 * @brief Destroy.
 */
appearance_node::~appearance_node() throw ()
{}

/**
 * @brief set_material eventIn handler.
 *
 * @param value     an sfnode value; should be a Material node.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void appearance_node::process_set_material(const field_value & value,
                                           double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->material_ = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("material_changed", this->material_, timestamp);
}

/**
 * @brief set_texture eventIn handler.
 *
 * @param value     an sfnode value; should be a Texture node.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void appearance_node::process_set_texture(const field_value & value,
                                     double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->texture_ = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("texture_changed", this->texture_, timestamp);
}

/**
 * @brief set_textureTransform eventIn handler.
 *
 * @param value     an sfnode value; should be a TextureTransform node.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void appearance_node::process_set_textureTransform(const field_value & value,
                                                   double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->textureTransform = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("textureTransform_changed",
                    this->textureTransform,
                    timestamp);
}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool appearance_node::modified() const
{
    return (this->node::modified()
          || (this->material_.value && this->material_.value->modified())
          || (this->texture_.value && this->texture_.value->modified())
          || (this->textureTransform.value
                && this->textureTransform.value->modified()));
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void appearance_node::render(openvrml::viewer & viewer,
                             const rendering_context context)
{
    openvrml::material_node * const material =
        this->material_.value
        ? this->material_.value->to_material()
        : 0;
    texture_node * const texture = this->texture_.value
                                 ? this->texture_.value->to_texture()
                                 : 0;

    if (material) {
        float trans = material->transparency();
        color diffuse = material->diffuse_color();
        size_t nTexComponents = texture ? texture->components() : 0;
        if (nTexComponents == 2 || nTexComponents == 4) { trans = 0.0; }
        if (nTexComponents >= 3) { diffuse = color(1.0, 1.0, 1.0); }

        viewer.enable_lighting(true);   // turn lighting on for this object
        viewer.set_material(material->ambient_intensity(),
                            diffuse,
                            material->emissive_color(),
                            material->shininess(),
                            material->specular_color(),
                            trans);

        material->modified(false);
    } else {
        viewer.set_color(color(1.0, 1.0, 1.0)); // default color
        viewer.enable_lighting(false);   // turn lighting off for this object
    }

    if (texture) {
        if (this->textureTransform.value) {
            this->textureTransform.value->render(viewer, context);
        } else {
            static const vec2f center(0.0, 0.0);
            static const float rotation = 0.0;
            static const vec2f scale(1.0, 1.0);
            static const vec2f translation(0.0, 0.0);
            viewer.set_texture_transform(center, rotation, scale, translation);
        }
        texture->render(viewer, context);
    }
    this->node::modified(false);
}

/**
 * @brief Get the material node.
 *
 * @returns an sfnode object containing the Material node associated with
 *          this Appearance.
 */
const node_ptr & appearance_node::material() const throw ()
{
    return this->material_.value;
}

/**
 * @brief Get the texture node.
 *
 * @return an sfnode object containing the texture node associated with
 *         this Appearance.
 */
const node_ptr & appearance_node::texture() const throw ()
{
    return this->texture_.value;
}

/**
 * @brief Get the texture transform node.
 *
 * @return an sfnode object containing the TextureTransform node
 *         associated with this Appearance.
 */
const node_ptr & appearance_node::texture_transform() const throw ()
{
    return this->textureTransform.value;
}


/**
 * @class audio_clip_class
 *
 * @brief Class object for AudioClip nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
audio_clip_class::audio_clip_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
audio_clip_class::~audio_clip_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating AudioClip nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by audio_clip_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
audio_clip_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef vrml97_node_type_impl<audio_clip_node> audio_clip_type_t;

    const node_type_ptr type(new audio_clip_type_t(*this, id));
    audio_clip_type_t & audioClipNodeType =
        static_cast<audio_clip_type_t &>(*type);
    typedef audio_clip_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &audio_clip_node::process_set_description,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<audio_clip_node, sfstring>
                    (&audio_clip_node::description)));
        } else if (*interface == supportedInterfaces[1]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &audio_clip_node::process_set_loop,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<audio_clip_node, sfbool>
                    (&audio_clip_node::loop)));
        } else if (*interface == supportedInterfaces[2]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &audio_clip_node::process_set_pitch,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<audio_clip_node, sffloat>
                    (&audio_clip_node::pitch)));
        } else if (*interface == supportedInterfaces[3]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &audio_clip_node::process_set_startTime,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<audio_clip_node, sftime>
                    (&audio_clip_node::startTime)));
        } else if (*interface == supportedInterfaces[4]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &audio_clip_node::process_set_stopTime,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<audio_clip_node, sftime>
                    (&audio_clip_node::stopTime)));
        } else if (*interface == supportedInterfaces[5]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &audio_clip_node::process_set_url,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<audio_clip_node, mfstring>
                    (&audio_clip_node::url)));
        } else if (*interface == supportedInterfaces[6]) {
            audioClipNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<audio_clip_node, sftime>
                    (&audio_clip_node::duration)));
        } else if (*interface == supportedInterfaces[7]) {
            audioClipNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<audio_clip_node, sfbool>
                    (&audio_clip_node::active)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class audio_clip_node
 *
 * @brief AudioClip node instances.
 *
 * @todo Implement sound support.
 */

/**
 * @var audio_clip_node::audio_clip_class
 *
 * @brief Class object for AudioClip nodes.
 */

/**
 * @var sfstring audio_clip_node::description
 *
 * @brief description exposedField.
 */

/**
 * @var sfbool audio_clip_node::loop
 *
 * @brief loop exposedField.
 */

/**
 * @var sffloat audio_clip_node::pitch
 *
 * @brief pitch exposedField.
 */

/**
 * @var sftime audio_clip_node::startTime
 *
 * @brief startTime exposedField.
 */

/**
 * @var sftime audio_clip_node::stopTime
 *
 * @brief stopTime exposedField.
 */

/**
 * @var mfstring audio_clip_node::url
 *
 * @brief url exposedField.
 */

/**
 * @var sftime audio_clip_node::duration
 *
 * @brief duration_changed eventOut.
 */

/**
 * @var sfbool audio_clip_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
audio_clip_node::audio_clip_node(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    pitch(1.0),
    active(false)
{}

/**
 * @brief Destroy.
 */
audio_clip_node::~audio_clip_node() throw ()
{}

/**
 * @brief Cast to an audio_clip_node node.
 *
 * @return a pointer to this node.
 */
audio_clip_node* audio_clip_node::to_audio_clip() const
{
    return (audio_clip_node*)this;
}

/**
 * @brief Called to update the AudioClip for the current time.
 *
 * @param time  the current time.
 *
 * @todo Implement me!
 */
void audio_clip_node::update(double time)
{}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void audio_clip_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_audio_clip(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void audio_clip_node::do_shutdown(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.remove_audio_clip(*this);
}

/**
 * @brief set_description eventIn handler.
 *
 * @param value     an sfstring.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void audio_clip_node::process_set_description(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->description = dynamic_cast<const sfstring &>(value);
    this->emit_event("description_changed", this->description, timestamp);
}

/**
 * @brief set_loop eventIn handler.
 *
 * @param value     an sfbool.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfbool.
 */
void audio_clip_node::process_set_loop(const field_value & value,
                                       double timestamp)
    throw (std::bad_cast)
{
    this->loop = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("loop_changed", this->loop, timestamp);
}

/**
 * @brief set_pitch eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 */
void audio_clip_node::process_set_pitch(const field_value & value,
                                        double timestamp)
    throw (std::bad_cast)
{
    this->pitch = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("pitch_changed", this->pitch, timestamp);
}

/**
 * @brief set_startTime eventIn handler.
 *
 * @param value     an sftime.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sftime.
 */
void audio_clip_node::process_set_startTime(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->startTime = dynamic_cast<const sftime &>(value);
    this->node::modified(true);
    this->emit_event("startTime_changed", this->startTime, timestamp);
}

/**
 * @brief set_stopTime eventIn handler.
 *
 * @param value     an sftime.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sftime.
 */
void audio_clip_node::process_set_stopTime(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast)
{
    this->stopTime = dynamic_cast<const sftime &>(value);
    this->node::modified(true);
    this->emit_event("stopTime_changed", this->stopTime, timestamp);
}

/**
 * @brief set_url eventIn handler.
 *
 * @param value     an mfstring.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void audio_clip_node::process_set_url(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->url = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("url_changed", this->url, timestamp);
}


/**
 * @class background_class
 *
 * @brief Class object for Background nodes.
 */

/**
 * @typedef background_class::bound_nodes_t
 *
 * @brief A bound Background node stack.
 */

/**
 * @var background_node * background_class::first
 *
 * @brief The first Background node in the initial scene graph.
 */

/**
 * @var background_class::bound_nodes_t background_class::bound_nodes
 *
 * @brief The bound Background node stack.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
background_class::background_class(openvrml::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
background_class::~background_class() throw ()
{}

/**
 * @brief Set the first Background node in the world.
 *
 * The first Background node in the world is used as the initial background.
 * This method is used by Background::do_initialize.
 *
 * @param background    a Background node.
 */
void background_class::set_first(background_node & background) throw ()
{
    this->first = &background;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by background_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool background_class::has_first() const throw ()
{
    return this->first;
}

/**
 * @brief Push a background_node on the top of the bound node stack.
 *
 * @param background    the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_class::bind(background_node & background,
                            const double timestamp)
    throw (std::bad_alloc)
{
    using std::find;

    //
    // If the node is already the active node, do nothing.
    //
    if (!this->bound_nodes.empty()
        && &background == this->bound_nodes.back()) {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const bound_nodes_t::iterator pos =
        find(this->bound_nodes.begin(), this->bound_nodes.end(), &background);
    if (pos != this->bound_nodes.end()) { this->bound_nodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->bound_nodes.empty()) {
        background_node & current =
            dynamic_cast<background_node &>(*this->bound_nodes.back());
        current.bound.value = false;
        current.emit_event("isBound", current.bound, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->bound_nodes.push_back(&background);
    background.bound.value = true;
    background.emit_event("isBound", background.bound, timestamp);
}

/**
 * @brief Remove a background_node from the bound node stack.
 *
 * @param background    the node to unbind.
 * @param timestamp     the current time.
 */
void background_class::unbind(background_node & background,
                              const double timestamp)
    throw ()
{
    using std::find;

    const bound_nodes_t::iterator pos =
        find(this->bound_nodes.begin(), this->bound_nodes.end(), &background);
    if (pos != this->bound_nodes.end()) {
        background.bound.value = false;
        background.emit_event("isBound", background.bound, timestamp);

        if (pos == this->bound_nodes.end() - 1
            && this->bound_nodes.size() > 1) {
            background_node & newActive = dynamic_cast<background_node &>
                                          (**(this->bound_nodes.end() - 2));
            newActive.bound.value = true;
            newActive.emit_event("isBound", newActive.bound, timestamp);
        }
        this->bound_nodes.erase(pos);
    }
}

/**
 * @brief node_class-specific initialization.
 *
 * @param initialViewpoint  the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void background_class::initialize(openvrml::viewpoint_node * initialViewpoint,
                                  const double timestamp)
    throw ()
{
    if (this->first) {
        this->first->process_event("set_bind", sfbool(true), timestamp);
    }
}

namespace {
    /**
     * @brief Load and scale textures as needed.
     */
    img * getTexture(const std::vector<std::string> & urls,
                     doc2 & baseDoc,
                     img * tex,
                     int thisIndex,
                     openvrml::viewer & viewer)
    {
        // Check whether the url has already been loaded
        if (!urls.empty()) {
            for (int index = thisIndex - 1; index >= 0; --index) {
                using std::string;
                const char * currentTex = tex[index].url();
                const string relPath = baseDoc.url_path();
                size_t currentLen = currentTex
                                  ? strlen(currentTex)
                                  : 0;
                string::size_type relPathLen = relPath.length();
                if (relPathLen >= currentLen) { relPathLen = 0; }

                if (currentTex) {
                    for (size_t i = 0; i < urls.size(); ++i) {
                        if (urls[i] == currentTex
                            || urls[i]  == (currentTex + relPathLen)) {
                            return &tex[index];
                        }
                    }
                }
            }

            // Have to load it
            if (!tex[thisIndex].try_urls(urls, &baseDoc)) {
                using std::cerr;
                using std::endl;
                cerr << "Error: couldn't read Background texture from URL "
                     << mfstring(urls.begin(), urls.end()) << endl;
            } else if (tex[thisIndex].pixels() && tex[thisIndex].nc()) {
                //
                // The texture needs to be scaled.
                //

                // Ensure the image dimensions are powers of two
                static const size_t size[] = {
                    2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
                };
                static const size_t sizes = sizeof size / sizeof (size_t);
                size_t w = tex[thisIndex].w();
                size_t h = tex[thisIndex].h();
                size_t i, j;
                for (i = 0; i < sizes; ++i) { if (w < size[i]) { break; } }
                for (j = 0; j < sizes; ++j) { if (h < size[j]) { break; } }

                if (i > 0 && j > 0) {
                    // Always scale images down in size and reuse the same
                    // pixel memory. This can cause some ugliness...
                    if (w != size[i - 1] || h != size[j - 1]) {
                        viewer.scale_texture(w, h, size[i - 1], size[j - 1],
                                             tex[thisIndex].nc(),
                                             tex[thisIndex].pixels());
                        tex[thisIndex].resize(size[i - 1], size[j - 1]);
                    }
                }
            }
        }

        return &tex[thisIndex];
    }
}

/**
 * @brief node_class-specific rendering.
 *
 * Render the active Background node.
 *
 * @param viewer    a Viewer.
 */
void background_class::render(openvrml::viewer & viewer) throw ()
{
    if (!this->bound_nodes.empty()) {
        assert(this->bound_nodes.back());
        background_node & background = *this->bound_nodes.back();

        // Background isn't selectable, so don't waste the time.
        if (viewer.mode() == viewer::pick_mode) { return; }

        if (background.viewerObject && background.modified()) {
            viewer.remove_object(background.viewerObject);
            background.viewerObject = 0;
        }

        if (background.viewerObject) {
            viewer.insert_reference(background.viewerObject);
        } else {
            if (background.modified() || background.texPtr[0] == 0) {
                doc2 baseDoc(background.scene()->url());
                background.texPtr[0] = getTexture(background.backUrl.value,
                                                  baseDoc,
                                                  background.tex,
                                                  0,
                                                  viewer);
                background.texPtr[1] = getTexture(background.bottomUrl.value,
                                                  baseDoc,
                                                  background.tex,
                                                  1,
                                                  viewer);
                background.texPtr[2] = getTexture(background.frontUrl.value,
                                                  baseDoc,
                                                  background.tex,
                                                  2,
                                                  viewer);
                background.texPtr[3] = getTexture(background.leftUrl.value,
                                                  baseDoc,
                                                  background.tex,
                                                  3,
                                                  viewer);
                background.texPtr[4] = getTexture(background.rightUrl.value,
                                                  baseDoc,
                                                  background.tex,
                                                  4,
                                                  viewer);
                background.texPtr[5] = getTexture(background.topUrl.value,
                                                  baseDoc,
                                                  background.tex,
                                                  5,
                                                  viewer);
            }

            size_t i, whc[18];    // Width, height, and nComponents for 6 tex
            unsigned char *pixels[6];
            int nPix = 0;

            for (i = 0; i < 6; ++i) {
                whc[3 * i + 0] = background.texPtr[i]->w();
                whc[3 * i + 1] = background.texPtr[i]->h();
                whc[3 * i + 2] = background.texPtr[i]->nc();
                pixels[i] = background.texPtr[i]->pixels();
                if (whc[3 * i + 0] > 0
                    && whc[3 * i + 1] > 0
                    && whc[3 * i + 2] > 0
                    && pixels[i]) {
                    ++nPix;
                }
            }

            background.viewerObject =
                    viewer.insert_background(background.groundAngle.value,
                                             background.groundColor.value,
                                             background.skyAngle.value,
                                             background.skyColor.value,
                                             whc,
                                             (nPix > 0) ? pixels : 0);

            background.modified(false);
        }
    } else {
        //
        // Default background.
        //
        using std::vector;
        static const vector<float> groundAngle;
        static const vector<color> groundColor;
        static const vector<float> skyAngle;
        static const vector<color> skyColor;
        viewer.insert_background(groundAngle, groundColor, skyAngle, skyColor);
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Background nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by background_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
background_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<background_node>(*this, id));
    vrml97_node_type_impl<background_node> & backgroundNodeType =
        static_cast<vrml97_node_type_impl<background_node> &>(*type);
    typedef vrml97_node_type_impl<background_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            backgroundNodeType.add_eventin(supportedInterfaces[0].field_type,
                                           supportedInterfaces[0].id,
                                           &background_node::process_set_bind);
        } else if (*interface == supportedInterfaces[1]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &background_node::process_set_groundAngle,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mffloat>
                    (&background_node::groundAngle)));
        } else if (*interface == supportedInterfaces[2]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &background_node::process_set_groundColor,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mfcolor>
                    (&background_node::groundColor)));
        } else if (*interface == supportedInterfaces[3]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &background_node::process_set_backUrl,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mfstring>
                    (&background_node::backUrl)));
        } else if (*interface == supportedInterfaces[4]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &background_node::process_set_bottomUrl,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mfstring>
                    (&background_node::bottomUrl)));
        } else if (*interface == supportedInterfaces[5]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &background_node::process_set_frontUrl,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mfstring>
                    (&background_node::frontUrl)));
        } else if (*interface == supportedInterfaces[6]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &background_node::process_set_leftUrl,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mfstring>
                    (&background_node::leftUrl)));
        } else if (*interface == supportedInterfaces[7]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &background_node::process_set_rightUrl,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mfstring>
                    (&background_node::rightUrl)));
        } else if (*interface == supportedInterfaces[8]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                &background_node::process_set_topUrl,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mfstring>
                    (&background_node::topUrl)));
        } else if (*interface == supportedInterfaces[9]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                &background_node::process_set_skyAngle,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mffloat>
                    (&background_node::skyAngle)));
        } else if (*interface == supportedInterfaces[10]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                &background_node::process_set_skyColor,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, mfcolor>
                    (&background_node::skyColor)));
        } else if (*interface == supportedInterfaces[11]) {
            backgroundNodeType.add_eventout(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<background_node, sfbool>
                    (&background_node::bound)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class background_node
 *
 * @brief Background node instances.
 */

/**
 * @var background_node::background_class
 *
 * @brief Class object for Background nodes.
 */

/**
 * @var mffloat background_node::groundAngle
 *
 * @brief groundAngle exposedField.
 */

/**
 * @var mfcolor background_node::groundColor
 *
 * @brief groundColor exposedField.
 */

/**
 * @var mfstring background_node::backUrl
 *
 * @brief backUrl exposedField.
 */

/**
 * @var mfstring background_node::bottomUrl
 *
 * @brief bottomUrl exposedField.
 */

/**
 * @var mfstring background_node::frontUrl
 *
 * @brief frontUrl exposedField.
 */

/**
 * @var mfstring background_node::leftUrl
 *
 * @brief leftUrl exposedField.
 */

/**
 * @var mfstring background_node::rightUrl
 *
 * @brief rightUrl exposedField.
 */

/**
 * @var mfstring background_node::topUrl
 *
 * @brief topUrl exposedField.
 */

/**
 * @var mffloat background_node::skyAngle
 *
 * @brief skyAngle exposedField.
 */

/**
 * @var mfcolor background_node::skyColor
 *
 * @brief skyColor exposedField.
 */

/**
 * @var sfbool background_node::bound
 *
 * @brief isBound eventOut.
 */

/**
 * @var img * background_node::texPtr[6]
 *
 * @brief Pointers to the @link openvrml::img imgs@endlink in @a tex.
 *
 * The pointers are initialized to zero, and become non-null once the textures
 * are loaded and, if necessary, resized.
 */

/**
 * @var img background_node::tex[6]
 *
 * @brief Texture data.
 */

/**
 * @var viewer::object_t background_node::viewerObject
 *
 * @brief Handle for the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
background_node::background_node(const node_type & type,
                                 const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    bound(false),
    viewerObject(0)
{
    std::fill(this->texPtr, this->texPtr + 6, static_cast<img *>(0));
}

/**
 * @brief Destroy.
 */
background_node::~background_node() throw ()
{
    // remove d_viewerObject...
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void background_node::do_initialize(const double timestamp) throw ()
{
    assert(dynamic_cast<background_class *>(&this->type.node_class));
    background_class & nodeClass =
        static_cast<background_class &>(this->type.node_class);
    if (!nodeClass.has_first()) { nodeClass.set_first(*this); }
}

/**
 * @brief Shut down.
 *
 * Calls background_class::unbind to unbind the node if it is bound.
 *
 * @param timestamp the current time.
 */
void background_node::do_shutdown(const double timestamp) throw ()
{
    background_class & nodeClass =
        static_cast<background_class &>(this->type.node_class);
    nodeClass.unbind(*this, timestamp);
}

/**
 * @brief set_bind eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfbool.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_bind(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    const sfbool & bind = dynamic_cast<const sfbool &>(value);
    assert(dynamic_cast<background_class *>(&this->type.node_class));
    background_class & nodeClass =
        static_cast<background_class &>(this->type.node_class);
    if (bind.value) {
        nodeClass.bind(*this, timestamp);
    } else {
        nodeClass.unbind(*this, timestamp);
    }
}

/**
 * @brief set_groundAngle eventIn handler.
 *
 * @param value     an mffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_groundAngle(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->groundAngle = dynamic_cast<const mffloat &>(value);
    this->node::modified(true);
    this->emit_event("groundAngle_changed", this->groundAngle, timestamp);
}

/**
 * @brief set_groundColor eventIn handler.
 *
 * @param value     an mfcolor.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfcolor.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_groundColor(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->groundColor = dynamic_cast<const mfcolor &>(value);
    this->node::modified(true);
    this->emit_event("groundColor_changed", this->groundColor, timestamp);
}

/**
 * @brief set_backUrl eventIn handler.
 *
 * @param value     an mfstring.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_backUrl(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->backUrl = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("backUrl_changed", this->backUrl, timestamp);
}

/**
 * @brief set_bottomUrl eventIn handler.
 *
 * @param value     an mfstring.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_bottomUrl(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->bottomUrl = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("bottomUrl_changed", this->bottomUrl, timestamp);
}

/**
 * @brief set_frontUrl eventIn handler.
 *
 * @param value     an mfstring.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_frontUrl(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->frontUrl = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("frontUrl_changed", this->backUrl, timestamp);
}

/**
 * @brief set_leftUrl eventIn handler.
 *
 * @param value     an mfstring.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_leftUrl(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->leftUrl = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("leftUrl_changed", this->leftUrl, timestamp);
}

/**
 * @brief set_rightUrl eventIn handler.
 *
 * @param value     an mfstring.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_rightUrl(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->rightUrl = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("rightUrl_changed", this->rightUrl, timestamp);
}

/**
 * @brief set_topUrl eventIn handler.
 *
 * @param value     an mfstring.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_topUrl(const field_value & value,
                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->topUrl = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("topUrl_changed", this->topUrl, timestamp);
}

/**
 * @brief set_skyAngle eventIn handler.
 *
 * @param value     an mffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_skyAngle(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->skyAngle = dynamic_cast<const mffloat &>(value);
    this->node::modified(true);
    this->emit_event("skyAngle_changed", this->skyAngle, timestamp);
}

/**
 * @brief set_skyColor eventIn handler.
 *
 * @param value     an mfcolor.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfcolor.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void background_node::process_set_skyColor(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->skyColor = dynamic_cast<const mfcolor &>(value);
    this->node::modified(true);
    this->emit_event("skyColor_changed", this->skyColor, timestamp);
}


/**
 * @class billboard_class
 *
 * @brief Class object for Billboard nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
billboard_class::billboard_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
billboard_class::~billboard_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Billboard nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by billboard_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
billboard_class::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<billboard_node>(*this, id));
    vrml97_node_type_impl<billboard_node> & billboardNodeType =
        static_cast<vrml97_node_type_impl<billboard_node> &>(*type);
    typedef vrml97_node_type_impl<billboard_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            billboardNodeType
                .add_eventin(supportedInterfaces[0].field_type,
                             supportedInterfaces[0].id,
                             &billboard_node::process_addChildren);
        } else if (*interface == supportedInterfaces[1]) {
            billboardNodeType
                .add_eventin(supportedInterfaces[1].field_type,
                             supportedInterfaces[1].id,
                             &billboard_node::process_removeChildren);
        } else if (*interface == supportedInterfaces[2]) {
            billboardNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &billboard_node::process_set_axisOfRotation,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<billboard_node, sfvec3f>
                    (&billboard_node::axisOfRotation)));
        } else if (*interface == supportedInterfaces[3]) {
            billboardNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &billboard_node::process_set_children,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<billboard_node, mfnode>
                    (&billboard_node::children_)));
        } else if (*interface == supportedInterfaces[4]) {
            billboardNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<billboard_node, sfvec3f>
                    (&billboard_node::bboxCenter)));
        } else if (*interface == supportedInterfaces[5]) {
            billboardNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<billboard_node, sfvec3f>
                    (&billboard_node::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class billboard_node
 *
 * @brief Billboard node instances.
 */

/**
 * @var billboard_node::billboard_class
 *
 * @brief Class object for Billboard nodes.
 */

/**
 * @var sfvec3f billboard_node::axisOfRotation
 *
 * @brief axisOfRotation exposedField.
 */

/**
 * @var viewer::object_t billboard_node::xformObject
 *
 * @brief Handle for the renderer.
 */

/**
 * @brief Get the bounding box transformation matrix.
 *
 * @param node      a pointer to a billboard_node.
 * @param modelview input ModelView transformation matrix.
 *
 * @return the bounding box transformation matrix.
 */
const mat4f billboard_node::billboard_to_matrix(const billboard_node & node,
                                                const mat4f & modelview)
{
    const mat4f inverse_modelview = modelview.inverse();

    mat4f result;

    // Viewer position in local coordinate system
    const vec3f position = vec3f(inverse_modelview[3][0],
                                 inverse_modelview[3][1],
                                 inverse_modelview[3][2]).normalize();

    // Viewer-alignment
    if ((node.axisOfRotation.value[0] == 0)
            && (node.axisOfRotation.value[1] == 0)
            && (node.axisOfRotation.value[2] == 0)) {
        //
        // Viewer's up vector
        //
        const vec3f up = vec3f(inverse_modelview[1][0],
                               inverse_modelview[1][1],
                               inverse_modelview[1][2]).normalize();

        // get x-vector from the cross product of Viewer's
        // up vector and billboard-to-viewer vector.
        const vec3f X = up * position;

        result[0][0] = X[0];
        result[0][1] = X[1];
        result[0][2] = X[2];
        result[0][3] = 0.0;

        result[1][0] = up[0];
        result[1][1] = up[1];
        result[1][2] = up[2];
        result[1][3] = 0.0;

        result[2][0] = position[0];
        result[2][1] = position[1];
        result[2][2] = position[2];
        result[2][3] = 0.0;

        result[3][0] = 0.0;
        result[3][1] = 0.0;
        result[3][2] = 0.0;
        result[3][3] = 1.0;
    } else { // use axis of rotation
        // axis of rotation will be the y-axis vector
        const vec3f Y(node.axisOfRotation.value);

        // Plane defined by the axisOfRotation and billboard-to-viewer vector
        const vec3f X = (Y * position).normalize();

        // Get Z axis vector from cross product of X and Y
        const vec3f Z = X * Y;

        // Transform Z axis vector of current coordinate system to new
        // coordinate system.
        float nz[3] = { X[2], Y[2], Z[2] };

        // calculate the angle by which the Z axis vector of current coordinate
        // system has to be rotated around the Y axis to new coordinate system.
        float angle = float(acos(nz[2]));
        if(nz[0] > 0) { angle = -angle; }
        result = mat4f::rotation(rotation(Y, angle));
    }
    return result;
}

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
billboard_node::billboard_node(const node_type & type,
                               const scope_ptr & scope):
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    group_node(type, scope),
    axisOfRotation(vec3f(0.0, 1.0, 0.0)),
    xformObject(0)
{}

/**
 * @brief Destroy.
 */
billboard_node::~billboard_node() throw ()
{
    // delete xformObject...
}

/**
 * @brief Render the node.
 *
 * @param viewer    a viewer.
 * @param context   the rendering context.
 */
void billboard_node::render(openvrml::viewer & viewer,
                            rendering_context context)
{
    mat4f new_LM = context.matrix();
    mat4f LM = billboard_to_matrix(*this, new_LM);
    new_LM = LM * new_LM;
    context.matrix(new_LM);

    if (this->xformObject && this->modified()) {
        viewer.remove_object(this->xformObject);
        this->xformObject = 0;
    }

    if (this->xformObject) {
        viewer.insert_reference(this->xformObject);
    } else if (this->children_.value.size() > 0) {
        this->xformObject = viewer.begin_object(this->id().c_str());

        viewer.transform(LM);

        // Render children
        this->group_node::render(viewer, context);

        viewer.end_object();
    }

    this->node::modified(false);
}

/**
 * @brief set_axisOfRotation eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void billboard_node::process_set_axisOfRotation(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->axisOfRotation = dynamic_cast<const sfvec3f &>(value);
    this->emit_event("axisOfRotation_changed",
                     this->axisOfRotation,
                     timestamp);
}


/**
 * @class box_class
 *
 * @brief Class object for Box nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
box_class::box_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
box_class::~box_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Box nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by box_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
box_class::create_type(const std::string & id,
                       const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "size");
    const node_type_ptr type(new vrml97_node_type_impl<box_node>(*this, id));
    vrml97_node_type_impl<box_node> & boxNodeType =
        static_cast<vrml97_node_type_impl<box_node> &>(*type);
    typedef vrml97_node_type_impl<box_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterface) {
            boxNodeType.add_field(
                supportedInterface.field_type,
                supportedInterface.id,
                node_field_ptr_ptr(new node_field_ptr_impl<box_node, sfvec3f>(
                                       &box_node::size)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class box_node
 *
 * @brief Box node instances.
 */

/**
 * @var box_node::box_class
 *
 * @brief Class object for Box nodes.
 */

/**
 * @var sfvec3f box_node::size
 *
 * @brief size field.
 */

/**
 * @var bounding_sphere box_node::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
box_node::box_node(const node_type & type,
                   const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope),
    size(vec3f(2.0, 2.0, 2.0))
{
    this->bounding_volume_dirty(true); // lazy calc of bvolume
}

/**
 * @brief Destroy.
 */
box_node::~box_node() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @return display object identifier.
 */
viewer::object_t box_node::insert_geometry(openvrml::viewer & viewer,
                                           const rendering_context context)
{
    return viewer.insert_box(this->size.value);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & box_node::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const vec3f corner = this->size.value / 2.0f;
        const float r = corner.length();
        const_cast<box_node *>(this)->bsphere.radius(r);
        const_cast<box_node *>(this)->bounding_volume_dirty(false); // logical const
    }
    return this->bsphere;
}


/**
 * @class collision_class
 *
 * @brief Class object for Collision nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
collision_class::collision_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
collision_class::~collision_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Collision nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by collision_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
collision_class::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<collision_node>(*this, id));
    vrml97_node_type_impl<collision_node> & collisionNodeType =
        static_cast<vrml97_node_type_impl<collision_node> &>(*type);
    typedef vrml97_node_type_impl<collision_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            collisionNodeType
                .add_eventin(supportedInterfaces[0].field_type,
                             supportedInterfaces[0].id,
                             &collision_node::process_addChildren);
        } else if (*interface == supportedInterfaces[1]) {
            collisionNodeType
                .add_eventin(supportedInterfaces[1].field_type,
                             supportedInterfaces[1].id,
                             &collision_node::process_removeChildren);
        } else if (*interface == supportedInterfaces[2]) {
            collisionNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &collision_node::process_set_children,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<collision_node, mfnode>
                    (&collision_node::children_)));
        } else if (*interface == supportedInterfaces[3]) {
            collisionNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &collision_node::process_set_collide,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<collision_node, sfbool>
                    (&collision_node::collide)));
        } else if (*interface == supportedInterfaces[4]) {
            collisionNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<collision_node, sfvec3f>
                    (&collision_node::bboxCenter)));
        } else if (*interface == supportedInterfaces[5]) {
            collisionNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<collision_node, sfvec3f>
                    (&collision_node::bboxSize)));
        } else if (*interface == supportedInterfaces[6]) {
            collisionNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<collision_node, sfnode>
                    (&collision_node::proxy)));
        } else if (*interface == supportedInterfaces[7]) {
            collisionNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<collision_node, sftime>
                    (&collision_node::collideTime)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class collision_node
 *
 * @brief Collision node instances.
 */

/**
 * @var collision_node::collision_class
 *
 * @brief Class object for Collision nodes.
 */

/**
 * @var sfbool collision_node::collide
 *
 * @brief collide exposedField.
 */

/**
 * @var sfnode collision_node::proxy
 *
 * @brief proxy field.
 */

/**
 * @var sftime collision_node::collideTime
 *
 * @brief collideTime eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
collision_node::collision_node(const node_type & type,
                               const scope_ptr & scope):
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    group_node(type, scope),
    collide(true)
{}

/**
 * @brief Destroy.
 */
collision_node::~collision_node() throw () {}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool collision_node::modified() const {
  return ((this->proxy.value && this->proxy.value->modified())
          || this->group_node::modified());
}

/**
 * @brief set_collide eventIn handler.
 *
 * @param value     an sfbool.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void collision_node::process_set_collide(const field_value & value,
                                         const double timestamp)
    throw (std::bad_cast)
{
    this->collide = dynamic_cast<const sfbool &>(value);
    this->emit_event("collide_changed", this->collide, timestamp);
}


/**
 * @class color_class
 *
 * @brief Class object for Color nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
color_class::color_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
color_class::~color_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Color nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by color_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
color_class::create_type(const std::string & id,
                         const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "color");
    const node_type_ptr type(new vrml97_node_type_impl<color_node>(*this, id));
    vrml97_node_type_impl<color_node> & colorNodeType =
        static_cast<vrml97_node_type_impl<color_node> &>(*type);
    typedef vrml97_node_type_impl<color_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterface) {
            colorNodeType.add_exposedfield(
                supportedInterface.field_type,
                supportedInterface.id,
                &color_node::process_set_color,
                node_field_ptr_ptr(new node_field_ptr_impl<color_node, mfcolor>
                                    (&color_node::color_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class color_node
 *
 * @brief Color node instances.
 */

/**
 * @var color_node::color_class
 *
 * @brief Class object for Color nodes.
 */

/**
 * @var mfcolor color_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
color_node::color_node(const node_type & type,
                       const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::color_node(type, scope)
{}

/**
 * @brief Destroy.
 */
color_node::~color_node() throw ()
{}

/**
 * @brief Get the color array.
 *
 * @return the color array associated with the node.
 */
const std::vector<color> & color_node::color() const throw ()
{
    return this->color_.value;
}

/**
 * @brief set_color eventIn handler.
 *
 * @param value     an mfcolor.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfcolor.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void color_node::process_set_color(const field_value & value,
                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->color_ = dynamic_cast<const mfcolor &>(value);
    this->node::modified(true);
    this->emit_event("color_changed", this->color_, timestamp);
}


/**
 * @class color_interpolator_class
 *
 * @brief Class object for ColorInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
color_interpolator_class::color_interpolator_class(
    openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
color_interpolator_class::~color_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating ColorInterpolator
 *      nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by color_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
color_interpolator_class::create_type(const std::string & id,
                                      const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<color_interpolator_node>(*this, id));
    vrml97_node_type_impl<color_interpolator_node> &
        colorInterpolatorNodeType =
        static_cast<vrml97_node_type_impl<color_interpolator_node> &>(*type);
    typedef vrml97_node_type_impl<color_interpolator_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            colorInterpolatorNodeType
                .add_eventin(supportedInterfaces[0].field_type,
                             supportedInterfaces[0].id,
                             &color_interpolator_node::process_set_fraction);
        } else if (*interface == supportedInterfaces[1]) {
            colorInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &color_interpolator_node::process_set_key,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<color_interpolator_node, mffloat>
                    (&color_interpolator_node::key)));
        } else if (*interface == supportedInterfaces[2]) {
            colorInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &color_interpolator_node::process_set_keyValue,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<color_interpolator_node, mfcolor>
                    (&color_interpolator_node::keyValue)));
        } else if (*interface == supportedInterfaces[3]) {
            colorInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<color_interpolator_node, sfcolor>
                    (&color_interpolator_node::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class color_interpolator_node
 *
 * @brief ColorInterpolator node instances.
 */

/**
 * @var color_interpolator_node::color_interpolator_class
 *
 * @brief Class object for ColorInterpolator nodes.
 */

/**
 * @var mffloat color_interpolator_node::key
 *
 * @brief key exposedField.
 */

/**
 * @var mfcolor color_interpolator_node::keyValue
 *
 * @brief keyValue exposedField.
 */

/**
 * @var sfcolor color_interpolator_node::value
 *
 * @brief value_changed eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
color_interpolator_node::color_interpolator_node(const node_type & type,
                                                 const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
color_interpolator_node::~color_interpolator_node() throw () {}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void color_interpolator_node::process_set_fraction(const field_value & value,
                                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    using std::vector;

    float f = dynamic_cast<const sffloat &>(value).value;

    vector<float>::size_type n = this->key.value.size() - 1;
    if (f < this->key.value[0]) {
        this->value.value = this->keyValue.value[0];
    } else if (f > this->key.value[n]) {
        this->value.value = this->keyValue.value[n];
    } else {
        // convert to HSV for the interpolation...
        for (vector<float>::size_type i = 0; i < n; ++i) {
            if (this->key.value[i] <= f
                    && f <= this->key.value[i + 1]) {
                const color & rgb1 = this->keyValue.value[i];
                const color & rgb2 = this->keyValue.value[i + 1];

                f = (f - this->key.value[i])
                    / (this->key.value[i + 1] - this->key.value[i]);
                float hsv1[3], hsv2[3];
                rgb1.hsv(hsv1);
                rgb2.hsv(hsv2);

                // Interpolate angles via the shortest direction
                if (fabs(hsv2[0] - hsv1[0]) > 180.0) {
                    if (hsv2[0] > hsv1[0]) {
                        hsv1[0] += 360.0;
                    } else {
                        hsv2[0] += 360.0;
                    }
                }
                float h = hsv1[0] + f * (hsv2[0] - hsv1[0]);
                float s = hsv1[1] + f * (hsv2[1] - hsv1[1]);
                float v = hsv1[2] + f * (hsv2[2] - hsv1[2]);
                if (h >= 360.0) {
                    h -= 360.0;
                } else if (h < 0.0) {
                    h += 360.0;
                }
                this->value.value.hsv(h, s, v);
                break;
            }
        }
    }

    // Send the new value
    this->emit_event("value_changed", this->value, timestamp);
}

/**
 * @brief set_key eventIn handler.
 *
 * @param value     an mffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void color_interpolator_node::process_set_key(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->key = dynamic_cast<const mffloat &>(value);
    this->emit_event("key_changed", this->key, timestamp);
}

/**
 * @brief set_keyValue eventIn handler.
 *
 * @param value     an mfcolor.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfcolor.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void color_interpolator_node::process_set_keyValue(const field_value & value,
                                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mfcolor &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class cone_class
 *
 * @brief Class object for Cone nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
cone_class::cone_class(openvrml::browser & browser): node_class(browser) {}

/**
 * @brief Destroy.
 */
cone_class::~cone_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Cone nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by cone_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
cone_class::create_type(const std::string & id,
                        const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<cone_node>(*this, id));
    vrml97_node_type_impl<cone_node> & coneNodeType =
        static_cast<vrml97_node_type_impl<cone_node> &>(*type);
    typedef vrml97_node_type_impl<cone_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            coneNodeType.add_field(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_field_ptr_ptr(new node_field_ptr_impl<cone_node, sffloat>
                                   (&cone_node::bottomRadius)));
        } else if (*interface == supportedInterfaces[1]) {
            coneNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(new node_field_ptr_impl<cone_node, sffloat>
                                   (&cone_node::height)));
        } else if (*interface == supportedInterfaces[2]) {
            coneNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_field_ptr_ptr(new node_field_ptr_impl<cone_node, sfbool>
                                   (&cone_node::side)));
        } else if (*interface == supportedInterfaces[3]) {
            coneNodeType.add_field(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<cone_node, sfbool>
                                   (&cone_node::bottom)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class cone_node
 *
 * @brief Cone node instances.
 */

/**
 * @var cone_node::cone_class
 *
 * @brief Class object for Cone nodes.
 */

/**
 * @var sfbool cone_node::bottom
 *
 * @brief bottom field.
 */

/**
 * @var sffloat cone_node::bottomRadius
 *
 * @brief bottomRadius field.
 */

/**
 * @var sffloat cone_node::height
 *
 * @brief height field.
 */

/**
 * @var sfbool cone_node::side
 *
 * @brief side field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
cone_node::cone_node(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope),
    bottom(true),
    bottomRadius(1.0),
    height(2.0),
    side(true)
{}

/**
 * @brief Destroy.
 */
cone_node::~cone_node() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t cone_node::insert_geometry(openvrml::viewer & viewer,
                                            const rendering_context context)
{
    return viewer.insert_cone(this->height.value,
                              this->bottomRadius.value,
                              this->bottom.value,
                              this->side.value);
}


/**
 * @class coordinate_class
 *
 * @brief Class object for Material nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
coordinate_class::coordinate_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
coordinate_class::~coordinate_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Coordinate nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by coordinate_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
coordinate_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "point");
    const node_type_ptr
        type(new vrml97_node_type_impl<coordinate_node>(*this, id));
    vrml97_node_type_impl<coordinate_node> & coordinateNodeType =
        static_cast<vrml97_node_type_impl<coordinate_node> &>(*type);
    typedef vrml97_node_type_impl<coordinate_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterface) {
            coordinateNodeType.add_exposedfield(
                supportedInterface.field_type,
                supportedInterface.id,
                &coordinate_node::process_set_point,
                node_field_ptr_ptr(
                    new node_field_ptr_impl<coordinate_node, mfvec3f>
                    (&coordinate_node::point_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class coordinate_node
 *
 * @brief Coordinate node instances.
 */

/**
 * @var coordinate_node::coordinate_class
 *
 * @brief Class object for Coordinate nodes.
 */

/**
 * @var mfvec3f coordinate_node::point_
 *
 * @brief point exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
coordinate_node::coordinate_node(const node_type & type,
                                 const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::coordinate_node(type, scope)
{}

/**
 * @brief Destroy.
 */
coordinate_node::~coordinate_node() throw ()
{}

/**
 * @brief set_point eventIn handler.
 *
 * @param value     an array of vectors representing points.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec3f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void coordinate_node::process_set_point(const field_value & value,
                                        const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->point_ = dynamic_cast<const mfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("point_changed", this->point_, timestamp);
}

/**
 * @brief Get the points encapsulated by this node.
 *
 * @return the array of points for this node.
 */
const std::vector<vec3f> & coordinate_node::point() const throw ()
{
    return this->point_.value;
}


/**
 * @class coordinate_interpolator_class
 *
 * @brief Class object for CoordinateInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
coordinate_interpolator_class::coordinate_interpolator_class(
    openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
coordinate_interpolator_class::~coordinate_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *      CoordinateInterpolator nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by coordinate_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
coordinate_interpolator_class::create_type(
    const std::string & id,
    const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id, "key"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::mfvec3f_id,
                       "value_changed")
    };

    typedef vrml97_node_type_impl<coordinate_interpolator_node>
        coordinate_interpolator_type_t;

    const node_type_ptr type(new coordinate_interpolator_type_t(*this, id));
    coordinate_interpolator_type_t & coordinateInterpolatorNodeType =
        static_cast<coordinate_interpolator_type_t &>(*type);
    typedef coordinate_interpolator_type_t::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            coordinateInterpolatorNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &coordinate_interpolator_node::process_set_fraction);
        } else if (*interface == supportedInterfaces[1]) {
            coordinateInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &coordinate_interpolator_node::process_set_key,
                node_field_ptr_ptr(
                    new coordinate_interpolator_type_t::mffloat_ptr(
                        &coordinate_interpolator_node::key)));
        } else if (*interface == supportedInterfaces[2]) {
            coordinateInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &coordinate_interpolator_node::process_set_keyValue,
                node_field_ptr_ptr(
                    new coordinate_interpolator_type_t::mfvec3f_ptr(
                        &coordinate_interpolator_node::keyValue)));
        } else if (*interface == supportedInterfaces[3]) {
            coordinateInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(
                    new coordinate_interpolator_type_t::mfvec3f_ptr(
                        &coordinate_interpolator_node::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class coordinate_interpolator_node
 *
 * @brief CoordinateInterpolator node instances.
 */

/**
 * @var coordinate_interpolator_node::coordinate_interpolator_class
 *
 * @brief Class object for CoordinateInterpolator nodes.
 */

/**
 * @var mffloat coordinate_interpolator_node::key
 *
 * @brief key exposedField.
 */

/**
 * @var mfvec3f coordinate_interpolator_node::keyValue
 *
 * @brief keyValue exposedField.
 */

/**
 * @var mfvec3f coordinate_interpolator_node::value
 *
 * @brief value_changed eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
coordinate_interpolator_node::coordinate_interpolator_node(
    const node_type & type,
    const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
coordinate_interpolator_node::~coordinate_interpolator_node() throw ()
{}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
coordinate_interpolator_node::process_set_fraction(const field_value & value,
                                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    float f = dynamic_cast<const sffloat &>(value).value;

    size_t nCoords = this->keyValue.value.size() / this->key.value.size();
    size_t n = this->key.value.size() - 1;

    if (f < this->key.value[0]) {
        this->value.value.assign(this->keyValue.value.begin(),
                                 this->keyValue.value.begin() + nCoords);
    } else if (f > this->key.value[n]) {
        this->value.value
                .assign(this->keyValue.value.begin() + n * nCoords,
                        this->keyValue.value.begin() + (n + 1) * nCoords);
    } else {
        // Reserve enough space for the new value
        this->value.value.resize(nCoords);

        for (size_t i = 0; i < n; ++i) {
            if (this->key.value[i] <= f
                    && f <= this->key.value[i + 1]) {
                std::vector<vec3f>::const_iterator v1 =
                        this->keyValue.value.begin() + i * nCoords;
                std::vector<vec3f>::const_iterator v2 =
                        this->keyValue.value.begin() + (i + 1) * nCoords;

                f = (f - this->key.value[i])
                    / (this->key.value[i + 1] - this->key.value[i]);

                for (size_t j = 0; j < nCoords; ++j) {
                    const vec3f vec(v1->x() + f * (v2->x() - v1->x()),
                                    v1->y() + f * (v2->y() - v1->y()),
                                    v1->z() + f * (v2->z() - v1->z()));
                    this->value.value[j] = vec;
                    ++v1;
                    ++v2;
                }
                break;
            }
        }
    }

    // Send the new value
    this->emit_event("value_changed", this->value, timestamp);
}

/**
 * @brief set_key eventIn handler.
 *
 * @param value     an mffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void coordinate_interpolator_node::process_set_key(const field_value & value,
                                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->key = dynamic_cast<const mffloat &>(value);
    this->emit_event("key_changed", this->key, timestamp);
}

/**
 * @brief set_keyValue field mutator.
 *
 * @param value     an mfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec3f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
coordinate_interpolator_node::process_set_keyValue(const field_value & value,
                                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mfvec3f &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class cylinder_class
 *
 * @brief Class object for Cylinder nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
cylinder_class::cylinder_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
cylinder_class::~cylinder_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Cylinder nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by cylinder_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
cylinder_class::create_type(const std::string & id,
                            const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef vrml97_node_type_impl<cylinder_node> cylinder_type_t;

    const node_type_ptr type(new cylinder_type_t(*this, id));
    cylinder_type_t & cylinderNodeType = static_cast<cylinder_type_t &>(*type);
    typedef cylinder_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            cylinderNodeType.add_field(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_field_ptr_ptr(
                    new cylinder_type_t::sfbool_ptr(&cylinder_node::bottom)));
        } else if (*interface == supportedInterfaces[1]) {
            cylinderNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(
                    new cylinder_type_t::sffloat_ptr(&cylinder_node::height)));
        } else if (*interface == supportedInterfaces[2]) {
            cylinderNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_field_ptr_ptr(
                    new cylinder_type_t::sffloat_ptr(&cylinder_node::radius)));
        } else if (*interface == supportedInterfaces[3]) {
            cylinderNodeType.add_field(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(
                    new cylinder_type_t::sfbool_ptr(&cylinder_node::side)));
        } else if (*interface == supportedInterfaces[4]) {
            cylinderNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(
                    new cylinder_type_t::sfbool_ptr(&cylinder_node::top)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class cylinder_node
 *
 * @brief Cylinder node instances.
 */

/**
 * @var cylinder_node::cylinder_class
 *
 * @brief Class object for Cylinder nodes.
 */

/**
 * @var sfbool cylinder_node::bottom
 *
 * @brief bottom field.
 */

/**
 * @var sffloat cylinder_node::height
 *
 * @brief height field.
 */

/**
 * @var sffloat cylinder_node::radius
 *
 * @brief radius field.
 */

/**
 * @var sfbool cylinder_node::side
 *
 * @brief side field.
 */

/**
 * @var sfbool cylinder_node::top
 *
 * @brief top field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
cylinder_node::cylinder_node(const node_type & type,
                             const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope),
    bottom(true),
    height(2.0),
    radius(1.0),
    side(true),
    top(true)
{}

/**
 * @brief Destroy.
 */
cylinder_node::~cylinder_node() throw ()
{
    // need access to viewer to remove d_viewerObject...
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a viewer.
 * @param context   the rendering context.
 */
viewer::object_t
cylinder_node::insert_geometry(openvrml::viewer & viewer,
                               const rendering_context context)
{
    return viewer.insert_cylinder(this->height.value,
                                  this->radius.value,
                                  this->bottom.value,
                                  this->side.value,
                                  this->top.value);
}


/**
 * @class cylinder_sensor_class
 *
 * @brief Class object for CylinderSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
cylinder_sensor_class::cylinder_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
cylinder_sensor_class::~cylinder_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating CylinderSensor
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by cylinder_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
cylinder_sensor_class::create_type(const std::string & id,
                                   const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef vrml97_node_type_impl<cylinder_sensor_node> cylinder_sensor_type_t;

    const node_type_ptr type(new cylinder_sensor_type_t(*this, id));
    cylinder_sensor_type_t & cylinderSensorNodeType =
            static_cast<cylinder_sensor_type_t &>(*type);
    typedef cylinder_sensor_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &cylinder_sensor_node::process_set_autoOffset,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sfbool_ptr(
                                       &cylinder_sensor_node::autoOffset)));
        } else if (*interface == supportedInterfaces[1]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &cylinder_sensor_node::process_set_diskAngle,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sffloat_ptr(
                                       &cylinder_sensor_node::diskAngle)));
        } else if (*interface == supportedInterfaces[2]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &cylinder_sensor_node::process_set_enabled,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sfbool_ptr(
                                       &cylinder_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[3]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &cylinder_sensor_node::process_set_maxAngle,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sffloat_ptr(
                                       &cylinder_sensor_node::maxAngle)));
        } else if (*interface == supportedInterfaces[4]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &cylinder_sensor_node::process_set_minAngle,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sffloat_ptr(
                                       &cylinder_sensor_node::minAngle)));
        } else if (*interface == supportedInterfaces[5]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &cylinder_sensor_node::process_set_offset,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sffloat_ptr(
                                       &cylinder_sensor_node::offset)));
        } else if (*interface == supportedInterfaces[6]) {
            cylinderSensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sfbool_ptr(
                                       &cylinder_sensor_node::active)));
        } else if (*interface == supportedInterfaces[7]) {
            cylinderSensorNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sfrotation_ptr(
                                       &cylinder_sensor_node::rotation)));
        } else if (*interface == supportedInterfaces[8]) {
            cylinderSensorNodeType.add_eventout(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_field_ptr_ptr(new cylinder_sensor_type_t::sfvec3f_ptr(
                                       &cylinder_sensor_node::trackPoint)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class cylinder_sensor_node
 *
 * @brief CylinderSensor node instances.
 */

/**
 * @var cylinder_sensor_node::cylinder_sensor_class
 *
 * @brief Class object for CylinderSensor nodes.
 */

/**
 * @var sfbool cylinder_sensor_node::autoOffset
 *
 * @brief autoOffset exposedField.
 */

/**
 * @var sffloat cylinder_sensor_node::diskAngle
 *
 * @brief diskAngle exposedField.
 */

/**
 * @var sfbool cylinder_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var sffloat cylinder_sensor_node::maxAngle
 *
 * @brief maxAngle exposedField.
 */

/**
 * @var sffloat cylinder_sensor_node::minAngle
 *
 * @brief minAngle exposedField.
 */

/**
 * @var sffloat cylinder_sensor_node::offset
 *
 * @brief offset exposedField.
 */

/**
 * @var sfbool cylinder_sensor_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfrotation cylinder_sensor_node::rotation
 *
 * @brief rotation_changed eventOut.
 */

/**
 * @var sfvec3f cylinder_sensor_node::trackPoint
 *
 * @brief trackPoint_changed eventOut.
 */

/**
 * @var float cylinder_sensor_node::rotation_val
 *
 * @brief The rotation value.
 */

/**
 * @var vec3f cylinder_sensor_node::activationPoint
 *
 * @brief The activation point.
 */

/**
 * @var bool cylinder_sensor_node::disk
 */

/**
 * @var mat4f cylinder_sensor_node::activationMatrix
 *
 * @brief Activation matrix.
 */

/**
 * @var mat4f cylinder_sensor_node::modelview
 *
 * @brief Modelview matrix.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
cylinder_sensor_node::cylinder_sensor_node(const node_type & type,
                                           const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    autoOffset(true),
    diskAngle(0.262f),
    enabled_(true),
    maxAngle(-1.0f),
    minAngle(0.0f),
    offset(0.0f),
    active(false),
    rotation_val(0.0f),
    disk(false)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
cylinder_sensor_node::~cylinder_sensor_node() throw ()
{}

/**
 * @brief Cast to a cylinder_sensor_node.
 *
 * @return a pointer to the cylinder_sensor_node.
 */
cylinder_sensor_node * cylinder_sensor_node::to_cylinder_sensor() const
{
    return (cylinder_sensor_node*) this;
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void cylinder_sensor_node::render(openvrml::viewer & viewer,
                                  rendering_context context)
{
    //
    // Store the modelview matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation.
    //
    this->modelview = context.matrix();
}

/**
 * @brief Called in response to user interaction.
 */
void cylinder_sensor_node::activate(double timeStamp,
                                    bool isActive,
                                    double * p)
{
    // Become active
    if (isActive && !this->active.value) {
        this->active.value = isActive;

        // set activation point in local coords
        vec3f v(static_cast<float>(p[0]),
                static_cast<float>(p[1]),
                static_cast<float>(p[2]));
        this->activationMatrix = this->modelview.inverse();
        v *= this->activationMatrix;
        this->activationPoint = v;
        // Bearing vector in local coordinate system
        v[0] = this->activationMatrix[2][0];
        v[1] = this->activationMatrix[2][1];
        v[2] = this->activationMatrix[2][2];
        const vec3f bearing = v.normalize();
        const vec3f up(0.0, 1.0, 0.0);
        double ang = acos(bearing.dot(up));
        if (ang > pi_2) { ang = pi - ang; }
        this->disk = (ang < this->diskAngle.value);
        // send message
        this->emit_event("isActive", this->active, timeStamp);
    }

    // Become inactive
    else if (!isActive && this->active.value) {
        this->active.value = isActive;
        this->emit_event("isActive", this->active, timeStamp);

        // save auto offset of rotation
        if (this->autoOffset.value) {
            this->offset.value = rotation_val;
            this->emit_event("offset_changed", this->offset, timeStamp);
        }
    }

        // Tracking
    else if (isActive) {
        using openvrml_::fequal;

        // get local coord for touch point
        vec3f Vec(static_cast<float>(p[0]),
                  static_cast<float>(p[1]),
                  static_cast<float>(p[2]));
        Vec = Vec * this->activationMatrix;
        this->trackPoint.value = Vec;
        this->emit_event("trackPoint_changed", this->trackPoint, timeStamp);
        vec3f tempv;
        float rot, radius;
        vec3f dir1(Vec[0], 0, Vec[2]);
        radius = this->disk
               ? 1.0f
               : dir1.length();
        dir1 = dir1.normalize();
        vec3f dir2(this->activationPoint.x(), 0, this->activationPoint.z());
        dir2 = dir2.normalize();
        tempv = dir2 * dir1;
        vec3f cx(tempv);
        cx = cx.normalize();
        if (cx.length() == 0.0) { return; }
        rot = radius * float(acos(dir2.dot(dir1)));
        if (fequal<float>()(cx.y(), -1.0f)) { rot = -rot; }
        if (this->autoOffset.value) {
            rot = this->offset.value + rot;
        }
        if (this->minAngle.value < this->maxAngle.value) {
            if (rot < this->minAngle.value) {
                rot = this->minAngle.value;
            } else if (rot > this->maxAngle.value) {
                rot = this->maxAngle.value;
            }
        }
        this->rotation_val = rot;
        this->rotation.value = openvrml::rotation(0, 1, 0, rot);

        this->emit_event("rotation_changed", this->rotation, timeStamp);
    }
}

/**
 * @brief Indicate whether the sensor is enabled.
 *
 * @return @c true if the sensor is enabled; @c false otherwise.
 */
bool cylinder_sensor_node::enabled() const
{
    return this->enabled_.value;
}

/**
 * @brief set_autoOffset eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void cylinder_sensor_node::process_set_autoOffset(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast)
{
    this->autoOffset = dynamic_cast<const sfbool &>(value);
    this->emit_event("autoOffset_changed", this->autoOffset, timestamp);
}

/**
 * @brief set_diskAngle eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void cylinder_sensor_node::process_set_diskAngle(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast)
{
    this->diskAngle = dynamic_cast<const sffloat &>(value);
    this->emit_event("diskAngle_changed", this->diskAngle, timestamp);
}

/**
 * @brief set_enabled eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void cylinder_sensor_node::process_set_enabled(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast)
{
    this->enabled_ = dynamic_cast<const sfbool &>(value);
    this->emit_event("enabled_changed", this->enabled_, timestamp);
}

/**
 * @brief set_maxAngle eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void cylinder_sensor_node::process_set_maxAngle(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->maxAngle = dynamic_cast<const sffloat &>(value);
    this->emit_event("maxAngle_changed", this->maxAngle, timestamp);
}

/**
 * @brief set_minAngle eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void cylinder_sensor_node::process_set_minAngle(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->minAngle = dynamic_cast<const sffloat &>(value);
    this->emit_event("minAngle_changed", this->minAngle, timestamp);
}

/**
 * @brief set_offset eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void cylinder_sensor_node::process_set_offset(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast)
{
    this->offset = dynamic_cast<const sffloat &>(value);
    this->emit_event("offset_changed", this->offset, timestamp);
}


/**
 * @class directional_light_class
 *
 * @brief Class object for DirectionalLight nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
directional_light_class::directional_light_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
directional_light_class::~directional_light_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating DirectionalLight
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by directional_light_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
directional_light_class::create_type(const std::string & id,
                                     const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef vrml97_node_type_impl<directional_light_node>
        directional_light_type_t;

    const node_type_ptr type(new directional_light_type_t(*this, id));
    directional_light_type_t & directionalLightNodeType =
        static_cast<directional_light_type_t &>(*type);
    typedef directional_light_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &directional_light_node::process_set_ambientIntensity,
                node_field_ptr_ptr(
                    new directional_light_type_t::sffloat_ptr(
                        &directional_light_node::ambientIntensity)));
        } else if (*interface == supportedInterfaces[1]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &directional_light_node::process_set_color,
                node_field_ptr_ptr(
                    new directional_light_type_t::sfcolor_ptr(
                        &directional_light_node::color_)));
        } else if (*interface == supportedInterfaces[2]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &directional_light_node::process_set_direction,
                node_field_ptr_ptr(
                    new directional_light_type_t::sfvec3f_ptr(
                        &directional_light_node::direction)));
        } else if (*interface == supportedInterfaces[3]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &directional_light_node::process_set_intensity,
                node_field_ptr_ptr(
                    new directional_light_type_t::sffloat_ptr(
                        &directional_light_node::intensity_)));
        } else if (*interface == supportedInterfaces[4]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &directional_light_node::process_set_on,
                node_field_ptr_ptr(
                    new directional_light_type_t::sfbool_ptr(
                        &directional_light_node::on_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class directional_light_node
 *
 * @brief DirectionalLight node instances.
 */

/**
 * @var directional_light_node::directional_light_class
 *
 * @brief Class object for DirectionalLight nodes.
 */

/**
 * @var sfvec3f directional_light_node::direction
 *
 * @brief direction exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
directional_light_node::directional_light_node(const node_type & type,
                                               const scope_ptr & scope):
    node(type, scope),
    abstract_light_node(type, scope),
    direction(vec3f(0.0, 0.0, -1.0))
{}

/**
 * @brief Destroy.
 */
directional_light_node::~directional_light_node() throw () {}

/**
 * @brief Render the node.
 *
 * This should be called before rendering any sibling nodes.
 *
 * @param viewer    a viewer.
 * @param context   a rendering context.
 */
void directional_light_node::render(openvrml::viewer & viewer,
                              const rendering_context rc)
{
    if (this->on_.value) {
        viewer.insert_dir_light(this->ambientIntensity.value,
                                this->intensity_.value,
                                this->color_.value,
                                this->direction.value);
    }
    this->node::modified(false);
}

/**
 * @brief set_direction eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void directional_light_node::process_set_direction(const field_value & value,
                                            const double timestamp)
        throw (std::bad_cast) {
    this->direction = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("direction_changed", this->direction, timestamp);
}


/**
 * @class elevation_grid_class
 *
 * @brief Class object for ElevationGrid nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
elevation_grid_class::elevation_grid_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
elevation_grid_class::~elevation_grid_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating ElevationGrid
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by elevation_grid_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
elevation_grid_class::create_type(const std::string & id,
                                  const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mffloat_id, "set_height"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "normal"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "texCoord"),
        node_interface(node_interface::field_id, field_value::mffloat_id, "height"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "ccw"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "colorPerVertex"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "creaseAngle"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "normalPerVertex"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "solid"),
        node_interface(node_interface::field_id, field_value::sfint32_id, "xDimension"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "xSpacing"),
        node_interface(node_interface::field_id, field_value::sfint32_id, "zDimension"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "zSpacing")
    };
    const node_type_ptr type(new vrml97_node_type_impl<elevation_grid_node>(*this, id));
    vrml97_node_type_impl<elevation_grid_node> & elevationGridNodeType =
            static_cast<vrml97_node_type_impl<elevation_grid_node> &>(*type);
    typedef vrml97_node_type_impl<elevation_grid_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            elevationGridNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &elevation_grid_node::process_set_height);
        } else if (*interface == supportedInterfaces[1]) {
            elevationGridNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &elevation_grid_node::process_set_color,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfnode>
                                    (&elevation_grid_node::color)));
        } else if (*interface == supportedInterfaces[2]) {
            elevationGridNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &elevation_grid_node::process_set_normal,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfnode>
                                    (&elevation_grid_node::normal)));
        } else if (*interface == supportedInterfaces[3]) {
            elevationGridNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &elevation_grid_node::process_set_texCoord,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfnode>
                                    (&elevation_grid_node::texCoord)));
        } else if (*interface == supportedInterfaces[4]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, mffloat>
                                    (&elevation_grid_node::height)));
        } else if (*interface == supportedInterfaces[5]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfbool>
                                    (&elevation_grid_node::ccw)));
        } else if (*interface == supportedInterfaces[6]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfbool>
                                    (&elevation_grid_node::colorPerVertex)));
        } else if (*interface == supportedInterfaces[7]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sffloat>
                                    (&elevation_grid_node::creaseAngle)));
        } else if (*interface == supportedInterfaces[8]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfbool>
                                    (&elevation_grid_node::normalPerVertex)));
        } else if (*interface == supportedInterfaces[9]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfbool>
                                    (&elevation_grid_node::solid)));
        } else if (*interface == supportedInterfaces[10]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfint32>
                                    (&elevation_grid_node::xDimension)));
        } else if (*interface == supportedInterfaces[11]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sffloat>
                                    (&elevation_grid_node::xSpacing)));
        } else if (*interface == supportedInterfaces[12]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sfint32>
                                    (&elevation_grid_node::zDimension)));
        } else if (*interface == supportedInterfaces[13]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                node_field_ptr_ptr(new node_field_ptr_impl<elevation_grid_node, sffloat>
                                    (&elevation_grid_node::zSpacing)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class elevation_grid_node
 *
 * @brief ElevationGrid node instances.
 */

/**
 * @var elevation_grid_node::elevation_grid_class
 *
 * @brief Class object for ElevationGrid nodes.
 */

/**
 * @var sfnode elevation_grid_node::color
 *
 * @brief color exposedField.
 */

/**
 * @var sfnode elevation_grid_node::normal
 *
 * @brief normal exposedField.
 */

/**
 * @var sfnode elevation_grid_node::texCoord
 *
 * @brief texCoord exposedField.
 */

/**
 * @var sfbool elevation_grid_node::ccw
 *
 * @brief ccw field.
 */

/**
 * @var sfbool elevation_grid_node::colorPerVertex
 *
 * @brief colorPerVertex field.
 */

/**
 * @var sffloat elevation_grid_node::creaseAngle
 *
 * @brief creaseAngle field.
 */

/**
 * @var mffloat elevation_grid_node::height
 *
 * @brief height field.
 */

/**
 * @var sfbool elevation_grid_node::normalPerVertex
 *
 * @brief normalPerVertex field.
 */

/**
 * @var sfbool elevation_grid_node::solid
 *
 * @brief solid field.
 */

/**
 * @var sfint32 elevation_grid_node::xDimension
 *
 * @brief xDimension field.
 */

/**
 * @var sffloat elevation_grid_node::xSpacing
 *
 * @brief xSpacing field.
 */

/**
 * @var sfint32 elevation_grid_node::zDimension
 *
 * @brief zDimension field.
 */

/**
 * @var sffloat elevation_grid_node::zSpacing
 *
 * @brief zSpacing field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
elevation_grid_node::elevation_grid_node(const node_type & type,
                                         const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope),
    ccw(true),
    colorPerVertex(true),
    normalPerVertex(true),
    solid(true),
    xDimension(0),
    xSpacing(1.0f),
    zDimension(0),
    zSpacing(1.0f)
{}

/**
 * @brief Destroy.
 */
elevation_grid_node::~elevation_grid_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool elevation_grid_node::modified() const
{
    return (this->node::modified()
            || (this->color.value && this->color.value->modified())
            || (this->normal.value && this->normal.value->modified())
            || (this->texCoord.value && this->texCoord.value->modified()));
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t
elevation_grid_node::insert_geometry(openvrml::viewer & viewer,
                                     const rendering_context context)
{
    viewer::object_t obj = 0;

    if (!this->height.value.empty()) {
        using std::vector;

        openvrml::color_node * const colorNode = this->color.value->to_color();
        const vector<openvrml::color> & color = colorNode
                                              ? colorNode->color()
                                              : vector<openvrml::color>();

        openvrml::normal_node * const normalNode =
            this->normal.value->to_normal();
        const vector<vec3f> & normal = normalNode
                                     ? normalNode->vector()
                                     : vector<vec3f>();

        openvrml::texture_coordinate_node * const texCoordNode =
            this->texCoord.value->to_texture_coordinate();
        const vector<vec2f> & texCoord = texCoordNode
                                       ? texCoordNode->point()
                                       : vector<vec2f>();
        // insert geometry
        unsigned int optMask = 0;
        if (this->ccw.value) {
            optMask |= viewer::mask_ccw;
        }
        if (this->solid.value) {
            optMask |= viewer::mask_solid;
        }
        if (this->colorPerVertex.value) {
            optMask |= viewer::mask_color_per_vertex;
        }
        if (this->normalPerVertex.value) {
            optMask |= viewer::mask_normal_per_vertex;
        }

        obj = viewer.insert_elevation_grid(optMask,
                                           this->height.value,
                                           this->xDimension.value,
                                           this->zDimension.value,
                                           this->xSpacing.value,
                                           this->zSpacing.value,
                                           color,
                                           normal,
                                           texCoord);
    }

    if (this->color.value) { this->color.value->modified(false); }
    if (this->normal.value) { this->normal.value->modified(false); }
    if (this->texCoord.value) { this->texCoord.value->modified(false); }

    return obj;
}

/**
 * @brief set_color eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void elevation_grid_node::process_set_color(const field_value & value,
                                     const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->color = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("color_changed", this->color, timestamp);
}

/**
 * @brief set_height eventIn handler.
 *
 * @param value     an mffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void elevation_grid_node::process_set_height(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->height = dynamic_cast<const mffloat &>(value);
    this->node::modified(true);
    this->emit_event("height_changed", this->height, timestamp);
}

/**
 * @brief set_normal eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void elevation_grid_node::process_set_normal(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->normal = dynamic_cast<const sfnode &>(value);
    this->emit_event("normal_changed", this->normal, timestamp);
}

/**
 * @brief set_texCoord eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void elevation_grid_node::process_set_texCoord(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->texCoord = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("texCoord_changed", this->texCoord, timestamp);
}


/**
 * @class extrusion_class
 *
 * @brief Class object for Extrusion nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
extrusion_class::extrusion_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
extrusion_class::~extrusion_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Extrusion nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by extrusion_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
extrusion_class::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mfvec2f_id, "set_crossSection"),
        node_interface(node_interface::eventin_id, field_value::mfrotation_id, "set_orientation"),
        node_interface(node_interface::eventin_id, field_value::mfvec2f_id, "set_scale"),
        node_interface(node_interface::eventin_id, field_value::mfvec3f_id, "set_spine"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "beginCap"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "ccw"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "convex"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "creaseAngle"),
        node_interface(node_interface::field_id, field_value::mfvec2f_id, "crossSection"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "endCap"),
        node_interface(node_interface::field_id, field_value::mfrotation_id, "orientation"),
        node_interface(node_interface::field_id, field_value::mfvec2f_id, "scale"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "solid"),
        node_interface(node_interface::field_id, field_value::mfvec3f_id, "spine")
    };
    const node_type_ptr type(new vrml97_node_type_impl<extrusion_node>(*this, id));
    vrml97_node_type_impl<extrusion_node> & extrusionNodeType =
            static_cast<vrml97_node_type_impl<extrusion_node> &>(*type);
    typedef vrml97_node_type_impl<extrusion_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            extrusionNodeType.add_eventin(supportedInterfaces[0].field_type,
                                         supportedInterfaces[0].id,
                                         &extrusion_node::process_set_crossSection);
        } else if (*interface == supportedInterfaces[1]) {
            extrusionNodeType.add_eventin(supportedInterfaces[1].field_type,
                                         supportedInterfaces[1].id,
                                         &extrusion_node::process_set_orientation);
        } else if (*interface == supportedInterfaces[2]) {
            extrusionNodeType.add_eventin(supportedInterfaces[2].field_type,
                                         supportedInterfaces[2].id,
                                         &extrusion_node::process_set_scale);
        } else if (*interface == supportedInterfaces[3]) {
            extrusionNodeType.add_eventin(supportedInterfaces[3].field_type,
                                         supportedInterfaces[3].id,
                                         &extrusion_node::process_set_spine);
        } else if (*interface == supportedInterfaces[4]) {
            extrusionNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, sfbool>
                                    (&extrusion_node::beginCap)));
        } else if (*interface == supportedInterfaces[5]) {
            extrusionNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, sfbool>
                                    (&extrusion_node::ccw)));
        } else if (*interface == supportedInterfaces[6]) {
            extrusionNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, sfbool>
                                    (&extrusion_node::convex)));
        } else if (*interface == supportedInterfaces[7]) {
            extrusionNodeType.add_field(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, sffloat>
                                    (&extrusion_node::creaseAngle)));
        } else if (*interface == supportedInterfaces[8]) {
            extrusionNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, mfvec2f>
                                    (&extrusion_node::crossSection)));
        } else if (*interface == supportedInterfaces[9]) {
            extrusionNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, sfbool>
                                    (&extrusion_node::endCap)));
        } else if (*interface == supportedInterfaces[10]) {
            extrusionNodeType.add_field(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, mfrotation>
                                    (&extrusion_node::orientation)));
        } else if (*interface == supportedInterfaces[11]) {
            extrusionNodeType.add_field(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, mfvec2f>
                                    (&extrusion_node::scale)));
        } else if (*interface == supportedInterfaces[12]) {
            extrusionNodeType.add_field(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, sfbool>
                                    (&extrusion_node::solid)));
        } else if (*interface == supportedInterfaces[13]) {
            extrusionNodeType.add_field(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                node_field_ptr_ptr(new node_field_ptr_impl<extrusion_node, mfvec3f>
                                    (&extrusion_node::spine)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

namespace {
    const vec2f extrusionDefaultCrossSection_[] = { vec2f(1.0, 1.0),
                                                    vec2f(1.0, -1.0),
                                                    vec2f(-1.0, -1.0),
                                                    vec2f(-1.0, 1.0),
                                                    vec2f(1.0, 1.0) };
    const vec2f extrusionDefaultScale_[] = { vec2f(1.0, 1.0) };
    const rotation extrusionDefaultOrientation_[] =
            { rotation(0.0, 0.0, 1.0, 0.0) };
    const vec3f extrusionDefaultSpine_[] = { vec3f(0.0, 0.0, 0.0),
                                             vec3f(0.0, 1.0, 0.0) };
}

/**
 * @class extrusion_node
 *
 * @brief Extrusion node instances.
 */

/**
 * @var extrusion_node::extrusion_class
 *
 * @brief Class object for Extrusion nodes.
 */

/**
 * @var sfbool extrusion_node::beginCap
 *
 * @brief beginCap field.
 */

/**
 * @var sfbool extrusion_node::ccw
 *
 * @brief ccw field.
 */

/**
 * @var sfbool extrusion_node::convex
 *
 * @brief convex field.
 */

/**
 * @var sffloat extrusion_node::creaseAngle
 *
 * @brief creaseAngle field.
 */

/**
 * @var mfvec2f extrusion_node::crossSection
 *
 * @brief crossSection field.
 */

/**
 * @var sfbool extrusion_node::endCap
 *
 * @brief endCap field.
 */

/**
 * @var mfrotation extrusion_node::orientation
 *
 * @brief orientation field.
 */

/**
 * @var mfvec2f extrusion_node::scale
 *
 * @brief scale field.
 */

/**
 * @var sfbool extrusion_node::solid
 *
 * @brief solid field.
 */

/**
 * @var mfvec3f extrusion_node::spine
 *
 * @brief spine field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
extrusion_node::extrusion_node(const node_type & type,
                               const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope),
    beginCap(true),
    ccw(true),
    convex(true),
    creaseAngle(0),
    crossSection(extrusionDefaultCrossSection_,
                 extrusionDefaultCrossSection_ + 5),
    endCap(true),
    orientation(extrusionDefaultOrientation_,
                extrusionDefaultOrientation_ + 1),
    scale(extrusionDefaultScale_, extrusionDefaultScale_ + 1),
    solid(true),
    spine(extrusionDefaultSpine_, extrusionDefaultSpine_ + 2)
{}

/**
 * @brief Destroy.
 */
extrusion_node::~extrusion_node() throw () {}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t
extrusion_node::insert_geometry(openvrml::viewer & viewer,
                                const rendering_context context)
{
    viewer::object_t obj = 0;
    if (this->crossSection.value.size() > 0 && this->spine.value.size() > 1) {

        unsigned int optMask = 0;
        if (this->ccw.value)        { optMask |= viewer::mask_ccw; }
        if (this->convex.value)     { optMask |= viewer::mask_convex; }
        if (this->solid.value)      { optMask |= viewer::mask_solid; }
        if (this->beginCap.value)   { optMask |= viewer::mask_bottom; }
        if (this->endCap.value)     { optMask |= viewer::mask_top; }

        obj = viewer.insert_extrusion(optMask,
                                      this->spine.value,
                                      this->crossSection.value,
                                      this->orientation.value,
                                      this->scale.value);
    }

    return obj;
}

/**
 * @brief set_crossSection eventIn handler.
 *
 * @param value     an mfvec2f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec2f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void extrusion_node::process_set_crossSection(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->crossSection = dynamic_cast<const mfvec2f &>(value);
    this->node::modified(true);
}

/**
 * @brief set_orientation field mutator.
 *
 * @param value     an mfrotation value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfrotation.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void extrusion_node::process_set_orientation(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->orientation = dynamic_cast<const mfrotation &>(value);
    this->node::modified(true);
}

/**
 * @brief set_scale eventIn handler.
 *
 * @param value     an mfvec2f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec2f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void extrusion_node::process_set_scale(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->scale = dynamic_cast<const mfvec2f &>(value);
    this->node::modified(true);
}

/**
 * @brief set_spine eventIn handler.
 *
 * @param value     an mfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec3f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void extrusion_node::process_set_spine(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->spine = dynamic_cast<const mfvec3f &>(value);
    this->node::modified(true);
}


/**
 * @class fog_class
 *
 * @brief Class object for Fog nodes.
 */

/**
 * @typedef fog_class::bound_nodes_t
 *
 * @brief A bound Fog node stack.
 */

/**
 * @var fog_node * fog_class::first
 *
 * @brief The first Fog node in the initial scene graph.
 */

/**
 * @var fog_class::bound_nodes_t fog_class::bound_nodes
 *
 * @brief The bound Fog node stack.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
fog_class::fog_class(openvrml::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
fog_class::~fog_class() throw ()
{}

/**
 * @brief Set the first Fog node in the world.
 *
 * The first Fog node in the world is used for the initial fog.
 * This method is used by fog_node::do_initialize.
 *
 * @param fog   a Fog node.
 */
void fog_class::set_first(fog_node & fog) throw ()
{
    this->first = &fog;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by fog_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool fog_class::has_first() const throw ()
{
    return this->first;
}

/**
 * @brief Push a Fog on the top of the bound node stack.
 *
 * @param fog       the node to bind.
 * @param timestamp the current time.
 */
void fog_class::bind(fog_node & fog, const double timestamp)
    throw (std::bad_alloc)
{
    using std::find;

    //
    // If the node is already the active node, do nothing.
    //
    if (!this->bound_nodes.empty() && &fog == this->bound_nodes.back()) {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const bound_nodes_t::iterator pos =
        find(this->bound_nodes.begin(), this->bound_nodes.end(), &fog);
    if (pos != this->bound_nodes.end()) { this->bound_nodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->bound_nodes.empty()) {
        fog_node & current =
            dynamic_cast<fog_node &>(*this->bound_nodes.back());
        current.bound.value = false;
        current.emit_event("isBound", current.bound, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->bound_nodes.push_back(&fog);
    fog.bound.value = true;
    fog.emit_event("isBound", fog.bound, timestamp);
}

/**
 * @brief Remove a Fog from the bound node stack.
 *
 * @param fog       the node to unbind.
 * @param timestamp the current time.
 */
void fog_class::unbind(fog_node & fog, const double timestamp) throw ()
{
    const bound_nodes_t::iterator pos =
        std::find(this->bound_nodes.begin(), this->bound_nodes.end(), &fog);
    if (pos != this->bound_nodes.end()) {
        fog.bound.value = false;
        fog.emit_event("isBound", fog.bound, timestamp);

        if (pos == this->bound_nodes.end() - 1
            && this->bound_nodes.size() > 1) {
            fog_node & newActive =
                dynamic_cast<fog_node &>(**(this->bound_nodes.end() - 2));
            newActive.bound.value = true;
            newActive.emit_event("isBound", newActive.bound, timestamp);
        }
        this->bound_nodes.erase(pos);
    }
}

/**
 * @brief node_class-specific initialization.
 *
 * @param initialViewpoint  the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void fog_class::initialize(openvrml::viewpoint_node * initialViewpoint,
                           const double timestamp)
    throw ()
{
    if (this->first) {
        this->first->process_event("set_bind", sfbool(true), timestamp);
    }
}

/**
 * @brief node_class-specific rendering.
 *
 * Render the active Fog node.
 *
 * @param viewer    a Viewer.
 */
void fog_class::render(openvrml::viewer & viewer) throw ()
{
    if (!this->bound_nodes.empty()) {
        fog_node & fog = dynamic_cast<fog_node &>(*this->bound_nodes.back());
        viewer.set_fog(fog.color.value,
                       fog.visibilityRange.value,
                       fog.fogType.value.c_str());
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Fog nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by fog_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
fog_class::create_type(const std::string & id,
                       const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sfbool_id, "set_bind"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfstring_id, "fogType"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "visibilityRange"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isBound")
    };
    const node_type_ptr type(new vrml97_node_type_impl<fog_node>(*this, id));
    vrml97_node_type_impl<fog_node> & fogNodeType =
            static_cast<vrml97_node_type_impl<fog_node> &>(*type);
    typedef vrml97_node_type_impl<fog_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            fogNodeType.add_eventin(supportedInterfaces[0].field_type,
                                   supportedInterfaces[0].id,
                                   &fog_node::process_set_bind);
        } else if (*interface == supportedInterfaces[1]) {
            fogNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &fog_node::process_set_color,
                node_field_ptr_ptr(new node_field_ptr_impl<fog_node, sfcolor>
                                    (&fog_node::color)));
        } else if (*interface == supportedInterfaces[2]) {
            fogNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &fog_node::process_set_fogType,
                node_field_ptr_ptr(new node_field_ptr_impl<fog_node, sfstring>
                                    (&fog_node::fogType)));
        } else if (*interface == supportedInterfaces[3]) {
            fogNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &fog_node::process_set_visibilityRange,
                node_field_ptr_ptr(new node_field_ptr_impl<fog_node, sffloat>
                                    (&fog_node::visibilityRange)));
        } else if (*interface == supportedInterfaces[4]) {
            fogNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<fog_node, sfbool>
                                    (&fog_node::bound)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class fog_node
 *
 * @brief Fog node instances.
 */

/**
 * @var fog_node::fog_class
 *
 * @brief Class object for Fog nodes.
 */

/**
 * @var sfcolor fog_node::color
 *
 * @brief color exposedField.
 */

/**
 * @var sfstring fog_node::fogType
 *
 * @brief fogType exposedField.
 */

/**
 * @var sffloat fog_node::visibilityRange
 *
 * @brief visibilityRange exposedField.
 */

/**
 * @var sfbool fog_node::bound
 *
 * @brief isBound eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
fog_node::fog_node(const node_type & type,
                   const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    color(openvrml::color(1.0, 1.0, 1.0)),
    fogType("LINEAR"),
    visibilityRange(0.0),
    bound(false)
{}

/**
 * @brief Destroy.
 */
fog_node::~fog_node() throw ()
{}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void fog_node::do_initialize(const double timestamp) throw ()
{
    fog_class & nodeClass = static_cast<fog_class &>(this->type.node_class);
    if (!nodeClass.has_first()) { nodeClass.set_first(*this); }
}

/**
 * @brief Shut down.
 *
 * Calls fog_class::unbind to unbind the node if it is bound.
 *
 * @param timestamp the current time.
 */
void fog_node::do_shutdown(const double timestamp) throw ()
{
    fog_class & nodeClass = static_cast<fog_class &>(this->type.node_class);
    nodeClass.unbind(*this, timestamp);
}

/**
 * @brief set_bind eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfbool value.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void fog_node::process_set_bind(const field_value & value,
                                const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    const sfbool & bind = dynamic_cast<const sfbool &>(value);
    fog_class & nodeClass = static_cast<fog_class &>(this->type.node_class);
    if (bind.value) {
        nodeClass.bind(*this, timestamp);
    } else {
        nodeClass.unbind(*this, timestamp);
    }
}

/**
 * @brief set_color eventIn handler.
 *
 * @param value     an sfcolor value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfcolor value.
 */
void fog_node::process_set_color(const field_value & value,
                                 const double timestamp)
    throw (std::bad_cast)
{
    this->color = dynamic_cast<const sfcolor &>(value);
    this->node::modified(true);
    this->emit_event("color_changed", this->color, timestamp);
}

/**
 * @brief set_fogType eventIn handler.
 *
 * @param value     an sfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfstring value.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void fog_node::process_set_fogType(const field_value & value,
                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->fogType = dynamic_cast<const sfstring &>(value);
    this->node::modified(true);
    this->emit_event("fogType_changed", this->fogType, timestamp);
}

/**
 * @brief set_visibilityRange eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat value.
 */
void fog_node::process_set_visibilityRange(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast)
{
    this->visibilityRange = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("visibilityRange_changed", this->visibilityRange,
                     timestamp);
}


/**
 * @class font_style_class
 *
 * @brief Class object for FontStyle nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
font_style_class::font_style_class(openvrml::browser & browser):
    node_class(browser)
{}

font_style_class::~font_style_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating FontStyle nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by font_style_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
font_style_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<font_style_node>(*this, id));
    vrml97_node_type_impl<font_style_node> & fontStyleNodeType =
            static_cast<vrml97_node_type_impl<font_style_node> &>(*type);
    typedef vrml97_node_type_impl<font_style_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, mfstring>
                                    (&font_style_node::family_)));
        } else if (*interface == supportedInterfaces[1]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, sfbool>
                                    (&font_style_node::horizontal_)));
        } else if (*interface == supportedInterfaces[2]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, mfstring>
                                    (&font_style_node::justify_)));
        } else if (*interface == supportedInterfaces[3]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, sfstring>
                                    (&font_style_node::language_)));
        } else if (*interface == supportedInterfaces[4]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, sfbool>
                                    (&font_style_node::leftToRight)));
        } else if (*interface == supportedInterfaces[5]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, sffloat>
                                    (&font_style_node::size_)));
        } else if (*interface == supportedInterfaces[6]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, sffloat>
                                    (&font_style_node::spacing_)));
        } else if (*interface == supportedInterfaces[7]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, sfstring>
                                    (&font_style_node::style_)));
        } else if (*interface == supportedInterfaces[8]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_field_ptr_ptr(new node_field_ptr_impl<font_style_node, sfbool>
                                    (&font_style_node::topToBottom)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class font_style_node
 *
 * @brief FontStyle node instances.
 */

/**
 * @var font_style_node::font_style_class
 *
 * @brief Class object for FontStyle nodes.
 */

/**
 * @var mfstring font_style_node::family_
 *
 * @brief family field.
 */

/**
 * @var sfbool font_style_node::horizontal_
 *
 * @brief horizontal field.
 */

/**
 * @var mfstring font_style_node::justify_
 *
 * @brief justify field.
 */

/**
 * @var sfstring font_style_node::language_
 *
 * @brief language field.
 */

/**
 * @var sfbool font_style_node::leftToRight
 *
 * @brief leftToRight field.
 */

/**
 * @var sffloat font_style_node::size_
 *
 * @brief size field.
 */

/**
 * @var sffloat font_style_node::spacing_
 *
 * @brief spacing field.
 */

/**
 * @var sfstring font_style_node::style_
 *
 * @brief style field.
 */

/**
 * @var sfbool font_style_node::topToBottom
 *
 * @brief topToBottom field.
 */

namespace {
    const std::string fontStyleInitFamily_[] = { "SERIF" };
    const std::string fontStyleInitJustify_[] = { "BEGIN", "FIRST" };
}

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
font_style_node::font_style_node(const node_type & type,
                                 const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::font_style_node(type, scope),
    family_(fontStyleInitFamily_, fontStyleInitFamily_ + 1),
    horizontal_(true),
    justify_(fontStyleInitJustify_, fontStyleInitJustify_ + 2),
    leftToRight(true),
    size_(1.0),
    spacing_(1.0),
    style_("PLAIN"),
    topToBottom(true)
{}

/**
 * @brief Destroy.
 */
font_style_node::~font_style_node() throw ()
{}

/**
 * @brief Get the list of font families.
 *
 * @return an mfstring containing the font families that may be used for this
 *      font_style_node.
 */
const std::vector<std::string> & font_style_node::family() const throw ()
{
    return this->family_.value;
}

/**
 * @brief Get the flag indicating whether the text should be horizontal or
 *      vertical.
 *
 * @return @c true if the text should be horizontal, or @c false if the text
 *      should be vertical.
 */
bool font_style_node::horizontal() const throw ()
{
    return this->horizontal_.value;
}

/**
 * @brief Get the descriptor for the text justification.
 *
 * @return a string array describing the characteristics of the text
 *      justification.
 */
const std::vector<std::string> & font_style_node::justify() const throw ()
{
    return this->justify_.value;
}

/**
 * @brief Get the language code.
 *
 * @return the language code.
 */
const std::string & font_style_node::language() const throw ()
{
    return this->language_.value;
}

/**
 * @brief Get the flag indicating whether the text should be rendered
 *      left-to-right.
 *
 * @return @c true if the text should be rendered left-to-right, or @c false if
 *      the text should be rendered right-to-left.
 */
bool font_style_node::left_to_right() const throw ()
{
    return this->leftToRight.value;
}

/**
 * @brief Get the size of the text.
 *
 * @return the size of the text.
 */
float font_style_node::size() const throw ()
{
    return this->size_.value;
}

/**
 * @brief Get the spacing for the text.
 *
 * @return the spacing for the text.
 */
float font_style_node::spacing() const throw ()
{
    return this->spacing_.value;
}

/**
 * @brief Get the style for the text.
 *
 * @return an string descriptor of the text style.
 */
const std::string & font_style_node::style() const throw ()
{
    return this->style_.value;
}

/**
 * @brief Get the flag indicating whether the text should be rendered
 *      top-to-bottom.
 *
 * @return @c true if the text should be rendered top-to-bottom, or @c false if
 *      the text should be rendered bottom-to-top.
 */
bool font_style_node::top_to_bottom() const throw ()
{
    return this->topToBottom.value;
}


/**
 * @class group_class
 *
 * @brief Class object for Group nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
group_class::group_class(openvrml::browser & browser): node_class(browser) {}

/**
 * @brief Destroy.
 */
group_class::~group_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating group nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by group_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
group_class::create_type(const std::string & id,
                         const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<group_node>(*this, id));
    vrml97_node_type_impl<group_node> & groupNodeType =
        static_cast<vrml97_node_type_impl<group_node> &>(*type);
    typedef vrml97_node_type_impl<group_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            groupNodeType.add_eventin(supportedInterfaces[0].field_type,
                                     supportedInterfaces[0].id,
                                     &group_node::process_addChildren);
        } else if (*interface == supportedInterfaces[1]) {
            groupNodeType.add_eventin(supportedInterfaces[1].field_type,
                                     supportedInterfaces[1].id,
                                     &group_node::process_removeChildren);
        } else if (*interface == supportedInterfaces[2]) {
            groupNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &group_node::process_set_children,
                node_field_ptr_ptr(new node_field_ptr_impl<group_node, mfnode>
                                    (&group_node::children_)));
        } else if (*interface == supportedInterfaces[3]) {
            groupNodeType.add_field(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<group_node, sfvec3f>
                                    (&group_node::bboxCenter)));
        } else if (*interface == supportedInterfaces[4]) {
            groupNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<group_node, sfvec3f>
                                (&group_node::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class group_node
 *
 * @brief Represents Group node instances.
 */

/**
 * @var group_node::group_class
 *
 * @brief Class object for Group nodes.
 */

/**
 * @var sfvec3f group_node::bboxCenter
 *
 * @brief bboxCenter field.
 */

/**
 * @var sfvec3f group_node::bboxSize
 *
 * @brief bboxSize field.
 */

/**
 * @var mfnode group_node::children_
 *
 * @brief children exposedField.
 */

/**
 * @var viewer::object_t group_node::viewerObject
 *
 * @brief Handle for the renderer.
 */

/**
 * @var bounding_sphere group_node::bsphere
 *
 * @brief Cached copy of the bounding_sphere enclosing this node's children.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the scope to which the node belongs.
 */
group_node::group_node(const node_type & type,
             const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    bboxSize(vec3f(-1.0, -1.0, -1.0)),
    viewerObject(0)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
group_node::~group_node() throw ()
{
    // delete viewerObject...
}

/**
 * @brief addChildren eventIn handler.
 *
 * @param value     an mfnode containing nodes to add to this Group.
 * @param timestamp the current timestamp
 *
 * @exception std::bad_cast     if @p value is not an mfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void group_node::process_addChildren(const field_value & value,
                                const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    const mfnode & newChildren = dynamic_cast<const mfnode &>(value);
    size_t nNow = this->children_.value.size();

    for (size_t i = 0; i < newChildren.value.size(); ++i) {
        const node_ptr & child = newChildren.value[i];
        if (child && child->to_child()) {
            this->children_.value.push_back(child);
            child->relocate();
        } else {
            OPENVRML_PRINT_MESSAGE_("Attempt to add a " + child->type.id
                                    + " node as a child of a " + this->type.id
                                    + " node.");
        }
    }

    if (nNow != this->children_.value.size()) {
        this->node::modified(true);
        this->bounding_volume_dirty(true);
    }
}

/**
 * @brief removeChildren eventIn handler.
 *
 * @param value     an mfnode containing nodes to remove from this Group.
 * @param timestamp the current timestamp
 *
 * @exception std::bad_cast     if @p value is not an mfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void group_node::process_removeChildren(const field_value & value,
                                   const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    const mfnode & childrenToRemove = dynamic_cast<const mfnode &>(value);
    const size_t oldLength = this->children_.value.size();

    for (size_t i = 0; i < childrenToRemove.value.size(); ++i) {
        const node_ptr & node = childrenToRemove.value[i];
        if (node) {
            using std::remove;
            using std::vector;
            const vector<node_ptr>::iterator begin =
                this->children_.value.begin();
            const vector<node_ptr>::iterator end = this->children_.value.end();
            this->children_.value.erase(remove(begin, end, node), end);
        }
    }

    if (oldLength != this->children_.value.size()) {
        this->node::modified(true);
        this->bounding_volume_dirty(true);
    }
}

/**
 * @brief set_children eventIn handler.
 *
 * @param value     an mfnode containing nodes for this Group.
 * @param timestamp the current timestamp
 *
 * @exception std::bad_cast     if @p value is not an mfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void group_node::process_set_children(const field_value & value,
                                 const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->children_ = dynamic_cast<const mfnode &>(value);

    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        if (children_.value[i]) {
            children_.value[i]->relocate();
        }
    }

    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->emit_event("children_changed", this->children_, timestamp);
}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool group_node::modified() const
{
    if (this->node::modified()) { return true; }
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        if (this->children_.value[i]->modified()) { return true; }
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
void group_node::render(openvrml::viewer & viewer, rendering_context context)
{
    if (context.cull_flag != bounding_volume::inside) {
        assert(dynamic_cast<const bounding_sphere *>
               (&this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
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
void group_node::render_nocull(openvrml::viewer & viewer,
                               rendering_context context)
{
    using std::vector;

    if (this->viewerObject && this->modified()) {
        viewer.remove_object(this->viewerObject);
        this->viewerObject = 0;
    }

    if (this->viewerObject) {
        viewer.insert_reference(this->viewerObject);
    } else if (this->children_.value.size() > 0) {
        vector<node_ptr>::size_type i, n = this->children_.value.size();
        size_t nSensors = 0;

        this->viewerObject = viewer.begin_object(this->id().c_str());

        // Draw nodes that impact their siblings (DirectionalLights,
        // TouchSensors, any others? ...)
        for (i = 0; i < n; ++i) {
          const node_ptr & kid = this->children_.value[i];

            if (kid->to_light()
                    && !(kid->to_point_light() || kid->to_spot_light())) {
                kid->render(viewer, context);
            } else if ((kid->to_touch_sensor()
                        && kid->to_touch_sensor()->enabled())
                    || (kid->to_plane_sensor()
                        && kid->to_plane_sensor()->enabled())
                    || (kid->to_cylinder_sensor()
                        && kid->to_cylinder_sensor()->enabled())
                    || (kid->to_sphere_sensor()
                        && kid->to_sphere_sensor()->isEnabled())) {
                if (++nSensors == 1) { viewer.set_sensitive(this); }
            }
        }

        // Do the rest of the children (except the scene-level lights)
        for (i = 0; i<n; ++i) {
            const node_ptr & child = this->children_.value[i];
            if (!(child->to_light()
//                    || child->to_plane_sensor()
//                    || child->to_cylinder_sensor()
//                    || child->to_sphere_sensor()
                    || child->to_touch_sensor())) {
                child->render(viewer, context);
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
 */
const std::vector<node_ptr> & group_node::children() const throw ()
{
    return this->children_.value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void group_node::activate(double time, bool isOver, bool isActive, double *p)
{
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        const node_ptr & node = this->children_.value[i];
        if (node) {
            if (node->to_touch_sensor()
                    && node->to_touch_sensor()->enabled()) {
                node->to_touch_sensor()->activate(time, isOver, isActive, p);
            } else if (node->to_plane_sensor()
                    && node->to_plane_sensor()->enabled()) {
                node->to_plane_sensor()->activate(time, isActive, p);
            } else if (node->to_cylinder_sensor()
                    && node->to_cylinder_sensor()->enabled()) {
                node->to_cylinder_sensor()->activate(time, isActive, p);
            } else if (node->to_sphere_sensor()
                    && node->to_sphere_sensor()->isEnabled()) {
                node->to_sphere_sensor()->activate(time, isActive, p);
            }
        }
    }
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & group_node::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<group_node *>(this)->recalc_bsphere();
    }
    return this->bsphere;
}

/**
 * @brief Recalculate the bounding volume.
 */
void group_node::recalc_bsphere()
{
    this->bsphere = bounding_sphere();
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        const node_ptr & node = this->children_.value[i];
        if (node) {
            const openvrml::bounding_volume & ci_bv = node->bounding_volume();
            this->bsphere.extend(ci_bv);
        }
    }
    this->bounding_volume_dirty(false);
}


/**
 * @class image_texture_class
 *
 * @brief Class object for ImageTexture nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
image_texture_class::image_texture_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
image_texture_class::~image_texture_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating ImageTexture
 *         nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by image_texture_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
image_texture_class::create_type(const std::string & id,
                                 const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<image_texture_node>(*this, id));
    vrml97_node_type_impl<image_texture_node> & imageTextureNodeType =
            static_cast<vrml97_node_type_impl<image_texture_node> &>(*type);
    typedef vrml97_node_type_impl<image_texture_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            imageTextureNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &image_texture_node::process_set_url,
                node_field_ptr_ptr(new node_field_ptr_impl<image_texture_node, mfstring>
                                    (&image_texture_node::url)));
        } else if (*interface == supportedInterfaces[1]) {
            imageTextureNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(new node_field_ptr_impl<image_texture_node, sfbool>
                                    (&image_texture_node::repeatS)));
        } else if (*interface == supportedInterfaces[2]) {
            imageTextureNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_field_ptr_ptr(new node_field_ptr_impl<image_texture_node, sfbool>
                                    (&image_texture_node::repeatT)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class image_texture_node
 *
 * @brief Represents ImageTexture node instances.
 */

/**
 * @var image_texture_node::image_texture_class
 *
 * @brief Class object for ImageTexture nodes.
 */

/**
 * @var mfstring image_texture_node::url
 *
 * @brief url exposedField.
 */

/**
 * @var img * image_texture_node::image
 *
 * @brief Image data.
 */

/**
 * @var viewer::texture_object_t image_texture_node::texObject
 *
 * @brief Handler for the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
image_texture_node::image_texture_node(const node_type & type,
                                       const scope_ptr & scope):
    node(type, scope),
    abstract_texture_node(type, scope),
    image(0),
    texObject(0)
{}

/**
 * @brief Destroy.
 */
image_texture_node::~image_texture_node() throw ()
{
    delete this->image;
    // delete texObject...
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void image_texture_node::render(openvrml::viewer & viewer,
                                rendering_context context)
{
    if (modified()) {
        if (this->image) {
            delete this->image;        // URL is the only modifiable bit
            this->image = 0;
        }
        if (this->texObject) {
            viewer.remove_texture_object(this->texObject);
            this->texObject = 0;
        }
    }

    // should probably read the image during addToScene...
    // should cache on url so multiple references to the same file are
    // loaded just once... of course world authors should just DEF/USE
    // them...
    if (!this->image && this->url.value.size() > 0) {
        doc2 baseDoc(this->scene()->url());
        this->image = new img;
        if (!this->image->try_urls(this->url.value, &baseDoc)) {
            OPENVRML_PRINT_MESSAGE_("Couldn't read ImageTexture from URL "
                                    + this->url.value[0]);
        }
    }

    // Check texture cache
    if (this->texObject && this->image) {
        viewer.insert_texture_reference(this->texObject, this->image->nc());
    } else {
        unsigned char *pix;

        if (this->image && (pix = this->image->pixels())) {
            // Ensure the image dimensions are powers of two
            const size_t sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
            const size_t nSizes = sizeof(sizes) / sizeof(int);
            const size_t w = this->image->w();
            const size_t h = this->image->h();
            size_t i, j;
            for (i = 0; i < nSizes; ++i) { if (w < sizes[i]) { break; } }
            for (j = 0; j < nSizes; ++j) { if (h < sizes[j]) { break; } }

            if (i > 0 && j > 0) {
                // Always scale images down in size and reuse the same pixel
                // memory. This can cause some ugliness...
                if (w != sizes[i - 1] || h != sizes[j - 1]) {
                    viewer.scale_texture(w, h, sizes[i - 1], sizes[j - 1],
                                        this->image->nc(), pix);
                    this->image->resize(sizes[i - 1], sizes[j - 1]);
                }

                this->texObject = viewer.insert_texture(this->image->w(),
                                                        this->image->h(),
                                                        this->image->nc(),
                                                        this->repeatS.value,
                                                        this->repeatT.value,
                                                        pix,
                                                        true);
            }
        }
    }

    this->node::modified(false);
}

/**
 * @brief The number of components.
 *
 * @return the number of components.
 */
size_t image_texture_node::components() const throw ()
{
    return this->image ? this->image->nc() : 0;
}

/**
 * @brief The image width in pixels.
 *
 * @return the image width in pixels.
 */
size_t image_texture_node::width() const throw ()
{
    return this->image ? this->image->w() : 0;
}

/**
 * @brief The image height in pixels.
 *
 * @return the image height in pixels.
 */
size_t image_texture_node::height() const throw ()
{
    return this->image ? this->image->h() : 0;
}

/**
 * @brief The number of frames.
 *
 * @return 0.
 */
size_t image_texture_node::frames() const throw ()
{
    return 0;
}

/**
 * @brief The image pixel data.
 *
 * @return the image pixel data.
 */
const unsigned char * image_texture_node::pixels() const throw ()
{
    return this->image ? this->image->pixels() : 0;
}

/**
 * @brief set_url eventIn handler.
 *
 * @param value     an mfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void image_texture_node::process_set_url(const field_value & value,
                                  const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->url = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("url_changed", this->url, timestamp);
}


/**
 * @class indexed_face_set_class
 *
 * @brief Class object for IndexedFaceSet nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
indexed_face_set_class::indexed_face_set_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
indexed_face_set_class::~indexed_face_set_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating IndexedFaceSet nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by indexed_face_set_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
indexed_face_set_class::create_type(const std::string & id,
                                    const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mfint32_id, "set_colorIndex"),
        node_interface(node_interface::eventin_id, field_value::mfint32_id, "set_coordIndex"),
        node_interface(node_interface::eventin_id, field_value::mfint32_id, "set_normalIndex"),
        node_interface(node_interface::eventin_id, field_value::mfint32_id, "set_texCoordIndex"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "coord"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "normal"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "texCoord"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "ccw"),
        node_interface(node_interface::field_id, field_value::mfint32_id, "colorIndex"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "colorPerVertex"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "convex"),
        node_interface(node_interface::field_id, field_value::mfint32_id, "coordIndex"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "creaseAngle"),
        node_interface(node_interface::field_id, field_value::mfint32_id, "normalIndex"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "normalPerVertex"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "solid"),
        node_interface(node_interface::field_id, field_value::mfint32_id, "texCoordIndex")
    };
    const node_type_ptr type(new vrml97_node_type_impl<indexed_face_set_node>(*this, id));
    vrml97_node_type_impl<indexed_face_set_node> & indexedFaceSetNodeType =
            static_cast<vrml97_node_type_impl<indexed_face_set_node> &>(*type);
    typedef vrml97_node_type_impl<indexed_face_set_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            indexedFaceSetNodeType
                    .add_eventin(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &indexed_face_set_node::process_set_colorIndex);
        } else if (*interface == supportedInterfaces[1]) {
            indexedFaceSetNodeType
                    .add_eventin(supportedInterfaces[1].field_type,
                                supportedInterfaces[1].id,
                                &indexed_face_set_node::process_set_coordIndex);
        } else if (*interface == supportedInterfaces[2]) {
            indexedFaceSetNodeType
                    .add_eventin(supportedInterfaces[2].field_type,
                                supportedInterfaces[2].id,
                                &indexed_face_set_node::process_set_normalIndex);
        } else if (*interface == supportedInterfaces[3]) {
            indexedFaceSetNodeType
                    .add_eventin(supportedInterfaces[3].field_type,
                                supportedInterfaces[3].id,
                                &indexed_face_set_node::process_set_texCoordIndex);
        } else if (*interface == supportedInterfaces[4]) {
            indexedFaceSetNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &indexed_face_set_node::process_set_color,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfnode>
                                    (&indexed_face_set_node::color_)));
        } else if (*interface == supportedInterfaces[5]) {
            indexedFaceSetNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &indexed_face_set_node::process_set_coord,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfnode>
                                    (&indexed_face_set_node::coord)));
        } else if (*interface == supportedInterfaces[6]) {
            indexedFaceSetNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &indexed_face_set_node::process_set_normal,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfnode>
                                    (&indexed_face_set_node::normal)));
        } else if (*interface == supportedInterfaces[7]) {
            indexedFaceSetNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &indexed_face_set_node::process_set_texCoord,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfnode>
                                    (&indexed_face_set_node::texCoord)));
        } else if (*interface == supportedInterfaces[8]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfbool>
                                    (&indexed_face_set_node::ccw)));
        } else if (*interface == supportedInterfaces[9]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, mfint32>
                                    (&indexed_face_set_node::colorIndex)));
        } else if (*interface == supportedInterfaces[10]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfbool>
                                    (&indexed_face_set_node::colorPerVertex)));
        } else if (*interface == supportedInterfaces[11]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfbool>
                                    (&indexed_face_set_node::convex)));
        } else if (*interface == supportedInterfaces[12]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, mfint32>
                                    (&indexed_face_set_node::coordIndex)));
        } else if (*interface == supportedInterfaces[13]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sffloat>
                                    (&indexed_face_set_node::creaseAngle)));
        } else if (*interface == supportedInterfaces[14]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[14].field_type,
                supportedInterfaces[14].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, mfint32>
                                    (&indexed_face_set_node::normalIndex)));
        } else if (*interface == supportedInterfaces[15]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[15].field_type,
                supportedInterfaces[15].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfbool>
                                    (&indexed_face_set_node::normalPerVertex)));
        } else if (*interface == supportedInterfaces[16]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[16].field_type,
                supportedInterfaces[16].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, sfbool>
                                    (&indexed_face_set_node::solid)));
        } else if (*interface == supportedInterfaces[17]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[17].field_type,
                supportedInterfaces[17].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_face_set_node, mfint32>
                                    (&indexed_face_set_node::texCoordIndex)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class indexed_face_set_node
 *
 * @brief Represents IndexedFaceSet node instances.
 */

/**
 * @var indexed_face_set_node::indexed_face_set_class
 *
 * @brief Class object for IndexedFaceSet nodes.
 */

/**
 * @var sfbool indexed_face_set_node::ccw
 *
 * @brief ccw field.
 */

/**
 * @var sfbool indexed_face_set_node::convex
 *
 * @brief convex field.
 */

/**
 * @var sffloat indexed_face_set_node::creaseAngle
 *
 * @brief creaseAngle field.
 */

/**
 * @var sfnode indexed_face_set_node::normal
 *
 * @brief normal exposedField.
 */

/**
 * @var mfint32 indexed_face_set_node::normalIndex
 *
 * @brief set_normalIndex eventIn.
 */

/**
 * @var sfbool indexed_face_set_node::normalPerVertex
 *
 * @brief normalPerVertex field.
 */

/**
 * @var sfbool indexed_face_set_node::solid
 *
 * @brief solid field.
 */

/**
 * @var sfnode indexed_face_set_node::texCoord
 *
 * @brief texCoord exposedField.
 */

/**
 * @var mfint32 indexed_face_set_node::texCoordIndex
 *
 * @brief set_texCoordIndex eventIn.
 */

/**
 * @var bounding_sphere indexed_face_set_node::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
indexed_face_set_node::indexed_face_set_node(const node_type & type,
                                             const scope_ptr & scope):
    node(type, scope),
    abstract_indexed_set_node(type, scope),
    ccw(true),
    convex(true),
    creaseAngle(0.0),
    normalPerVertex(true),
    solid(true) {
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
indexed_face_set_node::~indexed_face_set_node() throw () {}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool indexed_face_set_node::modified() const {
    return (this->node::modified()
            || (this->color_.value && this->color_.value->modified())
            || (this->coord.value && this->coord.value->modified())
            || (this->normal.value && this->normal.value->modified())
            || (this->texCoord.value && this->texCoord.value->modified()));
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @todo stripify, crease angle, generate normals ...
 */
viewer::object_t indexed_face_set_node::insert_geometry(openvrml::viewer & viewer,
                                               const rendering_context context)
{
    using std::vector;

    if (context.draw_bounding_spheres) {
        assert(dynamic_cast<const bounding_sphere *>(&this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
        viewer.draw_bounding_sphere(bs, static_cast<bounding_volume::intersection>(4));
    }

    openvrml::coordinate_node * const coordinateNode = this->coord.value
        ? this->coord.value->to_coordinate()
        : 0;
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    openvrml::color_node * const colorNode = this->color_.value
                                           ? this->color_.value->to_color()
                                           : 0;
    const vector<openvrml::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<openvrml::color>();

    openvrml::normal_node * const normalNode = this->normal.value
                                   ? this->normal.value->to_normal()
                                   : 0;
    const vector<vec3f> & normal = normalNode
                                 ? normalNode->vector()
                                 : vector<vec3f>();

    openvrml::texture_coordinate_node * const texCoordNode =
        this->texCoord.value
        ? this->texCoord.value->to_texture_coordinate()
        : 0;
    const vector<vec2f> & texCoord = texCoordNode
                                   ? texCoordNode->point()
                                   : vector<vec2f>();

    unsigned int optMask = 0;
    if (this->ccw.value) {
        optMask |= viewer::mask_ccw;
    }
    if (this->convex.value) {
        optMask |= viewer::mask_convex;
    }
    if (this->solid.value) {
        optMask |= viewer::mask_solid;
    }
    if (this->colorPerVertex.value) {
        optMask |= viewer::mask_color_per_vertex;
    }
    if (this->normalPerVertex.value) {
        optMask |= viewer::mask_normal_per_vertex;
    }

    const viewer::object_t obj =
            viewer.insert_shell(optMask,
                                coord, this->coordIndex.value,
                                color, this->colorIndex.value,
                                normal, this->normalIndex.value,
                                texCoord, this->texCoordIndex.value);

    if (this->color_.value)     { this->color_.value->modified(false); }
    if (this->coord.value)      { this->coord.value->modified(false); }
    if (this->normal.value)     { this->normal.value->modified(false); }
    if (this->texCoord.value)   { this->texCoord.value->modified(false); }

    return obj;
}

/**
 * @brief Recalculate the bounding volume.
 */
void indexed_face_set_node::recalcBSphere()
{
    // take the bvolume of all the points. technically, we should figure
    // out just which points are used by the index and just use those,
    // but for a first pass this is fine (also: if we do it this way
    // then we don't have to update the bvolume when the index
    // changes). motto: always do it the simple way first...
    //
    openvrml::coordinate_node * const coordinateNode = this->coord.value
        ? this->coord.value->to_coordinate()
        : 0;
    if (coordinateNode) {
        const std::vector<vec3f> & coord = coordinateNode->point();
        this->bsphere = bounding_sphere();
        this->bsphere.enclose(coord);
    }
    this->bounding_volume_dirty(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & indexed_face_set_node::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<indexed_face_set_node *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief set_normal eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void indexed_face_set_node::process_set_normal(const field_value & value,
                                               const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->normal = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("normal_changed", this->normal, timestamp);
}

/**
 * @brief set_normalIndex eventIn handler.
 *
 * @param value     an mfint32 value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfint32.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void indexed_face_set_node::process_set_normalIndex(const field_value & value,
                                                    const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->normalIndex = dynamic_cast<const mfint32 &>(value);
    this->node::modified(true);
}

/**
 * @brief set_texCoord eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void indexed_face_set_node::process_set_texCoord(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->texCoord = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("texCoord_changed", this->texCoord, timestamp);
}

/**
 * @brief set_texCoordIndex eventIn handler.
 *
 * @param value     an mfint32 value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfint32.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
indexed_face_set_node::process_set_texCoordIndex(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->texCoordIndex = dynamic_cast<const mfint32 &>(value);
    this->node::modified(true);
}


/**
 * @class indexed_line_set_class
 *
 * @brief Class object for IndexedLineSet nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
indexed_line_set_class::indexed_line_set_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
indexed_line_set_class::~indexed_line_set_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating IndexedLineSet
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by indexed_line_set_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
indexed_line_set_class::create_type(const std::string & id,
                                    const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<indexed_line_set_node>(*this, id));
    vrml97_node_type_impl<indexed_line_set_node> & indexedLineSetNodeType =
        static_cast<vrml97_node_type_impl<indexed_line_set_node> &>(*type);
    typedef vrml97_node_type_impl<indexed_line_set_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            indexedLineSetNodeType
                    .add_eventin(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &indexed_line_set_node::process_set_colorIndex);
        } else if (*interface == supportedInterfaces[1]) {
            indexedLineSetNodeType
                    .add_eventin(supportedInterfaces[1].field_type,
                                supportedInterfaces[1].id,
                                &indexed_line_set_node::process_set_coordIndex);
        } else if (*interface == supportedInterfaces[2]) {
            indexedLineSetNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &indexed_line_set_node::process_set_color,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_line_set_node, sfnode>
                                    (&indexed_line_set_node::color_)));
        } else if (*interface == supportedInterfaces[3]) {
            indexedLineSetNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &indexed_line_set_node::process_set_coord,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_line_set_node, sfnode>
                                    (&indexed_line_set_node::coord)));
        } else if (*interface == supportedInterfaces[4]) {
            indexedLineSetNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_line_set_node, mfint32>
                                    (&indexed_line_set_node::colorIndex)));
        } else if (*interface == supportedInterfaces[5]) {
            indexedLineSetNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_line_set_node, sfbool>
                                    (&indexed_line_set_node::colorPerVertex)));
        } else if (*interface == supportedInterfaces[6]) {
            indexedLineSetNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<indexed_line_set_node, mfint32>
                                    (&indexed_line_set_node::coordIndex)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class indexed_line_set_node
 *
 * @brief Represents IndexedLineSet node instances.
 */

/**
 * @var indexed_line_set_node::indexed_line_set_class
 *
 * @brief Class object for IndexedLineSet nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the scope to which the node belongs.
 */
indexed_line_set_node::indexed_line_set_node(const node_type & type,
                                             const scope_ptr & scope):
    node(type, scope),
    abstract_indexed_set_node(type, scope)
{}

/**
 * @brief Destroy.
 */
indexed_line_set_node::~indexed_line_set_node() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @todo colors
 */
viewer::object_t
indexed_line_set_node::insert_geometry(openvrml::viewer & viewer,
                                       const rendering_context context)
{
    using std::vector;

    openvrml::coordinate_node * const coordinateNode = this->coord.value
        ? this->coord.value->to_coordinate()
        : 0;
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    openvrml::color_node * const colorNode = this->color_.value
                                           ? this->color_.value->to_color()
                                           : 0;
    const vector<openvrml::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<openvrml::color>();

    viewer::object_t obj =
        viewer.insert_line_set(coord, this->coordIndex.value,
                               this->colorPerVertex.value,
                               color, this->colorIndex.value);

    if (this->color_.value) { this->color_.value->modified(false); }
    if (this->coord.value)  { this->coord.value->modified(false); }

    return obj;
}


/**
 * @class inline_class
 *
 * @brief Class object for Inline nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
inline_class::inline_class(openvrml::browser & browser): node_class(browser) {}

/**
 * @brief Destroy.
 */
inline_class::~inline_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Inline nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by inline_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
inline_class::create_type(const std::string & id,
                          const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<inline_node>(*this, id));
    vrml97_node_type_impl<inline_node> & inlineNodeType =
            static_cast<vrml97_node_type_impl<inline_node> &>(*type);
    typedef vrml97_node_type_impl<inline_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            inlineNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &inline_node::process_set_url,
                node_field_ptr_ptr(new node_field_ptr_impl<inline_node, mfstring>
                                    (&inline_node::url)));
        } else if (*interface == supportedInterfaces[1]) {
            inlineNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(new node_field_ptr_impl<inline_node, sfvec3f>
                                    (&inline_node::bboxCenter)));
        } else if (*interface == supportedInterfaces[2]) {
            inlineNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_field_ptr_ptr(new node_field_ptr_impl<inline_node, sfvec3f>
                                    (&inline_node::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class inline_node
 *
 * @brief Represents Inline node instances.
 */

/**
 * @var inline_node::inline_class
 *
 * @brief Class object for Inline nodes.
 */

/**
 * @var sfvec3f inline_node::bboxCenter
 *
 * @brief bboxCenter field.
 */

/**
 * @var sfvec3f inline_node::bboxSize
 *
 * @brief bboxSize field.
 */

/**
 * @var mfstring inline_node::url
 *
 * @brief url exposedField.
 */

/**
 * @var scene * inline_node::inlineScene
 *
 * @brief The contained scene.
 */

/**
 * @var bool inline_node::hasLoaded
 *
 * @brief Flag to indicate whether the scene has been loaded.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
inline_node::inline_node(const node_type & type,
                         const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    inlineScene(0),
    hasLoaded(false)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
inline_node::~inline_node() throw ()
{}

/**
 * @brief Render the node.
 *
 * Render each of the children.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void inline_node::render(openvrml::viewer & viewer,
                         const rendering_context context)
{
    this->load();
    if (this->inlineScene) { this->inlineScene->render(viewer, context); }
}

/**
 * @brief Cast to an inline_node.
 *
 * @return a pointer to the inline_node.
 */
inline_node * inline_node::to_inline() const
{
    return const_cast<inline_node *>(this);
}

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<node_ptr> & inline_node::children() const throw ()
{
    static const std::vector<node_ptr> empty;
    return this->inlineScene
            ? this->inlineScene->nodes()
            : empty;
}

/**
 * Pass on to enabled touchsensor child.
 */
void inline_node::activate(double time, bool isOver, bool isActive, double *p)
{
    const std::vector<node_ptr> & children = this->children();
    for (size_t i = 0; i < children.size(); ++i) {
        const node_ptr & node = children[i];
        if (node) {
            if (node->to_touch_sensor()
                    && node->to_touch_sensor()->enabled()) {
                node->to_touch_sensor()->activate(time, isOver, isActive, p);
            } else if (node->to_plane_sensor()
                    && node->to_plane_sensor()->enabled()) {
                node->to_plane_sensor()->activate(time, isActive, p);
            } else if (node->to_cylinder_sensor()
                    && node->to_cylinder_sensor()->enabled()) {
                node->to_cylinder_sensor()->activate(time, isActive, p);
            } else if (node->to_sphere_sensor()
                    && node->to_sphere_sensor()->isEnabled()) {
                node->to_sphere_sensor()->activate(time, isActive, p);
            }
        }
    }
}

/**
 * @brief Load the children from the URL.
 */
void inline_node::load() {
    //
    // XXX Need to check whether Url has been modified.
    //
    if (this->hasLoaded) { return; }

    this->hasLoaded = true; // although perhaps not successfully
    this->bounding_volume_dirty(true);

    assert(this->scene());
    this->inlineScene = new openvrml::scene(this->scene()->browser,
                                            this->url.value,
                                            this->scene());
    this->inlineScene->initialize(browser::current_time());
}

/**
 * @brief set_url eventIn handler.
 *
 * @param value     an mfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @todo Currently this only changes the field value; it does not load a new
 *      Scene.
 */
void inline_node::process_set_url(const field_value & value,
                                  const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->url = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("url_changed", this->url, timestamp);
}


/**
 * @class lod_class
 *
 * @brief Class object for LOD nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
lod_class::lod_class(openvrml::browser & browser): node_class(browser) {}

/**
 * @brief Destroy.
 */
lod_class::~lod_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating LOD nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by lod_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
lod_class::create_type(const std::string & id,
                       const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<lod_node>(*this, id));
    vrml97_node_type_impl<lod_node> & lodNodeType =
            static_cast<vrml97_node_type_impl<lod_node> &>(*type);
    typedef vrml97_node_type_impl<lod_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            lodNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &lod_node::process_set_level,
                node_field_ptr_ptr(new node_field_ptr_impl<lod_node, mfnode>
                                    (&lod_node::level)));
        } else if (*interface == supportedInterfaces[1]) {
            lodNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(new node_field_ptr_impl<lod_node, sfvec3f>
                                    (&lod_node::center)));
        } else if (*interface == supportedInterfaces[2]) {
            lodNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_field_ptr_ptr(new node_field_ptr_impl<lod_node, mffloat>
                                    (&lod_node::range)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class lod_node
 *
 * @brief Represents LOD node instances.
 */

/**
 * @var lod_node::lod_class
 *
 * @brief Class object for LOD nodes.
 */

/**
 * @var mfnode lod_node::level
 *
 * @brief level exposedField.
 */

/**
 * @var sfvec3f lod_node::center
 *
 * @brief center field.
 */

/**
 * @var mffloat lod_node::range
 *
 * @brief range field.
 */

/**
 * @var mfnode lod_node::children_
 *
 * @brief Caches the active level for fast access by lod_node::children.
 */

/**
 * @var bounding_sphere lod_node::bsphere
 *
 * @brief Cached copy of the bounding_sphere enclosing this node's children.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
lod_node::lod_node(const node_type & type,
                   const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    children_(1)
{
    this->bounding_volume_dirty(true); // lazy calc of bvolume
}

/**
 * @brief Destroy.
 */
lod_node::~lod_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool lod_node::modified() const
{
    if (this->node::modified()) { return true; }

    // This should really check which range is being rendered...
    for (size_t i = 0; i < this->level.value.size(); ++i) {
        if (this->level.value[i]->modified()) { return true; }
    }
    return false;
}

/**
 * @brief Render the node.
 *
 * Render one of the children.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void lod_node::render(openvrml::viewer & viewer,
                      const rendering_context context)
{
    this->node::modified(false);
    if (this->level.value.size() <= 0) { return; }

    float x, y, z;

    mat4f MV = context.matrix();
    MV = MV.inverse();
    x = MV[3][0]; y = MV[3][1]; z = MV[3][2];
    float dx = x - this->center.value.x();
    float dy = y - this->center.value.y();
    float dz = z - this->center.value.z();
    float d2 = dx * dx + dy * dy + dz * dz;

    size_t i;
    for (i = 0; i < this->range.value.size(); ++i) {
        if (d2 < this->range.value[i] * this->range.value[i]) {
            break;
        }
    }

    // Should choose an "optimal" level...
    if (this->range.value.size() == 0) { i = this->level.value.size() - 1; }

    // Not enough levels...
    if (i >= this->level.value.size()) { i = this->level.value.size() - 1; }

    this->level.value[i]->render(viewer, context);

    // Don't re-render on their accounts
    for (i = 0; i < this->level.value.size(); ++i) {
        this->level.value[i]->modified(false);
    }
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & lod_node::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<lod_node *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<node_ptr> & lod_node::children() const throw ()
{
    return this->children_.value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void lod_node::activate(double time, bool isOver, bool isActive, double *p)
{
    const std::vector<node_ptr> & children = this->children();
    const node_ptr & node = children[0];
    if (node) {
        if (node->to_touch_sensor() && node->to_touch_sensor()->enabled()) {
            node->to_touch_sensor()->activate(time, isOver, isActive, p);
        } else if (node->to_plane_sensor()
                && node->to_plane_sensor()->enabled()) {
            node->to_plane_sensor()->activate(time, isActive, p);
        } else if (node->to_cylinder_sensor()
                && node->to_cylinder_sensor()->enabled()) {
            node->to_cylinder_sensor()->activate(time, isActive, p);
        } else if (node->to_sphere_sensor()
                && node->to_sphere_sensor()->isEnabled()) {
            node->to_sphere_sensor()->activate(time, isActive, p);
        }
    }
}

/**
 * @brief Recalculate the bounding volume.
 */
void lod_node::recalcBSphere()
{
    this->bsphere = bounding_sphere();

    // let's say our bsphere is the union of the bspheres of all the
    // levels. we could have said it was just the bsphere of the current
    // level, but the current level depends on the viewer position, and
    // we'd like to make the calculation idependent of that. we could do
    // some sort of trick where we reset the bsphere during render, but
    // that seems like overkill unless this simpler method proves to be
    // a bottleneck.
    //
    // hmm: just thought of a problem: one of the uses of the lod is to
    // switch in delayed-load inlines. this would necessarily switch
    // them in all at once. live with it for now.
    //
    for (size_t i = 0; i < this->level.value.size(); i++) {
        const node_ptr & node = this->level.value[i];
        if (node) {
            const openvrml::bounding_volume & ci_bv = node->bounding_volume();
            this->bsphere.extend(ci_bv);
        }
    }
    this->bounding_volume_dirty(false);
}

/**
 * @brief set_level eventIn handler.
 *
 * @param value     an mfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void lod_node::process_set_level(const field_value & value,
                                 const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->level = dynamic_cast<const mfnode &>(value);
    this->node::modified(true);
    this->emit_event("level_changed", this->level, timestamp);
}


/**
 * @class material_class
 *
 * @brief Class object for Material nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
material_class::material_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
material_class::~material_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Material nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by material_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
material_class::create_type(const std::string & id,
                            const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "ambientIntensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "diffuseColor"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "emissiveColor"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "shininess"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "specularColor"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "transparency")
    };
    const node_type_ptr type(new vrml97_node_type_impl<material_node>(*this, id));
    vrml97_node_type_impl<material_node> & materialNodeType =
            static_cast<vrml97_node_type_impl<material_node> &>(*type);
    typedef vrml97_node_type_impl<material_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &material_node::process_set_ambientIntensity,
                node_field_ptr_ptr(new node_field_ptr_impl<material_node, sffloat>
                                    (&material_node::ambientIntensity)));
        } else if (*interface == supportedInterfaces[1]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &material_node::process_set_diffuseColor,
                node_field_ptr_ptr(new node_field_ptr_impl<material_node, sfcolor>
                                    (&material_node::diffuseColor)));
        } else if (*interface == supportedInterfaces[2]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &material_node::process_set_emissiveColor,
                node_field_ptr_ptr(new node_field_ptr_impl<material_node, sfcolor>
                                    (&material_node::emissiveColor)));
        } else if (*interface == supportedInterfaces[3]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &material_node::process_set_shininess,
                node_field_ptr_ptr(new node_field_ptr_impl<material_node, sffloat>
                                    (&material_node::shininess_)));
        } else if (*interface == supportedInterfaces[4]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &material_node::process_set_specularColor,
                node_field_ptr_ptr(new node_field_ptr_impl<material_node, sfcolor>
                                    (&material_node::specularColor)));
        } else if (*interface == supportedInterfaces[5]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &material_node::process_set_transparency,
                node_field_ptr_ptr(new node_field_ptr_impl<material_node, sffloat>
                                    (&material_node::transparency_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class material_node
 *
 * @brief Material node instances.
 */

/**
 * @var material_node::material_class
 *
 * @brief Class object for Material nodes.
 */

/**
 * @var sffloat material_node::ambientIntensity
 *
 * @brief ambientIntensity exposedField.
 */

/**
 * @var sfcolor material_node::diffuseColor
 *
 * @brief diffuseColor exposedField.
 */

/**
 * @var sfcolor material_node::emissiveColor
 *
 * @brief emissiveColor exposedField.
 */

/**
 * @var sffloat material_node::shininess_
 *
 * @brief shininess exposedField.
 */

/**
 * @var sfcolor material_node::specularColor
 *
 * @brief specularColor exposedField.
 */

/**
 * @var sffloat material_node::transparency_
 *
 * @brief transparency exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
material_node::material_node(const node_type & type,
                             const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::material_node(type, scope),
    ambientIntensity(0.2f),
    diffuseColor(color(0.8f, 0.8f, 0.8f)),
    emissiveColor(color(0.0, 0.0, 0.0)),
    shininess_(0.2f),
    specularColor(color(0.0, 0.0, 0.0)),
    transparency_(0.0)
{}

/**
 * @brief Destroy.
 */
material_node::~material_node() throw ()
{}

/**
 * @brief set_ambientIntensity eventIn handler.
 *
 * @param value     a value from 0.0 to 1.0.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 */
void material_node::process_set_ambientIntensity(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast)
{
    this->ambientIntensity = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("ambientIntensity_changed", this->ambientIntensity,
                    timestamp);
}

/**
 * @brief set_diffuseColor eventIn handler.
 *
 * @param value     an sfcolor value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfcolor.
 */
void material_node::process_set_diffuseColor(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    this->diffuseColor = dynamic_cast<const sfcolor &>(value);
    this->node::modified(true);
    this->emit_event("diffuseColor_changed", this->diffuseColor, timestamp);
}

/**
 * @brief set_emissiveColor eventIn handler.
 *
 * @param value     an sfcolor value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfcolor.
 */
void material_node::process_set_emissiveColor(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast)
{
    this->emissiveColor = dynamic_cast<const sfcolor &>(value);
    this->node::modified(true);
    this->emit_event("emissiveColor_changed", this->emissiveColor, timestamp);
}

/**
 * @brief set_shininess eventIn handler.
 *
 * @param value     a value from 0.0 to 1.0.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 */
void material_node::process_set_shininess(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast)
{
    this->shininess_ = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("shininess_changed", this->shininess_, timestamp);
}

/**
 * @brief set_specularColor eventIn handler.
 *
 * @param value     an sfcolor value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfcolor.
 */
void material_node::process_set_specularColor(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast)
{
    this->specularColor = dynamic_cast<const sfcolor &>(value);
    this->node::modified(true);
    this->emit_event("specularColor_changed", this->specularColor, timestamp);
}

/**
 * @brief set_transparency eventIn handler.
 *
 * @param value     a value from 0.0 to 1.0.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 */
void material_node::process_set_transparency(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    this->transparency_ = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("transparency_changed", this->transparency_, timestamp);
}

/**
 * @brief Get the ambient intensity.
 *
 * @return the ambient intensity.
 */
float material_node::ambient_intensity() const throw ()
{
    return this->ambientIntensity.value;
}

/**
 * @brief Get the diffuse color.
 *
 * @return the diffuse color.
 */
const color & material_node::diffuse_color() const throw ()
{
    return this->diffuseColor.value;
}

/**
 * @brief Get the emissive color.
 *
 * @return the emissive color.
 */
const color & material_node::emissive_color() const throw ()
{
    return this->emissiveColor.value;
}

/**
 * @brief Get the shininess.
 *
 * @return the shininess.
 */
float material_node::shininess() const throw ()
{
    return this->shininess_.value;
}

/**
 * @brief Get the specular color.
 *
 * @return the specular color.
 */
const color & material_node::specular_color() const throw ()
{
    return this->specularColor.value;
}

/**
 * @brief Get the transparency.
 *
 * @return the transparency.
 */
float material_node::transparency() const throw ()
{
    return this->transparency_.value;
}


/**
 * @class movie_texture_class
 *
 * @brief Class object for MovieTexture nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
movie_texture_class::movie_texture_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
movie_texture_class::~movie_texture_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating MovieTexture
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by movie_texture_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
movie_texture_class::create_type(const std::string & id,
                                 const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<movie_texture_node>(*this, id));
    vrml97_node_type_impl<movie_texture_node> & movieTextureNodeType =
            static_cast<vrml97_node_type_impl<movie_texture_node> &>(*type);
    typedef vrml97_node_type_impl<movie_texture_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &movie_texture_node::process_set_loop,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, sfbool>
                                    (&movie_texture_node::loop)));
        } else if (*interface == supportedInterfaces[1]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &movie_texture_node::process_set_speed,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, sffloat>
                                    (&movie_texture_node::speed)));
        } else if (*interface == supportedInterfaces[2]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &movie_texture_node::process_set_startTime,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, sftime>
                                    (&movie_texture_node::startTime)));
        } else if (*interface == supportedInterfaces[3]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &movie_texture_node::process_set_stopTime,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, sftime>
                                    (&movie_texture_node::stopTime)));
        } else if (*interface == supportedInterfaces[4]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &movie_texture_node::process_set_url,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, mfstring>
                                    (&movie_texture_node::url)));
        } else if (*interface == supportedInterfaces[5]) {
            movieTextureNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, sfbool>
                                    (&movie_texture_node::repeatS)));
        } else if (*interface == supportedInterfaces[6]) {
            movieTextureNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, sfbool>
                                    (&movie_texture_node::repeatT)));
        } else if (*interface == supportedInterfaces[7]) {
            movieTextureNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, sftime>
                                    (&movie_texture_node::duration)));
        } else if (*interface == supportedInterfaces[8]) {
            movieTextureNodeType.add_eventout(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_field_ptr_ptr(new node_field_ptr_impl<movie_texture_node, sfbool>
                                    (&movie_texture_node::active)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class movie_texture_node
 *
 * @brief MovieTexture node instances.
 */

/**
 * @var movie_texture_node::movie_texture_class
 *
 * @brief Class object for MovieTexture nodes.
 */

/**
 * @var sfbool movie_texture_node::loop
 *
 * @brief loop exposedField.
 */

/**
 * @var sffloat movie_texture_node::speed
 *
 * @brief speed exposedField.
 */

/**
 * @var sftime movie_texture_node::startTime
 *
 * @brief startTime exposedField.
 */

/**
 * @var sftime movie_texture_node::stopTime
 *
 * @brief stopTime exposedField.
 */

/**
 * @var mfstring movie_texture_node::url
 *
 * @brief url exposedField.
 */

/**
 * @var sftime movie_texture_node::duration
 *
 * @brief duration_changed eventOut.
 */

/**
 * @var sfbool movie_texture_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var img * movie_texture_node::image
 *
 * @brief Image data.
 */

/**
 * @var int movie_texture_node::frame
 *
 * @brief Index of the currently shown frame.
 */

/**
 * @var int movie_texture_node::lastFrame
 *
 * @brief Index of the previously shown frame.
 */

/**
 * @var double movie_texture_node::lastFrameTime
 *
 * @brief Timestamp corresponding to the showing of the previous frame.
 */

/**
 * @var viewer::texture_object_t movie_texture_node::texObject
 *
 * @brief Handle for the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
movie_texture_node::movie_texture_node(const node_type & type,
                                       const scope_ptr & scope):
    node(type, scope),
    abstract_texture_node(type, scope),
    loop(false),
    speed(1.0),
    image(0),
    frame(0),
    lastFrame(-1),
    lastFrameTime(-1.0),
    texObject(0)
{}

/**
 * @brief Destroy.
 */
movie_texture_node::~movie_texture_node() throw ()
{
    delete this->image;
}

/**
 * @brief Cast to a movie_texture_node.
 *
 * @return a pointer to the movie_texture_node.
 */
movie_texture_node* movie_texture_node::to_movie_texture() const
{ return (movie_texture_node*) this; }

/**
 * @brief Update the node for the current timestamp.
 *
 * @param time  the current time.
 */
void movie_texture_node::update(const double time)
{
    if (modified()) {
        if (this->image) {
            const char * imageUrl = this->image->url();
            size_t imageLen = strlen(imageUrl);
            size_t i, nUrls = this->url.value.size();
            for (i = 0; i < nUrls; ++i) {
                size_t len = this->url.value[i].length();

                if (this->url.value[i] == imageUrl
                        || (imageLen > len
                            && this->url.value[i]
                                == (imageUrl + imageLen - len))) {
                    break;
                }
            }

            // if (d_image->url() not in d_url list) ...
            if (i == nUrls) {
                delete this->image;
                this->image = 0;
            }
        }
    }

    // Load the movie if needed (should check startTime...)
    if (!this->image && this->url.value.size() > 0) {
        doc2 baseDoc(this->scene()->url());
        this->image = new img;
        if (!this->image->try_urls(this->url.value, &baseDoc)) {
            std::cerr << "Error: couldn't read MovieTexture from URL "
                      << this->url << std::endl;
        }

        const size_t nFrames = this->image->nframes();
        this->duration.value = (nFrames >= 0)
                             ? double(nFrames)
                             : -1.0;
        this->emit_event("duration_changed", this->duration, time);
        this->frame = (this->speed.value >= 0)
                    ? 0
                    : nFrames - 1;
        // Set the last frame equal to the start time.
        // This is needed to properly handle the case where the startTime
        // and stopTime are set at runtime to the same value (spec says
        // that a single loop should occur in this case...)
        this->lastFrameTime = this->startTime.value;
    }

    // No pictures to show
    if (!this->image || this->image->nframes() == 0) { return; }

    // See section 4.6.9 of the VRML97 spec for a detailed explanation
    // of the logic here.
    if (!this->active.value) {
        if (time >= this->startTime.value) {
            if (time >= this->stopTime.value) {
                if (this->startTime.value >= this->stopTime.value) {
                    if (this->loop.value) {
                        this->active.value = true;
                        this->emit_event("isActive", this->active, time);
                        this->lastFrameTime = time;
                        this->frame = (this->speed.value >= 0)
                                    ? 0
                                    : this->image->nframes() - 1;
                        this->modified(true);
	            } else if (this->startTime.value > this->lastFrameTime) {
                        this->active.value = true;
                        this->emit_event("isActive", this->active, time);
                        this->lastFrameTime = time;
                        this->frame = (this->speed.value >= 0)
                                    ? 0
                                    : this->image->nframes() - 1;
                        this->modified(true);
	            }
	        }
            } else if (this->stopTime.value > time) {
                this->active.value = true;
                this->emit_event("isActive", this->active, time);
                this->lastFrameTime = time;
                this->frame = (this->speed.value >= 0) ? 0 :
                                 this->image->nframes() - 1;
                this->modified(true);
            }
        }
    }
    // Check whether stopTime has passed
    else if (this->active.value
             && ((this->stopTime.value > this->startTime.value
		  && this->stopTime.value <= time))
             || ((this->frame < 0) && !this->loop.value)) {
        this->active.value = false;
        this->emit_event("isActive", this->active, time);
        this->modified(true);
    } else if (this->frame < 0 && this->loop.value) {
        // Reset frame to 0 to begin loop again.
        this->frame = 0;
    }

    // Check whether the frame should be advanced
    else if (this->active.value
             && this->lastFrameTime + fabs(1 / this->speed.value)
                <= time) {
        if (this->speed.value < 0.0) {
            --this->frame;
        } else {
            ++this->frame;
        }

        this->lastFrameTime = time;
        this->modified(true);
    }

    // Tell the scene when the next update is needed.
    if (this->active.value) {
        double d = this->lastFrameTime + fabs(1 / this->speed.value)
                    - time;
        this->type.node_class.browser.delta(0.9 * d);
    }
}

/**
 * @brief Render the node.
 *
 * Render a frame if there is one available.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void movie_texture_node::render(openvrml::viewer & viewer,
                                const rendering_context context)
{
    if (!this->image || this->frame < 0) { return; }

    unsigned char * pix = this->image->pixels(this->frame);

    if (this->frame != this->lastFrame && this->texObject) {
        viewer.remove_texture_object(this->texObject);
        this->texObject = 0;
    }

    if (!pix) {
        this->frame = -1;
    } else if (this->texObject) {
        viewer.insert_texture_reference(this->texObject, this->image->nc());
    } else {
        // Ensure image dimensions are powers of 2 (move to NodeTexture...)
        const size_t sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
        const size_t nSizes = sizeof(sizes) / sizeof(int);
        const size_t w = this->image->w();
        const size_t h = this->image->h();
        size_t i, j;
        for (i = 0; i < nSizes; ++i) { if (w < sizes[i]) { break; } }
        for (j = 0; j < nSizes; ++j) { if (h < sizes[j]) { break; } }

        if (i > 0 && j > 0) {
            // Always scale images down in size and reuse the same pixel memory.
            if (w != sizes[i - 1] || h != sizes[j - 1]) {
                viewer.scale_texture(w, h, sizes[i - 1], sizes[j - 1],
                                     this->image->nc(), pix);
                this->image->resize(sizes[i - 1], sizes[j - 1]);
            }

            this->texObject = viewer.insert_texture(this->image->w(),
                                                    this->image->h(),
                                                    this->image->nc(),
                                                    this->repeatS.value,
                                                    this->repeatT.value,
                                                    pix,
                                                    !this->active.value);
        }
    }

    this->lastFrame = this->frame;
    this->node::modified(false);
}

/**
 * @brief The number of components.
 *
 * @return the number of components.
 */
size_t movie_texture_node::components() const throw ()
{
    return this->image ? this->image->nc() : 0;
}

/**
 * @brief The width in pixels.
 *
 * @return the width in pixels.
 */
size_t movie_texture_node::width() const throw ()
{
    return this->image ? this->image->w() : 0;
}

/**
 * @brief The height in pixels.
 *
 * @return the height in pixels.
 */
size_t movie_texture_node::height() const throw ()
{
    return this->image ? this->image->h() : 0;
}

/**
 * @brief The number of frames.
 *
 * @return the number of frames.
 */
size_t movie_texture_node::frames() const throw ()
{
    return this->image ? this->image->nframes() : 0;
}

/**
 * @brief Pixel data for the current frame.
 *
 * @return pixel data for the current frame.
 */
const unsigned char * movie_texture_node::pixels() const throw ()
{
    return this->image ? this->image->pixels() : 0;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void movie_texture_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_movie(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void movie_texture_node::do_shutdown(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.remove_movie(*this);
}

/**
 * @brief set_loop eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void movie_texture_node::process_set_loop(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast)
{
    this->loop = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("loop_changed", this->loop, timestamp);
}

/**
 * @brief set_speed eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void movie_texture_node::process_set_speed(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast)
{
    //
    // set_speed is ignored if the MovieTexture is active.
    //
    if (!this->active.value) {
        this->speed = dynamic_cast<const sffloat &>(value);
        this->node::modified(true);
        this->emit_event("speed_changed", this->speed, timestamp);
    }
}

/**
 * @brief set_startTime eventIn handler.
 *
 * @param value     an sftime value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sftime.
 */
void movie_texture_node::process_set_startTime(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast)
{
    this->startTime = dynamic_cast<const sftime &>(value);
    this->node::modified(true);
    this->emit_event("startTime_changed", this->startTime, timestamp);
}

/**
 * @brief set_stopTime eventIn handler.
 *
 * @param value     an sftime value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sftime.
 */
void movie_texture_node::process_set_stopTime(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast)
{
    this->stopTime = dynamic_cast<const sftime &>(value);
    this->node::modified(true);
    this->emit_event("stopTime_changed", this->stopTime, timestamp);
}

/**
 * @brief set_url eventIn handler.
 *
 * @param value     an mfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void movie_texture_node::process_set_url(const field_value & value,
                                         const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->url = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("url_changed", this->url, timestamp);
}


/**
 * @class navigation_info_class
 *
 * @brief Class object for NavigationInfo nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
navigation_info_class::navigation_info_class(openvrml::browser & browser):
        node_class(browser) {}

/**
 * @brief Destroy.
 */
navigation_info_class::~navigation_info_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating NavigationInfo
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by navigation_info_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
navigation_info_class::create_type(const std::string & id,
                                   const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sfbool_id, "set_bind"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "avatarSize"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "headlight"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "speed"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "type"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "visibilityLimit"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isBound")
    };
    const node_type_ptr type(new vrml97_node_type_impl<navigation_info_node>(*this, id));
    vrml97_node_type_impl<navigation_info_node> & navigationInfoNodeType =
            static_cast<vrml97_node_type_impl<navigation_info_node> &>(*type);
    typedef vrml97_node_type_impl<navigation_info_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            navigationInfoNodeType.add_eventin(supportedInterfaces[0].field_type,
                                   supportedInterfaces[0].id,
                                   &navigation_info_node::process_set_bind);
        } else if (*interface == supportedInterfaces[1]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &navigation_info_node::process_set_avatarSize,
                node_field_ptr_ptr(new node_field_ptr_impl<navigation_info_node, mffloat>
                                    (&navigation_info_node::avatarSize)));
        } else if (*interface == supportedInterfaces[2]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &navigation_info_node::process_set_headlight,
                node_field_ptr_ptr(new node_field_ptr_impl<navigation_info_node, sfbool>
                                    (&navigation_info_node::headlight_)));
        } else if (*interface == supportedInterfaces[3]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &navigation_info_node::process_set_speed,
                node_field_ptr_ptr(new node_field_ptr_impl<navigation_info_node, sffloat>
                                    (&navigation_info_node::speed_)));
        } else if (*interface == supportedInterfaces[4]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &navigation_info_node::process_set_type,
                node_field_ptr_ptr(new node_field_ptr_impl<navigation_info_node, mfstring>
                                    (&navigation_info_node::type)));
        } else if (*interface == supportedInterfaces[5]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &navigation_info_node::process_set_visibilityLimit,
                node_field_ptr_ptr(new node_field_ptr_impl<navigation_info_node, sffloat>
                                    (&navigation_info_node::visibilityLimit)));
        } else if (*interface == supportedInterfaces[6]) {
            navigationInfoNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<navigation_info_node, sfbool>
                                    (&navigation_info_node::bound)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

namespace {
    const float avatarSize_[] = { 0.25f, 1.6f, 0.75f };
    const std::string type_[] = { "WALK", "ANY" };
}

/**
 * @class navigation_info_node
 *
 * @brief NavigationInfo node instances.
 */

/**
 * @var navigation_info_node::navigation_info_class
 *
 * @brief Class object for NavigationInfo nodes.
 */

/**
 * @var mffloat navigation_info_node::avatarSize
 *
 * @brief avatarSize exposedField.
 */

/**
 * @var sfbool navigation_info_node::headlight_
 *
 * @brief headlight exposedField.
 */

/**
 * @var sffloat navigation_info_node::speed_
 *
 * @brief speed exposedField.
 */

/**
 * @var mfstring navigation_info_node::type
 *
 * @brief type exposedField.
 */

/**
 * @var sffloat navigation_info_node::visibilityLimit
 *
 * @brief visibilityLimit exposedField.
 */

/**
 * @var sfbool navigation_info_node::bound
 *
 * @brief isBound eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
navigation_info_node::navigation_info_node(const node_type & type,
                                           const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    avatarSize(avatarSize_, avatarSize_ + 3),
    headlight_(true),
    speed_(1.0),
    type(type_, type_ + 2),
    visibilityLimit(0.0),
    bound(false)
{}

/**
 * @brief Destroy.
 */
navigation_info_node::~navigation_info_node() throw ()
{}

/**
 * @brief Cast to a navigation_info_node.
 *
 * @return A pointer to the navigation_info_node.
 */
navigation_info_node* navigation_info_node::to_navigation_info() const
{ return (navigation_info_node*) this; }

/**
 * @brief The avatar size.
 *
 * @return the avatar size.
 */
const float * navigation_info_node::avatar_size() const
{
    return !this->avatarSize.value.empty()
        ? &this->avatarSize.value[0]
        : 0;
}

/**
 * @brief The headlight state.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */
bool navigation_info_node::headlight() const
{
    return this->headlight_.value;
}

/**
 * @brief The speed.
 *
 * @return the speed.
 */
float navigation_info_node::speed() const
{
    return this->speed_.value;
}

/**
 * @brief The visibility limit.
 *
 * @return the visibility limit.
 */
float navigation_info_node::visibility_limit() const
{
    return this->visibilityLimit.value;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void navigation_info_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_navigation_info(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void navigation_info_node::do_shutdown(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.remove_navigation_info(*this);
}

/**
 * @brief set_avatarSize eventIn handler.
 *
 * @param value     an mffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void navigation_info_node::process_set_avatarSize(const field_value & value,
                                                  const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->avatarSize = dynamic_cast<const mffloat &>(value);
    this->node::modified(true);
    this->emit_event("avatarSize_changed", this->avatarSize, timestamp);
}

/**
 * @brief set_bind eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfbool.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void navigation_info_node::process_set_bind(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    navigation_info_node * current =
        this->node::type.node_class.browser.bindable_navigation_info_top();
    const sfbool & b = dynamic_cast<const sfbool &>(value);

    if (b.value) {        // set_bind TRUE
        if (this != current) {
            if (current) {
                current->bound.value = false;
                current->emit_event("isBound", current->bound, timestamp);
            }
            this->node::type.node_class.browser.bindable_push(this);
            this->bound.value = true;
            this->emit_event("isBound", this->bound, timestamp);
        }
    } else {            // set_bind FALSE
        this->node::type.node_class.browser.bindable_remove(this);
        if (this == current) {
            this->bound.value = false;
            this->emit_event("isBound", this->bound, timestamp);
            current = this->node::type.node_class.browser
                        .bindable_navigation_info_top();
            if (current) {
                current->bound.value = true;
                current->emit_event("isBound", current->bound, timestamp);
            }
        }
    }
}

/**
 * @brief set_headlight eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfbool.
 */
void navigation_info_node::process_set_headlight(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast)
{
    this->headlight_ = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("headlight_changed", this->headlight_, timestamp);
}

/**
 * @brief set_speed eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 */
void navigation_info_node::process_set_speed(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    this->speed_ = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("speed_changed", this->speed_, timestamp);
}

/**
 * @brief set_type eventIn handler.
 *
 * @param value     an mfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void navigation_info_node::process_set_type(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->type = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("type_changed", this->type, timestamp);
}

/**
 * @brief set_visibilityLimit eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 */
void
navigation_info_node::process_set_visibilityLimit(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast)
{
    this->visibilityLimit = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("visibilityLimit_changed", this->visibilityLimit,
                    timestamp);
}


/**
 * @class normal_class
 *
 * @brief Class object for Normal nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
normal_class::normal_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
normal_class::~normal_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Normal nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by normal_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
normal_class::create_type(const std::string & id,
                          const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "vector");
    const node_type_ptr type(new vrml97_node_type_impl<normal_node>(*this, id));
    vrml97_node_type_impl<normal_node> & normalNodeType =
            static_cast<vrml97_node_type_impl<normal_node> &>(*type);
    typedef vrml97_node_type_impl<normal_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterface) {
            normalNodeType.add_exposedfield(
                supportedInterface.field_type,
                supportedInterface.id,
                &normal_node::process_set_vector,
                node_field_ptr_ptr(new node_field_ptr_impl<normal_node, mfvec3f>
                                    (&normal_node::vector_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class normal_node
 *
 * @brief Normal node instances.
 */

/**
 * @var normal_node::normal_class
 *
 * @brief Class object for Normal nodes.
 */

/**
 * @var mfvec3f normal_node::vector_
 *
 * @brief vector exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
normal_node::normal_node(const node_type & type,
                         const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::normal_node(type, scope)
{}

/**
 * @brief Destroy.
 */
normal_node::~normal_node() throw () {}

/**
 * @brief Get the array of normal vectors.
 *
 * @return the array of normal vectors.
 */
const std::vector<vec3f> & normal_node::vector() const throw ()
{
    return this->vector_.value;
}

/**
 * @brief set_vector eventIn handler.
 *
 * @param value     an mfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec3f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void normal_node::process_set_vector(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->vector_ = dynamic_cast<const mfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("vector_changed", this->vector_, timestamp);
}


/**
 * @class normal_interpolator_class
 *
 * @brief Class object for NormalInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
normal_interpolator_class::normal_interpolator_class(
    openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
normal_interpolator_class::~normal_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *         NormalInterpolator nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by normal_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
normal_interpolator_class::create_type(const std::string & id,
                                       const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sffloat_id, "set_fraction"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "key"),
        node_interface(node_interface::exposedfield_id, field_value::mfvec3f_id, "keyValue"),
        node_interface(node_interface::eventout_id, field_value::mfvec3f_id, "value_changed")
    };
    const node_type_ptr type(new vrml97_node_type_impl<normal_interpolator_node>(*this, id));
    vrml97_node_type_impl<normal_interpolator_node> & normalInterpolatorNodeType =
            static_cast<vrml97_node_type_impl<normal_interpolator_node> &>(*type);
    typedef vrml97_node_type_impl<normal_interpolator_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            normalInterpolatorNodeType
                    .add_eventin(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &normal_interpolator_node::process_set_fraction);
        } else if (*interface == supportedInterfaces[1]) {
            normalInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &normal_interpolator_node::process_set_key,
                node_field_ptr_ptr(new node_field_ptr_impl<normal_interpolator_node, mffloat>
                                    (&normal_interpolator_node::key)));
        } else if (*interface == supportedInterfaces[2]) {
            normalInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &normal_interpolator_node::process_set_keyValue,
                node_field_ptr_ptr(new node_field_ptr_impl<normal_interpolator_node, mfvec3f>
                                    (&normal_interpolator_node::keyValue)));
        } else if (*interface == supportedInterfaces[3]) {
            normalInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<normal_interpolator_node, mfvec3f>
                                    (&normal_interpolator_node::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class normal_interpolator_node
 *
 * @brief NormalInterpolator node instances.
 */

/**
 * @var normal_interpolator_node::normal_interpolator_class
 *
 * @brief Class object for NormalInterpolator nodes.
 */

/**
 * @var mffloat normal_interpolator_node::key
 *
 * @brief key exposedField.
 */

/**
 * @var mfvec3f normal_interpolator_node::keyValue
 *
 * @brief keyValue exposedField.
 */

/**
 * @var mfvec3f normal_interpolator_node::value
 *
 * @brief value_changed eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
normal_interpolator_node::normal_interpolator_node(const node_type & type,
                                       const scope_ptr & scope):
        node(type, scope),
        abstract_child_node(type, scope) {}

/**
 * @brief Destroy.
 */
normal_interpolator_node::~normal_interpolator_node() throw () {}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void normal_interpolator_node::process_set_fraction(const field_value & value,
                                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    float f = dynamic_cast<const sffloat &>(value).value;

    size_t nNormals = this->keyValue.value.size() / this->key.value.size();
    size_t n = this->key.value.size() - 1;

    if (f < this->key.value[0]) {
        this->value.value.assign(this->keyValue.value.begin(),
                                 this->keyValue.value.begin() + nNormals);
    } else if (f > this->key.value[n]) {
        this->value.value
                .assign(this->keyValue.value.begin() + n * nNormals,
                        this->keyValue.value.begin() + (n + 1) * nNormals);
    } else {
        // Reserve enough space for the new value
        this->value.value.resize(nNormals);

        for (size_t i = 0; i < n; ++i) {
            if (this->key.value[i] <= f && f <= this->key.value[i + 1]) {
                using std::vector;

                vector<vec3f>::const_iterator v1 =
                    this->keyValue.value.begin() + i * nNormals;
                vector<vec3f>::const_iterator v2 =
                    this->keyValue.value.begin() + (i + 1) * nNormals;

                f = (f - this->key.value[i])
                    / (this->key.value[i + 1] - this->key.value[i]);

                // Interpolate on the surface of unit sphere.
                for (size_t j = 0; j < nNormals; ++j) {
                    using openvrml_::fequal;

                    float alpha, beta;
                    const float dot_product = v1->dot(*v2);
                    if (!fequal<float>()(dot_product, 1.0f)
                            && v1->normalize() != v2->normalize()) {
                        // Vectors are not opposite and not coincident.
                        const float omega = float(acos(dot_product));
                        const float sinomega = float(sin(omega));
                        alpha = float(sin((1.0 - f) * omega)) / sinomega;
                        beta = float(sin(f * omega)) / sinomega;
                    } else {
                        // Do linear interpolation.
                        alpha = 1.0f - f;
                        beta = f;
                    }
                    const vec3f vec(alpha * v1->x() + beta * v2->x(),
                                    alpha * v1->y() + beta * v2->y(),
                                    alpha * v1->z() + beta * v2->z());
                    this->value.value[j] = vec;

                    ++v1;
                    ++v2;
                }

                break;
            }
        }
    }

    // Send the new value
    this->emit_event("value_changed", this->value, timestamp);
}

/**
 * @brief set_key eventIn handler.
 *
 * @param value     an mffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void normal_interpolator_node::process_set_key(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->key = dynamic_cast<const mffloat &>(value);
    this->emit_event("key_changed", this->key, timestamp);
}

/**
 * @brief set_keyValue eventIn handler.
 *
 * @param value     an mfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec3f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void normal_interpolator_node::process_set_keyValue(const field_value & value,
                                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mfvec3f &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class orientation_interpolator_class
 *
 * @brief Class object for OrientationInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
orientation_interpolator_class::orientation_interpolator_class(
    openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
orientation_interpolator_class::~orientation_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *      OrientationInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by orientation_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
orientation_interpolator_class::create_type(
    const std::string & id,
    const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<orientation_interpolator_node>(*this, id));
    vrml97_node_type_impl<orientation_interpolator_node> &
        orientationInterpolatorNodeType =
        static_cast<vrml97_node_type_impl<orientation_interpolator_node> &>(*type);
    typedef vrml97_node_type_impl<orientation_interpolator_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            orientationInterpolatorNodeType
                    .add_eventin(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &orientation_interpolator_node::process_set_fraction);
        } else if (*interface == supportedInterfaces[1]) {
            orientationInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &orientation_interpolator_node::process_set_key,
                node_field_ptr_ptr(new node_field_ptr_impl<orientation_interpolator_node,
                                                     mffloat>
                                (&orientation_interpolator_node::key)));
        } else if (*interface == supportedInterfaces[2]) {
            orientationInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &orientation_interpolator_node::process_set_keyValue,
                node_field_ptr_ptr(new node_field_ptr_impl<orientation_interpolator_node,
                                                     mfrotation>
                                (&orientation_interpolator_node::keyValue)));
        } else if (*interface == supportedInterfaces[3]) {
            orientationInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<orientation_interpolator_node,
                                                     sfrotation>
                                (&orientation_interpolator_node::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class orientation_interpolator_node
 *
 * @brief OrientationInterpolator node instances.
 */

/**
 * @var orientation_interpolator_node::orientation_interpolator_class
 *
 * @brief Class object for OrientationInterpolator nodes.
 */

/**
 * @var mffloat orientation_interpolator_node::key
 *
 * @brief key exposedField.
 */

/**
 * @var mfrotation orientation_interpolator_node::keyValue
 *
 * @brief keyValue exposedField.
 */

/**
 * @var sfrotation orientation_interpolator_node::value
 *
 * @brief value_changed eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
orientation_interpolator_node::orientation_interpolator_node(
    const node_type & type,
    const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
orientation_interpolator_node::~orientation_interpolator_node() throw ()
{}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
orientation_interpolator_node::process_set_fraction(const field_value & value,
                                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    float f = dynamic_cast<const sffloat &>(value).value;

    size_t n = this->key.value.size() - 1;
    if (f < this->key.value[0]) {
        this->value.value = this->keyValue.value[0];
    } else if (f > this->key.value[n]) {
        this->value.value = this->keyValue.value[n];
    } else {
        for (size_t i = 0; i < n; ++i) {
            if (this->key.value[i] <= f && f <= this->key.value[i + 1]) {
                const rotation & v1 = this->keyValue.value[i];
                const rotation & v2 = this->keyValue.value[i + 1];

                // Interpolation factor
                f = (f - this->key.value[i])
                    / (this->key.value[i + 1] - this->key.value[i]);

                float x, y, z, r1, r2;
                r1 = v1[3];

                // Make sure the vectors are not pointing opposite ways
                if (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] < 0.0) {
                    x = v1[0] + f * (-v2[0] - v1[0]);
                    y = v1[1] + f * (-v2[1] - v1[1]);
                    z = v1[2] + f * (-v2[2] - v1[2]);
                    r2 = -v2[3];
                } else {
                    x = v1[0] + f * (v2[0] - v1[0]);
                    y = v1[1] + f * (v2[1] - v1[1]);
                    z = v1[2] + f * (v2[2] - v1[2]);
                    r2 = v2[3];
                }

                // Interpolate angles via the shortest direction
                if (fabs(r2 - r1) > pi) {
                    if (r2 > r1) {
                        r1 += float(2.0 * pi);
                    } else {
                        r2 += float(2.0 * pi);
                    }
                }
                float angle = r1 + f * (r2 - r1);
                if (angle >= 2.0 * pi) {
                    angle -= float(2.0 * pi);
                } else if (angle < 0.0) {
                    angle += float(2.0 * pi);
                }
                vec3f Vec(x,y,z);
                Vec = Vec.normalize();
                this->value.value.axis(Vec);
                this->value.value.angle(angle);
                break;
            }
        }
    }

    // Send the new value
    this->emit_event("value_changed", this->value, timestamp);
}

/**
 * @brief set_key eventIn handler.
 *
 * @param value     an mffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void orientation_interpolator_node::process_set_key(const field_value & value,
                                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->key = dynamic_cast<const mffloat &>(value);
    this->emit_event("key_changed", this->key, timestamp);
}

/**
 * @brief set_keyValue eventIn handler.
 *
 * @param value     an mfrotation value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfrotation.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
orientation_interpolator_node::process_set_keyValue(const field_value & value,
                                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mfrotation &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class pixel_texture_class
 *
 * @brief Class object for PixelTexture nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
pixel_texture_class::pixel_texture_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
pixel_texture_class::~pixel_texture_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating PixelTexture
 *         nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by pixel_texture_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
pixel_texture_class::create_type(const std::string & id,
                                 const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef vrml97_node_type_impl<pixel_texture_node> node_type_t;

    const node_type_ptr type(new node_type_t(*this, id));
    node_type_t & pixelTextureNodeType = static_cast<node_type_t &>(*type);
    typedef node_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            pixelTextureNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &pixel_texture_node::process_set_image,
                node_field_ptr_ptr(new node_field_ptr_impl<pixel_texture_node, sfimage>
                                    (&pixel_texture_node::image)));
        } else if (*interface == supportedInterfaces[1]) {
            pixelTextureNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(new node_field_ptr_impl<pixel_texture_node, sfbool>
                                    (&pixel_texture_node::repeatS)));
        } else if (*interface == supportedInterfaces[2]) {
            pixelTextureNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_field_ptr_ptr(new node_field_ptr_impl<pixel_texture_node, sfbool>
                                    (&pixel_texture_node::repeatT)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class pixel_texture_node
 *
 * @brief Represents PixelTexture node instances.
 */

/**
 * @var pixel_texture_node::pixel_texture_class
 *
 * @brief Class object for PixelTexture nodes.
 */

/**
 * @var sfimage pixel_texture_node::image
 *
 * @brief image exposedField.
 */

/**
 * @var viewer::texture_object_t pixel_texture_node::texObject
 *
 * @brief Handle for the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the scope to which the node belongs.
 */
pixel_texture_node::pixel_texture_node(const node_type & type,
                                       const scope_ptr & scope):
    node(type, scope),
    abstract_texture_node(type, scope),
    texObject(0) {}

/**
 * @brief Destroy.
 */
pixel_texture_node::~pixel_texture_node() throw ()
{
    // viewer.remove_texture_object(this->texObject); ...
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 *
 * @todo Currently we modify the image when rescaling it to have
 *       2<sup><var>n</var></sup> sides. This is wrong. We should maintain a
 *       cache of rescaled images in the renderer.
 */
void pixel_texture_node::render(openvrml::viewer & viewer,
                                const rendering_context context)
{
    if (modified()) {
        if (this->texObject) {
            viewer.remove_texture_object(this->texObject);
            this->texObject = 0;
        }
    }

    if (this->image.array()) {
        if (this->texObject) {
            viewer.insert_texture_reference(this->texObject,
                                            this->image.comp());
        } else {
            // Ensure the image dimensions are powers of two
            static const size_t sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
            static const size_t nSizes = sizeof sizes / sizeof(int);
            const size_t w = this->image.x();
            const size_t h = this->image.y();
            size_t i, j;
            for (i = 0; i < nSizes; ++i) { if (w < sizes[i]) { break; } }
            for (j = 0; j < nSizes; ++j) { if (h < sizes[j]) { break; } }

            if (i > 0 && j > 0) {
                // Always scale images down in size and reuse the same pixel
                // memory.
                if (w != sizes[i - 1] || h != sizes[j - 1]) {
                    using std::vector;
                    using std::copy;

                    const size_t numBytes =
                        this->image.x() * this->image.y() * this->image.comp();
                    vector<unsigned char> pixels(numBytes);
                    copy(this->image.array(), this->image.array() + numBytes,
                         pixels.begin());

                    viewer.scale_texture(w, h, sizes[i - 1], sizes[j - 1],
                                         this->image.comp(), &pixels[0]);
                    this->image.set(sizes[i - 1], sizes[j - 1],
                                    this->image.comp(), &pixels[0]);
                }

                this->texObject = viewer.insert_texture(this->image.x(),
                                                        this->image.y(),
                                                        this->image.comp(),
                                                        this->repeatS.value,
                                                        this->repeatT.value,
                                                        this->image.array(),
                                                        true);
            }
        }
    }
    this->node::modified(false);
}

/**
 * @brief The number of components in the image.
 *
 * @return the number of components in the image.
 */
size_t pixel_texture_node::components() const throw ()
{
    return this->image.comp();
}

/**
 * @brief The width of the image in pixels.
 *
 * @return the width of the image in pixels.
 */
size_t pixel_texture_node::width() const throw ()
{
    return this->image.x();
}

/**
 * @brief The height of the image in pixels.
 *
 * @return the height of the image in pixels.
 */
size_t pixel_texture_node::height() const throw ()
{
    return this->image.y();
}

/**
 * @brief The number of frames.
 *
 * @return 0
 */
size_t pixel_texture_node::frames() const throw ()
{
    return 0;
}

/**
 * @brief The pixel data.
 *
 * @return the pixel data.
 */
const unsigned char * pixel_texture_node::pixels() const throw ()
{
    return this->image.array();
}

/**
 * @brief set_image eventIn handler.
 *
 * @param value     an sfimage value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an SFImage.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void pixel_texture_node::process_set_image(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->image = dynamic_cast<const sfimage &>(value);
    this->node::modified(true);
    this->emit_event("image_changed", this->image, timestamp);
}


/**
 * @class plane_sensor_class
 *
 * @brief Class object for PlaneSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
plane_sensor_class::plane_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
plane_sensor_class::~plane_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating PlaneSensor
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by plane_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
plane_sensor_class::create_type(const std::string & id,
                                const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "autoOffset"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "maxPosition"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "minPosition"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "offset"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive"),
        node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "trackPoint_changed"),
        node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "translation_changed")
    };
    const node_type_ptr type(new vrml97_node_type_impl<plane_sensor_node>(*this, id));
    vrml97_node_type_impl<plane_sensor_node> & planeSensorNodeType =
            static_cast<vrml97_node_type_impl<plane_sensor_node> &>(*type);
    typedef vrml97_node_type_impl<plane_sensor_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &plane_sensor_node::process_set_autoOffset,
                node_field_ptr_ptr(new node_field_ptr_impl<plane_sensor_node, sfbool>
                                    (&plane_sensor_node::autoOffset)));
        } else if (*interface == supportedInterfaces[1]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &plane_sensor_node::process_set_enabled,
                node_field_ptr_ptr(new node_field_ptr_impl<plane_sensor_node, sfbool>
                                    (&plane_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[2]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &plane_sensor_node::process_set_maxPosition,
                node_field_ptr_ptr(new node_field_ptr_impl<plane_sensor_node, sfvec2f>
                                    (&plane_sensor_node::maxPosition)));
        } else if (*interface == supportedInterfaces[3]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &plane_sensor_node::process_set_minPosition,
                node_field_ptr_ptr(new node_field_ptr_impl<plane_sensor_node, sfvec2f>
                                    (&plane_sensor_node::minPosition)));
        } else if (*interface == supportedInterfaces[4]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &plane_sensor_node::process_set_offset,
                node_field_ptr_ptr(new node_field_ptr_impl<plane_sensor_node, sfvec3f>
                                    (&plane_sensor_node::offset)));
        } else if (*interface == supportedInterfaces[5]) {
            planeSensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<plane_sensor_node, sfbool>
                                    (&plane_sensor_node::active)));
        } else if (*interface == supportedInterfaces[6]) {
            planeSensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<plane_sensor_node, sfvec3f>
                                    (&plane_sensor_node::trackPoint)));
        } else if (*interface == supportedInterfaces[7]) {
            planeSensorNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(new node_field_ptr_impl<plane_sensor_node, sfvec3f>
                                    (&plane_sensor_node::translation)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class plane_sensor_node
 *
 * The PlaneSensor node maps pointing device motion into two-dimensional
 * translation in a plane parallel to the <var>z</var>&nbsp;=&nbsp;0 plane of
 * the local coordinate system. The PlaneSensor node uses the descendent
 * geometry of its parent node to determine whether it is liable to generate
 * events.
 */

/**
 * @var plane_sensor_node::plane_sensor_class
 *
 * @brief Class object for PlaneSensor instances.
 */

/**
 * @var sfbool plane_sensor_node::autoOffset
 *
 * @brief autoOffset exposedField.
 */

/**
 * @var sfbool plane_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfvec2f plane_sensor_node::maxPosition
 *
 * @brief maxPosition exposedField.
 */

/**
 * @var sfvec2f plane_sensor_node::minPosition
 *
 * @brief minPosition exposedField.
 */

/**
 * @var sfvec3f plane_sensor_node::offset
 *
 * @brief offset exposedField.
 */

/**
 * @var sfbool plane_sensor_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfvec3f plane_sensor_node::trackPoint
 *
 * @brief trackPoint_changed eventOut.
 */

/**
 * @var sfvec3f plane_sensor_node::translation
 *
 * @brief translation_changed eventOut.
 */

/**
 * @var sfvec3f plane_sensor_node::activationPoint
 *
 * @brief The point at which the PlaneSensor was activated.
 */

/**
 * @var mat4f plane_sensor_node::activationMatrix
 *
 * @brief Activation matrix.
 */

/**
 * @var mat4f plane_sensor_node::modelview
 *
 * @brief The modelview matrix.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
plane_sensor_node::plane_sensor_node(const node_type & type,
                         const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    autoOffset(true),
    enabled_(true),
    maxPosition(vec2f(-1.0, -1.0)),
    minPosition(vec2f(0.0, 0.0)),
    offset(vec3f(0.0, 0.0, 0.0)),
    active(false)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
plane_sensor_node::~plane_sensor_node() throw ()
{}

/**
 * @brief Cast to a plane_sensor_node.
 *
 * @return a pointer to the plane_sensor_node.
 */
plane_sensor_node * plane_sensor_node::to_plane_sensor() const
{
    return (plane_sensor_node*) this;
}

/**
 * @brief Render the node.
 *
 * Render a frame if there is one available.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void plane_sensor_node::render(openvrml::viewer & viewer,
                               const rendering_context context)
{
    //
    // Store the ModelView matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation.
    //
    this->modelview = context.matrix();
}

/**
 * @todo The local coords are computed for one instance; do we need to convert
 *       @p p to local coords for each instance (USE) of the sensor?
 */
void plane_sensor_node::activate(double timeStamp, bool isActive, double * p)
{
    // Become active
    if (isActive && !this->active.value) {
        this->active.value = isActive;

        vec3f V(static_cast<float>(p[0]),
                static_cast<float>(p[1]),
                static_cast<float>(p[2]));
        this->activationMatrix = this->modelview.inverse();
        V *= this->activationMatrix;
        this->activationPoint.value = V;
        this->emit_event("isActive", this->active, timeStamp);
    }

    // Become inactive
    else if (!isActive && this->active.value) {
        this->active.value = isActive;
        this->emit_event("isActive", this->active, timeStamp);

        // auto offset
        if (this->autoOffset.value) {
            this->offset = this->translation;
            this->emit_event("offset_changed", this->offset, timeStamp);
        }
    }

    // Tracking
    else if (isActive) {
        vec3f V(static_cast<float>(p[0]),
                static_cast<float>(p[1]),
                static_cast<float>(p[2]));
        V *= this->activationMatrix;
        this->trackPoint.value = V;
        this->emit_event("trackPoint_changed", this->trackPoint, timeStamp);

        vec3f t(V[0] - this->activationPoint.value.x() + this->offset.value.x(),
                V[1] - this->activationPoint.value.y() + this->offset.value.y(),
                0.0);

        if (this->minPosition.value.x() == this->maxPosition.value.x() ) {
            t.x(this->minPosition.value.x());
        } else if (this->minPosition.value.x() < this->maxPosition.value.x()) {
            if (t.x() < this->minPosition.value.x()) {
                t.x(this->minPosition.value.x());
            } else if (t.x() > this->maxPosition.value.x()) {
                t.x(this->maxPosition.value.x());
            }
        }

        if (this->minPosition.value.y() == this->maxPosition.value.y()) {
            t.y(this->minPosition.value.y());
        } else if (this->minPosition.value.y() < this->maxPosition.value.y()) {
            if (t.y() < this->minPosition.value.y()) {
                t.y(this->minPosition.value.y());
            } else if (t.y() > this->maxPosition.value.y()) {
                t.y(this->maxPosition.value.y());
            }
        }

        this->translation.value = t;
        this->emit_event("translation_changed", this->translation, timeStamp);
    }
}

/**
 * @brief Return whether the PlaneSensor is enabled.
 *
 * @return @c true if the PlaneSensor is enabled, @c false otherwise.
 */
bool plane_sensor_node::enabled() const
{
    return this->enabled_.value;
}

/**
 * @brief set_autoOffset eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void plane_sensor_node::process_set_autoOffset(const field_value & value,
                                        const double timestamp)
    throw (std::bad_cast)
{
    this->autoOffset = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("autoOffset_changed", this->autoOffset, timestamp);
}

/**
 * @brief set_enabled eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void plane_sensor_node::process_set_enabled(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->enabled_ = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("enabled_changed", this->enabled_, timestamp);
}

/**
 * @brief set_maxPosition eventIn handler.
 *
 * @param value     an sfvec2f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec2f.
 */
void plane_sensor_node::process_set_maxPosition(const field_value & value,
                                         const double timestamp)
    throw (std::bad_cast)
{
    this->maxPosition = dynamic_cast<const sfvec2f &>(value);
    this->node::modified(true);
    this->emit_event("maxPosition_changed", this->maxPosition, timestamp);
}

/**
 * @brief set_minPosition eventIn handler.
 *
 * @param value     an sfvec2f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec2f.
 */
void plane_sensor_node::process_set_minPosition(const field_value & value,
                                         const double timestamp)
        throw (std::bad_cast) {
    this->minPosition = dynamic_cast<const sfvec2f &>(value);
    this->node::modified(true);
    this->emit_event("minPosition_changed", this->minPosition, timestamp);
}

/**
 * @brief set_offset eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void plane_sensor_node::process_set_offset(const field_value & value,
                                    const double timestamp)
        throw (std::bad_cast) {
    this->offset = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("offset_changed", this->offset, timestamp);
}


/**
 * @class point_light_class
 *
 * @brief Class object for PointLight nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
point_light_class::point_light_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
point_light_class::~point_light_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating PointLight nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by point_light_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
point_light_class::create_type(const std::string & id,
                               const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "ambientIntensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "attenuation"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "intensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "location"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "on"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "radius")
    };
    const node_type_ptr type(new vrml97_node_type_impl<point_light_node>(*this, id));
    vrml97_node_type_impl<point_light_node> & pointLightNodeType =
            static_cast<vrml97_node_type_impl<point_light_node> &>(*type);
    typedef vrml97_node_type_impl<point_light_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &point_light_node::process_set_ambientIntensity,
                node_field_ptr_ptr(new node_field_ptr_impl<point_light_node, sffloat>
                                    (&point_light_node::ambientIntensity)));
        } else if (*interface == supportedInterfaces[1]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &point_light_node::process_set_attenuation,
                node_field_ptr_ptr(new node_field_ptr_impl<point_light_node, sfvec3f>
                                    (&point_light_node::attenuation)));
        } else if (*interface == supportedInterfaces[2]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &point_light_node::process_set_color,
                node_field_ptr_ptr(new node_field_ptr_impl<point_light_node, sfcolor>
                                    (&point_light_node::color_)));
        } else if (*interface == supportedInterfaces[3]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &point_light_node::process_set_intensity,
                node_field_ptr_ptr(new node_field_ptr_impl<point_light_node, sffloat>
                                    (&point_light_node::intensity_)));
        } else if (*interface == supportedInterfaces[4]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &point_light_node::process_set_location,
                node_field_ptr_ptr(new node_field_ptr_impl<point_light_node, sfvec3f>
                                    (&point_light_node::location)));
        } else if (*interface == supportedInterfaces[5]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &point_light_node::process_set_on,
                node_field_ptr_ptr(new node_field_ptr_impl<point_light_node, sfbool>
                                    (&point_light_node::on_)));
        } else if (*interface == supportedInterfaces[6]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &point_light_node::process_set_radius,
                node_field_ptr_ptr(new node_field_ptr_impl<point_light_node, sffloat>
                                    (&point_light_node::radius)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class point_light_node
 *
 * @brief PointLight node instances.
 */

/**
 * @var point_light_node::point_light_class
 *
 * @brief Class object for PointLight instances.
 */

/**
 * @var sfvec3f point_light_node::attenuation
 *
 * @brief attenuation exposedField.
 */

/**
 * @var sfvec3f point_light_node::location
 *
 * @brief location exposedField.
 */

/**
 * @var sffloat point_light_node::radius
 *
 * @brief radius exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
point_light_node::point_light_node(const node_type & type,
                       const scope_ptr & scope):
    node(type, scope),
    abstract_light_node(type, scope),
    attenuation(vec3f(1.0, 0.0, 0.0)),
    location(vec3f(0.0, 0.0, 0.0)),
    radius(100)
{}

/**
 * @brief Destroy.
 */
point_light_node::~point_light_node() throw ()
{}

/**
 * @brief Cast to a point_light_node.
 *
 * @return a pointer to the point_light_node.
 */
point_light_node* point_light_node::to_point_light() const
{
    return (point_light_node*) this;
}

/**
 * @brief Render the scoped light.
 *
 * This should be called before rendering any geometry nodes in the scene.
 * Since this is called from Scene::render() before traversing the
 * scene graph, the proper transformation matrix hasn't been set up.
 * Somehow it needs to figure out the accumulated xforms of its
 * parents and apply them before rendering. This is not easy with
 * DEF/USEd nodes...
 *
 * @param viewer    a viewer.
 */
void point_light_node::renderScoped(openvrml::viewer & viewer)
{
    if (this->on_.value && this->radius.value > 0.0) {
        viewer.insert_point_light(this->ambientIntensity.value,
                                  this->attenuation.value,
                                  this->color_.value,
                                  this->intensity_.value,
                                  this->location.value,
                                  this->radius.value);
    }
    this->node::modified(false);
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void point_light_node::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_scoped_light(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void point_light_node::do_shutdown(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.remove_scoped_light(*this);
}

/**
 * @brief set_attenuation eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void point_light_node::process_set_attenuation(const field_value & value,
                                        const double timestamp)
    throw (std::bad_cast)
{
    this->attenuation = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("attenuation_changed", this->attenuation, timestamp);
}

/**
 * @brief set_location eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void point_light_node::process_set_location(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->location = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("location_changed", this->location, timestamp);
}

/**
 * @brief set_radius eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void point_light_node::process_set_radius(const field_value & value,
                                   const double timestamp)
    throw (std::bad_cast)
{
    this->radius = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("radius_changed", this->radius, timestamp);
}


/**
 * @class point_set_class
 *
 * @brief Class object for PointSet nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
point_set_class::point_set_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
point_set_class::~point_set_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating PointSet nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by point_set_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
point_set_class::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "coord")
    };
    const node_type_ptr type(new vrml97_node_type_impl<point_set_node>(*this, id));
    vrml97_node_type_impl<point_set_node> & pointSetNodeType =
            static_cast<vrml97_node_type_impl<point_set_node> &>(*type);
    typedef vrml97_node_type_impl<point_set_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            pointSetNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &point_set_node::process_set_color,
                node_field_ptr_ptr(new node_field_ptr_impl<point_set_node, sfnode>
                                    (&point_set_node::color)));
        } else if (*interface == supportedInterfaces[1]) {
            pointSetNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &point_set_node::process_set_coord,
                node_field_ptr_ptr(new node_field_ptr_impl<point_set_node, sfnode>
                                    (&point_set_node::coord)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class point_set_node
 *
 * @brief Represents PointSet node instances.
 */

/**
 * @var point_set_node::point_set_class
 *
 * @brief Class object for PointSet instances.
 */

/**
 * @var sfnode point_set_node::color
 *
 * @brief color exposedField.
 */

/**
 * @var sfnode point_set_node::coord
 *
 * @brief coord exposedField.
 */

/**
 * @var bounding_sphere point_set_node::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
point_set_node::point_set_node(const node_type & type,
                   const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
point_set_node::~point_set_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool point_set_node::modified() const
{
    return (modified()
            || (this->color.value && this->color.value->modified())
            || (this->coord.value && this->coord.value->modified()));
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t point_set_node::insert_geometry(openvrml::viewer & viewer,
                                        const rendering_context context)
{
    using std::vector;

    if (context.draw_bounding_spheres) {
        assert(dynamic_cast<const bounding_sphere *>
               (&this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
        viewer.draw_bounding_sphere(bs, bounding_volume::intersection(4));
    }

    openvrml::coordinate_node * const coordinateNode = this->coord.value
        ? this->coord.value->to_coordinate()
        : 0;
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    openvrml::color_node * const colorNode = this->color.value
        ? this->color.value->to_color()
        : 0;
    const vector<openvrml::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<openvrml::color>();

    viewer::object_t obj = viewer.insert_point_set(coord, color);

    if (this->color.value) { this->color.value->modified(false); }
    if (this->coord.value) { this->coord.value->modified(false); }

    return obj;
}

/**
 * @brief Recalculate the bounding volume.
 */
void point_set_node::recalcBSphere()
{
    this->bsphere = bounding_sphere();
    openvrml::coordinate_node * const coordinateNode = this->coord.value
        ? this->coord.value->to_coordinate()
        : 0;
    if (coordinateNode) {
        const std::vector<vec3f> & coord = coordinateNode->point();
        for(std::vector<vec3f>::const_iterator vec(coord.begin());
                vec != coord.end(); ++vec) {
            this->bsphere.extend(*vec);
        }
    }
    this->bounding_volume_dirty(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & point_set_node::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<point_set_node *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief set_color eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void point_set_node::process_set_color(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->color = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("color_changed", this->color, timestamp);
}

/**
 * @brief set_coord eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void point_set_node::process_set_coord(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->coord = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("coord_changed", this->coord, timestamp);
}


/**
 * @class position_interpolator_class
 *
 * @brief Class object for PositionInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
position_interpolator_class::position_interpolator_class(
    openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
position_interpolator_class::~position_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *      PositionInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by position_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
position_interpolator_class::create_type(const std::string & id,
                                         const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr
        type(new vrml97_node_type_impl<position_interpolator_node>(*this, id));
    vrml97_node_type_impl<position_interpolator_node> & positionInterpolatorNodeType =
            static_cast<vrml97_node_type_impl<position_interpolator_node> &>(*type);
    typedef vrml97_node_type_impl<position_interpolator_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            positionInterpolatorNodeType
                    .add_eventin(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &position_interpolator_node::process_set_fraction);
        } else if (*interface == supportedInterfaces[1]) {
            positionInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &position_interpolator_node::process_set_key,
                node_field_ptr_ptr(new node_field_ptr_impl<position_interpolator_node, mffloat>
                                    (&position_interpolator_node::key)));
        } else if (*interface == supportedInterfaces[2]) {
            positionInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &position_interpolator_node::process_set_keyValue,
                node_field_ptr_ptr(new node_field_ptr_impl<position_interpolator_node, mfvec3f>
                                    (&position_interpolator_node::keyValue)));
        } else if (*interface == supportedInterfaces[3]) {
            positionInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<position_interpolator_node, sfvec3f>
                                    (&position_interpolator_node::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class position_interpolator_node
 *
 * @brief PositionInterpolator node instances.
 */

/**
 * @var position_interpolator_node::position_interpolator_class
 *
 * @brief Class object for PositionInterpolator instances.
 */

/**
 * @var mffloat position_interpolator_node::key
 *
 * @brief key exposedField.
 */

/**
 * @var mfvec3f position_interpolator_node::keyValue
 *
 * @brief keyValue exposedField.
 */

/**
 * @var sfvec3f position_interpolator_node::value
 *
 * @brief value_changed eventOut.
 */


/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
position_interpolator_node::position_interpolator_node(const node_type & type,
                                           const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
position_interpolator_node::~position_interpolator_node() throw ()
{}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
position_interpolator_node::process_set_fraction(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    using std::vector;

    float f = dynamic_cast<const sffloat &>(value).value;

    vector<float>::size_type n = this->key.value.size() - 1;
    if (f < this->key.value[0]) {
        this->value.value = this->keyValue.value[0];
    } else if (f > this->key.value[n]) {
        this->value.value = this->keyValue.value[n];
    } else {
        // should cache the last index used...
        for (vector<float>::size_type i = 0; i < n; ++i) {
            if (this->key.value[i] <= f && f <= this->key.value[i + 1]) {
                const vec3f & v1 = this->keyValue.value[i];
                const vec3f & v2 = this->keyValue.value[i + 1];

                f = (f - this->key.value[i])
                    / (this->key.value[i + 1] - this->key.value[i]);
                const vec3f valueVec(v1.x() + f * (v2.x() - v1.x()),
                                     v1.y() + f * (v2.y() - v1.y()),
                                     v1.z() + f * (v2.z() - v1.z()));
                this->value.value = valueVec;
                break;
            }
        }
    }

    // Send the new value
    this->emit_event("value_changed", this->value, timestamp);
}

/**
 * @brief set_key eventIn handler.
 *
 * @param value     an mffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void position_interpolator_node::process_set_key(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->key = dynamic_cast<const mffloat &>(value);
    this->emit_event("key_changed", this->key, timestamp);
}

/**
 * @brief set_keyValue field mutator.
 *
 * @param value     an mfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec3f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
position_interpolator_node::process_set_keyValue(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mfvec3f &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class proximity_sensor_class
 *
 * @brief Class object for ProximitySensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
proximity_sensor_class::proximity_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
proximity_sensor_class::~proximity_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating ProximitySensor
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by proximity_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
proximity_sensor_class::create_type(const std::string & id,
                                    const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "center"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "size"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive"),
        node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "position_changed"),
        node_interface(node_interface::eventout_id, field_value::sfrotation_id, "orientation_changed"),
        node_interface(node_interface::eventout_id, field_value::sftime_id, "enterTime"),
        node_interface(node_interface::eventout_id, field_value::sftime_id, "exitTime")
    };
    const node_type_ptr type(new vrml97_node_type_impl<proximity_sensor_node>(*this, id));
    vrml97_node_type_impl<proximity_sensor_node> & proximitySensorNodeType =
            static_cast<vrml97_node_type_impl<proximity_sensor_node> &>(*type);
    typedef vrml97_node_type_impl<proximity_sensor_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            proximitySensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &proximity_sensor_node::process_set_center,
                node_field_ptr_ptr(new node_field_ptr_impl<proximity_sensor_node, sfvec3f>
                                    (&proximity_sensor_node::center)));
        } else if (*interface == supportedInterfaces[1]) {
            proximitySensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &proximity_sensor_node::process_set_size,
                node_field_ptr_ptr(new node_field_ptr_impl<proximity_sensor_node, sfvec3f>
                                    (&proximity_sensor_node::size)));
        } else if (*interface == supportedInterfaces[2]) {
            proximitySensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &proximity_sensor_node::process_set_enabled,
                node_field_ptr_ptr(new node_field_ptr_impl<proximity_sensor_node, sfbool>
                                    (&proximity_sensor_node::enabled)));
        } else if (*interface == supportedInterfaces[3]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<proximity_sensor_node, sfbool>
                                    (&proximity_sensor_node::active)));
        } else if (*interface == supportedInterfaces[4]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<proximity_sensor_node, sfvec3f>
                                    (&proximity_sensor_node::position)));
        } else if (*interface == supportedInterfaces[5]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<proximity_sensor_node, sfrotation>
                                    (&proximity_sensor_node::orientation)));
        } else if (*interface == supportedInterfaces[6]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<proximity_sensor_node, sftime>
                                    (&proximity_sensor_node::enterTime)));
        } else if (*interface == supportedInterfaces[7]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(new node_field_ptr_impl<proximity_sensor_node, sftime>
                                    (&proximity_sensor_node::exitTime)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class proximity_sensor_node
 *
 * @brief Represents ProximitySensor node instances.
 */

/**
 * @var proximity_sensor_node::proximity_sensor_class
 *
 * @brief Class object for ProximitySensor instances.
 */

/**
 * @var sfvec3f proximity_sensor_node::center
 *
 * @brief center exposedField.
 */

/**
 * @var sfbool proximity_sensor_node::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfvec3f proximity_sensor_node::size
 *
 * @brief size exposedField.
 */

/**
 * @var sfbool proximity_sensor_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfvec3f proximity_sensor_node::position
 *
 * @brief position_changed eventOut.
 */

/**
 * @var sfvec3f proximity_sensor_node::orientation
 *
 * @brief orientation_changed eventOut.
 */

/**
 * @var sftime proximity_sensor_node::enterTime
 *
 * @brief enterTime eventOut.
 */

/**
 * @var sftime proximity_sensor_node::exitTime
 *
 * @brief exitTime eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
proximity_sensor_node::proximity_sensor_node(const node_type & type,
                                             const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    center(vec3f(0.0, 0.0, 0.0)),
    enabled(true),
    size(vec3f(0.0, 0.0, 0.0)),
    active(false),
    position(vec3f(0.0, 0.0, 0.0)),
    enterTime(0.0),
    exitTime(0.0)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
proximity_sensor_node::~proximity_sensor_node() throw ()
{}

/**
 * @brief Render the node: generate proximity events.
 *
 * If necessary, events prior to the current time are generated due to
 * interpolation of enterTimes and exitTimes. The timestamp should never be
 * increased.
 *
 * This is in a render() method since the it needs the viewer position
 * with respect to the local coordinate system.
 * Could do this with Node::inverseTransform(double [4][4]) now...
 *
 * The positions and times are not interpolated to report the exact
 * place and time of entries and exits from the sensor regions as
 * required by the spec, since that would require the last viewer position
 * to be stored in the node, which is problematic in the presence of
 * DEF/USEd nodes...
 * I suppose the scene could keep the last viewer position in the global
 * coordinate system and it could be transformed all the way down the
 * scenegraph, but that sounds painful.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void proximity_sensor_node::render(openvrml::viewer & viewer,
                                   const rendering_context context)
{
    if (this->enabled.value
            && this->size.value.x() > 0.0
            && this->size.value.y() > 0.0
            && this->size.value.z() > 0.0
            && viewer.mode() == viewer::draw_mode) {
        using openvrml_::fabs;
        using openvrml_::fless_equal;

        sftime timeNow(browser::current_time());
        float x, y, z;

        // Is viewer inside the box?
        mat4f MV = context.matrix();
        MV = MV.inverse();
        x = MV[3][0]; y = MV[3][1]; z = MV[3][2];
        bool inside = fless_equal<float>()(fabs(x - this->center.value.x()),
                                           0.5f * this->size.value.x())
                   && fless_equal<float>()(fabs(y - this->center.value.y()),
                                           0.5f * this->size.value.y())
                   && fless_equal<float>()(fabs(z - this->center.value.z()),
                                           0.5f * this->size.value.z());
        bool wasIn = this->active.value;

        // Check if viewer has entered the box
        if (inside && ! wasIn) {
            this->active.value = true;
            this->emit_event("isActive", this->active, timeNow.value);

            this->enterTime = timeNow;
            this->emit_event("enterTime", this->enterTime, timeNow.value);
        }

        // Check if viewer has left the box
        else if (wasIn && !inside) {
            this->active.value = false;
            this->emit_event("isActive", this->active, timeNow.value);

            this->exitTime = timeNow;
            this->emit_event("exitTime", this->exitTime, timeNow.value);
        }

        // Check for movement within the box
        if (wasIn || inside) {
            if (position.value != vec3f(x, y, z)) {
                this->position.value = vec3f(x, y, z);
                this->emit_event("position_changed", this->position,
                                 timeNow.value);
            }

            vec3f trans, scale, shear;
            rotation orientation;
            MV.transformation(trans, orientation, scale, shear);
            if (this->orientation.value != orientation) {
                this->orientation.value = orientation;
                this->emit_event("orientation_changed", this->orientation,
                                 timeNow.value);
            }
        }
    } else {
        this->node::modified(false);
    }
}

/**
 * @brief set_center eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void proximity_sensor_node::process_set_center(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast)
{
    this->center = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("center_changed", this->center, timestamp);
}

/**
 * @brief set_size eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void proximity_sensor_node::process_set_size(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    this->size = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("size_changed", this->size, timestamp);
}

/**
 * @brief set_enabled eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p sfbool is not an value.
 */
void proximity_sensor_node::process_set_enabled(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->enabled = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("enabled_changed", this->enabled, timestamp);
}


/**
 * @class scalar_interpolator_class
 *
 * @brief Class object for ScalarInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
scalar_interpolator_class::scalar_interpolator_class(
    openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
scalar_interpolator_class::~scalar_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *      CoordinateInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by CoordinateInterpolatorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
scalar_interpolator_class::create_type(const std::string & id,
                                       const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sffloat_id, "set_fraction"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "key"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "keyValue"),
        node_interface(node_interface::eventout_id, field_value::sffloat_id, "value_changed")
    };
    const node_type_ptr type(new vrml97_node_type_impl<scalar_interpolator_node>(*this, id));
    vrml97_node_type_impl<scalar_interpolator_node> & scalarInterpolatorNodeType =
            static_cast<vrml97_node_type_impl<scalar_interpolator_node> &>(*type);
    typedef vrml97_node_type_impl<scalar_interpolator_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            scalarInterpolatorNodeType
                    .add_eventin(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &scalar_interpolator_node::process_set_fraction);
        } else if (*interface == supportedInterfaces[1]) {
            scalarInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &scalar_interpolator_node::process_set_key,
                node_field_ptr_ptr(new node_field_ptr_impl<scalar_interpolator_node, mffloat>
                                    (&scalar_interpolator_node::key)));
        } else if (*interface == supportedInterfaces[2]) {
            scalarInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &scalar_interpolator_node::process_set_keyValue,
                node_field_ptr_ptr(new node_field_ptr_impl<scalar_interpolator_node, mffloat>
                                    (&scalar_interpolator_node::keyValue)));
        } else if (*interface == supportedInterfaces[3]) {
            scalarInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<scalar_interpolator_node, sffloat>
                                    (&scalar_interpolator_node::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class scalar_interpolator_node
 *
 * @brief ScalarInterpolator node instances.
 */

/**
 * @var scalar_interpolator_node::scalar_interpolator_class
 *
 * @brief Class object for ScalarInterpolator instances.
 */

/**
 * @var mffloat scalar_interpolator_node::key
 *
 * @brief key exposedField.
 */

/**
 * @var mffloat scalar_interpolator_node::keyValue
 *
 * @brief keyValue exposedField.
 */

/**
 * @var sffloat scalar_interpolator_node::value
 *
 * @brief value_changed eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
scalar_interpolator_node::scalar_interpolator_node(const node_type & type,
                                                   const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
scalar_interpolator_node::~scalar_interpolator_node() throw ()
{}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void scalar_interpolator_node::process_set_fraction(const field_value & value,
                                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    using std::vector;

    float f = dynamic_cast<const sffloat &>(value).value;

    vector<float>::size_type n = this->key.value.size() - 1;
    if (f < this->key.value[0]) {
        this->value.value = this->keyValue.value[0];
    } else if (f > this->key.value[n]) {
        this->value.value = this->keyValue.value[n];
    } else {
        for (vector<float>::size_type i = 0; i < n; ++i) {
            if (this->key.value[i] <= f && f <= this->key.value[i + 1]) {
                float v1 = this->keyValue.value[i];
                float v2 = this->keyValue.value[i + 1];

                f = (f - this->key.value[i]) / (this->key.value[i + 1]
                        - this->key.value[i]);
                this->value.value = v1 + f * (v2 - v1);
                break;
            }
        }
    }

    // Send the new value
    this->emit_event("value_changed", this->value, timestamp);
}

/**
 * @brief set_key eventIn handler.
 *
 * @param value     an mffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void scalar_interpolator_node::process_set_key(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->key = dynamic_cast<const mffloat &>(value);
    this->emit_event("key_changed", this->key, timestamp);
}

/**
 * @brief set_keyValue field mutator.
 *
 * @param value     an mffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void scalar_interpolator_node::process_set_keyValue(const field_value & value,
                                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mffloat &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class shape_class
 *
 * @brief Class object for Shape nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
shape_class::shape_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
shape_class::~shape_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Shape nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by shape_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
shape_class::create_type(const std::string & id,
                         const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "appearance"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "geometry")
    };
    const node_type_ptr type(new vrml97_node_type_impl<shape_node>(*this, id));
    vrml97_node_type_impl<shape_node> & shapeNodeType =
            static_cast<vrml97_node_type_impl<shape_node> &>(*type);
    typedef vrml97_node_type_impl<shape_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            shapeNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &shape_node::process_set_appearance,
                node_field_ptr_ptr(new node_field_ptr_impl<shape_node, sfnode>
                                    (&shape_node::appearance)));
        } else if (*interface == supportedInterfaces[1]) {
            shapeNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &shape_node::process_set_geometry,
                node_field_ptr_ptr(new node_field_ptr_impl<shape_node, sfnode>
                                    (&shape_node::geometry)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class shape_node
 *
 * @brief Represents Shape node instances.
 */

/**
 * @var shape_node::shape_class
 *
 * @brief Class object for Shape instances.
 */

/**
 * @var sfnode shape_node::appearance
 *
 * @brief appearance exposedField.
 */

/**
 * @var sfnode shape_node::geometry
 *
 * @brief geometry exposedField.
 */

/**
 * @var viewer::object_t shape_node::viewerObject
 *
 * @brief A reference to the node's previously used rendering data.
 *
 * If supported by the Viewer implementation, this member holds a reference
 * to the node's rendering data once the node has already been rendered once.
 * The intent is to capitalize on USE references in the VRML scene graph.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
shape_node::shape_node(const node_type & type,
             const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    viewerObject(0)
{}

/**
 * @brief Destroy.
 */
shape_node::~shape_node() throw ()
{
    // need viewer to free viewerObject ...
}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool shape_node::modified() const
{
    return (modified()
            || (this->geometry.value && this->geometry.value->modified())
            || (this->appearance.value && this->appearance.value->modified()));
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void shape_node::render(openvrml::viewer & viewer, const rendering_context context)
{
    if (this->viewerObject && modified()) {
        viewer.remove_object(this->viewerObject);
        this->viewerObject = 0;
    }

    geometry_node * g = this->geometry.value
                      ? this->geometry.value->to_geometry()
                      : 0;

    if (this->viewerObject) {
        viewer.insert_reference(this->viewerObject);
    } else if (g) {
        this->viewerObject = viewer.begin_object(this->id().c_str());

        // Don't care what color it is if we are picking
        bool picking = (viewer::pick_mode == viewer.mode());
        if (!picking) {
            size_t nTexComponents = 0;

            if (!picking && this->appearance.value
                    && this->appearance.value->to_appearance()) {
                openvrml::appearance_node * const a =
                    this->appearance.value->to_appearance();
                a->render(viewer, context);

                if (a->texture() && a->texture()->to_texture()) {
                    nTexComponents = a->texture()->to_texture()->components();
                }
            } else {
                viewer.set_color(color(1.0, 1.0, 1.0)); // default object color
                viewer.enable_lighting(false);  // turn lighting off
            }

            // hack for opengl material mode
            viewer.set_material_mode(nTexComponents, g->color());
        }

        g->render(viewer, context);

        viewer.end_object();
    } else if (this->appearance.value) {
        this->appearance.value->modified(false);
    }
    this->node::modified(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & shape_node::bounding_volume() const
{
    //
    // just pass off to the geometry's getbvolume() method
    //
    const openvrml::bounding_volume & result =
        this->geometry.value
        ? this->geometry.value->bounding_volume()
        : this->node::bounding_volume();
    const_cast<shape_node *>(this)->bounding_volume_dirty(false);
    return result;
}

/**
 * @brief set_appearance eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void shape_node::process_set_appearance(const field_value & value,
                                        const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->appearance = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("appearance_changed", this->appearance, timestamp);
}

/**
 * @brief set_geometry eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void shape_node::process_set_geometry(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->geometry = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("geometry_changed", this->geometry, timestamp);
}


/**
 * @class sound_class
 *
 * @brief Class object for Sound nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
sound_class::sound_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
sound_class::~sound_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Sound nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by sound_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
sound_class::create_type(const std::string & id,
                         const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "direction"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "intensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "location"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "maxBack"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "maxFront"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "minBack"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "minFront"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "priority"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "source"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "spatialize")
    };
    const node_type_ptr type(new vrml97_node_type_impl<sound_node>(*this, id));
    vrml97_node_type_impl<sound_node> & soundNodeType =
            static_cast<vrml97_node_type_impl<sound_node> &>(*type);
    typedef vrml97_node_type_impl<sound_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &sound_node::process_set_direction,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sfvec3f>
                                    (&sound_node::direction)));
        } else if (*interface == supportedInterfaces[1]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &sound_node::process_set_intensity,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sffloat>
                                    (&sound_node::intensity)));
        } else if (*interface == supportedInterfaces[2]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &sound_node::process_set_location,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sfvec3f>
                                    (&sound_node::location)));
        } else if (*interface == supportedInterfaces[3]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &sound_node::process_set_maxBack,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sffloat>
                                    (&sound_node::maxBack)));
        } else if (*interface == supportedInterfaces[4]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &sound_node::process_set_maxFront,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sffloat>
                                    (&sound_node::maxFront)));
        } else if (*interface == supportedInterfaces[5]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &sound_node::process_set_minBack,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sffloat>
                                    (&sound_node::minBack)));
        } else if (*interface == supportedInterfaces[6]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &sound_node::process_set_minFront,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sffloat>
                                    (&sound_node::minFront)));
        } else if (*interface == supportedInterfaces[7]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &sound_node::process_set_priority,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sffloat>
                                    (&sound_node::priority)));
        } else if (*interface == supportedInterfaces[8]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                &sound_node::process_set_source,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sfnode>
                                    (&sound_node::source)));
        } else if (*interface == supportedInterfaces[9]) {
            soundNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_field_ptr_ptr(new node_field_ptr_impl<sound_node, sfbool>
                                    (&sound_node::spatialize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class sound_node
 *
 * @brief Represents Sound node instances.
 */

/**
 * @var sound_node::sound_class
 *
 * @brief Class object for Sound instances.
 */

/**
 * @var sfvec3f sound_node::direction
 *
 * @brief direction exposedField.
 */

/**
 * @var sffloat sound_node::intensity
 *
 * @brief intensity exposedField.
 */

/**
 * @var sfvec3f sound_node::location
 *
 * @brief location exposedField.
 */

/**
 * @var sffloat sound_node::maxBack
 *
 * @brief maxBack exposedField.
 */

/**
 * @var sffloat sound_node::maxFront
 *
 * @brief maxFront exposedField.
 */

/**
 * @var sffloat sound_node::minBack
 *
 * @brief minBack exposedField.
 */

/**
 * @var sffloat sound_node::minFront
 *
 * @brief minFront exposedField.
 */

/**
 * @var sffloat sound_node::priority
 *
 * @brief priority exposedField.
 */

/**
 * @var sfnode sound_node::source
 *
 * @brief source exposedField.
 */

/**
 * @var sfbool sound_node::spatialize
 *
 * @brief spatialize field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope     the scope associated with the instance.
 */
sound_node::sound_node(const node_type & type,
                       const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    direction(vec3f(0, 0, 1)),
    intensity(1),
    maxBack(10),
    maxFront(10),
    minBack(1),
    minFront(1),
    spatialize(true)
{}

/**
 * @brief Destroy.
 */
sound_node::~sound_node() throw ()
{}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void sound_node::render(openvrml::viewer & viewer,
                        const rendering_context context)
{
    // If this clip has been modified, update the internal data
    if (this->source.value && this->source.value->modified()) {
        this->source.value->render(viewer, context);
    }
}

/**
 * @brief set_direction eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void sound_node::process_set_direction(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast)
{
    this->direction = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("direction_changed", this->direction, timestamp);
}

/**
 * @brief set_intensity eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void sound_node::process_set_intensity(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast)
{
    this->intensity = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("intensity_changed", this->intensity, timestamp);
}

/**
 * @brief set_location eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void sound_node::process_set_location(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast)
{
    this->location = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("location_changed", this->location, timestamp);
}

/**
 * @brief set_maxBack eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void sound_node::process_set_maxBack(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->maxBack = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("maxBack_changed", this->maxBack, timestamp);
}

/**
 * @brief set_maxFront eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void sound_node::process_set_maxFront(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast)
{
    this->maxFront = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("maxFront_changed", this->maxFront, timestamp);
}

/**
 * @brief set_minBack eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void sound_node::process_set_minBack(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->minBack = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("minBack_changed", this->minBack, timestamp);
}

/**
 * @brief set_minFront eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void sound_node::process_set_minFront(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast)
{
    this->minFront = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("minFront_changed", this->minFront, timestamp);
}

/**
 * @brief set_priority eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void sound_node::process_set_priority(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast)
{
    this->priority = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("priority_changed", this->priority, timestamp);
}

/**
 * @brief set_source eventIn handler.
 *
 * @param value     an sfnode.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfnode.
 */
void sound_node::process_set_source(const field_value & value,
                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->source = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("source_changed", this->source, timestamp);
}


/**
 * @class sphere_class
 *
 * @brief Class object for Sphere nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
sphere_class::sphere_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
sphere_class::~sphere_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Sphere nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by sphere_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr sphere_class::create_type(const std::string & id,
                                          const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
            node_interface(node_interface::field_id, field_value::sffloat_id, "radius");
    const node_type_ptr type(new vrml97_node_type_impl<sphere_node>(*this, id));
    vrml97_node_type_impl<sphere_node> & spereNodeType =
            static_cast<vrml97_node_type_impl<sphere_node> &>(*type);
    typedef vrml97_node_type_impl<sphere_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterface) {
            spereNodeType.add_field(
                supportedInterface.field_type,
                supportedInterface.id,
                node_field_ptr_ptr(new node_field_ptr_impl<sphere_node, sffloat>
                                    (&sphere_node::radius)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class sphere_node
 *
 * @brief Sphere node instances.
 */

/**
 * @var sphere_node::sphere_class
 *
 * @brief Class object for Sphere instances.
 */

/**
 * @var sffloat sphere_node::radius
 *
 * @brief radius field.
 */

/**
 * @var bounding_sphere sphere_node::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
sphere_node::sphere_node(const node_type & type,
               const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope),
    radius(1.0)
{
    this->bounding_volume_dirty(true); // lazy calc of bvolumes
}

/**
 * @brief Destroy.
 */
sphere_node::~sphere_node() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t sphere_node::insert_geometry(openvrml::viewer & viewer,
                                         const rendering_context context)
{
    return viewer.insert_sphere(this->radius.value);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & sphere_node::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<sphere_node *>(this)->bsphere.radius(this->radius.value);
        const_cast<sphere_node *>(this)->bounding_volume_dirty(false); // logical const
    }
    return this->bsphere;
}


/**
 * @class sphere_sensor_class
 *
 * @brief Class object for SphereSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
sphere_sensor_class::sphere_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
sphere_sensor_class::~sphere_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating SphereSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by sphere_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
sphere_sensor_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "autoOffset"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "offset"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive"),
        node_interface(node_interface::eventout_id, field_value::sfrotation_id, "rotation_changed"),
        node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "trackPoint_changed")
    };
    const node_type_ptr type(new vrml97_node_type_impl<sphere_sensor_node>(*this, id));
    vrml97_node_type_impl<sphere_sensor_node> & sphereSensorNodeType =
            static_cast<vrml97_node_type_impl<sphere_sensor_node> &>(*type);
    typedef vrml97_node_type_impl<sphere_sensor_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            sphereSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &sphere_sensor_node::process_set_autoOffset,
                node_field_ptr_ptr(new node_field_ptr_impl<sphere_sensor_node, sfbool>
                                    (&sphere_sensor_node::autoOffset)));
        } else if (*interface == supportedInterfaces[1]) {
            sphereSensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &sphere_sensor_node::process_set_enabled,
                node_field_ptr_ptr(new node_field_ptr_impl<sphere_sensor_node, sfbool>
                                    (&sphere_sensor_node::enabled)));
        } else if (*interface == supportedInterfaces[2]) {
            sphereSensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &sphere_sensor_node::process_set_offset,
                node_field_ptr_ptr(new node_field_ptr_impl<sphere_sensor_node, sfrotation>
                                    (&sphere_sensor_node::offset)));
        } else if (*interface == supportedInterfaces[3]) {
            sphereSensorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<sphere_sensor_node, sfbool>
                                    (&sphere_sensor_node::active)));
        } else if (*interface == supportedInterfaces[4]) {
            sphereSensorNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<sphere_sensor_node, sfrotation>
                                    (&sphere_sensor_node::rotation)));
        } else if (*interface == supportedInterfaces[5]) {
            sphereSensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<sphere_sensor_node, sfvec3f>
                                    (&sphere_sensor_node::trackPoint)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class sphere_sensor_node
 *
 * @brief SphereSensor node instances.
 */

/**
 * @var sphere_sensor_node::sphere_sensor_class
 *
 * @brief Class object for SphereSensor instances.
 */

/**
 * @var sfbool sphere_sensor_node::autoOffset
 *
 * @brief autoOffset exposedField.
 */

/**
 * @var sfbool sphere_sensor_node::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfrotation sphere_sensor_node::offset
 *
 * @brief offset exposedField.
 */

/**
 * @var sfbool sphere_sensor_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfrotation sphere_sensor_node::rotation
 *
 * @brief rotation_changed eventOut.
 */

/**
 * @var sfvec3f sphere_sensor_node::trackPoint
 *
 * @brief trackPoint_changed eventOut.
 */

/**
 * @var sfvec3f sphere_sensor_node::activationPoint
 *
 * @brief The start point of a drag operation.
 */

/**
 * @var sfvec3f sphere_sensor_node::centerPoint
 *
 * @brief Center point.
 */

/**
 * @var mat4f sphere_sensor_node::modelview
 *
 * @brief Modelview matrix.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
sphere_sensor_node::sphere_sensor_node(const node_type & type,
                           const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    autoOffset(true),
    enabled(true),
    offset(openvrml::rotation(0.0, 1.0, 0.0, 0.0)),
    active(false)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
sphere_sensor_node::~sphere_sensor_node() throw ()
{}

/**
 * @brief Cast to a sphere_sensor_node.
 *
 * @return a pointer to the node.
 */
sphere_sensor_node * sphere_sensor_node::to_sphere_sensor() const
{
    return const_cast<sphere_sensor_node *>(this);
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void sphere_sensor_node::render(openvrml::viewer & viewer,
                                const rendering_context context)
{
    //
    // Store the ModelView matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation
    //
    this->modelview = context.matrix();
}

/**
 * @brief Activate or deactivate the sphere_sensor_node.
 *
 * Activating a drag sensor means that the pointing device button has been
 * depressed and a drag operation has been initiated. The sensor is deactivated
 * when the button is released at the end of the operation.
 *
 * @param timeStamp the current time.
 * @param isActive  @c true if the drag operation is in progress; @c false
 *                  otherwise.
 * @param p         the pointing device position.
 */
void sphere_sensor_node::activate(double timeStamp, bool isActive, double * p)
{
    // Become active
    if (isActive && !this->active.value) {
        this->active.value = isActive;

        // set activation point in world coords
        const vec3f floatVec(static_cast<float>(p[0]),
                             static_cast<float>(p[1]),
                             static_cast<float>(p[2]));
        this->activationPoint.value = floatVec;

        if (this->autoOffset.value) { this->rotation = this->offset; }

        // calculate the center of the object in world coords
        vec3f V;
        mat4f M = this->modelview.inverse();
        V = V * M;
        this->centerPoint.value = V;

        // send message
        this->emit_event("isActive", this->active, timeStamp);
    }
    // Become inactive
    else if (!isActive && this->active.value) {
        this->active.value = isActive;
        this->emit_event("isActive", this->active, timeStamp);

        // save auto offset of rotation
        if (this->autoOffset.value) {
            this->offset = this->rotation;
            this->emit_event("offset_changed", this->offset, timeStamp);
        }
    }
    // Tracking
    else if (isActive) {
        // get local coord for touch point
        vec3f V(static_cast<float>(p[0]),
                static_cast<float>(p[1]),
                static_cast<float>(p[2]));
        mat4f M = this->modelview.inverse();
        V = V * M;
        this->trackPoint.value = V;
        this->emit_event("trackPoint_changed", this->trackPoint, timeStamp);

        vec3f V2(static_cast<float>(p[0]),
                 static_cast<float>(p[1]),
                 static_cast<float>(p[2]));
        vec3f tempv = V2 - this->centerPoint.value;
        vec3f dir1(tempv);
        float dist = dir1.length(); // get the length of the pre-normalized vector
        dir1 = dir1.normalize();
        tempv = this->activationPoint.value - this->centerPoint.value;
        vec3f dir2(tempv);
        dir2 = dir2.normalize();

        tempv = dir1 * dir2;
        vec3f cx(tempv);
        cx = cx.normalize();

        openvrml::rotation newRot(cx, dist * float(acos(dir1.dot(dir2))));
        if (this->autoOffset.value) {
            newRot = newRot * this->offset.value;
        }
        this->rotation.value = newRot;

        this->emit_event("rotation_changed", this->rotation, timeStamp);
    }
}

/**
 * @brief Determine whether the SphereSensor is enabled.
 *
 * @return @c true if the SphereSensor is enabled; @c false otherwise.
 */
bool sphere_sensor_node::isEnabled() const throw ()
{
    return this->enabled.value;
}

/**
 * @brief set_autoOffset eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void sphere_sensor_node::process_set_autoOffset(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->autoOffset = dynamic_cast<const sfbool &>(value);
    this->emit_event("autoOffset_changed", this->autoOffset, timestamp);
}

/**
 * @brief set_enabled eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void sphere_sensor_node::process_set_enabled(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    this->enabled = dynamic_cast<const sfbool &>(value);
    this->emit_event("enabled_changed", this->enabled, timestamp);
}

/**
 * @brief set_offset eventIn handler.
 *
 * @param value     an sfrotation value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfrotation.
 */
void sphere_sensor_node::process_set_offset(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast)
{
    this->offset = dynamic_cast<const sfrotation &>(value);
    this->emit_event("offset_changed", this->offset, timestamp);
}


/**
 * @class spot_light_class
 *
 * @brief Class object for SpotLight nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
spot_light_class::spot_light_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
spot_light_class::~spot_light_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating SpotLight nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by spot_light_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
spot_light_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<spot_light_node>(*this, id));
    vrml97_node_type_impl<spot_light_node> & spotLightNodeType =
            static_cast<vrml97_node_type_impl<spot_light_node> &>(*type);
    typedef vrml97_node_type_impl<spot_light_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &spot_light_node::process_set_ambientIntensity,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sffloat>
                                    (&spot_light_node::ambientIntensity)));
        } else if (*interface == supportedInterfaces[1]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &spot_light_node::process_set_attenuation,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sfvec3f>
                                    (&spot_light_node::attenuation)));
        } else if (*interface == supportedInterfaces[2]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &spot_light_node::process_set_beamWidth,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sffloat>
                                    (&spot_light_node::beamWidth)));
        } else if (*interface == supportedInterfaces[3]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &spot_light_node::process_set_color,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sfcolor>
                                    (&spot_light_node::color_)));
        } else if (*interface == supportedInterfaces[4]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &spot_light_node::process_set_cutOffAngle,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sffloat>
                                    (&spot_light_node::cutOffAngle)));
        } else if (*interface == supportedInterfaces[5]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &spot_light_node::process_set_direction,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sfvec3f>
                                    (&spot_light_node::direction)));
        } else if (*interface == supportedInterfaces[6]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &spot_light_node::process_set_intensity,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sffloat>
                                    (&spot_light_node::intensity_)));
        } else if (*interface == supportedInterfaces[7]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &spot_light_node::process_set_location,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sfvec3f>
                                    (&spot_light_node::location)));
        } else if (*interface == supportedInterfaces[8]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                &spot_light_node::process_set_on,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sfbool>
                                    (&spot_light_node::on_)));
        } else if (*interface == supportedInterfaces[9]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                &spot_light_node::process_set_radius,
                node_field_ptr_ptr(new node_field_ptr_impl<spot_light_node, sffloat>
                                    (&spot_light_node::radius)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class spot_light_node
 *
 * @brief SpotLight node instances.
 */

/**
 * @var spot_light_node::spot_light_class
 *
 * @brief Class object for SpotLight instances.
 */

/**
 * @var sfvec3f spot_light_node::attenuation
 *
 * @brief attenuation exposedField.
 */

/**
 * @var sffloat spot_light_node::beamWidth
 *
 * @brief beamWidth exposedField.
 */

/**
 * @var sffloat spot_light_node::cutOffAngle
 *
 * @brief cutOffAngle exposedField.
 */

/**
 * @var sfvec3f spot_light_node::direction
 *
 * @brief direction exposedField.
 */

/**
 * @var sfvec3f spot_light_node::location
 *
 * @brief location exposedField.
 */

/**
 * @var sffloat spot_light_node::radius
 *
 * @brief radius exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
spot_light_node::spot_light_node(const node_type & type,
                                 const scope_ptr & scope):
    node(type, scope),
    abstract_light_node(type, scope),
    attenuation(vec3f(1.0, 0.0, 0.0)),
    beamWidth(1.570796f),
    cutOffAngle(0.785398f),
    direction(vec3f(0.0, 0.0, -1.0)),
    location(vec3f(0.0, 0.0, 0.0)),
    radius(100)
{}

/**
 * @brief Destroy.
 */
spot_light_node::~spot_light_node() throw ()
{}

/**
 * @brief Cast to a spot_light_node.
 *
 * @return a pointer to the node.
 */
spot_light_node * spot_light_node::to_spot_light() const
{
    return const_cast<spot_light_node *>(this);
}

/**
 * @brief Render the scoped light.
 *
 * @param viewer    a Viewer.
 *
 * @todo This should be called before rendering any geometry in the scene.
 *      Since this is called from Scene::render before traversing the
 *      scene graph, the proper transformation matrix hasn't been set up.
 *      Somehow it needs to figure out the accumulated xforms of its
 *      parents and apply them before rendering. This is not easy with
 *      DEF/USEd nodes...
 */
void spot_light_node::renderScoped(openvrml::viewer & viewer)
{
    if (this->on_.value && this->radius.value > 0.0) {
        viewer.insert_spot_light(this->ambientIntensity.value,
                                 this->attenuation.value,
                                 this->beamWidth.value,
                                 this->color_.value,
                                 this->cutOffAngle.value,
                                 this->direction.value,
                                 this->intensity_.value,
                                 this->location.value,
                                 this->radius.value);
    }
    this->node::modified(false);
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void spot_light_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_scoped_light(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void spot_light_node::do_shutdown(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.remove_scoped_light(*this);
}

/**
 * @brief set_attenuation eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void spot_light_node::process_set_attenuation(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast)
{
    this->attenuation = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("attenuation_changed", this->attenuation, timestamp);
}

/**
 * @brief set_beamWidth eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void spot_light_node::process_set_beamWidth(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast)
{
    this->beamWidth = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("beamWidth_changed", this->beamWidth, timestamp);
}

/**
 * @brief set_cutOffAngle eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void spot_light_node::process_set_cutOffAngle(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast)
{
    this->cutOffAngle = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("cutOffAngle_changed", this->cutOffAngle, timestamp);
}

/**
 * @brief set_direction eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void spot_light_node::process_set_direction(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast)
{
    this->direction = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("direction_changed", this->direction, timestamp);
}

/**
 * @brief set_location eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void spot_light_node::process_set_location(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast)
{
    this->location = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("location_changed", this->location, timestamp);
}

/**
 * @brief set_radius eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void spot_light_node::process_set_radius(const field_value & value,
                                         const double timestamp)
    throw (std::bad_cast)
{
    this->radius = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("radius_changed", this->radius, timestamp);
}


/**
 * @class switch_class
 *
 * @brief Class object for Switch nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
switch_class::switch_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
switch_class::~switch_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Switch nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by switch_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
switch_class::create_type(const std::string & id,
                          const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                      field_value::mfnode_id,
                      "choice"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sfint32_id,
                      "whichChoice")
    };
    const node_type_ptr type(new vrml97_node_type_impl<switch_node>(*this, id));
    vrml97_node_type_impl<switch_node> & switchNodeType =
            static_cast<vrml97_node_type_impl<switch_node> &>(*type);
    typedef vrml97_node_type_impl<switch_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            switchNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &switch_node::process_set_choice,
                node_field_ptr_ptr(new node_field_ptr_impl<switch_node, mfnode>
                                    (&switch_node::choice)));
        } else if (*interface == supportedInterfaces[1]) {
            switchNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &switch_node::process_set_whichChoice,
                node_field_ptr_ptr(new node_field_ptr_impl<switch_node, sfint32>
                                    (&switch_node::whichChoice)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class switch_node
 *
 * @brief Switch node instances.
 */

/**
 * @var switch_node::switch_class
 *
 * @brief Class object for Switch instances.
 */

/**
 * @var mfnode switch_node::choice
 *
 * @brief choice exposedField.
 */

/**
 * @var sfint32 switch_node::whichChoice
 *
 * @brief whichChoice exposedField.
 */

/**
 * @var mfnode switch_node::children_
 *
 * @brief The children currently in the scene graph.
 */

/**
 * @var bounding_sphere switch_node::bsphere
 *
 * @brief Cached copy of the bsphere enclosing this node's children.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
switch_node::switch_node(const node_type & type,
               const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    whichChoice(-1),
    children_(1)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
switch_node::~switch_node() throw () {}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool switch_node::modified() const {
    if (this->node::modified()) { return true; }

    long w = this->whichChoice.value;

    return (w >= 0 && size_t(w) < this->choice.value.size()
            && this->choice.value[w]->modified());
}

/**
 * @brief Render the node.
 *
 * The child corresponding to @a whichChoice is rendered. Nothing is rendered if
 * @a whichChoice is -1.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void switch_node::render(openvrml::viewer & viewer, const rendering_context context)
{
    if (this->children_.value[0]) {
        this->children_.value[0]->render(viewer, context);
    }
    this->node::modified(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & switch_node::bounding_volume() const {
    if (this->bounding_volume_dirty()) {
        const_cast<switch_node *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<node_ptr> & switch_node::children() const throw ()
{
    return this->children_.value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void switch_node::activate(double time, bool isOver, bool isActive, double *p)
{
    const std::vector<node_ptr> & children = this->children();
    const node_ptr & node = children[0];
    if (node) {
        if (node->to_touch_sensor() && node->to_touch_sensor()->enabled()) {
            node->to_touch_sensor()->activate(time, isOver, isActive, p);
        } else if (node->to_plane_sensor()
                && node->to_plane_sensor()->enabled()) {
            node->to_plane_sensor()->activate(time, isActive, p);
        } else if (node->to_cylinder_sensor()
                && node->to_cylinder_sensor()->enabled()) {
            node->to_cylinder_sensor()->activate(time, isActive, p);
        } else if (node->to_sphere_sensor()
                && node->to_sphere_sensor()->isEnabled()) {
            node->to_sphere_sensor()->activate(time, isActive, p);
        }
    }
}

/**
 * @brief Recalculate the bounding volume.
 */
void switch_node::recalcBSphere()
{
    this->bsphere = bounding_sphere();
    long w = this->whichChoice.value;
    if (w >= 0 && size_t(w) < this->choice.value.size()) {
        const node_ptr & node = this->choice.value[w];
        if (node) {
            const openvrml::bounding_volume & ci_bv = node->bounding_volume();
            this->bsphere.extend(ci_bv);
        }
    }
    this->bounding_volume_dirty(false);
}

/**
 * @brief set_choice eventIn handler.
 *
 * @param value     an mfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void switch_node::process_set_choice(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->choice = dynamic_cast<const mfnode &>(value);
    const size_t whichChoice = size_t(this->whichChoice.value);
    this->children_.value[0] = (whichChoice < this->choice.value.size())
                             ? this->choice.value[whichChoice]
                             : node_ptr(0);
    this->node::modified(true);
    this->emit_event("choice_changed", this->choice, timestamp);
}

/**
 * @brief set_whichChoice eventIn handler.
 *
 * @param value     an sfint32 value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfint32.
 */
void switch_node::process_set_whichChoice(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast)
{
    this->whichChoice = dynamic_cast<const sfint32 &>(value);
    const size_t whichChoice = size_t(this->whichChoice.value);
    this->children_.value[0] = (whichChoice < this->choice.value.size())
                             ? this->choice.value[whichChoice]
                             : node_ptr(0);
    this->node::modified(true);
    this->emit_event("whichChoice_changed", this->whichChoice, timestamp);
}


/**
 * @class text_class
 *
 * @brief Class object for Text nodes.
 */

/**
 * @var FT_Library text_class::freeTypeLibrary
 *
 * @brief FreeType library handle.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Library
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
text_class::text_class(openvrml::browser & browser):
    node_class(browser)
{
# if OPENVRML_ENABLE_TEXT_NODE
    FT_Error error = 0;
    error = FT_Init_FreeType(&this->freeTypeLibrary);
    if (error) {
        browser.err << "Error initializing FreeType library." << std::endl;
    }
# endif // OPENVRML_ENABLE_TEXT_NODE
}

/**
 * @brief Destroy.
 */
text_class::~text_class() throw ()
{
# if OPENVRML_ENABLE_TEXT_NODE
    FT_Error error = 0;
    error = FT_Done_FreeType(this->freeTypeLibrary);
    if (error) {
        browser.err << "Error shutting down FreeType library." << std::endl;
    }
# endif // OPENVRML_ENABLE_TEXT_NODE
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Text nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by text_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
text_class::create_type(const std::string & id,
                        const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<text_node>(*this, id));
    vrml97_node_type_impl<text_node> & textNodeType =
        static_cast<vrml97_node_type_impl<text_node> &>(*type);
    typedef vrml97_node_type_impl<text_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            textNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &text_node::process_set_string,
                node_field_ptr_ptr(new node_field_ptr_impl<text_node, mfstring>
                                    (&text_node::string)));
        } else if (*interface == supportedInterfaces[1]) {
            textNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &text_node::process_set_fontStyle,
                node_field_ptr_ptr(new node_field_ptr_impl<text_node, sfnode>
                                    (&text_node::fontStyle)));
        } else if (*interface == supportedInterfaces[2]) {
            textNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &text_node::process_set_length,
                node_field_ptr_ptr(new node_field_ptr_impl<text_node, mffloat>
                                    (&text_node::length)));
        } else if (*interface == supportedInterfaces[3]) {
            textNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &text_node::process_set_maxExtent,
                node_field_ptr_ptr(new node_field_ptr_impl<text_node, sffloat>
                                    (&text_node::maxExtent)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class text_node
 *
 * @brief Text node instances.
 */

/**
 * @var text_node::text_class
 *
 * @brief Class object for Text instances.
 */

/**
 * @var mfstring text_node::string
 *
 * @brief string exposedField.
 */

/**
 * @var sfnode text_node::fontStyle
 *
 * @brief fontStyle exposedField.
 */

/**
 * @var mffloat text_node::length
 *
 * @brief length exposedField.
 */

/**
 * @var sffloat text_node::maxExtent
 *
 * @brief maxExtent exposedField.
 */

/**
 * @struct text_node::glyph_geometry
 *
 * @brief Used to hold the geometry of individual glyphs.
 */

/**
 * @var std::vector<vec2f> text_node::glyph_geometry::coord
 *
 * @brief Glyph coordinates.
 */

/**
 * @var std::vector<int32> text_node::glyph_geometry::coord_index
 *
 * @brief Glyph coordinate indices.
 */

/**
 * @var float text_node::glyph_geometry::advance_width
 *
 * @brief The distance the pen should advance horizontally after drawing the
 *      glyph.
 */

/**
 * @var float text_node::glyph_geometry::advance_height
 *
 * @brief The distance the pen should advance vertically after drawing the
 *      glyph.
 */

# ifdef OPENVRML_ENABLE_TEXT_NODE
namespace {

    /**
     * @brief Determine whether three vertices are ordered counter-clockwise.
     *
     * Does not throw.
     *
     * @param p0 first vertex.
     * @param p1 second vertex.
     * @param p2 third vertex.
     *
     * @return 1 if the vertices are counter-clockwise, -1 if the vertices are
     *         clockwise, or 0 if the vertices are neither.
     */
    int ccw_(const vec2f & p0, const vec2f & p1, const vec2f & p2) throw ()
    {
        const float dx1 = p1.x() - p0.x();
        const float dy1 = p1.y() - p0.y();
        const float dx2 = p2.x() - p0.x();
        const float dy2 = p2.y() - p0.y();

        if (dx1 * dy2 > dy1 * dx2) { return 1; }
        if (dx1 * dy2 < dy1 * dx2) { return -1; }
        if ((dx1 * dx2 < 0) || (dy1 * dy2 < 0)) return -1;
        if ((dx1 * dx1 + dy1 * dy1) < (dx2 * dx2 + dy2 * dy2)) { return 1; }
        return 0;
    }

    /**
     * @brief Determine whether two line segments intersect.
     *
     * Does not throw.
     *
     * @param l0p0 first point of the first line.
     * @param l0p1 second point of the first line.
     * @param l1p0 first point of the second line.
     * @param l1p1 second point of the second line.
     *
     * @return @c true if the line segments intersect; @c false otherwise.
     */
    bool intersect_(const vec2f & l0p0, const vec2f & l0p1,
                    const vec2f & l1p0, const vec2f & l1p1)
        throw ()
    {
        return ccw_(l0p0, l0p1, l1p0) * ccw_(l0p0, l0p1, l1p1) <= 0
            && ccw_(l1p0, l1p1, l0p0) * ccw_(l1p0, l1p1, l0p1) <= 0;
    }

    /**
     * @brief Determine whether a line segment intersects any line segments
     *        in a contour.
     *
     * Does not throw.
     *
     * @param v0      initial vertex of the line segment.
     * @param v1      final vertex of the line segment.
     * @param contour a contour (a series of line segments).
     *
     * @return @c true if the line segment defined by (@p v0, @p v1)
     *         intersects any line segment in @p contour; @c false otherwise.
     */
    bool intersects_segment_in_contour(const vec2f & v0,
                                       const vec2f & v1,
                                       const std::vector<vec2f> & contour)
        throw ()
    {
        for (size_t j = 0; j < contour.size() - 1; ++j) {
            //
            // Endpoints of the segment to test for intersection.
            //
            const vec2f & contour_v0 = contour[j];
            const vec2f & contour_v1 = contour[j + 1];
            //
            // We don't care if the endpoints match (and the intersection
            // test will treat this as an intersection).
            //
            if (contour_v0 == v0 || contour_v0 == v1
                || contour_v1 == v0 || contour_v1 == v1) { continue; }
            if (intersect_(v0, v1, contour_v0, contour_v1)) { return true; }
        }
        return false;
    }

    /**
     * @brief Get the exterior vertext that should be used to connect to the
     *      interior contour.
     *
     * Finds the first vertex in @p exteriorContour such that a line segment
     * from the interior contour vertex at @p interiorIndex through the
     * exterior contour vertex does not cross @p interiorContour.
     *
     * Does not throw.
     *
     * @param exterior_contour the exterior contour.
     * @param interior_contour the interior contour.
     * @param interior_index   the index of a vertex in @p interiorContour to
     *                         be used as the interior connecting vertex.
     *
     * @return the index of a vertex in @p exteriorContour that is usable as
     *         the exterior connecting vertex, or -1 if no such vertex is
     *         found.
     */
    long get_exterior_connecting_vertex_index_(
        const std::vector<vec2f> & exterior_contour,
        const std::vector<const std::vector<vec2f> *> & interior_contours,
        const vec2f & interior_vertex)
        throw ()
    {
        assert(exterior_contour.size() > 1);
        assert(!interior_contours.empty());

        typedef std::vector<const std::vector<vec2f> *> interior_contours_type;

        for (size_t i = 0; i < exterior_contour.size(); ++i) {
            const vec2f & exterior_vertex = exterior_contour[i];
            bool intersects_interior = false;
            for (interior_contours_type::const_iterator interior_contour =
                     interior_contours.begin();
                 interior_contour != interior_contours.end()
                     && !intersects_interior;
                 ++interior_contour) {
                assert(*interior_contour);
                if (intersects_segment_in_contour(interior_vertex,
                                                  exterior_vertex,
                                                  **interior_contour)) {
                    intersects_interior = true;
                }
            }
            if (!intersects_interior
                && !intersects_segment_in_contour(interior_vertex,
                                                  exterior_vertex,
                                                  exterior_contour)) {
                return i;
            }
        }
        return -1;
    }

    bool inside_contour_(const std::vector<vec2f> & contour,
                         const vec2f & point)
        throw ()
    {
        bool result = false;
        const size_t nvert = contour.size();
        for (size_t i = 0, j = nvert - 1; i < nvert; j = i++) {
            const vec2f & vi = contour[i];
            const vec2f & vj = contour[j];
            if ((((vi.y() <= point.y()) && (point.y() < vj.y()))
                        || ((vj.y() <= point.y()) && (point.y() < vi.y())))
                    && (point.x() < (vj.x() - vi.x())
                        * (point.y() - vi.y()) / (vj.y() - vi.y()) + vi.x())) {
                result = !result;
            }
        }
        return result;
    }

    enum contour_type_ { exterior_, interior_ };

    contour_type_ get_type_(const std::vector<vec2f> & contour,
                            const std::vector<std::vector<vec2f> > & contours)
        throw ()
    {
        using std::vector;

        assert(!contour.empty());
        const vec2f & vertex = contour[0];

        bool is_interior = false;
        for (vector<vector<vec2f> >::const_iterator test_contour =
                 contours.begin();
             test_contour != contours.end();
             ++test_contour) {
            if (&*test_contour == &contour) { continue; }
            if (inside_contour_(*test_contour, vertex)) {
                is_interior = !is_interior;
            }
        }
        return is_interior
            ? interior_
            : exterior_;
    }

    struct polygon_ {
        const std::vector<vec2f> * exterior;
        std::vector<const std::vector<vec2f> *> interiors;
    };

    struct inside_ : std::binary_function<const std::vector<vec2f> *,
                                          const std::vector<vec2f> *,
                                          bool> {
        bool operator()(const std::vector<vec2f> * const lhs,
                        const std::vector<vec2f> * const rhs) const
        {
            assert(lhs);
            assert(rhs);
            assert(!lhs->empty());
            //
            // Assume contours don't intersect. So if one point on lhs is
            // inside rhs, then assume all of lhs is inside rhs.
            //
            return inside_contour_(*rhs, lhs->front());
        }
    };

    const std::vector<polygon_>
    get_polygons_(const std::vector<std::vector<vec2f> > & contours)
        throw (std::bad_alloc)
    {
        using std::vector;
        typedef std::multiset<const vector<vec2f> *, inside_>
            segregated_contours;

        //
        // First, divide the contours into interior and exterior contours.
        //
        segregated_contours interiors, exteriors;
        for (vector<vector<vec2f> >::const_iterator contour = contours.begin();
             contour != contours.end();
             ++contour) {
            switch (get_type_(*contour, contours)) {
            case interior_:
                interiors.insert(&*contour);
                break;
            case exterior_:
                exteriors.insert(&*contour);
                break;
            default:
                assert(false);
            }
        }

        //
        // For each exterior, find its associated interiors and group them in
        // a Polygon_.
        //
        vector<polygon_> polygons;
        while (!exteriors.empty()) {
            polygon_ polygon;
            polygon.exterior = *exteriors.begin();
            segregated_contours::iterator interior = interiors.begin();
            while (interior != interiors.end()) {
                assert(!(*interior)->empty());
                if (inside_contour_(*polygon.exterior, (*interior)->front())) {
                    polygon.interiors.push_back(*interior);
                    segregated_contours::iterator next = interior;
                    ++next;
                    interiors.erase(interior);
                    interior = next;
                } else {
                    ++interior;
                }
            }
            polygons.push_back(polygon);
            exteriors.erase(exteriors.begin());
        }
        return polygons;
    }

    long get_vertex_index_(const std::vector<vec2f> & vertices,
                           const vec2f & vertex)
        throw ()
    {
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (vertices[i] == vertex) { return i; }
        }
        return -1;
    }
}
# endif // OPENVRML_ENABLE_TEXT_NODE

/**
 * @brief Construct from a set of contours.
 *
 * @param contours          a vector of closed contours that make up the
 *                          glyph's outline.
 * @param advance_width     the distance the pen should advance horizontally
 *                          after drawing the glyph.
 * @param advance_height    the distance the pen should advance vertically
 *                          after drawing the glyph.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
text_node::glyph_geometry::glyph_geometry(
        const std::vector<std::vector<vec2f> > & contours,
        const float advance_width,
        const float advance_height)
    throw (std::bad_alloc):
    advance_width(advance_width),
    advance_height(advance_height)
{
# ifdef OPENVRML_ENABLE_TEXT_NODE
    using std::vector;

    const vector<polygon_> polygons = get_polygons_(contours);
    for (vector<polygon_>::const_iterator polygon = polygons.begin();
         polygon != polygons.end();
         ++polygon) {
        //
        // connectionMap is keyed on a pointer to a vertex on the exterior
        // contour, and maps to a pointer to the interior contour whose
        // first vertex is closest to the exterior vertex.
        //
        typedef std::multimap<const vec2f *, const std::vector<vec2f> *>
            connection_map_t;
        connection_map_t connection_map;

        //
        // Fill connection_map. For each interior contour, find the exterior
        // vertex that is closest to the first vertex in the interior contour,
        // and the put the pair in the map.
        //
        for (vector<const vector<vec2f> *>::const_iterator interior =
                 polygon->interiors.begin();
             interior != polygon->interiors.end();
             ++interior) {
            assert(*interior);
            assert(!(*interior)->empty());
            long exterior_vertex_index =
                get_exterior_connecting_vertex_index_(*polygon->exterior,
                                                      polygon->interiors,
                                                      (*interior)->front());
            assert(exterior_vertex_index > -1);
            const vec2f * const exterior_vertex =
                    &(*polygon->exterior)[exterior_vertex_index];
            assert(exterior_vertex);
            const connection_map_t::value_type value(exterior_vertex,
                                                     *interior);
            connection_map.insert(value);
        }

        //
        // Finally, draw the polygon.
        //
        assert(!polygon->exterior->empty());
        for (size_t i = 0; i < polygon->exterior->size(); ++i) {
            const vec2f & exterior_vertex = (*polygon->exterior)[i];
            long exterior_index = get_vertex_index_(this->coord,
                                                    exterior_vertex);
            if (exterior_index > -1) {
                this->coord_index.push_back(exterior_index);
            } else {
                this->coord.push_back(exterior_vertex);
                assert(!this->coord.empty());
                exterior_index = this->coord.size() - 1;
                this->coord_index.push_back(exterior_index);
            }
            connection_map_t::iterator pos;
            while ((pos = connection_map.find(&exterior_vertex))
                    != connection_map.end()) {
                for (int i = pos->second->size() - 1; i > -1; --i) {
                    const vec2f & interior_vertex = (*pos->second)[i];
                    const long interior_index =
                        get_vertex_index_(this->coord, interior_vertex);
                    if (interior_index > -1) {
                        this->coord_index.push_back(interior_index);
                    } else {
                        this->coord.push_back(interior_vertex);
                        assert(!this->coord.empty());
                        this->coord_index.push_back(this->coord.size() - 1);
                    }
                }
                this->coord_index.push_back(exterior_index);
                connection_map.erase(pos);
            }
        }
        assert(connection_map.empty());
        this->coord_index.push_back(-1);
    }
# endif // OPENVRML_ENABLE_TEXT_NODE
}

/**
 * @struct text_node::text_geometry
 *
 * @brief Holds the text geometry.
 */

/**
 * @var std::vector<vec3f> text_node::text_geometry::coord
 *
 * @brief Text geometry coordinates.
 */

/**
 * @var std::vector<int32> text_node::text_geometry::coord_index
 *
 * @brief Text geometry coordinate indices.
 */

/**
 * @var std::vector<vec3f> text_node::text_geometry::normal
 *
 * @brief Text geometry normals.
 */

/**
 * @var std::vector<vec2f> text_node::text_geometry::tex_coord
 *
 * @brief Text geometry texture coordinates.
 */

/**
 * @typedef text_node::ucs4_string_t
 *
 * @brief A vector of FcChar32 vectors.
 */

/**
 * @typedef text_node::glyph_geometry_map_t
 *
 * @brief Maps FT_UInts to glyph_geometry.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-basic_types.html#FT_UInt
 */

/**
 * @var text_node::ucs4_string_t text_node::ucs4_string
 *
 * @brief UCS-4 equivalent of the (UTF-8) data in @a string.
 */

/**
 * @var FT_Face text_node::face
 *
 * @brief Handle to the font face.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Face
 */

/**
 * @var text_node::glyph_geometry_map_t text_node::glyph_geometry_map
 *
 * @brief Map of glyph indices to glyph_geometry.
 *
 * glyph_geometry instances are created as needed as new glyphs are
 * encountered. Once they are created, they are cached in the
 * glyph_geometry_map for rapid retrieval the next time the glyph is
 * encountered.
 */

/**
 * @var text_node::text_geometry text_node::text_geometry_
 *
 * @brief The text geometry.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
text_node::text_node(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    abstract_geometry_node(type, scope),
    face(0)
{}

/**
 * @brief Destroy.
 */
text_node::~text_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool text_node::modified() const
{
    return (this->node::modified()
            || (this->fontStyle.value && this->fontStyle.value->modified()));
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t text_node::insert_geometry(openvrml::viewer & viewer,
                                            const rendering_context context)
{
    const viewer::object_t retval =
        viewer.insert_shell(viewer::mask_ccw,
                            this->text_geometry_.coord,
                            this->text_geometry_.coord_index,
                            std::vector<openvrml::color>(), // color
                            std::vector<int32>(), // colorIndex
                            this->text_geometry_.normal,
                            std::vector<int32>(), // normalIndex
                            this->text_geometry_.tex_coord,
                            std::vector<int32>()); // texCoordIndex
    if (this->fontStyle.value) { this->fontStyle.value->modified(false); }
    return retval;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void text_node::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    this->update_ucs4();
    this->update_face();
    this->update_geometry();
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void text_node::do_shutdown(const double timestamp) throw ()
{
# if OPENVRML_ENABLE_TEXT_NODE
    if (this->face) {
        FT_Error ftError = FT_Done_Face(this->face);
        assert(ftError == FT_Err_Ok); // Surely this can't fail.
    }
# endif // OPENVRML_ENABLE_TEXT_NODE
}

/**
 * @brief set_string eventIn handler.
 *
 * @param value     an mfstring value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void text_node::process_set_string(const field_value & value,
                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->string = dynamic_cast<const mfstring &>(value);
    this->update_ucs4();
    this->update_geometry();
    this->node::modified(true);
    this->emit_event("string_changed", this->string, timestamp);
}

/**
 * @brief set_fontStyle eventIn handler.
 *
 * @param value     an sfnode value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfnode.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void text_node::process_set_fontStyle(const field_value & value,
                                const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->fontStyle = dynamic_cast<const sfnode &>(value);
    this->update_face();
    this->update_geometry();
    this->node::modified(true);
    this->emit_event("fontStyle_changed", this->fontStyle, timestamp);
}

/**
 * @brief set_length eventIn handler.
 *
 * @param value     an mffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void text_node::process_set_length(const field_value & value,
                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->length = dynamic_cast<const mffloat &>(value);
    this->update_geometry();
    this->node::modified(true);
    this->emit_event("length_changed", this->length, timestamp);
}

/**
 * @brief set_maxExtent eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void text_node::process_set_maxExtent(const field_value & value,
                                const double timestamp) throw (std::bad_cast)
{
    this->maxExtent = dynamic_cast<const sffloat &>(value);
    this->update_geometry();
    this->node::modified(true);
    this->emit_event("maxExtent_changed", this->maxExtent, timestamp);
}

/**
 * @brief Called when @a string changes to update the UCS-4 text.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void text_node::update_ucs4() throw (std::bad_alloc)
{
# ifdef OPENVRML_ENABLE_TEXT_NODE
    this->ucs4_string.clear();
    this->ucs4_string.resize(this->string.value.size());

    for (size_t i = 0; i < this->string.value.size(); ++i) {
        using std::string;
        using std::vector;

        const string & element = this->string.value[i];

        vector<FcChar32> & ucs4Element = this->ucs4_string[i];

        //
        // First, we need to convert the characters from UTF-8 to UCS-4.
        //
        vector<FcChar8> utf8String(element.begin(), element.end());
        int nchar = 0, wchar = 0;
        FcUtf8Len(&utf8String[0], utf8String.size(), &nchar, &wchar);
        ucs4Element.resize(nchar);
        {
            vector<FcChar8>::iterator utf8interface = utf8String.begin();
            vector<FcChar32>::iterator ucs4interface = ucs4Element.begin();
            while (utf8interface != utf8String.end()) {
                const int utf8bytes =
                    FcUtf8ToUcs4(&*utf8interface, &*ucs4interface,
                                 utf8String.end() - utf8interface);
                utf8interface += utf8bytes;
                ucs4interface++;
            }
        }
    }
# endif // OPENVRML_ENABLE_TEXT_NODE
}

# ifdef OPENVRML_ENABLE_TEXT_NODE
namespace {

    //
    // FcChar8_traits is a model of the standard library Character Traits
    // concept.
    //
    struct FcChar8_traits {
        typedef unsigned char char_type;
        typedef int int_type;
        typedef std::streampos pos_type;
        typedef std::streamoff off_type;
        typedef mbstate_t state_type;

        static void assign(char_type & c1, const char_type & c2);
        static bool eq(const char_type & c1, const char_type & c2);
        static bool lt(const char_type & c1, const char_type & c2);
        static int compare(const char_type * s1, const char_type * s2,
                           size_t n);
        static size_t length(const char_type * s);
        static const char_type * find(const char_type * s, size_t n,
                                      const char_type & a);
        static char_type * move(char_type * s1, const char_type * s2,
                                size_t n);
        static char_type * copy(char_type * s1, const char_type * s2,
                                size_t n);
        static char_type * assign(char_type * s, size_t n, char_type a);
        static int_type eof();
        static int_type not_eof(const int_type & c);
        static char_type to_char_type(const int_type & e);
        static int_type to_int_type(const char_type & c);
        static bool eq_int_type(const int_type & e1, const int_type & e2);
    };

    inline void FcChar8_traits::assign(char_type & c1, const char_type & c2)
    {
        c1 = c2;
    }

    inline bool FcChar8_traits::eq(const char_type & c1, const char_type & c2)
    {
        return c1 == c2;
    }

    inline bool FcChar8_traits::lt(const char_type & c1, const char_type & c2)
    {
        return c1 < c2;
    }

    inline int FcChar8_traits::compare(const char_type * s1,
                                       const char_type * s2,
                                       size_t n)
    {
        for (size_t i = 0; i < n; ++i) {
            if (!eq(s1[i], s2[i])) { return lt(s1[i], s2[i]) ? -1 : 1; }
        }
        return 0;
    }

    inline size_t FcChar8_traits::length(const char_type * s)
    {
        const char_type * p = s;
        while (*p) { ++p; }
        return (p - s);
    }

    inline FcChar8_traits::char_type *
    FcChar8_traits::move(char_type * s1, const char_type * s2, size_t n)
    {
        return reinterpret_cast<char_type *>(
            memmove(s1, s2, n * sizeof(char_type)));
    }

    inline const FcChar8_traits::char_type *
    FcChar8_traits::find(const char_type * s, size_t n, const char_type & a)
    {
        for (const char_type * p = s; size_t(p - s) < n; ++p) {
            if (*p == a) { return p; }
        }
        return 0;
    }

    inline FcChar8_traits::char_type *
    FcChar8_traits::copy(char_type * s1, const char_type * s2, size_t n)
    {
        return reinterpret_cast<char_type *>(
            memcpy(s1, s2, n * sizeof(char_type)));
    }

    inline FcChar8_traits::char_type *
    FcChar8_traits::assign(char_type * s, size_t n, char_type a)
    {
        for (char_type * p = s; p < s + n; ++p) { assign(*p, a); }
        return s;
    }

    inline FcChar8_traits::int_type FcChar8_traits::eof()
    {
        return static_cast<int_type>(-1);
    }

    inline FcChar8_traits::int_type FcChar8_traits::not_eof(const int_type & c)
    {
        return eq_int_type(c, eof()) ? int_type(0) : c;
    }

    inline FcChar8_traits::char_type
    FcChar8_traits::to_char_type(const int_type & e)
    {
        return char_type(e);
    }

    inline FcChar8_traits::int_type
    FcChar8_traits::to_int_type(const char_type & c)
    {
        return int_type(c);
    }

    inline bool FcChar8_traits::eq_int_type(const int_type & e1,
                                            const int_type & e2)
    {
        return e1 == e2;
    }
}
# endif // OPENVRML_ENABLE_TEXT_NODE

/**
 * @brief Called when @a fontStyle changes to update the font face.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void text_node::update_face() throw (std::bad_alloc)
{
# ifdef OPENVRML_ENABLE_TEXT_NODE
    static const char * const fcResultMessage[] = { "match",
                                                    "no match",
                                                    "type mismatch",
                                                    "no id" };

    class FontconfigError : public std::runtime_error {
    public:
        explicit FontconfigError(const FcResult result):
            std::runtime_error(fcResultMessage[result])
        {}

        virtual ~FontconfigError() throw ()
        {}
    };

    class FreeTypeError : public std::runtime_error {
    public:
        //
        // The normal build of FreeType doesn't include a means of mapping
        // error codes to human-readable strings.  There's a means of letting
        // client apps do this by defining some macros, but that's too much
        // trouble for now.
        //
        explicit FreeTypeError(const FT_Error error):
            std::runtime_error("FreeType error.")
        {}

        virtual ~FreeTypeError() throw ()
        {}
    };

    using std::string;
    typedef std::basic_string<FcChar8, FcChar8_traits> FcChar8_string;

    FcChar8_string language;

    std::vector<string> family;
    family.push_back("SERIF");

    string style;

    openvrml::font_style_node * const fontStyle = this->fontStyle.value
        ? this->fontStyle.value->to_font_style()
        : 0;
    if (fontStyle) {
        if (!fontStyle->family().empty()) {
            family = fontStyle->family();
            style = fontStyle->style();
            language.assign(fontStyle->language().begin(),
                            fontStyle->language().end());
        }
    }

    try {
        FcPattern * initialPattern = 0;
        FcPattern * matchedPattern = 0;

        try {
            using std::vector;

            string fontName;
            //
            // Set the family.
            //
            for (size_t i = 0; i < family.size(); ++i) {
                const std::string & element = family[i];
                if (element == "SERIF") {
                    fontName += "serif";
                } else if (element == "SANS") {
                    fontName += "sans";
                } else if (element == "TYPEWRITER") {
                    fontName += "monospace";
                } else {
                    fontName += element;
                }
                if (i + 1 < family.size()) { fontName += ", "; }
            }

            //
            // Set the weight.
            //
            if (style.find("BOLD") != string::npos) {
                fontName += ":bold";
            }

            //
            // Set the slant.
            //
            if (style.find("ITALIC") != string::npos) {
                fontName += ":italic";
            }

            //
            // For now, at least, we only want outline fonts.
            //
            fontName += ":outline=True";

            initialPattern =
                FcNameParse(FcChar8_string(fontName.begin(),
                                           fontName.end()).c_str());
            if (!initialPattern) { throw std::bad_alloc(); }

            //
            // Set the language.
            //
            if (!language.empty()) {
                FcPatternAddString(initialPattern, FC_LANG, language.c_str());
            }

            FcConfigSubstitute(0, initialPattern, FcMatchPattern);
            FcDefaultSubstitute(initialPattern);

            FcResult result = FcResultMatch;
            matchedPattern = FcFontMatch(0, initialPattern, &result);
            if (result != FcResultMatch) { throw FontconfigError(result); }
            assert(matchedPattern);

            FcChar8 * filename = 0;
            result = FcPatternGetString(matchedPattern, FC_FILE, 0, &filename);
            if (result != FcResultMatch) { throw FontconfigError(result); }

            int id = 0;
            result = FcPatternGetInteger(matchedPattern, FC_INDEX, 0, &id);
            if (result != FcResultMatch) { throw FontconfigError(result); }

            text_class & nodeClass =
                    static_cast<text_class &>(this->type.node_class);

            size_t filenameLen = 0;
            for (; filename[filenameLen]; ++filenameLen) {}

            const vector<char> ftFilename(filename,
                                          filename + filenameLen + 1);

            FT_Face newFace = 0;
            FT_Error ftError = FT_Err_Ok;
            ftError = FT_New_Face(nodeClass.freeTypeLibrary,
                                  &ftFilename[0], id, &newFace);
            if (ftError) { throw FreeTypeError(ftError); }

            if (this->face) {
                ftError = FT_Done_Face(this->face);
                assert(ftError == FT_Err_Ok); // Surely this can't fail.
            }

            this->face = newFace;
            this->glyph_geometry_map.clear();

            FcPatternDestroy(initialPattern);
            FcPatternDestroy(matchedPattern);
        } catch (std::runtime_error & ex) {
            FcPatternDestroy(initialPattern);
            FcPatternDestroy(matchedPattern);
            throw;
        }
    } catch (std::bad_alloc & ex) {
        throw;
    } catch (FontconfigError & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    } catch (FreeTypeError & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
# endif // OPENVRML_ENABLE_TEXT_NODE
}

# ifdef OPENVRML_ENABLE_TEXT_NODE
namespace {

    struct GlyphContours_ {
        const float scale;
        std::vector<std::vector<vec2f> > contours;

        explicit GlyphContours_(float scale);
    };

    GlyphContours_::GlyphContours_(const float scale):
        scale(scale)
    {}

    const float stepSize_ = 0.2;

    int moveTo_(FT_Vector * const to, void * const user) throw ()
    {
        using std::vector;
        assert(user);
        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);
        try {
            c.contours.push_back(vector<vec2f>(1));
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        const vec2f vertex(to->x * c.scale, to->y * c.scale);
        c.contours.back().front() = vertex;
        return 0;
    }

    int lineTo_(FT_Vector * const to, void * const user) throw ()
    {
        assert(user);
        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);
        const vec2f vertex(to->x * c.scale, to->y * c.scale);
        try {
            c.contours.back().push_back(vertex);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }

    /**
     * @brief de Casteljau's algorithm.
     *
     * This is a nice recursive algorithm defined by de-Casteljau which
     * calculates for a given control polygon the point that lies on the bezier
     * curve for any value of t, and can be used to evaluate and draw the
     * Bezier spline without using the Bernstein polynomials.
     *
     * The algorithm advances by creating in each step a polygons of degree one
     * less than the one created in the previous step until there is only one
     * point left, which is the point on the curve. The polygon vertices for
     * each step are defined by linear interpolation of two consecutive
     * vertices of the polygon from the previous step with a value of t (the
     * parameter):
     *
     * @param buffer    an array including the control points for the curve in
     *                  the first @p npoints elements. The total size of the
     *                  array must be @p npoints * @p npoints. The remaining
     *                  elements of the array will be used by the algorithm to
     *                  store temporary values.
     * @param npoints   the number of control points.
     * @param contour   the points on the curve are added to this array.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void evaluateCurve_(vec2f * const buffer,
                        const size_t npoints,
                        std::vector<vec2f> & contour)
        throw (std::bad_alloc)
    {
        for (size_t i = 1; i <= (1 / stepSize_); i++){
            const float t = i * stepSize_; // Parametric points 0 <= t <= 1
            for (size_t j = 1; j < npoints; j++) {
                for (size_t k = 0; k < (npoints - j); k++) {
                    buffer[j * npoints + k][0] =
                            (1 - t) * buffer[(j - 1) * npoints + k][0]
                            + t * buffer[(j - 1) * npoints + k + 1][0];
                    buffer[j * npoints + k][1] =
                            (1 - t) * buffer[(j - 1) * npoints + k][1]
                            + t * buffer[(j - 1) * npoints + k + 1][1];
                }
            }
            //
            // Specify next vertex to be included on curve
            //
            contour.push_back(buffer[(npoints - 1) * npoints]); // throws std::bad_alloc
        }
    }

    int conicTo_(FT_Vector * const control,
                 FT_Vector * const to,
                 void * const user)
        throw ()
    {
        using std::vector;

        assert(control);
        assert(to);
        assert(user);

        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);

        assert(!c.contours.empty());
        vector<vec2f> & contour = c.contours.back();
        const vec2f & lastVertex = contour[contour.size() - 1];

        assert(!contour.empty());
        const size_t npoints = 3;
        vec2f buffer[npoints * npoints] = {
            vec2f(lastVertex[0], lastVertex[1]),
            vec2f(control->x * c.scale, control->y * c.scale),
            vec2f(to->x * c.scale, to->y * c.scale)
        };

        try {
            evaluateCurve_(buffer, npoints, contour);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }

    int cubicTo_(FT_Vector * const control1,
                 FT_Vector * const control2,
                 FT_Vector * const to,
                 void * const user)
        throw ()
    {
        using std::vector;

        assert(control1);
        assert(control2);
        assert(to);
        assert(user);

        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);

        assert(!c.contours.empty());
        vector<vec2f> & contour = c.contours.back();
        assert(!contour.empty());
        const vec2f & lastVertex = contour.back();

        static const size_t npoints = 4;
        vec2f buffer[npoints * npoints] = {
            vec2f(lastVertex[0], lastVertex[1]),
            vec2f(control1->x * c.scale, control1->y * c.scale),
            vec2f(control2->x * c.scale, control2->y * c.scale),
            vec2f(to->x * c.scale, to->y * c.scale)
        };

        try {
            evaluateCurve_(buffer, npoints, contour);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }
}
# endif // OPENVRML_ENABLE_TEXT_NODE

/**
 * @brief Called to update @a text_geometry.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void text_node::update_geometry() throw (std::bad_alloc)
{
# ifdef OPENVRML_ENABLE_TEXT_NODE
    using std::pair;
    using std::string;
    using std::vector;

    bool horizontal = true;
    string justify[2] = { "BEGIN", "FIRST" };
    bool leftToRight = true;
    bool topToBottom = true;
    float size = 1.0;
    float spacing = 1.0;
    openvrml::font_style_node * fontStyle;
    if (this->fontStyle.value
            && (fontStyle = this->fontStyle.value->to_font_style())) {
        horizontal = fontStyle->horizontal();
        if (!fontStyle->justify().empty()) {
            justify[0] = fontStyle->justify()[0];
        }
        if (fontStyle->justify().size() > 1) {
            justify[1] = fontStyle->justify()[1];
        }
        leftToRight = fontStyle->left_to_right();
        topToBottom = fontStyle->top_to_bottom();
        size = fontStyle->size();
        spacing = fontStyle->spacing();
    }

    text_geometry newGeometry;
    float geometryXMin = 0.0, geometryXMax = 0.0;
    float geometryYMin = 0.0, geometryYMax = 0.0;
    size_t npolygons = 0;
    const ucs4_string_t::const_iterator stringBegin =
        this->ucs4_string.begin();
    for (ucs4_string_t::const_iterator string = stringBegin;
         string != this->ucs4_string.end();
         ++string) {
        float penPos[2] = { 0.0, 0.0 };
        const size_t line = std::distance(stringBegin, string);
        const float lineAdvance = size * spacing * line;
        if (horizontal) {
            if (topToBottom) {
                penPos[1] -= lineAdvance;
            } else {
                penPos[1] += lineAdvance;
            }
        } else {
            if (leftToRight) {
                penPos[0] += lineAdvance;
            } else {
                penPos[0] -= lineAdvance;
            }
        }

        struct LineGeometry {
            vector<vec2f> coord;
            vector<int32> coordIndex;
            float xMin, xMax;
            float yMin, yMax;
        };

        LineGeometry lineGeometry = {};
        for (vector<FcChar32>::const_iterator character = string->begin();
                character != string->end(); ++character) {
            assert(this->face);
            const FT_UInt glyphIndex =
                    FcFreeTypeCharIndex(this->face, *character);

            const glyph_geometry * glyphGeometry = 0;
            const glyph_geometry_map_t::iterator pos =
                    this->glyph_geometry_map.find(glyphIndex);
            if (pos != this->glyph_geometry_map.end()) {
                glyphGeometry = &pos->second;
            } else {
                FT_Error error = FT_Err_Ok;
                error = FT_Load_Glyph(this->face,
                                      glyphIndex,
                                      FT_LOAD_NO_SCALE);
                assert(error == FT_Err_Ok);
                FT_Glyph glyph;
                error = FT_Get_Glyph(this->face->glyph, &glyph);
                assert(error == FT_Err_Ok);
                static FT_Outline_Funcs outlineFuncs = { moveTo_,
                                                         lineTo_,
                                                         conicTo_,
                                                         cubicTo_,
                                                         0,
                                                         0 };
                const float glyphScale = (this->face->bbox.yMax > 0.0)
                                       ? size / this->face->bbox.yMax
                                       : size;
                GlyphContours_ glyphContours(glyphScale);
                assert(glyph->format == ft_glyph_format_outline);
                const FT_OutlineGlyph outlineGlyph =
                        reinterpret_cast<FT_OutlineGlyph>(glyph);
                error = FT_Outline_Decompose(&outlineGlyph->outline,
                                             &outlineFuncs,
                                             &glyphContours);
                assert(error == FT_Err_Ok);

                assert(this->face->glyph);
                const float advanceWidth =
                        FT_HAS_HORIZONTAL(this->face)
                        ? this->face->glyph->metrics.horiAdvance * glyphScale
                        : 0.0;
                const float advanceHeight =
                        FT_HAS_VERTICAL(this->face)
                        ? this->face->glyph->metrics.vertAdvance * glyphScale
                        : 0.0;

                const glyph_geometry_map_t::value_type
                        value(glyphIndex,
                              glyph_geometry(glyphContours.contours,
                                             advanceWidth,
                                             advanceHeight));
                const pair<glyph_geometry_map_t::iterator, bool> result =
                        this->glyph_geometry_map.insert(value);
                assert(result.second);
                glyphGeometry = &result.first->second;
            }

            for (size_t i = 0; i < glyphGeometry->coord.size(); ++i) {
                const vec2f & glyphVertex = glyphGeometry->coord[i];
                const vec2f textVertex(glyphVertex[0] + penPos[0],
                                       glyphVertex[1] + penPos[1]);
                lineGeometry.coord.push_back(textVertex);
                lineGeometry.xMin = (lineGeometry.xMin < textVertex[0])
                                  ? lineGeometry.xMin
                                  : textVertex[0];
                lineGeometry.xMax = (lineGeometry.xMax > textVertex[0])
                                  ? lineGeometry.xMax
                                  : textVertex[0];
                lineGeometry.yMin = (lineGeometry.yMin < textVertex[1])
                                  ? lineGeometry.yMin
                                  : textVertex[1];
                lineGeometry.yMax = (lineGeometry.yMax > textVertex[1])
                                  ? lineGeometry.yMax
                                  : textVertex[1];
            }

            for (size_t i = 0; i < glyphGeometry->coord_index.size(); ++i) {
                const long index = glyphGeometry->coord_index[i];
                if (index > -1) {
                    const size_t offset = lineGeometry.coord.size()
                                          - glyphGeometry->coord.size();
                    lineGeometry.coordIndex.push_back(offset + index);
                } else {
                    lineGeometry.coordIndex.push_back(-1);
                    ++npolygons;
                }
            }
            if (horizontal) {
                const float xAdvance = glyphGeometry->advance_width;
                if (leftToRight) {
                    penPos[0] += xAdvance;
                } else {
                    penPos[0] -= xAdvance;
                }
            } else {
                const float yAdvance = glyphGeometry->advance_height;
                if (topToBottom) {
                    penPos[1] -= yAdvance;
                } else {
                    penPos[1] += yAdvance;
                }
            }
        }

        //
        // Scale to length.
        //
        const float length = (line < this->length.value.size())
                           ? this->length.value[line]
                           : 0.0;
        if (length > 0.0) {
            const float currentLength = lineGeometry.xMax - lineGeometry.xMin;
            for (size_t i = 0; i < lineGeometry.coord.size(); ++i) {
                const vec2f & vertex = lineGeometry.coord[i];
                const vec2f scaledVertex(vertex[0] / currentLength * length,
                                         vertex[1]);
                lineGeometry.coord[i] = scaledVertex;
            }
        }

        //
        // Add the line to the text geometry. We need to adjust for the major
        // alignment.
        //
        float xOffset = 0.0f, yOffset = 0.0f;
        //
        // Offset is 0 for "BEGIN" or "FIRST" (or anything else, in our case).
        //
        if (justify[0] == "MIDDLE") {
            if (horizontal) {
                xOffset = -((lineGeometry.xMax - lineGeometry.xMin) / 2.0f);
            } else {
                yOffset = (lineGeometry.yMax - lineGeometry.yMin) / 2.0f;
            }
        } else if (justify[0] == "END") {
            if (horizontal) {
                xOffset = -(lineGeometry.xMax - lineGeometry.xMin);
            } else {
                yOffset = lineGeometry.yMax - lineGeometry.yMin;
            }
        }
        for (size_t i = 0; i < lineGeometry.coordIndex.size(); ++i) {
            const long index = lineGeometry.coordIndex[i];
            if (index > -1) {
                const vec2f & lineVertex = lineGeometry.coord[index];
                const vec3f textVertex(lineVertex.x() + xOffset,
                                       lineVertex.y() + yOffset,
                                       0.0f);
                newGeometry.coord.push_back(textVertex);
                newGeometry.coord_index
                    .push_back(newGeometry.coord.size() - 1);
                geometryXMin = (geometryXMin < textVertex.x())
                             ? geometryXMin
                             : textVertex.x();
                geometryXMax = (geometryXMax > textVertex.x())
                             ? geometryXMax
                             : textVertex.x();
                geometryYMin = (geometryYMin < textVertex.y())
                             ? geometryYMin
                             : textVertex.y();
                geometryYMax = (geometryYMax > textVertex.y())
                             ? geometryYMax
                             : textVertex.y();
            } else {
                newGeometry.coord_index.push_back(-1);
            }
        }
    }

    //
    // Scale to maxExtent.
    //
    const float maxExtent = (this->maxExtent.value > 0.0)
                          ? this->maxExtent.value
                          : 0.0;
    if (maxExtent > 0.0) {
        const float currentMaxExtent = geometryXMax - geometryXMin;
        if (currentMaxExtent > maxExtent) {
            for (size_t i = 0; i < newGeometry.coord.size(); ++i) {
                const vec3f & vertex = newGeometry.coord[i];
                const vec3f scaledVertex(
                    vertex.x() / currentMaxExtent * maxExtent,
                    vertex.y(),
                    vertex.z()
                );
                newGeometry.coord[i] = scaledVertex;
            }
        }
    }

    //
    // Adjust for the minor alignment.
    //
    float xOffset = 0.0f, yOffset = 0.0f;
    if (justify[1] == "FIRST" || justify[1] == "") {
    } else if (justify[1] == "BEGIN") {
        if (horizontal) {
            yOffset = -(size * spacing);
        } else {
            xOffset = 0.0f;
        }
    } else if (justify[1] == "MIDDLE") {
        if (horizontal) {
            yOffset = ((size * spacing * this->string.value.size()) / 2.0f)
                      - (size * spacing);
        } else {
            xOffset = ((size * spacing * this->string.value.size()) / 2.0f)
                      - (size * spacing);
        }
    } else if (justify[1] == "END") {
        if (horizontal) {
            yOffset = size * spacing * (this->string.value.size() - 1);
        } else {
            xOffset = size * spacing * (this->string.value.size() - 1);
        }
    }
    for (size_t i = 0; i < newGeometry.coord.size(); ++i) {
        const vec3f & vertex = newGeometry.coord[i];
        const vec3f adjustedVertex(vertex.x() + xOffset,
                                   vertex.y() + yOffset,
                                   vertex.z());
        newGeometry.coord[i] = adjustedVertex;
    }

    //
    // Create the normals.
    //
    newGeometry.normal.resize(npolygons); // Throws std::bad_alloc.
    for (size_t i = 0; i < newGeometry.normal.size(); ++i) {
        static const vec3f normal(0.0, 0.0, 1.0);
        newGeometry.normal[i] = normal;
    }

    //
    // Create the texture coordinates.
    //
    newGeometry.tex_coord.resize(newGeometry.coord.size()); // std::bad_alloc
    for (size_t i = 0; i < newGeometry.tex_coord.size(); ++i) {
        const vec3f & vertex = newGeometry.coord[i];
        newGeometry.tex_coord[i] = vec2f(vertex.x() / size, vertex.y() / size);
    }

    this->text_geometry_ = newGeometry;
# endif // OPENVRML_ENABLE_TEXT_NODE
}


/**
 * @class texture_coordinate_class
 *
 * @brief Class object for TextureCoordinate nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
texture_coordinate_class::texture_coordinate_class(
    openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
texture_coordinate_class::~texture_coordinate_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating TextureCoordinate
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by texture_coordinate_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
texture_coordinate_class::create_type(const std::string & id,
                                      const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::exposedfield_id,
                      field_value::mfvec2f_id,
                      "point");
    const node_type_ptr
        type(new vrml97_node_type_impl<texture_coordinate_node>(*this, id));
    vrml97_node_type_impl<texture_coordinate_node> & textureCoordinateNodeType =
            static_cast<vrml97_node_type_impl<texture_coordinate_node> &>(*type);
    typedef vrml97_node_type_impl<texture_coordinate_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterface) {
            textureCoordinateNodeType.add_exposedfield(
                supportedInterface.field_type,
                supportedInterface.id,
                &texture_coordinate_node::process_set_point,
                node_field_ptr_ptr(new node_field_ptr_impl<texture_coordinate_node,
                                                     mfvec2f>
                                (&texture_coordinate_node::point_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class texture_coordinate_node
 *
 * @brief TextureCoordinate node instances.
 */

/**
 * @var texture_coordinate_node::texture_coordinate_class
 *
 * @brief Class object for TextureCoordinate instances.
 */

/**
 * @var mfvec2f texture_coordinate_node::point_
 *
 * @brief point exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
texture_coordinate_node::texture_coordinate_node(const node_type & type,
                                                 const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::texture_coordinate_node(type, scope)
{}

/**
 * @brief Destroy.
 */
texture_coordinate_node::~texture_coordinate_node() throw () {}

/**
 * @brief Get the points encapsulated by this node.
 *
 * @return the mfvec2f array of points for this node.
 */
const std::vector<vec2f> & texture_coordinate_node::point() const throw ()
{
    return this->point_.value;
}

/**
 * @brief set_point eventIn handler.
 *
 * @param value     an array of vectors representing points.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec2f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void texture_coordinate_node::process_set_point(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->point_ = dynamic_cast<const mfvec2f &>(value);
    this->node::modified(true);
    this->emit_event("point_changed", this->point_, timestamp);
}


/**
 * @class texture_transform_class
 *
 * @brief Class object for TextureTransform nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
texture_transform_class::texture_transform_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
texture_transform_class::~texture_transform_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating TextureTransform
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by texture_transform_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
texture_transform_class::create_type(const std::string & id,
                                     const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "center"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "rotation"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "scale"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "translation")
    };
    const node_type_ptr type(new vrml97_node_type_impl<texture_transform_node>(*this, id));
    vrml97_node_type_impl<texture_transform_node> & textureTransformNodeType =
            static_cast<vrml97_node_type_impl<texture_transform_node> &>(*type);
    typedef vrml97_node_type_impl<texture_transform_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            textureTransformNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &texture_transform_node::process_set_center,
                node_field_ptr_ptr(new node_field_ptr_impl<texture_transform_node, sfvec2f>
                                    (&texture_transform_node::center)));
        } else if (*interface == supportedInterfaces[1]) {
            textureTransformNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &texture_transform_node::process_set_rotation,
                node_field_ptr_ptr(new node_field_ptr_impl<texture_transform_node, sffloat>
                                    (&texture_transform_node::rotation)));
        } else if (*interface == supportedInterfaces[2]) {
            textureTransformNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &texture_transform_node::process_set_scale,
                node_field_ptr_ptr(new node_field_ptr_impl<texture_transform_node, sfvec2f>
                                    (&texture_transform_node::scale)));
        } else if (*interface == supportedInterfaces[3]) {
            textureTransformNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &texture_transform_node::process_set_translation,
                node_field_ptr_ptr(new node_field_ptr_impl<texture_transform_node, sfvec2f>
                                    (&texture_transform_node::translation)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class texture_transform_node
 *
 * @brief TextureTransform node instances.
 */

/**
 * @var texture_transform_node::texture_transform_class
 *
 * @brief Class object for TextureTransform instances.
 */

/**
 * @var sfvec2f texture_transform_node::center
 *
 * @brief center exposedField.
 */

/**
 * @var sffloat texture_transform_node::rotation
 *
 * @brief rotation exposedField.
 */

/**
 * @var sfvec2f texture_transform_node::scale
 *
 * @brief scale exposedField.
 */

/**
 * @var sfvec2f texture_transform_node::translation
 *
 * @brief translation exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
texture_transform_node::texture_transform_node(const node_type & type,
                                               const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::texture_transform_node(type, scope),
    center(vec2f(0.0, 0.0)),
    rotation(0.0),
    scale(vec2f(1.0, 1.0)),
    translation(vec2f(0.0, 0.0))
{}

/**
 * @brief Destroy.
 */
texture_transform_node::~texture_transform_node() throw ()
{}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void texture_transform_node::render(openvrml::viewer & viewer,
                                    const rendering_context context)
{
    viewer.set_texture_transform(this->center.value,
                                 this->rotation.value,
                                 this->scale.value,
                                 this->translation.value);
    this->node::modified(false);
}

/**
 * @brief set_center eventIn handler.
 *
 * @param value     an sfvec2f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec2f.
 */
void texture_transform_node::process_set_center(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->center = dynamic_cast<const sfvec2f &>(value);
    this->node::modified(true);
    this->emit_event("center_changed", this->center, timestamp);
}

/**
 * @brief set_rotation eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void texture_transform_node::process_set_rotation(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast)
{
    this->rotation = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("rotation_changed", this->rotation, timestamp);
}

/**
 * @brief set_scale eventIn handler.
 *
 * @param value     an sfvec2f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec2f.
 */
void texture_transform_node::process_set_scale(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast)
{
    this->scale = dynamic_cast<const sfvec2f &>(value);
    this->node::modified(true);
    this->emit_event("scale_changed", this->scale, timestamp);
}

/**
 * @brief set_translation eventIn handler.
 *
 * @param value     an sfvec2f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec2f.
 */
void texture_transform_node::process_set_translation(const field_value & value,
                                                     const double timestamp)
    throw (std::bad_cast)
{
    this->translation = dynamic_cast<const sfvec2f &>(value);
    this->node::modified(true);
    this->emit_event("translation_changed", this->translation, timestamp);
}


/**
 * @class time_sensor_class
 *
 * @brief Class object for TimeSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser   the browser associated with this node_class.
 */
time_sensor_class::time_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
time_sensor_class::~time_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating TimeSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by time_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
time_sensor_class::create_type(const std::string & id,
                               const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef time_sensor_node node_t;
    typedef vrml97_node_type_impl<time_sensor_node> node_type_t;

    const node_type_ptr type(new node_type_t(*this, id));
    node_type_t & timeSensorNodeType = static_cast<node_type_t &>(*type);
    typedef node_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &time_sensor_node::process_set_cycleInterval,
                node_field_ptr_ptr(
                    new node_type_t::sftime_ptr(&node_t::cycleInterval)));
        } else if (*interface == supportedInterfaces[1]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &time_sensor_node::process_set_enabled,
                node_field_ptr_ptr(
                    new node_type_t::sfbool_ptr(&node_t::enabled)));
        } else if (*interface == supportedInterfaces[2]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &time_sensor_node::process_set_loop,
                node_field_ptr_ptr(
                    new node_type_t::sfbool_ptr(&node_t::loop)));
        } else if (*interface == supportedInterfaces[3]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &time_sensor_node::process_set_startTime,
                node_field_ptr_ptr(
                    new node_type_t::sftime_ptr(&node_t::startTime)));
        } else if (*interface == supportedInterfaces[4]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &time_sensor_node::process_set_stopTime,
                node_field_ptr_ptr(
                    new node_type_t::sftime_ptr(&node_t::stopTime)));
        } else if (*interface == supportedInterfaces[5]) {
            timeSensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(
                    new node_type_t::sftime_ptr(&node_t::cycleTime)));
        } else if (*interface == supportedInterfaces[6]) {
            timeSensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(
                    new node_type_t::sffloat_ptr(&node_t::fraction)));
        } else if (*interface == supportedInterfaces[7]) {
            timeSensorNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(
                    new node_type_t::sfbool_ptr(&node_t::active)));
        } else if (*interface == supportedInterfaces[8]) {
            timeSensorNodeType.add_eventout(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_field_ptr_ptr(
                    new node_type_t::sftime_ptr(&node_t::time)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class time_sensor_node
 *
 * @brief TimeSensor node instances.
 */

/**
 * @var time_sensor_node::time_sensor_class
 *
 * @brief Class object for TimeSensor instances.
 */

/**
 * @var sftime time_sensor_node::cycleInterval
 *
 * @brief cycleInterval exposedField.
 */

/**
 * @var sfbool time_sensor_node::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfbool time_sensor_node::loop
 *
 * @brief loop exposedField.
 */

/**
 * @var sftime time_sensor_node::startTime
 *
 * @brief startTime exposedField.
 */

/**
 * @var sftime time_sensor_node::stopTime
 *
 * @brief stopTime exposedField.
 */

/**
 * @var sftime time_sensor_node::cycleTime
 *
 * @brief cycleTime eventOut.
 */

/**
 * @var sffloat time_sensor_node::fraction
 *
 * @brief fraction_changed eventOut.
 */

/**
 * @var sfbool time_sensor_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sftime time_sensor_node::time
 *
 * @brief time eventOut.
 */

/**
 * @var double time_sensor_node::lastTime
 *
 * @brief The timestamp previously received.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
time_sensor_node::time_sensor_node(const node_type & type,
                                   const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    cycleInterval(1.0),
    enabled(true),
    loop(false),
    startTime(0.0),
    stopTime(0.0),
    active(false),
    lastTime(-1.0)
{}

/**
 * @brief Destroy.
 */
time_sensor_node::~time_sensor_node() throw ()
{}

/**
 * @brief Cast to a time_sensor_node.
 *
 * @return a pointer to the object.
 */
time_sensor_node * time_sensor_node::to_time_sensor() const
{
    return (time_sensor_node*) this;
}

/**
 * Generate timer events. If necessary, events prior to the timestamp (inTime)
 * are generated to respect stopTimes and cycleIntervals. The timestamp
 * should never be increased. This assumes the event loop delivers pending
 * events in order (ascending time stamps). Should inTime be modified?
 * Should ensure continuous events are delivered before discrete ones
 * (such as cycleTime, isActive).
 */
void time_sensor_node::update(const double currentTime)
{
    sftime timeNow(currentTime);

    if (this->enabled.value) {
        if (this->lastTime > timeNow.value) { this->lastTime = timeNow.value; }

        // Become active at startTime if either the valid stopTime hasn't
        // passed or we are looping.
        if (!this->active.value
                && this->startTime.value <= timeNow.value
                && this->startTime.value >= this->lastTime
                && ((this->stopTime.value < this->startTime.value
                    || this->stopTime.value > timeNow.value)
                    || this->loop.value)) {
            this->active.value = true;

            // Start at first tick >= startTime
            this->emit_event("isActive", this->active, timeNow.value);
            this->emit_event("time", timeNow, timeNow.value);
            this->emit_event("fraction_changed", sffloat(0.0), timeNow.value);
            this->emit_event("cycleTime", timeNow, timeNow.value);
        }

        // Running (active and enabled)
        else if (this->active.value) {
            using openvrml_::fequal;
            using openvrml_::fless_equal;

            double f, cycleInt = this->cycleInterval.value;
            bool deactivate = false;

            // Are we done? Choose min of stopTime or start + single cycle.
            if ((this->stopTime.value > this->startTime.value
                        && fless_equal<double>()(this->stopTime.value,
                                                 timeNow.value))
                    || (!this->loop.value
                        && fless_equal<double>()(this->startTime.value
                                                 + cycleInt,
                                                 timeNow.value))) {
                this->active.value = false;

                // Must respect stopTime/cycleInterval exactly
                if (this->startTime.value + cycleInt < this->stopTime.value) {
                    timeNow = sftime(this->startTime.value + cycleInt);
                } else {
                    timeNow = this->stopTime;
                }

                deactivate = true;
            }

            f = (cycleInt > 0.0 && timeNow.value > this->startTime.value)
              ? fmod(timeNow.value - this->startTime.value, cycleInt)
              : 0.0;

            fequal<double> feq;

            // Fraction of cycle message
            sffloat fraction_changed(feq(f, 0.0)
                                     ? 1.0f
                                     : float(f / cycleInt));
            this->emit_event("fraction_changed",
                             fraction_changed,
                             timeNow.value);

            // Current time message
            this->emit_event("time", timeNow, timeNow.value);

            // End of cycle message (this may miss cycles...)
            if (feq(fraction_changed.value, 1.0)) {
                this->emit_event("cycleTime", timeNow, timeNow.value);
            }

            if (deactivate) {
                this->emit_event("isActive", this->active, timeNow.value);
            }
        }

        // Tell the scene this node needs quick updates while it is active.
        // Should check whether time, fraction_changed eventOuts are
        // being used, and set delta to cycleTime if not...
        if (this->active.value) {
            this->type.node_class.browser.delta(0.0);
        }
        this->lastTime = currentTime;
    }
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void time_sensor_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_time_sensor(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void time_sensor_node::do_shutdown(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.remove_time_sensor(*this);
}

/**
 * @brief set_cycleInterval eventIn handler.
 *
 * @param value     an sftime value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sftime.
 */
void time_sensor_node::process_set_cycleInterval(const field_value & value,
                                                 const double timestamp)
    throw (std::bad_cast)
{
    if (!this->active.value) {
        this->cycleInterval = dynamic_cast<const sftime &>(value);
        this->lastTime = timestamp;
        this->emit_event("cycleInterval_changed", this->cycleInterval,
                        timestamp);
    }
}

/**
 * @brief set_enabled eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void time_sensor_node::process_set_enabled(const field_value & value,
                                           const double timestamp)
    throw (std::bad_cast)
{
    this->enabled = dynamic_cast<const sfbool &>(value);
    if (this->enabled.value != this->active.value) {
        if (this->active.value) {
            using openvrml_::fequal;

            //
            // Was active; shutdown.
            //
            double cycleInt = this->cycleInterval.value;
            double f = (cycleInt > 0.0)
                     ? fmod(this->time.value - this->startTime.value, cycleInt)
                     : 0.0;

            // Fraction of cycle message
            this->fraction.value =
                float(fequal<double>()(f, 0.0) ? 1.0 : (f / cycleInt));
        } else {
            //
            // Was inactive; startup.
            //
            this->cycleTime.value = timestamp;
            this->emit_event("cycleTime", this->cycleTime, timestamp);

            // Fraction of cycle message
            this->fraction.value = 0.0;
        }
        this->time.value = timestamp;
        this->emit_event("time", this->time, timestamp);
        this->emit_event("fraction_changed", this->fraction, timestamp);
        this->active = this->enabled;
        this->emit_event("isActive", this->active, timestamp);
    }
    this->emit_event("enabled_changed", this->enabled, timestamp);
}

/**
 * @brief set_loop eventIn handler.
 *
 * @param value    an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void time_sensor_node::process_set_loop(const field_value & value,
                                        const double timestamp)
    throw (std::bad_cast)
{
    this->loop = dynamic_cast<const sfbool &>(value);
    this->emit_event("loop_changed", this->loop, timestamp);
}

/**
 * @brief set_startTime eventIn handler.
 *
 * @param value     an sftime value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sftime.
 */
void time_sensor_node::process_set_startTime(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    if (!this->active.value) {
        this->startTime = dynamic_cast<const sftime &>(value);
        this->lastTime = timestamp;
        this->emit_event("startTime_changed", this->startTime, timestamp);
    }
}

/**
 * @brief set_stopTime eventIn handler.
 *
 * @param value     an sftime value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sftime.
 */
void time_sensor_node::process_set_stopTime(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast)
{
    this->stopTime = dynamic_cast<const sftime &>(value);
    this->emit_event("stopTime_changed", this->stopTime, timestamp);
}


/**
 * @class touch_sensor_class
 *
 * @brief Class object for TouchSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
touch_sensor_class::touch_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
touch_sensor_class::~touch_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating TouchSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by touch_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
touch_sensor_class::create_type(const std::string & id,
                                const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<touch_sensor_node>(*this, id));
    vrml97_node_type_impl<touch_sensor_node> & touchSensorNodeType =
            static_cast<vrml97_node_type_impl<touch_sensor_node> &>(*type);
    typedef vrml97_node_type_impl<touch_sensor_node>::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            touchSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &touch_sensor_node::process_set_enabled,
                node_field_ptr_ptr(new node_field_ptr_impl<touch_sensor_node, sfbool>
                                    (&touch_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[1]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(new node_field_ptr_impl<touch_sensor_node, sfvec3f>
                                    (&touch_sensor_node::hitNormal)));
        } else if (*interface == supportedInterfaces[2]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_field_ptr_ptr(new node_field_ptr_impl<touch_sensor_node, sfvec3f>
                                    (&touch_sensor_node::hitPoint)));
        } else if (*interface == supportedInterfaces[3]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(new node_field_ptr_impl<touch_sensor_node, sfvec2f>
                                    (&touch_sensor_node::hitTexCoord)));
        } else if (*interface == supportedInterfaces[4]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(new node_field_ptr_impl<touch_sensor_node, sfbool>
                                    (&touch_sensor_node::active)));
        } else if (*interface == supportedInterfaces[5]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(new node_field_ptr_impl<touch_sensor_node, sfbool>
                                    (&touch_sensor_node::over)));
        } else if (*interface == supportedInterfaces[6]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(new node_field_ptr_impl<touch_sensor_node, sftime>
                                    (&touch_sensor_node::touchTime)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class touch_sensor_node
 *
 * @brief TouchSensor node instances.
 */

/**
 * @var touch_sensor_node::touch_sensor_class
 *
 * @brief Class object for TouchSensor instances.
 */

/**
 * @var sfbool touch_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfvec3f touch_sensor_node::hitNormal
 *
 * @brief hitNormal_changed eventOut.
 */

/**
 * @var sfvec3f touch_sensor_node::hitPoint
 *
 * @brief hitPoint_changed eventOut.
 */

/**
 * @var sfvec2f touch_sensor_node::hitTexCoord
 *
 * @brief hitTexCoord_changed eventOut.
 */

/**
 * @var sfbool touch_sensor_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfbool touch_sensor_node::over
 *
 * @brief isOver eventOut.
 */

/**
 * @var sftime touch_sensor_node::touchTime
 *
 * @brief touchTime eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
touch_sensor_node::touch_sensor_node(const node_type & type,
                                     const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    enabled_(true),
    active(false),
    over(false),
    touchTime(0.0)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
touch_sensor_node::~touch_sensor_node() throw ()
{}

/**
 * @brief Cast to a touch_sensor_node.
 *
 * @return a pointer to the object.
 */
touch_sensor_node* touch_sensor_node::to_touch_sensor() const
{
    return (touch_sensor_node*) this;
}

/**
 * @todo Doesn't compute the xxx_changed eventOuts yet...
 */
void touch_sensor_node::activate(double timeStamp, bool isOver, bool isActive,
                                 double *)
{
    if (isOver && !isActive && this->active.value) {
        this->touchTime.value = timeStamp;
        this->emit_event("touchTime", this->touchTime, timeStamp);
    }

    if (isOver != this->over.value) {
        this->over.value = isOver;
        this->emit_event("isOver", this->over, timeStamp);
    }

    if (isActive != this->active.value) {
        this->active.value = isActive;
        this->emit_event("isActive", this->active, timeStamp);
    }
    // if (isOver && any routes from eventOuts)
    //   generate xxx_changed eventOuts...
}

/**
 * @brief Return whether the TouchSensor is enabled.
 *
 * @return @c true if the TouchSensor is enabled, @c false otherwise.
 */
bool touch_sensor_node::enabled() const
{
    return this->enabled_.value;
}

/**
 * @brief set_enabled eventIn handler.
 *
 * @param value    an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void touch_sensor_node::process_set_enabled(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast)
{
    this->enabled_ = dynamic_cast<const sfbool &>(value);
    this->emit_event("enabled_changed", this->enabled_, timestamp);
}


/**
 * @class transform_class
 *
 * @brief Class object for Transform nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
transform_class::transform_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
transform_class::~transform_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Transform nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by transform_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
transform_class::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
    const node_type_ptr type(new vrml97_node_type_impl<transform_node>(*this, id));
    vrml97_node_type_impl<transform_node> & transformNodeType =
        static_cast<vrml97_node_type_impl<transform_node> &>(*type);
    typedef vrml97_node_type_impl<transform_node>::node_field_ptr_ptr
        node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            transformNodeType.add_eventin(supportedInterfaces[0].field_type,
                                      supportedInterfaces[0].id,
                                      &transform_node::process_addChildren);
        } else if (*interface == supportedInterfaces[1]) {
            transformNodeType.add_eventin(supportedInterfaces[1].field_type,
                                      supportedInterfaces[1].id,
                                      &transform_node::process_removeChildren);
        } else if (*interface == supportedInterfaces[2]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &transform_node::process_set_center,
                node_field_ptr_ptr(new node_field_ptr_impl<transform_node, sfvec3f>
                                    (&transform_node::center)));
        } else if (*interface == supportedInterfaces[3]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &transform_node::process_set_children,
                node_field_ptr_ptr(new node_field_ptr_impl<transform_node, mfnode>
                                    (&transform_node::children_)));
        } else if (*interface == supportedInterfaces[4]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &transform_node::process_set_rotation,
                node_field_ptr_ptr(new node_field_ptr_impl<transform_node, sfrotation>
                                    (&transform_node::rotation)));
        } else if (*interface == supportedInterfaces[5]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &transform_node::process_set_scale,
                node_field_ptr_ptr(new node_field_ptr_impl<transform_node, sfvec3f>
                                    (&transform_node::scale)));
        } else if (*interface == supportedInterfaces[6]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &transform_node::process_set_scaleOrientation,
                node_field_ptr_ptr(new node_field_ptr_impl<transform_node, sfrotation>
                                    (&transform_node::scaleOrientation)));
        } else if (*interface == supportedInterfaces[7]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &transform_node::process_set_translation,
                node_field_ptr_ptr(new node_field_ptr_impl<transform_node, sfvec3f>
                                    (&transform_node::translation)));
        } else if (*interface == supportedInterfaces[8]) {
            transformNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_field_ptr_ptr(new node_field_ptr_impl<transform_node, sfvec3f>
                                    (&transform_node::bboxCenter)));
        } else if (*interface == supportedInterfaces[9]) {
            transformNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_field_ptr_ptr(new node_field_ptr_impl<transform_node, sfvec3f>
                                (&transform_node::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class transform_node
 *
 * @brief Transform node instances.
 */

/**
 * @var transform_node::transform_class
 *
 * @brief Class object for Transform instances.
 */

/**
 * @var sfvec3f transform_node::center
 *
 * @brief center exposedField.
 */

/**
 * @var sfrotation transform_node::rotation
 *
 * @brief rotation exposedField.
 */

/**
 * @var sfvec3f transform_node::scale
 *
 * @brief scale exposedField.
 */

/**
 * @var sfrotation transform_node::scaleOrientation
 *
 * @brief scaleOrientation exposedField.
 */

/**
 * @var sfvec3f transform_node::translation
 *
 * @brief translation exposedField.
 */

/**
 * @var viewer::object_t transform_node::xformObject
 *
 * @brief A handle to the renderer's representation of the Transform.
 */

/**
 * @var mat4f transform_node::transform_
 *
 * @brief Cached copy of this node's transformation.
 */

/**
 * @var transform_node::transform_dirty
 *
 * @brief Flag to indicate whether @a transform_ needs to be updated.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
transform_node::transform_node(const node_type & type,
                               const scope_ptr & scope):
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    group_node(type, scope),
    openvrml::transform_node(type, scope),
    center(vec3f(0.0, 0.0, 0.0)),
    rotation(openvrml::rotation(0.0, 0.0, 1.0, 0.0)),
    scale(vec3f(1.0, 1.0, 1.0)),
    scaleOrientation(openvrml::rotation(0.0, 0.0, 1.0, 0.0)),
    translation(vec3f(0.0, 0.0, 0.0)),
    transform_dirty(true),
    xformObject(0)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
transform_node::~transform_node() throw ()
{
    // delete xformObject...
}

/**
 * @brief Get the transformation associated with the node as a matrix.
 *
 * @return the transformation associated with the node.
 */
const mat4f & transform_node::transform() const throw ()
{
    this->update_transform();
    return this->transform_;
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
void transform_node::render(openvrml::viewer & viewer,
                            rendering_context context)
{
    if (context.cull_flag != bounding_volume::inside) {
        assert(dynamic_cast<const bounding_sphere *>
               (&this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
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

    mat4f LM = this->transform();
    mat4f new_LM = context.matrix();
    new_LM = LM * new_LM;
    context.matrix(new_LM);

    if (this->xformObject && modified()) {
        viewer.remove_object(this->xformObject);
        this->xformObject = 0;
    }

    if (this->xformObject) {
        viewer.insert_reference(this->xformObject);
    } else if (!this->children_.value.empty()) {
        this->xformObject = viewer.begin_object(this->id().c_str());

        // Apply transforms
        viewer.transform(this->transform());
        // Render children
        this->group_node::render_nocull(viewer, context);

        viewer.end_object();
    }
    this->node::modified(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & transform_node::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<transform_node *>(this)->recalc_bsphere();
    }
    return this->bsphere;
}

/**
 * @brief Recalculate the bounding volume.
 */
void transform_node::recalc_bsphere()
{
    this->bsphere = bounding_sphere();
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        const node_ptr & node = this->children_.value[i];
        if (node) {
            const openvrml::bounding_volume & ci_bv = node->bounding_volume();
            this->bsphere.extend(ci_bv);
        }
    }
    this->bsphere.transform(this->transform());
    this->bounding_volume_dirty(false);
}

/**
 * @brief Update @a transform.
 *
 * If @a transform_dirty is @c true, resynchronize the cached matrix
 * @a transform with the node fields; otherwise do nothing.
 *
 * @note P' = T × C × R × SR × S × -SR × -C × P
 */
void transform_node::update_transform() const throw ()
{
    if (this->transform_dirty) {
        this->transform_ = mat4f::transformation(this->translation.value,
                                                 this->rotation.value,
                                                 this->scale.value,
                                                 this->scaleOrientation.value,
                                                 this->center.value);
        this->transform_dirty = false;
    }
}

/**
 * @brief set_center eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void transform_node::process_set_center(const field_value & value,
                                        const double timestamp)
    throw (std::bad_cast)
{
    this->center = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transform_dirty = true;
    this->emit_event("center_changed", this->center, timestamp);
}

/**
 * @brief set_rotation eventIn handler.
 *
 * @param value     an sfrotation value.
 * @param timestamp     the current time.
 *
 * @exception std::bad_cast if @p value is not an sfrotation.
 */
void transform_node::process_set_rotation(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast)
{
    this->rotation = dynamic_cast<const sfrotation &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transform_dirty = true;
    this->emit_event("rotation_changed", this->rotation, timestamp);
}

/**
 * @brief set_scale eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void transform_node::process_set_scale(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast)
{
    this->scale = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transform_dirty = true;
    this->emit_event("scale_changed", this->scale, timestamp);
}

/**
 * @brief set_scaleOrientation eventIn handler.
 *
 * @param value     an sfrotation value.
 * @param timestamp     the current time.
 *
 * @exception std::bad_cast if @p value is not an sfrotation.
 */
void transform_node::process_set_scaleOrientation(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast)
{
    this->scaleOrientation = dynamic_cast<const sfrotation &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transform_dirty = true;
    this->emit_event("scaleOrientation_changed",
                     this->scaleOrientation,
                     timestamp);
}

/**
 * @brief set_translation eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void transform_node::process_set_translation(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    this->translation = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transform_dirty = true;
    this->emit_event("translation_changed", this->translation, timestamp);
}


/**
 * @class viewpoint_class
 *
 * @brief Class object for Viewpoint nodes.
 */

/**
 * @typedef viewpoint_class::bound_nodes_t
 *
 * @brief A stack of bound Viewpoint nodes.
 */

/**
 * @var viewpoint_node * viewpoint_class::first
 *
 * @brief The first Viewpoint node in the initial scene graph.
 */

/**
 * @var viewpoint_class::bound_nodes_t viewpoint_class::bound_nodes
 *
 * @brief The stack of bound Viewpoint nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
viewpoint_class::viewpoint_class(openvrml::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
viewpoint_class::~viewpoint_class() throw ()
{}

/**
 * @brief Set the first Viewpoint node in the world.
 *
 * The first Viewpoint node in the world is used as the initial viewpoint.
 * This method is used by viewpoint_node::do_initialize.
 *
 * @param viewpoint    a Viewpoint node.
 */
void viewpoint_class::set_first(viewpoint_node & viewpoint) throw ()
{
    this->first = &viewpoint;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by viewpoint_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool viewpoint_class::has_first() const throw ()
{
    return this->first;
}

/**
 * @brief Push a Viewpoint on the top of the bound node stack.
 *
 * @param viewpoint    the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void viewpoint_class::bind(viewpoint_node & viewpoint, const double timestamp)
    throw (std::bad_alloc)
{
    //
    // If the node is already the active node, do nothing.
    //
    if (!this->bound_nodes.empty() && &viewpoint == this->bound_nodes.back()) {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const bound_nodes_t::iterator pos =
        std::find(this->bound_nodes.begin(), this->bound_nodes.end(),
                  &viewpoint);
    if (pos != this->bound_nodes.end()) { this->bound_nodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->bound_nodes.empty()) {
        viewpoint_node & current =
                dynamic_cast<viewpoint_node &>(*this->bound_nodes.back());
        current.bound.value = false;
        current.emit_event("isBound", current.bound, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->bound_nodes.push_back(&viewpoint);
    viewpoint.bound.value = true;
    viewpoint.emit_event("isBound", viewpoint.bound, timestamp);

    this->browser.active_viewpoint(viewpoint);
}

/**
 * @brief Remove a Viewpoint from the bound node stack.
 *
 * @param viewpoint    the node to unbind.
 * @param timestamp     the current time.
 */
void viewpoint_class::unbind(viewpoint_node & viewpoint,
                             const double timestamp)
    throw ()
{
    const bound_nodes_t::iterator pos =
        std::find(this->bound_nodes.begin(), this->bound_nodes.end(),
                  &viewpoint);
    if (pos != this->bound_nodes.end()) {
        viewpoint.bound.value = false;
        viewpoint.emit_event("isBound", viewpoint.bound, timestamp);

        if (pos == this->bound_nodes.end() - 1
                && this->bound_nodes.size() > 1) {
            viewpoint_node & newActive =
                    dynamic_cast<viewpoint_node &>(**(this->bound_nodes.end() - 2));
            newActive.bound.value = true;
            newActive.emit_event("isBound", newActive.bound, timestamp);

            this->browser.active_viewpoint(viewpoint);
        } else {
            this->browser.reset_default_viewpoint();
        }
        this->bound_nodes.erase(pos);
    }
}

/**
 * @brief node_class-specific initialization.
 *
 * @param initial_viewpoint the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void viewpoint_class::initialize(openvrml::viewpoint_node * initial_viewpoint,
                                 const double timestamp)
    throw ()
{
    if (!initial_viewpoint) { initial_viewpoint = this->first; }
    if (initial_viewpoint) {
        initial_viewpoint->process_event("set_bind", sfbool(true), timestamp);
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Viewpoint nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by viewpoint_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
viewpoint_class::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef viewpoint_node node_t;
    typedef vrml97_node_type_impl<viewpoint_node> node_type_t;

    const node_type_ptr type(new node_type_t(*this, id));
    node_type_t & viewpointNodeType = static_cast<node_type_t &>(*type);
    typedef node_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            viewpointNodeType.add_eventin(supportedInterfaces[0].field_type,
                                          supportedInterfaces[0].id,
                                          &node_t::process_set_bind);
        } else if (*interface == supportedInterfaces[1]) {
            viewpointNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &viewpoint_node::process_set_fieldOfView,
                node_field_ptr_ptr(
                    new node_type_t::sffloat_ptr(&node_t::fieldOfView)));
        } else if (*interface == supportedInterfaces[2]) {
            viewpointNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &viewpoint_node::process_set_jump,
                node_field_ptr_ptr(
                    new node_type_t::sfbool_ptr(&node_t::jump)));
        } else if (*interface == supportedInterfaces[3]) {
            viewpointNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &viewpoint_node::process_set_orientation,
                node_field_ptr_ptr(
                    new node_type_t::sfrotation_ptr(&node_t::orientation_)));
        } else if (*interface == supportedInterfaces[4]) {
            viewpointNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &viewpoint_node::process_set_position,
                node_field_ptr_ptr(
                    new node_type_t::sfvec3f_ptr(&node_t::position_)));
        } else if (*interface == supportedInterfaces[5]) {
            viewpointNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(
                    new node_type_t::sfstring_ptr(&node_t::description_)));
        } else if (*interface == supportedInterfaces[6]) {
            viewpointNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_field_ptr_ptr(
                    new node_type_t::sftime_ptr(&node_t::bindTime)));
        } else if (*interface == supportedInterfaces[7]) {
            viewpointNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_field_ptr_ptr(
                    new node_type_t::sfbool_ptr(&node_t::bound)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class viewpoint_node
 *
 * @brief Viewpoint node instance.
 */

/**
 * @var viewpoint_node::viewpoint_class
 *
 * @brief Class object for Viewpoint instances.
 */

/**
 * @var sffloat viewpoint_node::fieldOfView
 *
 * @brief fieldOfView exposedField.
 */

/**
 * @var sfbool viewpoint_node::jump
 *
 * @brief jump exposedField.
 */

/**
 * @var sfrotation viewpoint_node::orientation_
 *
 * @brief orientation exposedField.
 */

/**
 * @var sfvec3f viewpoint_node::position_
 *
 * @brief position exposedField.
 */

/**
 * @var sfstring viewpoint_node::description_
 *
 * @brief description field.
 */

/**
 * @var sfbool viewpoint_node::bound
 *
 * @brief isBound eventOut.
 */

/**
 * @var sftime viewpoint_node::bindTime
 *
 * @brief bindTime eventOut.
 */

/**
 * @var mat4f viewpoint_node::parent_transform
 *
 * @brief The accumulated transformation applied by any parent nodes.
 */

/**
 * @var mat4f viewpoint_node::final_transformation
 *
 * @brief The product of @a parent_transform and any transformation applied
 *        by the Viewpoint node.
 */

/**
 * @var bool viewpoint_node::final_transformation_dirty
 *
 * @brief Flag that indicates if @a final_transformation needs to be updated.
 */

/**
 * @var mat4f viewpoint_node::user_view_transform_
 *
 * @brief The transformation applied to the user view.
 *
 * Generally the result of any user navigation.
 */

namespace {
    const float DEFAULT_FIELD_OF_VIEW = 0.785398f;
}

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
viewpoint_node::viewpoint_node(const node_type & type,
                               const scope_ptr & scope):
    node(type, scope),
    abstract_base(type, scope),
    child_node(type, scope),
    openvrml::viewpoint_node(type, scope),
    fieldOfView(DEFAULT_FIELD_OF_VIEW),
    jump(true),
    orientation_(rotation(0.0, 0.0, 1.0, 0.0)),
    position_(vec3f(0.0, 0.0, 10.0)),
    bound(false),
    bindTime(0),
    final_transformation_dirty(true)
{}

/**
 * @brief Destroy.
 */
viewpoint_node::~viewpoint_node() throw ()
{}

/**
 * @brief Get the transformation of the viewpoint_node in the global coordinate
 *      system.
 *
 * @return the transformation of the viewpoint_node in the global coordinate
 *      system.
 */
const mat4f & viewpoint_node::transformation() const throw ()
{
    this->update_final_transformation();
    return this->final_transformation;
}

/**
 * @brief Get the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @return the transformation of the user view relative to the viewpoint_node.
 */
const mat4f & viewpoint_node::user_view_transform() const throw ()
{
    return this->user_view_transform_;
}

/**
 * @brief Set the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @param transform the new transformation.
 */
void viewpoint_node::user_view_transform(const mat4f & transform) throw ()
{
    this->user_view_transform_ = transform;
}

/**
 * @brief description accessor.
 *
 * @return the description.
 */
const std::string & viewpoint_node::description() const throw ()
{
    return this->description_.value;
}

/**
 * @brief Get the field of view.
 *
 * @return the field of view in radians.
 */
float viewpoint_node::field_of_view() const throw ()
{
    return this->fieldOfView.value;
}

/**
 * @brief orientation accessor.
 *
 * @return the orientation.
 */
const sfrotation & viewpoint_node::orientation() const
{
    return this->orientation_;
}

/**
 * @brief position accessor.
 *
 * @return the position.
 */
const sfvec3f & viewpoint_node::position() const
{
    return this->position_;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void viewpoint_node::do_initialize(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.add_viewpoint(*this);
    assert(dynamic_cast<viewpoint_class *>(&this->type.node_class));
    viewpoint_class & nodeClass =
            static_cast<viewpoint_class &>(this->type.node_class);
    if (!nodeClass.has_first()) { nodeClass.set_first(*this); }
}

namespace {

    struct accumulate_transform_ : std::unary_function<const node *, void> {
        explicit accumulate_transform_(mat4f & transform) throw ():
            transform(&transform)
        {}

        void operator()(const openvrml::node * node) const throw ()
        {
            assert(node);
            const openvrml::transform_node * const transformNode =
                node->to_transform();
            if (transformNode) {
                *this->transform =
                    transformNode->transform() * *this->transform;
            }
        }

    private:
        mat4f * transform;
    };
}

/**
 * @brief Relocate.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void viewpoint_node::do_relocate() throw (std::bad_alloc)
{
    assert(this->scene());
    const node_path path = this->scene()->browser.find_node(*this);
    assert(!path.empty());
    this->parent_transform = mat4f();
    std::for_each(path.begin(), path.end(),
                  accumulate_transform_(this->parent_transform));
    this->final_transformation_dirty = true;
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void viewpoint_node::do_shutdown(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.remove_viewpoint(*this);
}

/**
 * @brief set_bind eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfbool value.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void viewpoint_node::process_set_bind(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    const sfbool & bind = dynamic_cast<const sfbool &>(value);
    assert(dynamic_cast<viewpoint_class *>(&this->type.node_class));
    viewpoint_class & nodeClass =
        static_cast<viewpoint_class &>(this->type.node_class);
    if (bind.value) {
        nodeClass.bind(*this, timestamp);
    } else {
        nodeClass.unbind(*this, timestamp);
    }
}

/**
 * @brief set_fieldOfView eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat value.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void viewpoint_node::process_set_fieldOfView(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    this->fieldOfView = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("fieldOfView_changed", this->fieldOfView, timestamp);
}

/**
 * @brief set_jump eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfbool value.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void viewpoint_node::process_set_jump(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast)
{
    this->jump = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("jump_changed", this->jump, timestamp);
}

/**
 * @brief set_orientation eventIn handler.
 *
 * @param value     an sfrotation value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfrotation value.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void viewpoint_node::process_set_orientation(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast)
{
    this->orientation_ = dynamic_cast<const sfrotation &>(value);
    this->node::modified(true);
    this->final_transformation_dirty = true;
    this->emit_event("orientation_changed", this->orientation_, timestamp);
}

/**
 * @brief set_position eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sfvec3f value.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void viewpoint_node::process_set_position(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast)
{
    this->position_ = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->final_transformation_dirty = true;
    this->emit_event("position_changed", this->position_, timestamp);
}

/**
 * @brief Update @a final_transformation.
 *
 * If @a final_transformation_dirty is @c true, update @a final_transformation
 * and sets @a final_transformation_dirty to @c false; otherwise, do nothing.
 */
void viewpoint_node::update_final_transformation() const throw ()
{
    if (this->final_transformation_dirty) {
        static const vec3f scale(1.0, 1.0, 1.0);
        static const rotation scaleOrientation;
        static const vec3f center;
        const mat4f & t = mat4f::transformation(this->position_.value,
                                                this->orientation_.value,
                                                scale,
                                                scaleOrientation,
                                                center);
        this->final_transformation = t * this->parent_transform;
        this->final_transformation_dirty = false;
    }
}


/**
 * @class visibility_sensor_class
 *
 * @brief Class object for VisibilitySensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
visibility_sensor_class::visibility_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
visibility_sensor_class::~visibility_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating VisibilitySensor
 *      nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by visibility_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
visibility_sensor_class::create_type(const std::string & id,
                                     const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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

    typedef visibility_sensor_node node_t;
    typedef vrml97_node_type_impl<visibility_sensor_node> node_type_t;

    const node_type_ptr type(new node_type_t(*this, id));
    node_type_t & visibilitySensorNodeType = static_cast<node_type_t &>(*type);
    typedef node_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            visibilitySensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &visibility_sensor_node::process_set_center,
                node_field_ptr_ptr(
                    new node_type_t::sfvec3f_ptr(&node_t::center)));
        } else if (*interface == supportedInterfaces[1]) {
            visibilitySensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &visibility_sensor_node::process_set_enabled,
                node_field_ptr_ptr(
                    new node_type_t::sfbool_ptr(&node_t::enabled)));
        } else if (*interface == supportedInterfaces[2]) {
            visibilitySensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &visibility_sensor_node::process_set_size,
                node_field_ptr_ptr(
                    new node_type_t::sfvec3f_ptr(&node_t::size)));
        } else if (*interface == supportedInterfaces[3]) {
            visibilitySensorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_field_ptr_ptr(
                    new node_type_t::sftime_ptr(&node_t::enterTime)));
        } else if (*interface == supportedInterfaces[4]) {
            visibilitySensorNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_field_ptr_ptr(
                    new node_type_t::sftime_ptr(&node_t::exitTime)));
        } else if (*interface == supportedInterfaces[5]) {
            visibilitySensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_field_ptr_ptr(
                    new node_type_t::sfbool_ptr(&node_t::active)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class visibility_sensor_node
 *
 * @brief VisibilitySensor node instances.
 */

/**
 * @var visibility_sensor_node::visibility_sensor_class
 *
 * @brief Class object for VisibilitySensor instances.
 */

/**
 * @var visibility_sensor_node::center
 *
 * @brief center exposedField.
 */

/**
 * @var visibility_sensor_node::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var visibility_sensor_node::size
 *
 * @brief size exposedField.
 */

/**
 * @var visibility_sensor_node::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var visibility_sensor_node::enterTime
 *
 * @brief enterTime eventOut.
 */

/**
 * @var visibility_sensor_node::exitTime
 *
 * @brief exitTime eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
visibility_sensor_node::visibility_sensor_node(const node_type & type,
                                               const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope),
    center(vec3f(0.0, 0.0, 0.0)),
    enabled(true),
    size(vec3f(0.0, 0.0, 0.0)),
    active(false),
    enterTime(0.0),
    exitTime(0.0)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
visibility_sensor_node::~visibility_sensor_node() throw ()
{}

/**
 * @brief Generate visibility events.
 *
 * This is in a render() method since the it needs to be computed
 * with respect to the accumulated transformations above it in the
 * scene graph. Move to update() when xforms are accumulated in Groups...
 */
void visibility_sensor_node::render(openvrml::viewer & viewer,
                                    const rendering_context context)
{
    if (this->enabled.value) {
        sftime timeNow(browser::current_time());
        vec3f xyz[2] = { this->center.value,
                         this->center.value + this->size.value };

        // hack: enclose box in a sphere...
        viewer.transform_points(2, &xyz[0]);
        float r  = (xyz[1] - xyz[0]).length();

        // Was the sphere visible last time through? How does this work
        // for USE'd nodes? I need a way for each USE to store whether
        // it was active.
        bool wasIn = this->active.value;

        // Is the sphere visible? ...
        bool inside = xyz[0].z() < 0.0; // && z > - scene->visLimit()
        if (inside) {
            using openvrml_::fequal;

            navigation_info_node * ni =
                this->type.node_class.browser.bindable_navigation_info_top();
            if (ni && !fequal<float>()(ni->visibility_limit(), 0.0f)
                    && xyz[0][2] < -(ni->visibility_limit())) {
                inside = false;
            }
        }

        // This bit assumes 90degree fieldOfView to get rid of trig calls...
        if (inside) {
            inside = fabs(xyz[0].x()) < -0.5 * xyz[0].z() + r
                    && fabs(xyz[0].y()) < -0.5 * xyz[0].z() + r;
        }

        // Just became visible
        if (inside && !wasIn) {
            this->active.value = true;
            this->emit_event("isActive", this->active, timeNow.value);

            this->enterTime = timeNow;
            this->emit_event("enterTime", this->enterTime, timeNow.value);
        }

        // Check if viewer has left the box
        else if (wasIn && !inside) {
            this->active.value = false;
            this->emit_event("isActive", this->active, timeNow.value);

            this->exitTime = timeNow;
            this->emit_event("exitTime", this->exitTime, timeNow.value);
        }
    } else {
        this->node::modified(false);
    }
}

/**
 * @brief set_center eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void visibility_sensor_node::process_set_center(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->center = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("center_changed", this->center, timestamp);
}

/**
 * @brief set_enabled eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void visibility_sensor_node::process_set_enabled(const field_value & value,
                                                 double timestamp)
    throw (std::bad_cast)
{
    this->enabled = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("enabled_changed", this->enabled, timestamp);
}

/**
 * @brief set_size eventIn handler.
 *
 * @param value     an sfvec3f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void visibility_sensor_node::process_set_size(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast)
{
    this->size = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("size_changed", this->size, timestamp);
}


/**
 * @class world_info_class
 *
 * @brief Class object for WorldInfo nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
world_info_class::world_info_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
world_info_class::~world_info_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating WorldInfo nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by world_info_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
world_info_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "info"),
        node_interface(node_interface::field_id,
                       field_value::sfstring_id,
                       "title")
    };

    typedef vrml97_node_type_impl<world_info_node> node_type_t;

    const node_type_ptr type(new node_type_t(*this, id));
    node_type_t & worldInfoNodeType = static_cast<node_type_t &>(*type);
    typedef node_type_t::node_field_ptr_ptr node_field_ptr_ptr;
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            worldInfoNodeType.add_field(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_field_ptr_ptr(
                    new node_type_t::mfstring_ptr(&world_info_node::info)));
        } else if (*interface == supportedInterfaces[1]) {
            worldInfoNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_field_ptr_ptr(
                    new node_type_t::sfstring_ptr(&world_info_node::title)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class world_info_node
 *
 * @brief WorldInfo node instances.
 */

/**
 * @var world_info_node::world_info_class
 *
 * @brief Class object for WorldInfo instances.
 */

/**
 * @var world_info_node::info
 *
 * @brief string field.
 */

/**
 * @var world_info_node::title
 *
 * @brief title field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
world_info_node::world_info_node(const node_type & type,
                                 const scope_ptr & scope):
    node(type, scope),
    abstract_child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
world_info_node::~world_info_node() throw ()
{}

} // namespace vrml97_node

} // namespace openvrml
