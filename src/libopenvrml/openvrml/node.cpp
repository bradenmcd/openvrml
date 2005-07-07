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
# include "browser.h"

/**
 * @defgroup nodes Nodes
 */

/**
 * @class openvrml::unsupported_interface
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
openvrml::unsupported_interface::
unsupported_interface(const node_interface & interface)
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
openvrml::unsupported_interface::
unsupported_interface(const node_type & type, const std::string & interface_id)
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
openvrml::unsupported_interface::
unsupported_interface(const node_type & type,
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
openvrml::unsupported_interface::~unsupported_interface() throw ()
{}


/**
 * @class openvrml::node_interface
 *
 * @ingroup nodes
 *
 * @brief Type information for an interface of a node.
 */

/**
 * @enum openvrml::node_interface::type_id
 *
 * @brief Identify the type of interface.
 */

/**
 * @var openvrml::node_interface::type_id openvrml::node_interface::invalid_type_id
 *
 * @brief Used to indicate an invalid or unknown interface type.
 */

/**
 * @var openvrml::node_interface::type_id openvrml::node_interface::eventin_id
 *
 * @brief An eventIn.
 */

/**
 * @var openvrml::node_interface::type_id openvrml::node_interface::eventout_id
 *
 * @brief An eventOut.
 */

/**
 * @var openvrml::node_interface::type_id openvrml::node_interface::exposedfield_id
 *
 * @brief An exposedField.
 */

/**
 * @var openvrml::node_interface::type_id openvrml::node_interface::field_id
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
 * @relatesalso openvrml::node_interface
 *
 * If @p type is node_interface::invalid_type_id, @c failbit is set on @p out.
 *
 * @param out   an output stream.
 * @param type  a node interface type.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out,
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
std::istream & openvrml::operator>>(std::istream & in,
                                    node_interface::type_id & type)
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
 * @var openvrml::node_interface::type
 *
 * @brief The interface type.
 */

/**
 * @var openvrml::node_interface::field_type
 *
 * @brief The field data type handled by this interface.
 */

/**
 * @var openvrml::node_interface::id
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
openvrml::node_interface::node_interface(const type_id type,
                                         const field_value::type_id field_type,
                                         const std::string & id):
    type(type),
    field_type(field_type),
    id(id)
{}

/**
 * @relatesalso openvrml::node_interface
 *
 * @brief Compare for equality.
 *
 * @param lhs   a node_interface.
 * @param rhs   a node_interface.
 *
 * @return @c true if the two node_interfaces are equal, @c false otherwise.
 */
bool openvrml::operator==(const node_interface & lhs,
                          const node_interface & rhs)
    throw ()
{
    return lhs.type == rhs.type
        && lhs.field_type == rhs.field_type
        && lhs.id == rhs.id;
}

/**
 * @relatesalso openvrml::node_interface
 *
 * @brief Compare for openvrml::inequality.
 *
 * @param lhs   a node_interface.
 * @param rhs   a node_interface.
 *
 * @return @c true if the two node_interfaces are equal, @c false otherwise.
 */
bool openvrml::operator!=(const node_interface & lhs,
                          const node_interface & rhs)
    throw ()
{
    return !(lhs == rhs);
}

/**
 * @relatesalso openvrml::node_interface
 *
 * @brief Stream output.
 *
 * @param out       output stream.
 * @param interface node_interface.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out,
                                    const node_interface & interface)
{
    return out << interface.type << ' ' << interface.field_type << ' '
               << interface.id;
}

/**
 * @relatesalso openvrml::node_interface
 *
 * @brief Stream input.
 *
 * @param in        input stream.
 * @param interface node_interface.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in,
                                    node_interface & interface)
{
    return in >> interface.type >> interface.field_type >> interface.id;
}


/**
 * @struct openvrml::node_interface_matches_eventin
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
 * @fn openvrml::node_interface_matches_eventin::result_type openvrml::node_interface_matches_eventin::operator()(const first_argument_type & interface, const second_argument_type & eventin_id) const
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
 * @struct openvrml::node_interface_matches_eventout
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
 * @fn openvrml::node_interface_matches_eventout::result_type openvrml::node_interface_matches_eventout::operator()(const first_argument_type & interface, const second_argument_type & eventout_id) const
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
 * @struct openvrml::node_interface_matches_exposedfield
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
 * @fn openvrml::node_interface_matches_exposedfield::result_type openvrml::node_interface_matches_exposedfield::operator()(const first_argument_type & interface, const second_argument_type & exposedfield_id) const
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
 * @struct openvrml::node_interface_matches_field
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
 * @fn openvrml::node_interface_matches_field::result_type openvrml::node_interface_matches_field::operator()(const first_argument_type & interface, const second_argument_type & field_id) const
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
 * @struct openvrml::node_interface_compare
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
 * @fn openvrml::node_interface_compare::result_type openvrml::node_interface_compare::operator()(const first_argument_type & lhs, const second_argument_type & rhs) const
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
 * @typedef std::set<openvrml::node_interface, openvrml::node_interface_compare> openvrml::node_interface_set
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
const openvrml::node_interface_set::const_iterator
openvrml::find_interface(const node_interface_set & interfaces,
                         const std::string & id)
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
 * @class openvrml::node_class
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
 * @var openvrml::browser * openvrml::node_class::browser_
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
openvrml::node_class::node_class(openvrml::browser & b) throw ():
    browser_(&b)
{}

/**
 * @brief Destructor.
 */
openvrml::node_class::~node_class() throw ()
{}

/**
 * @brief The <code>browser</code> associated with this
 *        <code>node_class</code>.
 *
 * @return the <code>browser</code> associated with this
 *         <code>node_class</code>.
 */
