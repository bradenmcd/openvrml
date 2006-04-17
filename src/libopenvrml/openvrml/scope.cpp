// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2002  Braden McDaniel
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
# include "scope.h"
# include "node.h"

/**
 * @class openvrml::scope
 *
 * @brief The scope class keeps track of defined nodes and
 *      prototypes.
 *
 * PROTO definitions add node types to the namespace. PROTO implementations are
 * a separate node type namespace, and require that any nested PROTOs @b not be
 * available outside the PROTO implementation. PROTOs defined outside the
 * current namespace are available.
 */

/**
 * @var openvrml::scope::node
 *
 * @brief node identifiers are stored in the scope, so node needs special
 *        privilege to access them.
 */

/**
 * @var std::list<boost::shared_ptr<openvrml::node_type> > openvrml::scope::node_type_list
 *
 * @brief List of <code>node_type</code>s in the scope.
 */

/**
 * @var std::map<std::string, openvrml::node *> openvrml::scope::named_node_map
 *
 * @brief Map of the named <code>node</code>s in the scope.
 */

/**
 * @internal
 *
 * @var const std::string openvrml::scope::id_
 *
 * @brief <code>scope</code> identifier.
 */

/**
 * @internal
 *
 * @var const boost::shared_ptr<const openvrml::scope> openvrml::scope::parent_
 *
 * @brief The parent scope; null if the scope is a root scope.
 */

/**
 * @brief Construct.
 *
 * @param[in] id        the identifier for the scope.
 * @param[in] parent    the parent scope.
 *
 * For the root scope, @p id should be the URI of the world. For child scopes,
 * @p id should be the name of the PROTO to which the scope corresponds.
 */
openvrml::scope::scope(const std::string & id,
                       const boost::shared_ptr<const scope> & parent):
    id_(id),
    parent_(parent)
{}

/**
 * @brief <code>scope</code> identifier.
 *
 * @return the <code>scope</code> identifier.
 */
const std::string & openvrml::scope::id() const OPENVRML_NOTHROW
{
    return this->id_;
}

/**
 * @brief The parent <code>scope</code>.
 *
 * @return the parent <code>scope</code>; or null if the <code>scope</code> is
 *         a root <code>scope</code>.
 */
const boost::shared_ptr<const openvrml::scope> &
openvrml::scope::parent() const OPENVRML_NOTHROW
{
    return this->parent_;
}

/**
 * @brief Add a node type.
 *
 * Print an error message if the argument type is already defined.
 *
 * @param[in] type  a node_type.
 *
 * @return a pair whose first element is a pointer to a @c node_type whose
 *         @c node_type::id is the same as that of @p type.  The second element
 *         is a boolean value.  If the second element is @c true, @p type was
 *         successfully added to the @c scope and the first element is the same
 *         as @p type.  If the second element is @c false, @p type was not
 *         added to the scope and the first element is a @c node_type that
 *         already exists in the @c scope.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p type is not null.
 */
const std::pair<boost::shared_ptr<openvrml::node_type>, bool>
openvrml::scope::add_type(const boost::shared_ptr<node_type> & type)
    OPENVRML_THROW1(std::bad_alloc)
{
    assert(type);

    using boost::shared_ptr;

    std::pair<shared_ptr<node_type>, bool> result;
    if ((result.first = this->find_type(type->id()))) {
        result.second = false;
    } else {
        //
        // Throws std::bad_alloc.
        //
        const std::list<shared_ptr<node_type> >::iterator pos =
            this->node_type_list.insert(this->node_type_list.begin(), type);
        result.first = type;
        result.second = pos != this->node_type_list.end();
    }
    return result;
}

namespace {
    struct OPENVRML_LOCAL has_id_ :
        std::unary_function<boost::shared_ptr<openvrml::node_type>, bool> {
        explicit has_id_(const std::string & id):
            id(&id)
        {}

        bool
        operator()(const boost::shared_ptr<openvrml::node_type> & type) const
        {
            assert(type);
            return type->id() == *this->id;
        }

    private:
        const std::string * id;
    };

    typedef std::list<boost::shared_ptr<openvrml::node_type> >
        node_type_list_t;
}

/**
 * @brief Find a node type, given a type name. Returns 0 if type is
 *      not defined.
 */
const boost::shared_ptr<openvrml::node_type> &
openvrml::scope::find_type(const std::string & id) const
{
    //
    // Look through the types unique to this scope.
    //
    const node_type_list_t::const_iterator end = this->node_type_list.end();
    const node_type_list_t::const_iterator pos =
        std::find_if(this->node_type_list.begin(), end, has_id_(id));
    if (pos != end) { return *pos; }

    //
    // Look in the parent scope for the type.
    //
    static const boost::shared_ptr<node_type> null;
    return this->parent_
            ? this->parent_->find_type(id)
            : null;
}

/**
 * @brief The first type in the scope.
 *
 * @return the first node_type in the scope, or a null shared_ptr if the scope
 *         has no node_types.
 */
const boost::shared_ptr<openvrml::node_type> &
openvrml::scope::first_type() const
{
    static const boost::shared_ptr<node_type> null;
    return !this->node_type_list.empty()
            ? this->node_type_list.front()
            : null;
}

/**
 * @brief Find the node in the scope with node::id @p id.
 *
 * @return a pointer to a node with node::id @p id, or 0 if no such node exists
 *         in the scope.
 */
openvrml::node * openvrml::scope::find_node(const std::string & id) const
{
    typedef std::map<std::string, node *> named_node_map_t;
    const named_node_map_t::const_iterator pos = this->named_node_map.find(id);
    return (pos != this->named_node_map.end())
            ? pos->second
            : 0;
}

/**
 * @relates openvrml::scope
 *
 * @param[in] s a @c scope.
 *
 * @return the full &ldquo;path&rdquo; to the @c scope.
 */
const std::string openvrml::path(const scope & s)
    OPENVRML_THROW1(std::bad_alloc)
{
    std::string path_str;
    const scope * s_ptr = &s;
    while (s_ptr->parent()) {
        path_str = '#' + s_ptr->id() + path_str;
        s_ptr = s_ptr->parent().get();
    }
    return s_ptr->id() + path_str;
}
