// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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
# include <sstream>
# include "node.h"
# include "scope.h"
# include "browser.h"

namespace openvrml {

/**
 * @class unsupported_interface
 *
 * @brief Exception to indicate that a node interface is not supported.
 *
 * This exception is thrown by node::field and node::process_event to
 * indicate that the node doesn't support the interface through which the
 * caller is trying to modify the node. It is also thrown by
 * node_class::create_type if the class object doesn't support an interface
 * specified in the node_interface_set given to that method.
 */

/**
 * @brief Construct.
 *
 * @param message   An informative error message.
 */
unsupported_interface::unsupported_interface(const std::string & message):
    std::runtime_error(message)
{}

/**
 * @brief Construct.
 *
 * @param type          the node type.
 * @param interface_id  the name of the interface that is not available.
 */
unsupported_interface::unsupported_interface(const node_type & type,
                                             const std::string & interface_id):
    std::runtime_error(type.id + " has no interface \"" + interface_id + '"')
{}

namespace {
    const std::string to_string(const node_interface::type_id interface_type)
    {
        std::ostringstream out;
        out << interface_type;
        return out.str();
    }
}

/**
 * @brief Construct.
 *
 * @param type              the node type.
 * @param interface_type    the type of the interface that is not available.
 * @param interface_id      the name of the interface that is not available.
 */
unsupported_interface::unsupported_interface(
    const node_type & type,
    const node_interface::type_id interface_type,
    const std::string & interface_id):
    std::runtime_error(type.id + " has no " + to_string(interface_type) + " \""
                       + interface_id + '"')
{}

/**
 * @brief Destroy.
 */
unsupported_interface::~unsupported_interface() throw ()
{}


/**
 * @class node_interface
 *
 * @brief Type information for an interface of a node.
 */

/**
 * @enum node_interface::type_id
 *
 * @brief Identify the type of interface.
 */

/**
 * @var node_interface::type_id node_interface::invalid_type_id
 *
 * @brief Used to indicate an invalid or unknown interface type.
 */

/**
 * @var node_interface::type_id node_interface::eventin_id
 *
 * @brief An eventIn.
 */

/**
 * @var node_interface::type_id node_interface::eventout_id
 *
 * @brief An eventOut.
 */

/**
 * @var node_interface::type_id node_interface::exposedfield_id
 *
 * @brief An exposedField.
 */

/**
 * @var node_interface::type_id node_interface::field_id
 *
 * @brief A field.
 */

namespace {
    const char * const node_interface_type_id_[] = {
        "<invalid interface type>",
        "eventIn",
        "eventOut",
        "exposedField",
        "field"
    };
}

/**
 * @brief Stream inserter.
 *
 * @relates node_interface
 *
 * If @p type is node_interface::invalidType, @c failbit is set on @p out.
 *
 * @param out   an output stream.
 * @param type  a node interface type.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out,
                          const node_interface::type_id type)
{
    if (type == node_interface::invalid_type_id) {
        out.setstate(std::ios_base::failbit);
    } else {
        out << node_interface_type_id_[type];
    }
    return out;
}

/**
 * @brief Stream extractor.
 *
 * @relates node_interface
 *
 * @param in    an input stream.
 * @param type  a node interface type.
 *
 * @return @p in.
 */
std::istream & operator>>(std::istream & in, node_interface::type_id & type)
{
    using std::find;
    using std::string;

    string interface_type_id;
    in >> interface_type_id;

    static const char * const * const begin =
            node_interface_type_id_ + node_interface::eventin_id;
    static const char * const * const end =
            node_interface_type_id_ + node_interface::field_id + 1;
    const char * const * const pos = find(begin, end, interface_type_id);
    if (pos != end) {
        type = node_interface::type_id(pos - begin);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}


/**
 * @var node_interface::type
 *
 * @brief The interface type.
 */

/**
 * @var node_interface::field_type
 *
 * @brief The field data type handled by this interface.
 */

/**
 * @var node_interface::id
 *
 * @brief The name of this interface.
 */

/**
 * @brief Constructor.
 *
 * @param type       the type of interface.
 * @param field_type the field data type handled by the interface.
 * @param id         the name of the interface.
 */
node_interface::node_interface(const type_id type,
                               const field_value::type_id field_type,
                               const std::string & id):
    type(type),
    field_type(field_type),
    id(id)
{}

/**
 * @fn bool operator==(const node_interface & lhs, const node_interface & rhs) throw ()
 *
 * @relates openvrml::node_interface
 *
 * @brief Compare for equality.
 *
 * @param lhs   a node_interface.
 * @param rhs   a node_interface.
 *
 * @return @c true if the two node_interfaces are equal, @c false otherwise.
 */

/**
 * @fn bool operator!=(const node_interface & lhs, const node_interface & rhs) throw ()
 *
 * @relates openvrml::node_interface
 *
 * @brief Compare for openvrml::inequality.
 *
 * @param lhs   a node_interface.
 * @param rhs   a node_interface.
 *
 * @return @c true if the two node_interfaces are equal, @c false otherwise.
 */

/**
 * @class node_interface_set
 *
 * @brief A group of unique @link node_interface node_interfaces@endlink.
 *
 * node_interface_sets are used to construct new
 * @link node_type node_types@endlink. node_type objects also expose their
 * interfaces as a node_interface_set. The interfaces in a node_interface_set
 * are guaranteed to be unique and non-conflicting.
 */



/**
 * @internal
 *
 * @struct node_interface_set::id_less
 *
 * @brief A functor for ordering @link node_interface node_interfaces@endlink
 *        in the set.
 *
 * Compares the @a id's of two @link node_interface node_interfaces@endlink
 * using @c operator<.
 */

/**
 * @fn bool node_interface_set::id_less::operator()(const node_interface & lhs, const node_interface & rhs) const
 *
 * @brief Simple lexicographic comparison of the @a id's.
 *
 * @return @c true if @p lhs.id is lexicographically less than @p rhs.id,
 *      @c false otherwise.
 */

/**
 * @internal
 *
 * @var std::set<node_interface, node_interface_set::id_less> node_interface_set::interfaces
 *
 * @brief Set of @link node_interface node_interfaces@endlink.
 */

/**
 * @typedef node_interface_set::const_iterator
 *
 * @brief An STL const_iterator.
 */

/**
 * @brief Add an interface.
 *
 * @param interface a node_interface.
 *
 * @exception std::invalid argument if @p nodeInterface conflicts with an interface
 *                              already in the node_interface_set.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void node_interface_set::add(const node_interface & interface)
    throw (std::invalid_argument, std::bad_alloc)
{
    if (!this->interfaces.insert(interface).second) {
        throw std::invalid_argument("Interface conflicts with an interface "
                                    "already in this set.");
    }
}

/**
 * @fn node_interface_set::const_iterator node_interface_set::begin() const throw ()
 *
 * @brief Returns an iterator to the beginning of the node_interface_set.
 *
 * @return a const_iterator pointing to the first node_interface in the set.
 */

/**
 * @fn node_interface_set::const_iterator node_interface_set::end() const throw ()
 *
 * @brief Returns an iterator to the end of the node_interface_set.
 *
 * @return a const_iterator pointing to one increment past the last
 *       node_interface in the set.
 */

namespace {

    /**
     * @internal
     */
    struct interface_id_matches_ :
            std::unary_function<openvrml::node_interface, bool> {
        explicit interface_id_matches_(const std::string & interface_id):
            interface_id(&interface_id)
        {}

        bool operator()(const openvrml::node_interface & interface) const
        {
            static const char eventin_prefix[] = "set_";
            static const char eventout_suffix[] = "_changed";

            return interface.id == *this->interface_id
                || (interface.type == node_interface::exposedfield_id
                    && (eventin_prefix + interface.id == *this->interface_id
                        || (interface.id + eventout_suffix
                            == *this->interface_id)))
                || (interface.type == node_interface::eventin_id
                    && interface.id == eventin_prefix + *this->interface_id)
                || (interface.type == node_interface::eventout_id
                    && interface.id == *this->interface_id + eventout_suffix);
        }

    private:
        const std::string * interface_id;
    };
}

/**
 * @brief Find an interface matching @p id.
 *
 * If no interface is found with an interface identifier that is an exact match
 * for @p id, this method will look for @c set_ and @c _changed variants.
 *
 * @param id    the interface id to look for.
 *
 * @return a const_iterator to the interface, or node_interface_set::end if no
 *      interface is found.
 */
node_interface_set::const_iterator
node_interface_set::find(const std::string & id) const throw ()
{
    return std::find_if(this->begin(), this->end(), interface_id_matches_(id));
}


/**
 * @class node_class
 *
 * @brief A class object for node instances.
 *
 * node_class can be thought of as a "supertype" of sorts. A given node
 * implementation can support as many node types as there are unique
 * combinations of the interfaces it supports. The most readily apparent
 * role of the node_class object for a node implementation is to serve as
 * a factory for these @link node_type node_types@endlink.
 */

/**
 * @var openvrml::browser & node_class::browser
 *
 * @brief The browser associated with this node_class.
 */

/**
 * @brief Constructor.
 *
 * A node_class is constructed using a browser. All node instances that share
 * a particular node_class "belong to" the browser associated with the
 * node_class.
 *
 * @param b the browser to be associated with the node_class.
 */
node_class::node_class(openvrml::browser & b) throw ():
    browser(b)
{}

/**
 * @brief Destructor.
 */
node_class::~node_class() throw ()
{}

/**
 * @brief node_class-specific initialization.
 *
 * This method is called during initialization of a browser object with a new
 * root Scene. It is called after the individual node instances have been
 * initialized, and before the world starts running.
 *
 * @param initial_viewpoint the viewpoint_node that should be bound initially;
 *                          or 0 if the default viewpoint_node should be bound.
 * @param time              the current time.
 */
void node_class::initialize(viewpoint_node * initial_viewpoint,
                            const double time)
    throw ()
{}

/**
 * @brief node_class-specific rendering.
 *
 * The default implementation of this method does nothing.
 *
 * @param viewer    the viewer to render to.
 */
void node_class::render(openvrml::viewer & viewer) throw ()
{}

/**
 * @fn const node_type_ptr node_class::create_type(const std::string & id, const node_interface_set & interfaces) throw (std::invalid_argument, std::bad_alloc)
 *
 * @brief Create a new node_type.
 *
 * @link node_type node_types@endlink can be said to subset the master type
 * provided by the node_class. Each node_class instance can support certain
 * node interfaces; the node_interface_set passed to createType must be a
 * subset of those supported interfaces.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    a node_interface_set containing the interfaces
 *                      for the new type.
 *
 * @return a node_type_ptr to the newly created node_type.
 *
 * @exception std::invalid_argument if the node_class cannot support one of the
 *                              @link node_interface node_interfaces@endlink in
 *                              @p interfaces.
 * @exception std::bad_alloc        if memory allocation fails.
 */


/**
 * @class node_type
 *
 * @brief Type information object for @link openvrml::node nodes@endlink.
 */

/**
 * @var openvrml::node_class & node_type::node_class
 *
 * @brief The class object associated with the node_type.
 */

/**
 * @var const std::string node_type::id
 *
 * @brief The name of the node_type.
 */

/**
 * @brief Constructor.
 *
 * @param c     the class object associated with the node_type.
 * @param id    the name for the node_type.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
node_type::node_type(openvrml::node_class & c, const std::string & id)
    throw (std::bad_alloc):
    node_class(c),
    id(id)
{}

/**
 * @brief Destructor.
 */
node_type::~node_type() throw ()
{}

namespace {
    struct IsEventIn_ : public std::unary_function<node_interface, bool> {
    private:
        const std::string & id;

    public:
        IsEventIn_(const std::string & id): id(id) {}

        bool operator()(const node_interface & nodeInterface) const
        {
            return (nodeInterface.type == node_interface::eventin_id
                        && (this->id == nodeInterface.id
                            || "set_" + this->id == nodeInterface.id))
                    || (nodeInterface.type == node_interface::exposedfield_id
                        && (this->id == nodeInterface.id
                            || this->id == "set_" + nodeInterface.id));
        }
    };
}

/**
 * @brief Determine if the node type has an eventIn.
 *
 * If the node type supports an eventIn named @p id, this method will return
 * the type identifier corresponding to the data type handled by the eventIn.
 * Note that the actual interface type that supports the eventIn may be either
 * an eventIn or an exposedField.
 *
 * @param id    the name of the eventIn.
 *
 * @return the data type of the eventIn, or field_value::invalid_type_id if no
 *         such eventIn exists.
 */
field_value::type_id node_type::has_eventin(const std::string & id) const
    throw ()
{
    const node_interface_set & interfaces = this->interfaces();
    const node_interface_set::const_iterator end = interfaces.end();
    const node_interface_set::const_iterator pos =
            std::find_if(interfaces.begin(), end, IsEventIn_(id));
    if (pos == end) { return field_value::invalid_type_id; }
    return pos->field_type;
}

namespace {
    struct IsEventOut_ : public std::unary_function<node_interface, bool> {
    private:
        const std::string & id;

    public:
        IsEventOut_(const std::string & id): id(id) {}

        bool operator()(const node_interface & nodeInterface) const
        {
            return (nodeInterface.type == node_interface::eventout_id
                        && (this->id == nodeInterface.id
                            || this->id + "_changed" == nodeInterface.id))
                    || (nodeInterface.type == node_interface::exposedfield_id
                        && (this->id == nodeInterface.id
                            || this->id == nodeInterface.id + "_changed"));
        }
    };
}

/**
 * @brief Determine if the node type has an eventOut.
 *
 * If the node type supports an eventOut named @p id, this method will return
 * the type identifier corresponding to the data type handled by the eventOut.
 * Note that the actual interface type that supports the eventOut may be either
 * an eventOut or an exposedField.
 *
 * @param id    the name of the eventOut.
 *
 * @return the data type of the eventOut, or field_value::invalid_type_id if no
 *         such eventOut exists.
 */
field_value::type_id node_type::has_eventout(const std::string & id) const
    throw ()
{
    const node_interface_set & interfaces = this->interfaces();
    const node_interface_set::const_iterator end = interfaces.end();
    const node_interface_set::const_iterator pos =
            std::find_if(interfaces.begin(), end, IsEventOut_(id));
    if (pos == end) { return field_value::invalid_type_id; }
    return pos->field_type;
}

namespace {
    struct IsExposedField_ : public std::unary_function<node_interface, bool> {
    private:
        const std::string & id;

    public:
        IsExposedField_(const std::string & id): id(id) {}

        bool operator()(const node_interface & nodeInterface) const
        {
            return nodeInterface.type == node_interface::exposedfield_id
                    && this->id == nodeInterface.id;
        }
    };
}

/**
 * @brief Determine if the node type has an exposedField.
 *
 * If the node type has an exposedField named @p id, this method will return
 * the type identifier corresponding to the data type handled by the
 * exposedField.
 *
 * @param id    the name of the exposedField.
 *
 * @return the data type of the exposedField, or field_value::invalid_type_id
 *         if no such exposedField exists.
 */
field_value::type_id node_type::has_exposedfield(const std::string & id) const
    throw ()
{
    const node_interface_set & interfaces = this->interfaces();
    const node_interface_set::const_iterator end = interfaces.end();
    const node_interface_set::const_iterator pos =
            std::find_if(interfaces.begin(), end, IsExposedField_(id));
    if (pos == end) { return field_value::invalid_type_id; }
    return pos->field_type;
}

namespace {
    struct IsField_ : public std::unary_function<node_interface, bool> {
    private:
        const std::string & id;

    public:
        IsField_(const std::string & id): id(id) {}

        bool operator()(const node_interface & nodeInterface) const
        {
            return nodeInterface.type == node_interface::field_id
                    && this->id == nodeInterface.id;
        }
    };
}

/**
 * @brief Determine if the node type has a field.
 *
 * If the node type has a field named @p id, this method will return the type
 * identifier corresponding to the data type accepted by the field.
 *
 * @param id    the name of the field.
 *
 * @return the data type of the field, or field_value::invalid_type_id if no
 *         such field exists.
 */
field_value::type_id node_type::has_field(const std::string & id) const
    throw ()
{
    const node_interface_set & interfaces = this->interfaces();
    const node_interface_set::const_iterator end = interfaces.end();
    const node_interface_set::const_iterator pos =
            std::find_if(interfaces.begin(), end, IsField_(id));
    if (pos == end) { return field_value::invalid_type_id; }
    return pos->field_type;
}

/**
 * @fn const node_interface_set & node_type::interfaces() const throw ()
 *
 * @brief Get the set of interfaces for the node_type.
 *
 * @return the set of interfaces.
 */

/**
 * @fn const node_ptr node_type::create_node(const scope_ptr & scope) const throw (std::bad_alloc)
 *
 * @brief Create a new node with this node_type.
 *
 * @param scope         the Scope that the new node should belong to.
 *
 * @return a node_ptr to a new node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */


/**
 * @class field_value_type_mismatch
 *
 * @brief Thrown when field value types do not match, generally in a @c ROUTE
 *      or @c IS.
 */

/**
 * @brief Constructor.
 */
field_value_type_mismatch::field_value_type_mismatch():
    std::runtime_error("Field value types do not match.")
{}

/**
 * @brief Destructor.
 */
field_value_type_mismatch::~field_value_type_mismatch() throw ()
{}


/**
 * @typedef node_path
 *
 * @brief A path to a node in the scene starting with one of the scene root
 *      nodes and ending with the objective node.
 */


/**
 * @class node
 *
 * @brief A node in the scene graph.
 */

/**
 * @class node::route
 *
 * @brief A route from one node to another through which events propagate.
 */

/**
 * @var const std::string node::route::from_eventout
 *
 * @brief The name of the eventOut the route is coming from.
 */

/**
 * @var const node_ptr node::route::to_node
 *
 * @brief The node the route is going to.
 */

/**
 * @var const std::string node::route::to_eventin
 *
 * @brief The name of the eventIn on @a to_node that the route is going to.
 */

/**
 * @brief Constructor.
 *
 * @param from_eventout the name of the eventOut the route is coming from.
 * @param to_node       the node the route is going to.
 * @param to_eventin    the name of an eventIn on @p toNode that the route is
 *                      going to.
 */
node::route::route(const std::string & from_eventout,
                   const node_ptr & to_node, const std::string & to_eventin):
    from_eventout(from_eventout),
    to_node(to_node),
    to_eventin(to_eventin)
{}

/**
 * @brief Copy constructor.
 *
 * @param route the route to copy.
 */
node::route::route(const route & route):
    from_eventout(route.from_eventout),
    to_node(route.to_node),
    to_eventin(route.to_eventin)
{}

/**
 * @fn bool operator==(const node::route & lhs, const node::route & rhs)
 *
 * @relates openvrml::node::route
 *
 * @brief Compare two @link node::route routes@endlink for equality.
 *
 * @param lhs   a route.
 * @param rhs   a route.
 *
 * @return @c true if the @link node::route routes@endlink are identical,
 *         @c false otherwise.
 */

/**
 * @fn bool operator!=(const node::route & lhs, const node::route & rhs)
 *
 * @relates openvrml::node::route
 *
 * @brief Compare two @link node::route routes@endlink for inequality.
 *
 * @param lhs   a route.
 * @param rhs   a route.
 *
 * @return @c true if the @link node::route routes@endlink are not identical,
 *         @c false otherwise.
 */

/**
 * @typedef node::routes_t
 *
 * @brief list of @link node::route routes@endlink.
 */

/**
 * @struct node::polled_eventout_value
 *
 * @brief Simple struct for use in implementing nodes that are polled for
 *      pending events.
 */

/**
 * @var const field_value_ptr node::polled_eventout_value::value
 *
 * @brief The value.
 */

/**
 * @var bool node::polled_eventout_value::modified
 *
 * @brief A flag to indicate whether the eventOut has been modified.
 */

/**
 * @brief Construct.
 */
node::polled_eventout_value::polled_eventout_value():
    modified(false)
{}

/**
 * @brief Construct.
 *
 * @param value     the value.
 * @param modified  a flag to indicate whether the eventOut has been modified.
 */
node::polled_eventout_value::polled_eventout_value(
    const field_value_ptr & value,
    const bool modified):
    value(value),
    modified(modified)
{}

/**
 * @internal
 *
 * @var scope_ptr node::scope_
 *
 * @brief The scope to which the node belongs.
 */

/**
 * @internal
 *
 * @var scene * node::scene_
 *
 * @brief The scene with which the node is associated.
 */

/**
 * @internal
 *
 * @var bool node::modified_
 *
 * @brief Indicate whether the node has been modified.
 *
 * @see node::modified
 */

/**
 * @internal
 *
 * @var bool node::bounding_volume_dirty_
 *
 * @brief Indicate whether the node's cached bounding volume needs updating.
 *
 * @see node::bounding_volume_dirty
 */

/**
 * @internal
 *
 * @var node::routes_t node::routes_
 *
 * @brief @link node::route routes@endlink from the node.
 */

/**
 * @internal
 *
 * @typedef node::eventout_is_map_t
 *
 * @brief map of eventOut identifiers to
 *        @link node::polled_eventout_value polled_eventout_values@endlink.
 */

/**
 * @internal
 *
 * @var node::eventout_is_map_t node::eventout_is_map
 *
 * @brief map of eventOut identifiers to
 *        @link node::polled_eventout_value polled_eventout_values@endlink.
 *
 * This is an ugly artifact of the PROTO implementation. When an eventOut of
 * a node in a PROTO definition is IS'd to an eventOut in the PROTO definition,
 * an entry is added to this map. When emitting an event, the node checks to
 * see if any entries in the map need updating, thereby propagating the event
 * out of the PROTO instance.
 */

/**
 * @var node::type
 *
 * @brief The type information object for the node.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the Scope associated with the instance.
 */
node::node(const node_type & type, const scope_ptr & scope) throw ():
    scope_(scope),
    scene_(0),
    modified_(false),
    bounding_volume_dirty_(false),
    type(type)
{}

/**
 * @internal
 *
 * @fn node::node(const node &)
 *
 * @brief Construct a copy.
 *
 * Not implemented. node is not copyable.
 */

namespace {

    /**
     * @internal
     *
     * @brief Map of node names to node instances.
     */
    typedef std::map<std::string, node *> named_node_map_t;

    struct node_is_ : std::unary_function<named_node_map_t::value_type, bool> {
        explicit node_is_(const node & n):
            n(&n)
        {}

        bool operator()(const named_node_map_t::value_type & value) const
            throw ()
        {
            return value.second == this->n;
        }

    private:
        const node * n;
    };
}

/**
 * @brief Destructor.
 *
 * Remove node name (if any) from the scope.
 */
node::~node() throw ()
{
    //
    // If this is the primordial node in a prototype definition, this->scope_
    // will be null.
    //
    if (this->scope_) {
        using std::find_if;
        const named_node_map_t::iterator end =
            this->scope_->named_node_map.end();
        const named_node_map_t::iterator pos =
            find_if(this->scope_->named_node_map.begin(), end,
                    node_is_(*this));
        if (pos != end) { this->scope_->named_node_map.erase(pos); }
    }
}

/**
 * @internal
 *
 * @fn node & node::operator=(const node &)
 *
 * @brief Assign.
 *
 * Not implemented. node is not copyable.
 */

/**
 * @brief Set the name of the node.
 *
 * @param node_id the name for the node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node::id(const std::string & node_id) throw (std::bad_alloc)
{
    assert(this->scope_);
    this->scope_->named_node_map[node_id] = this; // Throws std::bad_alloc.
}

/**
 * @brief Retrieve the name of this node.
 *
 * @return the node name.
 */
const std::string & node::id() const throw ()
{
    using std::find_if;
    using std::string;

    assert(this->scope_);

    const named_node_map_t::iterator end = this->scope_->named_node_map.end();
    const named_node_map_t::iterator pos =
        find_if(this->scope_->named_node_map.begin(), end, node_is_(*this));
    static const string empty;
    return (pos != end) ? pos->first : empty;
}

/**
 * @fn const scope_ptr & node::scope() const throw ()
 *
 * @brief Get the scope to which the node belongs.
 *
 * @return the scope to which the node belongs.
 */

/**
 * @fn scene * node::scene() const throw ()
 *
 * @brief Get the scene with which the node is associated.
 *
 * @return the scene with which the node is associated.
 */

/**
 * @brief Add a polled eventOut value.
 *
 * Used internally by the PROTO implementation.
 *
 * @param eventout_id    eventOut name.
 * @param eventout_value a polled_eventout_value.
 *
 * @exception unsupported_interface if the node has no eventOut @p eventout_id.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void node::add_eventout_is(const std::string & eventout_id,
                           polled_eventout_value & eventout_value)
    throw (unsupported_interface, std::bad_alloc)
{
    if (!this->type.has_eventout(eventout_id)) {
        throw unsupported_interface(this->type,
                                    node_interface::eventout_id,
                                    eventout_id);
    }
    const eventout_is_map_t::value_type value(eventout_id, &eventout_value);
    this->eventout_is_map.insert(value);
}

/**
 * @brief Initialize the node.
 *
 * This method works recursively, initializing any child nodes to the same
 * @p scene and @p timestamp. If the node has already been initialized, this
 * method has no effect.
 *
 * @param scene     the Scene to which the node will belong.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post @a scene points to @p scene.
 */
void node::initialize(openvrml::scene & scene, const double timestamp)
    throw (std::bad_alloc)
{
    if (!this->scene_) {
        this->scene_ = &scene;
        this->do_initialize(timestamp);

        const node_interface_set & interfaces = this->type.interfaces();
        for (node_interface_set::const_iterator interface(interfaces.begin());
                interface != interfaces.end(); ++interface) {
            if (interface->type == node_interface::exposedfield_id
                    || interface->type == node_interface::field_id) {
                if (interface->field_type == field_value::sfnode_id) {
                    assert(dynamic_cast<const sfnode *>
                           (&this->field(interface->id)));
                    const sfnode & sfn = static_cast<const sfnode &>
                                         (this->field(interface->id));
                    if (sfn.value) { sfn.value->initialize(scene, timestamp); }
                } else if (interface->field_type == field_value::mfnode_id) {
                    assert(dynamic_cast<const mfnode *>
                           (&this->field(interface->id)));
                    const mfnode & mfn = static_cast<const mfnode &>
                                         (this->field(interface->id));
                    for (size_t i = 0; i < mfn.value.size(); ++i) {
                        if (mfn.value[i]) {
                            mfn.value[i]->initialize(scene, timestamp);
                        }
                    }
                }
            }
        }
    }
    assert(this->scene_ == &scene);
}

/**
 * @brief Called when the node is relocated to a new position in the scene
 *      graph.
 *
 * This function delegates to the virtual function do_relocate. relocate
 * should be called by eventIn handlers that receive nodes.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node::relocate() throw (std::bad_alloc)
{
    typedef void (node::* Do_relocate)();

    class RelocateTraverser : public node_traverser {
        Do_relocate do_relocate;

    public:
        explicit RelocateTraverser(const Do_relocate do_relocate) throw ():
            do_relocate(do_relocate)
        {}

        virtual ~RelocateTraverser() throw ()
        {}

    private:
        virtual void on_entering(node & n) throw (std::bad_alloc)
        {
            (n.*this->do_relocate)();
        }
    };

    Do_relocate do_reloc = &node::do_relocate;

    RelocateTraverser(do_reloc).traverse(*this);
}

/**
 * @brief Generalized field mutator.
 *
 * @param id    the name of the field.
 * @param value the new value.
 *
 * @exception unsupported_interface  if the node has no field named @p id.
 * @exception std::bad_cast         if @p value is not the correct type.
 * @exception std::bad_alloc        if memory allocation fails.
 *
 * @pre @p value must be the appropriate type for the interface.
 */
void node::field(const std::string & id, const field_value & value)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    this->do_field(id, value);
}

/**
 * @fn void node::do_field(const std::string & id, const field_value & value) throw (unsupported_interface, std::bad_cast, std::bad_alloc)
 *
 * @brief Called by node::field to set a field.
 *
 * @param id    field identifier.
 * @param value value.
 *
 * @exception unsupported_interface if the node has no field @p id.
 * @exception std::bad_cast         if @p value is not the correct type.
 * @exception std::bad_alloc        if memory allocation fails.
 */

/**
 * @brief Generalized field accessor.
 *
 * @param id    the name of the field.
 *
 * @return the field value.
 *
 * @exception unsupported_interface if the node has no field named @p id.
 */
const field_value & node::field(const std::string & id) const
    throw (unsupported_interface)
{
    return this->do_field(id);
}

/**
 * @fn const field_value & node::do_field(const std::string & id) const throw (unsupported_interface)
 *
 * @brief Called by node::field to get a field.
 *
 * @param id    field identifier.
 *
 * @return the field value.
 *
 * @exception unsupported_interface if the node has no field @p id.
 */

/**
 * @brief Process an event.
 *
 * @param id        the name of the eventIn to which the event is being sent.
 * @param value     the new value.
 * @param timestamp the current time.
 *
 * @exception unsupported_interface if the node has no eventIn named @p id.
 * @exception std::bad_cast         if @p value is not the correct type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void node::process_event(const std::string & id,
                         const field_value & value,
                         const double timestamp)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    this->do_process_event(id, value, timestamp);
}

/**
 * @fn void node::do_process_event(const std::string & id, const field_value & value, double timestamp) throw (unsupported_interface, std::bad_cast, std::bad_alloc)
 *
 * @brief Called by node::process_event.
 *
 * @param id        eventIn identifier.
 * @param value     event value.
 * @param timestamp the current time.
 *
 * @exception unsupported_interface if the node has no eventIn @p id.
 * @exception std::bad_cast         if @p value is not the correct time.
 * @exception std::bad_alloc        if memory allocation fails.
 */

/**
 * @brief Generalized eventOut accessor.
 *
 * @param id    the name of the eventOut.
 *
 * @exception unsupported_interface if the node has no eventOut named @p id.
 */
const field_value & node::eventout(const std::string & id) const
    throw (unsupported_interface)
{
    return this->do_eventout(id);
}

/**
 * @fn const field_value & node::do_eventout(const std::string & id) const throw (unsupported_interface)
 *
 * @brief Called by node::eventout.
 *
 * @param id    eventOut identifier.
 *
 * @return the last value sent from the eventOut.
 *
 * @exception unsupported_interface if the node has no eventOut @p id.
 */

/**
 * @brief Shut down the node.
 *
 * This method works recursively, shutting down any child nodes. If the node
 * has already been shut down, this method has no effect.
 *
 * @param timestamp the current time.
 *
 * @post @a scene is 0.
 */
void node::shutdown(const double timestamp) throw ()
{
    if (this->scene_) {
        this->do_shutdown(timestamp);
        this->scene_ = 0;

        const node_interface_set & interfaces = this->type.interfaces();
        for (node_interface_set::const_iterator interface(interfaces.begin());
                interface != interfaces.end(); ++interface) {
            if (interface->type == node_interface::exposedfield_id
                    || interface->type == node_interface::field_id) {
                if (interface->field_type == field_value::sfnode_id) {
                    assert(dynamic_cast<const sfnode *>
                           (&this->field(interface->id)));
                    const sfnode & sfn = static_cast<const sfnode &>
                                         (this->field(interface->id));
                    if (sfn.value) { sfn.value->shutdown(timestamp); }
                } else if (interface->field_type == field_value::mfnode_id) {
                    assert(dynamic_cast<const mfnode *>
                           (&this->field(interface->id)));
                    const mfnode & mfn = static_cast<const mfnode &>
                                         (this->field(interface->id));
                    for (size_t i = 0; i < mfn.value.size(); ++i) {
                        if (mfn.value[i]) {
                            mfn.value[i]->shutdown(timestamp);
                        }
                    }
                }
            }
        }
    }
    assert(!this->scene_);
}

/**
 * @fn void node::do_shutdown(double timestamp) throw ()
 *
 * @brief Called by node::shutdown.
 *
 * @param timestamp the current time.
 */

/**
 * @brief Cast to a const script_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const script_node * node::to_script() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a script_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
script_node * node::to_script() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const appearance_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const appearance_node * node::to_appearance() const throw ()
{
    return 0;
}

/**
 * @brief Cast to an appearance_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
appearance_node * node::to_appearance() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const child_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const child_node * node::to_child() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a child_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
child_node * node::to_child() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const color_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const color_node * node::to_color() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a color_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
color_node * node::to_color() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const CoordinateNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const coordinate_node * node::to_coordinate() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a coordinate_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
coordinate_node * node::to_coordinate() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const FontStyleNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const font_style_node * node::to_font_style() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a font_style_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
font_style_node * node::to_font_style() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const geometry_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const geometry_node * node::to_geometry() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a geometry_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
geometry_node * node::to_geometry() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const grouping_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const grouping_node * node::to_grouping() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a grouping_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
grouping_node * node::to_grouping() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const material_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const material_node * node::to_material() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a material_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
material_node * node::to_material() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const normal_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const normal_node * node::to_normal() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a normal_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
normal_node * node::to_normal() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const sound_source_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const sound_source_node * node::to_sound_source() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a sound_source_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
sound_source_node * node::to_sound_source() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const texture_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const texture_node * node::to_texture() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a texture_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
texture_node * node::to_texture() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const texture_coordinate_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const texture_coordinate_node * node::to_texture_coordinate() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a texture_coordinate_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
texture_coordinate_node * node::to_texture_coordinate() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const texture_transform_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const texture_transform_node * node::to_texture_transform() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a texture_transform_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
texture_transform_node * node::to_texture_transform() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const transform_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const transform_node * node::to_transform() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a transform_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
transform_node * node::to_transform() throw ()
{
    return 0;
}

/**
 * @brief Cast to a const viewpoint_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const viewpoint_node * node::to_viewpoint() const throw ()
{
    return 0;
}

/**
 * @brief Cast to a viewpoint_node.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
viewpoint_node * node::to_viewpoint() throw ()
{
    return 0;
}

/**
 * @brief Cast to an anchor_node.
 *
 * @return 0.
 */
vrml97_node::anchor_node * node::to_anchor() const
{
    return 0;
}

/**
 * @brief Cast to an audio_clip_node.
 *
 * @return 0.
 */
vrml97_node::audio_clip_node * node::to_audio_clip() const
{
    return 0;
}

/**
 * @brief Cast to an abstract_light_node.
 *
 * @return 0.
 */
vrml97_node::abstract_light_node * node::to_light() const
{
    return 0;
}

/**
 * @brief Cast to a movie_texture_node.
 *
 * @return 0.
 */
vrml97_node::movie_texture_node * node::to_movie_texture() const
{
    return 0;
}

/**
 * @brief Cast to a navigation_info_node.
 *
 * @return 0.
 */
vrml97_node::navigation_info_node * node::to_navigation_info() const
{
    return 0;
}

/**
 * @brief Cast to a plane_sensor_node.
 *
 * @return 0.
 */
vrml97_node::plane_sensor_node * node::to_plane_sensor() const
{
    return 0;
}

/**
 * @brief Cast to a sphere_sensor_node.
 *
 * @return 0.
 */
vrml97_node::sphere_sensor_node * node::to_sphere_sensor() const
{
    return 0;
}

/**
 * @brief Cast to a cylinder_sensor_node.
 *
 * @return 0.
 */
vrml97_node::cylinder_sensor_node * node::to_cylinder_sensor() const
{
    return 0;
}

/**
 * @brief Cast to a point_light_node.
 *
 * @return 0.
 */
vrml97_node::point_light_node * node::to_point_light() const
{
    return 0;
}

/**
 * @brief Cast to a spot_light_node.
 *
 * @return 0.
 */
vrml97_node::spot_light_node * node::to_spot_light() const
{
    return 0;
}

/**
 * @brief Cast to a time_sensor_node.
 *
 * @return 0.
 */
vrml97_node::time_sensor_node * node::to_time_sensor() const
{
    return 0;
}

/**
 * @brief Cast to a touch_sensor_node.
 *
 * @return 0.
 */
vrml97_node::touch_sensor_node * node::to_touch_sensor() const
{
    return 0;
}

/**
 * @brief Add a route from an eventOut of this node to an eventIn of another
 *      node.
 *
 * If the route being added already exists, this method has no effect.
 *
 * @param from_eventout an eventOut of the node.
 * @param to_node       a node.
 * @param to_eventin    an eventIn of @p to_node.
 *
 * @exception unsupported_interface     if the node has no eventOut
 *                                      @p from_eventout; or if @p to_node has
 *                                      no eventIn @p to_eventin.
 * @exception field_value_type_mismatch if @p from_eventout and @p to_eventin
 *                                      have different field value types.
 *
 * @pre @p to_node is not null.
 */
void node::add_route(const std::string & from_eventout,
                     const node_ptr & to_node,
                     const std::string & to_eventin)
    throw (unsupported_interface, field_value_type_mismatch)
{
    assert(to_node);

    const field_value::type_id from_interface_type =
        this->type.has_eventout(from_eventout);
    if (from_interface_type == field_value::invalid_type_id) {
        throw unsupported_interface(this->type,
                                    node_interface::eventout_id,
                                    from_eventout);
    }

    const field_value::type_id to_interface_type =
        to_node->type.has_eventin(to_eventin);
    if (to_interface_type == field_value::invalid_type_id) {
        throw unsupported_interface(to_node->type,
                                    node_interface::eventin_id,
                                    to_eventin);
    }

    if (from_interface_type != to_interface_type) {
        throw field_value_type_mismatch();
    }

    const route r(from_eventout, to_node, to_eventin);

    //
    // Is this route already here?
    //
    const routes_t::iterator pos =
        std::find(this->routes_.begin(), this->routes_.end(), r);

    //
    // If not, add it.
    //
    if (pos == this->routes_.end()) { this->routes_.push_back(r); }
}


/**
 * @brief Remove a route from an eventOut of this node to an eventIn of another
 *      node.
 *
 * If no such route exists, this method has no effect.
 *
 * @param from_eventout an eventOut of the node.
 * @param to_node       a node.
 * @param to_eventin    an eventIn of @p to_node.
 */
void node::delete_route(const std::string & from_eventout,
                        const node_ptr & to_node,
                        const std::string & to_eventin) throw ()
{
    const routes_t::iterator pos =
        std::find(this->routes_.begin(), this->routes_.end(),
                  route(from_eventout, to_node, to_eventin));
    if (pos != this->routes_.end()) { this->routes_.erase(pos); }
}

/**
 * @brief Get the routes from this node.
 *
 * @return an std::vector of Routes from this node.
 */
const node::routes_t & node::routes() const
{
    return this->routes_;
}

/**
 * @brief Set the modified flag.
 *
 * Indicates the node needs to be revisited for rendering.
 *
 * @param value
 */
void node::modified(const bool value)
{
    this->modified_ = value;
    if (this->modified_) { this->type.node_class.browser.modified(true); }
}

/**
 * @brief Determine whether the node has been modified.
 *
 * The default implementation returns whether this node has been modified.
 * Subclasses that can have child nodes should override this method and
 * return @c true if any of their children have been modified.
 *
 * @return @c true if the node has been modified; @c false otherwise.
 */
bool node::modified() const
{
    return this->modified_;
}

/**
 * @brief Get this node's bounding volume.
 *
 * Nodes that have no bounding volume, or have a difficult to calculate
 * bvolume (like, say, Extrusion or Billboard) can just return an infinite
 * bsphere. Note that returning an infinite bvolume means that all the node's
 * ancestors will also end up with an infinite bvolume, and will never be
 * culled.
 *
 * @return a maximized bounding volume.
 */
const bounding_volume & node::bounding_volume() const
{
    class default_bounding_volume : public bounding_sphere {
    public:
        default_bounding_volume()
        {
            this->maximize();
        }
    };

    static const default_bounding_volume default_bvolume;

    const_cast<node *>(this)->bounding_volume_dirty(false);
    return default_bvolume;
}


/**
 * Override a node's calculated bounding volume. Not implemented.
 *
 * @todo Implement me!
 */
void node::bounding_volume(const openvrml::bounding_volume & v)
{
    // XXX Implement me!
}

/**
 * Indicate that a node's bounding volume needs to be recalculated
 * (or not). If a node's bvolume is invalid, then the bvolumes of
 * all that node's ancestors are also invalid. Normally, the node
 * itself will determine when its bvolume needs updating.
 */
void node::bounding_volume_dirty(const bool value)
{
    this->bounding_volume_dirty_ = value;
    if (value) { // only if dirtying, not clearing
        this->type.node_class.browser.flags_need_updating = true;
    }
}

/**
 * Return true if the node's bounding volume needs to be
 * recalculated.
 */
bool node::bounding_volume_dirty() const
{
    if (this->type.node_class.browser.flags_need_updating) {
        this->type.node_class.browser.update_flags();
        this->type.node_class.browser.flags_need_updating = false;
    }
    return this->bounding_volume_dirty_;
}

/**
 * @brief Render this node.
 *
 * Actually, most of the rendering work is delegated to the viewer, but this
 * method is responsible for traversal to the node's renderable children,
 * including culling. Each node class needs to implement this routine
 * appropriately. It's not abstract since it doesn't make sense to call render
 * on some nodes. Alternative would be to break render out into a seperate
 * mixins class, but that's probably overkill.
 *
 * @param viewer    viewer implementation responsible for actually doing the
 *                  drawing.
 * @param context   generic context argument; holds things like the accumulated
 *                  modelview transform.
 */
void node::render(openvrml::viewer & viewer, const rendering_context context)
{
    this->modified(false);
}

/**
 * @brief Send an event from this node.
 */
void node::emit_event(const std::string & id,
                      const field_value & value,
                      const double timestamp)
    throw (std::bad_cast, std::bad_alloc)
{
    assert(this->scene());
    eventout_is_map_t::const_iterator pos = this->eventout_is_map.find(id);
    if (pos != this->eventout_is_map.end()) {
        pos->second->value->assign(value);
        pos->second->modified = true;
    }

    for (routes_t::const_iterator itr = this->routes_.begin();
            itr != this->routes_.end(); ++itr) {
        if (id == itr->from_eventout) {
            field_value * const event_value = value.clone().release();
            this->scene()->browser.queue_event(timestamp,
                                               event_value,
                                               itr->to_node,
                                               itr->to_eventin);
        }
    }
}

namespace {
    const short indentIncrement_ = 4;

