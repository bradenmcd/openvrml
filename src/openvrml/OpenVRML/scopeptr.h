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
#   include <OpenVRML/common.h>

namespace OpenVRML {

    class scope;

    class OPENVRML_SCOPE ScopePtr {
        scope * scope_;
        size_t * count_;

    public:
        explicit ScopePtr(OpenVRML::scope * scope = 0);
        ScopePtr(const ScopePtr & scopePtr);
        ~ScopePtr();

        operator bool() const;

        ScopePtr & operator=(const ScopePtr & scopePtr);

        bool operator==(const ScopePtr & scopePtr) const;

        scope & operator*() const;
        scope * operator->() const;
        scope * get() const;

        void reset(OpenVRML::scope * scope = 0);

    private:
        void dispose();
    };

    inline ScopePtr::~ScopePtr()
    {
        this->dispose();
    }

    inline ScopePtr::operator bool() const
    {
        return this->scope_;
    }

    inline bool ScopePtr::operator==(const ScopePtr & scopePtr) const
    {
        return (this->scope_ == scopePtr.scope_);
    }

    inline scope & ScopePtr::operator*() const
    {
        assert(this->scope_);
        return *this->scope_;
    }

    inline scope * ScopePtr::operator->() const
    {
        assert(this->scope_);
        return this->scope_;
    }

    inline scope * ScopePtr::get() const
    {
        assert(this->scope_);
        return this->scope_;
    }
}

# endif
