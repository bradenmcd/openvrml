// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2002, 2003, 2004, 2005  Braden McDaniel
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
# include <boost/bind.hpp>
# include <boost/lexical_cast.hpp>
# include "private.h"
# include "node.h"
# include "scope.h"
# include "browser.h"

/**
 * @defgroup nodes Nodes
 */

namespace openvrml {

/**
 * @class unsupported_interface
 *
 * @ingroup nodes
 *
 * @brief Exception to indicate that a node interface is not supported.
 *
 * This exception is thrown by node::event_listener and node::event_emitter to
 * indicate that the node doesn't support the interface through which the
 * caller is trying to modify the node. It is also thrown by
 * node_class::create_type if the class object doesn't support an interface
 * specified in the node_interface_set given to that method.
 */

/**
 * @brief Construct.
 *
 * @param interface a node_interface.
 */
unsupported_interface::unsupported_interface(const node_interface & interface)
    throw ():
    std::logic_error("Unsupported interface \""
                     + boost::lexical_cast<std::string>(interface) + "\".")
{}

/**
 * @brief Construct.
 *
 * @param type          the node type.
 * @param interface_id  the name of the interface that is not available.
 */
unsupported_interface::unsupported_interface(const node_type & type,
                                             const std::string & interface_id)
    throw ():
    std::logic_error(type.id() + " has no interface \"" + interface_id + '"')
{}

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
    const std::string & interface_id)
    throw ():
    std::logic_error(type.id() + " has no "
                     + boost::lexical_cast<std::string>(interface_type)
                     + " \"" + interface_id + '"')
{}

/**
 * @brief Destroy.
 */
unsupported_interface::~unsupported_interface() throw ()
{}


/**
 * @class node_interface
 *
 * @ingroup nodes
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
 * @relatesalso node_interface
 *
 * If @p type is node_interface::invalid_type_id, @c failbit is set on @p out.
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
 * @relatesalso node_interface
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
 * @relatesalso node_interface
 *
 * @brief Compare for equality.
 *
 * @param lhs   a node_interface.
 * @param rhs   a node_interface.
 *
 * @return @c true if the two node_interfaces are equal, @c false otherwise.
 */
bool operator==(const node_interface & lhs, const node_interface & rhs)
    throw ()
{
    return lhs.type == rhs.type
        && lhs.field_type == rhs.field_type
        && lhs.id == rhs.id;
}

/**
 * @relatesalso node_interface
 *
 * @brief Compare for openvrml::inequality.
 *
 * @param lhs   a node_interface.
 * @param rhs   a node_interface.
 *
 * @return @c true if the two node_interfaces are equal, @c false otherwise.
 */
bool operator!=(const node_interface & lhs, const node_interface & rhs)
    throw ()
{
    return !(lhs == rhs);
}

/**
 * @relatesalso node_interface
 *
 * @brief Stream output.
 *
 * @param out       output stream.
 * @param interface node_interface.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const node_interface & interface)
{
    return out << interface.type << ' ' << interface.field_type << ' '
               << interface.id;
}

/**
 * @relatesalso node_interface
 *
 * @brief Stream input.
 *
 * @param in        input stream.
 * @param interface node_interface.
 *
 * @return @p in.
 */
std::istream & operator>>(std::istream & in, node_interface & interface)
{
    return in >> interface.type >> interface.field_type >> interface.id;
}


/**
 * @struct node_interface_matches_eventin
 *
 * @ingroup nodes
 *
 * @brief Determine if a node_interface matches an eventIn identifier.
 *
 * @par Model of
 * <a href="http://www.sgi.com/tech/stl/AdaptableBinaryPredicate.html">
 * AdaptableBinaryPredicate</a>,
 * <a href="http://www.sgi.com/tech/stl/DefaultConstructible.html">
 * DefaultConstructible</a>
 */

/**
 * @fn node_interface_matches_eventin::result_type node_interface_matches_eventin::operator()(const node_interface_matches_eventin::first_argument_type & interface, const node_interface_matches_eventin::second_argument_type & eventin_id) const
 *
 * @brief Determine if a node_interface matches an eventIn identifier.
 *
 * A node_interface matches an eventIn identifier if:
 * - the node_interface's @a type is node_interface::eventin_id and
 *   - the node_interface's @a id is lexicographically equal to the eventIn
 *     identifier
 *   - or the node_interface's @a id is lexicographically equal to the eventIn
 *     identifier with the string "set_" prepended.
 * - or, the node_interface's @a type is node_interface::exposedfield_id and
 *   - the node_interface's @a id is lexicographically equal to the eventIn
 *     identifier
 *   - or the node_interface's @a id with the string "set_" prepended is
 *     lexicographically equal to the eventIn identifier.
 *
 * @param interface     node_interface.
 * @param eventin_id    eventIn identifier.
 *
 * @return @c true if @p interface matches @p eventin_id; @c false otherwise.
 */


/**
 * @struct node_interface_matches_eventout
 *
 * @ingroup nodes
 *
 * @brief Determine if a node_interface matches an eventOut identifier.
 *
 * @par Model of
 * <a href="http://www.sgi.com/tech/stl/AdaptableBinaryPredicate.html">
 * AdaptableBinaryPredicate</a>,
 * <a href="http://www.sgi.com/tech/stl/DefaultConstructible.html">
 * DefaultConstructible</a>
 */

/**
 * @fn node_interface_matches_eventout::result_type node_interface_matches_eventout::operator()(const node_interface_matches_eventout::first_argument_type & interface, const node_interface_matches_eventout::second_argument_type & eventout_id) const
 *
 * @brief Determine if a node_interface matches an eventOut identifier.
 *
 * A node_interface matches an eventOut identifier if:
 * - the node_interface's @a type is node_interface::eventout_id and
 *   - the node_interface's @a id is lexicographically equal to the eventOut
 *     identifier
 *   - or the node_interface's @a id is lexicographically equal to the eventOut
 *     identifier with the string "_changed" appended.
 * - or, the node_interface's @a type is node_interface::exposedfield_id and
 *   - the node_interface's @a id is lexicographically equal to the eventOut
 *     identifier
 *   - or the node_interface's @a id with the string "_changed" appended is
 *     lexicographically equal to the eventOut identifier.
 *
 * @param interface     node_interface.
 * @param eventout_id   eventOut identifier.
 *
 * @return @c true if @p interface matches @p eventout_id; @c false otherwise.
 */


/**
 * @struct node_interface_matches_exposedfield
 *
 * @ingroup nodes
 *
 * @brief Determine if a node_interface matches an exposedField identifier.
 *
 * @par Model of
 * <a href="http://www.sgi.com/tech/stl/AdaptableBinaryPredicate.html">
 * AdaptableBinaryPredicate</a>,
 * <a href="http://www.sgi.com/tech/stl/DefaultConstructible.html">
 * DefaultConstructible</a>
 */