openvrml::browser & openvrml::node_class::browser() const throw ()
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
void openvrml::node_class::initialize(viewpoint_node * initial_viewpoint,
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
void openvrml::node_class::do_initialize(viewpoint_node * initial_viewpoint,
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
void openvrml::node_class::render(viewer & v) const throw ()
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
void openvrml::node_class::do_render(viewer & v) const throw ()
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
const boost::shared_ptr<openvrml::node_type>
openvrml::node_class::create_type(const std::string & id,
                                  const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    return this->do_create_type(id, interfaces);
}

/**
 * @fn const boost::shared_ptr<openvrml::node_type> openvrml::node_class::do_create_type(const std::string & id, const node_interface_set & interfaces) const throw (unsupported_interface, std::bad_alloc)
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
 * @typedef openvrml::initial_value_map
 *
 * @brief A map of the initial values with which a <code>node</code> is
 *        instantiated.
 */


/**
 * @class openvrml::node_type
 *
 * @ingroup nodes
 *
 * @brief Type information object for <code>node</code>s.
 */

/**
 * @internal
 *
 * @var openvrml::node_class & openvrml::node_type::node_class_
 *
 * @brief The class object associated with the <code>node_type</code>.
 */

/**
 * @internal
 *
 * @var const std::string openvrml::node_type::id_
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
openvrml::node_type::node_type(const openvrml::node_class & c,
                               const std::string & id)
    throw (std::bad_alloc):
    node_class_(c),
    id_(id)
{}

/**
 * @brief Destructor.
 */
openvrml::node_type::~node_type() throw ()
{}

/**
 * @brief The class object associated with the <code>node_type</code>.
 *
 * @return the class object associated with the <code>node_type</code>.
 */
const openvrml::node_class & openvrml::node_type::node_class() const throw ()
{
    return this->node_class_;
}

/**
 * @brief The name of the <code>node_type</code>.
 *
 * @return the name of the <code>node_type</code>.
 */
const std::string & openvrml::node_type::id() const throw ()
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
const openvrml::node_interface_set & openvrml::node_type::interfaces() const
    throw ()
{
    return this->do_interfaces();
}

/**
 * @fn const openvrml::node_interface_set & openvrml::node_type::do_interfaces() const throw ()
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
const openvrml::node_ptr
openvrml::node_type::
create_node(const boost::shared_ptr<scope> & scope,
            const initial_value_map & initial_values) const
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    return this->do_create_node(scope, initial_values);
}

/**
 * @fn const openvrml::node_ptr openvrml::node_type::do_create_node(const boost::shared_ptr<scope> & scope, const initial_value_map & initial_values) const throw (unsupported_interface, std::bad_cast, std::bad_alloc)
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
 * @class openvrml::field_value_type_mismatch
 *
 * @ingroup nodes
 *
 * @brief Thrown when field value types do not match, generally in a @c ROUTE
 *      or @c IS.
 */

/**
 * @brief Constructor.
 */
openvrml::field_value_type_mismatch::field_value_type_mismatch():
    std::logic_error("Field value types do not match.")
{}

/**
 * @brief Destructor.
 */
openvrml::field_value_type_mismatch::~field_value_type_mismatch() throw ()
{}


/**
 * @typedef std::deque<openvrml::node *> openvrml::node_path
 *
 * @ingroup nodes
 *
 * @brief A path to a node in the scene starting with one of the scene root
 *      nodes and ending with the objective node.
 */


/**
 * @fn To * openvrml::node_cast(node * n) throw ()
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
 * @class openvrml::node
 *
 * @ingroup nodes
 *
 * @brief A node in the scene graph.
 */

/**
 * @fn std::ostream & openvrml::node::operator<<(std::ostream & out, const node & n)
 *
 * @brief Stream output.
 */

/**
 * @fn openvrml::script_node * openvrml::node::node_cast<script_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>script_node</code>.
 */

/**
 * @fn openvrml::appearance_node * openvrml::node::node_cast<appearance_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>appearance_node</code>.
 */

/**
 * @fn openvrml::child_node * openvrml::node::node_cast<child_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>child_node</code>.
 */

/**
 * @fn openvrml::color_node * openvrml::node::node_cast<color_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>color_node</code>.
 */

/**
 * @fn openvrml::coordinate_node * openvrml::node::node_cast<coordinate_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>coordinate_node</code>.
 */

/**
 * @fn openvrml::font_style_node * openvrml::node::node_cast<font_style_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>font_style_node</code>.
 */

/**
 * @fn openvrml::geometry_node * openvrml::node::node_cast<geometry_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>geometry_node</code>.
 */

/**
 * @fn openvrml::grouping_node * openvrml::node::node_cast<grouping_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>grouping_node</code>.
 */

/**
 * @fn openvrml::material_node * openvrml::node::node_cast<material_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>material_node</code>.
 */

/**
 * @fn openvrml::navigation_info_node * openvrml::node::node_cast<navigation_info_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>navigation_info_node</code>.
 */

/**
 * @fn openvrml::normal_node * openvrml::node::node_cast<normal_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>normal_node</code>.
 */

/**
 * @fn openvrml::sound_source_node * openvrml::node::node_cast<sound_source_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>sound_source_node</code>.
 */

/**
 * @fn openvrml::texture_node * openvrml::node::node_cast<texture_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>texture_node</code>.
 */

/**
 * @fn openvrml::texture_coordinate_node * openvrml::node::node_cast<texture_coordinate_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>texture_coordinate_node</code>.
 */

/**
 * @fn openvrml::texture_transform_node * openvrml::node::node_cast<texture_transform_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>texture_transform_node</code>.
 */

/**
 * @fn openvrml::transform_node * openvrml::node::node_cast<transform_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>transform_node</code>.
 */

/**
 * @fn openvrml::viewpoint_node * openvrml::node::node_cast<viewpoint_node *>(node * n) throw ()
 *
 * @brief Cast to a <code>viewpoint_node</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfbool>
 *
 * @brief <code>sfbool</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfcolor>
 *
 * @brief <code>sfcolor</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sffloat>
 *
 * @brief <code>sffloat</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfimage>
 *
 * @brief <code>sfimage</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfint32>
 *
 * @brief <code>sfint32</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfnode>
 *
 * @brief <code>sfnode</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfrotation>
 *
 * @brief <code>sfrotation</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfstring>
 *
 * @brief <code>sfstring</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sftime>
 *
 * @brief <code>sftime</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfvec2f>
 *
 * @brief <code>sfvec2f</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<sfvec3f>
 *
 * @brief <code>sfvec3f</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mfcolor>
 *
 * @brief <code>mfcolor</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mffloat>
 *
 * @brief <code>mffloat</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mfint32>
 *
 * @brief <code>mfint32</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mfnode>
 *
 * @brief <code>mfnode</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mfrotation>
 *
 * @brief <code>mfrotation</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mfstring>
 *
 * @brief <code>mfstring</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mftime>
 *
 * @brief <code>mftime</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mfvec2f>
 *
 * @brief <code>mfvec2f</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::field_value_listener<mfvec3f>
 *
 * @brief <code>mfvec3f</code> <code>field_value_listener</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfbool>
 *
 * @brief <code>sfbool</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfcolor>
 *
 * @brief <code>sfcolor</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sffloat>
 *
 * @brief <code>sffloat</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfimage>
 *
 * @brief <code>sfimage</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfint32>
 *
 * @brief <code>sfint32</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfnode>
 *
 * @brief <code>sfnode</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfrotation>
 *
 * @brief <code>sfrotation</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfstring>
 *
 * @brief <code>sfstring</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sftime>
 *
 * @brief <code>sftime</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfvec2f>
 *
 * @brief <code>sfvec2f</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<sfvec3f>
 *
 * @brief <code>sfvec3f</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mfcolor>
 *
 * @brief <code>mfcolor</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mffloat>
 *
 * @brief <code>mffloat</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mfint32>
 *
 * @brief <code>mfint32</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mfnode>
 *
 * @brief <code>mfnode</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mfrotation>
 *
 * @brief <code>mfrotation</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mfstring>
 *
 * @brief <code>mfstring</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mftime>
 *
 * @brief <code>mftime</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mfvec2f>
 *
 * @brief <code>mfvec2f</code> <code>exposedfield</code>.
 */

/**
 * @var class openvrml::node::exposedfield<mfvec3f>
 *
 * @brief <code>mfvec3f</code> <code>exposedfield</code>.
 */

/**
 * @internal
 *
 * @var boost::recursive_mutex openvrml::node::mutex_
 *
 * @brief Object mutex.
 */

/**
 * @internal
 *
 * @var const openvrml::node_type & openvrml::node::type_
 *
 * @brief The type information object for the node.
 */

/**
 * @internal
 *
 * @var boost::shared_ptr<openvrml::scope> openvrml::node::scope_
 *
 * @brief The scope to which the node belongs.
 */

/**
 * @internal
 *
 * @var openvrml::scene * openvrml::node::scene_
 *
 * @brief The scene with which the node is associated.
 */

/**
 * @internal
 *
 * @var bool openvrml::node::modified_
 *
 * @brief Indicate whether the node has been modified.
 *
 * @see openvrml::node::modified
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope associated with the instance.
 */
openvrml::node::node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    type_(type),
    scope_(scope),
    scene_(0),
    modified_(false)
{}

namespace {

    /**
     * @internal
     *
     * @brief Map of node names to node instances.
     */
    typedef std::map<std::string, openvrml::node *> named_node_map_t;

    struct node_is_ : std::unary_function<named_node_map_t::value_type, bool> {
        explicit node_is_(const openvrml::node & n):
            n(&n)
        {}

        bool operator()(const named_node_map_t::value_type & value) const
            throw ()
        {
            return value.second == this->n;
        }

    private:
        const openvrml::node * n;
    };
}

/**
 * @brief Destructor.
 *
 * Remove node name (if any) from the scope.
 */
openvrml::node::~node() throw ()
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
const openvrml::node_type & openvrml::node::type() const throw ()
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
void openvrml::node::id(const std::string & node_id) throw (std::bad_alloc)
{
    assert(this->scope_);
    this->scope_->named_node_map[node_id] = this; // Throws std::bad_alloc.
}

/**
 * @brief Retrieve the name of this node.
 *
 * @return the node name.
 */
const std::string & openvrml::node::id() const throw ()
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
 * @fn const boost::shared_ptr<const openvrml::scope> & openvrml::node::scope() const throw ()
 *
 * @brief Get the scope to which the node belongs.
 *
 * @return the scope to which the node belongs.
 */

/**
 * @fn openvrml::scene * openvrml::node::scene() const throw ()
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
void openvrml::node::initialize(openvrml::scene & scene,
                                const double timestamp)
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
const openvrml::field_value &
openvrml::node::field(const std::string & id) const
    throw (unsupported_interface)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return this->do_field(id);
}

/**
 * @fn const openvrml::field_value & openvrml::node::do_field(const std::string & id) const throw (unsupported_interface)
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
openvrml::event_listener &
openvrml::node::event_listener(const std::string & id)
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
openvrml::event_emitter & openvrml::node::event_emitter(const std::string & id)
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
void openvrml::node::shutdown(const double timestamp) throw ()
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
 * @fn void openvrml::node::do_shutdown(double timestamp) throw ()
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
openvrml::script_node * openvrml::node::to_script() throw ()
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
openvrml::appearance_node * openvrml::node::to_appearance() throw ()
{
    return 0;
}

/**
 * @internal
 *
 * @brief Cast to a <code>bounded_volume_node</code>.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
openvrml::bounded_volume_node * openvrml::node::to_bounded_volume() throw ()
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
openvrml::child_node * openvrml::node::to_child() throw ()
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
openvrml::color_node * openvrml::node::to_color() throw ()
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
openvrml::coordinate_node * openvrml::node::to_coordinate() throw ()
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
openvrml::font_style_node * openvrml::node::to_font_style() throw ()
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
openvrml::geometry_node * openvrml::node::to_geometry() throw ()
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
openvrml::grouping_node * openvrml::node::to_grouping() throw ()
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
openvrml::material_node * openvrml::node::to_material() throw ()
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
openvrml::navigation_info_node * openvrml::node::to_navigation_info() throw ()
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
openvrml::normal_node * openvrml::node::to_normal() throw ()
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
openvrml::sound_source_node * openvrml::node::to_sound_source() throw ()
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
openvrml::texture_node * openvrml::node::to_texture() throw ()
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
openvrml::texture_coordinate_node *
openvrml::node::to_texture_coordinate() throw ()
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
openvrml::texture_transform_node *
openvrml::node::to_texture_transform() throw ()
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
openvrml::transform_node * openvrml::node::to_transform() throw ()
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
openvrml::viewpoint_node * openvrml::node::to_viewpoint() throw ()
{
    return 0;
}

/**
 * @brief Cast to an anchor_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::anchor_node * openvrml::node::to_anchor() const
{
    return 0;
}

/**
 * @brief Cast to an audio_clip_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::audio_clip_node * openvrml::node::to_audio_clip() const
{
    return 0;
}

/**
 * @brief Cast to an abstract_light_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::abstract_light_node * openvrml::node::to_light() const
{
    return 0;
}

/**
 * @brief Cast to a movie_texture_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::movie_texture_node *
openvrml::node::to_movie_texture() const
{
    return 0;
}

/**
 * @brief Cast to a plane_sensor_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::plane_sensor_node *
openvrml::node::to_plane_sensor() const
{
    return 0;
}

/**
 * @brief Cast to a sphere_sensor_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::sphere_sensor_node *
openvrml::node::to_sphere_sensor() const
{
    return 0;
}

/**
 * @brief Cast to a cylinder_sensor_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::cylinder_sensor_node *
openvrml::node::to_cylinder_sensor() const
{
    return 0;
}

/**
 * @brief Cast to a point_light_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::point_light_node *
openvrml::node::to_point_light() const
{
    return 0;
}

/**
 * @brief Cast to a spot_light_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::spot_light_node *
openvrml::node::to_spot_light() const
{
    return 0;
}

/**
 * @brief Cast to a time_sensor_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::time_sensor_node *
openvrml::node::to_time_sensor() const
{
    return 0;
}

/**
 * @brief Cast to a touch_sensor_node.
 *
 * @return 0.
 */
openvrml::vrml97_node::touch_sensor_node *
openvrml::node::to_touch_sensor() const
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
void openvrml::node::modified(const bool value)
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
bool openvrml::node::modified() const
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return this->modified_;
}

/**
 * @brief Emit an event.
 *
 * @param emitter   an event_emitter.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::node::emit_event(openvrml::event_emitter & emitter,
                                const double timestamp)
    throw (std::bad_alloc)
{
    emitter.emit_event(timestamp);
}

/**
 * @fn boost::recursive_mutex & openvrml::node::mutex() const throw ()
 *
 * @brief Get the mutex associated with the <code>node</code>.
 *
 * Concrete node types should lock the <code>node</code> mutex when modifying
 * field values outside the rendering thread.
 *
 * @return the mutex associated with the <code>node</code>.
 */

namespace {
    const short indent_increment_ = 4;

    struct print_field_ : std::unary_function<openvrml::node_interface, void> {
        print_field_(const openvrml::node & n,
                     std::ostream & out,
                     const size_t indent):
            n(n),
            out(out),
            indent(indent)
        {}

        void operator()(const openvrml::node_interface & interface) const
        {
            using openvrml::node_interface;
            if (interface.type == node_interface::exposedfield_id
                    || interface.type == node_interface::field_id) {
                this->out << std::string(this->indent + indent_increment_, ' ')
                          << interface.id << ' '
                          << n.field(interface.id);
            }
        }

    private:
        const openvrml::node & n;
        std::ostream & out;
        const size_t indent;
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
std::ostream & openvrml::node::print(std::ostream & out,
                                     const size_t indent) const
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    for (size_t i = 0; i < indent; ++i) { out << ' '; }
    std::string nodeId = this->id();
    if (!nodeId.empty()) { out << "DEF " << nodeId << " "; }
    out << this->type_.id() << " { ";
    const node_interface_set & interfaces = this->type_.interfaces();
    std::for_each(interfaces.begin(), interfaces.end(),
                  print_field_(*this, out, indent));
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
std::ostream & openvrml::operator<<(std::ostream & out, const node & n)
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
void openvrml::node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{}

/**
 * @fn openvrml::event_listener & openvrml::node::do_event_listener(const std::string & id) throw (unsupported_interface)
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
 * @fn openvrml::event_emitter & openvrml::node::do_event_emitter(const std::string & id) throw (unsupported_interface)
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
void openvrml::node::do_shutdown(const double timestamp) throw ()
{}

namespace {
    template <typename FieldValue>
    bool add_listener(openvrml::event_emitter & emitter,
                      openvrml::event_listener & listener)
        throw (std::bad_alloc, std::bad_cast)
    {
        using openvrml::field_value_emitter;
        using openvrml::field_value_listener;
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
bool openvrml::add_route(node & from,
                         const std::string & eventout,
                         node & to,
                         const std::string & eventin)
    throw (std::bad_alloc, unsupported_interface, field_value_type_mismatch)
{
    using std::bad_cast;

    event_emitter & emitter = from.event_emitter(eventout);
    event_listener & listener = to.event_listener(eventin);
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
        case field_value::invalid_type_id:
            assert(false);
        }
    } catch (const bad_cast &) {
        throw field_value_type_mismatch();
    }
    return added_route;
}

namespace {
    template <typename FieldValue>
    bool remove_listener(openvrml::event_emitter & emitter,
                         openvrml::event_listener & listener)
        throw (std::bad_cast)
    {
        using openvrml::field_value_emitter;
        using openvrml::field_value_listener;
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
bool openvrml::delete_route(node & from,
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
        case field_value::invalid_type_id:
            assert(false);
        }
    } catch (const bad_cast &) {
        //
        // Do nothing.  If route removal fails, we simply return false.
        //
    }
    return deleted_route;
}


/**
 * @fn template <> openvrml::script_node * openvrml::node_cast<openvrml::script_node *>(node * n) throw ()
 *
 * @brief Cast to a script_node.
 *
 * @param n node.
 *
 * @return a script_node pointer to the node pointed to by @p n, or 0 if the
 *         node is not a script_node.
 */

/**
 * @fn template <> openvrml::appearance_node * openvrml::node_cast<openvrml::appearance_node *>(node * n) throw ()
 *
 * @brief Cast to an appearance_node.
 *
 * @param n node.
 *
 * @return an appearance_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not an appearance_node.
 */

/**
 * @fn template <> openvrml::child_node * openvrml::node_cast<openvrml::child_node *>(node * n) throw ()
 *
 * @brief Cast to a child_node.
 *
 * @param n node.
 *
 * @return a child_node pointer to the node pointed to by @p n, or 0 if the
 *         node is not a child_node.
 */

/**
 * @fn template <> openvrml::color_node * openvrml::node_cast<openvrml::color_node *>(node * n) throw ()
 *
 * @brief Cast to a color_node.
 *
 * @param n node.
 *
 * @return a color_node pointer to the node pointed to by @p n, or 0 if the
 *         node is not a color_node.
 */

/**
 * @fn template <> openvrml::coordinate_node * openvrml::node_cast<openvrml::coordinate_node *>(node * n) throw ()
 *
 * @brief Cast to a coordinate_node.
 *
 * @param n node.
 *
 * @return a coordinate_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a coordinate_node.
 */

/**
 * @fn template <> openvrml::font_style_node * openvrml::node_cast<openvrml::font_style_node *>(node * n) throw ()
 *
 * @brief Cast to a font_style_node.
 *
 * @param n node.
 *
 * @return a font_style_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a font_style_node.
 */

/**
 * @fn template <> openvrml::geometry_node * openvrml::node_cast<openvrml::geometry_node *>(node * n) throw ()
 *
 * @brief Cast to a geometry_node.
 *
 * @param n node.
 *
 * @return a geometry_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a geometry_node.
 */

/**
 * @fn template <> openvrml::grouping_node * openvrml::node_cast<openvrml::grouping_node *>(node * n) throw ()
 *
 * @brief Cast to a grouping_node.
 *
 * @param n node.
 *
 * @return a grouping_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a grouping_node.
 */

/**
 * @fn template <> openvrml::material_node * openvrml::node_cast<openvrml::material_node *>(node * n) throw ()
 *
 * @brief Cast to a material_node.
 *
 * @param n node.
 *
 * @return a material_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a material_node.
 */

/**
 * @fn template <> openvrml::normal_node * openvrml::node_cast<openvrml::normal_node *>(node * n) throw ()
 *
 * @brief Cast to a normal_node.
 *
 * @param n node.
 *
 * @return a normal_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a normal_node.
 */

/**
 * @fn template <> openvrml::sound_source_node * openvrml::node_cast<openvrml::sound_source_node *>(node * n) throw ()
 *
 * @brief Cast to a sound_source_node.
 *
 * @param n node.
 *
 * @return a sound_source_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a sound_source_node.
 */

/**
 * @fn template <> openvrml::texture_node * openvrml::node_cast<openvrml::texture_node *>(node * n) throw ()
 *
 * @brief Cast to a texture_node.
 *
 * @param n node.
 *
 * @return a texture_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a texture_node.
 */

/**
 * @fn template <> openvrml::texture_coordinate_node * openvrml::node_cast<openvrml::texture_coordinate_node *>(node * n) throw ()
 *
 * @brief Cast to a texture_coordinate_node.
 *
 * @param n node.
 *
 * @return a texture_coordinate_node pointer to the node pointed to by @p n, or
 *         0 if the node is not a texture_coordinate_node.
 */

/**
 * @fn template <> openvrml::texture_transform_node * openvrml::node_cast<openvrml::texture_transform_node *>(node * n) throw ()
 *
 * @brief Cast to a texture_transform_node.
 *
 * @param n node.
 *
 * @return a texture_transform_node pointer to the node pointed to by @p n, or
 *         0 if the node is not a texture_transform_node.
 */

/**
 * @fn template <> openvrml::transform_node * openvrml::node_cast<openvrml::transform_node *>(node * n) throw ()
 *
 * @brief Cast to a transform_node.
 *
 * @param n node.
 *
 * @return a transform_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a transform_node.
 */

/**
 * @fn template <> openvrml::viewpoint_node * openvrml::node_cast<openvrml::viewpoint_node *>(node * n) throw ()
 *
 * @brief Cast to a viewpoint_node.
 *
 * @param n node.
 *
 * @return a viewpoint_node pointer to the node pointed to by @p n, or 0 if
 *         the node is not a viewpoint_node.
 */


/**
 * @class openvrml::appearance_node
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
openvrml::appearance_node::
appearance_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::appearance_node::~appearance_node() throw ()
{}

/**
 * @brief Insert appearance when rendering.
 *
 * @param v         viewer.
 * @param context   rendering_context.
 */
void openvrml::appearance_node::render_appearance(viewer & v,
                                                  rendering_context context)
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
void openvrml::appearance_node::do_render_appearance(viewer & v,
                                                     rendering_context context)
{}

/**
 * @brief Cast to an appearance_node.
 *
 * @return a pointer to this appearance_node.
 */
openvrml::appearance_node * openvrml::appearance_node::to_appearance() throw ()
{
    return this;
}

/**
 * @fn const openvrml::node_ptr & openvrml::appearance_node::material() const throw ()
 *
 * @brief Get the material node associated with this appearance node.
 *
 * @return the material_node associated with this appearance_node.
 */

/**
 * @fn const openvrml::node_ptr & openvrml::appearance_node::texture() const throw ()
 *
 * @brief Get the texture node associated with this appearance node.
 *
 * @return the texture_node associated with this appearance_node.
 */

/**
 * @fn const openvrml::node_ptr & openvrml::appearance_node::texture_transform() const throw ()
 *
 * @brief Get the texture transform node associated with this appearance node.
 *
 * @return the texture_transform_node associated with this appearance_node.
 */


/**
 * @class openvrml::bounded_volume_node
 *
 * @ingroup nodes
 *
 * @brief Abstract base class for nodes that represent a bounded volume in the
 *        scene graph.
 */

/**
 * @internal
 *
 * @var bool openvrml::bounded_volume_node::bounding_volume_dirty_
 *
 * @brief Indicate whether the node's cached bounding volume needs updating.
 */

/**
 * @brief Construct.
 */
openvrml::bounded_volume_node::
bounded_volume_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope) throw ():
    node(type, scope),
    bounding_volume_dirty_(false)
{}

/**
 * @brief Destroy.
 */
openvrml::bounded_volume_node::~bounded_volume_node() throw ()
{}

/**
 * @brief Get this node's bounding volume.
 *
 * Nodes that have no bounding volume, or have a difficult to calculate
 * bvolume (like, say, Extrusion or Billboard) can just return an infinite
 * bsphere. Note that returning an infinite bvolume means that all the node's
 * ancestors will also end up with an infinite bvolume, and will never be
 * culled.
 *
 * Delegates to <code>bounded_volume_node::do_bounding_volume</code>.
 *
 * @return a maximized bounding volume.
 */
const openvrml::bounding_volume &
openvrml::bounded_volume_node::bounding_volume() const
{
    const openvrml::bounding_volume & bv = this->do_bounding_volume();
    this->bounding_volume_dirty_ = false;
    return bv;
}

/**
 * @brief Called by <code>bounded_volume_node::bounding_volume</code>.
 *
 * @return a maximized bounding volume.
 */
const openvrml::bounding_volume &
openvrml::bounded_volume_node::do_bounding_volume() const
{
    class default_bounding_volume : public bounding_sphere {
    public:
        default_bounding_volume()
        {
            this->maximize();
        }
    };

    static const default_bounding_volume default_bvolume;
    return default_bvolume;
}

/**
 * @brief Set whether the node's bounding volume needs to be recalculated.
 *
 * Indicate that a node's bounding volume needs to be recalculated (or not).
 * If a node's bounding volume is invalid, then the bounding volumes of all
 * that node's ancestors are also invalid. Normally, the node itself will
 * determine when its bounding volume needs updating.
 *
 * @param value @c true if the node's bounding volume should be recalculated;
 *              @c false otherwise.
 */
void openvrml::bounded_volume_node::bounding_volume_dirty(const bool value)
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());
    this->bounding_volume_dirty_ = value;
    if (value) { // only if dirtying, not clearing
        this->type().node_class().browser().flags_need_updating = true;
    }
}

