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
# include "Audio.h"
# include "sound.h"
# include "private.h"

/**
 * @internal
 */
namespace std {

    /**
     * @internal
     */
    template <>
    struct char_traits<unsigned char> {

        typedef unsigned char char_type;
        typedef int int_type;
        typedef streampos pos_type;
        typedef streamoff off_type;
        typedef mbstate_t state_type;

        static void assign(char_type & c1, const char_type & c2)
        {
            c1 = c2;
        }

        static bool eq(const char_type & c1, const char_type & c2)
        {
            return c1 == c2;
        }

        static bool lt(const char_type & c1, const char_type & c2)
        {
            return c1 < c2;
        }

        static int compare(const char_type * s1, const char_type * s2, size_t n)
        {
            for (size_t i = 0; i < n; ++i) {
                if (!eq(s1[i], s2[i])) { return lt(s1[i], s2[i]) ? -1 : 1; }
            }
            return 0;
        }

        static size_t length(const char_type * s)
        {
            const char_type * p = s;
            while (*p) { ++p; }
            return (p - s);
        }

        static const char_type * find(const char_type * s, size_t n,
                                      const char_type & a)
        {
            for (const char_type * p = s; size_t(p - s) < n; ++p) {
                if (*p == a) { return p; }
            }
            return 0;
        }

        static char_type * move(char_type * s1, const char_type * s2, size_t n)
        {
            return reinterpret_cast<char_type *>
                        (memmove(s1, s2, n * sizeof(char_type)));
        }

        static char_type * copy(char_type * s1, const char_type * s2, size_t n)
        {
            return reinterpret_cast<char_type *>
                        (memcpy(s1, s2, n * sizeof(char_type)));
        }

        static char_type * assign(char_type * s, size_t n, char_type a)
        {
            for (char_type * p = s; p < s + n; ++p) {
                assign(*p, a);
            }
            return s;
        }

        static int_type eof()
        {
            return static_cast<int_type>(-1);
        }

        static int_type not_eof(const int_type & c)
        {
            return eq_int_type(c, eof()) ? int_type(0) : c;
        }

        static char_type to_char_type(const int_type & e)
        {
            return char_type(e);
        }

        static int_type to_int_type(const char_type & c)
        {
            return int_type(c);
        }

        static bool eq_int_type(const int_type & e1, const int_type & e2)
        {
            return e1 == e2;
        }
    };
}

namespace OpenVRML {

/**
 * @brief Implementations of the VRML97 nodes.
 *
 * It is a legacy of OpenVRML's initial architecture that these classes are
 * exposed as part of the public API. Once all the dependencies on members of
 * this namespace have been factored out of other parts of OpenVRML, all of
 * these classes will be moving into the library's implementation.
 */
namespace Vrml97Node {

namespace {

    /**
     * @internal
     */
    class Vrml97NodeType : public node_type {
    public:
        virtual ~Vrml97NodeType() throw () = 0;
        virtual void setFieldValue(OpenVRML::node & node, const std::string & id,
                                   const field_value &) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc) = 0;
        virtual const field_value & getFieldValue(const OpenVRML::node & node,
                                                 const std::string & id) const
            throw (unsupported_interface) = 0;
        virtual void
        dispatchEventIn(OpenVRML::node & node, const std::string & id,
                        const field_value &, double timestamp) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc) = 0;
        virtual const field_value &
        getEventOutValue(const OpenVRML::node & node,
                         const std::string & id) const
            throw (unsupported_interface) = 0;

    protected:
        Vrml97NodeType(node_class & nodeClass, const std::string & id);
    };

    Vrml97NodeType::Vrml97NodeType(node_class & nodeClass,
                                   const std::string & id):
        node_type(nodeClass, id)
    {}

    Vrml97NodeType::~Vrml97NodeType() throw ()
    {}


    template <typename NodeT>
    class NodeFieldPtr {
    public:
        virtual ~NodeFieldPtr() = 0;
        virtual field_value & dereference(NodeT & obj) = 0;
        virtual const field_value & dereference(const NodeT & obj) = 0;
    };

    template <typename NodeT>
    NodeFieldPtr<NodeT>::~NodeFieldPtr()
    {}


    template <typename NodeT, typename ConcreteFieldValue>
    class NodeFieldPtrImpl : public NodeFieldPtr<NodeT> {
        ConcreteFieldValue NodeT::* itsPtr;

    public:
        NodeFieldPtrImpl(ConcreteFieldValue NodeT::* ptr):
            itsPtr(ptr)
        {}

        virtual ~NodeFieldPtrImpl();

        virtual field_value & dereference(NodeT &);
        virtual const field_value & dereference(const NodeT &);
    };

    template <typename NodeT, typename ConcreteFieldValue>
    NodeFieldPtrImpl<NodeT, ConcreteFieldValue>::~NodeFieldPtrImpl()
    {}

    template <typename NodeT, typename ConcreteFieldValue>
    field_value &
    NodeFieldPtrImpl<NodeT, ConcreteFieldValue>::dereference(NodeT & obj)
    {
        return obj.*itsPtr;
    }

    template <typename NodeT, typename ConcreteFieldValue>
    const field_value &
    NodeFieldPtrImpl<NodeT, ConcreteFieldValue>::dereference(const NodeT & obj)
    {
        return obj.*itsPtr;
    }


    template <typename NodeT>
    class Vrml97NodeTypeImpl : public Vrml97NodeType {
    public:
        typedef OpenVRML_::SharedPtr<NodeFieldPtr<NodeT> > NodeFieldPtrPtr;
        typedef void (NodeT::* EventInHandlerPtr)(const field_value &, double);

    private:
        node_interface_set interfaces_;
        typedef std::map<std::string, EventInHandlerPtr> EventInHandlerMap;
        typedef std::map<std::string, NodeFieldPtrPtr> FieldValueMap;
        mutable FieldValueMap fieldValueMap;
        EventInHandlerMap eventInHandlerMap;
        FieldValueMap eventOutValueMap;

    public:
        Vrml97NodeTypeImpl(node_class & nodeClass, const std::string & id);
        virtual ~Vrml97NodeTypeImpl() throw ();

        void addEventIn(field_value::type_id, const std::string & id,
                        EventInHandlerPtr eventInHandlerPtr)
            throw (unsupported_interface, std::bad_alloc);
        void addEventOut(field_value::type_id, const std::string & id,
                         const NodeFieldPtrPtr & eventOutPtrPtr)
            throw (unsupported_interface, std::bad_alloc);
        void addExposedField(field_value::type_id, const std::string & id,
                             EventInHandlerPtr eventInHandlerPtr,
                             const NodeFieldPtrPtr & fieldPtrPtr)
            throw (unsupported_interface, std::bad_alloc);
        void addField(field_value::type_id, const std::string & id,
                      const NodeFieldPtrPtr & fieldPtrPtr)
            throw (unsupported_interface, std::bad_alloc);

        virtual void setFieldValue(OpenVRML::node & node, const std::string & id,
                                   const field_value &) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const field_value &
                getFieldValue(const OpenVRML::node & node,
                              const std::string & id) const
            throw (unsupported_interface);
        virtual void dispatchEventIn(OpenVRML::node & node,
                                     const std::string & id,
                                     const field_value &,
                                     double timestamp) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const field_value &
        getEventOutValue(const OpenVRML::node & node,
                         const std::string & id) const
            throw (unsupported_interface);

        virtual const node_interface_set & interfaces() const throw ();
        virtual const node_ptr create_node(const scope_ptr & scope) const
            throw (std::bad_alloc);

    private:
        void do_setFieldValue(NodeT & node, const std::string & id,
                              const field_value &) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        const field_value & do_getFieldValue(const NodeT & node,
                                            const std::string & id) const
            throw (unsupported_interface);
        void do_dispatchEventIn(NodeT & node, const std::string & id,
                                 const field_value &, double timestamp) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        const field_value & do_getEventOutValue(const NodeT & node,
                                                const std::string & id) const
            throw (unsupported_interface);
    };

    template <typename NodeT>
    Vrml97NodeTypeImpl<NodeT>::Vrml97NodeTypeImpl(node_class & nodeClass,
                                                  const std::string & id):
        Vrml97NodeType(nodeClass, id)
    {}

    template <typename NodeT>
    Vrml97NodeTypeImpl<NodeT>::~Vrml97NodeTypeImpl() throw ()
    {}

