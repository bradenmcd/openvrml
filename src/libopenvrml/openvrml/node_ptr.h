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

# ifndef OPENVRML_NODE_PTR_H
#   define OPENVRML_NODE_PTR_H

#   include <cassert>
#   include <map>
#   include <memory>
#   include <openvrml/common.h>

namespace openvrml {

    class node_ptr;

    bool operator==(const node_ptr & lhs, const node_ptr & rhs) throw ();

    class node;

    class node_ptr {
        friend class script_node;
        friend bool operator==(const node_ptr & lhs, const node_ptr & rhs)
            throw ();

        std::map<node *, size_t>::value_type * count_ptr;

    public:
        explicit node_ptr(node * node = 0) throw (std::bad_alloc);
        node_ptr(const node_ptr & ptr) throw ();
        ~node_ptr() throw ();

        operator bool() const throw ();

        node_ptr & operator=(const node_ptr & ptr) throw ();

        node & operator*() const throw ();
        node * operator->() const throw ();
        node * get() const throw ();

        void reset(node * node = 0) throw (std::bad_alloc);
        void swap(node_ptr & ptr) throw ();

    private:
        void dispose() throw ();
        void share(std::map<node *, size_t>::value_type * count_ptr) throw ();
    };


    inline node_ptr::~node_ptr() throw ()
    {
        this->dispose();
    }

    inline node_ptr::operator bool() const throw ()
    {
        return this->count_ptr;
    }

    inline node_ptr & node_ptr::operator=(const node_ptr & ptr) throw ()
    {
        this->share(ptr.count_ptr);
        return *this;
    }

    inline node & node_ptr::operator*() const throw ()
    {
        assert(this->count_ptr);
        assert(this->count_ptr->first);
        return *this->count_ptr->first;
    }

    inline node * node_ptr::operator->() const throw ()
    {
        assert(this->count_ptr);
        assert(this->count_ptr->first);
        return this->count_ptr->first;
    }

    inline node * node_ptr::get() const throw ()
    {
        return this->count_ptr ? this->count_ptr->first : 0;
    }

    inline void node_ptr::swap(node_ptr & ptr) throw ()
    {
        std::swap(this->count_ptr, ptr.count_ptr);
    }

    inline bool operator==(const node_ptr & lhs, const node_ptr & rhs) throw ()
    {
        return lhs.count_ptr == rhs.count_ptr;
    }

    inline bool operator!=(const node_ptr & lhs, const node_ptr & rhs) throw ()
    {
        return !(lhs == rhs);
    }
}

# endif // OPENVRML_NODE_PTR_H
