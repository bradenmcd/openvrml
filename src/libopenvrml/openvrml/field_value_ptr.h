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

# ifndef OPENVRML_FIELD_VALUE_PTR_H
#   define OPENVRML_FIELD_VALUE_PTR_H

#   include <cstddef>
#   include <cassert>
#   include <memory>
#   include <openvrml/common.h>

namespace openvrml {

    class field_value_ptr;

    bool operator==(const field_value_ptr & lhs,
                    const field_value_ptr & rhs) throw ();

    class field_value;

    class field_value_ptr {
        friend bool operator==(const field_value_ptr & lhs,
                               const field_value_ptr & rhs) throw ();

        field_value * value;
        size_t * count;

    public:
        explicit field_value_ptr(field_value * value = 0)
            throw (std::bad_alloc);
        explicit field_value_ptr(std::auto_ptr<field_value> & value)
            throw (std::bad_alloc);
        field_value_ptr(const field_value_ptr & ptr) throw ();
        ~field_value_ptr() throw ();

        operator bool() const throw ();

        field_value_ptr & operator=(const field_value_ptr & ptr) throw ();

        field_value & operator*() const throw ();
        field_value * operator->() const throw ();
        field_value * get() const throw ();

        void reset(field_value * value = 0) throw (std::bad_alloc);

    private:
        void dispose() throw ();
    };

    inline field_value_ptr::~field_value_ptr() throw ()
    {
        this->dispose();
    }

    inline field_value_ptr::operator bool() const throw ()
    {
        return this->value;
    }

    inline field_value & field_value_ptr::operator*() const throw ()
    {
        assert(this->value);
        return *this->value;
    }

    inline field_value * field_value_ptr::operator->() const throw ()
    {
        assert(this->value);
        return this->value;
    }

    inline field_value * field_value_ptr::get() const throw ()
    {
        assert(this->value);
        return this->value;
    }

    inline bool operator==(const field_value_ptr & lhs,
                           const field_value_ptr & rhs)
        throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const field_value_ptr & lhs,
                           const field_value_ptr & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }
}

# endif // OPENVRML_FIELD_VALUE_PTR_H
