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

# ifndef OPENVRML_NODE_CLASS_PTR_H
#   define OPENVRML_NODE_CLASS_PTR_H

#   include <cstddef>
#   include <cassert>
#   include <memory>
#   include <openvrml/common.h>

namespace openvrml {

    class node_class_ptr;

    bool operator==(const node_class_ptr & lhs, const node_class_ptr & rhs)
        throw ();

    class node_class;

    class node_class_ptr {
        friend bool operator==(const node_class_ptr & lhs,
                               const node_class_ptr & rhs) throw ();

        node_class * class_;
        size_t * count;

    public:
        explicit node_class_ptr(node_class * class_ = 0)
            throw (std::bad_alloc);
        node_class_ptr(const node_class_ptr & ptr) throw ();
        ~node_class_ptr() throw ();

        operator bool() const throw ();

        node_class_ptr & operator=(const node_class_ptr & ptr) throw ();

        node_class & operator*() const throw ();
        node_class * operator->() const throw ();
        node_class * get() const throw ();

        void reset(node_class * class_ = 0) throw (std::bad_alloc);

    private:
        void dispose() throw ();
    };

    inline node_class_ptr::~node_class_ptr() throw ()
    {
        this->dispose();
    }

    inline node_class_ptr::operator bool() const throw ()
    {
        return this->class_;
    }

    inline node_class & node_class_ptr::operator*() const throw ()
    {
        assert(this->class_);
        return *this->class_;
    }

    inline node_class * node_class_ptr::operator->() const throw ()
    {
        assert(this->class_);
        return this->class_;
    }

    inline node_class * node_class_ptr::get() const throw ()
    {
        assert(this->class_);
        return this->class_;
    }

    inline bool operator==(const node_class_ptr & lhs,
                           const node_class_ptr & rhs)
        throw ()
    {
        return lhs.class_ == rhs.class_;
    }

    inline bool operator!=(const node_class_ptr & lhs,
                           const node_class_ptr & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }
}

# endif // OPENVRML_NODE_CLASS_PTR_H
