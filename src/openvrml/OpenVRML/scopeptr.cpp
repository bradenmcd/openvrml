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

# include "scopeptr.h"
# include "scope.h"

namespace OpenVRML {

/**
 * @class ScopePtr
 *
 * @brief ScopePtr is a reference-counting smart pointer for Scopes.
 *
 * ScopePtr is patterned after the <a href="http://boost.org/libs/smart_ptr/shared_ptr.htm">Boost shared_ptr</a>,
 * and it works basically the same way. A ScopePtr should be constructed
 * or reset with a Scope created with @c new (or 0).
 *
 * @see http://boost.org/libs/smart_ptr/shared_ptr.htm
 */

/**
 * @brief Constructor.
 *
 * @param scope a pointer to a Scope constructed with <code>new</code>.
 */
ScopePtr::ScopePtr(Scope * const scope): scope(scope) {
    try {
        this->count = new size_t(1); // prevent leak if new throws
    } catch (...) {
        delete scope;
        throw;
    }
}

/**
 * @brief Copy constructor.
 *
 * @param scopePtr the ScopePtr to copy.
 */
ScopePtr::ScopePtr(const ScopePtr & scopePtr):
        scope(scopePtr.scope) {
    ++*(this->count = scopePtr.count); // never throws
}

/**
 * @brief Assignment operator.
 *
 * @param scopePtr
 */
ScopePtr & ScopePtr::operator=(const ScopePtr & scopePtr) {
    if (this->count != scopePtr.count) {
        ++*scopePtr.count;
        this->dispose();
        this->scope = scopePtr.scope;
        this->count = scopePtr.count;
    }
    return *this;
}

/**
 * @brief Reset the ScopePtr to a new Scope.
 *
 * @param scope a pointer to a Scope constructed with <code>new</code>.
 */
void ScopePtr::reset(Scope * const scope) {
    if (this->scope == scope) { return; }
    if (--*this->count == 0) {
        delete this->scope;
    } else {
        try {
            this->count = new size_t;
        } catch (...) {
            ++*this->count;
            delete scope;
            throw;
        }
    }
    *this->count = 1;
    this->scope = scope;
}

void ScopePtr::dispose() {
    if (--*this->count == 0) {
        delete this->scope;
        delete this->count;
    }
}

} // namespace OpenVRML
