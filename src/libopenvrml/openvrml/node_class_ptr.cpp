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

# include "node_class_ptr.h"
# include "node.h"

namespace openvrml {

/**
 * @class node_class_ptr
 *
 * @brief node_class_ptr is a reference-counting smart pointer for
 *        @link openvrml::node_class node_classes@endlink.
 *
 * node_class_ptr is patterned after the <a
 * href="http://boost.org/libs/smart_ptr/shared_ptr.htm">Boost shared_ptr</a>,
 * and it works basically the same way. A node_class_ptr should be constructed
 * or reset with a node_class created with @c new (or 0).
 *
 * @see http://boost.org/libs/smart_ptr/shared_ptr.htm
 */

/**
 * @fn bool node_class_ptr::operator==(const node_class_ptr & lhs, const node_class_ptr & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs point to the same node_class; @c false
 *         otherwise.
 */

/**
 * @var node_class * node_class_ptr::class_
 *
 * @brief The node_class.
 */

/**
 * @var size_t * node_class_ptr::count
 *
 * @brief Reference count.
 */

/**
 * @brief Constructor.
 *
 * @param class_ a pointer to a NodeClass constructed with @c new.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
node_class_ptr::node_class_ptr(node_class * const class_)
    throw (std::bad_alloc):
    class_(class_)
{
    try {
        this->count = new size_t(1); // prevent leak if new throws
    } catch (std::bad_alloc &) {
        delete class_;
        throw;
    }
}

/**
 * @fn node_class_ptr::~node_class_ptr() throw ()
 *
 * @brief Destroy.
 */

/**
 * @brief Copy constructor.
 *
 * @param ptr the node_class_ptr to copy.
 */
node_class_ptr::node_class_ptr(const node_class_ptr & ptr) throw ():
    class_(ptr.class_)
{
    ++*(this->count = ptr.count); // never throws
}

/**
 * @brief Assignment operator.
 *
 * @param ptr
 *
 * @return a reference to the node_class_ptr.
 */
node_class_ptr & node_class_ptr::operator=(const node_class_ptr & ptr) throw ()
{
    if (this->count != ptr.count) {
        ++*ptr.count;
        this->dispose();
        this->class_ = ptr.class_;
        this->count = ptr.count;
    }
    return *this;
}

/**
 * @fn node_class_ptr::operator bool() const throw ()
 *
 * @brief Automatic conversion to @c bool.
 *
 * @return @c true if the node_class_ptr is non-null; @c false otherwise.
 */

/**
 * @fn node_class & node_class_ptr::operator*() const throw ()
 *
 * @brief Dereference.
 *
 * @return a reference to the node_class.
 */

/**
 * @fn node_class * node_class_ptr::operator->() const throw ()
 *
 * @brief Delegate to the node_class.
 *
 * @return a pointer to the node_class.
 */

/**
 * @fn node_class * node_class_ptr::get() const throw ()
 *
 * @brief Get the node_class pointer.
 *
 * @return a pointer to the node_class.
 */

/**
 * @brief Reset the node_class_ptr to a new node_class.
 *
 * @param class_ a pointer to a node_class constructed with @c new.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void node_class_ptr::reset(node_class * const class_) throw (std::bad_alloc)
{
    if (this->class_ == class_) { return; }
    if (--*this->count == 0) {
        delete this->class_;
    } else {
        try {
            this->count = new size_t;
        } catch (std::bad_alloc &) {
            ++*this->count;
            delete class_;
            throw;
        }
    }
    *this->count = 1;
    this->class_ = class_;
}

/**
 * @brief Decrement the reference count; destroy the node_class if the count
 *        drops to zero.
 */
void node_class_ptr::dispose() throw ()
{
    if (--*this->count == 0) {
        delete this->class_;
        delete this->count;
    }
}

/**
 * @fn bool operator!=(const node_class_ptr & lhs, const node_class_ptr & rhs) throw ()
 *
 * @relates openvrml::node_class_ptr
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs point to different
 *         @link openvrml::node_class node_classes@endlink; @c false otherwise.
 */

} // namespace openvrml