/**
 * @fn node_interface_matches_exposedfield::result_type node_interface_matches_exposedfield::operator()(const node_interface_matches_exposedfield::first_argument_type & interface, const node_interface_matches_exposedfield::second_argument_type & exposedfield_id) const
 *
 * @brief Determine if a node_interface matches an exposedField identifier.
 *
 * A node_interface matches an exposedField identifier if the node_interface's
 * @a type is node_interface::exposedfield_id and the node_interface's @a id is
 * lexicographically equal to the exposedField identifier.
 *
 * @param interface         node_interface.
 * @param exposedfield_id   exposedField identifier.
 *
 * @return @c true if @p interface matches @p exposedfield_id; @c false
 *         otherwise.
 */


/**
 * @struct node_interface_matches_field
 *
 * @ingroup nodes
 *
 * @brief Determine if a node_interface matches an field identifier.
 *
 * @par Model of
 * <a href="http://www.sgi.com/tech/stl/AdaptableBinaryPredicate.html">
 * AdaptableBinaryPredicate</a>,
 * <a href="http://www.sgi.com/tech/stl/DefaultConstructible.html">
 * DefaultConstructible</a>
 */

/**
 * @fn node_interface_matches_field::result_type node_interface_matches_field::operator()(const node_interface_matches_field::first_argument_type & interface, const node_interface_matches_field::second_argument_type & field_id) const
 *
 * @brief Determine if a node_interface matches an field identifier.
 *
 * A node_interface matches a field identifier if the node_interface's @a type
 * is node_interface::field_id or node_interface::exposedfield_id and the
 * node_interface's @a id is lexicographically equal to the field identifier.
 *
 * @param interface node_interface.
 * @param field_id  field identifier.
 *
 * @return @c true if @p interface matches @p field_id; @c false otherwise.
 */


/**
 * @struct node_interface_compare
 *
 * @ingroup nodes
 *
 * @brief Function object to compare two node_interfaces based on their id.
 *
 * node_interface_compare provides a
 * <a href="http://www.sgi.com/tech/stl/StrictWeakOrdering.html">
 * StrictWeakOrdering</a> for node_interfaces and ensures that exposedField
 * definitions appropriately encroach on the namespace of eventIn and eventOut
 * definitions. That is, per 4.7 of the VRML97 spec: "An exposedField named zzz
 * can be referred to as 'set_zzz' and treated as an eventIn, and can be
 * referred to as 'zzz_changed' and treated as an eventOut."
 *
 * node_interface_compare is the comparator used by node_interface_set.
 *
 * @par Model of
 * <a href="http://www.sgi.com/tech/stl/AdaptableBinaryPredicate.html">
 * AdaptableBinaryPredicate</a>,
 * <a href="http://www.sgi.com/tech/stl/StrictWeakOrdering.html">
 * StrictWeakOrdering</a>,
 * <a href="http://www.sgi.com/tech/stl/DefaultConstructible.html">
 * DefaultConstructible</a>
 *
 * @see node_interface_set
 */

/**
 * @fn node_interface_compare::result_type node_interface_compare::operator()(const node_interface_compare::first_argument_type & lhs, const node_interface_compare::second_argument_type & rhs) const
 *
 * @brief Compare two node_interfaces based on their id.
 *
 * @param lhs   node_interface.
 * @param rhs   node_interface.
 *
 * @return @c true if @p lhs.id is lexicographically less than @p rhs.id;
 *         @c false otherwise.
 */


/**
 * @typedef std::set<node_interface, node_interface_compare> node_interface_set
 *
 * @ingroup nodes
 *
 * @brief A group of unique <code>node_interface</code>s.
 *
 * <code>node_interface_set</code>s are used to construct new
 * <code>node_type</code>s. <code>node_type</code> objects also expose their
 * interfaces as a <code>node_interface_set</code>. The interfaces in a
 * <code>node_interface_set</code> are guaranteed to be unique and
 * non-conflicting.
 */

/**
 * @ingroup nodes
 *
 * @brief Find an interface matching @p id.
 *
 * If no interface is found with an interface identifier that is an exact match
 * for @p id, this function will look for @c set_ and @c _changed variants. If
 * @p interfaces contains a field @c zzz along with an eventIn @c set_zzz
 * and/or an eventOut @c zzz_changed, the eventIn or eventOut will only be
 * found if the @c set_zzz or @c zzz_changed form, respectively, is used for
 * @p id.
 *
 * @param interfaces    a set of <code>node_interface</code>s.
 * @param id            the interface id to look for.
 *
 * @return a <code>const_iterator</code> to the interface, or
 *         <code>node_interface_set::end</code> if no interface is found.
 */
const node_interface_set::const_iterator
find_interface(const node_interface_set & interfaces, const std::string & id)
    throw ()
{
    using std::find_if;
    using boost::bind;
    node_interface_set::const_iterator pos =
        find_if(interfaces.begin(), interfaces.end(),
                bind(node_interface_matches_field(), _1, id));
    if (pos == interfaces.end()) {
        using std::logical_or;

        pos = find_if(interfaces.begin(), interfaces.end(),
                      bind(logical_or<bool>(),
                           bind(node_interface_matches_eventin(), _1, id),
                           bind(node_interface_matches_eventout(), _1, id)));
    }
    return pos;
}


/**
 * @class node_class
 *
 * @ingroup nodes
 *
 * @brief A class object for node instances.
 *
 * <code>node_class</code> can be thought of as a "supertype" of sorts. A given
 * node implementation can support as many node types as there are unique
 * combinations of the interfaces it supports. The most readily apparent
 * role of the <code>node_class</code> object for a node implementation is to
 * serve as a factory for these <code>node_type</code>s.
 */

/**
 * @internal
 *
 * @var openvrml::browser * node_class::browser_
 *
 * @brief The browser associated with this <code>node_class</code>.
 */

/**
 * @brief Constructor.
 *
 * A <code>node_class</code> is constructed using a <code>browser</code>. All
 * <code>node</code> instances that share a particular <code>node_class</code>
 * "belong to" the <code>browser</code> associated with the
 * <code>node_class</code>.
 *
 * @param b the <code>browser</code> to be associated with the
 *          <code>node_class</code>.
 */
node_class::node_class(openvrml::browser & b) throw ():
    browser_(&b)
{}

/**
 * @brief Destructor.
 */
node_class::~node_class() throw ()
{}

/**
 * @brief The <code>browser</code> associated with this
 *        <code>node_class</code>.
 *
 * @return the <code>browser</code> associated with this
 *         <code>node_class</code>.
 */
browser & node_class::browser() const throw ()
{
    assert(this->browser_);
    return *this->browser_;
}

/**
 * @brief <code>node_class</code>-specific initialization.
 *
 * This method is called during initialization of a <code>browser</code> object
 * with a new root <code>scene</code>. It is called after the individual
 * <code>node</code> instances have been initialized, and before the world
 * starts running. It delegates to <code>node_class::do_initialize</code>.
 *
 * @param initial_viewpoint the <code>viewpoint_node</code> that should be
 *                          bound initially; or 0 if the default
 *                          <code>viewpoint_node</code> should be bound.
 * @param time              the current time.
 */
