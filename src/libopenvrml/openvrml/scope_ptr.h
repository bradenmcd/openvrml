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

# ifndef OPENVRML_SCOPE_PTR_H
#   define OPENVRML_SCOPE_PTR_H

#   include <cstddef>
#   include <cassert>
#   include <memory>
#   include <openvrml/common.h>

namespace openvrml {

    class scope_ptr;

    bool operator==(const scope_ptr & lhs, const scope_ptr & rhs) throw ();

    class scope;

    class scope_ptr {
        friend bool operator==(const scope_ptr & lhs, const scope_ptr & rhs)
            throw ();

        scope * scope_;
        size_t * count_;

    public:
        explicit scope_ptr(openvrml::scope * scope = 0) throw (std::bad_alloc);
        scope_ptr(const scope_ptr & ptr) throw ();
        ~scope_ptr() throw ();

        operator bool() const throw ();

        scope_ptr & operator=(const scope_ptr & ptr) throw ();

        scope & operator*() const throw ();
        scope * operator->() const throw ();
        scope * get() const throw ();

        void reset(openvrml::scope * scope = 0) throw (std::bad_alloc);

    private:
        void dispose() throw ();
    };

    inline scope_ptr::~scope_ptr() throw ()
    {
        this->dispose();
    }

    inline scope_ptr::operator bool() const throw ()
    {
        return this->scope_;
    }

    inline scope & scope_ptr::operator*() const throw ()
    {
        assert(this->scope_);
        return *this->scope_;
    }

    inline scope * scope_ptr::operator->() const throw ()
    {
        assert(this->scope_);
        return this->scope_;
    }

    inline scope * scope_ptr::get() const throw ()
    {
        assert(this->scope_);
        return this->scope_;
    }

    inline bool operator==(const scope_ptr & lhs, const scope_ptr & rhs)
        throw ()
    {
        return lhs.scope_ == rhs.scope_;
    }

    inline bool operator!=(const scope_ptr & lhs, const scope_ptr & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }
}

# endif
