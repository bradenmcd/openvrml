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

#   include <stddef.h>
#   include <assert.h>
#   include "common.h"

namespace OpenVRML {

    class node_type;

    class OPENVRML_SCOPE NodeTypePtr {
        node_type * nodeType;
        size_t * count;

    public:
        explicit NodeTypePtr(node_type * nodeType = 0);
        NodeTypePtr(const NodeTypePtr & nodeTypePtr);
        ~NodeTypePtr();

        operator bool() const;

        NodeTypePtr & operator=(const NodeTypePtr & nodeTypePtr);

        bool operator==(const NodeTypePtr & nodeTypePtr) const;

        node_type & operator*() const;
        node_type * operator->() const;
        node_type * get() const;

        void reset(node_type * nodeType = 0);

    private:
        void dispose();
    };

    inline NodeTypePtr::~NodeTypePtr() {
        this->dispose();
    }

    inline NodeTypePtr::operator bool() const {
        return this->nodeType;
    }

    inline bool NodeTypePtr::operator==(const NodeTypePtr & nodeTypePtr) const
    {
        return (this->nodeType == nodeTypePtr.nodeType);
    }

    inline node_type & NodeTypePtr::operator*() const
    {
        assert(this->nodeType);
        return *this->nodeType;
    }

    inline node_type * NodeTypePtr::operator->() const
    {
        assert(this->nodeType);
        return this->nodeType;
    }

    inline node_type * NodeTypePtr::get() const
    {
        assert(this->nodeType);
        return this->nodeType;
    }
}

# endif
