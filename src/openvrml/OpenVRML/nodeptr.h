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

# ifndef OPENVRML_NODEPTR_H
#   define OPENVRML_NODEPTR_H

#   include <assert.h>
#   include <map>
#   include "common.h"

namespace OpenVRML {

    class Node;

    class OPENVRML_SCOPE NodePtr {

        friend class ScriptNode;

        std::map<Node *, size_t>::value_type * countPtr; // MSVC6 doesn't like std::map<>::pointer

    public:
        explicit NodePtr(Node * node = 0);
        NodePtr(const NodePtr & nodePtr);
        ~NodePtr();

        operator bool() const;

        NodePtr & operator=(const NodePtr & nodePtr);

        bool operator==(const NodePtr & nodePtr) const;

        Node & operator*() const;
        Node * operator->() const;
        Node * get() const;

        void reset(Node * node = 0);
        void swap(NodePtr & nodePtr) throw ();

    private:
        void dispose() throw ();
        void share(std::map<Node *, size_t>::value_type * countPtr) throw ();
    };


    inline NodePtr::~NodePtr()
    {
        this->dispose();
    }

    inline NodePtr::operator bool() const
    {
        return this->countPtr;
    }

    inline NodePtr & NodePtr::operator=(const NodePtr & nodePtr)
    {
        this->share(nodePtr.countPtr);
        return *this;
    }

    inline Node & NodePtr::operator*() const
    {
        assert(this->countPtr);
        assert(this->countPtr->first);
        return *this->countPtr->first;
    }

    inline Node * NodePtr::operator->() const
    {
        assert(this->countPtr);
        assert(this->countPtr->first);
        return this->countPtr->first;
    }

    inline Node * NodePtr::get() const
    {
        return this->countPtr ? this->countPtr->first : 0;
    }

    inline void NodePtr::swap(NodePtr & nodePtr) throw ()
    {
        std::swap(this->countPtr, nodePtr.countPtr);
    }

    inline bool NodePtr::operator==(const NodePtr & nodePtr) const
    {
        return (this->countPtr == nodePtr.countPtr);
    }
}

# endif