/**
 * @brief Whether the node's bounding volume needs to be recalculated.
 *
 * @return @c true if the node's bounding volume needs to be recalculated;
 *         @c false otherwise.
 */
bool openvrml::bounded_volume_node::bounding_volume_dirty() const
{
    boost::recursive_mutex::scoped_lock lock(this->mutex());
    if (this->type().node_class().browser().flags_need_updating) {
        this->type().node_class().browser().update_flags();
        this->type().node_class().browser().flags_need_updating = false;
    }
    return this->bounding_volume_dirty_;
}

/**
 * @brief Cast to an <code>bounded_volume_node</code>.
 *
 * @return a pointer to this <code>bounded_volume_node</code>.
 */
openvrml::bounded_volume_node *
openvrml::bounded_volume_node::to_bounded_volume() throw ()
{
    return this;
}


/**
 * @class openvrml::child_node
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
openvrml::child_node::
child_node(const node_type & type,
           const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope),
    bounded_volume_node(type, scope)
{}

/**
 * @brief Destructor.
 */
openvrml::child_node::~child_node() throw ()
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
void openvrml::child_node::relocate() throw (std::bad_alloc)
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
void openvrml::child_node::render_child(viewer & v,
                                        const rendering_context context)
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
void openvrml::child_node::do_render_child(viewer & v,
                                           rendering_context context)
{}

