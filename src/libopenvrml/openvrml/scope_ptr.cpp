// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
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

# include "scope_ptr.h"
# include "scope.h"

namespace openvrml {

/**
 * @class scope_ptr
 *
 * @brief scope_ptr is a reference-counting smart pointer for scopes.
 *
 * scope_ptr is patterned after the <a href="http://boost.org/libs/smart_ptr/shared_ptr.htm">Boost shared_ptr</a>,
 * and it works basically the same way. A scope_ptr should be constructed
 * or reset with a scope created with @c new (or 0).
 *
 * @see http://boost.org/libs/smart_ptr/shared_ptr.htm
 */

/**
 * @fn bool operator==(const scope_ptr & lhs, const scope_ptr & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs point to the same scope; @c false
 *         otherwise.
 */

/**
 * @var scope * scope_ptr::scope_
 *
 * @brief Pointer to a scope.
 */

/**
 * @var size_t * scope_ptr::count_
 *
 * @brief Pointer to the reference count.
 */

/**
 * @brief Construct.
 *
 * @param scope a pointer to a scope constructed with @c new.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
scope_ptr::scope_ptr(openvrml::scope * const scope) throw (std::bad_alloc):
    scope_(scope)
{
    try {
        this->count_ = new size_t(1); // prevent leak if new throws
    } catch (std::bad_alloc &) {
        delete scope;
        throw;
    }
}

/**
 * @brief Construct a copy.
 *
 * @param ptr the scope_ptr to copy.
 */
scope_ptr::scope_ptr(const scope_ptr & ptr) throw ():
    scope_(ptr.scope_)
{
    ++*(this->count_ = ptr.count_); // never throws
}

/**
 * @fn scope_ptr::operator bool() const throw ()
 *
 * @brief Automatic conversion to @c bool.
 *
 * @return @c true if the scope_ptr is non-null; @c false otherwise.
 */

/**
 * @brief Assignment operator.
 *
 * @param ptr
 */
scope_ptr & scope_ptr::operator=(const scope_ptr & ptr) throw ()
{
    if (this->count_ != ptr.count_) {
        ++*ptr.count_;
        this->dispose();
        this->scope_ = ptr.scope_;
        this->count_ = ptr.count_;
    }
    return *this;
}

/**
 * @fn scope & scope_ptr::operator*() const throw ()
 *
 * @brief Dereference.
 *
 * @return a reference to the scope.
 */

/**
 * @fn scope * scope_ptr::operator->() const throw ()
 *
 * @brief Delegate to the scope.
 *
 * @return a pointer to the scope.
 */

/**
 * @fn scope * scope_ptr::get() const throw ()
 *
 * @brief Get the scope pointer.
 *
 * @return a pointer to the scope.
 */

/**
 * @brief Reset the scope_ptr to a new scope.
 *
 * @param scope a pointer to a scope constructed with @c new.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void scope_ptr::reset(openvrml::scope * const scope) throw (std::bad_alloc)
{
    if (this->scope_ == scope) { return; }
    if (--*this->count_ == 0) {
        delete this->scope_;
    } else {
        try {
            this->count_ = new size_t;
        } catch (...) {
            ++*this->count_;
            delete scope;
            throw;
        }
    }
    *this->count_ = 1;
    this->scope_ = scope;
}

/**
 * @brief Decrement the reference count; destroy the scope if the count
 *        drops to zero.
 */
void scope_ptr::dispose() throw ()
{
    if (--*this->count_ == 0) {
        delete this->scope_;
        delete this->count_;
    }
}

/**
 * @fn bool operator!=(const scope_ptr & lhs, const scope_ptr & rhs) throw ()
 *
 * @relates openvrml::scope_ptr
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs point to different
 *         @link openvrml::scope scopes@endlink; @c false otherwise.
 */

} // namespace openvrml
