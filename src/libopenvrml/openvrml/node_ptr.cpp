// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 2000  Braden McDaniel
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

# include <cassert>
# include "node_ptr.h"
# include "browser.h"

namespace openvrml {

/**
 * @class node_ptr
 *
 * @brief A reference-counted smart pointer for
 *        @link openvrml::node nodes@endlink.
 */

/**
 * @var node_ptr::script_node
 *
 * @brief Script nodes can be self-referential, so node_ptr works some special
 *      magic.
 */

/**
 * @fn bool node_ptr::operator==(const node_ptr & lhs, const node_ptr & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs point to the same node; @c false
 *         otherwise.
 */

namespace {
    typedef std::map<openvrml::node *, size_t> count_map_t;
    count_map_t count_map;
}

/**
 * @internal
 *
 * @var std::map<node *, size_t>::value_type * node_ptr::count_ptr
 *
 * @brief Pointer to an entry in count_map.
 */

/**
 * @brief Construct.
 *
 * @param node a pointer to a node
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
node_ptr::node_ptr(node * const node) throw (std::bad_alloc):
    count_ptr(0)
{
    if (node) {
        count_map_t::iterator pos = count_map.find(node);
        if (pos == count_map.end()) {
            const std::pair<count_map_t::iterator, bool>
                    result(count_map.insert(count_map_t::value_type(node, 0)));
            assert(result.second);
            assert(result.first->first == node);
            pos = result.first;
        }
        ++pos->second;
        this->count_ptr = &*pos;
    }
}

/**
 * @brief Construct a copy.
 *
 * @param ptr
 */
node_ptr::node_ptr(const node_ptr & ptr) throw ():
    count_ptr(ptr.count_ptr)
{
    if (this->count_ptr) {
        ++this->count_ptr->second;
    }
}

/**
 * @fn node_ptr::~node_ptr()
 *
 * @brief Destructor.
 */

/**
 * @fn node_ptr::operator bool() const
 *
 * @brief Automatic conversion to bool.
 */

/**
 * @fn node_ptr & node_ptr::operator=(const node_ptr &)
 *
 * @brief Assignment operator.
 *
 * @param ptr
 */

/**
 * @fn node & node_ptr::operator*() const
 *
 * @brief Dereference operator.
 *
 * @return a reference to the underlying node.
 */

/**
 * @fn node * node_ptr::operator->() const
 *
 * @brief Access a method of the node.
 */

/**
 * @fn node * node_ptr::get() const
 *
 * @brief Get a raw pointer to the node.
 *
 * @return a raw pointer to the underlying node.
 */

/**
 * @fn void node_ptr::swap(node_ptr & ptr) throw ()
 *
 * @brief Swap the values of the node_ptr and @p ptr.
 */

/**
 * @brief Reset the node_ptr to point to a different node.
 *
 * @param node
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node_ptr::reset(node * const node) throw (std::bad_alloc)
{
    if (this->count_ptr && this->count_ptr->first == node) {
        return;
    }
    this->dispose();
    if (node) {
        std::pair<count_map_t::iterator, bool>
                result(count_map.insert(count_map_t::value_type(node, 1)));
        assert(result.first->first == node);
        this->count_ptr = &*result.first;
        if (!result.second) { // The node already exists in the table.
            ++this->count_ptr->second;
        }
    }
}

/**
 * @brief Relinquish ownership of the node.
 *
 * Decrement the reference count; if it drops to zero, call node::shutdown
 * on the node, delete the node, and remove its entry from the count map.
 */
void node_ptr::dispose() throw ()
{
    if (this->count_ptr) {
        --this->count_ptr->second;
        if (this->count_ptr->second == 0) {
            this->count_ptr->first->shutdown(browser::current_time());
            delete this->count_ptr->first;
            count_map.erase(this->count_ptr->first);
        }
        this->count_ptr = 0;
    }
}

/**
 * @brief Share ownership of a node.
 *
 * @param count_ptr a pointer to an entry in the count map to share.
 */
void node_ptr::share(std::map<node *, size_t>::value_type * count_ptr) throw ()
{
    if (this->count_ptr != count_ptr) {
        if (count_ptr) { ++count_ptr->second; }
        this->dispose();
        this->count_ptr = count_ptr;
    }
}

/**
 * @fn bool operator!=(const node_ptr & lhs, const node_ptr & rhs) throw ()
 *
 * @relates openvrml::node_ptr
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs point to different
 *         @link openvrml::node nodes@endlink; @c false otherwise.
 */

} // namespace openvrml