/**
 * @brief Cast to a child_node.
 *
 * @return a pointer to this child_node.
 */
openvrml::child_node * openvrml::child_node::to_child() throw ()
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
void openvrml::child_node::do_relocate() throw (std::bad_alloc)
{}


/**
 * @class openvrml::color_node
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
openvrml::color_node::
color_node(const node_type & type,
           const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::color_node::~color_node() throw ()
{}

/**
 * @brief Cast to a color_node.
 *
 * @return a pointer to this color_node.
 */
openvrml::color_node * openvrml::color_node::to_color() throw ()
{
    return this;
}

/**
 * @fn const std::vector<openvrml::color> & openvrml::color_node::color() const
 *
 * @brief Get the color array encapsulated by this node.
 *
 * @return the color array for this node.
 */


/**
 * @class openvrml::coordinate_node
 *
 * @brief Abstract base class for coordinate nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
openvrml::coordinate_node::
coordinate_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::coordinate_node::~coordinate_node() throw ()
{}

/**
 * @brief Cast to a coordinate_node.
 *
 * @return a pointer to this coordinate_node.
 */
openvrml::coordinate_node * openvrml::coordinate_node::to_coordinate() throw ()
{
    return this;
}

/**
 * @fn const std::vector<openvrml::vec3f> & openvrml::coordinate_node::point() const
 *
 * @brief Get the points encapsulated by this node.
 *
 * @return the array of points for this node.
 */


