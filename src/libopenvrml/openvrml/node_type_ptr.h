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

# ifndef OPENVRML_NODETYPEPTR_H
#   define OPENVRML_NODETYPEPTR_H

#   include <cstddef>
#   include <cassert>
#   include <memory>
#   include <openvrml/common.h>

namespace openvrml {

    class node_type_ptr;

    bool operator==(const node_type_ptr & lhs, const node_type_ptr & rhs)
        throw ();

    class node_type;

    class node_type_ptr {
        friend bool operator==(const node_type_ptr & lhs,
                               const node_type_ptr & rhs) throw ();

        node_type * type;
        size_t * count;

    public:
        explicit node_type_ptr(node_type * type = 0) throw (std::bad_alloc);
        node_type_ptr(const node_type_ptr & ptr) throw ();
        ~node_type_ptr() throw ();

        operator bool() const throw ();

        node_type_ptr & operator=(const node_type_ptr & ptr) throw ();

        node_type & operator*() const throw ();
        node_type * operator->() const throw ();
        node_type * get() const throw ();

        void reset(node_type * type = 0) throw (std::bad_alloc);

    private:
        void dispose() throw ();
    };

    inline node_type_ptr::~node_type_ptr() throw ()
    {
        this->dispose();
    }

    inline node_type_ptr::operator bool() const throw ()
    {
        return this->type;
    }

    inline node_type & node_type_ptr::operator*() const throw ()
    {
        assert(this->type);
        return *this->type;
    }

    inline node_type * node_type_ptr::operator->() const throw ()
    {
        assert(this->type);
        return this->type;
    }

    inline node_type * node_type_ptr::get() const throw ()
    {
        assert(this->type);
        return this->type;
    }

    inline bool operator==(const node_type_ptr & lhs,
                           const node_type_ptr & rhs)
        throw ()
    {
        return lhs.type == rhs.type;
    }

    inline bool operator!=(const node_type_ptr & lhs,
                           const node_type_ptr & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }
}

# endif