void node_class::initialize(viewpoint_node * initial_viewpoint,
                            const double time)
    throw ()
{
    this->do_initialize(initial_viewpoint, time);
}

/**
 * @brief <code>node_class</code>-specific initialization.
 *
 * Node implementations should override this method to perform any
 * <code>node_class</code>-wide initialization.
 *
 * @param initial_viewpoint the <code>viewpoint_node</code> that should be
 *                          bound initially; or 0 if the default
 *                          <code>viewpoint_node</code> should be bound.
 * @param time              the current time.
 *
 * @sa node_class::initialize
 */
void node_class::do_initialize(viewpoint_node * initial_viewpoint,
                               const double time)
    throw ()
{}

/**
 * @brief <code>node_class</code>-specific rendering.
 *
 * This function delegates to <code>node_class::do_render</code>.
 *
 * @param v    the viewer to render to.
 */
void node_class::render(viewer & v) const throw ()
{
    this->do_render(v);
}

/**
 * @brief <code>node_class</code>-specific rendering.
 *
 * The default implementation of this method does nothing.
 *
 * @param v    the viewer to render to.
 *
 * @sa node_class::render
 */
void node_class::do_render(viewer & v) const throw ()
{}

/**
 * @brief Create a new <code>node_type</code>.
 *
 * <code>node_type</code>s can be said to subset the master type provided by
 * the <code>node_class</code>. Each <code>node_class</code> instance can
 * support certain <code>node</code> interfaces; the
 * <code>node_interface_set</code> passed to
 * <code>node_class::create_type</code> must be a subset of those supported
 * interfaces.
 *
 * This function delegates to <code>node_class::do_create_type</code>.
 *
 * @param id            the name for the new <code>node_type</code>.
 * @param interfaces    a <code>node_interface_set</code> containing the
 *                      interfaces for the new type.
 *
 * @return a newly created <code>node_type</code>.
 *
 * @exception unsupported_interface if the <code>node_class</code> cannot
 *                                  support one of the
 *                                  <code>node_interface</code>s in
 *                                  @p interfaces.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<node_type>
node_class::create_type(const std::string & id,
                        const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    return this->do_create_type(id, interfaces);
}

/**
 * @fn const boost::shared_ptr<node_type> node_class::do_create_type(const std::string & id, const node_interface_set & interfaces) const throw (unsupported_interface, std::bad_alloc)
 *
 * @brief Create a new <code>node_type</code>.
 *
 * @param id            the name for the new <code>node_type</code>.
 * @param interfaces    a <code>node_interface_set</code> containing the
 *                      interfaces for the new type.
 *
 * @return a newly created <code>node_type</code>.
 *
 * @exception unsupported_interface if the <code>node_class</code> cannot
 *                                  support one of the
 *                                  <code>node_interface</code>s in
 *                                  @p interfaces.
 * @exception std::bad_alloc        if memory allocation fails.
 *
 * @sa node_class::create_type
 */


/**
 * @typedef initial_value_map
 *
 * @brief A map of the initial values with which a <code>node</code> is
 *        instantiated.
 */


/**
 * @class node_type
 *
 * @ingroup nodes
 *
 * @brief Type information object for <code>node</code>s.
 */

/**
 * @internal
 *
 * @var openvrml::node_class & node_type::node_class_
 *
 * @brief The class object associated with the <code>node_type</code>.
 */

/**
 * @internal
 *
 * @var const std::string node_type::id_
 *
 * @brief The name of the <code>node_type</code>.
 */

/**
 * @brief Constructor.
 *
 * @param c     the class object associated with the <code>node_type</code>.
 * @param id    the name for the <code>node_type</code>.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
node_type::node_type(const openvrml::node_class & c, const std::string & id)
    throw (std::bad_alloc):
    node_class_(c),
    id_(id)
{}

/**
 * @brief Destructor.
 */
node_type::~node_type() throw ()
{}

/**
 * @brief The class object associated with the <code>node_type</code>.
 *
 * @return the class object associated with the <code>node_type</code>.
 */
const node_class & node_type::node_class() const throw ()
{
    return this->node_class_;
}

/**
 * @brief The name of the <code>node_type</code>.
 *
 * @return the name of the <code>node_type</code>.
 */
const std::string & node_type::id() const throw ()
{
    return this->id_;
}

/**
 * @brief Get the set of interfaces for the <code>node_type</code>.
 *
 * This function delegates to <code>node_type::do_interfaces</code>.
 *
 * @return the set of interfaces.
 */
const node_interface_set & node_type::interfaces() const throw ()
{
    return this->do_interfaces();
}

/**
 * @fn const node_interface_set & node_type::do_interfaces() const throw ()
 *
 * @brief Get the set of interfaces for the <code>node_type</code>.
 *
 * Subclasses must implement this function.
 *
 * @return the set of interfaces.
 */

/**
 * @brief Create a new node with this <code>node_type</code>.
 *
 * @param scope             the scope to which the new <code>node</code> should
 *                          belong.
 * @param initial_values    a map of initial values for the <code>node</code>'s
 *                          fields and exposedFields.
 *
 * @return a node_ptr to a new node.
 *
 * @exception unsupported_interface if @p initial_values specifies a field
 *                                  name that is not supported by the node
 *                                  type.
 * @exception std::bad_cast         if a value in @p initial_values is the
 *                                  wrong type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_ptr
node_type::create_node(const boost::shared_ptr<openvrml::scope> & scope,
                       const initial_value_map & initial_values) const
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    return this->do_create_node(scope, initial_values);
}

/**
 * @fn const node_ptr node_type::do_create_node(const boost::shared_ptr<openvrml::scope> & scope, const initial_value_map & initial_values) const throw (unsupported_interface, std::bad_cast, std::bad_alloc)
 *
 * @brief Create a new node with this <code>node_type</code>.
 *
 * Subclasses must implement this function.
 *
 * @param scope             the <code>scope</code> to which the new
 *                          <code>node</code> should belong.
 * @param initial_values    a map of initial values for the <code>node</code>'s
 *                          fields and exposedFields.
 *
 * @return a node_ptr to a new node.
 *
 * @exception unsupported_interface if @p initial_values specifies a field
 *                                  name that is not supported by the node
 *                                  type.
 * @exception std::bad_cast         if a value in @p initial_values is the
 *                                  wrong type.
 * @exception std::bad_alloc        if memory allocation fails.
 */


/**
 * @class field_value_type_mismatch
 *
 * @ingroup nodes
 *
 * @brief Thrown when field value types do not match, generally in a @c ROUTE
 *      or @c IS.
 */

/**
 * @brief Constructor.
 */
field_value_type_mismatch::field_value_type_mismatch():
    std::logic_error("Field value types do not match.")
{}

/**
 * @brief Destructor.
 */
field_value_type_mismatch::~field_value_type_mismatch() throw ()
{}


/**
 * @typedef std::deque<node *> node_path
 *
 * @ingroup nodes
 *
 * @brief A path to a node in the scene starting with one of the scene root
 *      nodes and ending with the objective node.
 */