/**
 * @class openvrml::font_style_node
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
openvrml::font_style_node::
font_style_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope) throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::font_style_node::~font_style_node() throw ()
{}

/**
 * @brief Cast to a font_style_node.
 *
 * @return a pointer to this font_style_node.
 */
openvrml::font_style_node * openvrml::font_style_node::to_font_style() throw ()
{
    return this;
}

/**
 * @fn const std::vector<std::string> & openvrml::font_style_node::family() const throw ()
 *
 * @brief Get the list of font families.
 *
 * @return the font families that may be used for this FontStyle.
 */

/**
 * @fn bool openvrml::font_style_node::horizontal() const throw ()
 *
 * @brief Get the flag indicating whether the text should be horizontal or
 *      vertical.
 *
 * @return @c true if the text should be horizontal, or @c false if the text
 *      should be vertical.
 */

/**
 * @fn const std::vector<std::string> & openvrml::font_style_node::justify() const throw ()
 *
 * @brief Get the descriptor for the text justification.
 *
 * @return the characteristics of the text justification.
 */

/**
 * @fn const std::string & openvrml::font_style_node::language() const throw ()
 *
 * @brief Get the language code.
 *
 * @return the language code.
 */

/**
 * @fn bool openvrml::font_style_node::left_to_right() const throw ()
 *
 * @brief Get the flag indicating whether the text should be rendered
 *      left-to-right.
 *
 * @return @c true if the text should be rendered left-to-right, or @c false if
 *      the text should be rendered right-to-left.
 */

