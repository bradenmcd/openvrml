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

# include "nodetypeptr.h"
# include "node.h"

namespace OpenVRML {

/**
 * @class NodeTypePtr
 *
 * @brief NodeTypePtr is a reference-counting smart pointer for NodeTypes.
 *
 * NodeTypePtr is patterned after the <a href="http://boost.org/libs/smart_ptr/shared_ptr.htm">Boost shared_ptr</a>,
 * and it works basically the same way. A NodeTypePtr should be constructed
 * or reset with a NodeType created with @c new (or 0).
 *
 * @see http://boost.org/libs/smart_ptr/shared_ptr.htm
 */

/**
 * @brief Constructor.
 *
 * @param nodeType a pointer to a NodeType constructed with <code>new</code>.
 */
NodeTypePtr::NodeTypePtr(NodeType * const nodeType): nodeType(nodeType) {
    try {
        this->count = new size_t(1); // prevent leak if new throws
    } catch (...) {
        delete nodeType;
        throw;
    }
}

/**
 * @brief Copy constructor.
 *
 * @param nodeTypePtr the NodeTypePtr to copy.
 */
NodeTypePtr::NodeTypePtr(const NodeTypePtr & nodeTypePtr):
        nodeType(nodeTypePtr.nodeType) {
    ++*(this->count = nodeTypePtr.count); // never throws
}

/**
 * @brief Assignment operator.
 *
 * @param nodeTypePtr
 */
NodeTypePtr & NodeTypePtr::operator=(const NodeTypePtr & nodeTypePtr) {
    if (this->count != nodeTypePtr.count) {
        ++*nodeTypePtr.count;
        this->dispose();
        this->nodeType = nodeTypePtr.nodeType;
        this->count = nodeTypePtr.count;
    }
    return *this;
}

/**
 * @brief Reset the NodeTypePtr to a new NodeType.
 *
 * @param nodeType a pointer to a NodeType constructed with <code>new</code>.
 */
void NodeTypePtr::reset(NodeType * const nodeType) {
    if (this->nodeType == nodeType) { return; }
    if (--*this->count == 0) {
        delete this->nodeType;
    } else {
        try {
            this->count = new size_t;
        } catch (...) {
            ++*this->count;
            delete nodeType;
            throw;
        }
    }
    *this->count = 1;
    this->nodeType = nodeType;
}

void NodeTypePtr::dispose() {
    if (--*this->count == 0) {
        delete this->nodeType;
        delete this->count;
    }
}

} // namespace OpenVRML