/**
 * @fn To * node_cast(node * n) throw ()
 *
 * @ingroup nodes
 *
 * @relatesalso openvrml::node
 *
 * @brief Downcast a node to one of the abstract node types.
 *
 * @param n a node.
 *
 * @return a pointer to the downcast node, or 0 if the cast fails.
 */

/**
 * @class node
 *
 * @ingroup nodes
 *
 * @brief A node in the scene graph.
 */

/**
 * @fn std::ostream & node::operator<<(std::ostream & out, const node & n)
 *
 * @brief Stream output.
 */

/**
 * @fn script_node * node::node_cast<script_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>script_node</code>.
 */

/**
 * @fn appearance_node * node::node_cast<appearance_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>appearance_node</code>.
 */

/**
 * @fn child_node * node::node_cast<child_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>child_node</code>.
 */

/**
 * @fn color_node * node::node_cast<color_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>color_node</code>.
 */

/**
 * @fn coordinate_node * node::node_cast<coordinate_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>coordinate_node</code>.
 */

/**
 * @fn font_style_node * node::node_cast<font_style_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>font_style_node</code>.
 */

/**
 * @fn geometry_node * node::node_cast<geometry_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>geometry_node</code>.
 */

/**
 * @fn grouping_node * node::node_cast<grouping_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>grouping_node</code>.
 */

/**
 * @fn material_node * node::node_cast<material_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>material_node</code>.
 */

/**
 * @fn navigation_info_node * node::node_cast<navigation_info_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>navigation_info_node</code>.
 */

/**
 * @fn normal_node * node::node_cast<normal_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>normal_node</code>.
 */

/**
 * @fn sound_source_node * node::node_cast<sound_source_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>sound_source_node</code>.
 */

/**
 * @fn texture_node * node::node_cast<texture_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>texture_node</code>.
 */

/**
 * @fn texture_coordinate_node * node::node_cast<texture_coordinate_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>texture_coordinate_node</code>.
 */

/**
 * @fn texture_transform_node * node::node_cast<texture_transform_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>texture_transform_node</code>.
 */

/**
 * @fn transform_node * node::node_cast<transform_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>transform_node</code>.
 */

/**
 * @fn viewpoint_node * node::node_cast<viewpoint_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>viewpoint_node</code>.
 */

/**
 * @var class node::field_value_listener<sfbool>
 *
 * @brief <code>sfbool</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sfcolor>
 *
 * @brief <code>sfcolor</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sffloat>
 *
 * @brief <code>sffloat</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sfimage>
 *
 * @brief <code>sfimage</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sfint32>
 *
 * @brief <code>sfint32</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sfnode>
 *
 * @brief <code>sfnode</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sfrotation>
 *
 * @brief <code>sfrotation</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sfstring>
 *
 * @brief <code>sfstring</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sftime>
 *
 * @brief <code>sftime</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sfvec2f>
 *
 * @brief <code>sfvec2f</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<sfvec3f>
 *
 * @brief <code>sfvec3f</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mfcolor>
 *
 * @brief <code>mfcolor</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mffloat>
 *
 * @brief <code>mffloat</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mfint32>
 *
 * @brief <code>mfint32</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mfnode>
 *
 * @brief <code>mfnode</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mfrotation>
 *
 * @brief <code>mfrotation</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mfstring>
 *
 * @brief <code>mfstring</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mftime>
 *
 * @brief <code>mftime</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mfvec2f>
 *
 * @brief <code>mfvec2f</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::field_value_listener<mfvec3f>
 *
 * @brief <code>mfvec3f</code> <code>field_value_listener</code>.
 */

/**
 * @var class node::exposedfield<sfbool>
 *
 * @brief <code>sfbool</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sfcolor>
 *
 * @brief <code>sfcolor</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sffloat>
 *
 * @brief <code>sffloat</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sfimage>
 *
 * @brief <code>sfimage</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sfint32>
 *
 * @brief <code>sfint32</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sfnode>
 *
 * @brief <code>sfnode</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sfrotation>
 *
 * @brief <code>sfrotation</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sfstring>
 *
 * @brief <code>sfstring</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sftime>
 *
 * @brief <code>sftime</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sfvec2f>
 *
 * @brief <code>sfvec2f</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<sfvec3f>
 *
 * @brief <code>sfvec3f</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mfcolor>
 *
 * @brief <code>mfcolor</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mffloat>
 *
 * @brief <code>mffloat</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mfint32>
 *
 * @brief <code>mfint32</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mfnode>
 *
 * @brief <code>mfnode</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mfrotation>
 *
 * @brief <code>mfrotation</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mfstring>
 *
 * @brief <code>mfstring</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mftime>
 *
 * @brief <code>mftime</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mfvec2f>
 *
 * @brief <code>mfvec2f</code> <code>exposedfield</code>.
 */

/**
 * @var class node::exposedfield<mfvec3f>
 *
 * @brief <code>mfvec3f</code> <code>exposedfield</code>.
 */

/**
 * @internal
 *
 * @var boost::recursive_mutex node::mutex_
 *
 * @brief Object mutex.
 */

/**
 * @internal
 *
 * @var const node_type & node::type_
 *
 * @brief The type information object for the node.
 */

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
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the Scope associated with the instance.
 */
node::node(const node_type & type,
           const boost::shared_ptr<openvrml::scope> & scope) throw ():
    scope_(scope),
    scene_(0),
    modified_(false),
    bounding_volume_dirty_(false),
    type_(type)
{}

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
 * @brief The type information object for the node.
 *
 * @return the type information object for the node.
 */
const node_type & node::type() const throw ()
{
    return this->type_;
}

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
 * @fn const boost::shared_ptr<const openvrml::scope> & node::scope() const throw ()
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
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    if (!this->scene_) {
        this->scene_ = &scene;
        this->do_initialize(timestamp);

        const node_interface_set & interfaces = this->type_.interfaces();
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
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
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
 * @brief Get an event listener.
 *
 * @param id    an eventIn identifier.
 *
 * @exception unsupported_interface  if the node has no eventIn @p id.
 */
event_listener & node::event_listener(const std::string & id)
    throw (unsupported_interface)
{
    //
    // No need to lock here; the set of event listeners for a node instance
    // cannot change.
    //
    return this->do_event_listener(id);
}

/**
 * @brief Get an event emitter.
 *
 * @param id    an eventOut identifier.
 *
 * @exception unsupported_interface  if the node has no eventOut @p id.
 */
event_emitter & node::event_emitter(const std::string & id)
    throw (unsupported_interface)
{
    //
    // No need to lock here; the set of event emitters for a node instance
    // cannot change.
    //
    return this->do_event_emitter(id);
}

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
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    if (this->scene_) {
        this->do_shutdown(timestamp);
        this->scene_ = 0;

        const node_interface_set & interfaces = this->type_.interfaces();
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
 * @brief Cast to a navigation_info_node.
 *
 * Default implementation returns 0.
 *
 * @return 0.
 */
navigation_info_node * node::to_navigation_info() throw ()
{
    return 0;
}

/**
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @internal
 *
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
 * @brief Set the modified flag.
 *
 * Indicates the node needs to be revisited for rendering.
 *
 * @param value
 */
void node::modified(const bool value)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    this->modified_ = value;
    if (this->modified_) { this->type_.node_class().browser().modified(true); }
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
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
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
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    this->bounding_volume_dirty_ = value;
    if (value) { // only if dirtying, not clearing
        this->type_.node_class().browser().flags_need_updating = true;
    }
}

/**
 * Return true if the node's bounding volume needs to be
 * recalculated.
 */
bool node::bounding_volume_dirty() const
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    if (this->type_.node_class().browser().flags_need_updating) {
        this->type_.node_class().browser().update_flags();
        this->type_.node_class().browser().flags_need_updating = false;
    }
    return this->bounding_volume_dirty_;
}