/**
 * @fn float openvrml::font_style_node::size() const throw ()
 *
 * @brief Get the size of the text.
 *
 * @return the size of the text.
 */

/**
 * @fn float openvrml::font_style_node::spacing() const throw ()
 *
 * @brief Get the spacing for the text.
 *
 * @return the spacing for the text.
 */

/**
 * @fn const std::string & openvrml::font_style_node::style() const throw ()
 *
 * @brief Get the style for the text.
 *
 * @return the text style.
 */

/**
 * @fn bool openvrml::font_style_node::top_to_bottom() const throw ()
 *
 * @brief Get the flag indicating whether the text should be rendered
 *      top-to-bottom.
 *
 * @return @c true if the text should be rendered top-to-bottom, or @c false if
 *      the text should be rendered bottom-to-top.
 */


/**
 * @class openvrml::geometry_node
 *
 * @ingroup nodes
 *
 * @brief Abstract base class for geometry nodes.
 */

/**
 * @var openvrml::viewer::object_t openvrml::geometry_node::geometry_reference
 *
 * @brief Identifier for a geometry object in the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
openvrml::geometry_node::
geometry_node(const node_type & type,
              const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope),
    bounded_volume_node(type, scope),
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
openvrml::geometry_node::~geometry_node() throw ()
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
openvrml::geometry_node * openvrml::geometry_node::to_geometry() throw ()
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
openvrml::viewer::object_t
openvrml::geometry_node::render_geometry(viewer & v,
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
bool openvrml::geometry_node::emissive() const throw ()
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
openvrml::viewer::object_t
openvrml::geometry_node::do_render_geometry(viewer & v,
                                            rendering_context context)
{
    return 0;
}

/**
 * @brief emissive implementation.
 *
 * @return @c false.
 */
