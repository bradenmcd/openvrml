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

# include "node_type_ptr.h"
# include "node.h"

namespace OpenVRML {

/**
 * @class node_type_ptr
 *
 * @brief node_type_ptr is a reference-counting smart pointer for
 *        @link node_type node_types@endlink.
 *
 * node_type_ptr is patterned after the <a
 * href="http://boost.org/libs/smart_ptr/shared_ptr.htm">Boost shared_ptr</a>,
 * and it works basically the same way. A node_type_ptr should be constructed
 * or reset with a node_type created with @c new (or 0).
 *
 * @see http://boost.org/libs/smart_ptr/shared_ptr.htm
 */

/**
 * @brief Constructor.
 *
 * @param type a pointer to a node_type constructed with @c new.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
node_type_ptr::node_type_ptr(node_type * const type) throw (std::bad_alloc):
    type(type)
{
    try {
        this->count = new size_t(1); // prevent leak if new throws
    } catch (std::bad_alloc & ex) {
        delete type;
        throw;
    }
}

/**
 * @brief Copy constructor.
 *
 * @param ptr the node_type_ptr to copy.
 */
node_type_ptr::node_type_ptr(const node_type_ptr & ptr) throw ():
    type(ptr.type)
{
    ++*(this->count = ptr.count); // never throws
}

/**
 * @brief Assignment operator.
 *
 * @param ptr
 */
node_type_ptr & node_type_ptr::operator=(const node_type_ptr & ptr) throw ()
{
    if (this->count != ptr.count) {
        ++*ptr.count;
        this->dispose();
        this->type = ptr.type;
        this->count = ptr.count;
    }
    return *this;
}

/**
 * @fn node_type & node_type_ptr::operator*() const throw ()
 *
 * @brief Dereference.
 *
 * @return a reference to the node_type.
 */

/**
 * @fn node_type * node_type_ptr::operator->() const throw ()
 *
 * @brief Delegate to the node_type.
 *
 * @return a pointer to the node_type.
 */

/**
 * @fn node_type * node_type_ptr::get() const throw ()
 *
 * @brief Get the node_type pointer.
 *
 * @return a pointer to the node_type.
 */

/**
 * @brief Reset the node_type_ptr to a new node_type.
 *
 * @param type a pointer to a node_type constructed with @c new.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node_type_ptr::reset(node_type * const type) throw (std::bad_alloc)
{
    if (this->type == type) { return; }
    if (--*this->count == 0) {
        delete this->type;
    } else {
        try {
            this->count = new size_t;
        } catch (std::bad_alloc & ex) {
            ++*this->count;
            delete type;
            throw;
        }
    }
    *this->count = 1;
    this->type = type;
}

/**
 * @brief Decrement the reference count; destroy the node_type if the count
 *        drops to zero.
 */
void node_type_ptr::dispose() throw ()
{
    if (--*this->count == 0) {
        delete this->type;
        delete this->count;
    }
}

/**
 * @fn bool operator==(const node_type_ptr & lhs, const node_type_ptr & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @return @c true if @p lhs and @p rhs point to the same node_type; @c false
 *         otherwise.
 */

/**
 * @fn bool operator!=(const node_type_ptr & lhs, const node_type_ptr & rhs) throw ()
 *
 * @relates node_type_ptr
 *
 * @brief Compare for inequality.
 *
 * @return @c true if @p lhs and @p rhs point to different
 *         @link node_type node_types@endlink; @c false otherwise.
 */

} // namespace OpenVRML