/**
 * @brief Emit an event.
 *
 * @param emitter   an event_emitter.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node::emit_event(openvrml::event_emitter & emitter,
                      const double timestamp)
    throw (std::bad_alloc)
{
    emitter.emit_event(timestamp);
}

/**
 * @fn boost::recursive_mutex & node::mutex() const throw ()
 *
 * @brief Get the mutex associated with the <code>node</code>.
 *
 * Concrete node types should lock the <code>node</code> mutex when modifying
 * field values outside the rendering thread.
 *
 * @return the mutex associated with the <code>node</code>.
 */

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
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    for (size_t i = 0; i < indent; ++i) { out << ' '; }
    std::string nodeId = this->id();
    if (!nodeId.empty()) { out << "DEF " << nodeId << " "; }
    out << this->type_.id() << " { ";
    const node_interface_set & interfaces = this->type_.interfaces();
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
 * @fn event_listener & node::do_event_listener(const std::string & id) throw (unsupported_interface)
 *
 * @brief Get an event listener.
 *
 * This method is called by node::event_listener. Subclasses must implement
 * this method.
 *
 * @param id    eventIn identifier.
 *
 * @return the event listener.
 *
 * @exception unsupported_interface if the node has no eventIn @p id.
 */

/**
 * @fn event_emitter & node::do_event_emitter(const std::string & id) throw (unsupported_interface)
 *
 * @brief Get an event emitter.
 *
 * This method is called by node::event_emitter. Subclasses must implement this
 * method.
 *
 * @param id    eventOut identifier.
 *
 * @return the event emitter.
 *
 * @exception unsupported_interface if the node has no eventOut @p id.
 */

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

namespace {
    template <typename FieldValue>
    bool add_listener(event_emitter & emitter, event_listener & listener)
        throw (std::bad_alloc, std::bad_cast)
    {
        return static_cast<field_value_emitter<FieldValue> &>(emitter)
            .add(dynamic_cast<field_value_listener<FieldValue> &>(listener));
    }
}

/**
 * @ingroup nodes
 *
 * @brief Add a route from an eventOut of this node to an eventIn of another
 *      node.
 *
 * If the route being added already exists, this method has no effect.
 *
 * @param from      source node.
 * @param eventout  an eventOut of @p from.
 * @param to        destination node.
 * @param eventin   an eventIn of @p to.
 *
 * @return @c true if a route was successfully added; @c false otherwise (if
 *         the route already existed).
 *
 * @exception std::bad_alloc            if memory allocation fails.
 * @exception unsupported_interface     if the node has no eventOut
 *                                      @p eventout; or if @p to has no eventIn
 *                                      @p eventin.
 * @exception field_value_type_mismatch if @p eventout and @p eventin have
 *                                      different field value types.
 *
 * @pre @p to_node is not null.
 */
bool add_route(node & from_node,
               const std::string & from_eventout,
               node & to_node,
               const std::string & to_eventin)
    throw (std::bad_alloc, unsupported_interface, field_value_type_mismatch)
{
    using std::bad_cast;

    event_emitter & emitter = from_node.event_emitter(from_eventout);
    event_listener & listener = to_node.event_listener(to_eventin);
    bool added_route = false;
    try {
        switch (emitter.value().type()) {
        case field_value::sfbool_id:
            added_route = add_listener<sfbool>(emitter, listener);
            break;
        case field_value::sfcolor_id:
            added_route = add_listener<sfcolor>(emitter, listener);
            break;
        case field_value::sffloat_id:
            added_route = add_listener<sffloat>(emitter, listener);
            break;
        case field_value::sfimage_id:
            added_route = add_listener<sfimage>(emitter, listener);
            break;
        case field_value::sfint32_id:
            added_route = add_listener<sfint32>(emitter, listener);
            break;
        case field_value::sfnode_id:
            added_route = add_listener<sfnode>(emitter, listener);
            break;
        case field_value::sfrotation_id:
            added_route = add_listener<sfrotation>(emitter, listener);
            break;
        case field_value::sfstring_id:
            added_route = add_listener<sfstring>(emitter, listener);
            break;
        case field_value::sftime_id:
            added_route = add_listener<sftime>(emitter, listener);
            break;
        case field_value::sfvec2f_id:
            added_route = add_listener<sfvec2f>(emitter, listener);
            break;
        case field_value::sfvec3f_id:
            added_route = add_listener<sfvec3f>(emitter, listener);
            break;
        case field_value::mfcolor_id:
            added_route = add_listener<mfcolor>(emitter, listener);
            break;
        case field_value::mffloat_id:
            added_route = add_listener<mffloat>(emitter, listener);
            break;
        case field_value::mfint32_id:
            added_route = add_listener<mfint32>(emitter, listener);
            break;
        case field_value::mfnode_id:
            added_route = add_listener<mfnode>(emitter, listener);
            break;
        case field_value::mfrotation_id:
            added_route = add_listener<mfrotation>(emitter, listener);
            break;
        case field_value::mfstring_id:
            added_route = add_listener<mfstring>(emitter, listener);
            break;
        case field_value::mftime_id:
            added_route = add_listener<mftime>(emitter, listener);
            break;
        case field_value::mfvec2f_id:
            added_route = add_listener<mfvec2f>(emitter, listener);
            break;
        case field_value::mfvec3f_id:
            added_route = add_listener<mfvec3f>(emitter, listener);
            break;
        }
    } catch (const bad_cast &) {
        throw field_value_type_mismatch();
    }
    return added_route;
}

namespace {
    template <typename FieldValue>
    bool remove_listener(event_emitter & emitter, event_listener & listener)
        throw (std::bad_cast)
    {
        return static_cast<field_value_emitter<FieldValue> &>(emitter).remove(
            dynamic_cast<field_value_listener<FieldValue> &>(listener));
    }
}

/**
 * @ingroup nodes
 *
 * @brief Remove a route from an eventOut of this node to an eventIn of another
 *      node.
 *
 * If no such route exists, this method has no effect.
 *
 * @param from      source node.
 * @param eventout  an eventOut of @p from.
 * @param to        destination node.
 * @param eventin   an eventIn of @p to.
 *
 * @return @c true if a route was deleted; @c false otherwise (if no such route
 *         existed).
 *
 * @exception unsupported_interface if @p from has no eventOut @p eventout or
 *                                  if @p to has no eventIn @p eventin.
 */
