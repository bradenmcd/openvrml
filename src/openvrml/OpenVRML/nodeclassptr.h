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

# ifndef OPENVRML_NODECLASSPTR_H
#   define OPENVRML_NODECLASSPTR_H

#   include <stddef.h>
#   include <assert.h>
#   include "common.h"

namespace OpenVRML {

    class node_class;

    class OPENVRML_SCOPE NodeClassPtr {
        node_class * nodeClass;
        size_t * count;

    public:
        explicit NodeClassPtr(node_class * nodeClass = 0);
        NodeClassPtr(const NodeClassPtr & nodeClassPtr);
        ~NodeClassPtr();

        operator bool() const;

        NodeClassPtr & operator=(const NodeClassPtr & nodeClassPtr);

        bool operator==(const NodeClassPtr & nodeClassPtr) const;

        node_class & operator*() const;
        node_class * operator->() const;
        node_class * get() const;

        void reset(node_class * nodeClass = 0);

    private:
        void dispose();
    };

    inline NodeClassPtr::~NodeClassPtr() {
        this->dispose();
    }

    inline NodeClassPtr::operator bool() const {
        return this->nodeClass;
    }

    inline bool
    NodeClassPtr::operator==(const NodeClassPtr & nodeClassPtr) const
    {
        return (this->nodeClass == nodeClassPtr.nodeClass);
    }

    inline node_class & NodeClassPtr::operator*() const
    {
        assert(this->nodeClass);
        return *this->nodeClass;
    }

    inline node_class * NodeClassPtr::operator->() const
    {
        assert(this->nodeClass);
        return this->nodeClass;
    }

    inline node_class * NodeClassPtr::get() const
    {
        assert(this->nodeClass);
        return this->nodeClass;
    }
}

# endif
