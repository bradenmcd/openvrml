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

# ifndef OPENVRML_SCOPEPTR_H
#   define OPENVRML_SCOPEPTR_H

#   include <stddef.h>
#   include <assert.h>
#   include "common.h"

namespace OpenVRML {

    class Scope;

    class OPENVRML_SCOPE ScopePtr {
        Scope * scope;
        size_t * count;

    public:
        explicit ScopePtr(Scope * scope = 0);
        ScopePtr(const ScopePtr & scopePtr);
        ~ScopePtr();

        operator bool() const;

        ScopePtr & operator=(const ScopePtr & scopePtr);

        bool operator==(const ScopePtr & scopePtr) const;

        Scope & operator*() const;
        Scope * operator->() const;
        Scope * get() const;

        void reset(Scope * scope = 0);

    private:
        void dispose();
    };

    inline ScopePtr::~ScopePtr() {
        this->dispose();
    }

    inline ScopePtr::operator bool() const {
        return this->scope;
    }

    inline bool ScopePtr::operator==(const ScopePtr & scopePtr) const {
        return (this->scope == scopePtr.scope);
    }

    inline Scope & ScopePtr::operator*() const {
        assert(this->scope);
        return *this->scope;
    }

    inline Scope * ScopePtr::operator->() const {
        assert(this->scope);
        return this->scope;
    }

    inline Scope * ScopePtr::get() const {
        assert(this->scope);
        return this->scope;
    }
}

# endif