bool delete_route(node & from,
                  const std::string & eventout,
                  node & to,
                  const std::string & eventin)
    throw (unsupported_interface)
{
    using std::bad_cast;

    event_emitter & emitter = from.event_emitter(eventout);
    event_listener & listener = to.event_listener(eventin);

    bool deleted_route = false;

    try {
        switch (emitter.value().type()) {
        case field_value::sfbool_id:
            deleted_route = remove_listener<sfbool>(emitter, listener);
            break;
        case field_value::sfcolor_id:
            deleted_route = remove_listener<sfcolor>(emitter, listener);
            break;
        case field_value::sffloat_id:
            deleted_route = remove_listener<sffloat>(emitter, listener);
            break;
        case field_value::sfimage_id:
            deleted_route = remove_listener<sfimage>(emitter, listener);
            break;
        case field_value::sfint32_id:
            deleted_route = remove_listener<sfint32>(emitter, listener);
            break;
        case field_value::sfnode_id:
            deleted_route = remove_listener<sfnode>(emitter, listener);
            break;
        case field_value::sfrotation_id:
            deleted_route = remove_listener<sfrotation>(emitter, listener);
            break;
        case field_value::sfstring_id:
            deleted_route = remove_listener<sfstring>(emitter, listener);
            break;
        case field_value::sftime_id:
            deleted_route = remove_listener<sftime>(emitter, listener);
            break;
        case field_value::sfvec2f_id:
            deleted_route = remove_listener<sfvec2f>(emitter, listener);
            break;
        case field_value::sfvec3f_id:
            deleted_route = remove_listener<sfvec3f>(emitter, listener);
            break;
        case field_value::mfcolor_id:
            deleted_route = remove_listener<mfcolor>(emitter, listener);
            break;
        case field_value::mffloat_id:
            deleted_route = remove_listener<mffloat>(emitter, listener);
            break;
        case field_value::mfint32_id:
            deleted_route = remove_listener<mfint32>(emitter, listener);
            break;
        case field_value::mfnode_id:
            deleted_route = remove_listener<mfnode>(emitter, listener);
            break;
        case field_value::mfrotation_id:
            deleted_route = remove_listener<mfrotation>(emitter, listener);
            break;
        case field_value::mfstring_id:
            deleted_route = remove_listener<mfstring>(emitter, listener);
            break;
        case field_value::mftime_id:
            deleted_route = remove_listener<mftime>(emitter, listener);
            break;
        case field_value::mfvec2f_id:
            deleted_route = remove_listener<mfvec2f>(emitter, listener);
            break;
        case field_value::mfvec3f_id:
            deleted_route = remove_listener<mfvec3f>(emitter, listener);
            break;
        }
    } catch (const bad_cast &) {
        //
        // Do nothing.  If route removal fails, we simply return false.
        //
    }
    return deleted_route;
}


/**
 * @fn template <> script_node * node_cast<script_node *>(node * n) throw ()
 *
 * @brief Cast to a script_node.
 *
 * @param n node.
 *
 * @return a script_node pointer to the node pointed to by @p n, or 0 if the
 *         node is not a script_node.
 */

/**
 * @fn template <> appearance_node * node_cast<appearance_node *>(node * n) throw ()
 *
 * @brief Cast to an appearance_node.
 *
 * @param n node.
 *
 * @return an appearance_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not an appearance_node.
 */

/**
 * @fn template <> child_node * node_cast<child_node *>(node * n) throw ()
 *
 * @brief Cast to a child_node.
 *
 * @param n node.
 *
 * @return a child_node pointer to the node pointed to by @p n, or 0 if the
 *         node is not a child_node.
 */

/**
 * @fn template <> color_node * node_cast<color_node *>(node * n) throw ()
 *
 * @brief Cast to a color_node.
 *
 * @param n node.
 *
 * @return a color_node pointer to the node pointed to by @p n, or 0 if the
 *         node is not a color_node.
 */

/**
 * @fn template <> coordinate_node * node_cast<coordinate_node *>(node * n) throw ()
 *
 * @brief Cast to a coordinate_node.
 *
 * @param n node.
 *
 * @return a coordinate_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a coordinate_node.
 */

/**
 * @fn template <> font_style_node * node_cast<font_style_node *>(node * n) throw ()
 *
 * @brief Cast to a font_style_node.
 *
 * @param n node.
 *
 * @return a font_style_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a font_style_node.
 */

/**
 * @fn template <> geometry_node * node_cast<geometry_node *>(node * n) throw ()
 *
 * @brief Cast to a geometry_node.
 *
 * @param n node.
 *
 * @return a geometry_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a geometry_node.
 */

/**
 * @fn template <> grouping_node * node_cast<grouping_node *>(node * n) throw ()
 *
 * @brief Cast to a grouping_node.
 *
 * @param n node.
 *
 * @return a grouping_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a grouping_node.
 */

/**
 * @fn template <> material_node * node_cast<material_node *>(node * n) throw ()
 *
 * @brief Cast to a material_node.
 *
 * @param n node.
 *
 * @return a material_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a material_node.
 */

/**
 * @fn template <> normal_node * node_cast<normal_node *>(node * n) throw ()
 *
 * @brief Cast to a normal_node.
 *
 * @param n node.
 *
 * @return a normal_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a normal_node.
 */

/**
 * @fn template <> sound_source_node * node_cast<sound_source_node *>(node * n) throw ()
 *
 * @brief Cast to a sound_source_node.
 *
 * @param n node.
 *
 * @return a sound_source_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a sound_source_node.
 */

/**
 * @fn template <> texture_node * node_cast<texture_node *>(node * n) throw ()
 *
 * @brief Cast to a texture_node.
 *
 * @param n node.
 *
 * @return a texture_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a texture_node.
 */

/**
 * @fn template <> texture_coordinate_node * node_cast<texture_coordinate_node *>(node * n) throw ()
 *
 * @brief Cast to a texture_coordinate_node.
 *
 * @param n node.
 *
 * @return a texture_coordinate_node pointer to the node pointed to by @p n, or
 *         0 if the node is not a texture_coordinate_node.
 */

/**
 * @fn template <> texture_transform_node * node_cast<texture_transform_node *>(node * n) throw ()
 *
 * @brief Cast to a texture_transform_node.
 *
 * @param n node.
 *
 * @return a texture_transform_node pointer to the node pointed to by @p n, or
 *         0 if the node is not a texture_transform_node.
 */

/**
 * @fn template <> transform_node * node_cast<transform_node *>(node * n) throw ()
 *
 * @brief Cast to a transform_node.
 *
 * @param n node.
 *
 * @return a transform_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a transform_node.
 */

/**
 * @fn template <> viewpoint_node * node_cast<viewpoint_node *>(node * n) throw ()
 *
 * @brief Cast to a viewpoint_node.
 *
 * @param n node.
 *
 * @return a viewpoint_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a viewpoint_node.
 */