bool openvrml::geometry_node::do_emissive() const throw ()
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
const openvrml::color_node * openvrml::geometry_node::color() const throw ()
{
    return 0;
}


/**
 * @class openvrml::grouping_node
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
openvrml::grouping_node::
grouping_node(const node_type & type,
              const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::grouping_node::~grouping_node() throw ()
{}

/**
 * @brief Cast to a grouping_node.
 *
 * @return a pointer to this grouping_node.
 */
openvrml::grouping_node * openvrml::grouping_node::to_grouping() throw ()
{
    return this;
}

/**
 * @fn const std::vector<openvrml::node_ptr> & openvrml::grouping_node::children() const throw ()
 *
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */

/**
 * @fn void openvrml::grouping_node::activate(double timestamp, bool over, bool active, double * p)
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
 * @class openvrml::material_node
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
openvrml::material_node::
material_node(const node_type & type,
              const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::material_node::~material_node() throw ()
{}

/**
 * @brief Cast to a material_node.
 *
 * @return a pointer to this material_node.
 */
openvrml::material_node * openvrml::material_node::to_material() throw ()
{
    return this;
}

/**
 * @fn float openvrml::material_node::ambient_intensity() const throw ()
 *
 * @brief Get the ambient intensity.
 *
 * @return the ambient intensity.
 */

/**
 * @fn const openvrml::color & openvrml::material_node::diffuse_color() const throw ()
 *
 * @brief Get the diffuse color.
 *
 * @return the diffuse color.
 */

/**
 * @fn const openvrml::color & openvrml::material_node::emissive_color() const throw ()
 *
 * @brief Get the emissive color.
 *
 * @return the emissive color.
 */

/**
 * @fn float openvrml::material_node::shininess() const throw ()
 *
 * @brief Get the shininess.
 *
 * @return the shininess.
 */

/**
 * @fn const openvrml::color & openvrml::material_node::specular_color() const throw ()
 *
 * @brief Get the specular color.
 *
 * @return the specular color.
 */

/**
 * @fn float openvrml::material_node::transparency() const throw ()
 *
 * @brief Get the transparency.
 *
 * @return the transparency.
 */


/**
 * @class openvrml::navigation_info_node
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
openvrml::navigation_info_node::
navigation_info_node(const node_type & t,
                     const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(t, scope),
    bounded_volume_node(t, scope),
    child_node(t, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::navigation_info_node::~navigation_info_node() throw ()
{}

/**
 * @brief Cast to a navigation_info_node.
 *
 * @return a pointer to this navigation_info_node.
 */
openvrml::navigation_info_node *
openvrml::navigation_info_node::to_navigation_info() throw ()
{
    return this;
}

/**
 * @fn const std::vector<float> & openvrml::navigation_info_node::avatar_size() const throw ()
 *
 * @brief Get the avatar dimensions.
 *
 * @return the avatar dimensions.
 */

/**
 * @fn bool openvrml::navigation_info_node::headlight() const throw ()
 *
 * @brief Get the state of the headlight.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */

/**
 * @fn float openvrml::navigation_info_node::speed() const throw ()
 *
 * @brief Get the current speed of the user view.
 *
 * @return the current speed of the user view.
 */

/**
 * @fn const std::vector<std::string> & openvrml::navigation_info_node::type() const throw ()
 *
 * @brief Get the navigation type.
 *
 * @return the navigation type.
 */

/**
 * @fn float openvrml::navigation_info_node::visibility_limit() const throw ()
 *
 * @brief Get the visibility limit.
 *
 * @return the visibility limit.
 */


/**
 * @class openvrml::normal_node
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
openvrml::normal_node::
normal_node(const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::normal_node::~normal_node() throw ()
{}

/**
 * @brief Cast to a normal_node.
 *
 * @return a pointer to this normal_node.
 */
openvrml::normal_node * openvrml::normal_node::to_normal() throw ()
{
    return this;
}

/**
 * @fn const std::vector<openvrml::vec3f> & openvrml::normal_node::vector() const throw ()
 *
 * @brief Get the array of normal vectors.
 *
 * @return the array of normal vectors.
 */


/**
 * @class openvrml::sound_source_node
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
openvrml::sound_source_node::
sound_source_node(const node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destructor.
 */
openvrml::sound_source_node::~sound_source_node() throw ()
{}

/**
 * @brief Cast to a sound_source_node.
 *
 * @return a pointer to this sound_source_node.
 */
openvrml::sound_source_node * openvrml::sound_source_node::to_sound_source()
    throw ()
{
    return this;
}


/**
 * @class openvrml::texture_node
 *
 * @ingroup nodes
 *
 * @brief Abstract base class for texture nodes.
 */

/**
 * @var openvrml::viewer::texture_object_t openvrml::texture_node::texture_reference
 *
 * @brief Identifier for a texture object in the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the Scope the node belongs to.
 */
