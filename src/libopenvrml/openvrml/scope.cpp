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

namespace openvrml {

/**
 * @class scope
 *
 * @brief The scope class keeps track of defined nodes and
 *      prototypes.
 *
 * PROTO definitions add node types to the namespace.
 * PROTO implementations are a separate node type namespace,
 * and require that any nested PROTOs NOT be available outside
 * the PROTO implementation. PROTOs defined outside the current
 * namespace are available.
 */

/**
 * @var scope::node
 *
 * @brief node identifiers are stored in the scope, so node needs special
 *        privilege to access them.
 */

/**
 * @var std::list<node_type_ptr> scope::node_type_list
 *
 * @brief List of @link openvrml::node_type node_types@endlink in the scope.
 */

/**
 * @var std::map<std::string, node *> scope::named_node_map
 *
 * @brief Map of the named @link openvrml::node nodes@endlink in the scope.
 */

/**
 * @var const std::string scope::id
 *
 * @brief scope identifier.
 */

/**
 * @var const scope_ptr scope::parent
 *
 * @brief The parent scope; null if the scope is a root scope.
 */

/**
 * @brief Construct.
 *
 * @param id        the identifier for the scope.
 * @param parent    the parent scope.
 *
 * For the root scope, @p id should be the URI of the world. For child scopes,
 * @p id should be the name of the PROTO to which the scope corresponds.
 */
scope::scope(const std::string & id, const scope_ptr & parent):
    id(id),
    parent(parent)
{}

/**
 * @fn scope::scope(const scope &)
 *
 * @brief Not implemented.
 *
 * scopes are not copyable.
 */

/**
 * @brief Destroy.
 */
scope::~scope()
{}

/**
 * @fn scope & scope::operator=(const scope &)
 *
 * @brief Not implemented.
 *
 * scopes are not copyable.
 */

/**
 * @brief Add a node type.
 *
 * Print an error message if the argument type is already defined.
 *
 * @param type  a node_type_ptr
 *
 * @return @c true if @p type is successfully added to the scope; @c false
 *         otherwise (if a node_type with the same id is already defined for
 *         the scope).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p type is not null.
 */
bool scope::add_type(const node_type_ptr & type) throw (std::bad_alloc)
{
    assert(type);
    if (this->find_type(type->id)) { return false; }
    this->node_type_list.push_front(type); // Throws std::bad_alloc.
    return true;
}

namespace {
    struct has_id_ : std::unary_function<node_type_ptr, bool> {
        explicit has_id_(const std::string & id):
            id(&id)
        {}

        bool operator()(const node_type_ptr & type) const
        {
            assert(type);
            return type->id == *this->id;
        }

    private:
        const std::string * id;
    };

    typedef std::list<node_type_ptr> node_type_list_t;
}

/**
 * @brief Find a node type, given a type name. Returns 0 if type is
 *      not defined.
 */
const node_type_ptr & scope::find_type(const std::string & id) const {
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
    static const node_type_ptr null;
    return this->parent
            ? this->parent->find_type(id)
            : null;
}

/**
 * @brief The first type in the scope.
 *
 * @return the first node_type in the scope, or a null node_type_ptr if the
 *         scope has no node_types.
 */
const node_type_ptr & scope::first_type() const
{
    static const node_type_ptr null;
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
node * scope::find_node(const std::string & id) const
{
    typedef std::map<std::string, node *> named_node_map_t;
    const named_node_map_t::const_iterator pos = this->named_node_map.find(id);
    return (pos != this->named_node_map.end())
            ? pos->second
            : 0;
}

} // namespace openvrml