/**
 * @class appearance_node
 *
 * @ingroup nodes
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
                                 const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
appearance_node::~appearance_node() throw ()
{}

/**
 * @brief Insert appearance when rendering.
 *
 * @param v         viewer.
 * @param context   rendering_context.
 */
void appearance_node::render_appearance(viewer & v, rendering_context context)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());
    this->do_render_appearance(v, context);
    this->modified(false);
}

/**
 * @brief render_appearance implementation.
 *
 * @param v         viewer.
 * @param context   rendering_context.
 */
void appearance_node::do_render_appearance(viewer & v,
                                           rendering_context context)
{}

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
 * @ingroup nodes
 *
 * @brief Abstract base class for child nodes.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
child_node::child_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destructor.
 */
child_node::~child_node() throw ()
{}

/**
 * @brief Called when the node is relocated to a new position in the scene
 *      graph.
 *
 * This function delegates to the virtual function do_relocate. relocate
 * should be called by eventIn handlers that receive nodes.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void child_node::relocate() throw (std::bad_alloc)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());

    typedef void (child_node::* Do_relocate)();

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
            child_node * const child = node_cast<child_node *>(&n);
            if (child) { (child->*this->do_relocate)(); }
        }
    };

    Do_relocate do_reloc = &child_node::do_relocate;

    RelocateTraverser(do_reloc).traverse(*this);
}

/**
 * @brief Render the node.
 *
 * Actually, most of the rendering work is delegated to the viewer, but this
 * method is responsible for traversal to the node's renderable children,
 * including culling. Each node class needs to implement this routine
 * appropriately. It's not abstract since it doesn't make sense to call render
 * on some nodes. Alternative would be to break render out into a seperate
 * mixins class, but that's probably overkill.
 *
 * @param v         viewer implementation responsible for actually doing the
 *                  drawing.
 * @param context   generic context argument; holds things like the accumulated
 *                  modelview transform.
 */
void child_node::render_child(viewer & v, const rendering_context context)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());
    this->do_render_child(v, context);
    this->modified(false);
}

/**
 * @brief render_child implementation.
 *
 * Rendered child nodes should override this method.
 *
 * @param v         viewer implementation responsible for actually doing the
 *                  drawing.
 * @param context   generic context argument; holds things like the accumulated
 *                  modelview transform.
 */
void child_node::do_render_child(viewer & v, rendering_context context)
{}

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
 * @brief node subclass-specific relocation update.
 *
 * This method is called by child_node::relocate. Subclasses of child_node
 * should override this method for any subclass-specific updates that need to
 * be performed following relocation of a node to a new position in the scene
 * graph (for example, updating a node_path).
 *
 * The default implementation of this method does nothing.
 */
void child_node::do_relocate() throw (std::bad_alloc)
{}


/**
 * @class color_node
 *
 * @ingroup nodes
 *
 * @brief Abstract base class for color nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
color_node::color_node(const node_type & type, const boost::shared_ptr<openvrml::scope> & scope)
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
                                 const boost::shared_ptr<openvrml::scope> & scope)
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
 * @ingroup nodes
 *
 * @brief Abstract base class for font style nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
font_style_node::
font_style_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope) throw ():
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
 * @ingroup nodes
 *
 * @brief Abstract base class for geometry nodes.
 */

/**
 * @var viewer::object_t geometry_node::geometry_reference
 *
 * @brief Identifier for a geometry object in the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
geometry_node::geometry_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope),
    geometry_reference(0)
{}

/**
 * @brief Destroy.
 *
 * @todo Proper resource deallocation in the <code>viewer</code> depends on the
 *       <code>viewer</code> <strong>not</strong> having been decoupled from
 *       the browser. We need to handle this better via some refcounting
 *       scheme.
 */
geometry_node::~geometry_node() throw ()
{
    if (this->geometry_reference) {
        assert(this->type().node_class().browser().viewer());
        this->type().node_class().browser().viewer()
            ->remove_object(this->geometry_reference);
    }
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
 * @brief Insert geometry into a viewer.
 *
 * @param v         viewer.
 * @param context   rendering context.
 *
 * @return object identifier for the inserted geometry.
 */
viewer::object_t geometry_node::render_geometry(viewer & v,
                                                rendering_context context)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());

    if (this->geometry_reference != 0 && this->modified()) {
        v.remove_object(this->geometry_reference);
        this->geometry_reference = 0;
    }

    if (this->geometry_reference != 0) {
        v.insert_reference(this->geometry_reference);
    } else {
        this->geometry_reference = this->do_render_geometry(v, context);
        this->modified(false);
    }
    return this->geometry_reference;
}

/**
 * @brief Indicates whether the geometry should be drawn with the emissive
 *        color.
 *
 * @return @c true if the geometry should be drawn with the emissive color;
 *         @c false otherwise (if the geometry should be drawn with the diffuse
 *         color).
 */
bool geometry_node::emissive() const throw ()
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());
    return this->do_emissive();
}

/**
 * @brief render_geometry implementation.
 *
 * @param v         viewer.
 * @param context   rendering context.
 *
 * @return object identifier for the inserted geometry.
 */
viewer::object_t geometry_node::do_render_geometry(viewer & v,
                                                   rendering_context context)
{
    return 0;
}

/**
 * @brief emissive implementation.
 *
 * @return @c false.
 */
bool geometry_node::do_emissive() const throw ()
{
    return false;
}

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
 * @ingroup nodes
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
                             const boost::shared_ptr<openvrml::scope> & scope)
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
 * @ingroup nodes
 *
 * @brief Abstract base class for material nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
material_node::material_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope)
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
 * @class navigation_info_node
 *
 * @ingroup nodes
 *
 * @brief Abstract base class for normal nodes.
 */

/**
 * @brief Construct.
 *
 * @param t     the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
navigation_info_node::
navigation_info_node(const node_type & t,
                     const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(t, scope),
    child_node(t, scope)
{}

/**
 * @brief Destroy.
 */
navigation_info_node::~navigation_info_node() throw ()
{}

/**
 * @brief Cast to a navigation_info_node.
 *
 * @return a pointer to this navigation_info_node.
 */
navigation_info_node * navigation_info_node::to_navigation_info() throw ()
{
    return this;
}

/**
 * @fn const std::vector<float> & navigation_info_node::avatar_size() const throw ()
 *
 * @brief Get the avatar dimensions.
 *
 * @return the avatar dimensions.
 */

/**
 * @fn bool navigation_info_node::headlight() const throw ()
 *
 * @brief Get the state of the headlight.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */

/**
 * @fn float navigation_info_node::speed() const throw ()
 *
 * @brief Get the current speed of the user view.
 *
 * @return the current speed of the user view.
 */

/**
 * @fn const std::vector<std::string> & navigation_info_node::type() const throw ()
 *
 * @brief Get the navigation type.
 *
 * @return the navigation type.
 */

/**
 * @fn float navigation_info_node::visibility_limit() const throw ()
 *
 * @brief Get the visibility limit.
 *
 * @return the visibility limit.
 */