    class PrintField_ : public std::unary_function<node_interface, void> {
        const node & n;
        std::ostream & out;
        const size_t indent;

    public:
        PrintField_(const node & n,
                    std::ostream & out,
                    const size_t indent):
            n(n),
            out(out),
            indent(indent)
        {}

        void operator()(const node_interface & interface) const
        {
            if (interface.type == node_interface::exposedfield_id
                    || interface.type == node_interface::field_id) {
                this->out << std::string(this->indent + indentIncrement_, ' ')
                          << interface.id << ' '
                          << n.field(interface.id);
            }
        }
    };
}

/**
 * @brief Pretty print.
 *
 * @param out       output stream.
 * @param indent    number of spaces per indentation level.
 *
 * @return @p out.
 */
std::ostream & node::print(std::ostream & out, const size_t indent) const
{
    for (size_t i = 0; i < indent; ++i) { out << ' '; }
    std::string nodeId = this->id();
    if (!nodeId.empty()) { out << "DEF " << nodeId << " "; }
    out << this->type.id << " { ";
    const node_interface_set & interfaces = this->type.interfaces();
    std::for_each(interfaces.begin(), interfaces.end(),
                  PrintField_(*this, out, indent));
    return out << " }";
}

/**
 * @brief Stream output.
 *
 * @param out   output stream.
 * @param n     a node.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const node & n)
{
    return n.print(out, 0);
}

/**
 * @brief node subclass-specific initialization.
 *
 * This method is called by node::initialize. Subclasses of node should
 * override this method for any subclass-specific initialization.
 *
 * The default implementation of this method does nothing.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node::do_initialize(const double timestamp) throw (std::bad_alloc)
{}

/**
 * @brief node subclass-specific relocation update.
 *
 * This method is called by node::relocate. Subclasses of node should override
 * this method for any subclass-specific updates that need to be performed
 * following relocation of a node to a new position in the scene graph (for
 * example, updating a NodePath).
 *
 * The default implementation of this method does nothing.
 */
void node::do_relocate() throw (std::bad_alloc)
{}


/**
 * @brief node subclass-specific shut down.
 *
 * This method is called by node::shutdown. Subclasses of node should
 * override this method for any subclass-specific shut down. Note that
 * this method cannot throw.
 *
 * The default implementation of this method does nothing.
 *
 * @param timestamp the current time.
 */
void node::do_shutdown(const double timestamp) throw ()
{}


/**
 * @class appearance_node
 *
 * @brief Abstract base class for appearance nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
appearance_node::appearance_node(const node_type & type,
                                 const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
appearance_node::~appearance_node() throw ()
{}

/**
 * @brief Cast to an appearance_node.
 *
 * @return a pointer to this appearance_node.
 */
const appearance_node * appearance_node::to_appearance() const throw ()
{
    return this;
}

/**
 * @brief Cast to an appearance_node.
 *
 * @return a pointer to this appearance_node.
 */
appearance_node * appearance_node::to_appearance() throw ()
{
    return this;
}

/**
 * @fn const node_ptr & appearance_node::material() const throw ()
 *
 * @brief Get the material node associated with this appearance node.
 *
 * @return the material_node associated with this appearance_node.
 */

/**
 * @fn const node_ptr & appearance_node::texture() const throw ()
 *
 * @brief Get the texture node associated with this appearance node.
 *
 * @return the texture_node associated with this appearance_node.
 */

/**
 * @fn const node_ptr & appearance_node::texture_transform() const throw ()
 *
 * @brief Get the texture transform node associated with this appearance node.
 *
 * @return the texture_transform_node associated with this appearance_node.
 */


/**
 * @class child_node
 *
 * @brief Abstract base class for child nodes.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
child_node::child_node(const node_type & type, const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destructor.
 */
child_node::~child_node() throw ()
{}

/**
 * @brief Cast to a child_node.
 *
 * @return a pointer to this child_node.
 */
const child_node * child_node::to_child() const throw ()
{
    return this;
}

/**
 * @brief Cast to a child_node.
 *
 * @return a pointer to this child_node.
 */
child_node * child_node::to_child() throw ()
{
    return this;
}


/**
 * @class color_node
 *
 * @brief Abstract base class for color nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
color_node::color_node(const node_type & type, const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
color_node::~color_node() throw ()
{}

/**
 * @brief Cast to a color_node.
 *
 * @return a pointer to this color_node.
 */
const color_node * color_node::to_color() const throw ()
{
    return this;
}

/**
 * @brief Cast to a color_node.
 *
 * @return a pointer to this color_node.
 */
color_node * color_node::to_color() throw ()
{
    return this;
}

/**
 * @fn const std::vector<openvrml::color> & color_node::color() const
 *
 * @brief Get the color array encapsulated by this node.
 *
 * @return the color array for this node.
 */


/**
 * @class coordinate_node
 *
 * @brief Abstract base class for coordinate nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
coordinate_node::coordinate_node(const node_type & type,
                                 const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
coordinate_node::~coordinate_node() throw ()
{}

/**
 * @brief Cast to a coordinate_node.
 *
 * @return a pointer to this coordinate_node.
 */
const coordinate_node * coordinate_node::to_coordinate() const throw ()
{
    return this;
}

/**
 * @brief Cast to a coordinate_node.
 *
 * @return a pointer to this coordinate_node.
 */
coordinate_node * coordinate_node::to_coordinate() throw ()
{
    return this;
}

/**
 * @fn const std::vector<vec3f> & coordinate_node::point() const
 *
 * @brief Get the points encapsulated by this node.
 *
 * @return the array of points for this node.
 */


/**
 * @class font_style_node
 *
 * @brief Abstract base class for font style nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
font_style_node::font_style_node(const node_type & type,
                                 const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
font_style_node::~font_style_node() throw ()
{}

/**
 * @brief Cast to a font_style_node.
 *
 * @return a pointer to this font_style_node.
 */
const font_style_node * font_style_node::to_font_style() const throw ()
{
    return this;
}

/**
 * @brief Cast to a font_style_node.
 *
 * @return a pointer to this font_style_node.
 */
font_style_node * font_style_node::to_font_style() throw ()
{
    return this;
}

/**
 * @fn const std::vector<std::string> & font_style_node::family() const throw ()
 *
 * @brief Get the list of font families.
 *
 * @return the font families that may be used for this FontStyle.
 */

/**
 * @fn bool font_style_node::horizontal() const throw ()
 *
 * @brief Get the flag indicating whether the text should be horizontal or
 *      vertical.
 *
 * @return @c true if the text should be horizontal, or @c false if the text
 *      should be vertical.
 */

/**
 * @fn const std::vector<std::string> & font_style_node::justify() const throw ()
 *
 * @brief Get the descriptor for the text justification.
 *
 * @return the characteristics of the text justification.
 */

/**
 * @fn const std::string & font_style_node::language() const throw ()
 *
 * @brief Get the language code.
 *
 * @return the language code.
 */

/**
 * @fn bool font_style_node::left_to_right() const throw ()
 *
 * @brief Get the flag indicating whether the text should be rendered
 *      left-to-right.
 *
 * @return @c true if the text should be rendered left-to-right, or @c false if
 *      the text should be rendered right-to-left.
 */

/**
 * @fn float font_style_node::size() const throw ()
 *
 * @brief Get the size of the text.
 *
 * @return the size of the text.
 */

/**
 * @fn float font_style_node::spacing() const throw ()
 *
 * @brief Get the spacing for the text.
 *
 * @return the spacing for the text.
 */

/**
 * @fn const std::string & font_style_node::style() const throw ()
 *
 * @brief Get the style for the text.
 *
 * @return the text style.
 */

/**
 * @fn bool font_style_node::top_to_bottom() const throw ()
 *
 * @brief Get the flag indicating whether the text should be rendered
 *      top-to-bottom.
 *
 * @return @c true if the text should be rendered top-to-bottom, or @c false if
 *      the text should be rendered bottom-to-top.
 */


/**
 * @class geometry_node
 *
 * @brief Abstract base class for geometry nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
geometry_node::geometry_node(const node_type & type,
                             const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
geometry_node::~geometry_node() throw ()
{}

/**
 * @brief Cast to a geometry_node.
 *
 * @return a pointer to this geometry_node.
 */
const geometry_node * geometry_node::to_geometry() const throw ()
{
    return this;
}

/**
 * @brief Cast to a geometry_node.
 *
 * @return a pointer to this geometry_node.
 */
geometry_node * geometry_node::to_geometry() throw ()
{
    return this;
}

/**
 * @fn viewer::object_t geometry_node::insert_geometry(openvrml::viewer & v, rendering_context context)
 *
 * @param v         viewer.
 * @param context   rendering context.
 *
 * @return object identifier for the inserted geometry.
 */

/**
 * @brief Get the color node (if any) associated with this geometry.
 *
 * @return the color_node associated associated with this geometry, or 0 if
 *      there is no such node.
 *
 * @todo Reevaluate the way the renderer visits geometry nodes; potentially
 *      eliminate this method.
 */
const color_node * geometry_node::color() const throw ()
{
    return 0;
}


/**
 * @class grouping_node
 *
 * @brief Abstract base class for grouping nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
grouping_node::grouping_node(const node_type & type,
                             const scope_ptr & scope)
    throw ():
    node(type, scope),
    child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
grouping_node::~grouping_node() throw ()
{}

/**
 * @brief Cast to a grouping_node.
 *
 * @return a pointer to this grouping_node.
 */
const grouping_node * grouping_node::to_grouping() const throw ()
{
    return this;
}

/**
 * @brief Cast to a grouping_node.
 *
 * @return a pointer to this grouping_node.
 */
grouping_node * grouping_node::to_grouping() throw ()
{
    return this;
}

/**
 * @fn const std::vector<node_ptr> & grouping_node::children() const throw ()
 *
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */

/**
 * @fn void grouping_node::activate(double timestamp, bool over, bool active, double * p)
 *
 * @brief Called in response to user interaction to activate any child pointing
 *        device sensor nodes.
 *
 * @param timestamp the current time.
 * @param over      whether the pointer is over sensitive geometry.
 * @param active    whether the pointer has activated sensitive geometry.
 * @param p         the activation point.
 */


/**
 * @class material_node
 *
 * @brief Abstract base class for material nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
material_node::material_node(const node_type & type, const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
material_node::~material_node() throw ()
{}

/**
 * @brief Cast to a material_node.
 *
 * @return a pointer to this material_node.
 */
const material_node * material_node::to_material() const throw ()
{
    return this;
}

/**
 * @brief Cast to a material_node.
 *
 * @return a pointer to this material_node.
 */
material_node * material_node::to_material() throw ()
{
    return this;
}

/**
 * @fn float material_node::ambient_intensity() const throw ()
 *
 * @brief Get the ambient intensity.
 *
 * @return the ambient intensity.
 */

/**
 * @fn const color & material_node::diffuse_color() const throw ()
 *
 * @brief Get the diffuse color.
 *
 * @return the diffuse color.
 */

/**
 * @fn const color & material_node::emissive_color() const throw ()
 *
 * @brief Get the emissive color.
 *
 * @return the emissive color.
 */

/**
 * @fn float material_node::shininess() const throw ()
 *
 * @brief Get the shininess.
 *
 * @return the shininess.
 */

/**
 * @fn const color & material_node::specular_color() const throw ()
 *
 * @brief Get the specular color.
 *
 * @return the specular color.
 */

/**
 * @fn float material_node::transparency() const throw ()
 *
 * @brief Get the transparency.
 *
 * @return the transparency.
 */


/**
 * @class normal_node
 *
 * @brief Abstract base class for normal nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
normal_node::normal_node(const node_type & type, const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
normal_node::~normal_node() throw ()
{}

/**
 * @brief Cast to a normal_node.
 *
 * @return a pointer to this normal_node.
 */
const normal_node * normal_node::to_normal() const throw ()
{
    return this;
}

/**
 * @brief Cast to a normal_node.
 *
 * @return a pointer to this normal_node.
 */
normal_node * normal_node::to_normal() throw ()
{
    return this;
}

/**
 * @fn const std::vector<vec3f> & normal_node::vector() const throw ()
 *
 * @brief Get the array of normal vectors.
 *
 * @return the array of normal vectors.
 */


/**
 * @class sound_source_node
 *
 * @brief Abstract base class for sound source nodes.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
sound_source_node::sound_source_node(const node_type & type,
                                     const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destructor.
 */
sound_source_node::~sound_source_node() throw ()
{}

/**
 * @brief Cast to a sound_source_node.
 *
 * @return a pointer to this sound_source_node.
 */
const sound_source_node * sound_source_node::to_sound_source() const throw ()
{
    return this;
}

/**
 * @brief Cast to a sound_source_node.
 *
 * @return a pointer to this sound_source_node.
 */
sound_source_node * sound_source_node::to_sound_source() throw ()
{
    return this;
}


/**
 * @class texture_node
 *
 * @brief Abstract base class for texture nodes.
 *
 * @todo Clean this up. It would be better just to have an accessor for an
 *      SFImage along with the repeatS and repeatT accessors.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
texture_node::texture_node(const node_type & type, const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
texture_node::~texture_node() throw ()
{}

/**
 * @brief Cast to a texture_node.
 *
 * @return a pointer to this texture_node.
 */
const texture_node * texture_node::to_texture() const throw ()
{
    return this;
}

/**
 * @brief Cast to a texture_node.
 *
 * @return a pointer to this texture_node.
 */
texture_node * texture_node::to_texture() throw ()
{
    return this;
}

/**
 * @fn size_t texture_node::components() const throw ()
 *
 * @brief Get the number of components for the image.
 *
 * @return the number of components for the image.
 */

/**
 * @fn size_t texture_node::width() const throw ()
 *
 * @brief Get width of the image in pixels.
 *
 * @return the width of the image in pixels.
 */

/**
 * @fn size_t texture_node::height() const throw ()
 *
 * @brief Get height of the image in pixels.
 *
 * @return the height of the image in pixels.
 */

/**
 * @fn size_t texture_node::frames() const throw ()
 *
 * @brief Get the number of frames for a time-dependent texture.
 *
 * @return the number of frames.
 *
 * @todo This needs to go away. Time-dependent textures might not be
 *      frame-based. For instance, consider an SVG animation.
 */

/**
 * @fn const unsigned char * texture_node::pixels() const throw ()
 *
 * @brief Get the pixels.
 *
 * @return the pixels.
 */

/**
 * @fn bool texture_node::repeat_s() const throw ()
 *
 * @brief Get the flag indicating whether the texture should repeat in the
 *      <var>S</var> direction.
 *
 * @return @c true if the image should repeat in the <var>S</var> direction,
 *      @c false otherwise.
 */

/**
 * @fn bool texture_node::repeat_t() const throw ()
 *
 * @brief Get the flag indicating whether the texture should repeat in the
 *      <var>T</var> direction.
 *
 * @return @c true if the image should repeat in the <var>T</var> direction,
 *      @c false otherwise.
 */


/**
 * @class texture_coordinate_node
 *
 * @brief Abstract base class for texture coordinate nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
texture_coordinate_node::texture_coordinate_node(const node_type & type,
                                                 const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
texture_coordinate_node::~texture_coordinate_node() throw ()
{}

/**
 * @brief Cast to a texture_coordinate_node.
 *
 * @return a pointer to this texture_coordinate_node.
 */
const texture_coordinate_node *
texture_coordinate_node::to_texture_coordinate() const throw ()
{
    return this;
}

/**
 * @brief Cast to a texture_coordinate_node.
 *
 * @return a pointer to this texture_coordinate_node.
 */
texture_coordinate_node * texture_coordinate_node::to_texture_coordinate()
    throw ()
{
    return this;
}

/**
 * @fn const std::vector<vec2f> & texture_coordinate_node::point() const throw ()
 *
 * @brief The texture coordinates.
 *
 * @return the texture coordinates.
 */


/**
 * @class texture_transform_node
 *
 * @brief Abstract base class for texture transform nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
texture_transform_node::texture_transform_node(const node_type & type,
                                               const scope_ptr & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destructor.
 */
texture_transform_node::~texture_transform_node() throw ()
{}

/**
 * @brief Cast to a texture_transform_node.
 *
 * @return a pointer to this texture_transform_node.
 */
const texture_transform_node *
texture_transform_node::to_texture_transform() const throw ()
{
    return this;
}

/**
 * @brief Cast to a texture_transform_node.
 *
 * @return a pointer to this texture_transform_node.
 */
texture_transform_node * texture_transform_node::to_texture_transform()
    throw ()
{
    return this;
}


/**
 * @class transform_node
 *
 * @brief Abstract base class for texture transform nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
transform_node::transform_node(const node_type & type,
                               const scope_ptr & scope)
    throw ():
    node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope)
{}

/**
 * @brief Destroy.
 */
transform_node::~transform_node() throw ()
{}

/**
 * @brief Cast to a transform_node.
 *
 * @return a pointer to this transform_node.
 */
const transform_node * transform_node::to_transform() const throw ()
{
    return this;
}

/**
 * @brief Cast to a transform_node.
 *
 * @return a pointer to this transform_node.
 */
transform_node * transform_node::to_transform() throw ()
{
    return this;
}

/**
 * @fn const mat4f & transform_node::transform() const throw ()
 *
 * @brief Get the transformation associated with the node as a matrix.
 *
 * @return the transformation associated with the node.
 */


/**
 * @class viewpoint_node
 *
 * @brief Abstract base class for texture transform nodes.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the Scope the node belongs to.
 */
viewpoint_node::viewpoint_node(const node_type & type,
                               const scope_ptr & scope)
    throw ():
    node(type, scope),
    child_node(type, scope)
{}

/**
 * @brief Destructor.
 */
viewpoint_node::~viewpoint_node() throw ()
{}

/**
 * @brief Cast to a viewpoint_node.
 *
 * @return a pointer to this viewpoint_node.
 */
const viewpoint_node * viewpoint_node::to_viewpoint() const throw ()
{
    return this;
}

/**
 * @brief Cast to a viewpoint_node.
 *
 * @return a pointer to this viewpoint_node.
 */
viewpoint_node * viewpoint_node::to_viewpoint() throw ()
{
    return this;
}

/**
 * @fn const mat4f & viewpoint_node::transformation() const throw ()
 *
 * @brief Get the transformation of the viewpoint_node in the global coordinate
 *      system.
 *
 * @return the transformation of the viewpoint_node in the global coordinate
 *      system.
 */

/**
 * @fn const mat4f & viewpoint_node::user_view_transform() const throw ()
 *
 * @brief Get the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @return the transformation of the user view relative to the viewpoint_node.
 */

/**
 * @fn void viewpoint_node::user_view_transform(const mat4f & transform) throw ()
 *
 * @brief Set the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @param transform the new transformation.
 */

/**
 * @fn const std::string & viewpoint_node::description() const throw ()
 *
 * @brief Get the description.
 *
 * @return the description.
 */

/**
 * @fn float viewpoint_node::field_of_view() const throw ()
 *
 * @brief Get the field of view.
 *
 * @return the field of view in radians.
 */


/**
 * @class node_traverser
 *
 * @brief Traverse the children of each node in a node hierarchy only once.
 *
 * The node_traverser provides a generalized traversal mechanism that avoids
 * redundantly traversing branches of the node hierarchy. If a node occurs
 * multiple places in a branch, <b>the children of that node will be visted in
 * the traversal only once</b>.
 *
 * For each node encountered in the traversal, node_traverser::performAction
 * is called. Concrete subclasses of node_traverser implement this method in
 * order to perform some operation on each node.
 */

/**
 * @var std::set<node *> node_traverser::traversed_nodes
 *
 * @brief The set of nodes that have already been traversed.
 */

/**
 * @var bool node_traverser::halt
 *
 * @brief Flag to indicate if the traversal should be halted.
 */

/**
 * @brief Construct.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
node_traverser::node_traverser() throw (std::bad_alloc):
    halt(false)
{}

/**
 * @fn node_traverser::node_traverser(const node_traverser &)
 *
 * @brief Not implemented.
 *
 * node_traverser is not copyable.
 */

/**
 * @brief Destroy.
 */
node_traverser::~node_traverser() throw ()
{}

/**
 * @fn node_traverser& node_traverser::operator=(const node_traverser &)
 *
 * @brief Not implemented.
 *
 * node_traverser is not copyable.
 */

/**
 * @brief Traverse a node.
 *
 * No guarantee is made about the state of the node_traverser instance in the
 * event that this method throws.
 *
 * In addition to std::bad_alloc, this function throws any exception thrown
 * from onEntering or onLeaving.
 *
 * @param n  the root node of the branch to traverse.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node_traverser::traverse(node & n)
{
    assert(this->traversed_nodes.empty());
    try {
        this->do_traversal(n);
    } catch (...) {
        this->halt = false;
        this->traversed_nodes.clear();
        throw;
    }
    this->halt = false;
    this->traversed_nodes.clear();
}

/**
 * @brief Traverse an sfnode.
 *
 * No guarantee is made about the state of the node_traverser instance in the
 * event that this method throws.
 *
 * In addition to std::bad_alloc, this function throws any exception thrown
 * from onEntering or onLeaving.
 *
 * @param node  the root node of the branch to traverse.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node_traverser::traverse(const node_ptr & node)
{
    assert(this->traversed_nodes.empty());
    try {
        if (node) {
            if (this->traversed_nodes.find(node.get())
                    == this->traversed_nodes.end()) {
                this->do_traversal(*node);
            }
        }
    } catch (...) {
        this->halt = false;
        this->traversed_nodes.clear();
        throw;
    }
    this->halt = false;
    this->traversed_nodes.clear();
}

/**
 * @brief Traverse an mfnode.
 *
 * No guarantee is made about the state of the node_traverser instance in the
 * event that this method throws.
 *
 * In addition to std::bad_alloc, this function throws any exception thrown
 * from on_entering or on_leaving.
 *
 * @param nodes  the root @link openvrml::node nodes@endlink of the branch to
 *               traverse.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node_traverser::traverse(const std::vector<node_ptr> & nodes)
{
    assert(this->traversed_nodes.empty());
    try {
        for (std::vector<node_ptr>::const_iterator node(nodes.begin());
                node != nodes.end(); ++node) {
            if (*node) {
                if (this->traversed_nodes.find(node->get())
                        == this->traversed_nodes.end()) {
                    this->do_traversal(**node);
                }
            }
        }
    } catch (...) {
        this->halt = false;
        this->traversed_nodes.clear();
        throw;
    }
    this->halt = false;
    this->traversed_nodes.clear();
}

/**
 * @brief Halt the traversal.
 *
 * If this method is called during a traversal, no more descendent
 * @link openvrml::node nodes@endlink will be traversed. Note that if
 * halt_traversal is called in the implementation of on_entering, on_leaving
 * will still be called for the current node.
 */
void node_traverser::halt_traversal() throw ()
{
    this->halt = true;
}

/**
 * @internal
 *
 * @brief Traverse a node.
 *
 * @param n  the node to traverse.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node_traverser::do_traversal(node & n)
{
    std::set<node *>::iterator pos = this->traversed_nodes.find(&n);
    const bool alreadyTraversed = (pos != this->traversed_nodes.end());

    if (!alreadyTraversed) {
        this->on_entering(n);

        this->traversed_nodes.insert(&n);

        const node_interface_set & interfaces = n.type.interfaces();
        for (node_interface_set::const_iterator interface(interfaces.begin());
                interface != interfaces.end() && !this->halt; ++interface) {
            if (interface->type == node_interface::field_id
                    || interface->type == node_interface::exposedfield_id) {
                if (interface->field_type == field_value::sfnode_id) {
                    const sfnode & value =
                        static_cast<const sfnode &>(n.field(interface->id));
                    if (value.value) {
                        this->do_traversal(*value.value);
                    }
                } else if (interface->field_type == field_value::mfnode_id) {
                    const mfnode & children =
                        static_cast<const mfnode &>(n.field(interface->id));
                    for (size_t i = 0; i < children.value.size(); ++i) {
                        if (children.value[i]) {
                            this->do_traversal(*children.value[i]);
                        }
                    }
                }
            }
        }
        this->on_leaving(n);
    }
}

/**
 * @brief Called for each node in the traversal <em>before</em>
 *      traversing the its descendants.
 *
 * @param n  the node currently being traversed.
 */
void node_traverser::on_entering(node & n)
{}

/**
 * @brief Called for each node in the traversal <em>after</em>
 *      traversing the its descendants.
 *
 * @param n  the node currently being traversed.
 */
void node_traverser::on_leaving(node & n)
{}

} // namespace openvrml