    template <typename NodeT>
    void Vrml97NodeTypeImpl<NodeT>::addEventIn(
            const field_value::type_id type,
            const std::string & id,
            const EventInHandlerPtr eventInHandlerPtr)
        throw (unsupported_interface, std::bad_alloc)
    {
        const node_interface interface(node_interface::eventin_id, type, id);
        this->interfaces_.add(interface);
        const typename EventInHandlerMap::value_type
                value(id, eventInHandlerPtr);
        const bool succeeded = this->eventInHandlerMap.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    void Vrml97NodeTypeImpl<NodeT>::addEventOut(
            const field_value::type_id type,
            const std::string & id,
            const NodeFieldPtrPtr & eventOutPtrPtr)
        throw (unsupported_interface, std::bad_alloc)
    {
        const node_interface interface(node_interface::eventout_id, type, id);
        this->interfaces_.add(interface);
        const typename FieldValueMap::value_type value(id, eventOutPtrPtr);
        const bool succeeded = this->eventOutValueMap.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    void Vrml97NodeTypeImpl<NodeT>::addExposedField(
            const field_value::type_id type,
            const std::string & id,
            const EventInHandlerPtr eventInHandlerPtr,
            const NodeFieldPtrPtr & fieldPtrPtr)
        throw (unsupported_interface, std::bad_alloc)
    {
        const node_interface interface(node_interface::exposedfield_id, type, id);
        this->interfaces_.add(interface);

        bool succeeded;
        {
            const typename EventInHandlerMap::value_type
                    value("set_" + id, eventInHandlerPtr);
            succeeded = this->eventInHandlerMap.insert(value).second;
            assert(succeeded);
        }
        {
            const typename FieldValueMap::value_type value(id, fieldPtrPtr);
            succeeded = this->fieldValueMap.insert(value).second;
            assert(succeeded);
        }
        {
            const typename FieldValueMap::value_type
                    value(id + "_changed", fieldPtrPtr);
            succeeded = this->eventOutValueMap.insert(value).second;
            assert(succeeded);
        }
    }

    template <typename NodeT>
    void Vrml97NodeTypeImpl<NodeT>::addField(
            const field_value::type_id type,
            const std::string & id,
            const NodeFieldPtrPtr & nodeFieldPtrPtr)
        throw (unsupported_interface, std::bad_alloc)
    {
        const node_interface interface(node_interface::field_id, type, id);
        this->interfaces_.add(interface);
        const typename FieldValueMap::value_type value(id, nodeFieldPtrPtr);
        const bool succeeded = this->fieldValueMap.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    void Vrml97NodeTypeImpl<NodeT>::setFieldValue(
            OpenVRML::node & node,
            const std::string & id,
            const field_value & newVal) const
        throw (unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        assert(dynamic_cast<NodeT *>(&node));
        this->do_setFieldValue(dynamic_cast<NodeT &>(node), id, newVal);
    }

    template <typename NodeT>
    const field_value &
    Vrml97NodeTypeImpl<NodeT>::getFieldValue(const OpenVRML::node & node,
                                             const std::string & id) const
        throw (unsupported_interface)
    {
        assert(dynamic_cast<const NodeT *>(&node));
        return this->do_getFieldValue(dynamic_cast<const NodeT &>(node), id);
    }

    template <typename NodeT>
    void
    Vrml97NodeTypeImpl<NodeT>::dispatchEventIn(OpenVRML::node & node,
                                               const std::string & id,
                                               const field_value & value,
                                               const double timestamp) const
        throw (unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        assert(dynamic_cast<NodeT *>(&node));
        this->do_dispatchEventIn(dynamic_cast<NodeT &>(node), id, value,
                                 timestamp);
    }

    template <typename NodeT>
    const field_value &
    Vrml97NodeTypeImpl<NodeT>::getEventOutValue(const OpenVRML::node & node,
                                                const std::string & id) const
        throw (unsupported_interface)
    {
        assert(dynamic_cast<const NodeT *>(&node));
        return this->do_getEventOutValue(dynamic_cast<const NodeT &>(node), id);
    }

    template <typename NodeT>
    const node_interface_set & Vrml97NodeTypeImpl<NodeT>::interfaces() const
        throw ()
    {
        return this->interfaces_;
    }

    template <typename NodeT>
    const node_ptr
    Vrml97NodeTypeImpl<NodeT>::create_node(const scope_ptr & scope) const
        throw (std::bad_alloc)
    {
        return node_ptr(new NodeT(*this, scope));
    }

    template <typename NodeT>
    void Vrml97NodeTypeImpl<NodeT>::do_setFieldValue(
            NodeT & node,
            const std::string & id,
            const field_value & newVal) const
        throw (unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        typename FieldValueMap::iterator itr = this->fieldValueMap.find(id);
        if (itr == this->fieldValueMap.end()) {
            throw unsupported_interface(node.OpenVRML::node::type,
                                        node_interface::field_id,
                                        id);
        }
        itr->second->dereference(node).assign(newVal);
    }

    template <typename NodeT>
    const field_value &
    Vrml97NodeTypeImpl<NodeT>::do_getFieldValue(const NodeT & node,
                                                const std::string & id) const
        throw (unsupported_interface)
    {
        const typename FieldValueMap::const_iterator itr =
                this->fieldValueMap.find(id);
        if (itr == this->fieldValueMap.end()) {
            throw unsupported_interface(node.OpenVRML::node::type,
                                        node_interface::field_id,
                                        id);
        }
        return itr->second->dereference(node);
    }

    template <typename NodeT>
    void Vrml97NodeTypeImpl<NodeT>::do_dispatchEventIn(
            NodeT & node,
            const std::string & id,
            const field_value & value,
            const double timestamp) const
        throw (unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        typename EventInHandlerMap::const_iterator
                itr(this->eventInHandlerMap.find(id));
        if (itr == this->eventInHandlerMap.end()) {
            itr = this->eventInHandlerMap.find("set_" + id);
        }
        if (itr == this->eventInHandlerMap.end()) {
            throw unsupported_interface(node.OpenVRML::node::type,
                                        node_interface::eventin_id,
                                        id);
        }
        (node.*(itr->second))(value, timestamp);
    }

    template <typename NodeT>
    const field_value &
    Vrml97NodeTypeImpl<NodeT>::do_getEventOutValue(
            const NodeT & node,
            const std::string & id) const
        throw (unsupported_interface)
    {
        typename FieldValueMap::const_iterator
                itr(this->eventOutValueMap.find(id));
        if (itr == this->eventOutValueMap.end()) {
            itr = this->eventOutValueMap.find(id + "_changed");
        }
        if (itr == this->eventOutValueMap.end()) {
            throw unsupported_interface(node.OpenVRML::node::type,
                                        node_interface::eventout_id,
                                        id);
        }
        return itr->second->dereference(node);
    }
}

/**
 * @class AbstractBase
 *
 * @brief Abstract base class for VRML97 node implementations.
 *
 * AbstractBase encapsulates the mechanisms for field access and mutation,
 * event dispatch, and eventOut access.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
AbstractBase::AbstractBase(const node_type & type, const scope_ptr & scope):
    node(type, scope)
{}

/**
 * @brief Destructor.
 */
AbstractBase::~AbstractBase() throw ()
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
void AbstractBase::do_field(const std::string & id, const field_value & value)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    assert(dynamic_cast<const Vrml97NodeType *>(&this->type));
    static_cast<const Vrml97NodeType &>(this->type)
        .setFieldValue(*this, id, value);
}

/**
 * @brief Get a field value for a node.
 *
 * @param id    a field name.
 *
 * @exception unsupported_interface  if the node has no field @p id.
 */
const field_value & AbstractBase::do_field(const std::string & id) const
    throw (unsupported_interface)
{
    assert(dynamic_cast<const Vrml97NodeType *>(&this->type));
    return static_cast<const Vrml97NodeType &>(this->type)
        .getFieldValue(*this, id);
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
void AbstractBase::do_process_event(const std::string & id,
                                    const field_value & value,
                                    const double timestamp)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    assert(dynamic_cast<const Vrml97NodeType *>(&this->type));
    static_cast<const Vrml97NodeType &>(this->type)
        .dispatchEventIn(*this, id, value, timestamp);
}

/**
 * @brief Get an eventOut value for a node.
 *
 * @param id    an eventOut name.
 *
 * @exception unsupported_interface  if the node has no eventOut @p id.
 */
const field_value & AbstractBase::do_eventout(const std::string & id) const
    throw (unsupported_interface)
{
    assert(dynamic_cast<const Vrml97NodeType *>(&this->type));
    return static_cast<const Vrml97NodeType &>(this->type)
        .getEventOutValue(*this, id);
}


/**
 * @class AbstractChild
 *
 * @brief Base class for all child nodes.
 *
 * VRML97 defines "child" nodes as those that may occur at the root of the
 * scene, in the "children" field of a Group, Transform, Anchor, or Billboard
 * node, in the "level" field of a LOD node, or in the "choice" field of a
 * Switch node.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type for the node.
 * @param scope     the scope the new node should belong to.
 */
AbstractChild::AbstractChild(const node_type & type,
                             const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    child_node(type, scope)
{}

/**
 * @brief Destructor.
 */
AbstractChild::~AbstractChild() throw ()
{}

/**
 * @class AbstractGeometry
 *
 * @brief Base class for all geometry nodes.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type for the node.
 * @param scope     the scope the new node should belong to.
 */
AbstractGeometry::AbstractGeometry(const node_type & type,
                                   const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    geometry_node(type, scope),
    d_viewerObject(0)
{}

/**
 * @brief Destructor.
 */
AbstractGeometry::~AbstractGeometry() throw ()
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
void AbstractGeometry::render(OpenVRML::viewer & viewer, rendering_context context)
{
    if (this->d_viewerObject && this->modified()) {
        viewer.remove_object(this->d_viewerObject);
        this->d_viewerObject = 0;
    }

    if (this->d_viewerObject) {
        viewer.insert_reference(this->d_viewerObject);
    } else {
        this->d_viewerObject = this->insert_geometry(viewer, context);
        this->modified(false);
    }
}


/**
 * @class AbstractIndexedSet
 *
 * @brief Abstract base class for IndexedFaceSet and IndexedLineSet.
 */

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
AbstractIndexedSet::AbstractIndexedSet(const node_type & type,
                                       const scope_ptr & scope):
    node(type, scope),
    AbstractGeometry(type, scope),
    colorPerVertex(true)
{}

/**
 * @brief Destructor.
 */
AbstractIndexedSet::~AbstractIndexedSet() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool AbstractIndexedSet::modified() const
{
    return (this->modified_
            || (this->color_.value && this->color_.value->modified())
            || (this->coord.value && this->coord.value->modified()));
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void AbstractIndexedSet::update_modified(node_path & path, int flags)
{
    if (this->modified()) { mark_path_modified(path, true); }
    path.push_front(this);
    if (this->color_.value) { this->color_.value->update_modified(path); }
    if (this->coord.value) { this->coord.value->update_modified(path); }
    path.pop_front();
}

const color_node * AbstractIndexedSet::color() const throw ()
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
void AbstractIndexedSet::processSet_color(const field_value & value,
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
void AbstractIndexedSet::processSet_colorIndex(const field_value & value,
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
void AbstractIndexedSet::processSet_coord(const field_value & value,
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
void AbstractIndexedSet::processSet_coordIndex(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->coordIndex = dynamic_cast<const mfint32 &>(value);
    this->node::modified(true);
}


/**
 * @class AbstractLight
 *
 * @brief Base class for all light nodes.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type for the node.
 * @param scope     the scope to which the node belongs.
 */
AbstractLight::AbstractLight(const node_type & type, const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    ambientIntensity(0.0),
    color(OpenVRML::color(1.0, 1.0, 1.0)),
    intensity(1.0),
    on(true)
{}

/**
 * @brief Destructor.
 */
AbstractLight::~AbstractLight() throw ()
{}

/**
 * @brief Downcast to a light node.
 *
 * @return a pointer to this object.
 */
AbstractLight * AbstractLight::to_light() const
{
    return const_cast<AbstractLight *>(this);
}

/**
 * @brief Render this node as scoped.
 *
 * The default version of this method does nothing.
 *
 * @param viewer a renderer.
 */
void AbstractLight::renderScoped(OpenVRML::viewer & viewer)
{}

/**
 * @brief set_ambientIntensity eventIn handler.
 *
 * @param value     an SFFloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void AbstractLight::processSet_ambientIntensity(const field_value & value,
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
void AbstractLight::processSet_color(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->color = dynamic_cast<const sfcolor &>(value);
    this->node::modified(true);
    this->emit_event("color_changed", this->color, timestamp);
}

/**
 * @brief set_intensity eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void AbstractLight::processSet_intensity(const field_value & value,
                                         const double timestamp)
    throw (std::bad_cast)
{
    this->intensity = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("intensity_changed", this->intensity, timestamp);
}

/**
 * @brief set_on eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void AbstractLight::processSet_on(const field_value & value,
                                  const double timestamp)
    throw (std::bad_cast)
{
    this->on = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("on_changed", this->on, timestamp);
}


/**
 * @class AbstractTexture
 *
 * @brief Abstract base class for VRML97 texture nodes.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type for the node instance.
 * @param scope     the scope to which the node belongs.
 */
AbstractTexture::AbstractTexture(const node_type & type,
                                 const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    texture_node(type, scope),
    repeatS(true),
    repeatT(true)
{}

/**
 * @brief Destructor.
 */
AbstractTexture::~AbstractTexture() throw ()
{}

/**
 * @brief Get the repeatS flag.
 *
 * @return @c true if the texture should repeat in the <var>S</var> direction,
 *      @c false otherwise.
 */
bool AbstractTexture::repeat_s() const throw ()
{
    return this->repeatS.value;
}

/**
 * @brief Get the repeatT flag.
 *
 * @return @c true if the texture should repeat in the <var>T</var> direction,
 *      @c false otherwise.
 */
bool AbstractTexture::repeat_t() const throw ()
{
    return this->repeatT.value;
}


/**
 * @class AnchorClass.
 *
 * @brief Class object for Anchor nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this AnchorClass.
 */
AnchorClass::AnchorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
AnchorClass::~AnchorClass() throw ()
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
 *                              supported by AnchorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
AnchorClass::create_type(const std::string & id,
                        const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "addChildren"),
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "removeChildren"),
        node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "children"),
        node_interface(node_interface::exposedfield_id, field_value::sfstring_id, "description"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "parameter"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "url"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxCenter"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxSize")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Anchor>(*this, id));
    Vrml97NodeTypeImpl<Anchor> & anchorNodeType =
            static_cast<Vrml97NodeTypeImpl<Anchor> &>(*type);
    typedef Vrml97NodeTypeImpl<Anchor>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            anchorNodeType.addEventIn(supportedInterfaces[0].field_type,
                                      supportedInterfaces[0].id,
                                      &Anchor::processAddChildren);
        } else if (*itr == supportedInterfaces[1]) {
            anchorNodeType.addEventIn(supportedInterfaces[1].field_type,
                                      supportedInterfaces[1].id,
                                      &Anchor::processRemoveChildren);
        } else if (*itr == supportedInterfaces[2]) {
            anchorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Anchor::processSet_children,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Anchor, mfnode>
                                    (&Anchor::children_)));
        } else if (*itr == supportedInterfaces[3]) {
            anchorNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Anchor::processSet_description,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Anchor, sfstring>
                                    (&Anchor::description)));
        } else if (*itr == supportedInterfaces[4]) {
            anchorNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &Anchor::processSet_parameter,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Anchor, mfstring>
                                    (&Anchor::parameter)));
        } else if (*itr == supportedInterfaces[5]) {
            anchorNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &Anchor::processSet_url,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Anchor, mfstring>
                                    (&Anchor::url)));
        } else if (*itr == supportedInterfaces[6]) {
            anchorNodeType.addField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Anchor, sfvec3f>
                                    (&Anchor::bboxCenter)));
        } else if (*itr == supportedInterfaces[7]) {
            anchorNodeType.addField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Anchor, sfvec3f>
                                (&Anchor::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Anchor
 *
 * @brief Represents Anchor node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
Anchor::Anchor(const node_type & type,
               const scope_ptr & scope):
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    Group(type, scope)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destructor.
 */
Anchor::~Anchor() throw ()
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
void Anchor::processSet_description(const field_value & value,
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
void Anchor::processSet_parameter(const field_value & value,
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
void Anchor::processSet_url(const field_value & value,
                            const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->url = dynamic_cast<const mfstring &>(value);
    this->emit_event("url_changed", this->url, timestamp);
}

/**
 * @brief Cast to an Anchor.
 *
 * @return a pointer to this node.
 */
Anchor * Anchor::to_anchor() const
{
    return const_cast<Anchor *>(this);
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void Anchor::render(OpenVRML::viewer & viewer, const rendering_context context)
{
    viewer.set_sensitive(this);

    // Render children
    this->Group::render(viewer, context);

    viewer.set_sensitive(0);
}

/**
 * @brief Handle a click by loading the url.
 */
void Anchor::activate()
{
    assert(this->scene());
    this->scene()->load_url(this->url.value, this->parameter.value);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & Anchor::bounding_volume() const
{
    return Group::bounding_volume();
}

/**
 * @class AppearanceClass
 *
 * @brief Class object for Appearance nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
AppearanceClass::AppearanceClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
AppearanceClass::~AppearanceClass() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Appearance nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by AppearanceClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
AppearanceClass::create_type(const std::string & id,
                            const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "material"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "texture"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "textureTransform")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Appearance>(*this, id));
    Vrml97NodeTypeImpl<Appearance> & appearanceNodeType =
            static_cast<Vrml97NodeTypeImpl<Appearance> &>(*type);
    typedef Vrml97NodeTypeImpl<Appearance>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            appearanceNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &Appearance::processSet_material,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Appearance, sfnode>
                                    (&Appearance::material_)));
        } else if (*itr == supportedInterfaces[1]) {
            appearanceNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Appearance::processSet_texture,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Appearance, sfnode>
                                    (&Appearance::texture_)));
        } else if (*itr == supportedInterfaces[2]) {
            appearanceNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Appearance::processSet_textureTransform,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Appearance, sfnode>
                                    (&Appearance::textureTransform)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Appearance
 *
 * @brief Appearance node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Appearance::Appearance(const node_type & type,
                       const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    appearance_node(type, scope)
{}

/**
 * @brief Destructor.
 */
Appearance::~Appearance() throw ()
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
void Appearance::processSet_material(const field_value & value,
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
void Appearance::processSet_texture(const field_value & value,
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
void Appearance::processSet_textureTransform(const field_value & value,
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
bool Appearance::modified() const
{
    return (this->modified_
          || (this->material_.value && this->material_.value->modified())
          || (this->texture_.value && this->texture_.value->modified())
          || (this->textureTransform.value
                && this->textureTransform.value->modified()));
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void Appearance::update_modified(node_path & path, int flags)
{
    if (this->modified()) { mark_path_modified(path, true); }
    path.push_front(this);
    if (this->material_.value) { this->material_.value->update_modified(path); }
    if (this->texture_.value) { this->texture_.value->update_modified(path); }
    if (this->textureTransform.value) {
        this->textureTransform.value->update_modified(path);
    }
    path.pop_front();
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void Appearance::render(OpenVRML::viewer & viewer, const rendering_context context)
{
    material_node * const material = this->material_.value
                                  ? this->material_.value->to_material()
                                  : 0;
    texture_node * const texture = this->texture_.value
                                ? this->texture_.value->to_texture()
                                : 0;

    if (material) {
        float trans = material->transparency();
        color diffuse = material->diffuse_color();
        size_t nTexComponents = texture ? texture->nComponents() : 0;
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
const node_ptr & Appearance::material() const throw ()
{
    return this->material_.value;
}

/**
 * @brief Get the texture node.
 *
 * @return an sfnode object containing the texture node associated with
 *         this Appearance.
 */
const node_ptr & Appearance::texture() const throw ()
{
    return this->texture_.value;
}

/**
 * @brief Get the texture transform node.
 *
 * @return an sfnode object containing the TextureTransform node
 *         associated with this Appearance.
 */
const node_ptr & Appearance::texture_transform() const throw ()
{
    return this->textureTransform.value;
}


/**
 * @class AudioClipClass
 *
 * @brief Class object for AudioClip nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
AudioClipClass::AudioClipClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
AudioClipClass::~AudioClipClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating AudioClip nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by AudioClipClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
AudioClipClass::create_type(const std::string & id,
                           const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfstring_id, "description"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "loop"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "pitch"),
        node_interface(node_interface::exposedfield_id, field_value::sftime_id, "startTime"),
        node_interface(node_interface::exposedfield_id, field_value::sftime_id, "stopTime"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "url"),
        node_interface(node_interface::eventout_id, field_value::sftime_id, "duration_changed"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<AudioClip>(*this, id));
    Vrml97NodeTypeImpl<AudioClip> & audioClipNodeType =
            static_cast<Vrml97NodeTypeImpl<AudioClip> &>(*type);
    typedef Vrml97NodeTypeImpl<AudioClip>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            audioClipNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &AudioClip::processSet_description,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<AudioClip, sfstring>
                                    (&AudioClip::description)));
        } else if (*itr == supportedInterfaces[1]) {
            audioClipNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &AudioClip::processSet_loop,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<AudioClip, sfbool>
                                    (&AudioClip::loop)));
        } else if (*itr == supportedInterfaces[2]) {
            audioClipNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &AudioClip::processSet_pitch,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<AudioClip, sffloat>
                                    (&AudioClip::pitch)));
        } else if (*itr == supportedInterfaces[3]) {
            audioClipNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &AudioClip::processSet_startTime,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<AudioClip, sftime>
                                    (&AudioClip::startTime)));
        } else if (*itr == supportedInterfaces[4]) {
            audioClipNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &AudioClip::processSet_stopTime,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<AudioClip, sftime>
                                    (&AudioClip::stopTime)));
        } else if (*itr == supportedInterfaces[5]) {
            audioClipNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &AudioClip::processSet_url,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<AudioClip, mfstring>
                                    (&AudioClip::url)));
        } else if (*itr == supportedInterfaces[6]) {
            audioClipNodeType.addEventOut(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<AudioClip, sftime>
                                    (&AudioClip::duration)));
        } else if (*itr == supportedInterfaces[7]) {
            audioClipNodeType.addEventOut(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<AudioClip, sfbool>
                                    (&AudioClip::active)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class AudioClip
 *
 * @brief AudioClip node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
AudioClip::AudioClip(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    pitch(1.0),
    active(false),
    audio(0),
    url_modified(false),
    audio_index(0),
    audio_intensity(1.0),
    audio_fd(-1)
{}

/**
 * @brief Destructor.
 */
AudioClip::~AudioClip() throw ()
{
    delete this->audio;
}

/**
 * @brief Cast to an AudioClip node.
 *
 * @return a pointer to this node.
 */
AudioClip* AudioClip::to_audio_clip() const
{
    return (AudioClip*)this;
}

void AudioClip::update(const double currentTime)
{
    // If the URL has been modified, update the audio object
    if (this->url_modified) {
        doc relDoc(this->relativeUrl.value, static_cast<doc const *>(0));
        delete this->audio;
        std::string emptyUrl;
        this->audio = new Audio(emptyUrl);
        if (this->audio->tryURLs(this->url, &relDoc)) {
            this->duration.value = this->audio->duration();
            this->emit_event("duration_changed", this->duration, currentTime);
        } else {
#if HAVE_SOUND
            cerr << "Error: couldn't read AudioClip from URL "
                 << this->url << endl;
#endif
            delete this->audio;
            this->audio = 0;
        }

        this->url_modified = false;
    }

    // If there's no audio or START <= 0, we don't play anything
    if (this->audio == 0 || this->startTime.value <= 0)
        return;

    // Determine if this clip should be playing right now
    bool audible = false;

    // If START < STOP  and  START <= NOW < STOP
    if (this->stopTime.value > this->startTime.value)
        audible = (this->startTime.value <= currentTime &&
                   currentTime < this->stopTime.value);

    // If STOP < START  and  START <= NOW
    else
        audible = (currentTime >= this->startTime.value);

    // If the clip is not looping, it's not audible after
    // its duration has expired.
    if (!this->loop.value) {
        if (currentTime - this->startTime.value > this->audio->duration()) {
            audible = false;
        }
    }

    // If the clip is audible, play it.  Otherwise, stop it.
    if (audible) {
        // If the sound device is not already open, open it.
        if (this->audio_fd < 0) {
            this->audio_fd = openSound(this->audio->channels(),
                                       this->audio->bitsPerSample(),
                                       this->audio->samplesPerSec());

            // If we can't get a sound device, silently return
            if (this->audio_fd < 0) {
                return;
            }

            this->audio_index =
                this->audio->getByteIndex(currentTime
                                          - this->startTime.value);

            this->active.value = true;
            this->emit_event("isActive", this->active, currentTime);
        }

        // Send out a sound buffer
        this->audio_index = outputSoundChunk(this->audio->numBytes(),
                                             this->audio->samples(),
                                             this->audio->bitsPerSample(),
                                             this->audio_index,
                                             this->loop.value,
                                             this->audio_intensity,
                                             this->audio_fd);
    }

    // Otherwise, close the sound device
    else {
        if (this->audio_fd >= 0) {
            this->audio_fd = closeSound(this->audio_fd);
            this->active.value = false;
            this->emit_event("isActive", this->active, currentTime);
        }
    }
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void AudioClip::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_audio_clip(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void AudioClip::do_shutdown(const double timestamp) throw ()
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
void AudioClip::processSet_description(const field_value & value,
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
void AudioClip::processSet_loop(const field_value & value, double timestamp)
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
void AudioClip::processSet_pitch(const field_value & value, double timestamp)
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
void AudioClip::processSet_startTime(const field_value & value,
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
void AudioClip::processSet_stopTime(const field_value & value,
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
void AudioClip::processSet_url(const field_value & value,
                               const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->url = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("url_changed", this->url, timestamp);
}


/**
 * @class BackgroundClass
 *
 * @brief Class object for Background nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
BackgroundClass::BackgroundClass(OpenVRML::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destructor.
 */
BackgroundClass::~BackgroundClass() throw ()
{}

/**
 * @brief Set the first Background node in the world.
 *
 * The first Background node in the world is used as the initial background.
 * This method is used by Background::do_initialize.
 *
 * @param background    a Background node.
 */
void BackgroundClass::setFirst(Background & background) throw ()
{
    this->first = &background;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by Background::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool BackgroundClass::hasFirst() const throw ()
{
    return this->first;
}

/**
 * @brief Push a Background on the top of the bound node stack.
 *
 * @param background    the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void BackgroundClass::bind(Background & background, const double timestamp)
    throw (std::bad_alloc)
{
    using std::find;

    //
    // If the node is already the active node, do nothing.
    //
    if (!this->boundNodes.empty() && &background == this->boundNodes.back()) {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const BoundNodes::iterator pos =
        find(this->boundNodes.begin(), this->boundNodes.end(), &background);
    if (pos != this->boundNodes.end()) { this->boundNodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->boundNodes.empty()) {
        Background & current =
                dynamic_cast<Background &>(*this->boundNodes.back());
        current.bound.value = false;
        current.emit_event("isBound", current.bound, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->boundNodes.push_back(&background);
    background.bound.value = true;
    background.emit_event("isBound", background.bound, timestamp);
}

/**
 * @brief Remove a Background from the bound node stack.
 *
 * @param background    the node to unbind.
 * @param timestamp     the current time.
 */
void BackgroundClass::unbind(Background & background, const double timestamp)
    throw ()
{
    using std::find;

    const BoundNodes::iterator pos =
        find(this->boundNodes.begin(), this->boundNodes.end(), &background);
    if (pos != this->boundNodes.end()) {
        background.bound.value = false;
        background.emit_event("isBound", background.bound, timestamp);

        if (pos == this->boundNodes.end() - 1
                && this->boundNodes.size() > 1) {
            Background & newActive =
                    dynamic_cast<Background &>(**(this->boundNodes.end() - 2));
            newActive.bound.value = true;
            newActive.emit_event("isBound", newActive.bound, timestamp);
        }
        this->boundNodes.erase(pos);
    }
}

/**
 * @brief NodeClass-specific initialization.
 *
 * @param initialViewpoint  the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void BackgroundClass::initialize(viewpoint_node * initialViewpoint,
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
    Image * getTexture(const mfstring & urls, doc2 & baseDoc,
                       Image * tex, int thisIndex, OpenVRML::viewer & viewer)
    {
        // Check whether the url has already been loaded
        size_t n = urls.value.size();
        if (n > 0) {
            for (int index=thisIndex-1; index >= 0; --index) {
                const char * currentTex = tex[index].url();
                const std::string relPath = baseDoc.url_path();
                int currentLen = currentTex ? strlen(currentTex) : 0;
                int relPathLen = relPath.length();
                if (relPathLen >= currentLen) { relPathLen = 0; }

                if (currentTex) {
                    for (size_t i = 0; i < n; ++i) {
                        if (urls.value[i] == currentTex
                                || urls.value[i]
                                    == (currentTex + relPathLen)) {
                            return &tex[index];
                        }
                    }
                }
            }

            // Have to load it
            if (!tex[thisIndex].tryURLs(urls, &baseDoc)) {
                std::cerr << "Error: couldn't read Background texture from URL "
                          << urls << std::endl;
            } else if ( tex[thisIndex].pixels() && tex[thisIndex].nc() ) {
                //
                // The texture needs to be scaled.
                //

                // Ensure the image dimensions are powers of two
                int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
                const int nSizes = sizeof(sizes) / sizeof(int);
                int w = tex[thisIndex].w();
                int h = tex[thisIndex].h();
                int i, j;
                for (i=0; i<nSizes; ++i) {
                    if (w < sizes[i]) { break; }
                }
                for (j=0; j<nSizes; ++j) {
                    if (h < sizes[j]) { break; }
                }

                if (i > 0 && j > 0) {
                    // Always scale images down in size and reuse the same pixel
                    // memory. This can cause some ugliness...
                    if (w != sizes[i-1] || h != sizes[j-1]) {
                        viewer.scale_texture(w, h, sizes[i - 1], sizes[j - 1],
                                             tex[thisIndex].nc(),
                                             tex[thisIndex].pixels());
                        tex[thisIndex].setSize(sizes[i - 1], sizes[j - 1]);
                    }
                }
            }
        }

        return &tex[thisIndex];
    }
}

/**
 * @brief NodeClass-specific rendering.
 *
 * Render the active Background node.
 *
 * @param viewer    a Viewer.
 */
void BackgroundClass::render(OpenVRML::viewer & viewer) throw ()
{
    if (!this->boundNodes.empty()) {
        assert(this->boundNodes.back());
        Background & background = *this->boundNodes.back();

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
                background.texPtr[0] = getTexture(background.backUrl,
                                                  baseDoc,
                                                  background.tex,
                                                  0,
                                                  viewer);
                background.texPtr[1] = getTexture(background.bottomUrl,
                                                  baseDoc,
                                                  background.tex,
                                                  1,
                                                  viewer);
                background.texPtr[2] = getTexture(background.frontUrl,
                                                  baseDoc,
                                                  background.tex,
                                                  2,
                                                  viewer);
                background.texPtr[3] = getTexture(background.leftUrl,
                                                  baseDoc,
                                                  background.tex,
                                                  3,
                                                  viewer);
                background.texPtr[4] = getTexture(background.rightUrl,
                                                  baseDoc,
                                                  background.tex,
                                                  4,
                                                  viewer);
                background.texPtr[5] = getTexture(background.topUrl,
                                                  baseDoc,
                                                  background.tex,
                                                  5,
                                                  viewer);
            }

            int i, whc[18];    // Width, height, and nComponents for 6 tex
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
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Background nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by BackgroundClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
BackgroundClass::create_type(const std::string & id,
                            const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sfbool_id, "set_bind"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "groundAngle"),
        node_interface(node_interface::exposedfield_id, field_value::mfcolor_id, "groundColor"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "backUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "bottomUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "frontUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "leftUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "rightUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "topUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "skyAngle"),
        node_interface(node_interface::exposedfield_id, field_value::mfcolor_id, "skyColor"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isBound")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Background>(*this, id));
    Vrml97NodeTypeImpl<Background> & backgroundNodeType =
            static_cast<Vrml97NodeTypeImpl<Background> &>(*type);
    typedef Vrml97NodeTypeImpl<Background>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            backgroundNodeType.addEventIn(supportedInterfaces[0].field_type,
                                          supportedInterfaces[0].id,
                                          &Background::processSet_bind);
        } else if (*itr == supportedInterfaces[1]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Background::processSet_groundAngle,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mffloat>
                                    (&Background::groundAngle)));
        } else if (*itr == supportedInterfaces[2]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Background::processSet_groundColor,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mfcolor>
                                    (&Background::groundColor)));
        } else if (*itr == supportedInterfaces[3]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Background::processSet_backUrl,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mfstring>
                                    (&Background::backUrl)));
        } else if (*itr == supportedInterfaces[4]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &Background::processSet_bottomUrl,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mfstring>
                                    (&Background::bottomUrl)));
        } else if (*itr == supportedInterfaces[5]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &Background::processSet_frontUrl,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mfstring>
                                    (&Background::frontUrl)));
        } else if (*itr == supportedInterfaces[6]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &Background::processSet_leftUrl,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mfstring>
                                    (&Background::leftUrl)));
        } else if (*itr == supportedInterfaces[7]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &Background::processSet_rightUrl,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mfstring>
                                    (&Background::rightUrl)));
        } else if (*itr == supportedInterfaces[8]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                &Background::processSet_topUrl,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mfstring>
                                    (&Background::topUrl)));
        } else if (*itr == supportedInterfaces[9]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                &Background::processSet_skyAngle,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mffloat>
                                    (&Background::skyAngle)));
        } else if (*itr == supportedInterfaces[10]) {
            backgroundNodeType.addExposedField(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                &Background::processSet_skyColor,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, mfcolor>
                                    (&Background::skyColor)));
        } else if (*itr == supportedInterfaces[11]) {
            backgroundNodeType.addEventOut(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Background, sfbool>
                                    (&Background::bound)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Background
 *
 * @brief Background node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Background::Background(const node_type & type,
                       const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    bound(false),
    viewerObject(0)
{
    std::fill(this->texPtr, this->texPtr + 6, static_cast<Image *>(0));
}

/**
 * @brief Destructor.
 */
Background::~Background() throw ()
{
    // remove d_viewerObject...
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void Background::do_initialize(const double timestamp) throw ()
{
    assert(dynamic_cast<BackgroundClass *>(&this->type._class));
    BackgroundClass & nodeClass =
            static_cast<BackgroundClass &>(this->type._class);
    if (!nodeClass.hasFirst()) { nodeClass.setFirst(*this); }
}

/**
 * @brief Shut down.
 *
 * Calls BackgroundClass::unbind to unbind the node if it is bound.
 *
 * @param timestamp the current time.
 */
void Background::do_shutdown(const double timestamp) throw ()
{
    BackgroundClass & nodeClass =
            static_cast<BackgroundClass &>(this->type._class);
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
void Background::processSet_bind(const field_value & value,
                                 const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    const sfbool & bind = dynamic_cast<const sfbool &>(value);
    assert(dynamic_cast<BackgroundClass *>(&this->type._class));
    BackgroundClass & nodeClass =
            static_cast<BackgroundClass &>(this->type._class);
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
void Background::processSet_groundAngle(const field_value & value,
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
void Background::processSet_groundColor(const field_value & value,
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
void Background::processSet_backUrl(const field_value & value,
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
void Background::processSet_bottomUrl(const field_value & value,
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
void Background::processSet_frontUrl(const field_value & value,
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
void Background::processSet_leftUrl(const field_value & value,
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
void Background::processSet_rightUrl(const field_value & value,
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
void Background::processSet_topUrl(const field_value & value,
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
void Background::processSet_skyAngle(const field_value & value,
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
void Background::processSet_skyColor(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->skyColor = dynamic_cast<const mfcolor &>(value);
    this->node::modified(true);
    this->emit_event("skyColor_changed", this->skyColor, timestamp);
}


/**
 * @class BillboardClass
 *
 * @brief Class object for Billboard nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
BillboardClass::BillboardClass(OpenVRML::browser & browser): node_class(browser)
{}

/**
 * @brief Destructor.
 */
BillboardClass::~BillboardClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Billboard nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by BillboardClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
BillboardClass::create_type(const std::string & id,
                           const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "addChildren"),
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "removeChildren"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "axisOfRotation"),
        node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "children"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxCenter"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxSize")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Billboard>(*this, id));
    Vrml97NodeTypeImpl<Billboard> & billboardNodeType =
            static_cast<Vrml97NodeTypeImpl<Billboard> &>(*type);
    typedef Vrml97NodeTypeImpl<Billboard>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            billboardNodeType.addEventIn(supportedInterfaces[0].field_type,
                                         supportedInterfaces[0].id,
                                         &Billboard::processAddChildren);
        } else if (*itr == supportedInterfaces[1]) {
            billboardNodeType.addEventIn(supportedInterfaces[1].field_type,
                                         supportedInterfaces[1].id,
                                         &Billboard::processRemoveChildren);
        } else if (*itr == supportedInterfaces[2]) {
            billboardNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Billboard::processSet_axisOfRotation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Billboard, sfvec3f>
                                    (&Billboard::axisOfRotation)));
        } else if (*itr == supportedInterfaces[3]) {
            billboardNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Billboard::processSet_children,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Billboard, mfnode>
                                    (&Billboard::children_)));
        } else if (*itr == supportedInterfaces[4]) {
            billboardNodeType.addField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Billboard, sfvec3f>
                                    (&Billboard::bboxCenter)));
        } else if (*itr == supportedInterfaces[5]) {
            billboardNodeType.addField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Billboard, sfvec3f>
                                    (&Billboard::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Billboard
 *
 * @brief Billboard node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Billboard::Billboard(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    Group(type, scope),
    axisOfRotation(vec3f(0.0, 1.0, 0.0)),
    xformObject(0)
{}

/**
 * @brief Destructor.
 */
Billboard::~Billboard() throw ()
{
    // delete xformObject...
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
void Billboard::render(OpenVRML::viewer & viewer, rendering_context context)
{
    mat4f LM;
    mat4f new_LM = context.matrix();
    billboard_to_matrix(this, new_LM, LM);
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
        this->Group::render(viewer, context);

        viewer.end_object();
    }

    this->node::modified(false);
}

/**
 * @brief Calculate bb transformation matrix and store it in @p M.
 *
 * Here we are dealing with mat4f format (Matrices are stored
 * in row-major order).
 *
 * @param t_arg a pointer to a Billboard node.
 * @param L_MV  input ModelView transformation matrix.
 * @retval M    a copy of the resulting transform.
 */
void Billboard::billboard_to_matrix(const Billboard* t_arg,
                                    const mat4f & L_MV,
                                    mat4f & M)
{
    mat4f MV = L_MV.inverse();

    // Viewer position in local coordinate system
    vec3f VP(MV[3][0], MV[3][1], MV[3][2]);
    vec3f NVP = VP.normalize();

    // Viewer-alignment
    if ((t_arg->axisOfRotation.value[0] == 0)
            && (t_arg->axisOfRotation.value[1] == 0)
            && (t_arg->axisOfRotation.value[2] == 0)) {
        //
        // Viewer's up vector
        //
        vec3f Y(MV[1][0], MV[1][1], MV[1][2]);
        vec3f NY = Y.normalize();

        // get x-vector from the cross product of Viewer's
        // up vector and billboard-to-viewer vector.
        vec3f X = NY * NVP;
        M[0][0] = X[0]; M[0][1] = X[1]; M[0][2] = X[2]; M[0][3] = 0.0;
        M[1][0] = NY[0]; M[1][1] = NY[1]; M[1][2] = NY[2]; M[1][3] = 0.0;
        M[2][0] = NVP[0]; M[2][1] = NVP[1]; M[2][2] = NVP[2]; M[2][3] = 0.0,
        M[3][0] = M[3][1] = M[3][2] = 0.0; M[3][3] = 1.0;
    } else { // use axis of rotation
        // axis of rotation will be the y-axis vector
        vec3f Y(t_arg->axisOfRotation.value);

        // Plane defined by the axisOfRotation and billboard-to-viewer vector
        vec3f X = (Y * VP).normalize();

        // Get Z axis vector from cross product of X and Y
        vec3f Z = X * Y;

        // Transform Z axis vector of current coordinate system to new
        // coordinate system.
        float nz[3];
        nz[0] = X[2]; nz[1] = Y[2]; nz[2] = Z[2];

        // calculate the angle by which the Z axis vector of current coordinate
        // system has to be rotated around the Y axis to new coordinate system.
        float angle = acos(nz[2]);
        if(nz[0] > 0) { angle = -angle; }
        M = mat4f::rotation(rotation(Y, angle));
    }
}

/**
 * @brief set_axisOfRotation eventIn handler.
 *
 * @param value     an sfvec3f.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec3f.
 */
void Billboard::processSet_axisOfRotation(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast)
{
    this->axisOfRotation = dynamic_cast<const sfvec3f &>(value);
    this->emit_event("axisOfRotation_changed", this->axisOfRotation, timestamp);
}


/**
 * @class BoxClass
 *
 * @brief Class object for Box nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
BoxClass::BoxClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
BoxClass::~BoxClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Box nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by BoxClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr BoxClass::create_type(const std::string & id,
                                       const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
            node_interface(node_interface::field_id, field_value::sfvec3f_id, "size");
    const node_type_ptr type(new Vrml97NodeTypeImpl<Box>(*this, id));
    Vrml97NodeTypeImpl<Box> & boxNodeType =
            static_cast<Vrml97NodeTypeImpl<Box> &>(*type);
    typedef Vrml97NodeTypeImpl<Box>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterface) {
            boxNodeType.addField(
                supportedInterface.field_type,
                supportedInterface.id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Box, sfvec3f>(&Box::size)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Box
 *
 * @brief Box node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Box::Box(const node_type & type,
         const scope_ptr & scope):
    node(type, scope),
    AbstractGeometry(type, scope),
    size(vec3f(2.0, 2.0, 2.0))
{
    this->bounding_volume_dirty(true); // lazy calc of bvolume
}

/**
 * @brief Destructor.
 */
Box::~Box() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @return display object identifier.
 */
viewer::object_t Box::insert_geometry(OpenVRML::viewer & viewer,
                                      const rendering_context context)
{
    return viewer.insert_box(this->size.value);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & Box::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const vec3f corner = this->size.value / 2.0f;
        const float r = corner.length();
        const_cast<Box *>(this)->bsphere.radius(r);
        const_cast<Box *>(this)->bounding_volume_dirty(false); // logical const
    }
    return this->bsphere;
}


/**
 * @class CollisionClass
 *
 * @brief Class object for Collision nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
CollisionClass::CollisionClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
CollisionClass::~CollisionClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Collision nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by CollisionClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        CollisionClass::create_type(const std::string & id,
                                   const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "addChildren"),
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "removeChildren"),
        node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "children"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "collide"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxCenter"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxSize"),
        node_interface(node_interface::field_id, field_value::sfnode_id, "proxy"),
        node_interface(node_interface::eventout_id, field_value::sftime_id, "collideTime")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Collision>(*this, id));
    Vrml97NodeTypeImpl<Collision> & collisionNodeType =
            static_cast<Vrml97NodeTypeImpl<Collision> &>(*type);
    typedef Vrml97NodeTypeImpl<Collision>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            collisionNodeType.addEventIn(supportedInterfaces[0].field_type,
                                         supportedInterfaces[0].id,
                                         &Collision::processAddChildren);
        } else if (*itr == supportedInterfaces[1]) {
            collisionNodeType.addEventIn(supportedInterfaces[1].field_type,
                                         supportedInterfaces[1].id,
                                         &Collision::processRemoveChildren);
        } else if (*itr == supportedInterfaces[2]) {
            collisionNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Collision::processSet_children,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Collision, mfnode>
                                    (&Collision::children_)));
        } else if (*itr == supportedInterfaces[3]) {
            collisionNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Collision::processSet_collide,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Collision, sfbool>
                                    (&Collision::collide)));
        } else if (*itr == supportedInterfaces[4]) {
            collisionNodeType.addField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Collision, sfvec3f>
                                    (&Collision::bboxCenter)));
        } else if (*itr == supportedInterfaces[5]) {
            collisionNodeType.addField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Collision, sfvec3f>
                                    (&Collision::bboxSize)));
        } else if (*itr == supportedInterfaces[6]) {
            collisionNodeType.addField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Collision, sfnode>
                                    (&Collision::proxy)));
        } else if (*itr == supportedInterfaces[7]) {
            collisionNodeType.addEventOut(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Collision, sftime>
                                    (&Collision::collideTime)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Collision
 *
 * @brief Collision node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Collision::Collision(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    Group(type, scope),
    collide(true)
{}

/**
 * @brief Destructor.
 */
Collision::~Collision() throw () {}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool Collision::modified() const {
  return ((this->proxy.value && this->proxy.value->modified())
          || this->Group::modified());
}

/**
 * @brief set_collide eventIn handler.
 *
 * @param value     an sfbool.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void Collision::processSet_collide(const field_value & value,
                                   const double timestamp)
    throw (std::bad_cast)
{
    this->collide = dynamic_cast<const sfbool &>(value);
    this->emit_event("collide_changed", this->collide, timestamp);
}


/**
 * @class ColorClass
 *
 * @brief Class object for Color nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
ColorClass::ColorClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
ColorClass::~ColorClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Color nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by ColorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr ColorClass::create_type(const std::string & id,
                                         const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterface =
            node_interface(node_interface::exposedfield_id, field_value::mfcolor_id, "color");
    const node_type_ptr type(new Vrml97NodeTypeImpl<Color>(*this, id));
    Vrml97NodeTypeImpl<Color> & colorNodeType =
            static_cast<Vrml97NodeTypeImpl<Color> &>(*type);
    typedef Vrml97NodeTypeImpl<Color>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterface) {
            colorNodeType.addExposedField(
                supportedInterface.field_type,
                supportedInterface.id,
                &Color::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Color, mfcolor>
                                    (&Color::color_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Color
 *
 * @brief Color node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
Color::Color(const node_type & type,
             const scope_ptr & scope):
        node(type, scope),
        AbstractBase(type, scope),
        color_node(type, scope) {}

/**
 * @brief Destructor.
 */
Color::~Color() throw () {}

/**
 * @brief Get the color array.
 *
 * @return the color array associated with the node.
 */
const std::vector<color> & Color::color() const throw ()
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
void Color::processSet_color(const field_value & value, const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->color_ = dynamic_cast<const mfcolor &>(value);
    this->node::modified(true);
    this->emit_event("color_changed", this->color_, timestamp);
}


/**
 * @class ColorInterpolatorClass
 *
 * @brief Class object for ColorInterpolator nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
ColorInterpolatorClass::ColorInterpolatorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
ColorInterpolatorClass::~ColorInterpolatorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating ColorInterpolator
 *      nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by ColorInterpolatorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
ColorInterpolatorClass::create_type(const std::string & id,
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
        type(new Vrml97NodeTypeImpl<ColorInterpolator>(*this, id));
    Vrml97NodeTypeImpl<ColorInterpolator> & colorInterpolatorNodeType =
            static_cast<Vrml97NodeTypeImpl<ColorInterpolator> &>(*type);
    typedef Vrml97NodeTypeImpl<ColorInterpolator>::NodeFieldPtrPtr
        NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            colorInterpolatorNodeType
                    .addEventIn(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &ColorInterpolator::processSet_fraction);
        } else if (*itr == supportedInterfaces[1]) {
            colorInterpolatorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &ColorInterpolator::processSet_key,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ColorInterpolator,
                                                     mffloat>
                                (&ColorInterpolator::key)));
        } else if (*itr == supportedInterfaces[2]) {
            colorInterpolatorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &ColorInterpolator::processSet_keyValue,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ColorInterpolator,
                                                     mfcolor>
                                (&ColorInterpolator::keyValue)));
        } else if (*itr == supportedInterfaces[3]) {
            colorInterpolatorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ColorInterpolator,
                                                     sfcolor>
                                (&ColorInterpolator::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class ColorInterpolator
 *
 * @brief ColorInterpolator node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
ColorInterpolator::ColorInterpolator(const node_type & type,
                                     const scope_ptr & scope):
        node(type, scope),
        AbstractChild(type, scope) {}

/**
 * @brief Destructor.
 */
ColorInterpolator::~ColorInterpolator() throw () {}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void ColorInterpolator::processSet_fraction(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    float f = dynamic_cast<const sffloat &>(value).value;

    int n = this->key.value.size() - 1;
    if (f < this->key.value[0]) {
        this->value.value = this->keyValue.value[0];
    } else if (f > this->key.value[n]) {
        this->value.value = this->keyValue.value[n];
    } else {
        // convert to HSV for the interpolation...
        for (int i = 0; i < n; ++i) {
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
void ColorInterpolator::processSet_key(const field_value & value,
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
void ColorInterpolator::processSet_keyValue(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mfcolor &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class ConeClass
 *
 * @brief Class object for Cone nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
ConeClass::ConeClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
ConeClass::~ConeClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Cone nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by ConeClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr ConeClass::create_type(const std::string & id,
                                        const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::field_id, field_value::sffloat_id, "bottomRadius"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "height"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "side"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "bottom")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Cone>(*this, id));
    Vrml97NodeTypeImpl<Cone> & coneNodeType =
            static_cast<Vrml97NodeTypeImpl<Cone> &>(*type);
    typedef Vrml97NodeTypeImpl<Cone>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            coneNodeType.addField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cone, sffloat>
                                    (&Cone::bottomRadius)));
        } else if (*itr == supportedInterfaces[1]) {
            coneNodeType.addField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cone, sffloat>
                                    (&Cone::height)));
        } else if (*itr == supportedInterfaces[2]) {
            coneNodeType.addField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cone, sfbool>
                                    (&Cone::side)));
        } else if (*itr == supportedInterfaces[3]) {
            coneNodeType.addField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cone, sfbool>
                                    (&Cone::bottom)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Cone
 *
 * @brief Cone node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Cone::Cone(const node_type & type,
           const scope_ptr & scope):
        node(type, scope),
        AbstractGeometry(type, scope),
        bottom(true),
        bottomRadius(1.0),
        height(2.0),
        side(true) {}

/**
 * @brief Destructor.
 */
Cone::~Cone() throw () {}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t Cone::insert_geometry(OpenVRML::viewer & viewer,
                                       const rendering_context context)
{
    return viewer.insert_cone(this->height.value,
                              this->bottomRadius.value,
                              this->bottom.value,
                              this->side.value);
}


/**
 * @class CoordinateClass
 *
 * @brief Class object for Material nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
CoordinateClass::CoordinateClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
CoordinateClass::~CoordinateClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Coordinate nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by CoordinateClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
CoordinateClass::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "point");
    const node_type_ptr type(new Vrml97NodeTypeImpl<Coordinate>(*this, id));
    Vrml97NodeTypeImpl<Coordinate> & coordinateNodeType =
            static_cast<Vrml97NodeTypeImpl<Coordinate> &>(*type);
    typedef Vrml97NodeTypeImpl<Coordinate>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterface) {
            coordinateNodeType.addExposedField(
                supportedInterface.field_type,
                supportedInterface.id,
                &Coordinate::processSet_point,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Coordinate, mfvec3f>
                                    (&Coordinate::point_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Coordinate
 *
 * @brief Coordinate node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
Coordinate::Coordinate(const node_type & type,
                       const scope_ptr & scope):
        node(type, scope),
        AbstractBase(type, scope),
        coordinate_node(type, scope) {}

/**
 * @brief Destructor.
 */
Coordinate::~Coordinate() throw () {}

/**
 * @brief set_point eventIn handler.
 *
 * @param value     an array of vectors representing points.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an mfvec3f.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void Coordinate::processSet_point(const field_value & value,
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
const std::vector<vec3f> & Coordinate::point() const throw ()
{
    return this->point_.value;
}


/**
 * @class CoordinateInterpolatorClass
 *
 * @brief Class object for CoordinateInterpolator nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
CoordinateInterpolatorClass::CoordinateInterpolatorClass(OpenVRML::browser & browser):
    node_class(browser) {}

/**
 * @brief Destructor.
 */
CoordinateInterpolatorClass::~CoordinateInterpolatorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *      CoordinateInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by CoordinateInterpolatorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
CoordinateInterpolatorClass::create_type(const std::string & id,
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
    const node_type_ptr
        type(new Vrml97NodeTypeImpl<CoordinateInterpolator>(*this, id));
    Vrml97NodeTypeImpl<CoordinateInterpolator> &
        coordinateInterpolatorNodeType =
        static_cast<Vrml97NodeTypeImpl<CoordinateInterpolator> &>(*type);
    typedef Vrml97NodeTypeImpl<CoordinateInterpolator>::NodeFieldPtrPtr
        NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            coordinateInterpolatorNodeType
                    .addEventIn(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &CoordinateInterpolator::processSet_fraction);
        } else if (*itr == supportedInterfaces[1]) {
            coordinateInterpolatorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &CoordinateInterpolator::processSet_key,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CoordinateInterpolator,
                                                     mffloat>
                                    (&CoordinateInterpolator::key)));
        } else if (*itr == supportedInterfaces[2]) {
            coordinateInterpolatorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &CoordinateInterpolator::processSet_keyValue,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CoordinateInterpolator,
                                                     mfvec3f>
                                    (&CoordinateInterpolator::keyValue)));
        } else if (*itr == supportedInterfaces[3]) {
            coordinateInterpolatorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CoordinateInterpolator,
                                                     mfvec3f>
                                    (&CoordinateInterpolator::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class CoordinateInterpolator
 *
 * @brief CoordinateInterpolator node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
CoordinateInterpolator::CoordinateInterpolator(const node_type & type,
                                               const scope_ptr & scope):
        node(type, scope),
        AbstractChild(type, scope) {}

/**
 * @brief Destructor.
 */
CoordinateInterpolator::~CoordinateInterpolator() throw () {}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void CoordinateInterpolator::processSet_fraction(const field_value & value,
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
void CoordinateInterpolator::processSet_key(const field_value & value,
                                            const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
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
void CoordinateInterpolator::processSet_keyValue(const field_value & value,
                                                 const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->keyValue = dynamic_cast<const mfvec3f &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class CylinderClass
 *
 * @brief Class object for Cylinder nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
CylinderClass::CylinderClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
CylinderClass::~CylinderClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Cylinder nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by CylinderClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr CylinderClass::create_type(const std::string & id,
                                            const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::field_id, field_value::sfbool_id, "bottom"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "height"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "radius"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "side"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "top")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Cylinder>(*this, id));
    Vrml97NodeTypeImpl<Cylinder> & cylinderNodeType =
            static_cast<Vrml97NodeTypeImpl<Cylinder> &>(*type);
    typedef Vrml97NodeTypeImpl<Cylinder>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            cylinderNodeType.addField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cylinder, sfbool>
                                    (&Cylinder::bottom)));
        } else if (*itr == supportedInterfaces[1]) {
            cylinderNodeType.addField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cylinder, sffloat>
                                    (&Cylinder::height)));
        } else if (*itr == supportedInterfaces[2]) {
            cylinderNodeType.addField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cylinder, sffloat>
                                    (&Cylinder::radius)));
        } else if (*itr == supportedInterfaces[3]) {
            cylinderNodeType.addField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cylinder, sfbool>
                                    (&Cylinder::side)));
        } else if (*itr == supportedInterfaces[4]) {
            cylinderNodeType.addField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Cylinder, sfbool>
                                    (&Cylinder::top)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Cylinder
 *
 * @brief Cylinder node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Cylinder::Cylinder(const node_type & type,
                   const scope_ptr & scope):
        node(type, scope),
        AbstractGeometry(type, scope),
        bottom(true),
        height(2.0),
        radius(1.0),
        side(true),
        top(true) {}

/**
 * @brief Destructor.
 */
Cylinder::~Cylinder() throw () {
    // need access to viewer to remove d_viewerObject...
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t Cylinder::insert_geometry(OpenVRML::viewer & viewer,
                                           const rendering_context context)
{
    return viewer.insert_cylinder(this->height.value,
                                  this->radius.value,
                                  this->bottom.value,
                                  this->side.value,
                                  this->top.value);
}


/**
 * @class CylinderSensorClass
 *
 * @brief Class object for CylinderSensor nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
CylinderSensorClass::CylinderSensorClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
CylinderSensorClass::~CylinderSensorClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating CylinderSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by CylinderSensorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        CylinderSensorClass::create_type(const std::string & id,
                                        const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "autoOffset"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "diskAngle"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "maxAngle"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "minAngle"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "offset"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive"),
        node_interface(node_interface::eventout_id, field_value::sfrotation_id, "rotation_changed"),
        node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "trackPoint_changed")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<CylinderSensor>(*this, id));
    Vrml97NodeTypeImpl<CylinderSensor> & cylinderSensorNodeType =
            static_cast<Vrml97NodeTypeImpl<CylinderSensor> &>(*type);
    typedef Vrml97NodeTypeImpl<CylinderSensor>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            cylinderSensorNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &CylinderSensor::processSet_autoOffset,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sfbool>
                                    (&CylinderSensor::autoOffset)));
        } else if (*itr == supportedInterfaces[1]) {
            cylinderSensorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &CylinderSensor::processSet_diskAngle,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sffloat>
                                    (&CylinderSensor::diskAngle)));
        } else if (*itr == supportedInterfaces[2]) {
            cylinderSensorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &CylinderSensor::processSet_enabled,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sfbool>
                                    (&CylinderSensor::enabled)));
        } else if (*itr == supportedInterfaces[3]) {
            cylinderSensorNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &CylinderSensor::processSet_maxAngle,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sffloat>
                                    (&CylinderSensor::maxAngle)));
        } else if (*itr == supportedInterfaces[4]) {
            cylinderSensorNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &CylinderSensor::processSet_minAngle,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sffloat>
                                    (&CylinderSensor::minAngle)));
        } else if (*itr == supportedInterfaces[5]) {
            cylinderSensorNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &CylinderSensor::processSet_offset,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sffloat>
                                    (&CylinderSensor::offset)));
        } else if (*itr == supportedInterfaces[6]) {
            cylinderSensorNodeType.addEventOut(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sfbool>
                                    (&CylinderSensor::active)));
        } else if (*itr == supportedInterfaces[7]) {
            cylinderSensorNodeType.addEventOut(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sfrotation>
                                    (&CylinderSensor::rotation)));
        } else if (*itr == supportedInterfaces[8]) {
            cylinderSensorNodeType.addEventOut(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<CylinderSensor, sfvec3f>
                                    (&CylinderSensor::trackPoint)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class CylinderSensor
 *
 * @brief CylinderSensor node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
CylinderSensor::CylinderSensor(const node_type & type,
                               const scope_ptr & scope):
        node(type, scope),
        AbstractChild(type, scope),
        autoOffset(true),
        diskAngle(0.262),
        enabled(true),
        maxAngle(-1.0),
        minAngle(0.0),
        offset(0.0),
        active(false) {
    this->node::modified(true);
}

/**
 * @brief Destructor.
 */
CylinderSensor::~CylinderSensor() throw () {}

/**
 * @brief Cast to a CylinderSensor.
 */
CylinderSensor* CylinderSensor::to_cylinder_sensor() const {   // mgiger 6/16/00
    return (CylinderSensor*) this;
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void CylinderSensor::render(OpenVRML::viewer & viewer, rendering_context context)
{
    //
    // Store the ModelView matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation.
    //
    this->modelview = context.matrix();
}

void CylinderSensor::activate(double timeStamp, bool isActive, double *p)
{
    using OpenVRML_::fpequal;

    // Become active
    if (isActive && !this->active.value) {
        this->active.value = isActive;

        // set activation point in local coords
        vec3f Vec(p[0], p[1], p[2]);
        this->activationMatrix = this->modelview.inverse();
        Vec = Vec * this->activationMatrix;
        this->activationPoint.value = Vec;
        // Bearing vector in local coordinate system
        Vec[0] = this->activationMatrix[2][0];
        Vec[1] = this->activationMatrix[2][1];
        Vec[2] = this->activationMatrix[2][2];
        vec3f BV(Vec);
        vec3f Y(0,1,0);
        BV = BV.normalize();
        double ang = acos(BV.dot(Y));
        if (ang > pi_2) { ang = pi - ang; }
        if (ang < this->diskAngle.value) {
            disk.value = true;
        } else {
            disk.value = false;
        }
        // send message
        this->emit_event("isActive", this->active, timeStamp);
    }

    // Become inactive
    else if (!isActive && this->active.value) {
        this->active.value = isActive;
        this->emit_event("isActive", this->active, timeStamp);

        // save auto offset of rotation
        if (this->autoOffset.value) {
            this->offset = rotation_val;
            this->emit_event("offset_changed", this->offset, timeStamp);
        }
    }

        // Tracking
    else if (isActive) {
        // get local coord for touch point
        vec3f Vec(p[0], p[1], p[2]);
        Vec = Vec * this->activationMatrix;
        this->trackPoint.value = Vec;
        this->emit_event("trackPoint_changed", this->trackPoint, timeStamp);
        vec3f tempv;
        float rot, radius;
        vec3f dir1(Vec[0], 0, Vec[2]);
        if (disk.value) {
            radius = 1.0;
        } else {
            radius = dir1.length();    // get the radius
        }
        dir1 = dir1.normalize();
        vec3f dir2(this->activationPoint.value.x(),
                   0,
                   this->activationPoint.value.z());
        dir2 = dir2.normalize();
        tempv = dir2 * dir1;
        vec3f cx(tempv);
        cx = cx.normalize();
        if (cx.length() == 0.0) { return; }
        rot = radius * acos(dir2.dot(dir1));
        if (fpequal(cx.y(), -1.0)) { rot = -rot; }
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
        rotation_val.value = rot;
        this->rotation.value = OpenVRML::rotation(0, 1, 0, rot);

        this->emit_event("rotation_changed", this->rotation, timeStamp);
    }
}

/**
 * @brief set_autoOffset eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void CylinderSensor::processSet_autoOffset(const field_value & value,
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
void CylinderSensor::processSet_diskAngle(const field_value & value,
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
void CylinderSensor::processSet_enabled(const field_value & value,
                                        const double timestamp)
    throw (std::bad_cast)
{
    this->enabled = dynamic_cast<const sfbool &>(value);
    this->emit_event("enabled_changed", this->enabled, timestamp);
}

/**
 * @brief set_maxAngle eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
void CylinderSensor::processSet_maxAngle(const field_value & value,
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
void CylinderSensor::processSet_minAngle(const field_value & value,
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
void CylinderSensor::processSet_offset(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast)
{
    this->offset = dynamic_cast<const sffloat &>(value);
    this->emit_event("offset_changed", this->offset, timestamp);
}


/**
 * @class DirectionalLightClass
 *
 * @brief Class object for DirectionalLight nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
DirectionalLightClass::DirectionalLightClass(OpenVRML::browser & browser):
        node_class(browser) {}

/**
 * @brief Destructor.
 */
DirectionalLightClass::~DirectionalLightClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating DirectionalLight
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by DirectionalLightClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        DirectionalLightClass::create_type(const std::string & id,
                                          const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "ambientIntensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "direction"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "intensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "on")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<DirectionalLight>(*this, id));
    Vrml97NodeTypeImpl<DirectionalLight> & directionalLightNodeType =
            static_cast<Vrml97NodeTypeImpl<DirectionalLight> &>(*type);
    typedef Vrml97NodeTypeImpl<DirectionalLight>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            directionalLightNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &DirectionalLight::processSet_ambientIntensity,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<DirectionalLight, sffloat>
                                    (&DirectionalLight::ambientIntensity)));
        } else if (*itr == supportedInterfaces[1]) {
            directionalLightNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &DirectionalLight::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<DirectionalLight, sfcolor>
                                    (&DirectionalLight::color)));
        } else if (*itr == supportedInterfaces[2]) {
            directionalLightNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &DirectionalLight::processSet_direction,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<DirectionalLight, sfvec3f>
                                    (&DirectionalLight::direction)));
        } else if (*itr == supportedInterfaces[3]) {
            directionalLightNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &DirectionalLight::processSet_intensity,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<DirectionalLight, sffloat>
                                    (&DirectionalLight::intensity)));
        } else if (*itr == supportedInterfaces[4]) {
            directionalLightNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &DirectionalLight::processSet_on,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<DirectionalLight, sfbool>
                                    (&DirectionalLight::on)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class DirectionalLight
 *
 * @brief DirectionalLight node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
DirectionalLight::DirectionalLight(const node_type & type,
                                   const scope_ptr & scope):
    node(type, scope),
    AbstractLight(type, scope),
    direction(vec3f(0.0, 0.0, -1.0))
{}

/**
 * @brief Destructor.
 */
DirectionalLight::~DirectionalLight() throw () {}

/**
 * @brief Render the node.
 *
 * This should be called before rendering any sibling nodes.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void DirectionalLight::render(OpenVRML::viewer & viewer,
                              const rendering_context rc)
{
    if (this->on.value) {
        viewer.insert_dir_light(this->ambientIntensity.value,
                                this->intensity.value,
                                this->color.value,
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
void DirectionalLight::processSet_direction(const field_value & value,
                                            const double timestamp)
        throw (std::bad_cast) {
    this->direction = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("direction_changed", this->direction, timestamp);
}


/**
 * @class ElevationGridClass
 *
 * @brief Class object for ElevationGrid nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
ElevationGridClass::ElevationGridClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
ElevationGridClass::~ElevationGridClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating ElevationGrid nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by ElevationGridClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        ElevationGridClass::create_type(const std::string & id,
                                       const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<ElevationGrid>(*this, id));
    Vrml97NodeTypeImpl<ElevationGrid> & elevationGridNodeType =
            static_cast<Vrml97NodeTypeImpl<ElevationGrid> &>(*type);
    typedef Vrml97NodeTypeImpl<ElevationGrid>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            elevationGridNodeType.addEventIn(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &ElevationGrid::processSet_height);
        } else if (*itr == supportedInterfaces[1]) {
            elevationGridNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &ElevationGrid::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfnode>
                                    (&ElevationGrid::color)));
        } else if (*itr == supportedInterfaces[2]) {
            elevationGridNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &ElevationGrid::processSet_normal,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfnode>
                                    (&ElevationGrid::normal)));
        } else if (*itr == supportedInterfaces[3]) {
            elevationGridNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &ElevationGrid::processSet_texCoord,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfnode>
                                    (&ElevationGrid::texCoord)));
        } else if (*itr == supportedInterfaces[4]) {
            elevationGridNodeType.addField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, mffloat>
                                    (&ElevationGrid::height)));
        } else if (*itr == supportedInterfaces[5]) {
            elevationGridNodeType.addField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfbool>
                                    (&ElevationGrid::ccw)));
        } else if (*itr == supportedInterfaces[6]) {
            elevationGridNodeType.addField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfbool>
                                    (&ElevationGrid::colorPerVertex)));
        } else if (*itr == supportedInterfaces[7]) {
            elevationGridNodeType.addField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sffloat>
                                    (&ElevationGrid::creaseAngle)));
        } else if (*itr == supportedInterfaces[8]) {
            elevationGridNodeType.addField(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfbool>
                                    (&ElevationGrid::normalPerVertex)));
        } else if (*itr == supportedInterfaces[9]) {
            elevationGridNodeType.addField(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfbool>
                                    (&ElevationGrid::solid)));
        } else if (*itr == supportedInterfaces[10]) {
            elevationGridNodeType.addField(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfint32>
                                    (&ElevationGrid::xDimension)));
        } else if (*itr == supportedInterfaces[11]) {
            elevationGridNodeType.addField(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sffloat>
                                    (&ElevationGrid::xSpacing)));
        } else if (*itr == supportedInterfaces[12]) {
            elevationGridNodeType.addField(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sfint32>
                                    (&ElevationGrid::zDimension)));
        } else if (*itr == supportedInterfaces[13]) {
            elevationGridNodeType.addField(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ElevationGrid, sffloat>
                                    (&ElevationGrid::zSpacing)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class ElevationGrid
 *
 * @brief ElevationGrid node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
ElevationGrid::ElevationGrid(const node_type & type,
                             const scope_ptr & scope):
        node(type, scope),
        AbstractGeometry(type, scope),
        ccw(true),
        colorPerVertex(true),
        normalPerVertex(true),
        solid(true),
        xDimension(0),
        xSpacing(1.0f),
        zDimension(0),
        zSpacing(1.0f) {}

/**
 * @brief Destructor.
 */
ElevationGrid::~ElevationGrid() throw () {}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool ElevationGrid::modified() const
{
    return (this->modified_
            || (this->color.value && this->color.value->modified())
            || (this->normal.value && this->normal.value->modified())
            || (this->texCoord.value && this->texCoord.value->modified()));
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void ElevationGrid::update_modified(node_path & path, int flags)
{
    if (this->modified()) { mark_path_modified(path, true); }
    path.push_front(this);
    if (this->color.value) { this->color.value->update_modified(path); }
    if (this->normal.value) { this->normal.value->update_modified(path); }
    if (this->texCoord.value) { this->texCoord.value->update_modified(path); }
    path.pop_front();
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t ElevationGrid::insert_geometry(OpenVRML::viewer & viewer,
                                             const rendering_context context)
{
    viewer::object_t obj = 0;

    if (!this->height.value.empty()) {
        using std::vector;

        color_node * const colorNode = this->color.value->to_color();
        const vector<OpenVRML::color> & color = colorNode
                                              ? colorNode->color()
                                              : vector<OpenVRML::color>();

        normal_node * const normalNode = this->normal.value->to_normal();
        const vector<vec3f> & normal = normalNode
                                     ? normalNode->vector()
                                     : vector<vec3f>();

        texture_coordinate_node * const texCoordNode =
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
void ElevationGrid::processSet_color(const field_value & value,
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
void ElevationGrid::processSet_height(const field_value & value,
                                      const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
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
void ElevationGrid::processSet_normal(const field_value & value,
                                      const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
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
void ElevationGrid::processSet_texCoord(const field_value & value,
                                        const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->texCoord = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("texCoord_changed", this->texCoord, timestamp);
}


/**
 * @class ExtrusionClass
 *
 * @brief Class object for Extrusion nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
ExtrusionClass::ExtrusionClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
ExtrusionClass::~ExtrusionClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Extrusion nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by ExtrusionClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        ExtrusionClass::create_type(const std::string & id,
                                   const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<Extrusion>(*this, id));
    Vrml97NodeTypeImpl<Extrusion> & extrusionNodeType =
            static_cast<Vrml97NodeTypeImpl<Extrusion> &>(*type);
    typedef Vrml97NodeTypeImpl<Extrusion>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            extrusionNodeType.addEventIn(supportedInterfaces[0].field_type,
                                         supportedInterfaces[0].id,
                                         &Extrusion::processSet_crossSection);
        } else if (*itr == supportedInterfaces[1]) {
            extrusionNodeType.addEventIn(supportedInterfaces[1].field_type,
                                         supportedInterfaces[1].id,
                                         &Extrusion::processSet_orientation);
        } else if (*itr == supportedInterfaces[2]) {
            extrusionNodeType.addEventIn(supportedInterfaces[2].field_type,
                                         supportedInterfaces[2].id,
                                         &Extrusion::processSet_scale);
        } else if (*itr == supportedInterfaces[3]) {
            extrusionNodeType.addEventIn(supportedInterfaces[3].field_type,
                                         supportedInterfaces[3].id,
                                         &Extrusion::processSet_spine);
        } else if (*itr == supportedInterfaces[4]) {
            extrusionNodeType.addField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, sfbool>
                                    (&Extrusion::beginCap)));
        } else if (*itr == supportedInterfaces[5]) {
            extrusionNodeType.addField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, sfbool>
                                    (&Extrusion::ccw)));
        } else if (*itr == supportedInterfaces[6]) {
            extrusionNodeType.addField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, sfbool>
                                    (&Extrusion::convex)));
        } else if (*itr == supportedInterfaces[7]) {
            extrusionNodeType.addField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, sffloat>
                                    (&Extrusion::creaseAngle)));
        } else if (*itr == supportedInterfaces[8]) {
            extrusionNodeType.addField(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, mfvec2f>
                                    (&Extrusion::crossSection)));
        } else if (*itr == supportedInterfaces[9]) {
            extrusionNodeType.addField(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, sfbool>
                                    (&Extrusion::endCap)));
        } else if (*itr == supportedInterfaces[10]) {
            extrusionNodeType.addField(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, mfrotation>
                                    (&Extrusion::orientation)));
        } else if (*itr == supportedInterfaces[11]) {
            extrusionNodeType.addField(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, mfvec2f>
                                    (&Extrusion::scale)));
        } else if (*itr == supportedInterfaces[12]) {
            extrusionNodeType.addField(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, sfbool>
                                    (&Extrusion::solid)));
        } else if (*itr == supportedInterfaces[13]) {
            extrusionNodeType.addField(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Extrusion, mfvec3f>
                                    (&Extrusion::spine)));
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
 * @class Extrusion
 *
 * @brief Extrusion node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Extrusion::Extrusion(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    AbstractGeometry(type, scope),
    beginCap(true),
    ccw(true),
    convex(true),
    creaseAngle(0),
    crossSection(extrusionDefaultCrossSection_,
                 extrusionDefaultCrossSection_ + 5),
    endCap(true),
    orientation(extrusionDefaultOrientation_, extrusionDefaultOrientation_ + 1),
    scale(extrusionDefaultScale_, extrusionDefaultScale_ + 1),
    solid(true),
    spine(extrusionDefaultSpine_, extrusionDefaultSpine_ + 2)
{}

/**
 * @brief Destructor.
 */
Extrusion::~Extrusion() throw () {}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t Extrusion::insert_geometry(OpenVRML::viewer & viewer,
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
void Extrusion::processSet_crossSection(const field_value & value,
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
void Extrusion::processSet_orientation(const field_value & value,
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
void Extrusion::processSet_scale(const field_value & value,
                                 const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
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
void Extrusion::processSet_spine(const field_value & value,
                                 const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->spine = dynamic_cast<const mfvec3f &>(value);
    this->node::modified(true);
}


/**
 * @class FogClass
 *
 * @brief Class object for Fog nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
FogClass::FogClass(OpenVRML::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destructor.
 */
FogClass::~FogClass() throw () {}

/**
 * @brief Set the first Fog node in the world.
 *
 * The first Fog node in the world is used for the initial fog.
 * This method is used by Fog::do_initialize.
 *
 * @param fog   a Fog node.
 */
void FogClass::setFirst(Fog & fog) throw ()
{
    this->first = &fog;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by Fog::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool FogClass::hasFirst() const throw ()
{
    return this->first;
}

/**
 * @brief Push a Fog on the top of the bound node stack.
 *
 * @param fog       the node to bind.
 * @param timestamp the current time.
 */
void FogClass::bind(Fog & fog, const double timestamp) throw (std::bad_alloc)
{
    using std::find;

    //
    // If the node is already the active node, do nothing.
    //
    if (!this->boundNodes.empty() && &fog == this->boundNodes.back()) {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const BoundNodes::iterator pos =
        find(this->boundNodes.begin(), this->boundNodes.end(), &fog);
    if (pos != this->boundNodes.end()) { this->boundNodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->boundNodes.empty()) {
        Fog & current = dynamic_cast<Fog &>(*this->boundNodes.back());
        current.bound.value = false;
        current.emit_event("isBound", current.bound, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->boundNodes.push_back(&fog);
    fog.bound.value = true;
    fog.emit_event("isBound", fog.bound, timestamp);
}

/**
 * @brief Remove a Fog from the bound node stack.
 *
 * @param fog       the node to unbind.
 * @param timestamp the current time.
 */
void FogClass::unbind(Fog & fog, const double timestamp) throw ()
{
    const BoundNodes::iterator pos =
            std::find(this->boundNodes.begin(), this->boundNodes.end(), &fog);
    if (pos != this->boundNodes.end()) {
        fog.bound.value = false;
        fog.emit_event("isBound", fog.bound, timestamp);

        if (pos == this->boundNodes.end() - 1
                && this->boundNodes.size() > 1) {
            Fog & newActive =
                    dynamic_cast<Fog &>(**(this->boundNodes.end() - 2));
            newActive.bound.value = true;
            newActive.emit_event("isBound", newActive.bound, timestamp);
        }
        this->boundNodes.erase(pos);
    }
}

/**
 * @brief NodeClass-specific initialization.
 *
 * @param initialViewpoint  the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void FogClass::initialize(viewpoint_node * initialViewpoint,
                          const double timestamp)
    throw ()
{
    if (this->first) {
        this->first->process_event("set_bind", sfbool(true), timestamp);
    }
}

/**
 * @brief NodeClass-specific rendering.
 *
 * Render the active Fog node.
 *
 * @param viewer    a Viewer.
 */
void FogClass::render(OpenVRML::viewer & viewer) throw ()
{
    if (!this->boundNodes.empty()) {
        Fog & fog = dynamic_cast<Fog &>(*this->boundNodes.back());
        viewer.set_fog(fog.color.value,
                       fog.visibilityRange.value,
                       fog.fogType.value.c_str());
    }
}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Fog nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by FogClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr FogClass::create_type(const std::string & id,
                                       const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sfbool_id, "set_bind"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfstring_id, "fogType"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "visibilityRange"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isBound")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Fog>(*this, id));
    Vrml97NodeTypeImpl<Fog> & fogNodeType =
            static_cast<Vrml97NodeTypeImpl<Fog> &>(*type);
    typedef Vrml97NodeTypeImpl<Fog>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            fogNodeType.addEventIn(supportedInterfaces[0].field_type,
                                   supportedInterfaces[0].id,
                                   &Fog::processSet_bind);
        } else if (*itr == supportedInterfaces[1]) {
            fogNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Fog::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Fog, sfcolor>
                                    (&Fog::color)));
        } else if (*itr == supportedInterfaces[2]) {
            fogNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Fog::processSet_fogType,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Fog, sfstring>
                                    (&Fog::fogType)));
        } else if (*itr == supportedInterfaces[3]) {
            fogNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Fog::processSet_visibilityRange,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Fog, sffloat>
                                    (&Fog::visibilityRange)));
        } else if (*itr == supportedInterfaces[4]) {
            fogNodeType.addEventOut(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Fog, sfbool>
                                    (&Fog::bound)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Fog
 *
 * @brief Fog node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Fog::Fog(const node_type & type,
         const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    color(OpenVRML::color(1.0, 1.0, 1.0)),
    fogType("LINEAR"),
    visibilityRange(0.0),
    bound(false)
{}

/**
 * @brief Destructor.
 */
Fog::~Fog() throw ()
{}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void Fog::do_initialize(const double timestamp) throw ()
{
    FogClass & nodeClass = static_cast<FogClass &>(this->type._class);
    if (!nodeClass.hasFirst()) { nodeClass.setFirst(*this); }
}

/**
 * @brief Shut down.
 *
 * Calls FogClass::unbind to unbind the node if it is bound.
 *
 * @param timestamp the current time.
 */
void Fog::do_shutdown(const double timestamp) throw ()
{
    FogClass & nodeClass = static_cast<FogClass &>(this->type._class);
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
void Fog::processSet_bind(const field_value & value, const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    const sfbool & bind = dynamic_cast<const sfbool &>(value);
    FogClass & nodeClass = static_cast<FogClass &>(this->type._class);
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
void Fog::processSet_color(const field_value & value, const double timestamp)
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
void Fog::processSet_fogType(const field_value & value,
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
void Fog::processSet_visibilityRange(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->visibilityRange = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("visibilityRange_changed", this->visibilityRange,
                    timestamp);
}


/**
 * @class FontStyleClass
 *
 * @brief Class object for FontStyle nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
FontStyleClass::FontStyleClass(OpenVRML::browser & browser): node_class(browser) {}

FontStyleClass::~FontStyleClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating FontStyle nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by FontStyleClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
FontStyleClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<FontStyle>(*this, id));
    Vrml97NodeTypeImpl<FontStyle> & fontStyleNodeType =
            static_cast<Vrml97NodeTypeImpl<FontStyle> &>(*type);
    typedef Vrml97NodeTypeImpl<FontStyle>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            fontStyleNodeType.addField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, mfstring>
                                    (&FontStyle::family_)));
        } else if (*itr == supportedInterfaces[1]) {
            fontStyleNodeType.addField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, sfbool>
                                    (&FontStyle::horizontal_)));
        } else if (*itr == supportedInterfaces[2]) {
            fontStyleNodeType.addField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, mfstring>
                                    (&FontStyle::justify_)));
        } else if (*itr == supportedInterfaces[3]) {
            fontStyleNodeType.addField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, sfstring>
                                    (&FontStyle::language_)));
        } else if (*itr == supportedInterfaces[4]) {
            fontStyleNodeType.addField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, sfbool>
                                    (&FontStyle::leftToRight)));
        } else if (*itr == supportedInterfaces[5]) {
            fontStyleNodeType.addField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, sffloat>
                                    (&FontStyle::size_)));
        } else if (*itr == supportedInterfaces[6]) {
            fontStyleNodeType.addField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, sffloat>
                                    (&FontStyle::spacing_)));
        } else if (*itr == supportedInterfaces[7]) {
            fontStyleNodeType.addField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, sfstring>
                                    (&FontStyle::style_)));
        } else if (*itr == supportedInterfaces[8]) {
            fontStyleNodeType.addField(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<FontStyle, sfbool>
                                    (&FontStyle::topToBottom)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class FontStyle
 *
 * @brief FontStyle node instances.
 */

namespace {
    const std::string fontStyleInitFamily_[] = { "SERIF" };
    const std::string fontStyleInitJustify_[] = { "BEGIN", "FIRST" };
}

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
FontStyle::FontStyle(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    font_style_node(type, scope),
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
 * @brief Destructor.
 */
FontStyle::~FontStyle() throw ()
{}

/**
 * @brief Get the list of font families.
 *
 * @return an mfstring containing the font families that may be used for this
 *      FontStyle.
 */
const std::vector<std::string> & FontStyle::family() const throw ()
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
bool FontStyle::horizontal() const throw ()
{
    return this->horizontal_.value;
}

/**
 * @brief Get the descriptor for the text justification.
 *
 * @return a string array describing the characteristics of the text
 *      justification.
 */
const std::vector<std::string> & FontStyle::justify() const throw ()
{
    return this->justify_.value;
}

/**
 * @brief Get the language code.
 *
 * @return the language code.
 */
const std::string & FontStyle::language() const throw ()
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
bool FontStyle::left_to_right() const throw ()
{
    return this->leftToRight.value;
}

/**
 * @brief Get the size of the text.
 *
 * @return the size of the text.
 */
float FontStyle::size() const throw ()
{
    return this->size_.value;
}

/**
 * @brief Get the spacing for the text.
 *
 * @return the spacing for the text.
 */
float FontStyle::spacing() const throw ()
{
    return this->spacing_.value;
}

/**
 * @brief Get the style for the text.
 *
 * @return an string descriptor of the text style.
 */
const std::string & FontStyle::style() const throw ()
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
bool FontStyle::top_to_bottom() const throw ()
{
    return this->topToBottom.value;
}


/**
 * @class GroupClass
 *
 * @brief Class object for Group nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
GroupClass::GroupClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
GroupClass::~GroupClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Group nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by GroupClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr GroupClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<Group>(*this, id));
    Vrml97NodeTypeImpl<Group> & groupNodeType =
            static_cast<Vrml97NodeTypeImpl<Group> &>(*type);
    typedef Vrml97NodeTypeImpl<Group>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            groupNodeType.addEventIn(supportedInterfaces[0].field_type,
                                     supportedInterfaces[0].id,
                                     &Group::processAddChildren);
        } else if (*itr == supportedInterfaces[1]) {
            groupNodeType.addEventIn(supportedInterfaces[1].field_type,
                                     supportedInterfaces[1].id,
                                     &Group::processRemoveChildren);
        } else if (*itr == supportedInterfaces[2]) {
            groupNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Group::processSet_children,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Group, mfnode>
                                    (&Group::children_)));
        } else if (*itr == supportedInterfaces[3]) {
            groupNodeType.addField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Group, sfvec3f>
                                    (&Group::bboxCenter)));
        } else if (*itr == supportedInterfaces[4]) {
            groupNodeType.addField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Group, sfvec3f>
                                (&Group::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Group
 *
 * @brief Represents Group node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
Group::Group(const node_type & type,
             const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    bboxSize(vec3f(-1.0, -1.0, -1.0)),
    viewerObject(0)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destructor.
 */
Group::~Group() throw () {
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
void Group::processAddChildren(const field_value & value,
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
void Group::processRemoveChildren(const field_value & value,
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
void Group::processSet_children(const field_value & value,
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
bool Group::modified() const
{
    if (this->modified_) { return true; }
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        if (this->children_.value[i]->modified()) { return true; }
    }
    return false;
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void Group::update_modified(node_path & path, int flags)
{
    // if the mark_modifed short circuit doesn't
    // pan out, we should be a little smarter here...
    if (this->modified()) { mark_path_modified(path, true, flags); }
    path.push_front(this);
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        this->children_.value[i]->update_modified(path, flags);
    }
    path.pop_front();
}

/**
 * @brief Render the node.
 *
 * Render each of the children.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void Group::render(OpenVRML::viewer & viewer, rendering_context context)
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
    this->renderNoCull(viewer, context);
}


/**
 * because children will already have done the culling, we don't need
 * to repeat it here.
 */
void Group::renderNoCull(OpenVRML::viewer & viewer, rendering_context context)
{
    if (this->viewerObject && this->modified()) {
        viewer.remove_object(this->viewerObject);
        this->viewerObject = 0;
    }

    if (this->viewerObject) {
        viewer.insert_reference(this->viewerObject);
    } else if (this->children_.value.size() > 0) {
        int i, n = this->children_.value.size();
        int nSensors = 0;

        this->viewerObject = viewer.begin_object(this->id().c_str());

        // Draw nodes that impact their siblings (DirectionalLights,
        // TouchSensors, any others? ...)
        for (i = 0; i < n; ++i) {
          const node_ptr & kid = this->children_.value[i];

            if (kid->to_light()
                    && !(kid->to_point_light() || kid->to_spot_light())) {
                kid->render(viewer, context);
            } else if ((kid->to_touch_sensor()
                        && kid->to_touch_sensor()->isEnabled())
                    || (kid->to_plane_sensor()
                        && kid->to_plane_sensor()->isEnabled())
                    || (kid->to_cylinder_sensor()
                        && kid->to_cylinder_sensor()->isEnabled())
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
const std::vector<node_ptr> & Group::children() const throw ()
{
    return this->children_.value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void Group::activate(double time, bool isOver, bool isActive, double *p)
{
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        const node_ptr & node = this->children_.value[i];
        if (node) {
            if (node->to_touch_sensor()
                    && node->to_touch_sensor()->isEnabled()) {
                node->to_touch_sensor()->activate(time, isOver, isActive, p);
            } else if (node->to_plane_sensor()
                    && node->to_plane_sensor()->isEnabled()) {
                node->to_plane_sensor()->activate(time, isActive, p);
            } else if (node->to_cylinder_sensor()
                    && node->to_cylinder_sensor()->isEnabled()) {
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
const bounding_volume & Group::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<Group *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief Recalculate the bounding volume.
 */
void Group::recalcBSphere()
{
    this->bsphere.reset();
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        const node_ptr & node = this->children_.value[i];
        if (node) {
            const OpenVRML::bounding_volume & ci_bv = node->bounding_volume();
            this->bsphere.extend(ci_bv);
        }
    }
    this->bounding_volume_dirty(false);
}


/**
 * @class ImageTextureClass
 *
 * @brief Class object for ImageTexture nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
ImageTextureClass::ImageTextureClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
ImageTextureClass::~ImageTextureClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating ImageTexture nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by ImageTextureClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
ImageTextureClass::create_type(const std::string & id,
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
        type(new Vrml97NodeTypeImpl<ImageTexture>(*this, id));
    Vrml97NodeTypeImpl<ImageTexture> & imageTextureNodeType =
            static_cast<Vrml97NodeTypeImpl<ImageTexture> &>(*type);
    typedef Vrml97NodeTypeImpl<ImageTexture>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            imageTextureNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &ImageTexture::processSet_url,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ImageTexture, mfstring>
                                    (&ImageTexture::url)));
        } else if (*itr == supportedInterfaces[1]) {
            imageTextureNodeType.addField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ImageTexture, sfbool>
                                    (&ImageTexture::repeatS)));
        } else if (*itr == supportedInterfaces[2]) {
            imageTextureNodeType.addField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ImageTexture, sfbool>
                                    (&ImageTexture::repeatT)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class ImageTexture
 *
 * @brief Represents ImageTexture node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
ImageTexture::ImageTexture(const node_type & type,
                           const scope_ptr & scope):
        node(type, scope),
        AbstractTexture(type, scope),
        image(0),
        texObject(0) {}

/**
 * @brief Destructor.
 */
ImageTexture::~ImageTexture() throw () {
    delete this->image;
    // delete texObject...
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void ImageTexture::render(OpenVRML::viewer & viewer, rendering_context context)
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
        this->image = new Image;
        if (!this->image->tryURLs(this->url, &baseDoc)) {
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
            int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
            int nSizes = sizeof(sizes) / sizeof(int);
            int w = this->image->w();
            int h = this->image->h();
            int i, j;
            for (i = 0; i < nSizes; ++i) { if (w < sizes[i]) { break; } }
            for (j = 0; j < nSizes; ++j) { if (h < sizes[j]) { break; } }

            if (i > 0 && j > 0) {
                // Always scale images down in size and reuse the same pixel
                // memory. This can cause some ugliness...
                if (w != sizes[i - 1] || h != sizes[j - 1]) {
                    viewer.scale_texture(w, h, sizes[i - 1], sizes[j - 1],
                                        this->image->nc(), pix);
                    this->image->setSize(sizes[i - 1], sizes[j - 1]);
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

size_t ImageTexture::nComponents() const throw () {
    return this->image ? this->image->nc() : 0;
}

size_t ImageTexture::width() const throw () {
    return this->image ? this->image->w() : 0;
}

size_t ImageTexture::height() const throw () {
    return this->image ? this->image->h() : 0;
}

size_t ImageTexture::nFrames() const throw () { return 0; }

const unsigned char * ImageTexture::pixels() const throw () {
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
void ImageTexture::processSet_url(const field_value & value,
                                  const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->url = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("url_changed", this->url, timestamp);
}


/**
 * @class IndexedFaceSetClass
 *
 * @brief Class object for IndexedFaceSet nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
IndexedFaceSetClass::IndexedFaceSetClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
IndexedFaceSetClass::~IndexedFaceSetClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating IndexedFaceSet nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by IndexedFaceSetClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        IndexedFaceSetClass::create_type(const std::string & id,
                                        const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<IndexedFaceSet>(*this, id));
    Vrml97NodeTypeImpl<IndexedFaceSet> & indexedFaceSetNodeType =
            static_cast<Vrml97NodeTypeImpl<IndexedFaceSet> &>(*type);
    typedef Vrml97NodeTypeImpl<IndexedFaceSet>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            indexedFaceSetNodeType
                    .addEventIn(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &IndexedFaceSet::processSet_colorIndex);
        } else if (*itr == supportedInterfaces[1]) {
            indexedFaceSetNodeType
                    .addEventIn(supportedInterfaces[1].field_type,
                                supportedInterfaces[1].id,
                                &IndexedFaceSet::processSet_coordIndex);
        } else if (*itr == supportedInterfaces[2]) {
            indexedFaceSetNodeType
                    .addEventIn(supportedInterfaces[2].field_type,
                                supportedInterfaces[2].id,
                                &IndexedFaceSet::processSet_normalIndex);
        } else if (*itr == supportedInterfaces[3]) {
            indexedFaceSetNodeType
                    .addEventIn(supportedInterfaces[3].field_type,
                                supportedInterfaces[3].id,
                                &IndexedFaceSet::processSet_texCoordIndex);
        } else if (*itr == supportedInterfaces[4]) {
            indexedFaceSetNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &IndexedFaceSet::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfnode>
                                    (&IndexedFaceSet::color_)));
        } else if (*itr == supportedInterfaces[5]) {
            indexedFaceSetNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &IndexedFaceSet::processSet_coord,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfnode>
                                    (&IndexedFaceSet::coord)));
        } else if (*itr == supportedInterfaces[6]) {
            indexedFaceSetNodeType.addExposedField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &IndexedFaceSet::processSet_normal,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfnode>
                                    (&IndexedFaceSet::normal)));
        } else if (*itr == supportedInterfaces[7]) {
            indexedFaceSetNodeType.addExposedField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &IndexedFaceSet::processSet_texCoord,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfnode>
                                    (&IndexedFaceSet::texCoord)));
        } else if (*itr == supportedInterfaces[8]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfbool>
                                    (&IndexedFaceSet::ccw)));
        } else if (*itr == supportedInterfaces[9]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, mfint32>
                                    (&IndexedFaceSet::colorIndex)));
        } else if (*itr == supportedInterfaces[10]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfbool>
                                    (&IndexedFaceSet::colorPerVertex)));
        } else if (*itr == supportedInterfaces[11]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfbool>
                                    (&IndexedFaceSet::convex)));
        } else if (*itr == supportedInterfaces[12]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, mfint32>
                                    (&IndexedFaceSet::coordIndex)));
        } else if (*itr == supportedInterfaces[13]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sffloat>
                                    (&IndexedFaceSet::creaseAngle)));
        } else if (*itr == supportedInterfaces[14]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[14].field_type,
                supportedInterfaces[14].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, mfint32>
                                    (&IndexedFaceSet::normalIndex)));
        } else if (*itr == supportedInterfaces[15]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[15].field_type,
                supportedInterfaces[15].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfbool>
                                    (&IndexedFaceSet::normalPerVertex)));
        } else if (*itr == supportedInterfaces[16]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[16].field_type,
                supportedInterfaces[16].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, sfbool>
                                    (&IndexedFaceSet::solid)));
        } else if (*itr == supportedInterfaces[17]) {
            indexedFaceSetNodeType.addField(
                supportedInterfaces[17].field_type,
                supportedInterfaces[17].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedFaceSet, mfint32>
                                    (&IndexedFaceSet::texCoordIndex)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class IndexedFaceSet
 *
 * @brief Represents IndexedFaceSet node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
IndexedFaceSet::IndexedFaceSet(const node_type & type,
                               const scope_ptr & scope):
        node(type, scope),
        AbstractIndexedSet(type, scope),
        ccw(true),
        convex(true),
        creaseAngle(0.0),
        normalPerVertex(true),
        solid(true) {
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destructor.
 */
IndexedFaceSet::~IndexedFaceSet() throw () {}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool IndexedFaceSet::modified() const {
    return (this->modified_
            || (this->color_.value && this->color_.value->modified())
            || (this->coord.value && this->coord.value->modified())
            || (this->normal.value && this->normal.value->modified())
            || (this->texCoord.value && this->texCoord.value->modified()));
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void IndexedFaceSet::update_modified(node_path& path, int flags) {
    if (this->modified()) { mark_path_modified(path, true, flags); }
    path.push_front(this);
    if (this->color_.value) {
        this->color_.value->update_modified(path, flags);
    }
    if (this->coord.value) {
        this->coord.value->update_modified(path, flags);
    }
    if (this->normal.value) {
        this->normal.value->update_modified(path, flags);
    }
    if (this->texCoord.value) {
        this->texCoord.value->update_modified(path, flags);
    }
    path.pop_front();
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @todo stripify, crease angle, generate normals ...
 */
viewer::object_t IndexedFaceSet::insert_geometry(OpenVRML::viewer & viewer,
                                               const rendering_context context)
{
    using std::vector;

    if (context.draw_bounding_spheres) {
        assert(dynamic_cast<const bounding_sphere *>(&this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
        viewer.draw_bounding_sphere(bs, static_cast<bounding_volume::intersection>(4));
    }

    coordinate_node * const coordinateNode = this->coord.value
                                          ? this->coord.value->to_coordinate()
                                          : 0;
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    color_node * const colorNode = this->color_.value
                                 ? this->color_.value->to_color()
                                 : 0;
    const vector<OpenVRML::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<OpenVRML::color>();

    normal_node * const normalNode = this->normal.value
                                   ? this->normal.value->to_normal()
                                   : 0;
    const vector<vec3f> & normal = normalNode
                                 ? normalNode->vector()
                                 : vector<vec3f>();

    texture_coordinate_node * const texCoordNode = this->texCoord.value
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
void IndexedFaceSet::recalcBSphere()
{
    // take the bvolume of all the points. technically, we should figure
    // out just which points are used by the index and just use those,
    // but for a first pass this is fine (also: if we do it this way
    // then we don't have to update the bvolume when the index
    // changes). motto: always do it the simple way first...
    //
    coordinate_node * const coordinateNode = this->coord.value
                                          ? this->coord.value->to_coordinate()
                                          : 0;
    if (coordinateNode) {
        const std::vector<vec3f> & coord = coordinateNode->point();
        this->bsphere.reset();
        this->bsphere.enclose(coord);
    }
    this->bounding_volume_dirty(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & IndexedFaceSet::bounding_volume() const {
    if (this->bounding_volume_dirty()) {
        const_cast<IndexedFaceSet *>(this)->recalcBSphere();
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
void IndexedFaceSet::processSet_normal(const field_value & value,
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
void IndexedFaceSet::processSet_normalIndex(const field_value & value,
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
void IndexedFaceSet::processSet_texCoord(const field_value & value,
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
void IndexedFaceSet::processSet_texCoordIndex(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->texCoordIndex = dynamic_cast<const mfint32 &>(value);
    this->node::modified(true);
}


/**
 * @class IndexedLineSetClass
 *
 * @brief Class object for IndexedLineSet nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
IndexedLineSetClass::IndexedLineSetClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
IndexedLineSetClass::~IndexedLineSetClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating IndexedLineSet
 *         nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by IndexedLineSetClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
IndexedLineSetClass::create_type(const std::string & id,
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
        type(new Vrml97NodeTypeImpl<IndexedLineSet>(*this, id));
    Vrml97NodeTypeImpl<IndexedLineSet> & indexedLineSetNodeType =
        static_cast<Vrml97NodeTypeImpl<IndexedLineSet> &>(*type);
    typedef Vrml97NodeTypeImpl<IndexedLineSet>::NodeFieldPtrPtr
        NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            indexedLineSetNodeType
                    .addEventIn(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &IndexedLineSet::processSet_colorIndex);
        } else if (*itr == supportedInterfaces[1]) {
            indexedLineSetNodeType
                    .addEventIn(supportedInterfaces[1].field_type,
                                supportedInterfaces[1].id,
                                &IndexedLineSet::processSet_coordIndex);
        } else if (*itr == supportedInterfaces[2]) {
            indexedLineSetNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &IndexedLineSet::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedLineSet, sfnode>
                                    (&IndexedLineSet::color_)));
        } else if (*itr == supportedInterfaces[3]) {
            indexedLineSetNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &IndexedLineSet::processSet_coord,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedLineSet, sfnode>
                                    (&IndexedLineSet::coord)));
        } else if (*itr == supportedInterfaces[4]) {
            indexedLineSetNodeType.addField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedLineSet, mfint32>
                                    (&IndexedLineSet::colorIndex)));
        } else if (*itr == supportedInterfaces[5]) {
            indexedLineSetNodeType.addField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedLineSet, sfbool>
                                    (&IndexedLineSet::colorPerVertex)));
        } else if (*itr == supportedInterfaces[6]) {
            indexedLineSetNodeType.addField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<IndexedLineSet, mfint32>
                                    (&IndexedLineSet::coordIndex)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class IndexedLineSet
 *
 * @brief Represents IndexedLineSet node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
IndexedLineSet::IndexedLineSet(const node_type & type,
                               const scope_ptr & scope):
        node(type, scope),
        AbstractIndexedSet(type, scope) {}

/**
 * @brief Destructor.
 */
IndexedLineSet::~IndexedLineSet() throw () {}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @todo colors
 */
viewer::object_t IndexedLineSet::insert_geometry(OpenVRML::viewer & viewer,
                                              const rendering_context context)
{
    using std::vector;

    coordinate_node * const coordinateNode = this->coord.value
                                          ? this->coord.value->to_coordinate()
                                          : 0;
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    color_node * const colorNode = this->color_.value
                                 ? this->color_.value->to_color()
                                 : 0;
    const vector<OpenVRML::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<OpenVRML::color>();

    viewer::object_t obj =
        viewer.insert_line_set(coord, this->coordIndex.value,
                               this->colorPerVertex.value,
                               color, this->colorIndex.value);

    if (this->color_.value) { this->color_.value->modified(false); }
    if (this->coord.value)  { this->coord.value->modified(false); }

    return obj;
}


/**
 * @class InlineClass
 *
 * @brief Class object for Inline nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
InlineClass::InlineClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
InlineClass::~InlineClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Inline nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by InlineClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
InlineClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<Inline>(*this, id));
    Vrml97NodeTypeImpl<Inline> & inlineNodeType =
            static_cast<Vrml97NodeTypeImpl<Inline> &>(*type);
    typedef Vrml97NodeTypeImpl<Inline>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            inlineNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &Inline::processSet_url,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Inline, mfstring>
                                    (&Inline::url)));
        } else if (*itr == supportedInterfaces[1]) {
            inlineNodeType.addField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Inline, sfvec3f>
                                    (&Inline::bboxCenter)));
        } else if (*itr == supportedInterfaces[2]) {
            inlineNodeType.addField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Inline, sfvec3f>
                                    (&Inline::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Inline
 *
 * @brief Represents Inline node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
Inline::Inline(const node_type & type,
               const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    inlineScene(0),
    hasLoaded(false)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destructor.
 */
Inline::~Inline() throw () {}

/**
 * @brief Render the node.
 *
 * Render each of the children.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void Inline::render(OpenVRML::viewer & viewer, const rendering_context context)
{
    this->load();
    if (this->inlineScene) { this->inlineScene->render(viewer, context); }
}

Inline * Inline::to_inline() const { return const_cast<Inline *>(this); }

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<node_ptr> & Inline::children() const throw ()
{
    static const std::vector<node_ptr> empty;
    return this->inlineScene
            ? this->inlineScene->nodes()
            : empty;
}

/**
 * Pass on to enabled touchsensor child.
 */
void Inline::activate(double time, bool isOver, bool isActive, double *p)
{
    const std::vector<node_ptr> & children = this->children();
    for (size_t i = 0; i < children.size(); ++i) {
        const node_ptr & node = children[i];
        if (node) {
            if (node->to_touch_sensor()
                    && node->to_touch_sensor()->isEnabled()) {
                node->to_touch_sensor()->activate(time, isOver, isActive, p);
            } else if (node->to_plane_sensor()
                    && node->to_plane_sensor()->isEnabled()) {
                node->to_plane_sensor()->activate(time, isActive, p);
            } else if (node->to_cylinder_sensor()
                    && node->to_cylinder_sensor()->isEnabled()) {
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
void Inline::load() {
    //
    // XXX Need to check whether Url has been modified.
    //
    if (this->hasLoaded) { return; }

    this->hasLoaded = true; // although perhaps not successfully
    this->bounding_volume_dirty(true);

    assert(this->scene());
    this->inlineScene = new OpenVRML::scene(this->scene()->browser,
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
void Inline::processSet_url(const field_value & value, const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->url = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("url_changed", this->url, timestamp);
}


/**
 * @class LODClass
 *
 * @brief Class object for LOD nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
LODClass::LODClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
LODClass::~LODClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating LOD nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by LODClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr LODClass::create_type(const std::string & id,
                                       const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "level"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "center"),
        node_interface(node_interface::field_id, field_value::mffloat_id, "range")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<LOD>(*this, id));
    Vrml97NodeTypeImpl<LOD> & lodNodeType =
            static_cast<Vrml97NodeTypeImpl<LOD> &>(*type);
    typedef Vrml97NodeTypeImpl<LOD>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            lodNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &LOD::processSet_level,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<LOD, mfnode>
                                    (&LOD::level)));
        } else if (*itr == supportedInterfaces[1]) {
            lodNodeType.addField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<LOD, sfvec3f>
                                    (&LOD::center)));
        } else if (*itr == supportedInterfaces[2]) {
            lodNodeType.addField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<LOD, mffloat>
                                    (&LOD::range)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class LOD
 *
 * @brief Represents LOD node instances.
 */

/**
 * @var bounding_sphere LOD::bsphere
 *
 * @brief Cached copy of the bounding_sphere enclosing this node's children.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
LOD::LOD(const node_type & type,
         const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    children_(1)
{
    this->bounding_volume_dirty(true); // lazy calc of bvolume
}

/**
 * @brief Destructor.
 */
LOD::~LOD() throw () {}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool LOD::modified() const {
    if (this->modified_) { return true; }

    // This should really check which range is being rendered...
    for (size_t i = 0; i < this->level.value.size(); ++i) {
        if (this->level.value[i]->modified()) { return true; }
    }
    return false;
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void LOD::update_modified(node_path & path, int flags) {
    //
    // what happens if one of the other children suddenly becomes the one
    // selected? to be safe: check them all. this potentially means some
    // extra work, but it's a lot easier to reason about.
    //
    if (this->modified()) { mark_path_modified(path, true); }
    path.push_front(this);
    for (size_t i = 0; i < this->level.value.size(); ++i) {
        this->level.value[i]->update_modified(path);
    }
    path.pop_front();
}

/**
 * @brief Render the node.
 *
 * Render one of the children.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void LOD::render(OpenVRML::viewer & viewer, const rendering_context context)
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
const bounding_volume & LOD::bounding_volume() const {
    if (this->bounding_volume_dirty()) {
        const_cast<LOD *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<node_ptr> & LOD::children() const throw ()
{
    return this->children_.value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void LOD::activate(double time, bool isOver, bool isActive, double *p)
{
    const std::vector<node_ptr> & children = this->children();
    const node_ptr & node = children[0];
    if (node) {
        if (node->to_touch_sensor() && node->to_touch_sensor()->isEnabled()) {
            node->to_touch_sensor()->activate(time, isOver, isActive, p);
        } else if (node->to_plane_sensor()
                && node->to_plane_sensor()->isEnabled()) {
            node->to_plane_sensor()->activate(time, isActive, p);
        } else if (node->to_cylinder_sensor()
                && node->to_cylinder_sensor()->isEnabled()) {
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
void LOD::recalcBSphere() {
    this->bsphere.reset();

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
            const OpenVRML::bounding_volume & ci_bv = node->bounding_volume();
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
void LOD::processSet_level(const field_value & value, const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->level = dynamic_cast<const mfnode &>(value);
    this->node::modified(true);
    this->emit_event("level_changed", this->level, timestamp);
}


/**
 * @class MaterialClass
 *
 * @brief Class object for Material nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
MaterialClass::MaterialClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
MaterialClass::~MaterialClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Material nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by MaterialClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr MaterialClass::create_type(const std::string & id,
                                            const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "ambientIntensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "diffuseColor"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "emissiveColor"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "shininess"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "specularColor"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "transparency")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Material>(*this, id));
    Vrml97NodeTypeImpl<Material> & materialNodeType =
            static_cast<Vrml97NodeTypeImpl<Material> &>(*type);
    typedef Vrml97NodeTypeImpl<Material>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            materialNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &Material::processSet_ambientIntensity,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Material, sffloat>
                                    (&Material::ambientIntensity)));
        } else if (*itr == supportedInterfaces[1]) {
            materialNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Material::processSet_diffuseColor,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Material, sfcolor>
                                    (&Material::diffuseColor)));
        } else if (*itr == supportedInterfaces[2]) {
            materialNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Material::processSet_emissiveColor,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Material, sfcolor>
                                    (&Material::emissiveColor)));
        } else if (*itr == supportedInterfaces[3]) {
            materialNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Material::processSet_shininess,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Material, sffloat>
                                    (&Material::shininess_)));
        } else if (*itr == supportedInterfaces[4]) {
            materialNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &Material::processSet_specularColor,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Material, sfcolor>
                                    (&Material::specularColor)));
        } else if (*itr == supportedInterfaces[5]) {
            materialNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &Material::processSet_transparency,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Material, sffloat>
                                    (&Material::transparency_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Material
 *
 * @brief Material node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
Material::Material(const node_type & type,
                   const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    material_node(type, scope),
    ambientIntensity(0.2),
    diffuseColor(color(0.8, 0.8, 0.8)),
    emissiveColor(color(0.0, 0.0, 0.0)),
    shininess_(0.2),
    specularColor(color(0.0, 0.0, 0.0)),
    transparency_(0.0)
{}

/**
 * @brief Destructor.
 */
Material::~Material() throw () {}

/**
 * @brief set_ambientIntensity eventIn handler.
 *
 * @param value     a value from 0.0 to 1.0.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 */
void Material::processSet_ambientIntensity(const field_value & value,
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
void Material::processSet_diffuseColor(const field_value & value,
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
void Material::processSet_emissiveColor(const field_value & value,
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
void Material::processSet_shininess(const field_value & value,
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
void Material::processSet_specularColor(const field_value & value,
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
void Material::processSet_transparency(const field_value & value,
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
float Material::ambient_intensity() const throw ()
{
    return this->ambientIntensity.value;
}

/**
 * @brief Get the diffuse color.
 *
 * @return the diffuse color.
 */
const color & Material::diffuse_color() const throw ()
{
    return this->diffuseColor.value;
}

/**
 * @brief Get the emissive color.
 *
 * @return the emissive color.
 */
const color & Material::emissive_color() const throw ()
{
    return this->emissiveColor.value;
}

/**
 * @brief Get the shininess.
 *
 * @return the shininess.
 */
float Material::shininess() const throw ()
{
    return this->shininess_.value;
}

/**
 * @brief Get the specular color.
 *
 * @return the specular color.
 */
const color & Material::specular_color() const throw ()
{
    return this->specularColor.value;
}

/**
 * @brief Get the transparency.
 *
 * @return the transparency.
 */
float Material::transparency() const throw () 
{
    return this->transparency_.value;
}


/**
 * @class MovieTextureClass
 *
 * @brief Class object for MovieTexture nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
MovieTextureClass::MovieTextureClass(OpenVRML::browser & browser):
        node_class(browser) {}

/**
 * @brief Destructor.
 */
MovieTextureClass::~MovieTextureClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating MovieTexture nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by MovieTextureClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
MovieTextureClass::create_type(const std::string & id,
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
        type(new Vrml97NodeTypeImpl<MovieTexture>(*this, id));
    Vrml97NodeTypeImpl<MovieTexture> & movieTextureNodeType =
            static_cast<Vrml97NodeTypeImpl<MovieTexture> &>(*type);
    typedef Vrml97NodeTypeImpl<MovieTexture>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            movieTextureNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &MovieTexture::processSet_loop,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, sfbool>
                                    (&MovieTexture::loop)));
        } else if (*itr == supportedInterfaces[1]) {
            movieTextureNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &MovieTexture::processSet_speed,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, sffloat>
                                    (&MovieTexture::speed)));
        } else if (*itr == supportedInterfaces[2]) {
            movieTextureNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &MovieTexture::processSet_startTime,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, sftime>
                                    (&MovieTexture::startTime)));
        } else if (*itr == supportedInterfaces[3]) {
            movieTextureNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &MovieTexture::processSet_stopTime,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, sftime>
                                    (&MovieTexture::stopTime)));
        } else if (*itr == supportedInterfaces[4]) {
            movieTextureNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &MovieTexture::processSet_url,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, mfstring>
                                    (&MovieTexture::url)));
        } else if (*itr == supportedInterfaces[5]) {
            movieTextureNodeType.addField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, sfbool>
                                    (&MovieTexture::repeatS)));
        } else if (*itr == supportedInterfaces[6]) {
            movieTextureNodeType.addField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, sfbool>
                                    (&MovieTexture::repeatT)));
        } else if (*itr == supportedInterfaces[7]) {
            movieTextureNodeType.addEventOut(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, sftime>
                                    (&MovieTexture::duration)));
        } else if (*itr == supportedInterfaces[8]) {
            movieTextureNodeType.addEventOut(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<MovieTexture, sfbool>
                                    (&MovieTexture::active)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class MovieTexture
 *
 * @brief MovieTexture node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
MovieTexture::MovieTexture(const node_type & type,
                           const scope_ptr & scope):
        node(type, scope),
        AbstractTexture(type, scope),
        loop(false),
        speed(1.0),
        image(0),
        frame(0),
        lastFrame(-1),
        lastFrameTime(-1.0),
        texObject(0) {}

/**
 * @brief Destructor.
 */
MovieTexture::~MovieTexture() throw ()
{
    delete this->image;
}

MovieTexture* MovieTexture::to_movie_texture() const
{ return (MovieTexture*) this; }

void MovieTexture::update(const double currentTime)
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
        this->image = new Image;
        if (!this->image->tryURLs(this->url, &baseDoc)) {
            std::cerr << "Error: couldn't read MovieTexture from URL "
                      << this->url << std::endl;
        }

        int nFrames = this->image->nFrames();
        this->duration = sftime((nFrames >= 0) ? double(nFrames) : double(-1));
        this->emit_event("duration_changed", this->duration, currentTime);
        this->frame = (this->speed.value >= 0) ? 0 : nFrames-1;
        // Set the last frame equal to the start time.
        // This is needed to properly handle the case where the startTime
        // and stopTime are set at runtime to the same value (spec says
        // that a single loop should occur in this case...)
        this->lastFrameTime = this->startTime.value;
    }

    // No pictures to show
    if (!this->image || this->image->nFrames() == 0) { return; }

    // See section 4.6.9 of the VRML97 spec for a detailed explanation
    // of the logic here.
    if (!this->active.value) {
        if (currentTime >= this->startTime.value) {
            if (currentTime >= this->stopTime.value) {
                if (this->startTime.value >= this->stopTime.value) {
                    if (this->loop.value) {
                        this->active.value = true;
                        this->emit_event("isActive", this->active, currentTime);
                        this->lastFrameTime = currentTime;
                        this->frame = (this->speed.value >= 0) ? 0 :
                                         this->image->nFrames() - 1;
                        this->modified(true);
	            } else if (this->startTime.value > this->lastFrameTime) {
                        this->active.value = true;
                        this->emit_event("isActive", this->active, currentTime);
                        this->lastFrameTime = currentTime;
                        this->frame = (this->speed.value >= 0) ? 0 :
                                         this->image->nFrames() - 1;
                        this->modified(true);
	            }
	        }
            } else if (this->stopTime.value > currentTime) {
                this->active.value = true;
                this->emit_event("isActive", this->active, currentTime);
                this->lastFrameTime = currentTime;
                this->frame = (this->speed.value >= 0) ? 0 :
                                 this->image->nFrames() - 1;
                this->modified(true);
            }
        }
    }
    // Check whether stopTime has passed
    else if (this->active.value
             && ((this->stopTime.value > this->startTime.value
		  && this->stopTime.value <= currentTime))
             || ((this->frame < 0) && !this->loop.value)) {
        this->active.value = false;
        this->emit_event("isActive", this->active, currentTime);
        this->modified(true);
    } else if (this->frame < 0 && this->loop.value) {
        // Reset frame to 0 to begin loop again.
        this->frame = 0;
    }

    // Check whether the frame should be advanced
    else if (this->active.value
             && this->lastFrameTime + fabs(1 / this->speed.value)
                <= currentTime) {
        if (this->speed.value < 0.0) {
            --this->frame;
        } else {
            ++this->frame;
        }

        this->lastFrameTime = currentTime;
        this->modified(true);
    }

    // Tell the scene when the next update is needed.
    if (this->active.value) {
        double d = this->lastFrameTime + fabs(1 / this->speed.value)
                    - currentTime;
        this->type._class.browser.delta(0.9 * d);
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
void MovieTexture::render(OpenVRML::viewer & viewer, const rendering_context context)
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
        int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
        int nSizes = sizeof(sizes) / sizeof(int);
        int w = this->image->w();
        int h = this->image->h();
        int i, j;
        for (i = 0; i < nSizes; ++i) { if (w < sizes[i]) { break; } }
        for (j = 0; j < nSizes; ++j) { if (h < sizes[j]) { break; } }

        if (i > 0 && j > 0) {
            // Always scale images down in size and reuse the same pixel memory.
            if (w != sizes[i - 1] || h != sizes[j - 1]) {
                viewer.scale_texture(w, h, sizes[i - 1], sizes[j - 1],
                                     this->image->nc(), pix);
                this->image->setSize(sizes[i - 1], sizes[j - 1]);
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

size_t MovieTexture::nComponents() const throw () {
    return this->image ? this->image->nc() : 0;
}

size_t MovieTexture::width() const throw () {
    return this->image ? this->image->w() : 0;
}

size_t MovieTexture::height() const throw () {
    return this->image ? this->image->h() : 0;
}

size_t MovieTexture::nFrames() const throw () {
    return this->image ? this->image->nFrames() : 0;
}

const unsigned char * MovieTexture::pixels() const throw () {
    return this->image ? this->image->pixels() : 0;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MovieTexture::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_movie(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void MovieTexture::do_shutdown(const double timestamp) throw ()
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
void MovieTexture::processSet_loop(const field_value & value,
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
void MovieTexture::processSet_speed(const field_value & value,
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
void MovieTexture::processSet_startTime(const field_value & value,
                                        const double timestamp)
        throw (std::bad_cast) {
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
void MovieTexture::processSet_stopTime(const field_value & value,
                                       const double timestamp)
        throw (std::bad_cast) {
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
void MovieTexture::processSet_url(const field_value & value,
                                  const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->url = dynamic_cast<const mfstring &>(value);
    this->node::modified(true);
    this->emit_event("url_changed", this->url, timestamp);
}


/**
 * @class NavigationInfoClass
 *
 * @brief Class object for NavigationInfo nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
NavigationInfoClass::NavigationInfoClass(OpenVRML::browser & browser):
        node_class(browser) {}

/**
 * @brief Destructor.
 */
NavigationInfoClass::~NavigationInfoClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating NavigationInfo nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by NavigationInfoClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        NavigationInfoClass::create_type(const std::string & id,
                                        const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sfbool_id, "set_bind"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "avatarSize"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "headlight"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "speed"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "type"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "visibilityLimit"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isBound")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<NavigationInfo>(*this, id));
    Vrml97NodeTypeImpl<NavigationInfo> & navigationInfoNodeType =
            static_cast<Vrml97NodeTypeImpl<NavigationInfo> &>(*type);
    typedef Vrml97NodeTypeImpl<NavigationInfo>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            navigationInfoNodeType.addEventIn(supportedInterfaces[0].field_type,
                                   supportedInterfaces[0].id,
                                   &NavigationInfo::processSet_bind);
        } else if (*itr == supportedInterfaces[1]) {
            navigationInfoNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &NavigationInfo::processSet_avatarSize,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NavigationInfo, mffloat>
                                    (&NavigationInfo::avatarSize)));
        } else if (*itr == supportedInterfaces[2]) {
            navigationInfoNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &NavigationInfo::processSet_headlight,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NavigationInfo, sfbool>
                                    (&NavigationInfo::headlight)));
        } else if (*itr == supportedInterfaces[3]) {
            navigationInfoNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &NavigationInfo::processSet_speed,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NavigationInfo, sffloat>
                                    (&NavigationInfo::speed)));
        } else if (*itr == supportedInterfaces[4]) {
            navigationInfoNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &NavigationInfo::processSet_type,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NavigationInfo, mfstring>
                                    (&NavigationInfo::type)));
        } else if (*itr == supportedInterfaces[5]) {
            navigationInfoNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &NavigationInfo::processSet_visibilityLimit,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NavigationInfo, sffloat>
                                    (&NavigationInfo::visibilityLimit)));
        } else if (*itr == supportedInterfaces[6]) {
            navigationInfoNodeType.addEventOut(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NavigationInfo, sfbool>
                                    (&NavigationInfo::bound)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

namespace {
    const float avatarSize_[] = { 0.25, 1.6, 0.75 };
    const std::string type_[] = { "WALK", "ANY" };
}

/**
 * @class NavigationInfo
 *
 * @brief NavigationInfo node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
NavigationInfo::NavigationInfo(const node_type & type,
                               const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    avatarSize(avatarSize_, avatarSize_ + 3),
    headlight(true),
    speed(1.0),
    type(type_, type_ + 2),
    visibilityLimit(0.0),
    bound(false)
{}

/**
 * @brief Destructor.
 */
NavigationInfo::~NavigationInfo() throw ()
{}

NavigationInfo* NavigationInfo::to_navigation_info() const
{ return (NavigationInfo*) this; }

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void NavigationInfo::do_initialize(const double timestamp)
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
void NavigationInfo::do_shutdown(const double timestamp) throw ()
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
void NavigationInfo::processSet_avatarSize(const field_value & value,
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
void NavigationInfo::processSet_bind(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    NavigationInfo * current =
        this->node::type._class.browser.bindable_navigation_info_top();
    const sfbool & b = dynamic_cast<const sfbool &>(value);

    if (b.value) {        // set_bind TRUE
        if (this != current) {
            if (current) {
                current->bound.value = false;
                current->emit_event("isBound", current->bound, timestamp);
            }
            this->node::type._class.browser.bindable_push(this);
            this->bound.value = true;
            this->emit_event("isBound", this->bound, timestamp);
        }
    } else {            // set_bind FALSE
        this->node::type._class.browser.bindable_remove(this);
        if (this == current) {
            this->bound.value = false;
            this->emit_event("isBound", this->bound, timestamp);
            current = this->node::type._class.browser
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
void NavigationInfo::processSet_headlight(const field_value & value,
                                          const double timestamp)
    throw (std::bad_cast)
{
    this->headlight = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("headlight_changed", this->headlight, timestamp);
}

/**
 * @brief set_speed eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 */
void NavigationInfo::processSet_speed(const field_value & value,
                                      const double timestamp)
    throw (std::bad_cast)
{
    this->speed = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("speed_changed", this->speed, timestamp);
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
void NavigationInfo::processSet_type(const field_value & value,
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
void NavigationInfo::processSet_visibilityLimit(const field_value & value,
                                                const double timestamp)
    throw (std::bad_cast)
{
    this->visibilityLimit = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("visibilityLimit_changed", this->visibilityLimit,
                    timestamp);
}


/**
 * @class NormalClass
 *
 * @brief Class object for Material nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
NormalClass::NormalClass(OpenVRML::browser & browser): node_class(browser) {}

/**
 * @brief Destructor.
 */
NormalClass::~NormalClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Normal nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by NormalClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
NormalClass::create_type(const std::string & id,
                         const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "vector");
    const node_type_ptr type(new Vrml97NodeTypeImpl<Normal>(*this, id));
    Vrml97NodeTypeImpl<Normal> & normalNodeType =
            static_cast<Vrml97NodeTypeImpl<Normal> &>(*type);
    typedef Vrml97NodeTypeImpl<Normal>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterface) {
            normalNodeType.addExposedField(
                supportedInterface.field_type,
                supportedInterface.id,
                &Normal::processSet_vector,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Normal, mfvec3f>
                                    (&Normal::vector_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Normal
 *
 * @brief Normal node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
Normal::Normal(const node_type & type,
               const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    normal_node(type, scope)
{}

/**
 * @brief Destructor.
 */
Normal::~Normal() throw () {}

/**
 * @brief Get the array of normal vectors.
 *
 * @return the array of normal vectors.
 */
const std::vector<vec3f> & Normal::vector() const throw ()
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
void Normal::processSet_vector(const field_value & value,
                               const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->vector_ = dynamic_cast<const mfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("vector_changed", this->vector_, timestamp);
}


/**
 * @class NormalInterpolatorClass
 *
 * @brief Class object for NormalInterpolator nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
NormalInterpolatorClass::NormalInterpolatorClass(OpenVRML::browser & browser):
        node_class(browser) {}

/**
 * @brief Destructor.
 */
NormalInterpolatorClass::~NormalInterpolatorClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating NormalInterpolator
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by NormalInterpolatorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        NormalInterpolatorClass::create_type(const std::string & id,
                                            const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sffloat_id, "set_fraction"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "key"),
        node_interface(node_interface::exposedfield_id, field_value::mfvec3f_id, "keyValue"),
        node_interface(node_interface::eventout_id, field_value::mfvec3f_id, "value_changed")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<NormalInterpolator>(*this, id));
    Vrml97NodeTypeImpl<NormalInterpolator> & normalInterpolatorNodeType =
            static_cast<Vrml97NodeTypeImpl<NormalInterpolator> &>(*type);
    typedef Vrml97NodeTypeImpl<NormalInterpolator>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            normalInterpolatorNodeType
                    .addEventIn(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &NormalInterpolator::processSet_fraction);
        } else if (*itr == supportedInterfaces[1]) {
            normalInterpolatorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &NormalInterpolator::processSet_key,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NormalInterpolator, mffloat>
                                    (&NormalInterpolator::key)));
        } else if (*itr == supportedInterfaces[2]) {
            normalInterpolatorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &NormalInterpolator::processSet_keyValue,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NormalInterpolator, mfvec3f>
                                    (&NormalInterpolator::keyValue)));
        } else if (*itr == supportedInterfaces[3]) {
            normalInterpolatorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<NormalInterpolator, mfvec3f>
                                    (&NormalInterpolator::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class NormalInterpolator
 *
 * @brief NormalInterpolator node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
NormalInterpolator::NormalInterpolator(const node_type & type,
                                       const scope_ptr & scope):
        node(type, scope),
        AbstractChild(type, scope) {}

/**
 * @brief Destructor.
 */
NormalInterpolator::~NormalInterpolator() throw () {}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void NormalInterpolator::processSet_fraction(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    using OpenVRML_::fptolerance;

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
                std::vector<vec3f>::const_iterator v1 =
                        this->keyValue.value.begin() + i * nNormals;
                std::vector<vec3f>::const_iterator v2 =
                        this->keyValue.value.begin() + (i + 1) * nNormals;

                f = (f - this->key.value[i])
                    / (this->key.value[i + 1] - this->key.value[i]);

                // Interpolate on the surface of unit sphere.
                // Contributed by S. K. Bose. (bose@garuda.barc.ernet.in)
                for (size_t j = 0; j < nNormals; ++j) {
                    float alpha, beta;
                    float dotval = v1->dot(*v2);
                    if ((dotval + 1.0) > fptolerance) { // Vectors are not opposite
                        if ((1.0-dotval) > fptolerance) { // Vectors are not coincide
                            float omega = acos(dotval);
                            float sinomega = sin(omega);
                            alpha = sin((1.0 - f) * omega) / sinomega;
                            beta = sin(f * omega) / sinomega;
                        } else {
                            // Do linear interpolation...
                            alpha = 1.0 - f;
                            beta = f;
                        }
                    } else { // Do linear interpolation...
                        alpha = 1.0 -f;
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
void NormalInterpolator::processSet_key(const field_value & value,
                                        const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
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
void NormalInterpolator::processSet_keyValue(const field_value & value,
                                             const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    this->keyValue = dynamic_cast<const mfvec3f &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class OrientationInterpolatorClass
 *
 * @brief Class object for OrientationInterpolator nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
OrientationInterpolatorClass::OrientationInterpolatorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
OrientationInterpolatorClass::~OrientationInterpolatorClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *      OrientationInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by OrientationInterpolatorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
OrientationInterpolatorClass::create_type(const std::string & id,
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
        type(new Vrml97NodeTypeImpl<OrientationInterpolator>(*this, id));
    Vrml97NodeTypeImpl<OrientationInterpolator> &
        orientationInterpolatorNodeType =
        static_cast<Vrml97NodeTypeImpl<OrientationInterpolator> &>(*type);
    typedef Vrml97NodeTypeImpl<OrientationInterpolator>::NodeFieldPtrPtr
        NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            orientationInterpolatorNodeType
                    .addEventIn(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &OrientationInterpolator::processSet_fraction);
        } else if (*itr == supportedInterfaces[1]) {
            orientationInterpolatorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &OrientationInterpolator::processSet_key,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<OrientationInterpolator,
                                                     mffloat>
                                (&OrientationInterpolator::key)));
        } else if (*itr == supportedInterfaces[2]) {
            orientationInterpolatorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &OrientationInterpolator::processSet_keyValue,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<OrientationInterpolator,
                                                     mfrotation>
                                (&OrientationInterpolator::keyValue)));
        } else if (*itr == supportedInterfaces[3]) {
            orientationInterpolatorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<OrientationInterpolator,
                                                     sfrotation>
                                (&OrientationInterpolator::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class OrientationInterpolator
 *
 * @brief OrientationInterpolator node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
OrientationInterpolator::OrientationInterpolator(const node_type & type,
                                                 const scope_ptr & scope):
        node(type, scope),
        AbstractChild(type, scope) {}

/**
 * @brief Destructor.
 */
OrientationInterpolator::~OrientationInterpolator() throw () {}

/**
 * @brief set_fraction eventIn handler.
 *
 * @param value     an sffloat value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast     if @p value is not an sffloat.
 * @exception std::bad_alloc    if memory allocation fails.
 */
void OrientationInterpolator::processSet_fraction(const field_value & value,
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
                        r1 += 2.0 * pi;
                    } else {
                        r2 += 2.0 * pi;
                    }
                }
                float angle = r1 + f * (r2 - r1);
                if (angle >= 2.0 * pi) {
                    angle -= 2.0 * pi;
                } else if (angle < 0.0) {
                    angle += 2.0 * pi;
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
void OrientationInterpolator::processSet_key(const field_value & value,
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
void OrientationInterpolator::processSet_keyValue(const field_value & value,
                                                  const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mfrotation &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class PixelTextureClass
 *
 * @brief Class object for PixelTexture nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
PixelTextureClass::PixelTextureClass(OpenVRML::browser & browser):
        node_class(browser) {}

/**
 * @brief Destructor.
 */
PixelTextureClass::~PixelTextureClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating PixelTexture nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by PixelTextureClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
PixelTextureClass::create_type(const std::string & id,
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
    const node_type_ptr
        type(new Vrml97NodeTypeImpl<PixelTexture>(*this, id));
    Vrml97NodeTypeImpl<PixelTexture> & pixelTextureNodeType =
        static_cast<Vrml97NodeTypeImpl<PixelTexture> &>(*type);
    typedef Vrml97NodeTypeImpl<PixelTexture>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            pixelTextureNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &PixelTexture::processSet_image,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PixelTexture, sfimage>
                                    (&PixelTexture::image)));
        } else if (*itr == supportedInterfaces[1]) {
            pixelTextureNodeType.addField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PixelTexture, sfbool>
                                    (&PixelTexture::repeatS)));
        } else if (*itr == supportedInterfaces[2]) {
            pixelTextureNodeType.addField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PixelTexture, sfbool>
                                    (&PixelTexture::repeatT)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class PixelTexture
 *
 * @brief Represents PixelTexture node instances.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
PixelTexture::PixelTexture(const node_type & type,
                           const scope_ptr & scope):
        node(type, scope),
        AbstractTexture(type, scope),
        texObject(0) {}

/**
 * @brief Destructor.
 */
PixelTexture::~PixelTexture() throw ()
{
    // viewer.remove_texture_object(this->texObject); ...
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void PixelTexture::render(OpenVRML::viewer & viewer,
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
            const int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
            const int nSizes = sizeof(sizes) / sizeof(int);
            int w = this->image.x();
            int h = this->image.y();
            int i, j;
            for (i = 0; i < nSizes; ++i) { if (w < sizes[i]) { break; } }
            for (j = 0; j < nSizes; ++j) { if (h < sizes[j]) { break; } }

            if (i > 0 && j > 0) {
                // Always scale images down in size and reuse the same pixel memory.

                // What if we had multiple renderers serving the same scene, and
                // the renderers had different requirements for rescaling the
                // image? I think it would be better to keep the rescaled image
                // in a cache in the renderer.
                //   -- Braden McDaniel <braden@endoframe.com>, 9 Dec, 2000
                if (w != sizes[i - 1] || h != sizes[j - 1]) {
                    const size_t numBytes = this->image.x() * this->image.y()
                                            * this->image.comp();
                    unsigned char * pixels = new unsigned char[numBytes];
                    std::copy(this->image.array(),
                              this->image.array() + numBytes,
                              pixels);

                    viewer.scale_texture(w, h, sizes[i - 1], sizes[j - 1],
                                         this->image.comp(), pixels);
                    this->image.set(sizes[i - 1], sizes[j - 1],
                                    this->image.comp(), pixels);
                    delete [] pixels;
                }

                this->texObject =
                        viewer.insert_texture(this->image.x(),
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

size_t PixelTexture::nComponents() const throw ()
{
    return this->image.comp();
}

size_t PixelTexture::width() const throw ()
{
    return this->image.x();
}

size_t PixelTexture::height() const throw ()
{
    return this->image.y();
}

size_t PixelTexture::nFrames() const throw ()
{
    return 0;
}

const unsigned char * PixelTexture::pixels() const throw ()
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
void PixelTexture::processSet_image(const field_value & value,
                                    const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->image = dynamic_cast<const sfimage &>(value);
    this->node::modified(true);
    this->emit_event("image_changed", this->image, timestamp);
}


/**
 * @class PlaneSensorClass
 *
 * @brief Class object for PlaneSensor nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node class object.
 */
PlaneSensorClass::PlaneSensorClass(OpenVRML::browser & browser):
        node_class(browser) {}

/**
 * @brief Destructor.
 */
PlaneSensorClass::~PlaneSensorClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating PlaneSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by PlaneSensorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        PlaneSensorClass::create_type(const std::string & id,
                                     const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<PlaneSensor>(*this, id));
    Vrml97NodeTypeImpl<PlaneSensor> & planeSensorNodeType =
            static_cast<Vrml97NodeTypeImpl<PlaneSensor> &>(*type);
    typedef Vrml97NodeTypeImpl<PlaneSensor>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            planeSensorNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &PlaneSensor::processSet_autoOffset,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PlaneSensor, sfbool>
                                    (&PlaneSensor::autoOffset)));
        } else if (*itr == supportedInterfaces[1]) {
            planeSensorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &PlaneSensor::processSet_enabled,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PlaneSensor, sfbool>
                                    (&PlaneSensor::enabled)));
        } else if (*itr == supportedInterfaces[2]) {
            planeSensorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &PlaneSensor::processSet_maxPosition,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PlaneSensor, sfvec2f>
                                    (&PlaneSensor::maxPosition)));
        } else if (*itr == supportedInterfaces[3]) {
            planeSensorNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &PlaneSensor::processSet_minPosition,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PlaneSensor, sfvec2f>
                                    (&PlaneSensor::minPosition)));
        } else if (*itr == supportedInterfaces[4]) {
            planeSensorNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &PlaneSensor::processSet_offset,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PlaneSensor, sfvec3f>
                                    (&PlaneSensor::offset)));
        } else if (*itr == supportedInterfaces[5]) {
            planeSensorNodeType.addEventOut(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PlaneSensor, sfbool>
                                    (&PlaneSensor::active)));
        } else if (*itr == supportedInterfaces[6]) {
            planeSensorNodeType.addEventOut(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PlaneSensor, sfvec3f>
                                    (&PlaneSensor::trackPoint)));
        } else if (*itr == supportedInterfaces[7]) {
            planeSensorNodeType.addEventOut(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PlaneSensor, sfvec3f>
                                    (&PlaneSensor::translation)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class PlaneSensor
 *
 * The PlaneSensor node maps pointing device motion into
 * two-dimensional translation in a plane parallel to the Z=0 plane of
 * the local coordinate system. The PlaneSensor node uses the
 * descendent geometry of its parent node to determine whether it is
 * liable to generate events
 *
 * @todo need copy constructor for d_parentTransform ...
 */

/**
 * @var PlaneSensor::PlaneSensorClass
 *
 * @brief Class object for PlaneSensor instances.
 */

/**
 * @var sfbool PlaneSensor::autoOffset
 *
 * @brief autoOffset exposedField.
 */

/**
 * @var sfbool PlaneSensor::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfvec2f PlaneSensor::maxPosition
 *
 * @brief maxPosition exposedField.
 */

/**
 * @var sfvec2f PlaneSensor::minPosition
 *
 * @brief minPosition exposedField.
 */

/**
 * @var sfvec3f PlaneSensor::offset
 *
 * @brief offset exposedField.
 */

/**
 * @var sfbool PlaneSensor::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfvec3f PlaneSensor::trackPoint
 *
 * @brief trackPoint_changed eventOut.
 */

/**
 * @var sfvec3f PlaneSensor::translation
 *
 * @brief translation_changed eventOut.
 */

/**
 * @var sfvec3f PlaneSensor::activationPoint
 *
 * @brief The point at which the PlaneSensor was activated.
 */

/**
 * @var Node * PlaneSensor::parentTransform
 *
 * @brief The parent Transform.
 */

/**
 * @var mat4f PlaneSensor::activationMatrix
 */

/**
 * @var mat4f PlaneSensor::modelview
 *
 * @brief The modelview matrix.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
PlaneSensor::PlaneSensor(const node_type & type,
                         const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    autoOffset(true),
    enabled(true),
    maxPosition(vec2f(-1.0, -1.0)),
    minPosition(vec2f(0.0, 0.0)),
    offset(vec3f(0.0, 0.0, 0.0)),
    active(false)
{
    this->node::modified(true);
}

/**
 * @brief Destructor.
 */
PlaneSensor::~PlaneSensor() throw ()
{}

/**
 * @brief Cast to a PlaneSensor.
 *
 * @return a pointer to the PlaneSensor.
 */
PlaneSensor * PlaneSensor::to_plane_sensor() const
{
    return (PlaneSensor*) this;
}

/**
 * @brief Render the node.
 *
 * Render a frame if there is one available.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void PlaneSensor::render(OpenVRML::viewer & viewer, const rendering_context context)
{
    //
    // Store the ModelView matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation.
    //
    this->modelview = context.matrix();
}

/**
 * @todo This is not correct. The local coords are computed for one instance,
 * need to convert p to local coords for each instance (DEF/USE) of the
 * sensor...
 */
void PlaneSensor::activate(double timeStamp, bool isActive, double * p)
{
    // Become active
    if (isActive && !this->active.value) {
        this->active.value = isActive;

        vec3f V(p[0], p[1], p[2]);
        this->activationMatrix = this->modelview.inverse();
        V = V * this->activationMatrix;
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
        vec3f V(p[0], p[1], p[2]);
        V = V * this->activationMatrix;
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
 * @brief set_autoOffset eventIn handler.
 *
 * @param value     an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void PlaneSensor::processSet_autoOffset(const field_value & value,
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
void PlaneSensor::processSet_enabled(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->enabled = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("enabled_changed", this->enabled, timestamp);
}

/**
 * @brief set_maxPosition eventIn handler.
 *
 * @param value     an sfvec2f value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfvec2f.
 */
void PlaneSensor::processSet_maxPosition(const field_value & value,
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
void PlaneSensor::processSet_minPosition(const field_value & value,
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
void PlaneSensor::processSet_offset(const field_value & value,
                                    const double timestamp)
        throw (std::bad_cast) {
    this->offset = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("offset_changed", this->offset, timestamp);
}


/**
 * @class PointLightClass
 *
 * @brief Class object for PointLight nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this class object.
 */
PointLightClass::PointLightClass(OpenVRML::browser & browser):
        node_class(browser) {}

/**
 * @brief Destructor.
 */
PointLightClass::~PointLightClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating PointLight nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by PointLightClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
PointLightClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<PointLight>(*this, id));
    Vrml97NodeTypeImpl<PointLight> & pointLightNodeType =
            static_cast<Vrml97NodeTypeImpl<PointLight> &>(*type);
    typedef Vrml97NodeTypeImpl<PointLight>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            pointLightNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &PointLight::processSet_ambientIntensity,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointLight, sffloat>
                                    (&PointLight::ambientIntensity)));
        } else if (*itr == supportedInterfaces[1]) {
            pointLightNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &PointLight::processSet_attenuation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointLight, sfvec3f>
                                    (&PointLight::attenuation)));
        } else if (*itr == supportedInterfaces[2]) {
            pointLightNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &PointLight::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointLight, sfcolor>
                                    (&PointLight::color)));
        } else if (*itr == supportedInterfaces[3]) {
            pointLightNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &PointLight::processSet_intensity,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointLight, sffloat>
                                    (&PointLight::intensity)));
        } else if (*itr == supportedInterfaces[4]) {
            pointLightNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &PointLight::processSet_location,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointLight, sfvec3f>
                                    (&PointLight::location)));
        } else if (*itr == supportedInterfaces[5]) {
            pointLightNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &PointLight::processSet_on,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointLight, sfbool>
                                    (&PointLight::on)));
        } else if (*itr == supportedInterfaces[6]) {
            pointLightNodeType.addExposedField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &PointLight::processSet_radius,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointLight, sffloat>
                                    (&PointLight::radius)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class PointLight
 *
 * @brief PointLight node instances.
 */

/**
 * @var PointLight::PointLightClass
 *
 * @brief Class object for PointLight instances.
 */

/**
 * @var sfvec3f PointLight::attenuation
 *
 * @brief attenuation exposedField.
 */

/**
 * @var sfvec3f PointLight::location
 *
 * @brief location exposedField.
 */

/**
 * @var sffloat PointLight::radius
 *
 * @brief radius exposedField.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
PointLight::PointLight(const node_type & type,
                       const scope_ptr & scope):
    node(type, scope),
    AbstractLight(type, scope),
    attenuation(vec3f(1.0, 0.0, 0.0)),
    location(vec3f(0.0, 0.0, 0.0)),
    radius(100)
{}

/**
 * @brief Destructor.
 */
PointLight::~PointLight() throw ()
{}

/**
 * @brief Cast to a PointLight.
 *
 * @return a pointer to the PointLight.
 */
PointLight* PointLight::to_point_light() const
{
    return (PointLight*) this;
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
 * @param viewer    a Viewer.
 */
void PointLight::renderScoped(OpenVRML::viewer & viewer)
{
    if (this->on.value && this->radius.value > 0.0) {
        viewer.insert_point_light(this->ambientIntensity.value,
                                  this->attenuation.value,
                                  this->color.value,
                                  this->intensity.value,
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
void PointLight::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_scoped_light(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void PointLight::do_shutdown(const double timestamp) throw ()
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
void PointLight::processSet_attenuation(const field_value & value,
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
void PointLight::processSet_location(const field_value & value,
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
void PointLight::processSet_radius(const field_value & value,
                                   const double timestamp)
    throw (std::bad_cast)
{
    this->radius = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("radius_changed", this->radius, timestamp);
}


/**
 * @class PointSetClass
 *
 * @brief Class object for @link PointSet PointSets@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
PointSetClass::PointSetClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
PointSetClass::~PointSetClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating PointSet nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by PointSetClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr PointSetClass::create_type(const std::string & id,
                                            const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "coord")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<PointSet>(*this, id));
    Vrml97NodeTypeImpl<PointSet> & pointSetNodeType =
            static_cast<Vrml97NodeTypeImpl<PointSet> &>(*type);
    typedef Vrml97NodeTypeImpl<PointSet>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            pointSetNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &PointSet::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointSet, sfnode>
                                    (&PointSet::color)));
        } else if (*itr == supportedInterfaces[1]) {
            pointSetNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &PointSet::processSet_coord,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PointSet, sfnode>
                                    (&PointSet::coord)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class PointSet
 *
 * @brief Represents PointSet node instances.
 */

/**
 * @var PointSet::PointSetClass
 *
 * @brief Class object for PointSet instances.
 */

/**
 * @var sfnode PointSet::color
 *
 * @brief color exposedField.
 */

/**
 * @var sfnode PointSet::coord
 *
 * @brief coord exposedField.
 */

/**
 * @var bounding_sphere PointSet::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
PointSet::PointSet(const node_type & type,
                   const scope_ptr & scope):
    node(type, scope),
    AbstractGeometry(type, scope)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destructor.
 */
PointSet::~PointSet() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool PointSet::modified() const
{
    return (modified_
            || (this->color.value && this->color.value->modified())
            || (this->coord.value && this->coord.value->modified()));
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void PointSet::update_modified(node_path & path, int flags)
{
    if (this->modified()) { mark_path_modified(path, true); }
    path.push_front(this);
    if (this->color.value) { this->color.value->update_modified(path); }
    if (this->coord.value) { this->coord.value->update_modified(path); }
    path.pop_front();
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t PointSet::insert_geometry(OpenVRML::viewer & viewer,
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

    coordinate_node * const coordinateNode = this->coord.value
                                          ? this->coord.value->to_coordinate()
                                          : 0;
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    color_node * const colorNode = this->color.value
                                 ? this->color.value->to_color()
                                 : 0;
    const vector<OpenVRML::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<OpenVRML::color>();

    viewer::object_t obj = viewer.insert_point_set(coord, color);

    if (this->color.value) { this->color.value->modified(false); }
    if (this->coord.value) { this->coord.value->modified(false); }

    return obj;
}

/**
 * @brief Recalculate the bounding volume.
 */
void PointSet::recalcBSphere()
{
    this->bsphere.reset();
    coordinate_node * const coordinateNode = this->coord.value
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
const bounding_volume & PointSet::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<PointSet *>(this)->recalcBSphere();
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
void PointSet::processSet_color(const field_value & value,
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
void PointSet::processSet_coord(const field_value & value,
                                const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->coord = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("coord_changed", this->coord, timestamp);
}


/**
 * @class PositionInterpolatorClass
 *
 * @brief Class object for @link PositionInterpolator PositionInterpolators@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
PositionInterpolatorClass::PositionInterpolatorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
PositionInterpolatorClass::~PositionInterpolatorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *      CoordinateInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by CoordinateInterpolatorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
PositionInterpolatorClass::create_type(const std::string & id,
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
        type(new Vrml97NodeTypeImpl<PositionInterpolator>(*this, id));
    Vrml97NodeTypeImpl<PositionInterpolator> & positionInterpolatorNodeType =
            static_cast<Vrml97NodeTypeImpl<PositionInterpolator> &>(*type);
    typedef Vrml97NodeTypeImpl<PositionInterpolator>::NodeFieldPtrPtr
        NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            positionInterpolatorNodeType
                    .addEventIn(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &PositionInterpolator::processSet_fraction);
        } else if (*itr == supportedInterfaces[1]) {
            positionInterpolatorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &PositionInterpolator::processSet_key,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PositionInterpolator, mffloat>
                                    (&PositionInterpolator::key)));
        } else if (*itr == supportedInterfaces[2]) {
            positionInterpolatorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &PositionInterpolator::processSet_keyValue,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PositionInterpolator, mfvec3f>
                                    (&PositionInterpolator::keyValue)));
        } else if (*itr == supportedInterfaces[3]) {
            positionInterpolatorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<PositionInterpolator, sfvec3f>
                                    (&PositionInterpolator::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class PositionInterpolator
 *
 * @brief PositionInterpolator node instances.
 */

/**
 * @var PositionInterpolator::PositionInterpolatorClass
 *
 * @brief Class object for PositionInterpolator instances.
 */

/**
 * @var mffloat PositionInterpolator::key
 *
 * @brief key exposedField.
 */

/**
 * @var mfvec3f PositionInterpolator::keyValue
 *
 * @brief keyValue exposedField.
 */

/**
 * @var sfvec3f PositionInterpolator::value
 *
 * @brief value_changed eventOut.
 */


/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
PositionInterpolator::PositionInterpolator(const node_type & type,
                                           const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope)
{}

/**
 * @brief Destructor.
 */
PositionInterpolator::~PositionInterpolator() throw ()
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
void PositionInterpolator::processSet_fraction(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    float f = dynamic_cast<const sffloat &>(value).value;

    int n = this->key.value.size() - 1;
    if (f < this->key.value[0]) {
        this->value.value = this->keyValue.value[0];
    } else if (f > this->key.value[n]) {
        this->value.value = this->keyValue.value[n];
    } else {
        // should cache the last index used...
        for (int i = 0; i < n; ++i) {
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
void PositionInterpolator::processSet_key(const field_value & value,
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
void PositionInterpolator::processSet_keyValue(const field_value & value,
                                               const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mfvec3f &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class ProximitySensorClass
 *
 * @brief Class object for @link ProximitySensor ProximitySensors@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
ProximitySensorClass::ProximitySensorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
ProximitySensorClass::~ProximitySensorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating ProximitySensor
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by ProximitySensorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
ProximitySensorClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<ProximitySensor>(*this, id));
    Vrml97NodeTypeImpl<ProximitySensor> & proximitySensorNodeType =
            static_cast<Vrml97NodeTypeImpl<ProximitySensor> &>(*type);
    typedef Vrml97NodeTypeImpl<ProximitySensor>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            proximitySensorNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &ProximitySensor::processSet_center,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ProximitySensor, sfvec3f>
                                    (&ProximitySensor::center)));
        } else if (*itr == supportedInterfaces[1]) {
            proximitySensorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &ProximitySensor::processSet_size,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ProximitySensor, sfvec3f>
                                    (&ProximitySensor::size)));
        } else if (*itr == supportedInterfaces[2]) {
            proximitySensorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &ProximitySensor::processSet_enabled,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ProximitySensor, sfbool>
                                    (&ProximitySensor::enabled)));
        } else if (*itr == supportedInterfaces[3]) {
            proximitySensorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ProximitySensor, sfbool>
                                    (&ProximitySensor::active)));
        } else if (*itr == supportedInterfaces[4]) {
            proximitySensorNodeType.addEventOut(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ProximitySensor, sfvec3f>
                                    (&ProximitySensor::position)));
        } else if (*itr == supportedInterfaces[5]) {
            proximitySensorNodeType.addEventOut(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ProximitySensor, sfrotation>
                                    (&ProximitySensor::orientation)));
        } else if (*itr == supportedInterfaces[6]) {
            proximitySensorNodeType.addEventOut(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ProximitySensor, sftime>
                                    (&ProximitySensor::enterTime)));
        } else if (*itr == supportedInterfaces[7]) {
            proximitySensorNodeType.addEventOut(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ProximitySensor, sftime>
                                    (&ProximitySensor::exitTime)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class ProximitySensor
 *
 * @brief Represents ProximitySensor node instances.
 */

/**
 * @var ProximitySensor::ProximitySensorClass
 *
 * @brief Class object for ProximitySensor instances.
 */

/**
 * @var sfvec3f ProximitySensor::center
 *
 * @brief center exposedField.
 */

/**
 * @var sfbool ProximitySensor::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfvec3f ProximitySensor::size
 *
 * @brief size exposedField.
 */

/**
 * @var sfbool ProximitySensor::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfvec3f ProximitySensor::position
 *
 * @brief position_changed eventOut.
 */

/**
 * @var sfvec3f ProximitySensor::orientation
 *
 * @brief orientation_changed eventOut.
 */

/**
 * @var sftime ProximitySensor::enterTime
 *
 * @brief enterTime eventOut.
 */

/**
 * @var sftime ProximitySensor::exitTime
 *
 * @brief exitTime eventOut.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
ProximitySensor::ProximitySensor(const node_type & type,
                                 const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
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
 * @brief Destructor.
 */
ProximitySensor::~ProximitySensor() throw ()
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
void ProximitySensor::render(OpenVRML::viewer & viewer,
                             const rendering_context context)
{
    using OpenVRML_::fpequal;

    if (this->enabled.value
            && this->size.value.x() > 0.0
            && this->size.value.y() > 0.0
            && this->size.value.z() > 0.0
            && viewer.mode() == viewer::draw_mode) {
        sftime timeNow(browser::current_time());
        float x, y, z;

        // Is viewer inside the box?
        mat4f MV = context.matrix();
        MV = MV.inverse();
        x = MV[3][0]; y = MV[3][1]; z = MV[3][2];
        bool inside = (fabs(x - this->center.value.x())
                            <= 0.5 * this->size.value.x()
                        && fabs(y - this->center.value.y())
                            <= 0.5 * this->size.value.y()
                        && fabs(z - this->center.value.z())
                            <= 0.5 * this->size.value.z());
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
void ProximitySensor::processSet_center(const field_value & value,
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
void ProximitySensor::processSet_size(const field_value & value,
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
void ProximitySensor::processSet_enabled(const field_value & value,
                                         double timestamp)
    throw (std::bad_cast)
{
    this->enabled = dynamic_cast<const sfbool &>(value);
    this->node::modified(true);
    this->emit_event("enabled_changed", this->enabled, timestamp);
}


/**
 * @class ScalarInterpolatorClass
 *
 * @brief Class object for @link ScalarInterpolator
 *        ScalarInterpolators@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
ScalarInterpolatorClass::ScalarInterpolatorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
ScalarInterpolatorClass::~ScalarInterpolatorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating
 *      CoordinateInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by CoordinateInterpolatorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
ScalarInterpolatorClass::create_type(const std::string & id,
                                    const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sffloat_id, "set_fraction"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "key"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "keyValue"),
        node_interface(node_interface::eventout_id, field_value::sffloat_id, "value_changed")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<ScalarInterpolator>(*this, id));
    Vrml97NodeTypeImpl<ScalarInterpolator> & scalarInterpolatorNodeType =
            static_cast<Vrml97NodeTypeImpl<ScalarInterpolator> &>(*type);
    typedef Vrml97NodeTypeImpl<ScalarInterpolator>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            scalarInterpolatorNodeType
                    .addEventIn(supportedInterfaces[0].field_type,
                                supportedInterfaces[0].id,
                                &ScalarInterpolator::processSet_fraction);
        } else if (*itr == supportedInterfaces[1]) {
            scalarInterpolatorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &ScalarInterpolator::processSet_key,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ScalarInterpolator, mffloat>
                                    (&ScalarInterpolator::key)));
        } else if (*itr == supportedInterfaces[2]) {
            scalarInterpolatorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &ScalarInterpolator::processSet_keyValue,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ScalarInterpolator, mffloat>
                                    (&ScalarInterpolator::keyValue)));
        } else if (*itr == supportedInterfaces[3]) {
            scalarInterpolatorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<ScalarInterpolator, sffloat>
                                    (&ScalarInterpolator::value)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class ScalarInterpolator
 *
 * @brief ScalarInterpolator node instances.
 */

/**
 * @var ScalarInterpolator::ScalarInterpolatorClass
 *
 * @brief Class object for ScalarInterpolator instances.
 */

/**
 * @var mffloat ScalarInterpolator::key
 *
 * @brief key exposedField.
 */

/**
 * @var mffloat ScalarInterpolator::keyValue
 *
 * @brief keyValue exposedField.
 */

/**
 * @var sffloat ScalarInterpolator::value
 *
 * @brief value_changed eventOut.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
ScalarInterpolator::ScalarInterpolator(const node_type & type,
                                       const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope)
{}

/**
 * @brief Destructor.
 */
ScalarInterpolator::~ScalarInterpolator() throw ()
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
void ScalarInterpolator::processSet_fraction(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    float f = dynamic_cast<const sffloat &>(value).value;

    int n = this->key.value.size() - 1;
    if (f < this->key.value[0]) {
        this->value.value = this->keyValue.value[0];
    } else if (f > this->key.value[n]) {
        this->value.value = this->keyValue.value[n];
    } else {
        for (int i=0; i<n; ++i) {
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
void ScalarInterpolator::processSet_key(const field_value & value,
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
void ScalarInterpolator::processSet_keyValue(const field_value & value,
                                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->keyValue = dynamic_cast<const mffloat &>(value);
    this->emit_event("keyValue_changed", this->keyValue, timestamp);
}


/**
 * @class ShapeClass
 *
 * @brief Class object for @link Shape Shape@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
ShapeClass::ShapeClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
ShapeClass::~ShapeClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Shape nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by ShapeClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr ShapeClass::create_type(const std::string & id,
                                         const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "appearance"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "geometry")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Shape>(*this, id));
    Vrml97NodeTypeImpl<Shape> & shapeNodeType =
            static_cast<Vrml97NodeTypeImpl<Shape> &>(*type);
    typedef Vrml97NodeTypeImpl<Shape>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            shapeNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &Shape::processSet_appearance,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Shape, sfnode>
                                    (&Shape::appearance)));
        } else if (*itr == supportedInterfaces[1]) {
            shapeNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Shape::processSet_geometry,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Shape, sfnode>
                                    (&Shape::geometry)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Shape
 *
 * @brief Represents Shape node instances.
 */

/**
 * @var Shape::ShapeClass
 *
 * @brief Class object for Shape instances.
 */

/**
 * @var sfnode Shape::appearance
 *
 * @brief appearance exposedField.
 */

/**
 * @var sfnode Shape::geometry
 *
 * @brief geometry exposedField.
 */

/**
 * @var viewer::object_t Shape::viewerObject
 *
 * @brief A reference to the node's previously used rendering data.
 *
 * If supported by the Viewer implementation, this member holds a reference
 * to the node's rendering data once the node has already been rendered once.
 * The intent is to capitalize on USE references in the VRML scene graph.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
Shape::Shape(const node_type & type,
             const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    viewerObject(0)
{}

/**
 * @brief Destructor.
 */
Shape::~Shape() throw ()
{
    // need viewer to free viewerObject ...
}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool Shape::modified() const
{
    return (modified_
            || (this->geometry.value && this->geometry.value->modified())
            || (this->appearance.value && this->appearance.value->modified()));
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void Shape::update_modified(node_path & path, int flags)
{
    if (this->modified()) { mark_path_modified(path, true, flags); }
    path.push_front(this);
    if (this->appearance.value) {
        this->appearance.value->update_modified(path, flags);
    }
    if (this->geometry.value) {
        this->geometry.value->update_modified(path, flags);
    }
    path.pop_front();
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void Shape::render(OpenVRML::viewer & viewer, const rendering_context context)
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
            int nTexComponents = 0;

            if (!picking && this->appearance.value
                    && this->appearance.value->to_appearance()) {
                appearance_node * a = this->appearance.value->to_appearance();
                a->render(viewer, context);

                if (a->texture() && a->texture()->to_texture()) {
                    nTexComponents = a->texture()->to_texture()->nComponents();
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
const bounding_volume & Shape::bounding_volume() const
{
    //
    // just pass off to the geometry's getbvolume() method
    //
    const OpenVRML::bounding_volume & result =
        this->geometry.value
        ? this->geometry.value->bounding_volume()
        : this->node::bounding_volume();
    const_cast<Shape *>(this)->bounding_volume_dirty(false);
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
void Shape::processSet_appearance(const field_value & value,
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
void Shape::processSet_geometry(const field_value & value,
                                const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->geometry = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("geometry_changed", this->geometry, timestamp);
}


/**
 * @class SoundClass
 *
 * @brief Class object for @link Sound Sound@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
SoundClass::SoundClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
SoundClass::~SoundClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Sound nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by SoundClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr SoundClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<Sound>(*this, id));
    Vrml97NodeTypeImpl<Sound> & soundNodeType =
            static_cast<Vrml97NodeTypeImpl<Sound> &>(*type);
    typedef Vrml97NodeTypeImpl<Sound>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            soundNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &Sound::processSet_direction,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sfvec3f>
                                    (&Sound::direction)));
        } else if (*itr == supportedInterfaces[1]) {
            soundNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Sound::processSet_intensity,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sffloat>
                                    (&Sound::intensity)));
        } else if (*itr == supportedInterfaces[2]) {
            soundNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Sound::processSet_location,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sfvec3f>
                                    (&Sound::location)));
        } else if (*itr == supportedInterfaces[3]) {
            soundNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Sound::processSet_maxBack,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sffloat>
                                    (&Sound::maxBack)));
        } else if (*itr == supportedInterfaces[4]) {
            soundNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &Sound::processSet_maxFront,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sffloat>
                                    (&Sound::maxFront)));
        } else if (*itr == supportedInterfaces[5]) {
            soundNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &Sound::processSet_minBack,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sffloat>
                                    (&Sound::minBack)));
        } else if (*itr == supportedInterfaces[6]) {
            soundNodeType.addExposedField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &Sound::processSet_minFront,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sffloat>
                                    (&Sound::minFront)));
        } else if (*itr == supportedInterfaces[7]) {
            soundNodeType.addExposedField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &Sound::processSet_priority,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sffloat>
                                    (&Sound::priority)));
        } else if (*itr == supportedInterfaces[8]) {
            soundNodeType.addExposedField(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                &Sound::processSet_source,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sfnode>
                                    (&Sound::source)));
        } else if (*itr == supportedInterfaces[9]) {
            soundNodeType.addField(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sound, sfbool>
                                    (&Sound::spatialize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Sound
 *
 * @brief Represents Sound node instances.
 */

/**
 * @var Sound::SoundClass
 *
 * @brief Class object for Sound instances.
 */

/**
 * @var sfvec3f Sound::direction
 *
 * @brief direction exposedField.
 */

/**
 * @var sffloat Sound::intensity
 *
 * @brief intensity exposedField.
 */

/**
 * @var sfvec3f Sound::location
 *
 * @brief location exposedField.
 */

/**
 * @var sffloat Sound::maxBack
 *
 * @brief maxBack exposedField.
 */

/**
 * @var sffloat Sound::maxFront
 *
 * @brief maxFront exposedField.
 */

/**
 * @var sffloat Sound::minBack
 *
 * @brief minBack exposedField.
 */

/**
 * @var sffloat Sound::minFront
 *
 * @brief minFront exposedField.
 */

/**
 * @var sffloat Sound::priority
 *
 * @brief priority exposedField.
 */

/**
 * @var sfnode Sound::source
 *
 * @brief source exposedField.
 */

/**
 * @var sfbool Sound::spatialize
 *
 * @brief spatialize field.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the instance.
 * @param scope     the scope associated with the instance.
 */
Sound::Sound(const node_type & type,
             const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    direction(vec3f(0, 0, 1)),
    intensity(1),
    maxBack(10),
    maxFront(10),
    minBack(1),
    minFront(1),
    spatialize(true)
{}

/**
 * @brief Destructor.
 */
Sound::~Sound() throw ()
{}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void Sound::update_modified(node_path & path, int flags)
{
    if (this->modified()) { mark_path_modified(path, true); }
    path.push_front(this);
    if (this->source.value) { this->source.value->update_modified(path); }
    path.pop_front();
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void Sound::render(OpenVRML::viewer & viewer, const rendering_context context)
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
void Sound::processSet_direction(const field_value & value,
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
void Sound::processSet_intensity(const field_value & value,
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
void Sound::processSet_location(const field_value & value,
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
void Sound::processSet_maxBack(const field_value & value,
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
void Sound::processSet_maxFront(const field_value & value,
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
void Sound::processSet_minBack(const field_value & value,
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
void Sound::processSet_minFront(const field_value & value,
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
void Sound::processSet_priority(const field_value & value,
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
void Sound::processSet_source(const field_value & value, double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->source = dynamic_cast<const sfnode &>(value);
    this->node::modified(true);
    this->emit_event("source_changed", this->source, timestamp);
}


/**
 * @class SphereClass
 *
 * @brief Class object for @link Sphere Sphere@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
SphereClass::SphereClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
SphereClass::~SphereClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Sphere nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by SphereClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr SphereClass::create_type(const std::string & id,
                                          const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
            node_interface(node_interface::field_id, field_value::sffloat_id, "radius");
    const node_type_ptr type(new Vrml97NodeTypeImpl<Sphere>(*this, id));
    Vrml97NodeTypeImpl<Sphere> & spereNodeType =
            static_cast<Vrml97NodeTypeImpl<Sphere> &>(*type);
    typedef Vrml97NodeTypeImpl<Sphere>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterface) {
            spereNodeType.addField(
                supportedInterface.field_type,
                supportedInterface.id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Sphere, sffloat>
                                    (&Sphere::radius)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Sphere
 *
 * @brief Sphere node instances.
 */

/**
 * @var Sphere::SphereClass
 *
 * @brief Class object for Sphere instances.
 */

/**
 * @var sffloat Sphere::radius
 *
 * @brief radius field.
 */

/**
 * @var bounding_sphere Sphere::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Sphere::Sphere(const node_type & type,
               const scope_ptr & scope):
    node(type, scope),
    AbstractGeometry(type, scope),
    radius(1.0)
{
    this->bounding_volume_dirty(true); // lazy calc of bvolumes
}

/**
 * @brief Destructor.
 */
Sphere::~Sphere() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t Sphere::insert_geometry(OpenVRML::viewer & viewer,
                                         const rendering_context context)
{
    return viewer.insert_sphere(this->radius.value);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & Sphere::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<Sphere *>(this)->bsphere.radius(this->radius.value);
        const_cast<Sphere *>(this)->bounding_volume_dirty(false); // logical const
    }
    return this->bsphere;
}


/**
 * @class SphereSensorClass
 *
 * @brief Class object for @link SphereSensor SphereSensor@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
SphereSensorClass::SphereSensorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
SphereSensorClass::~SphereSensorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating SphereSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by SphereSensorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
SphereSensorClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<SphereSensor>(*this, id));
    Vrml97NodeTypeImpl<SphereSensor> & sphereSensorNodeType =
            static_cast<Vrml97NodeTypeImpl<SphereSensor> &>(*type);
    typedef Vrml97NodeTypeImpl<SphereSensor>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            sphereSensorNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &SphereSensor::processSet_autoOffset,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SphereSensor, sfbool>
                                    (&SphereSensor::autoOffset)));
        } else if (*itr == supportedInterfaces[1]) {
            sphereSensorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &SphereSensor::processSet_enabled,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SphereSensor, sfbool>
                                    (&SphereSensor::enabled)));
        } else if (*itr == supportedInterfaces[2]) {
            sphereSensorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &SphereSensor::processSet_offset,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SphereSensor, sfrotation>
                                    (&SphereSensor::offset)));
        } else if (*itr == supportedInterfaces[3]) {
            sphereSensorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SphereSensor, sfbool>
                                    (&SphereSensor::active)));
        } else if (*itr == supportedInterfaces[4]) {
            sphereSensorNodeType.addEventOut(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SphereSensor, sfrotation>
                                    (&SphereSensor::rotation)));
        } else if (*itr == supportedInterfaces[5]) {
            sphereSensorNodeType.addEventOut(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SphereSensor, sfvec3f>
                                    (&SphereSensor::trackPoint)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class SphereSensor
 *
 * @brief SphereSensor node instances.
 */

/**
 * @var SphereSensor::SphereSensorClass
 *
 * @brief Class object for SphereSensor instances.
 */

/**
 * @var sfbool SphereSensor::autoOffset
 *
 * @brief autoOffset exposedField.
 */

/**
 * @var sfbool SphereSensor::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfrotation SphereSensor::offset
 *
 * @brief offset exposedField.
 */

/**
 * @var sfbool SphereSensor::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfrotation SphereSensor::rotation
 *
 * @brief rotation_changed eventOut.
 */

/**
 * @var sfvec3f SphereSensor::trackPoint
 *
 * @brief trackPoint_changed eventOut.
 */

/**
 * @var sfvec3f SphereSensor::activationPoint
 *
 * @brief The start point of a drag operation.
 */

/**
 * @var sfvec3f SphereSensor::centerPoint
 *
 * @brief Center point.
 */

/**
 * @var mat4f SphereSensor::modelview
 *
 * @brief Modelview matrix.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
SphereSensor::SphereSensor(const node_type & type,
                           const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    autoOffset(true),
    enabled(true),
    offset(OpenVRML::rotation(0.0, 1.0, 0.0, 0.0)),
    active(false)
{
    this->node::modified(true);
}

/**
 * @brief Destructor.
 */
SphereSensor::~SphereSensor() throw ()
{}

/**
 * @brief Cast to a SphereSensor.
 *
 * @return a pointer to the node.
 */
SphereSensor * SphereSensor::to_sphere_sensor() const
{
    return const_cast<SphereSensor *>(this);
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void SphereSensor::render(OpenVRML::viewer & viewer, const rendering_context context)
{
    //
    // Store the ModelView matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation
    //
    this->modelview = context.matrix();
}

/**
 * @brief Activate or deactivate the SphereSensor.
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
void SphereSensor::activate(double timeStamp, bool isActive, double * p)
{
    // Become active
    if (isActive && !this->active.value) {
        this->active.value = isActive;

        // set activation point in world coords
        const vec3f floatVec(p[0], p[1], p[2]);
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
        vec3f V(p[0], p[1], p[2]);
        mat4f M = this->modelview.inverse();
        V = V * M;
        this->trackPoint.value = V;
        this->emit_event("trackPoint_changed", this->trackPoint, timeStamp);

        vec3f V2(p[0], p[1], p[2]);
        vec3f tempv = V2 - this->centerPoint.value;
        vec3f dir1(tempv);
        double dist = dir1.length();                // get the length of the pre-normalized vector
        dir1 = dir1.normalize();
        tempv = this->activationPoint.value - this->centerPoint.value;
        vec3f dir2(tempv);
        dir2 = dir2.normalize();

        tempv = dir1 * dir2;
        vec3f cx(tempv);
        cx = cx.normalize();

        OpenVRML::rotation newRot(cx, dist * acos(dir1.dot(dir2)));
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
bool SphereSensor::isEnabled() const throw ()
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
void SphereSensor::processSet_autoOffset(const field_value & value,
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
void SphereSensor::processSet_enabled(const field_value & value,
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
void SphereSensor::processSet_offset(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->offset = dynamic_cast<const sfrotation &>(value);
    this->emit_event("offset_changed", this->offset, timestamp);
}


/**
 * @class SpotLightClass
 *
 * @brief Class object for @link SpotLight SpotLight@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
SpotLightClass::SpotLightClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
SpotLightClass::~SpotLightClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating PointLight nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by PointLightClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
SpotLightClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<SpotLight>(*this, id));
    Vrml97NodeTypeImpl<SpotLight> & spotLightNodeType =
            static_cast<Vrml97NodeTypeImpl<SpotLight> &>(*type);
    typedef Vrml97NodeTypeImpl<SpotLight>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &SpotLight::processSet_ambientIntensity,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sffloat>
                                    (&SpotLight::ambientIntensity)));
        } else if (*itr == supportedInterfaces[1]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &SpotLight::processSet_attenuation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sfvec3f>
                                    (&SpotLight::attenuation)));
        } else if (*itr == supportedInterfaces[2]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &SpotLight::processSet_beamWidth,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sffloat>
                                    (&SpotLight::beamWidth)));
        } else if (*itr == supportedInterfaces[3]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &SpotLight::processSet_color,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sfcolor>
                                    (&SpotLight::color)));
        } else if (*itr == supportedInterfaces[4]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &SpotLight::processSet_cutOffAngle,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sffloat>
                                    (&SpotLight::cutOffAngle)));
        } else if (*itr == supportedInterfaces[5]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &SpotLight::processSet_direction,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sfvec3f>
                                    (&SpotLight::direction)));
        } else if (*itr == supportedInterfaces[6]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &SpotLight::processSet_intensity,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sffloat>
                                    (&SpotLight::intensity)));
        } else if (*itr == supportedInterfaces[7]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &SpotLight::processSet_location,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sfvec3f>
                                    (&SpotLight::location)));
        } else if (*itr == supportedInterfaces[8]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                &SpotLight::processSet_on,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sfbool>
                                    (&SpotLight::on)));
        } else if (*itr == supportedInterfaces[9]) {
            spotLightNodeType.addExposedField(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                &SpotLight::processSet_radius,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<SpotLight, sffloat>
                                    (&SpotLight::radius)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class SpotLight
 *
 * @brief SpotLight node instances.
 */

/**
 * @var SpotLight::SpotLightClass
 *
 * @brief Class object for SpotLight instances.
 */

/**
 * @var sfvec3f SpotLight::attenuation
 *
 * @brief attenuation exposedField.
 */

/**
 * @var sffloat SpotLight::beamWidth
 *
 * @brief beamWidth exposedField.
 */

/**
 * @var sffloat SpotLight::cutOffAngle
 *
 * @brief cutOffAngle exposedField.
 */

/**
 * @var sfvec3f SpotLight::direction
 *
 * @brief direction exposedField.
 */

/**
 * @var sfvec3f SpotLight::location
 *
 * @brief location exposedField.
 */

/**
 * @var sffloat SpotLight::radius
 *
 * @brief radius exposedField.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
SpotLight::SpotLight(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    AbstractLight(type, scope),
    attenuation(vec3f(1.0, 0.0, 0.0)),
    beamWidth(1.570796),
    cutOffAngle(0.785398),
    direction(vec3f(0.0, 0.0, -1.0)),
    location(vec3f(0.0, 0.0, 0.0)),
    radius(100)
{}

/**
 * @brief Destructor.
 */
SpotLight::~SpotLight() throw ()
{}

/**
 * @brief Cast to a SpotLight.
 *
 * @return a pointer to the node.
 */
SpotLight * SpotLight::to_spot_light() const
{
    return const_cast<SpotLight *>(this);
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
void SpotLight::renderScoped(OpenVRML::viewer & viewer)
{
    if (this->on.value && this->radius.value > 0.0) {
        viewer.insert_spot_light(this->ambientIntensity.value,
                                 this->attenuation.value,
                                 this->beamWidth.value,
                                 this->color.value,
                                 this->cutOffAngle.value,
                                 this->direction.value,
                                 this->intensity.value,
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
void SpotLight::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_scoped_light(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void SpotLight::do_shutdown(const double timestamp) throw ()
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
void SpotLight::processSet_attenuation(const field_value & value,
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
void SpotLight::processSet_beamWidth(const field_value & value,
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
void SpotLight::processSet_cutOffAngle(const field_value & value,
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
void SpotLight::processSet_direction(const field_value & value,
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
void SpotLight::processSet_location(const field_value & value,
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
void SpotLight::processSet_radius(const field_value & value,
                                  const double timestamp)
    throw (std::bad_cast)
{
    this->radius = dynamic_cast<const sffloat &>(value);
    this->node::modified(true);
    this->emit_event("radius_changed", this->radius, timestamp);
}


/**
 * @class SwitchClass
 *
 * @brief Class object for @link Switch Switch@endlink.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
SwitchClass::SwitchClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
SwitchClass::~SwitchClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Switch nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by SwitchClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr SwitchClass::create_type(const std::string & id,
                                          const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                      field_value::mfnode_id,
                      "choice"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sfint32_id,
                      "whichChoice")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Switch>(*this, id));
    Vrml97NodeTypeImpl<Switch> & switchNodeType =
            static_cast<Vrml97NodeTypeImpl<Switch> &>(*type);
    typedef Vrml97NodeTypeImpl<Switch>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            switchNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &Switch::processSet_choice,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Switch, mfnode>
                                    (&Switch::choice)));
        } else if (*itr == supportedInterfaces[1]) {
            switchNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Switch::processSet_whichChoice,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Switch, sfint32>
                                    (&Switch::whichChoice)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Switch
 *
 * @brief Switch node instances.
 */

/**
 * @var Switch::SwitchClass
 *
 * @brief Class object for Switch instances.
 */

/**
 * @var mfnode Switch::choice
 *
 * @brief choice exposedField.
 */

/**
 * @var sfint32 Switch::whichChoice
 *
 * @brief whichChoice exposedField.
 */

/**
 * @var mfnode Switch::children
 *
 * @brief The children currently in the scene graph.
 */

/**
 * @var bounding_sphere Switch::bsphere
 *
 * @brief Cached copy of the bsphere enclosing this node's children.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
Switch::Switch(const node_type & type,
               const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    whichChoice(-1),
    children_(1)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destructor.
 */
Switch::~Switch() throw () {}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool Switch::modified() const {
    if (modified_) { return true; }

    long w = this->whichChoice.value;

    return (w >= 0 && size_t(w) < this->choice.value.size()
            && this->choice.value[w]->modified());
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void Switch::update_modified(node_path & path, int flags) {
    //
    // ok: again we get this issue of whether to check _all_ the children
    // or just the current choice (ref LOD). again, chooise to test them
    // all. note that the original modified() just tested the current
    // one. keep that in mind, and change it back when confirmed safe.
    //
    if (this->modified()) { mark_path_modified(path, true); }
    path.push_front(this);
    for (size_t i = 0; i < this->choice.value.size(); ++i) {
        this->choice.value[i]->update_modified(path);
    }
    path.pop_front();
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
void Switch::render(OpenVRML::viewer & viewer, const rendering_context context)
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
const bounding_volume & Switch::bounding_volume() const {
    if (this->bounding_volume_dirty()) {
        const_cast<Switch *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<node_ptr> & Switch::children() const throw ()
{
    return this->children_.value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void Switch::activate(double time, bool isOver, bool isActive, double *p)
{
    const std::vector<node_ptr> & children = this->children();
    const node_ptr & node = children[0];
    if (node) {
        if (node->to_touch_sensor() && node->to_touch_sensor()->isEnabled()) {
            node->to_touch_sensor()->activate(time, isOver, isActive, p);
        } else if (node->to_plane_sensor()
                && node->to_plane_sensor()->isEnabled()) {
            node->to_plane_sensor()->activate(time, isActive, p);
        } else if (node->to_cylinder_sensor()
                && node->to_cylinder_sensor()->isEnabled()) {
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
void Switch::recalcBSphere()
{
    this->bsphere.reset();
    long w = this->whichChoice.value;
    if (w >= 0 && size_t(w) < this->choice.value.size()) {
        const node_ptr & node = this->choice.value[w];
        if (node) {
            const OpenVRML::bounding_volume & ci_bv = node->bounding_volume();
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
void Switch::processSet_choice(const field_value & value,
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
void Switch::processSet_whichChoice(const field_value & value,
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
 * @class TextClass
 *
 * @brief Class object for Text nodes.
 */

/**
 * @var FT_Library TextClass::freeTypeLibrary
 *
 * @brief FreeType library handle.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Library
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
TextClass::TextClass(OpenVRML::browser & browser):
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
 * @brief Destructor.
 */
TextClass::~TextClass() throw ()
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
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Text nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by TextClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr TextClass::create_type(const std::string & id,
                                        const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "string"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "fontStyle"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "length"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "maxExtent")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Text>(*this, id));
    Vrml97NodeTypeImpl<Text> & textNodeType =
            static_cast<Vrml97NodeTypeImpl<Text> &>(*type);
    typedef Vrml97NodeTypeImpl<Text>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            textNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &Text::processSet_string,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Text, mfstring>
                                    (&Text::string)));
        } else if (*itr == supportedInterfaces[1]) {
            textNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Text::processSet_fontStyle,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Text, sfnode>
                                    (&Text::fontStyle)));
        } else if (*itr == supportedInterfaces[2]) {
            textNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Text::processSet_length,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Text, mffloat>
                                    (&Text::length)));
        } else if (*itr == supportedInterfaces[3]) {
            textNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Text::processSet_maxExtent,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Text, sffloat>
                                    (&Text::maxExtent)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Text
 *
 * @brief Text node instances.
 */

/**
 * @var Text::TextClass
 *
 * @brief Class object for Text instances.
 */

/**
 * @var mfstring Text::string
 *
 * @brief string exposedField.
 */

/**
 * @var sfnode Text::fontStyle
 *
 * @brief fontStyle exposedField.
 */

/**
 * @var mffloat Text::length
 *
 * @brief length exposedField.
 */

/**
 * @var sffloat Text::maxExtent
 *
 * @brief maxExtent exposedField.
 */

/**
 * @struct Text::GlyphGeometry
 *
 * @brief Used to hold the geometry of individual glyphs.
 */

/**
 * @var mfvec2f Text::GlyphGeometry::coord
 *
 * @brief Glyph coordinates.
 */

/**
 * @var mfint32 Text::GlyphGeometry::coordIndex
 *
 * @brief Glyph coordinate indices.
 */

/**
 * @var float Text::GlyphGeometry::advanceWidth
 *
 * @brief The distance the pen should advance horizontally after drawing the
 *      glyph.
 */

/**
 * @var float Text::GlyphGeometry::advanceHeight
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
        using OpenVRML_::fpequal;
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
 * @param contours      a vector of closed contours that make up the glyph's
 *                      outline.
 * @param advanceWidth  the distance the pen should advance horizontally after
 *                      drawing the glyph.
 * @param advanceHeight the distance the pen should advance vertically after
 *                      drawing the glyph.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
Text::GlyphGeometry::GlyphGeometry(
        const std::vector<std::vector<vec2f> > & contours,
        const float advanceWidth,
        const float advanceHeight)
    throw (std::bad_alloc):
    advanceWidth(advanceWidth),
    advanceHeight(advanceHeight)
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
            ConnectionMap;
        ConnectionMap connectionMap;

        //
        // Fill connectionMap. For each interior contour, find the exterior
        // vertex that is closest to the first vertex in the interior contour,
        // and the put the pair in the map.
        //
        for (vector<const vector<vec2f> *>::const_iterator interior =
                 polygon->interiors.begin();
             interior != polygon->interiors.end();
             ++interior) {
            assert(*interior);
            assert(!(*interior)->empty());
            long exteriorVertexIndex =
                get_exterior_connecting_vertex_index_(*polygon->exterior,
                                                      polygon->interiors,
                                                      (*interior)->front());
            assert(exteriorVertexIndex > -1);
            const vec2f * const exteriorVertex =
                    &(*polygon->exterior)[exteriorVertexIndex];
            assert(exteriorVertex);
            const ConnectionMap::value_type value(exteriorVertex, *interior);
            connectionMap.insert(value);
        }

        //
        // Finally, draw the polygon.
        //
        assert(!polygon->exterior->empty());
        for (size_t i = 0; i < polygon->exterior->size(); ++i) {
            const vec2f & exteriorVertex = (*polygon->exterior)[i];
            long exteriorIndex = get_vertex_index_(this->coord,
                                                   exteriorVertex);
            if (exteriorIndex > -1) {
                this->coordIndex.push_back(exteriorIndex);
            } else {
                this->coord.push_back(exteriorVertex);
                assert(!this->coord.empty());
                exteriorIndex = this->coord.size() - 1;
                this->coordIndex.push_back(exteriorIndex);
            }
            ConnectionMap::iterator pos;
            while ((pos = connectionMap.find(&exteriorVertex))
                    != connectionMap.end()) {
                for (int i = pos->second->size() - 1; i > -1; --i) {
                    const vec2f & interiorVertex = (*pos->second)[i];
                    const long interiorIndex =
                        get_vertex_index_(this->coord, interiorVertex);
                    if (interiorIndex > -1) {
                        this->coordIndex.push_back(interiorIndex);
                    } else {
                        this->coord.push_back(interiorVertex);
                        assert(!this->coord.empty());
                        this->coordIndex.push_back(this->coord.size() - 1);
                    }
                }
                this->coordIndex.push_back(exteriorIndex);
                connectionMap.erase(pos);
            }
        }
        assert(connectionMap.empty());
        this->coordIndex.push_back(-1);
    }
# endif // OPENVRML_ENABLE_TEXT_NODE
}

/**
 * @struct Text::TextGeometry
 *
 * @brief Holds the text geometry.
 */

/**
 * @var mfvec3f Text::TextGeometry::coord
 *
 * @brief Text geometry coordinates.
 */

/**
 * @var mfint32 Text::TextGeometry::coordIndex
 *
 * @brief Text geometry coordinate indices.
 */

/**
 * @var mfvec3f Text::TextGeometry::normal
 *
 * @brief Text geometry normals.
 */

/**
 * @typedef Text::Ucs4String
 *
 * @brief A vector of FcChar32 vectors.
 */

/**
 * @typedef Text::GlyphGeometryMap
 *
 * @brief Maps FT_UInts to GlyphGeometry.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-basic_types.html#FT_UInt
 */

/**
 * @var Text::Ucs4String Text::ucs4String
 *
 * @brief UCS-4 equivalent of the (UTF-8) data in @a string.
 */

/**
 * @var FT_Face Text::face
 *
 * @brief Handle to the font face.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Face
 */

/**
 * @var Text::GlyphGeometryMap Text::glyphGeometryMap
 *
 * @brief Map of glyph indices to GlyphGeometry.
 *
 * GlyphGeometry instances are created as needed, as new glyphs are
 * encountered. Once they are created, they are cached in the glyphGeometryMap
 * for rapid retrieval the next time the glyph is encountered.
 */

/**
 * @var Text::TextGeometry Text::textGeometry
 *
 * @brief The text geometry.
 */

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
Text::Text(const node_type & type,
           const scope_ptr & scope):
    node(type, scope),
    AbstractGeometry(type, scope),
    face(0)
{}

/**
 * @brief Destructor.
 */
Text::~Text() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool Text::modified() const {
    return (this->node::modified()
            || (this->fontStyle.value && this->fontStyle.value->modified()));
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void Text::update_modified(node_path & path, int flags) {
    if (this->modified()) { mark_path_modified(path, true); }
    path.push_front(this);
    if (this->fontStyle.value) {
        this->fontStyle.value->update_modified(path);
    }
    path.pop_front();
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
viewer::object_t Text::insert_geometry(OpenVRML::viewer & viewer,
                                     const rendering_context context)
{
    const viewer::object_t retval =
        viewer.insert_shell(viewer::mask_ccw,
                            this->textGeometry.coord,
                            this->textGeometry.coordIndex,
                            std::vector<OpenVRML::color>(), // color
                            std::vector<int32>(), // colorIndex
                            this->textGeometry.normal,
                            std::vector<int32>(), // normalIndex
                            this->textGeometry.texCoord,
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
void Text::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    this->updateUcs4();
    this->updateFace();
    this->updateGeometry();
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void Text::do_shutdown(const double timestamp) throw ()
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
void Text::processSet_string(const field_value & value,
                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->string = dynamic_cast<const mfstring &>(value);
    this->updateUcs4();
    this->updateGeometry();
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
void Text::processSet_fontStyle(const field_value & value,
                                const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->fontStyle = dynamic_cast<const sfnode &>(value);
    this->updateFace();
    this->updateGeometry();
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
void Text::processSet_length(const field_value & value,
                             const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->length = dynamic_cast<const mffloat &>(value);
    this->updateGeometry();
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
void Text::processSet_maxExtent(const field_value & value,
                                const double timestamp) throw (std::bad_cast)
{
    this->maxExtent = dynamic_cast<const sffloat &>(value);
    this->updateGeometry();
    this->node::modified(true);
    this->emit_event("maxExtent_changed", this->maxExtent, timestamp);
}

/**
 * @brief Called when @a string changes to update the UCS-4 text.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void Text::updateUcs4() throw (std::bad_alloc)
{
# ifdef OPENVRML_ENABLE_TEXT_NODE
    this->ucs4String.clear();
    this->ucs4String.resize(this->string.value.size());

    for (size_t i = 0; i < this->string.value.size(); ++i) {
        using std::string;
        using std::vector;

        const string & element = this->string.value[i];

        vector<FcChar32> & ucs4Element = this->ucs4String[i];

        //
        // First, we need to convert the characters from UTF-8 to UCS-4.
        //
        vector<FcChar8> utf8String(element.begin(), element.end());
        int nchar = 0, wchar = 0;
        FcUtf8Len(&utf8String[0], utf8String.size(), &nchar, &wchar);
        ucs4Element.resize(nchar);
        {
            vector<FcChar8>::iterator utf8itr = utf8String.begin();
            vector<FcChar32>::iterator ucs4itr = ucs4Element.begin();
            while (utf8itr != utf8String.end()) {
                const int utf8bytes = FcUtf8ToUcs4(&*utf8itr, &*ucs4itr,
                                                   utf8String.end() - utf8itr);
                utf8itr += utf8bytes;
                ucs4itr++;
            }
        }
    }
# endif // OPENVRML_ENABLE_TEXT_NODE
}

/**
 * @brief Called when @a fontStyle changes to update the font face.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void Text::updateFace() throw (std::bad_alloc)
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
    typedef std::basic_string<FcChar8> FcChar8String;

    FcChar8String language;

    std::vector<string> family;
    family.push_back("SERIF");

    string style;

    font_style_node * const fontStyle = this->fontStyle.value
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
                FcNameParse(FcChar8String(fontName.begin(),
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

            TextClass & nodeClass =
                    static_cast<TextClass &>(this->type._class);

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
            this->glyphGeometryMap.clear();

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

    int conicTo_(FT_Vector * const control, FT_Vector * const to,
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

    int cubicTo_(FT_Vector * const control1, FT_Vector * const control2,
                 FT_Vector * const to, void * const user)
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
        const vec2f & lastVertex = contour[contour.size() - 1];

        assert(!contour.empty());
        const size_t npoints = 4;
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
 * @brief Called to update @a textGeometry.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void Text::updateGeometry() throw (std::bad_alloc)
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
    font_style_node * fontStyle;
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

    TextGeometry newGeometry;
    float geometryXMin = 0.0, geometryXMax = 0.0;
    float geometryYMin = 0.0, geometryYMax = 0.0;
    size_t npolygons = 0;
    const Ucs4String::const_iterator stringBegin = this->ucs4String.begin();
    for (Ucs4String::const_iterator string = stringBegin;
            string != this->ucs4String.end(); ++string) {
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

            const GlyphGeometry * glyphGeometry = 0;
            const GlyphGeometryMap::iterator pos =
                    this->glyphGeometryMap.find(glyphIndex);
            if (pos != this->glyphGeometryMap.end()) {
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

                const GlyphGeometryMap::value_type
                        value(glyphIndex, GlyphGeometry(glyphContours.contours,
                                                        advanceWidth,
                                                        advanceHeight));
                const pair<GlyphGeometryMap::iterator, bool> result =
                        this->glyphGeometryMap.insert(value);
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

            for (size_t i = 0; i < glyphGeometry->coordIndex.size(); ++i) {
                const long index = glyphGeometry->coordIndex[i];
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
                const float xAdvance = glyphGeometry->advanceWidth;
                if (leftToRight) {
                    penPos[0] += xAdvance;
                } else {
                    penPos[0] -= xAdvance;
                }
            } else {
                const float yAdvance = glyphGeometry->advanceHeight;
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
                newGeometry.coordIndex.push_back(newGeometry.coord.size() - 1);
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
                newGeometry.coordIndex.push_back(-1);
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
    newGeometry.texCoord.resize(newGeometry.coord.size()); // std::bad_alloc
    for (size_t i = 0; i < newGeometry.texCoord.size(); ++i) {
        const vec3f & vertex = newGeometry.coord[i];
        newGeometry.texCoord[i] = vec2f(vertex.x() / size, vertex.y() / size);
    }

    this->textGeometry = newGeometry;
# endif // OPENVRML_ENABLE_TEXT_NODE
}


/**
 * @class TextureCoordinateClass
 *
 * @brief Class object for TextureCoordinate nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
TextureCoordinateClass::TextureCoordinateClass(OpenVRML::browser & browser):
        node_class(browser) {}

/**
 * @brief Destructor.
 */
TextureCoordinateClass::~TextureCoordinateClass() throw () {}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating TextureCoordinate
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by TextureCoordinateClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
        TextureCoordinateClass::create_type(const std::string & id,
                                           const node_interface_set & interfaces)
        throw (unsupported_interface, std::bad_alloc) {
    static const node_interface supportedInterface =
        node_interface(node_interface::exposedfield_id,
                      field_value::mfvec2f_id,
                      "point");
    const node_type_ptr
        type(new Vrml97NodeTypeImpl<TextureCoordinate>(*this, id));
    Vrml97NodeTypeImpl<TextureCoordinate> & textureCoordinateNodeType =
            static_cast<Vrml97NodeTypeImpl<TextureCoordinate> &>(*type);
    typedef Vrml97NodeTypeImpl<TextureCoordinate>::NodeFieldPtrPtr
        NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterface) {
            textureCoordinateNodeType.addExposedField(
                supportedInterface.field_type,
                supportedInterface.id,
                &TextureCoordinate::processSet_point,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TextureCoordinate,
                                                     mfvec2f>
                                (&TextureCoordinate::point_)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class TextureCoordinate
 *
 * @brief TextureCoordinate node instances.
 */

/**
 * @var TextureCoordinate::TextureCoordinateClass
 *
 * @brief Class object for TextureCoordinate instances.
 */

/**
 * @var mfvec2f TextureCoordinate::point
 *
 * @brief point exposedField.
 */

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
TextureCoordinate::TextureCoordinate(const node_type & type,
                                     const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    texture_coordinate_node(type, scope)
{}

/**
 * @brief Destructor.
 */
TextureCoordinate::~TextureCoordinate() throw () {}

/**
 * @brief Get the points encapsulated by this node.
 *
 * @return the mfvec2f array of points for this node.
 */
const std::vector<vec2f> & TextureCoordinate::point() const throw ()
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
void TextureCoordinate::processSet_point(const field_value & value,
                                         const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    this->point_ = dynamic_cast<const mfvec2f &>(value);
    this->node::modified(true);
    this->emit_event("point_changed", this->point_, timestamp);
}


/**
 * @class TextureTransformClass
 *
 * @brief Class object for TextureTransform nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
TextureTransformClass::TextureTransformClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
TextureTransformClass::~TextureTransformClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating TextureTransform
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by TextureTransformClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
TextureTransformClass::create_type(const std::string & id,
                                  const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "center"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "rotation"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "scale"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "translation")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<TextureTransform>(*this, id));
    Vrml97NodeTypeImpl<TextureTransform> & textureTransformNodeType =
            static_cast<Vrml97NodeTypeImpl<TextureTransform> &>(*type);
    typedef Vrml97NodeTypeImpl<TextureTransform>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            textureTransformNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &TextureTransform::processSet_center,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TextureTransform, sfvec2f>
                                    (&TextureTransform::center)));
        } else if (*itr == supportedInterfaces[1]) {
            textureTransformNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &TextureTransform::processSet_rotation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TextureTransform, sffloat>
                                    (&TextureTransform::rotation)));
        } else if (*itr == supportedInterfaces[2]) {
            textureTransformNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &TextureTransform::processSet_scale,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TextureTransform, sfvec2f>
                                    (&TextureTransform::scale)));
        } else if (*itr == supportedInterfaces[3]) {
            textureTransformNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &TextureTransform::processSet_translation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TextureTransform, sfvec2f>
                                    (&TextureTransform::translation)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class TextureTransform
 *
 * @brief TextureTransform node instances.
 */

/**
 * @var TextureTransform::TextureTransformClass
 *
 * @brief Class object for TextureTransform instances.
 */

/**
 * @var sfvec2f TextureTransform::center
 *
 * @brief center exposedField.
 */

/**
 * @var sffloat TextureTransform::rotation
 *
 * @brief rotation exposedField.
 */

/**
 * @var sfvec2f TextureTransform::scale
 *
 * @brief scale exposedField.
 */

/**
 * @var sfvec2f TextureTransform::translation
 *
 * @brief translation exposedField.
 */

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
TextureTransform::TextureTransform(const node_type & type,
                                   const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    texture_transform_node(type, scope),
    center(vec2f(0.0, 0.0)),
    rotation(0.0),
    scale(vec2f(1.0, 1.0)),
    translation(vec2f(0.0, 0.0))
{}

/**
 * @brief Destructor.
 */
TextureTransform::~TextureTransform() throw ()
{}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void TextureTransform::render(OpenVRML::viewer & viewer,
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
void TextureTransform::processSet_center(const field_value & value,
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
void TextureTransform::processSet_rotation(const field_value & value,
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
void TextureTransform::processSet_scale(const field_value & value,
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
void TextureTransform::processSet_translation(const field_value & value,
                                              const double timestamp)
    throw (std::bad_cast)
{
    this->translation = dynamic_cast<const sfvec2f &>(value);
    this->node::modified(true);
    this->emit_event("translation_changed", this->translation, timestamp);
}


/**
 * @class TimeSensorClass
 *
 * @brief Class object for TimeSensor nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser   the browser associated with this NodeClass.
 */
TimeSensorClass::TimeSensorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
TimeSensorClass::~TimeSensorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating TimeSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by TimeSensorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
TimeSensorClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<TimeSensor>(*this, id));
    Vrml97NodeTypeImpl<TimeSensor> & timeSensorNodeType =
            static_cast<Vrml97NodeTypeImpl<TimeSensor> &>(*type);
    typedef Vrml97NodeTypeImpl<TimeSensor>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            timeSensorNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &TimeSensor::processSet_cycleInterval,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sftime>
                                    (&TimeSensor::cycleInterval)));
        } else if (*itr == supportedInterfaces[1]) {
            timeSensorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &TimeSensor::processSet_enabled,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sfbool>
                                    (&TimeSensor::enabled)));
        } else if (*itr == supportedInterfaces[2]) {
            timeSensorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &TimeSensor::processSet_loop,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sfbool>
                                    (&TimeSensor::loop)));
        } else if (*itr == supportedInterfaces[3]) {
            timeSensorNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &TimeSensor::processSet_startTime,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sftime>
                                    (&TimeSensor::startTime)));
        } else if (*itr == supportedInterfaces[4]) {
            timeSensorNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &TimeSensor::processSet_stopTime,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sftime>
                                    (&TimeSensor::stopTime)));
        } else if (*itr == supportedInterfaces[5]) {
            timeSensorNodeType.addEventOut(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sftime>
                                    (&TimeSensor::cycleTime)));
        } else if (*itr == supportedInterfaces[6]) {
            timeSensorNodeType.addEventOut(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sffloat>
                                    (&TimeSensor::fraction)));
        } else if (*itr == supportedInterfaces[7]) {
            timeSensorNodeType.addEventOut(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sfbool>
                                    (&TimeSensor::active)));
        } else if (*itr == supportedInterfaces[8]) {
            timeSensorNodeType.addEventOut(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TimeSensor, sftime>
                                    (&TimeSensor::time)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class TimeSensor
 *
 * @brief TimeSensor node instances.
 */

/**
 * @var TimeSensor::TimeSensorClass
 *
 * @brief Class object for TimeSensor instances.
 */

/**
 * @var sftime TimeSensor::cycleInterval
 *
 * @brief cycleInterval exposedField.
 */

/**
 * @var sfbool TimeSensor::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfbool TimeSensor::loop
 *
 * @brief loop exposedField.
 */

/**
 * @var sftime TimeSensor::startTime
 *
 * @brief startTime exposedField.
 */

/**
 * @var sftime TimeSensor::stopTime
 *
 * @brief stopTime exposedField.
 */

/**
 * @var sftime TimeSensor::cycleTime
 *
 * @brief cycleTime eventOut.
 */

/**
 * @var sffloat TimeSensor::fraction
 *
 * @brief fraction_changed eventOut.
 */

/**
 * @var sfbool TimeSensor::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sftime TimeSensor::time
 *
 * @brief time eventOut.
 */

/**
 * @var double TimeSensor::lastTime
 *
 * @brief The timestamp previously received.
 */

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
TimeSensor::TimeSensor(const node_type & type,
                       const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    cycleInterval(1.0),
    enabled(true),
    loop(false),
    startTime(0.0),
    stopTime(0.0),
    active(false),
    lastTime(-1.0)
{}

/**
 * @brief Destructor.
 */
TimeSensor::~TimeSensor() throw ()
{}

/**
 * @brief Cast to a TimeSensor.
 *
 * @return a pointer to the object.
 */
TimeSensor * TimeSensor::to_time_sensor() const
{
    return (TimeSensor*) this;
}

/**
 * Generate timer events. If necessary, events prior to the timestamp (inTime)
 * are generated to respect stopTimes and cycleIntervals. The timestamp
 * should never be increased. This assumes the event loop delivers pending
 * events in order (ascending time stamps). Should inTime be modified?
 * Should ensure continuous events are delivered before discrete ones
 * (such as cycleTime, isActive).
 */
void TimeSensor::update(const double currentTime)
{
    using OpenVRML_::fpzero;
    using OpenVRML_::fpequal;

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
            double f, cycleInt = this->cycleInterval.value;
            bool deactivate = false;

            // Are we done? Choose min of stopTime or start + single cycle.
            if ((this->stopTime.value > this->startTime.value
                        && this->stopTime.value <= timeNow.value)
                    || (!this->loop.value
                        && this->startTime.value + cycleInt <= timeNow.value)) {
                this->active.value = false;

                // Must respect stopTime/cycleInterval exactly
                if (this->startTime.value + cycleInt < this->stopTime.value) {
                    timeNow = sftime(this->startTime.value + cycleInt);
                } else {
                    timeNow = this->stopTime;
                }

                deactivate = true;
            }

            if (cycleInt > 0.0 && timeNow.value > this->startTime.value) {
                f = fmod(timeNow.value - this->startTime.value, cycleInt);
            } else {
                f = 0.0;
            }

            // Fraction of cycle message
            sffloat fraction_changed(fpzero(f) ? 1.0 : (f / cycleInt));
            this->emit_event("fraction_changed", fraction_changed, timeNow.value);

            // Current time message
            this->emit_event("time", timeNow, timeNow.value);

            // End of cycle message (this may miss cycles...)
            if (fpequal(fraction_changed.value, 1.0)) {
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
            this->type._class.browser.delta(0.0);
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
void TimeSensor::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_time_sensor(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void TimeSensor::do_shutdown(const double timestamp) throw ()
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
void TimeSensor::processSet_cycleInterval(const field_value & value,
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
void TimeSensor::processSet_enabled(const field_value & value,
                                    const double timestamp)
    throw (std::bad_cast)
{
    using OpenVRML_::fpzero;

    this->enabled = dynamic_cast<const sfbool &>(value);
    if (this->enabled.value != this->active.value) {
        if (this->active.value) {
            //
            // Was active; shutdown.
            //
            double cycleInt = this->cycleInterval.value;
            double f = (cycleInt > 0.0)
                     ? fmod(this->time.value - this->startTime.value, cycleInt)
                     : 0.0;

            // Fraction of cycle message
            this->fraction.value = fpzero(f) ? 1.0 : (f / cycleInt);
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
void TimeSensor::processSet_loop(const field_value & value,
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
void TimeSensor::processSet_startTime(const field_value & value,
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
void TimeSensor::processSet_stopTime(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->stopTime = dynamic_cast<const sftime &>(value);
    this->emit_event("stopTime_changed", this->stopTime, timestamp);
}


/**
 * @class TouchSensorClass
 *
 * @brief Class object for TouchSensor nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
TouchSensorClass::TouchSensorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
TouchSensorClass::~TouchSensorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating TouchSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by TouchSensorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
TouchSensorClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<TouchSensor>(*this, id));
    Vrml97NodeTypeImpl<TouchSensor> & touchSensorNodeType =
            static_cast<Vrml97NodeTypeImpl<TouchSensor> &>(*type);
    typedef Vrml97NodeTypeImpl<TouchSensor>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            touchSensorNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &TouchSensor::processSet_enabled,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TouchSensor, sfbool>
                                    (&TouchSensor::enabled)));
        } else if (*itr == supportedInterfaces[1]) {
            touchSensorNodeType.addEventOut(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TouchSensor, sfvec3f>
                                    (&TouchSensor::hitNormal)));
        } else if (*itr == supportedInterfaces[2]) {
            touchSensorNodeType.addEventOut(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TouchSensor, sfvec3f>
                                    (&TouchSensor::hitPoint)));
        } else if (*itr == supportedInterfaces[3]) {
            touchSensorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TouchSensor, sfvec2f>
                                    (&TouchSensor::hitTexCoord)));
        } else if (*itr == supportedInterfaces[4]) {
            touchSensorNodeType.addEventOut(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TouchSensor, sfbool>
                                    (&TouchSensor::active)));
        } else if (*itr == supportedInterfaces[5]) {
            touchSensorNodeType.addEventOut(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TouchSensor, sfbool>
                                    (&TouchSensor::over)));
        } else if (*itr == supportedInterfaces[6]) {
            touchSensorNodeType.addEventOut(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<TouchSensor, sftime>
                                    (&TouchSensor::touchTime)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class TouchSensor
 *
 * @brief TouchSensor node instances.
 */

/**
 * @var TouchSensor::TouchSensorClass
 *
 * @brief Class object for TouchSensor instances.
 */

/**
 * @var sfbool TouchSensor::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var sfvec3f TouchSensor::hitNormal
 *
 * @brief hitNormal_changed eventOut.
 */

/**
 * @var sfvec3f TouchSensor::hitPoint
 *
 * @brief hitPoint_changed eventOut.
 */

/**
 * @var sfvec2f TouchSensor::hitTexCoord
 *
 * @brief hitTexCoord_changed eventOut.
 */

/**
 * @var sfbool TouchSensor::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var sfbool TouchSensor::over
 *
 * @brief isOver eventOut.
 */

/**
 * @var sftime TouchSensor::touchTime
 *
 * @brief touchTime eventOut.
 */

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
TouchSensor::TouchSensor(const node_type & type,
                         const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
    enabled(true),
    active(false),
    over(false),
    touchTime(0.0)
{
    this->node::modified(true);
}

/**
 * @brief Destructor.
 */
TouchSensor::~TouchSensor() throw ()
{}

/**
 * @brief Cast to a TouchSensor.
 *
 * @return a pointer to the object.
 */
TouchSensor* TouchSensor::to_touch_sensor() const
{
    return (TouchSensor*) this;
}

/**
 * @todo Doesn't compute the xxx_changed eventOuts yet...
 */
void TouchSensor::activate(double timeStamp, bool isOver, bool isActive,
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
bool TouchSensor::isEnabled() const
{
    return this->enabled.value;
}

/**
 * @brief set_enabled eventIn handler.
 *
 * @param value    an sfbool value.
 * @param timestamp the current time.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
void TouchSensor::processSet_enabled(const field_value & value,
                                     const double timestamp)
    throw (std::bad_cast)
{
    this->enabled = dynamic_cast<const sfbool &>(value);
    this->emit_event("enabled_changed", this->enabled, timestamp);
}


/**
 * @class TransformClass
 *
 * @brief Class object for Transform nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
TransformClass::TransformClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
TransformClass::~TransformClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Transform nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by TransformClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
TransformClass::create_type(const std::string & id,
                           const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "addChildren"),
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "removeChildren"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "center"),
        node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "children"),
        node_interface(node_interface::exposedfield_id, field_value::sfrotation_id, "rotation"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "scale"),
        node_interface(node_interface::exposedfield_id, field_value::sfrotation_id, "scaleOrientation"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "translation"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxCenter"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxSize")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<Transform>(*this, id));
    Vrml97NodeTypeImpl<Transform> & transformNodeType =
            static_cast<Vrml97NodeTypeImpl<Transform> &>(*type);
    typedef Vrml97NodeTypeImpl<Transform>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            transformNodeType.addEventIn(supportedInterfaces[0].field_type,
                                      supportedInterfaces[0].id,
                                      &Transform::processAddChildren);
        } else if (*itr == supportedInterfaces[1]) {
            transformNodeType.addEventIn(supportedInterfaces[1].field_type,
                                      supportedInterfaces[1].id,
                                      &Transform::processRemoveChildren);
        } else if (*itr == supportedInterfaces[2]) {
            transformNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Transform::processSet_center,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Transform, sfvec3f>
                                    (&Transform::center)));
        } else if (*itr == supportedInterfaces[3]) {
            transformNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Transform::processSet_children,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Transform, mfnode>
                                    (&Transform::children_)));
        } else if (*itr == supportedInterfaces[4]) {
            transformNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &Transform::processSet_rotation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Transform, sfrotation>
                                    (&Transform::rotation)));
        } else if (*itr == supportedInterfaces[5]) {
            transformNodeType.addExposedField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                &Transform::processSet_scale,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Transform, sfvec3f>
                                    (&Transform::scale)));
        } else if (*itr == supportedInterfaces[6]) {
            transformNodeType.addExposedField(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                &Transform::processSet_scaleOrientation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Transform, sfrotation>
                                    (&Transform::scaleOrientation)));
        } else if (*itr == supportedInterfaces[7]) {
            transformNodeType.addExposedField(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                &Transform::processSet_translation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Transform, sfvec3f>
                                    (&Transform::translation)));
        } else if (*itr == supportedInterfaces[8]) {
            transformNodeType.addField(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Transform, sfvec3f>
                                    (&Transform::bboxCenter)));
        } else if (*itr == supportedInterfaces[9]) {
            transformNodeType.addField(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Transform, sfvec3f>
                                (&Transform::bboxSize)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Transform
 *
 * @brief Transform node instances.
 */

/**
 * @var Transform::TransformClass
 *
 * @brief Class object for Transform instances.
 */

/**
 * @var sfvec3f Transform::center
 *
 * @brief center exposedField.
 */

/**
 * @var sfrotation Transform::rotation
 *
 * @brief rotation exposedField.
 */

/**
 * @var sfvec3f Transform::scale
 *
 * @brief scale exposedField.
 */

/**
 * @var sfrotation Transform::scaleOrientation
 *
 * @brief scaleOrientation exposedField.
 */

/**
 * @var sfvec3f Transform::translation
 *
 * @brief translation exposedField.
 */

/**
 * @var viewer::object_t Transform::xformObject
 *
 * @brief A handle to the renderer's representation of the Transform.
 */

/**
 * @var mat4f Transform::M
 *
 * @brief Cached copy of this node's transformation.
 *
 * Currently this is used only by the culling code, but eventually
 * all the matrix manipulation needs to be moved from the Viewer
 * side over into core.
 */

/**
 * @var Transform::M_dirty
 *
 * @brief If true, we need to recalculate M.
 *
 * Is this the same as Node::modified_? No, since it's entirely a core-side
 * issue, and has nothing to do with the viewer being out of date wrt the
 * core scene graph.
 */

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
Transform::Transform(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    Group(type, scope),
    transform_node(type, scope),
    center(vec3f(0.0, 0.0, 0.0)),
    rotation(OpenVRML::rotation(0.0, 0.0, 1.0, 0.0)),
    scale(vec3f(1.0, 1.0, 1.0)),
    scaleOrientation(OpenVRML::rotation(0.0, 0.0, 1.0, 0.0)),
    translation(vec3f(0.0, 0.0, 0.0)),
    transformDirty(true),
    xformObject(0)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destructor.
 */
Transform::~Transform() throw ()
{
    // delete xformObject...
}

/**
 * @brief Get the transformation associated with the node as a matrix.
 *
 * @return the transformation associated with the node.
 */
const mat4f & Transform::transform() const throw ()
{
    this->updateTransform();
    return this->transform_;
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
void Transform::render(OpenVRML::viewer & viewer, rendering_context context)
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
        this->Group::renderNoCull(viewer, context);

        viewer.end_object();
    }
    this->node::modified(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const bounding_volume & Transform::bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<Transform *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief Recalculate the bounding volume.
 */
void Transform::recalcBSphere()
{
    this->bsphere.reset();
    for (size_t i = 0; i < this->children_.value.size(); ++i) {
        const node_ptr & node = this->children_.value[i];
        if (node) {
            const OpenVRML::bounding_volume & ci_bv = node->bounding_volume();
            this->bsphere.extend(ci_bv);
        }
    }
    this->bsphere.transform(this->transform());
    this->bounding_volume_dirty(false);
}



#if 0
void
Transform::recalcBSphere()
{
  cout << "Transform[" << this << "]::recalcBSphere()" << endl;
  updateTransform();
  d_bsphere.reset();
  for (int i = 0; i<d_children.size(); ++i) {
    Node* ci = d_children[i];
    const bounding_volume * ci_bv = ci->bounding_volume();
    if (ci_bv) { // shouldn't happen...
      bounding_sphere * bs = (bounding_sphere*)ci_bv;
      bounding_sphere tmp(*bs);
      tmp.transform(M);
      d_bsphere.extend(tmp);
    }
  }
  this->bounding_volume_dirty(false);
}
#endif


// P' = T × C × R × SR × S × -SR × -C × P
//
/**
 * @brief Update @a transform.
 *
 * If @a transformDirty is @c true, resynchronize the cached matrix
 * @a transform with the node fields.
 */
void Transform::updateTransform() const throw ()
{
    if (this->transformDirty) {
        this->transform_ = mat4f::transformation(this->translation.value,
                                                 this->rotation.value,
                                                 this->scale.value,
                                                 this->scaleOrientation.value,
                                                 this->center.value);
        this->transformDirty = false;
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
void Transform::processSet_center(const field_value & value,
                                  const double timestamp)
    throw (std::bad_cast)
{
    this->center = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transformDirty = true;
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
void Transform::processSet_rotation(const field_value & value,
                                    const double timestamp)
    throw (std::bad_cast)
{
    this->rotation = dynamic_cast<const sfrotation &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transformDirty = true;
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
void Transform::processSet_scale(const field_value & value,
                                 const double timestamp)
    throw (std::bad_cast)
{
    this->scale = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transformDirty = true;
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
void Transform::processSet_scaleOrientation(const field_value & value,
                                            const double timestamp)
    throw (std::bad_cast)
{
    this->scaleOrientation = dynamic_cast<const sfrotation &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transformDirty = true;
    this->emit_event("scaleOrientation_changed", this->scaleOrientation,
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
void Transform::processSet_translation(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast)
{
    this->translation = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->bounding_volume_dirty(true);
    this->transformDirty = true;
    this->emit_event("translation_changed", this->translation, timestamp);
}


/**
 * @class ViewpointClass
 *
 * @brief Class object for Viewpoint nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
ViewpointClass::ViewpointClass(OpenVRML::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destructor.
 */
ViewpointClass::~ViewpointClass() throw ()
{}

/**
 * @brief Set the first Viewpoint node in the world.
 *
 * The first Viewpoint node in the world is used as the initial viewpoint.
 * This method is used by Viewpoint::do_initialize.
 *
 * @param viewpoint    a Viewpoint node.
 */
void ViewpointClass::setFirst(Viewpoint & viewpoint) throw ()
{
    this->first = &viewpoint;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by Viewpoint::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool ViewpointClass::hasFirst() const throw ()
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
void ViewpointClass::bind(Viewpoint & viewpoint, const double timestamp)
    throw (std::bad_alloc)
{
    //
    // If the node is already the active node, do nothing.
    //
    if (!this->boundNodes.empty() && &viewpoint == this->boundNodes.back()) {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const BoundNodes::iterator pos =
        std::find(this->boundNodes.begin(), this->boundNodes.end(), &viewpoint);
    if (pos != this->boundNodes.end()) { this->boundNodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->boundNodes.empty()) {
        Viewpoint & current =
                dynamic_cast<Viewpoint &>(*this->boundNodes.back());
        current.bound.value = false;
        current.emit_event("isBound", current.bound, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->boundNodes.push_back(&viewpoint);
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
void ViewpointClass::unbind(Viewpoint & viewpoint, const double timestamp)
    throw ()
{
    const BoundNodes::iterator pos =
        std::find(this->boundNodes.begin(), this->boundNodes.end(), &viewpoint);
    if (pos != this->boundNodes.end()) {
        viewpoint.bound.value = false;
        viewpoint.emit_event("isBound", viewpoint.bound, timestamp);

        if (pos == this->boundNodes.end() - 1
                && this->boundNodes.size() > 1) {
            Viewpoint & newActive =
                    dynamic_cast<Viewpoint &>(**(this->boundNodes.end() - 2));
            newActive.bound.value = true;
            newActive.emit_event("isBound", newActive.bound, timestamp);

            this->browser.active_viewpoint(viewpoint);
        } else {
            this->browser.reset_default_viewpoint();
        }
        this->boundNodes.erase(pos);
    }
}

/**
 * @brief NodeClass-specific initialization.
 *
 * @param initialViewpoint  the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void ViewpointClass::initialize(viewpoint_node * initialViewpoint,
                                const double timestamp)
    throw ()
{
    if (!initialViewpoint) { initialViewpoint = this->first; }
    if (initialViewpoint) {
        initialViewpoint->process_event("set_bind", sfbool(true), timestamp);
    }
}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating Viewpoint nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by ViewpointClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
ViewpointClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<Viewpoint>(*this, id));
    Vrml97NodeTypeImpl<Viewpoint> & viewpointNodeType =
            static_cast<Vrml97NodeTypeImpl<Viewpoint> &>(*type);
    typedef Vrml97NodeTypeImpl<Viewpoint>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            viewpointNodeType.addEventIn(supportedInterfaces[0].field_type,
                                   supportedInterfaces[0].id,
                                   &Viewpoint::processSet_bind);
        } else if (*itr == supportedInterfaces[1]) {
            viewpointNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &Viewpoint::processSet_fieldOfView,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Viewpoint, sffloat>
                                    (&Viewpoint::fieldOfView)));
        } else if (*itr == supportedInterfaces[2]) {
            viewpointNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &Viewpoint::processSet_jump,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Viewpoint, sfbool>
                                    (&Viewpoint::jump)));
        } else if (*itr == supportedInterfaces[3]) {
            viewpointNodeType.addExposedField(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                &Viewpoint::processSet_orientation,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Viewpoint, sfrotation>
                                    (&Viewpoint::orientation_)));
        } else if (*itr == supportedInterfaces[4]) {
            viewpointNodeType.addExposedField(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                &Viewpoint::processSet_position,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Viewpoint, sfvec3f>
                                    (&Viewpoint::position_)));
        } else if (*itr == supportedInterfaces[5]) {
            viewpointNodeType.addField(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Viewpoint, sfstring>
                                    (&Viewpoint::description_)));
        } else if (*itr == supportedInterfaces[6]) {
            viewpointNodeType.addEventOut(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Viewpoint, sftime>
                                    (&Viewpoint::bindTime)));
        } else if (*itr == supportedInterfaces[7]) {
            viewpointNodeType.addEventOut(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<Viewpoint, sfbool>
                                    (&Viewpoint::bound)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class Viewpoint
 *
 * @todo need copy constructor for d_parentTransform ...
 */

/**
 * @var Viewpoint::ViewpointClass
 *
 * @brief Class object for Viewpoint instances.
 */

/**
 * @var sffloat Viewpoint::fieldOfView
 *
 * @brief fieldOfView exposedField.
 */

/**
 * @var sfbool Viewpoint::jump
 *
 * @brief jump exposedField.
 */

/**
 * @var sfrotation Viewpoint::orientation
 *
 * @brief orientation exposedField.
 */

/**
 * @var sfvec3f Viewpoint::position
 *
 * @brief position exposedField.
 */

/**
 * @var sfstring Viewpoint::description
 *
 * @brief description field.
 */

/**
 * @var sfbool Viewpoint::bound
 *
 * @brief isBound eventOut.
 */

/**
 * @var sftime Viewpoint::bindTime
 *
 * @brief bindTime eventOut.
 */

/**
 * @var Node * Viewpoint::parentTransform
 *
 * @brief A pointer to the parent Transform for this node.
 */

namespace {
    const float DEFAULT_FIELD_OF_VIEW = 0.785398;
}

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
Viewpoint::Viewpoint(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    AbstractBase(type, scope),
    child_node(type, scope),
    viewpoint_node(type, scope),
    fieldOfView(DEFAULT_FIELD_OF_VIEW),
    jump(true),
    orientation_(rotation(0.0, 0.0, 1.0, 0.0)),
    position_(vec3f(0.0, 0.0, 10.0)),
    bound(false),
    bindTime(0),
    finalTransformationDirty(true)
{}

/**
 * @brief Destructor.
 */
Viewpoint::~Viewpoint() throw ()
{}

/**
 * @brief Get the transformation of the viewpoint_node in the global coordinate
 *      system.
 *
 * @return the transformation of the viewpoint_node in the global coordinate
 *      system.
 */
const mat4f & Viewpoint::transformation() const throw ()
{
    this->updateFinalTransformation();
    return this->finalTransformation;
}

/**
 * @brief Get the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @return the transformation of the user view relative to the viewpoint_node.
 */
const mat4f & Viewpoint::user_view_transform() const throw ()
{
    return this->userViewTransform;
}

/**
 * @brief Set the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @param transform the new transformation.
 */
void Viewpoint::user_view_transform(const mat4f & transform) throw ()
{
    this->userViewTransform = transform;
}

/**
 * @brief description accessor.
 *
 * @return the description.
 */
const std::string & Viewpoint::description() const throw ()
{
    return this->description_.value;
}

/**
 * @brief Get the field of view.
 *
 * @return the field of view in radians.
 */
float Viewpoint::field_of_view() const throw ()
{
    return this->fieldOfView.value;
}

/**
 * @brief orientation accessor.
 *
 * @return the orientation.
 */
const sfrotation & Viewpoint::orientation() const
{
    return this->orientation_;
}

/**
 * @brief position accessor.
 *
 * @return the position.
 */
const sfvec3f & Viewpoint::position() const
{
    return this->position_;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void Viewpoint::do_initialize(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.add_viewpoint(*this);
    assert(dynamic_cast<ViewpointClass *>(&this->type._class));
    ViewpointClass & nodeClass =
            static_cast<ViewpointClass &>(this->type._class);
    if (!nodeClass.hasFirst()) { nodeClass.setFirst(*this); }
}

namespace {

    struct AccumulateTransform : std::unary_function<const node *, void> {
        explicit AccumulateTransform(mat4f & transform) throw ():
            transform(&transform)
        {}

        void operator()(const OpenVRML::node * node) const throw ()
        {
            assert(node);
            const transform_node * const transformNode = node->to_transform();
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
void Viewpoint::do_relocate() throw (std::bad_alloc)
{
    assert(this->scene());
    const node_path path = this->scene()->browser.find_node(*this);
    assert(!path.empty());
    this->parentTransform = mat4f();
    std::for_each(path.begin(), path.end(),
                  AccumulateTransform(this->parentTransform));
    this->finalTransformationDirty = true;
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void Viewpoint::do_shutdown(const double timestamp) throw ()
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
void Viewpoint::processSet_bind(const field_value & value,
                                const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    const sfbool & bind = dynamic_cast<const sfbool &>(value);
    assert(dynamic_cast<ViewpointClass *>(&this->type._class));
    ViewpointClass & nodeClass =
            static_cast<ViewpointClass &>(this->type._class);
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
void Viewpoint::processSet_fieldOfView(const field_value & value,
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
void Viewpoint::processSet_jump(const field_value & value,
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
void Viewpoint::processSet_orientation(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast)
{
    this->orientation_ = dynamic_cast<const sfrotation &>(value);
    this->node::modified(true);
    this->finalTransformationDirty = true;
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
void Viewpoint::processSet_position(const field_value & value,
                                    const double timestamp)
    throw (std::bad_cast)
{
    this->position_ = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->finalTransformationDirty = true;
    this->emit_event("position_changed", this->position_, timestamp);
}

void Viewpoint::updateFinalTransformation() const throw ()
{
    if (this->finalTransformationDirty) {
        static const vec3f scale(1.0, 1.0, 1.0);
        static const rotation scaleOrientation;
        static const vec3f center;
        const mat4f & t = mat4f::transformation(this->position_.value,
                                                this->orientation_.value,
                                                scale,
                                                scaleOrientation,
                                                center);
        this->finalTransformation = t * this->parentTransform;
        this->finalTransformationDirty = false;
    }
}


/**
 * @class VisibilitySensorClass
 *
 * @brief Class object for VisibilitySensor nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
VisibilitySensorClass::VisibilitySensorClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
VisibilitySensorClass::~VisibilitySensorClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating VisibilitySensor
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by VisibilitySensorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
VisibilitySensorClass::create_type(const std::string & id,
                                  const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "center"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "size"),
        node_interface(node_interface::eventout_id, field_value::sftime_id, "enterTime"),
        node_interface(node_interface::eventout_id, field_value::sftime_id, "exitTime"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive")
    };
    const node_type_ptr type(new Vrml97NodeTypeImpl<VisibilitySensor>(*this, id));
    Vrml97NodeTypeImpl<VisibilitySensor> & visibilitySensorNodeType =
            static_cast<Vrml97NodeTypeImpl<VisibilitySensor> &>(*type);
    typedef Vrml97NodeTypeImpl<VisibilitySensor>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            visibilitySensorNodeType.addExposedField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                &VisibilitySensor::processSet_center,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<VisibilitySensor, sfvec3f>
                                    (&VisibilitySensor::center)));
        } else if (*itr == supportedInterfaces[1]) {
            visibilitySensorNodeType.addExposedField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                &VisibilitySensor::processSet_enabled,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<VisibilitySensor, sfbool>
                                    (&VisibilitySensor::enabled)));
        } else if (*itr == supportedInterfaces[2]) {
            visibilitySensorNodeType.addExposedField(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                &VisibilitySensor::processSet_size,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<VisibilitySensor, sfvec3f>
                                    (&VisibilitySensor::size)));
        } else if (*itr == supportedInterfaces[3]) {
            visibilitySensorNodeType.addEventOut(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<VisibilitySensor, sftime>
                                    (&VisibilitySensor::enterTime)));
        } else if (*itr == supportedInterfaces[4]) {
            visibilitySensorNodeType.addEventOut(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<VisibilitySensor, sftime>
                                    (&VisibilitySensor::exitTime)));
        } else if (*itr == supportedInterfaces[5]) {
            visibilitySensorNodeType.addEventOut(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<VisibilitySensor, sfbool>
                                    (&VisibilitySensor::active)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class VisibilitySensor
 *
 * @brief VisibilitySensor node instances.
 */

/**
 * @var VisibilitySensor::VisibilitySensorClass
 *
 * @brief Class object for VisibilitySensor instances.
 */

/**
 * @var VisibilitySensor::center
 *
 * @brief center exposedField.
 */

/**
 * @var VisibilitySensor::enabled
 *
 * @brief enabled exposedField.
 */

/**
 * @var VisibilitySensor::size
 *
 * @brief size exposedField.
 */

/**
 * @var VisibilitySensor::active
 *
 * @brief isActive eventOut.
 */

/**
 * @var VisibilitySensor::enterTime
 *
 * @brief enterTime eventOut.
 */

/**
 * @var VisibilitySensor::exitTime
 *
 * @brief exitTime eventOut.
 */

/**
 * @brief Constructor.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
VisibilitySensor::VisibilitySensor(const node_type & type,
                                   const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope),
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
 * @brief Destructor.
 */
VisibilitySensor::~VisibilitySensor() throw ()
{}

/**
 * @brief Generate visibility events.
 *
 * This is in a render() method since the it needs to be computed
 * with respect to the accumulated transformations above it in the
 * scene graph. Move to update() when xforms are accumulated in Groups...
 */
void VisibilitySensor::render(OpenVRML::viewer & viewer,
                              const rendering_context context)
{
    using OpenVRML_::fpzero;

    if (this->enabled.value) {
        sftime timeNow(browser::current_time());
        float xyz[2][3];

        // hack: enclose box in a sphere...
        xyz[0][0] = this->center.value.x();
        xyz[0][1] = this->center.value.y();
        xyz[0][2] = this->center.value.z();
        xyz[1][0] = this->center.value.x() + this->size.value.x();
        xyz[1][1] = this->center.value.y() + this->size.value.y();
        xyz[1][2] = this->center.value.z() + this->size.value.z();
        viewer.transform_points(2, &xyz[0][0]);
        float dx = xyz[1][0] - xyz[0][0];
        float dy = xyz[1][1] - xyz[0][1];
        float dz = xyz[1][2] - xyz[0][2];
        float r  = dx * dx + dy * dy + dz * dz;
        if (!fpzero(r)) { r = sqrt(r); }

        // Was the sphere visible last time through? How does this work
        // for USE'd nodes? I need a way for each USE to store whether
        // it was active.
        bool wasIn = this->active.value;

        // Is the sphere visible? ...
        bool inside = xyz[0][2] < 0.0; // && z > - scene->visLimit()
        if (inside) {
            NavigationInfo * ni = this->type._class.browser
                                    .bindable_navigation_info_top();
            if (ni && !fpzero(ni->getVisibilityLimit())
                    && xyz[0][2] < -(ni->getVisibilityLimit())) {
                inside = false;
            }
        }

        // This bit assumes 90degree fieldOfView to get rid of trig calls...
        if (inside) {
            inside = fabs(xyz[0][0]) < -0.5 * xyz[0][2] + r
                    && fabs(xyz[0][1]) < -0.5 * xyz[0][2] + r;
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
void VisibilitySensor::processSet_center(const field_value & value,
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
void VisibilitySensor::processSet_enabled(const field_value & value,
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
void VisibilitySensor::processSet_size(const field_value & value,
                                       const double timestamp)
    throw (std::bad_cast)
{
    this->size = dynamic_cast<const sfvec3f &>(value);
    this->node::modified(true);
    this->emit_event("size_changed", this->size, timestamp);
}


/**
 * @class WorldInfoClass
 *
 * @brief Class object for WorldInfo nodes.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this NodeClass.
 */
WorldInfoClass::WorldInfoClass(OpenVRML::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
WorldInfoClass::~WorldInfoClass() throw ()
{}

/**
 * @brief Create a NodeType.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    the interfaces for the new NodeType.
 *
 * @return a node_type_ptr to a node_type capable of creating WorldInfo nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by WorldInfoClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_type_ptr
WorldInfoClass::create_type(const std::string & id,
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
    const node_type_ptr type(new Vrml97NodeTypeImpl<WorldInfo>(*this, id));
    Vrml97NodeTypeImpl<WorldInfo> & worldInfoNodeType =
            static_cast<Vrml97NodeTypeImpl<WorldInfo> &>(*type);
    typedef Vrml97NodeTypeImpl<WorldInfo>::NodeFieldPtrPtr NodeFieldPtrPtr;
    for (node_interface_set::const_iterator itr(interfaces.begin());
            itr != interfaces.end(); ++itr) {
        if (*itr == supportedInterfaces[0]) {
            worldInfoNodeType.addField(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<WorldInfo, mfstring>
                                    (&WorldInfo::info)));
        } else if (*itr == supportedInterfaces[1]) {
            worldInfoNodeType.addField(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                NodeFieldPtrPtr(new NodeFieldPtrImpl<WorldInfo, sfstring>
                                    (&WorldInfo::title)));
        } else {
            throw unsupported_interface("Invalid interface.");
        }
    }
    return type;
}

/**
 * @class WorldInfo
 *
 * @brief WorldInfo node instances.
 */

/**
 * @var WorldInfo::WorldInfoClass
 *
 * @brief Class object for WorldInfo instances.
 */

/**
 * @var WorldInfo::info
 *
 * @brief string field.
 */

/**
 * @var WorldInfo::title
 *
 * @brief title field.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
WorldInfo::WorldInfo(const node_type & type,
                     const scope_ptr & scope):
    node(type, scope),
    AbstractChild(type, scope)
{}

/**
 * @brief Destructor.
 */
WorldInfo::~WorldInfo() throw ()
{}

} // namespace Vrml97Node

} // namespace OpenVRML