/**
 * @class normal_node
 *
 * @ingroup nodes
 *
 * @brief Abstract base class for normal nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
normal_node::normal_node(const node_type & type, const boost::shared_ptr<openvrml::scope> & scope)
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
 * @ingroup nodes
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
                                     const boost::shared_ptr<openvrml::scope> & scope)
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
sound_source_node * sound_source_node::to_sound_source() throw ()
{
    return this;
}


/**
 * @class texture_node
 *
 * @ingroup nodes
 *
 * @brief Abstract base class for texture nodes.
 */

/**
 * @var viewer::texture_object_t texture_node::texture_reference
 *
 * @brief Identifier for a texture object in the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
texture_node::texture_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope),
    texture_reference(0)
{}

/**
 * @brief Destroy.
 *
 * @todo Proper resource deallocation in the <code>viewer</code> depends on the
 *       <code>viewer</code> <strong>not</strong> having been decoupled from
 *       the browser. We need to handle this better via some refcounting
 *       scheme.
 */
texture_node::~texture_node() throw ()
{
    if (this->texture_reference) {
        assert(this->type().node_class().browser().viewer());
        this->type().node_class().browser().viewer()
            ->remove_texture_object(this->texture_reference);
    }
}

/**
 * @brief Insert a texture into a viewer.
 *
 * @param v viewer.
 *
 * @return object identifier for the inserted texture.
 */
viewer::texture_object_t texture_node::render_texture(viewer & v)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());

    if (this->texture_reference != 0 && this->modified()) {
        v.remove_texture_object(this->texture_reference);
        this->texture_reference = 0;
    }

    if (this->texture_reference != 0) {
        v.insert_texture_reference(this->texture_reference,
                                   this->image().comp());
    } else {
        this->texture_reference = this->do_render_texture(v);
        this->modified(false);
    }
    return this->texture_reference;
}

/**
 * @brief render_texture implementation.
 *
 * @param v viewer.
 *
 * @return object identifier for the inserted texture.
 */
viewer::texture_object_t texture_node::do_render_texture(viewer & v)
{
    return 0;
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
 * @fn const image & texture_node::image() const throw ()
 *
 * @brief The image.
 *
 * @return the image.
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
 * @ingroup nodes
 *
 * @brief Abstract base class for texture coordinate nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
texture_coordinate_node::
texture_coordinate_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
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
 * @ingroup nodes
 *
 * @brief Abstract base class for texture transform nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
texture_transform_node::
texture_transform_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destructor.
 */
texture_transform_node::~texture_transform_node() throw ()
{}

/**
 * @brief Render the texture transform.
 *
 * @param v viewer.
 */
void texture_transform_node::render_texture_transform(viewer & v)
{
    this->do_render_texture_transform(v);
    this->modified(false);
}

/**
 * @brief Render the texture transform.
 *
 * @param v viewer.
 */
void texture_transform_node::do_render_texture_transform(viewer & v)
{}

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
 * @ingroup nodes
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
                               const boost::shared_ptr<openvrml::scope> & scope)
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
 * @ingroup nodes
 *
 * @brief Abstract base class for texture transform nodes.
 */

/**
 * @brief Constructor.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the Scope the node belongs to.
 */
viewpoint_node::
viewpoint_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope)
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
 * @ingroup nodes
 *
 * @brief Traverse the children of each node in a node hierarchy only once.
 *
 * The <code>node_traverser</code> provides a generalized traversal mechanism
 * that avoids redundantly traversing branches of the node hierarchy. If a node
 * occurs multiple places in a branch, <b>the children of that node will be
 * visted in the traversal only once</b>.
 *
 * For each node encountered in the traversal, <code>node_traverser</code>
 * does the following:
 *
 * -# Call <code>node_traverser::on_entering</code>.
 * -# Traverse descendants, if any.
 * -# Call <code>node_traverser::on_leaving</code>.
 *
 * Concrete subclasses of <code>node_traverser</code> implement the
 * <code>@link node_traverser::on_entering on_entering@endlink</code> and/or
 * <code>@link node_traverser::on_leaving on_leaving@endlink</code> member
 * functions in order to perform some operation(s) on each node. The traversal
 * can be ended prematurely by calling
 * <code>node_traverser::halt_traversal</code>.
 */

/**
 * @internal
 *
 * @var std::set<node *> node_traverser::traversed_nodes
 *
 * @brief The set of nodes that have already been traversed.
 */

/**
 * @internal
 *
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
 * @brief Destroy.
 */
node_traverser::~node_traverser() throw ()
{}

/**
 * @brief Traverse a node.
 *
 * No guarantee is made about the state of the node_traverser instance in the
 * event that this method throws.
 *
 * In addition to <code>std::bad_alloc</code>, this function throws any
 * exception thrown from
 * <code>@link node_traverser::on_entering on_entering@endlink</code> or
 * <code>@link node_traverser::on_leaving on_leaving@endlink</code>.
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
 * @brief Traverse a <code>node</code>.
 *
 * @overload
 *
 * @param node  the root node of the branch to traverse.
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
 * @brief Traverse a <code>std::vector</code> of <code>node</code>s.
 *
 * @overload
 *
 * @param nodes  the root @link openvrml::node nodes@endlink of the branch to
 *               traverse.
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
 * If this method is called during a traversal, no more descendant
 * @link openvrml::node nodes@endlink will be traversed. Note that if
 * <code>halt_traversal</code> is called in the implementation of
 * <code>@link node_traverser::on_entering on_entering@endlink</code>,
 * <code>@link node_traverser::on_leaving on_leaving@endlink</code> will still
 * be called for the current node and any parent nodes (that is, any node for
 * which <code>@link node_traverser::on_entering on_entering@endlink</code> has
 * been called). Implementations of
 * <code>@link node_traverser::on_leaving on_leaving@endlink</code> can call
 * <code>node_traverser::halted</code> to check whether the traversal has been
 * halted.
 */
void node_traverser::halt_traversal() throw ()
{
    this->halt = true;
}

/**
 * @brief Indicate whether the traversal has been halted.
 *
 * This function is useful in implementations of
 * <code>@link node_traverser::on_leaving on_leaving@endlink</code> that need
 * to check whether the traversal has been halted.
 *
 * @return @c true if the traversal has been halted; @c false otherwise.
 */
bool node_traverser::halted() throw ()
{
    return this->halt;
}

/**
 * @internal
 *
 * @brief Traverse a <code>node</code>.
 *
 * @param n  the <code>node</code> to traverse.
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

        const node_interface_set & interfaces = n.type().interfaces();
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
                    for (size_t i = 0;
                         i < children.value.size() && !this->halt;
                         ++i) {
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
 * @brief Called for each <code>node</code> in the traversal <em>before</em>
 *      traversing the its descendants.
 *
 * @param n  the <code>node</code> currently being traversed.
 */
void node_traverser::on_entering(node & n)
{}

/**
 * @brief Called for each <code>node</code> in the traversal <em>after</em>
 *      traversing the its descendants.
 *
 * @param n  the <code>node</code> currently being traversed.
 */
void node_traverser::on_leaving(node & n)
{}

} // namespace openvrml