openvrml::texture_node::
texture_node(const node_type & type,
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
openvrml::texture_node::~texture_node() throw ()
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
openvrml::viewer::texture_object_t
openvrml::texture_node::render_texture(viewer & v)
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
openvrml::viewer::texture_object_t
openvrml::texture_node::do_render_texture(viewer & v)
{
    return 0;
}

/**
 * @brief Cast to a texture_node.
 *
 * @return a pointer to this texture_node.
 */
openvrml::texture_node * openvrml::texture_node::to_texture() throw ()
{
    return this;
}

/**
 * @fn const openvrml::image & openvrml::texture_node::image() const throw ()
 *
 * @brief The image.
 *
 * @return the image.
 */

/**
 * @fn bool openvrml::texture_node::repeat_s() const throw ()
 *
 * @brief Get the flag indicating whether the texture should repeat in the
 *      <var>S</var> direction.
 *
 * @return @c true if the image should repeat in the <var>S</var> direction,
 *      @c false otherwise.
 */

/**
 * @fn bool openvrml::texture_node::repeat_t() const throw ()
 *
 * @brief Get the flag indicating whether the texture should repeat in the
 *      <var>T</var> direction.
 *
 * @return @c true if the image should repeat in the <var>T</var> direction,
 *      @c false otherwise.
 */


/**
 * @class openvrml::texture_coordinate_node
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
openvrml::texture_coordinate_node::
texture_coordinate_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::texture_coordinate_node::~texture_coordinate_node() throw ()
{}

/**
 * @brief Cast to a texture_coordinate_node.
 *
 * @return a pointer to this texture_coordinate_node.
 */
openvrml::texture_coordinate_node *
openvrml::texture_coordinate_node::to_texture_coordinate()
    throw ()
{
    return this;
}

/**
 * @fn const std::vector<openvrml::vec2f> & openvrml::texture_coordinate_node::point() const throw ()
 *
 * @brief The texture coordinates.
 *
 * @return the texture coordinates.
 */


/**
 * @class openvrml::texture_transform_node
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
openvrml::texture_transform_node::
texture_transform_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope)
{}

/**
 * @brief Destructor.
 */
openvrml::texture_transform_node::~texture_transform_node() throw ()
{}

/**
 * @brief Render the texture transform.
 *
 * @param v viewer.
 */
void openvrml::texture_transform_node::render_texture_transform(viewer & v)
{
    this->do_render_texture_transform(v);
    this->modified(false);
}

/**
 * @brief Render the texture transform.
 *
 * @param v viewer.
 */
void openvrml::texture_transform_node::do_render_texture_transform(viewer & v)
{}

/**
 * @brief Cast to a texture_transform_node.
 *
 * @return a pointer to this texture_transform_node.
 */
openvrml::texture_transform_node *
openvrml::texture_transform_node::to_texture_transform()
    throw ()
{
    return this;
}


/**
 * @class openvrml::transform_node
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
openvrml::transform_node::
transform_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::transform_node::~transform_node() throw ()
{}

/**
 * @brief Cast to a transform_node.
 *
 * @return a pointer to this transform_node.
 */
openvrml::transform_node * openvrml::transform_node::to_transform() throw ()
{
    return this;
}

/**
 * @fn const openvrml::mat4f & openvrml::transform_node::transform() const throw ()
 *
 * @brief Get the transformation associated with the node as a matrix.
 *
 * @return the transformation associated with the node.
 */


/**
 * @class openvrml::viewpoint_node
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
openvrml::viewpoint_node::
viewpoint_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope)
    throw ():
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope)
{}

/**
 * @brief Destructor.
 */
openvrml::viewpoint_node::~viewpoint_node() throw ()
{}

/**
 * @brief Cast to a viewpoint_node.
 *
 * @return a pointer to this viewpoint_node.
 */
openvrml::viewpoint_node * openvrml::viewpoint_node::to_viewpoint() throw ()
{
    return this;
}

/**
 * @fn const openvrml::mat4f & openvrml::viewpoint_node::transformation() const throw ()
 *
 * @brief Get the transformation of the viewpoint_node in the global coordinate
 *      system.
 *
 * @return the transformation of the viewpoint_node in the global coordinate
 *      system.
 */

/**
 * @fn const openvrml::mat4f & openvrml::viewpoint_node::user_view_transform() const throw ()
 *
 * @brief Get the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @return the transformation of the user view relative to the viewpoint_node.
 */

/**
 * @fn void openvrml::viewpoint_node::user_view_transform(const mat4f & transform) throw ()
 *
 * @brief Set the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @param transform the new transformation.
 */

/**
 * @fn const std::string & openvrml::viewpoint_node::description() const throw ()
 *
 * @brief Get the description.
 *
 * @return the description.
 */

/**
 * @fn float openvrml::viewpoint_node::field_of_view() const throw ()
 *
 * @brief Get the field of view.
 *
 * @return the field of view in radians.
 */


/**
 * @class openvrml::node_traverser
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
 * @var std::set<openvrml::node *> openvrml::node_traverser::traversed_nodes
 *
 * @brief The set of nodes that have already been traversed.
 */

/**
 * @internal
 *
 * @var bool openvrml::node_traverser::halt
 *
 * @brief Flag to indicate if the traversal should be halted.
 */

/**
 * @brief Construct.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::node_traverser::node_traverser() throw (std::bad_alloc):
    halt(false)
{}

/**
 * @brief Destroy.
 */
openvrml::node_traverser::~node_traverser() throw ()
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
void openvrml::node_traverser::traverse(node & n)
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
void openvrml::node_traverser::traverse(const node_ptr & node)
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
void openvrml::node_traverser::traverse(const std::vector<node_ptr> & nodes)
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
void openvrml::node_traverser::halt_traversal() throw ()
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
bool openvrml::node_traverser::halted() throw ()
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
void openvrml::node_traverser::do_traversal(node & n)
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
void openvrml::node_traverser::on_entering(node & n)
{}

/**
 * @brief Called for each <code>node</code> in the traversal <em>after</em>
 *      traversing the its descendants.
 *
 * @param n  the <code>node</code> currently being traversed.
 */
void openvrml::node_traverser::on_leaving(node & n)
{}
